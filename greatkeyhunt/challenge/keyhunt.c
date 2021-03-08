#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define INP_BUF_SZ 128

typedef struct {
  uint8_t encrypt_key[32];
  uint8_t mac_key[32];
  HMAC_CTX *hmac_ctx;
  EVP_CIPHER_CTX *cipher_ctx;
} key_data;

unsigned char *hash_n_times(unsigned char *inp, int len, int count);

void ssl_error_die() {
  ERR_print_errors_fp(stderr);
  exit(1);
}

void rand_bytes_or_die(unsigned char *buf, int num) {
  if(RAND_bytes(buf, num) != 1) {
    ssl_error_die();
    exit(1);
  }
}

key_data *alloc_key_data() {
  uint8_t n;
  rand_bytes_or_die(&n, 1);
  key_data *rv;
  for(int i=0; i<1024; i++) {
    void *ptr = malloc(sizeof(key_data));
    rand_bytes_or_die((unsigned char *)ptr, sizeof(key_data));
    if (i == n) {
      rv = (key_data *)ptr;
    }
  }
  rand_bytes_or_die(&n, 1);
  return rv;
}

void load_passphrase(key_data *kd) {
  unsigned char *buf = (unsigned char *)malloc(INP_BUF_SZ);
  printf("Passphrase: ");
  fflush(stdout);
  if (!fgets((char *)buf, INP_BUF_SZ, stdin)) {
    printf("Read error!\n");
    exit(1);
  }
  buf[strcspn((char *)buf, "\n")] = '\0';
  unsigned char *tmpkey = hash_n_times(buf, (int)strlen((char *)buf), 1024);
  memcpy(kd->encrypt_key, tmpkey, sizeof(kd->encrypt_key));
  unsigned char *tmphkey = hash_n_times(tmpkey, sizeof(kd->encrypt_key), 128);
  memcpy(kd->mac_key, tmphkey, sizeof(kd->mac_key));
  rand_bytes_or_die(tmphkey, EVP_MAX_MD_SIZE);
  rand_bytes_or_die(tmpkey, EVP_MAX_MD_SIZE);
  rand_bytes_or_die(buf, INP_BUF_SZ);
}

/*
 * Hash something n times
 */
unsigned char *hash_n_times(unsigned char *inp, int len, int count) {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  unsigned char *buf = (unsigned char *)malloc(EVP_MAX_MD_SIZE);
  EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
  EVP_DigestUpdate(ctx, (void *)inp, len);
  unsigned int l = EVP_MAX_MD_SIZE;
  EVP_DigestFinal_ex(ctx, buf, &l);
  for (int i=1; i<count; i++) {
    EVP_MD_CTX_reset(ctx);
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, buf, l);
    EVP_DigestFinal_ex(ctx, buf, &l);
  }
  EVP_MD_CTX_free(ctx);
  return buf;
}

void usage(char *name) {
  printf("%s <encrypt|decrypt> <infile> <outfile>\n", name);
  exit(1);
}

void encrypt_file(key_data *kd, FILE *inp, FILE *outp) {
  unsigned char iv[16];
  rand_bytes_or_die(iv, sizeof(iv));
  kd->hmac_ctx = HMAC_CTX_new();
  if (!HMAC_Init_ex(kd->hmac_ctx, kd->mac_key, sizeof(kd->mac_key), EVP_sha256(), NULL)) {
    ssl_error_die();
  }
  kd->cipher_ctx = EVP_CIPHER_CTX_new();
  if (!EVP_EncryptInit_ex(kd->cipher_ctx, EVP_aes_256_cbc(), NULL, kd->encrypt_key, iv)) {
    ssl_error_die();
  }
  // Write iv to mac stream
  if (!HMAC_Update(kd->hmac_ctx, iv, sizeof(iv))) {
    ssl_error_die();
  }
  // Write iv to output stream
  fwrite(iv, sizeof(char), sizeof(iv), outp);

  // Block read/write
  unsigned char *buf = (unsigned char *)malloc(INP_BUF_SZ);
  unsigned char *outbuf = (unsigned char *)malloc(INP_BUF_SZ*2);
  while (1) {
    int l = fread(buf, sizeof(char), INP_BUF_SZ, inp);
    if (l == 0)
      break;
    int outl = INP_BUF_SZ*2;
    if (!EVP_EncryptUpdate(kd->cipher_ctx, outbuf, &outl, buf, l)) {
      ssl_error_die();
    }
    fwrite(outbuf, sizeof(char), outl, outp);
    if (!HMAC_Update(kd->hmac_ctx, outbuf, outl)) {
      ssl_error_die();
    }
  }
  int outl = INP_BUF_SZ*2;
  if (!EVP_EncryptFinal_ex(kd->cipher_ctx, outbuf, &outl)) {
    ssl_error_die();
  }
  if (!HMAC_Update(kd->hmac_ctx, outbuf, outl)) {
    ssl_error_die();
  }
  fwrite(outbuf, sizeof(char), outl, outp);

  // Write mac out
  outl = INP_BUF_SZ*2;
  if (!HMAC_Final(kd->hmac_ctx, outbuf, (unsigned int *)&outl)) {
    ssl_error_die();
  }
  fwrite(outbuf, sizeof(char), outl, outp);

  // Cleanup
  HMAC_CTX_free(kd->hmac_ctx);
  EVP_CIPHER_CTX_free(kd->cipher_ctx);
  rand_bytes_or_die(buf, INP_BUF_SZ);
  rand_bytes_or_die(outbuf, INP_BUF_SZ*2);
  rand_bytes_or_die((unsigned char *)kd->hmac_ctx, sizeof(kd->hmac_ctx));
  rand_bytes_or_die((unsigned char *)kd->cipher_ctx, sizeof(kd->cipher_ctx));
}

off_t get_file_length(FILE *fp) {
  int fd = fileno(fp);
  struct stat st;
  fstat(fd, &st);
  return st.st_size;
}

void decrypt_file(key_data *kd, FILE *inp, FILE *outp) {
  unsigned char iv[16];
  if (fread(iv, sizeof(iv[0]), sizeof(iv), inp) != sizeof(iv)) {
    printf("Error reading iv.");
    exit(1);
  }
  kd->hmac_ctx = HMAC_CTX_new();
  if (!HMAC_Init_ex(kd->hmac_ctx, kd->mac_key, sizeof(kd->mac_key), EVP_sha256(), NULL)) {
    ssl_error_die();
  }
  kd->cipher_ctx = EVP_CIPHER_CTX_new();
  if (!EVP_DecryptInit_ex(kd->cipher_ctx, EVP_aes_256_cbc(), NULL, kd->encrypt_key, iv)) {
    ssl_error_die();
  }
  // Write iv to mac stream
  if (!HMAC_Update(kd->hmac_ctx, iv, sizeof(iv))) {
    ssl_error_die();
  }

  int len = get_file_length(inp);
  int enc_len = len - sizeof(iv);
  enc_len -= HMAC_size(kd->hmac_ctx);

  // Block read/write
  unsigned char *buf = (unsigned char *)malloc(INP_BUF_SZ);
  unsigned char *outbuf = (unsigned char *)malloc(INP_BUF_SZ*2);
  while (1) {
    int r_len = (INP_BUF_SZ > enc_len) ? enc_len : INP_BUF_SZ;
    int l = fread(buf, sizeof(char), r_len, inp);
    if (l == 0)
      break;
    int outl = INP_BUF_SZ*2;
    if (!EVP_DecryptUpdate(kd->cipher_ctx, outbuf, &outl, buf, l)) {
      ssl_error_die();
    }
    fwrite(outbuf, sizeof(char), outl, outp);
    if (!HMAC_Update(kd->hmac_ctx, buf, l)) {
      ssl_error_die();
    }
    enc_len -= l;
    if (enc_len == 0)
      break;
  }

  // Read and compare HMAC
  int outl = INP_BUF_SZ*2;
  fread(buf, sizeof(char), (int)HMAC_size(kd->hmac_ctx), inp);
  if (!HMAC_Final(kd->hmac_ctx, outbuf, (unsigned int *)&outl)) {
    ssl_error_die();
  }
  if (memcmp(outbuf, buf, outl)) {
    printf("Invalid MAC!\n");
    exit(2);
  }

  // Final decrypt
  outl = INP_BUF_SZ*2;
  if (!EVP_DecryptFinal_ex(kd->cipher_ctx, outbuf, &outl)) {
    ssl_error_die();
  }
  fwrite(outbuf, sizeof(char), outl, outp);

  // Cleanup
  EVP_CIPHER_CTX_free(kd->cipher_ctx);
  HMAC_CTX_free(kd->hmac_ctx);
}

void cleanup(key_data *kd) {
  memset(kd, sizeof(key_data), sizeof(key_data));
}

int main(int argc, char **argv) {
  if (argc != 4) {
    usage(argv[0]);
  }
  key_data *kd = alloc_key_data();
  load_passphrase(kd);

  FILE *ifp = fopen(argv[2], "r");
  if (ifp == NULL) {
    printf("Error opening input: %s\n", strerror(errno));
    exit(1);
  }
  FILE *ofp = fopen(argv[3], "w");
  if (ofp == NULL) {
    printf("Error opening output: %s\n", strerror(errno));
    exit(1);
  }

  if (!strcmp(argv[1], "encrypt")) {
    encrypt_file(kd, ifp, ofp);
  } else if (!strcmp(argv[1], "decrypt")) {
    decrypt_file(kd, ifp, ofp);
  } else {
    usage(argv[0]);
  }

  fflush(ofp);
  fclose(ofp);
  fclose(ifp);

  // Cleanup
  if (!getenv("NO_CLEANUP")) {
    kd = NULL;
    cleanup(kd);
  }
}

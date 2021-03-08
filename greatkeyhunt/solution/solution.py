import hashlib
import os
from Crypto.Cipher import AES

def test_chunk(chunk):
    if len(chunk) != 64:
        return False
    expected = chunk[32:]
    h = chunk[:32]
    for i in range(128):
        h = hashlib.sha256(h).digest()
    return h == expected

def find_key():
    data = open('../distfiles/core', 'rb').read()
    for c in range(0, len(data)-64, 4):
        chunk = data[c:c+64]
        if test_chunk(chunk):
            print('Key: ', chunk[:32].hex())
            print('Mac: ', chunk[32:].hex())
            return chunk[:32]


def main():
    key = find_key()
    if key:
        with open('../distfiles/flag.txt.enc', 'rb') as fp:
            sres = os.fstat(fp.fileno())
            iv = fp.read(16)
            cipher = AES.new(key, AES.MODE_CBC, IV=iv)
            data = fp.read(sres.st_size-48)
            ptext = cipher.decrypt(data)
            print(ptext)

if __name__ == '__main__':
    main()

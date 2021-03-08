using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace keygenme
{
    class Crypter
    {
        byte[] keyBytes;

        public Crypter(byte[] rawkey)
        {
            SHA256 hasher = SHA256.Create();
            this.keyBytes = hasher.ComputeHash(rawkey);
            //System.Diagnostics.Debug.WriteLine("Key ID: {0}", this.getKeyID());
        }

        public String getKeyID()
        {
            SHA256 hasher = SHA256.Create();
            byte[] idBytes = hasher.ComputeHash(this.keyBytes);
            var builder = new StringBuilder();
            for (int i=0;i<idBytes.Length;i++)
            {
                builder.Append(idBytes[i].ToString("x2"));
            }
            return builder.ToString();
        }

        public bool checkKeyId(String keyid)
        {
            return this.getKeyID() == keyid;
        }

        public String encryptStringHex(String plain)
        {
            byte[] plainBytes = Encoding.UTF8.GetBytes(plain);
            Aes aesCipher = this.buildCip();
            MemoryStream dest = new MemoryStream();
            CryptoStream cstream = new CryptoStream(dest, aesCipher.CreateEncryptor(), CryptoStreamMode.Write);
            cstream.Write(plainBytes, 0, plainBytes.Length);
            cstream.FlushFinalBlock();
            byte[] encBytes = dest.ToArray();
            var builder = new StringBuilder();
            for (int i = 0; i < encBytes.Length; i++)
            {
                builder.Append(encBytes[i].ToString("x2"));
            }
            return builder.ToString();
        }

        public String decryptHexString(String ctext)
        {
            byte[] cbytes = Crypter.decodeHex(ctext);
            Aes aesCipher = this.buildCip();
            MemoryStream src = new MemoryStream(cbytes);
            CryptoStream cstream = new CryptoStream(src, aesCipher.CreateDecryptor(), CryptoStreamMode.Read);
            StreamReader sreader = new StreamReader(cstream);
            return sreader.ReadToEnd();
        }

        private Aes buildCip()
        {
            Aes aesCrypter = Aes.Create();
            aesCrypter.Key = this.keyBytes;
            aesCrypter.IV = new byte[16];
            return aesCrypter;
        }

        private static byte[] decodeHex(String hex)
        {
            byte[] res = new byte[hex.Length / 2];
            for(int i = 0; i < hex.Length/2; i++)
            {
                res[i] = byte.Parse(hex.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
            }
            return res;
        }
    }
}

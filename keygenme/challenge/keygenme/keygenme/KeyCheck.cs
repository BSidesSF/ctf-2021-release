using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace keygenme
{
    class KeyCheck
    {
        private static readonly string keyIdentifier = "system";
        private static readonly int strength = 5;

        String keyStr = "";
        String[] keywords = null;
        String err = "";
        bool resChecked = false;
        bool success = false;
        byte[] keyBytes = null;

        public KeyCheck(String keyStr)
        {
            this.keyStr = keyStr.Trim();
            if (this.keyStr.Length > 0)
            {
                String[] keywords = this.keyStr.Split();
                this.keywords = new string[keywords.Length];
                for (int i = 0; i < keywords.Length; i++)
                {
                    this.keywords[i] = keywords[i].ToLowerInvariant();
                }
            }
        }

        private int[] wordsToInts(String[] words)
        {
            Wordlist wlist = Wordlist.Instance;
            int[] res = new int[words.Length];
            int[] fix = new int[] {1, 3, 11, 5, 1};
            int accum = 0x55;
            int lastWord = 0;
            for (int i=0; i<words.Length; i++)
            {
                int v = wlist.wordIndex(words[i]);
                if (v == -1)
                {
                    this.err = string.Format("{0} is not in the dictionary.", words[i]);
                    return null;
                }
                res[i] = v;
                if (i > 0 && v % i != 0 && i < (words.Length - 1))
                {
                    this.err = string.Format("{0} is not indexed correctly.", words[i]);
                    return null;
                }
                if (i > 0 && v % fix[i] != 0 && i < (words.Length - 1))
                {
                    this.err = string.Format("{0} is not indexed correctly!", words[i]);
                    return null;
                }
                lastWord = v;
                accum ^= v;
            }
            try
            {
                //System.Diagnostics.Debug.WriteLine("Word checkbits: {0}, should be {1}", accum, wlist.getWord(accum ^ lastWord));
            } catch(IndexOutOfRangeException)
            {
                //System.Diagnostics.Debug.WriteLine("Word checkbits: {0}, not found in list.", accum);
            }
            if (accum != 0)
            {
                this.err = "Check word does not match.";
                return null;
            }
            return res;
        }

        private bool dupeCheck()
        {
            for(int i = 0; i < this.keywords.Length; i++)
            {
                for(int k=0;k<this.keywords.Length; k++)
                {
                    if (i == k)
                        continue;
                    if (this.keywords[i] == this.keywords[k])
                        return false;
                }
            }
            return true;
        }

        private bool runCheck()
        {
            if (this.resChecked)
                return this.success;
            this.resChecked = true;
            if (this.keywords == null)
            {
                this.err = "No key provided.";
                return false;
            }
            if (this.keywords[0] != KeyCheck.keyIdentifier)
            {
                this.err = "Wrong key identifier.";
                return false;
            }
            if (this.keywords.Length != KeyCheck.strength)
            {
                this.err = "Incorrect key strength.";
                return false;
            }
            if (!this.dupeCheck())
            {
                this.err = "Duplicate key pieces.";
                return false;
            }
            int[] wordKeys = this.wordsToInts(this.keywords);
            if (wordKeys == null)
                return false;
            this.keyBytes = new byte[wordKeys.Length * 4];
            for(int i=0;i<wordKeys.Length;i++)
            {
                var b = BitConverter.GetBytes(wordKeys[i]);
                b.CopyTo(this.keyBytes, i * 4);
            }
            this.success = true;
            return this.success;
        }

        public String Error
        {
            get
            {
                this.runCheck();
                return this.err;
            }
        }

        public bool Success
        {
            get
            {
                return this.runCheck();
            }
        }

        public byte[] KeyBytes
        {
            get
            {
                this.runCheck();
                return this.keyBytes;
            }
        }
    }
}

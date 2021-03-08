using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Resources;
using System.Text;

namespace keygenme
{
    class Wordlist
    {
        private static Wordlist singleton = null;
        
        private String[] words;
        private Dictionary<String, int> revWords;
     
        private Wordlist() {
            var resReader = new ResourceReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("keygenme.keywords.resources"));
            string type;
            byte[] resData;
            resReader.GetResourceData("english.txt", out type, out resData);
            //System.Diagnostics.Debug.WriteLine("Type: " + type);
            BinaryReader breader = new BinaryReader(new MemoryStream(resData));
            string dictStr = breader.ReadString();
            string[] pieces = dictStr.Trim().Split('\n');
            this.revWords = new Dictionary<string, int>();
            for (int i = 0; i < pieces.Length; i++)
            {
                pieces[i] = pieces[i].Trim();
                this.revWords[pieces[i]] = i;
            }
            this.words = pieces;
            //System.Diagnostics.Debug.WriteLine("Loaded {0} words.", this.words.Length);
        }

        public static Wordlist Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new Wordlist();
                }
                return singleton;
            }
        }

        public String getWord(int n)
        {
            if (n < 0 || n >= this.words.Length)
                return "";
            return this.words[n];
        }

        public int wordIndex(String word)
        {
            try
            {
                return this.revWords[word];
            } catch(KeyNotFoundException)
            {
                return -1;
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace keygenme
{
    public partial class KeygenForm : Form
    {
        private static readonly string keyId = "ed2591b6660b2b05a0e5b22152b120cd59458eb2504c529927ff9a4890b6912e";

        public KeygenForm()
        {
            InitializeComponent();
        }

        private void KeygenForm_Load(object sender, EventArgs e)
        {
        }

        private void activate_btn_Click(object sender, EventArgs e)
        {
            String key_data = this.key_input.Text;
            KeyCheck check = new KeyCheck(key_data);
            //System.Diagnostics.Debug.WriteLine("Success state: {0}: {1}", check.Success, check.Error);
            this.resultsBox.Visible = true;
            this.flag_label.Visible = false;
            if (!check.Success)
            {
                this.results_text.Text = string.Format("Failed: {0}", check.Error);
                return;
            } else
            {
                Crypter crypter = new Crypter(check.KeyBytes);
                if (!crypter.checkKeyId(KeygenForm.keyId))
                {
                    this.results_text.Text = string.Format("Invalid KeyID for this build.");
                } else
                {
                    //System.Diagnostics.Debug.WriteLine("Encrypted: {0}", crypter.encryptStringHex("CTF{dot_dot_dot_net_dot_net}"));
                    String decString = crypter.decryptHexString(this.secret_text.Text);
                    this.flag_label.Text = decString;
                    this.flag_label.Visible = true;
                    this.results_text.Text = "Product unlocked.";
                }
            }
        }
    }
}

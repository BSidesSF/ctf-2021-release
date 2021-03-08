namespace keygenme
{
    partial class KeygenForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.key_input = new System.Windows.Forms.TextBox();
            this.activate_btn = new System.Windows.Forms.Button();
            this.akey_label = new System.Windows.Forms.Label();
            this.title = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.resultsBox = new System.Windows.Forms.GroupBox();
            this.flag_label = new System.Windows.Forms.Label();
            this.results_text = new System.Windows.Forms.Label();
            this.secret_text = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.resultsBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // key_input
            // 
            this.key_input.Location = new System.Drawing.Point(9, 51);
            this.key_input.Name = "key_input";
            this.key_input.Size = new System.Drawing.Size(351, 23);
            this.key_input.TabIndex = 0;
            // 
            // activate_btn
            // 
            this.activate_btn.Font = new System.Drawing.Font("Comic Sans MS", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.activate_btn.Location = new System.Drawing.Point(443, 51);
            this.activate_btn.Name = "activate_btn";
            this.activate_btn.Size = new System.Drawing.Size(75, 23);
            this.activate_btn.TabIndex = 1;
            this.activate_btn.Text = "Activate";
            this.activate_btn.UseVisualStyleBackColor = true;
            this.activate_btn.Click += new System.EventHandler(this.activate_btn_Click);
            // 
            // akey_label
            // 
            this.akey_label.AutoSize = true;
            this.akey_label.Font = new System.Drawing.Font("Comic Sans MS", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.akey_label.Location = new System.Drawing.Point(6, 33);
            this.akey_label.Name = "akey_label";
            this.akey_label.Size = new System.Drawing.Size(140, 15);
            this.akey_label.TabIndex = 2;
            this.akey_label.Text = "Enter activation key here:";
            // 
            // title
            // 
            this.title.AutoSize = true;
            this.title.Font = new System.Drawing.Font("Comic Sans MS", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.title.Location = new System.Drawing.Point(106, 27);
            this.title.Name = "title";
            this.title.Size = new System.Drawing.Size(152, 38);
            this.title.TabIndex = 3;
            this.title.Text = "KeygenMe!";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.akey_label);
            this.groupBox1.Controls.Add(this.key_input);
            this.groupBox1.Controls.Add(this.activate_btn);
            this.groupBox1.Font = new System.Drawing.Font("Comic Sans MS", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(113, 87);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(524, 103);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Key";
            // 
            // resultsBox
            // 
            this.resultsBox.Controls.Add(this.flag_label);
            this.resultsBox.Controls.Add(this.results_text);
            this.resultsBox.Font = new System.Drawing.Font("Comic Sans MS", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.resultsBox.Location = new System.Drawing.Point(113, 220);
            this.resultsBox.Name = "resultsBox";
            this.resultsBox.Size = new System.Drawing.Size(524, 169);
            this.resultsBox.TabIndex = 5;
            this.resultsBox.TabStop = false;
            this.resultsBox.Text = "Results";
            this.resultsBox.Visible = false;
            // 
            // flag_label
            // 
            this.flag_label.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.flag_label.Font = new System.Drawing.Font("Comic Sans MS", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.flag_label.Location = new System.Drawing.Point(9, 129);
            this.flag_label.Name = "flag_label";
            this.flag_label.Size = new System.Drawing.Size(509, 36);
            this.flag_label.TabIndex = 1;
            this.flag_label.Text = "FLAG";
            this.flag_label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.flag_label.Visible = false;
            // 
            // results_text
            // 
            this.results_text.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.results_text.AutoSize = true;
            this.results_text.Location = new System.Drawing.Point(9, 23);
            this.results_text.Name = "results_text";
            this.results_text.Size = new System.Drawing.Size(41, 15);
            this.results_text.TabIndex = 0;
            this.results_text.Text = "results";
            // 
            // secret_text
            // 
            this.secret_text.AutoSize = true;
            this.secret_text.Enabled = false;
            this.secret_text.Location = new System.Drawing.Point(113, 424);
            this.secret_text.Name = "secret_text";
            this.secret_text.Size = new System.Drawing.Size(388, 13);
            this.secret_text.TabIndex = 6;
            this.secret_text.Text = "294b0bee12606b1a8c611727318d7240cfc8c780a386c35758bcaa9e3b181e47";
            this.secret_text.Visible = false;
            // 
            // KeygenForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(755, 449);
            this.Controls.Add(this.secret_text);
            this.Controls.Add(this.resultsBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.title);
            this.Name = "KeygenForm";
            this.Text = "KeygenMe";
            this.Load += new System.EventHandler(this.KeygenForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.resultsBox.ResumeLayout(false);
            this.resultsBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox key_input;
        private System.Windows.Forms.Button activate_btn;
        private System.Windows.Forms.Label akey_label;
        private System.Windows.Forms.Label title;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox resultsBox;
        private System.Windows.Forms.Label flag_label;
        private System.Windows.Forms.Label results_text;
        private System.Windows.Forms.Label secret_text;
    }
}
namespace TrendViewer.View
{
    partial class About
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
            this.aboutMsgLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // aboutMsgLabel
            // 
            this.aboutMsgLabel.AutoSize = true;
            this.aboutMsgLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.aboutMsgLabel.Location = new System.Drawing.Point(32, 36);
            this.aboutMsgLabel.Name = "aboutMsgLabel";
            this.aboutMsgLabel.Size = new System.Drawing.Size(405, 37);
            this.aboutMsgLabel.TabIndex = 0;
            this.aboutMsgLabel.Text = "TrendViewer V 01.27.13.50";
            // 
            // About
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(475, 138);
            this.Controls.Add(this.aboutMsgLabel);
            this.MaximizeBox = false;
            this.Name = "About";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "About Trend Viewer";
            this.Load += new System.EventHandler(this.About_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label aboutMsgLabel;
    }
}
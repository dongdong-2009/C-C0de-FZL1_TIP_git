namespace TrendViewer.View
{
    partial class EditChartTitle
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
            this.okCmd = new System.Windows.Forms.Button();
            this.cancelCmd = new System.Windows.Forms.Button();
            this.TrendTitleLbl = new System.Windows.Forms.Label();
            this.XTitleLbl = new System.Windows.Forms.Label();
            this.YTitleLbl = new System.Windows.Forms.Label();
            this.chartTitleBox = new System.Windows.Forms.TextBox();
            this.xTitleBox = new System.Windows.Forms.TextBox();
            this.yTitleBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // okCmd
            // 
            this.okCmd.BackColor = System.Drawing.Color.Transparent;
            this.okCmd.Image = global::TrendViewer.Properties.Resources.Yes_v3;
            this.okCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.okCmd.Location = new System.Drawing.Point(139, 116);
            this.okCmd.Margin = new System.Windows.Forms.Padding(4);
            this.okCmd.Name = "okCmd";
            this.okCmd.Size = new System.Drawing.Size(104, 31);
            this.okCmd.TabIndex = 20;
            this.okCmd.Text = "OK";
            this.okCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.okCmd.UseVisualStyleBackColor = false;
            // 
            // cancelCmd
            // 
            this.cancelCmd.BackColor = System.Drawing.Color.Transparent;
            this.cancelCmd.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelCmd.Image = global::TrendViewer.Properties.Resources.Cancel;
            this.cancelCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelCmd.Location = new System.Drawing.Point(251, 116);
            this.cancelCmd.Margin = new System.Windows.Forms.Padding(4);
            this.cancelCmd.Name = "cancelCmd";
            this.cancelCmd.Size = new System.Drawing.Size(104, 31);
            this.cancelCmd.TabIndex = 21;
            this.cancelCmd.Text = "Cancel";
            this.cancelCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.cancelCmd.UseVisualStyleBackColor = false;
            // 
            // TrendTitleLbl
            // 
            this.TrendTitleLbl.BackColor = System.Drawing.Color.Transparent;
            this.TrendTitleLbl.Location = new System.Drawing.Point(5, 23);
            this.TrendTitleLbl.Name = "TrendTitleLbl";
            this.TrendTitleLbl.Size = new System.Drawing.Size(111, 16);
            this.TrendTitleLbl.TabIndex = 19;
            this.TrendTitleLbl.Text = "Trend Title:";
            this.TrendTitleLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // XTitleLbl
            // 
            this.XTitleLbl.BackColor = System.Drawing.Color.Transparent;
            this.XTitleLbl.Location = new System.Drawing.Point(24, 51);
            this.XTitleLbl.Name = "XTitleLbl";
            this.XTitleLbl.Size = new System.Drawing.Size(92, 16);
            this.XTitleLbl.TabIndex = 23;
            this.XTitleLbl.Text = "X-axis Label:";
            this.XTitleLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // YTitleLbl
            // 
            this.YTitleLbl.BackColor = System.Drawing.Color.Transparent;
            this.YTitleLbl.Location = new System.Drawing.Point(23, 79);
            this.YTitleLbl.Name = "YTitleLbl";
            this.YTitleLbl.Size = new System.Drawing.Size(93, 16);
            this.YTitleLbl.TabIndex = 24;
            this.YTitleLbl.Text = "Y-axis Label:";
            this.YTitleLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // chartTitleBox
            // 
            this.chartTitleBox.Location = new System.Drawing.Point(131, 20);
            this.chartTitleBox.MaxLength = 160;
            this.chartTitleBox.Name = "chartTitleBox";
            this.chartTitleBox.Size = new System.Drawing.Size(345, 22);
            this.chartTitleBox.TabIndex = 25;
            // 
            // xTitleBox
            // 
            this.xTitleBox.Location = new System.Drawing.Point(131, 48);
            this.xTitleBox.MaxLength = 140;
            this.xTitleBox.Name = "xTitleBox";
            this.xTitleBox.Size = new System.Drawing.Size(345, 22);
            this.xTitleBox.TabIndex = 26;
            // 
            // yTitleBox
            // 
            this.yTitleBox.Location = new System.Drawing.Point(131, 76);
            this.yTitleBox.MaxLength = 50;
            this.yTitleBox.Name = "yTitleBox";
            this.yTitleBox.Size = new System.Drawing.Size(345, 22);
            this.yTitleBox.TabIndex = 27;
            // 
            // EditChartTitle
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(495, 167);
            this.Controls.Add(this.yTitleBox);
            this.Controls.Add(this.xTitleBox);
            this.Controls.Add(this.chartTitleBox);
            this.Controls.Add(this.YTitleLbl);
            this.Controls.Add(this.XTitleLbl);
            this.Controls.Add(this.okCmd);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.TrendTitleLbl);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "EditChartTitle";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Change Trend Title";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.EditChartData_Paint);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.Button cancelCmd;
        private System.Windows.Forms.Label TrendTitleLbl;
        private System.Windows.Forms.Label XTitleLbl;
        private System.Windows.Forms.Label YTitleLbl;
        internal System.Windows.Forms.TextBox chartTitleBox;
        internal System.Windows.Forms.TextBox xTitleBox;
        internal System.Windows.Forms.TextBox yTitleBox;
    }
}
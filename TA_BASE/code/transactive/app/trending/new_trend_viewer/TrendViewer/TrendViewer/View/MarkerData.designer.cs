namespace TrendViewer.View
{
    partial class MarkerData
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
            this.cancelCmd = new System.Windows.Forms.Button();
            this.okCmd = new System.Windows.Forms.Button();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.markerEnabledCb = new System.Windows.Forms.CheckBox();
            this.fontColorCmd = new System.Windows.Forms.Button();
            this.fontColorPanel = new System.Windows.Forms.Panel();
            this.rangeLbl = new System.Windows.Forms.Label();
            this.valueBox = new System.Windows.Forms.NumericUpDown();
            this.backColorCmd = new System.Windows.Forms.Button();
            this.backColorPanel = new System.Windows.Forms.Panel();
            this.lineWidthBox = new System.Windows.Forms.NumericUpDown();
            this.startValueLbl = new System.Windows.Forms.Label();
            this.markerNameBox = new System.Windows.Forms.TextBox();
            this.markerNameLbl = new System.Windows.Forms.Label();
            this.FColorLbl = new System.Windows.Forms.Label();
            this.BColorLbl = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.valueBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.lineWidthBox)).BeginInit();
            this.SuspendLayout();
            // 
            // cancelCmd
            // 
            this.cancelCmd.BackColor = System.Drawing.Color.Transparent;
            this.cancelCmd.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelCmd.Image = global::TrendViewer.Properties.Resources.Cancel;
            this.cancelCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelCmd.Location = new System.Drawing.Point(415, 150);
            this.cancelCmd.Margin = new System.Windows.Forms.Padding(4);
            this.cancelCmd.Name = "cancelCmd";
            this.cancelCmd.Size = new System.Drawing.Size(114, 33);
            this.cancelCmd.TabIndex = 36;
            this.cancelCmd.Text = "Cancel";
            this.cancelCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.cancelCmd.UseVisualStyleBackColor = false;
            // 
            // okCmd
            // 
            this.okCmd.BackColor = System.Drawing.Color.Transparent;
            this.okCmd.Image = global::TrendViewer.Properties.Resources.Yes_v3;
            this.okCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.okCmd.Location = new System.Drawing.Point(294, 150);
            this.okCmd.Name = "okCmd";
            this.okCmd.Size = new System.Drawing.Size(114, 33);
            this.okCmd.TabIndex = 35;
            this.okCmd.Text = "OK";
            this.okCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.okCmd.UseVisualStyleBackColor = false;
            // 
            // markerEnabledCb
            // 
            this.markerEnabledCb.AutoSize = true;
            this.markerEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.markerEnabledCb.Checked = true;
            this.markerEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.markerEnabledCb.Location = new System.Drawing.Point(196, 114);
            this.markerEnabledCb.Name = "markerEnabledCb";
            this.markerEnabledCb.Size = new System.Drawing.Size(115, 20);
            this.markerEnabledCb.TabIndex = 43;
            this.markerEnabledCb.Text = "Enable Marker";
            this.markerEnabledCb.UseVisualStyleBackColor = false;
            // 
            // fontColorCmd
            // 
            this.fontColorCmd.BackColor = System.Drawing.Color.Transparent;
            this.fontColorCmd.Image = global::TrendViewer.Properties.Resources.Color_profile;
            this.fontColorCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.fontColorCmd.Location = new System.Drawing.Point(641, 48);
            this.fontColorCmd.Margin = new System.Windows.Forms.Padding(4);
            this.fontColorCmd.Name = "fontColorCmd";
            this.fontColorCmd.Size = new System.Drawing.Size(117, 24);
            this.fontColorCmd.TabIndex = 55;
            this.fontColorCmd.Text = "Select Colour";
            this.fontColorCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.fontColorCmd.UseVisualStyleBackColor = false;
            this.fontColorCmd.Click += new System.EventHandler(this.fontColorCmd_Click);
            // 
            // fontColorPanel
            // 
            this.fontColorPanel.BackColor = System.Drawing.Color.Transparent;
            this.fontColorPanel.Location = new System.Drawing.Point(537, 49);
            this.fontColorPanel.Name = "fontColorPanel";
            this.fontColorPanel.Size = new System.Drawing.Size(99, 21);
            this.fontColorPanel.TabIndex = 54;
            // 
            // rangeLbl
            // 
            this.rangeLbl.BackColor = System.Drawing.Color.Transparent;
            this.rangeLbl.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.rangeLbl.Location = new System.Drawing.Point(405, 86);
            this.rangeLbl.Name = "rangeLbl";
            this.rangeLbl.Size = new System.Drawing.Size(129, 16);
            this.rangeLbl.TabIndex = 52;
            this.rangeLbl.Text = "Range :";
            this.rangeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // valueBox
            // 
            this.valueBox.DecimalPlaces = 2;
            this.valueBox.Location = new System.Drawing.Point(196, 86);
            this.valueBox.Maximum = new decimal(new int[] {
            10000000,
            0,
            0,
            0});
            this.valueBox.Name = "valueBox";
            this.valueBox.Size = new System.Drawing.Size(101, 22);
            this.valueBox.TabIndex = 51;
            this.valueBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // backColorCmd
            // 
            this.backColorCmd.BackColor = System.Drawing.Color.Transparent;
            this.backColorCmd.Image = global::TrendViewer.Properties.Resources.Color_profile;
            this.backColorCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.backColorCmd.Location = new System.Drawing.Point(300, 48);
            this.backColorCmd.Margin = new System.Windows.Forms.Padding(4);
            this.backColorCmd.Name = "backColorCmd";
            this.backColorCmd.Size = new System.Drawing.Size(117, 24);
            this.backColorCmd.TabIndex = 50;
            this.backColorCmd.Text = "Select Colour";
            this.backColorCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.backColorCmd.UseVisualStyleBackColor = false;
            this.backColorCmd.Click += new System.EventHandler(this.backColorCmd_Click);
            // 
            // backColorPanel
            // 
            this.backColorPanel.BackColor = System.Drawing.Color.Transparent;
            this.backColorPanel.Location = new System.Drawing.Point(196, 51);
            this.backColorPanel.Name = "backColorPanel";
            this.backColorPanel.Size = new System.Drawing.Size(99, 21);
            this.backColorPanel.TabIndex = 49;
            // 
            // lineWidthBox
            // 
            this.lineWidthBox.DecimalPlaces = 2;
            this.lineWidthBox.Location = new System.Drawing.Point(540, 86);
            this.lineWidthBox.Name = "lineWidthBox";
            this.lineWidthBox.Size = new System.Drawing.Size(101, 22);
            this.lineWidthBox.TabIndex = 48;
            this.lineWidthBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // startValueLbl
            // 
            this.startValueLbl.BackColor = System.Drawing.Color.Transparent;
            this.startValueLbl.Location = new System.Drawing.Point(83, 86);
            this.startValueLbl.Name = "startValueLbl";
            this.startValueLbl.Size = new System.Drawing.Size(107, 16);
            this.startValueLbl.TabIndex = 47;
            this.startValueLbl.Text = "Start Value :";
            this.startValueLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // markerNameBox
            // 
            this.markerNameBox.Location = new System.Drawing.Point(196, 21);
            this.markerNameBox.Name = "markerNameBox";
            this.markerNameBox.Size = new System.Drawing.Size(562, 22);
            this.markerNameBox.TabIndex = 45;
            // 
            // markerNameLbl
            // 
            this.markerNameLbl.BackColor = System.Drawing.Color.Transparent;
            this.markerNameLbl.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.markerNameLbl.Location = new System.Drawing.Point(43, 24);
            this.markerNameLbl.Name = "markerNameLbl";
            this.markerNameLbl.Size = new System.Drawing.Size(147, 16);
            this.markerNameLbl.TabIndex = 44;
            this.markerNameLbl.Text = "Marker Name :";
            this.markerNameLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // FColorLbl
            // 
            this.FColorLbl.BackColor = System.Drawing.Color.Transparent;
            this.FColorLbl.Location = new System.Drawing.Point(424, 52);
            this.FColorLbl.Margin = new System.Windows.Forms.Padding(3, 0, 1, 0);
            this.FColorLbl.Name = "FColorLbl";
            this.FColorLbl.Size = new System.Drawing.Size(110, 16);
            this.FColorLbl.TabIndex = 57;
            this.FColorLbl.Text = "Font Colour :";
            this.FColorLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // BColorLbl
            // 
            this.BColorLbl.BackColor = System.Drawing.Color.Transparent;
            this.BColorLbl.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.BColorLbl.Location = new System.Drawing.Point(31, 52);
            this.BColorLbl.Name = "BColorLbl";
            this.BColorLbl.Size = new System.Drawing.Size(159, 16);
            this.BColorLbl.TabIndex = 56;
            this.BColorLbl.Text = "Background Colour :";
            this.BColorLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // MarkerData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(822, 198);
            this.Controls.Add(this.FColorLbl);
            this.Controls.Add(this.BColorLbl);
            this.Controls.Add(this.fontColorCmd);
            this.Controls.Add(this.fontColorPanel);
            this.Controls.Add(this.rangeLbl);
            this.Controls.Add(this.valueBox);
            this.Controls.Add(this.backColorCmd);
            this.Controls.Add(this.backColorPanel);
            this.Controls.Add(this.lineWidthBox);
            this.Controls.Add(this.startValueLbl);
            this.Controls.Add(this.markerNameBox);
            this.Controls.Add(this.markerNameLbl);
            this.Controls.Add(this.markerEnabledCb);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.okCmd);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "MarkerData";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Edit Data Marker";
            this.Load += new System.EventHandler(this.MarkerData_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.MarkerData_Paint);
            ((System.ComponentModel.ISupportInitialize)(this.valueBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.lineWidthBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cancelCmd;
        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.CheckBox markerEnabledCb;
        private System.Windows.Forms.Button fontColorCmd;
        internal System.Windows.Forms.Panel fontColorPanel;
        private System.Windows.Forms.Label rangeLbl;
        internal System.Windows.Forms.NumericUpDown valueBox;
        private System.Windows.Forms.Button backColorCmd;
        internal System.Windows.Forms.Panel backColorPanel;
        internal System.Windows.Forms.NumericUpDown lineWidthBox;
        private System.Windows.Forms.Label startValueLbl;
        internal System.Windows.Forms.TextBox markerNameBox;
        private System.Windows.Forms.Label markerNameLbl;
        private System.Windows.Forms.Label FColorLbl;
        private System.Windows.Forms.Label BColorLbl;
    }
}
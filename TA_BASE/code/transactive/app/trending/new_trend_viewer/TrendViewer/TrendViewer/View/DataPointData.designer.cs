namespace TrendViewer.View
{
    partial class DataPointData
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
            this.dataPointBox = new System.Windows.Forms.TextBox();
            this.seriesTypeBox = new System.Windows.Forms.ComboBox();
            this.cancelCmd = new System.Windows.Forms.Button();
            this.okCmd = new System.Windows.Forms.Button();
            this.colorPanel = new System.Windows.Forms.Panel();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.selectColorCmd = new System.Windows.Forms.Button();
            this.browseCmd = new System.Windows.Forms.Button();
            this.dataLegendBox = new System.Windows.Forms.TextBox();
            this.opcServerHostBox = new System.Windows.Forms.TextBox();
            this.opcServerNameBox = new System.Windows.Forms.TextBox();
            this.serverLbl = new System.Windows.Forms.Label();
            this.hostNameLbl = new System.Windows.Forms.Label();
            this.dataPointVisibleCb = new System.Windows.Forms.CheckBox();
            this.labelEnabledCb = new System.Windows.Forms.CheckBox();
            this.dataPointEnabledCb = new System.Windows.Forms.CheckBox();
            this.legendLbl = new System.Windows.Forms.Label();
            this.lintTypeLbl = new System.Windows.Forms.Label();
            this.colorLbl = new System.Windows.Forms.Label();
            this.dataPointLbl = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // dataPointBox
            // 
            this.dataPointBox.Location = new System.Drawing.Point(108, 76);
            this.dataPointBox.Name = "dataPointBox";
            this.dataPointBox.Size = new System.Drawing.Size(442, 22);
            this.dataPointBox.TabIndex = 1;
            // 
            // seriesTypeBox
            // 
            this.seriesTypeBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.seriesTypeBox.FormattingEnabled = true;
            this.seriesTypeBox.Location = new System.Drawing.Point(108, 132);
            this.seriesTypeBox.Name = "seriesTypeBox";
            this.seriesTypeBox.Size = new System.Drawing.Size(238, 24);
            this.seriesTypeBox.TabIndex = 6;
            // 
            // cancelCmd
            // 
            this.cancelCmd.BackColor = System.Drawing.Color.Transparent;
            this.cancelCmd.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelCmd.Image = global::TrendViewer.Properties.Resources.Cancel;
            this.cancelCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelCmd.Location = new System.Drawing.Point(357, 227);
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
            this.okCmd.Location = new System.Drawing.Point(236, 227);
            this.okCmd.Name = "okCmd";
            this.okCmd.Size = new System.Drawing.Size(114, 33);
            this.okCmd.TabIndex = 35;
            this.okCmd.Text = "OK";
            this.okCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.okCmd.UseVisualStyleBackColor = false;
            // 
            // colorPanel
            // 
            this.colorPanel.BackColor = System.Drawing.Color.Transparent;
            this.colorPanel.Location = new System.Drawing.Point(451, 133);
            this.colorPanel.Name = "colorPanel";
            this.colorPanel.Size = new System.Drawing.Size(99, 21);
            this.colorPanel.TabIndex = 37;
            // 
            // selectColorCmd
            // 
            this.selectColorCmd.BackColor = System.Drawing.Color.Transparent;
            this.selectColorCmd.Image = global::TrendViewer.Properties.Resources.Color_profile;
            this.selectColorCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.selectColorCmd.Location = new System.Drawing.Point(557, 132);
            this.selectColorCmd.Margin = new System.Windows.Forms.Padding(4);
            this.selectColorCmd.Name = "selectColorCmd";
            this.selectColorCmd.Size = new System.Drawing.Size(120, 25);
            this.selectColorCmd.TabIndex = 38;
            this.selectColorCmd.Text = "Select Colour";
            this.selectColorCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.selectColorCmd.UseVisualStyleBackColor = false;
            this.selectColorCmd.Click += new System.EventHandler(this.colorCmd_Click);
            // 
            // browseCmd
            // 
            this.browseCmd.BackColor = System.Drawing.Color.Transparent;
            this.browseCmd.Image = global::TrendViewer.Properties.Resources.Folder;
            this.browseCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.browseCmd.Location = new System.Drawing.Point(557, 75);
            this.browseCmd.Margin = new System.Windows.Forms.Padding(4);
            this.browseCmd.Name = "browseCmd";
            this.browseCmd.Size = new System.Drawing.Size(86, 25);
            this.browseCmd.TabIndex = 39;
            this.browseCmd.Text = "Browse";
            this.browseCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.browseCmd.UseVisualStyleBackColor = false;
            // 
            // dataLegendBox
            // 
            this.dataLegendBox.Location = new System.Drawing.Point(108, 104);
            this.dataLegendBox.MaxLength = 50;
            this.dataLegendBox.Name = "dataLegendBox";
            this.dataLegendBox.Size = new System.Drawing.Size(442, 22);
            this.dataLegendBox.TabIndex = 45;
            // 
            // opcServerHostBox
            // 
            this.opcServerHostBox.Location = new System.Drawing.Point(108, 20);
            this.opcServerHostBox.Name = "opcServerHostBox";
            this.opcServerHostBox.ReadOnly = true;
            this.opcServerHostBox.Size = new System.Drawing.Size(442, 22);
            this.opcServerHostBox.TabIndex = 53;
            // 
            // opcServerNameBox
            // 
            this.opcServerNameBox.Location = new System.Drawing.Point(108, 48);
            this.opcServerNameBox.Name = "opcServerNameBox";
            this.opcServerNameBox.ReadOnly = true;
            this.opcServerNameBox.Size = new System.Drawing.Size(442, 22);
            this.opcServerNameBox.TabIndex = 52;
            // 
            // serverLbl
            // 
            this.serverLbl.BackColor = System.Drawing.Color.Transparent;
            this.serverLbl.Location = new System.Drawing.Point(3, 23);
            this.serverLbl.Name = "serverLbl";
            this.serverLbl.Size = new System.Drawing.Size(92, 16);
            this.serverLbl.TabIndex = 51;
            this.serverLbl.Text = "Server :";
            this.serverLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // hostNameLbl
            // 
            this.hostNameLbl.BackColor = System.Drawing.Color.Transparent;
            this.hostNameLbl.Location = new System.Drawing.Point(-4, 51);
            this.hostNameLbl.Name = "hostNameLbl";
            this.hostNameLbl.Size = new System.Drawing.Size(99, 16);
            this.hostNameLbl.TabIndex = 50;
            this.hostNameLbl.Text = "Hostname :";
            this.hostNameLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // dataPointVisibleCb
            // 
            this.dataPointVisibleCb.AutoSize = true;
            this.dataPointVisibleCb.BackColor = System.Drawing.Color.Transparent;
            this.dataPointVisibleCb.Checked = true;
            this.dataPointVisibleCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.dataPointVisibleCb.Location = new System.Drawing.Point(108, 195);
            this.dataPointVisibleCb.Name = "dataPointVisibleCb";
            this.dataPointVisibleCb.Size = new System.Drawing.Size(73, 20);
            this.dataPointVisibleCb.TabIndex = 60;
            this.dataPointVisibleCb.Text = "Display";
            this.dataPointVisibleCb.UseVisualStyleBackColor = false;
            // 
            // labelEnabledCb
            // 
            this.labelEnabledCb.AutoSize = true;
            this.labelEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.labelEnabledCb.Checked = true;
            this.labelEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.labelEnabledCb.Location = new System.Drawing.Point(451, 169);
            this.labelEnabledCb.Name = "labelEnabledCb";
            this.labelEnabledCb.Size = new System.Drawing.Size(98, 20);
            this.labelEnabledCb.TabIndex = 59;
            this.labelEnabledCb.Text = "Enable Tag";
            this.labelEnabledCb.UseVisualStyleBackColor = false;
            // 
            // dataPointEnabledCb
            // 
            this.dataPointEnabledCb.AutoSize = true;
            this.dataPointEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.dataPointEnabledCb.Checked = true;
            this.dataPointEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.dataPointEnabledCb.Location = new System.Drawing.Point(108, 169);
            this.dataPointEnabledCb.Name = "dataPointEnabledCb";
            this.dataPointEnabledCb.Size = new System.Drawing.Size(129, 20);
            this.dataPointEnabledCb.TabIndex = 58;
            this.dataPointEnabledCb.Text = "Enable datapoint";
            this.dataPointEnabledCb.UseVisualStyleBackColor = false;
            // 
            // legendLbl
            // 
            this.legendLbl.BackColor = System.Drawing.Color.Transparent;
            this.legendLbl.Location = new System.Drawing.Point(12, 107);
            this.legendLbl.Name = "legendLbl";
            this.legendLbl.Size = new System.Drawing.Size(83, 16);
            this.legendLbl.TabIndex = 57;
            this.legendLbl.Text = "Legend :";
            this.legendLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lintTypeLbl
            // 
            this.lintTypeLbl.BackColor = System.Drawing.Color.Transparent;
            this.lintTypeLbl.Location = new System.Drawing.Point(-2, 136);
            this.lintTypeLbl.Name = "lintTypeLbl";
            this.lintTypeLbl.Size = new System.Drawing.Size(97, 16);
            this.lintTypeLbl.TabIndex = 56;
            this.lintTypeLbl.Text = "Line Type :";
            this.lintTypeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // colorLbl
            // 
            this.colorLbl.AutoSize = true;
            this.colorLbl.BackColor = System.Drawing.Color.Transparent;
            this.colorLbl.Location = new System.Drawing.Point(397, 136);
            this.colorLbl.Name = "colorLbl";
            this.colorLbl.Size = new System.Drawing.Size(53, 16);
            this.colorLbl.TabIndex = 55;
            this.colorLbl.Text = "Colour :";
            // 
            // dataPointLbl
            // 
            this.dataPointLbl.BackColor = System.Drawing.Color.Transparent;
            this.dataPointLbl.Location = new System.Drawing.Point(0, 79);
            this.dataPointLbl.Name = "dataPointLbl";
            this.dataPointLbl.Size = new System.Drawing.Size(95, 16);
            this.dataPointLbl.TabIndex = 54;
            this.dataPointLbl.Text = "Datapoint :";
            this.dataPointLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // DataPointData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(707, 274);
            this.Controls.Add(this.dataPointVisibleCb);
            this.Controls.Add(this.labelEnabledCb);
            this.Controls.Add(this.dataPointEnabledCb);
            this.Controls.Add(this.legendLbl);
            this.Controls.Add(this.lintTypeLbl);
            this.Controls.Add(this.colorLbl);
            this.Controls.Add(this.dataPointLbl);
            this.Controls.Add(this.opcServerHostBox);
            this.Controls.Add(this.opcServerNameBox);
            this.Controls.Add(this.serverLbl);
            this.Controls.Add(this.hostNameLbl);
            this.Controls.Add(this.dataLegendBox);
            this.Controls.Add(this.browseCmd);
            this.Controls.Add(this.selectColorCmd);
            this.Controls.Add(this.colorPanel);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.okCmd);
            this.Controls.Add(this.seriesTypeBox);
            this.Controls.Add(this.dataPointBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "DataPointData";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Edit Datapoint";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.DataPoint_Paint);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        internal System.Windows.Forms.TextBox dataPointBox;
        private System.Windows.Forms.Button cancelCmd;
        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Button selectColorCmd;
        internal System.Windows.Forms.ComboBox seriesTypeBox;
        internal System.Windows.Forms.Panel colorPanel;
        private System.Windows.Forms.Button browseCmd;
        internal System.Windows.Forms.TextBox dataLegendBox;
        internal System.Windows.Forms.TextBox opcServerHostBox;
        internal System.Windows.Forms.TextBox opcServerNameBox;
        private System.Windows.Forms.Label serverLbl;
        private System.Windows.Forms.Label hostNameLbl;
        internal System.Windows.Forms.CheckBox dataPointVisibleCb;
        internal System.Windows.Forms.CheckBox labelEnabledCb;
        internal System.Windows.Forms.CheckBox dataPointEnabledCb;
        private System.Windows.Forms.Label legendLbl;
        private System.Windows.Forms.Label lintTypeLbl;
        private System.Windows.Forms.Label colorLbl;
        private System.Windows.Forms.Label dataPointLbl;
    }
}

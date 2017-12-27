namespace TrendViewer.View
{
    partial class HistDataPointData
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
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.browseCmd = new System.Windows.Forms.Button();
            this.colorCmd = new System.Windows.Forms.Button();
            this.endTimeLbl = new System.Windows.Forms.Label();
            this.historyEndDateBox = new System.Windows.Forms.DateTimePicker();
            this.opcServerHostBox = new System.Windows.Forms.TextBox();
            this.opcServerNameBox = new System.Windows.Forms.TextBox();
            this.hostNameLbl = new System.Windows.Forms.Label();
            this.ServerLbl = new System.Windows.Forms.Label();
            this.startTimeLbl = new System.Windows.Forms.Label();
            this.historyStartDateBox = new System.Windows.Forms.DateTimePicker();
            this.dataLegendBox = new System.Windows.Forms.TextBox();
            this.legendLbl = new System.Windows.Forms.Label();
            this.labelEnabledCb = new System.Windows.Forms.CheckBox();
            this.dataPointEnabledCb = new System.Windows.Forms.CheckBox();
            this.colorPanel = new System.Windows.Forms.Panel();
            this.cancelCmd = new System.Windows.Forms.Button();
            this.okCmd = new System.Windows.Forms.Button();
            this.lineTypeLbl = new System.Windows.Forms.Label();
            this.colorLbl = new System.Windows.Forms.Label();
            this.colorDialog2 = new System.Windows.Forms.ColorDialog();
            this.seriesTypeBox = new System.Windows.Forms.ComboBox();
            this.dataPointBox = new System.Windows.Forms.TextBox();
            this.dataPointLbl = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // browseCmd
            // 
            this.browseCmd.BackColor = System.Drawing.Color.Transparent;
            this.browseCmd.Image = global::TrendViewer.Properties.Resources.Folder;
            this.browseCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.browseCmd.Location = new System.Drawing.Point(552, 74);
            this.browseCmd.Margin = new System.Windows.Forms.Padding(4);
            this.browseCmd.Name = "browseCmd";
            this.browseCmd.Size = new System.Drawing.Size(86, 25);
            this.browseCmd.TabIndex = 78;
            this.browseCmd.Text = "Browse";
            this.browseCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.browseCmd.UseVisualStyleBackColor = false;
            // 
            // colorCmd
            // 
            this.colorCmd.BackColor = System.Drawing.Color.Transparent;
            this.colorCmd.Image = global::TrendViewer.Properties.Resources.Color_profile;
            this.colorCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.colorCmd.Location = new System.Drawing.Point(552, 131);
            this.colorCmd.Margin = new System.Windows.Forms.Padding(4);
            this.colorCmd.Name = "colorCmd";
            this.colorCmd.Size = new System.Drawing.Size(120, 25);
            this.colorCmd.TabIndex = 77;
            this.colorCmd.Text = "Select Colour";
            this.colorCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.colorCmd.UseVisualStyleBackColor = false;
            this.colorCmd.Click += new System.EventHandler(this.colorCmd_Click);
            // 
            // endTimeLbl
            // 
            this.endTimeLbl.BackColor = System.Drawing.Color.Transparent;
            this.endTimeLbl.Location = new System.Drawing.Point(339, 192);
            this.endTimeLbl.Name = "endTimeLbl";
            this.endTimeLbl.Size = new System.Drawing.Size(83, 16);
            this.endTimeLbl.TabIndex = 76;
            this.endTimeLbl.Text = "End Date :";
            this.endTimeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // historyEndDateBox
            // 
            this.historyEndDateBox.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.historyEndDateBox.Location = new System.Drawing.Point(446, 189);
            this.historyEndDateBox.Name = "historyEndDateBox";
            this.historyEndDateBox.Size = new System.Drawing.Size(99, 22);
            this.historyEndDateBox.TabIndex = 75;
            // 
            // opcServerHostBox
            // 
            this.opcServerHostBox.Location = new System.Drawing.Point(103, 18);
            this.opcServerHostBox.Name = "opcServerHostBox";
            this.opcServerHostBox.ReadOnly = true;
            this.opcServerHostBox.Size = new System.Drawing.Size(442, 22);
            this.opcServerHostBox.TabIndex = 74;
            // 
            // opcServerNameBox
            // 
            this.opcServerNameBox.Location = new System.Drawing.Point(103, 46);
            this.opcServerNameBox.Name = "opcServerNameBox";
            this.opcServerNameBox.ReadOnly = true;
            this.opcServerNameBox.Size = new System.Drawing.Size(442, 22);
            this.opcServerNameBox.TabIndex = 73;
            // 
            // hostNameLbl
            // 
            this.hostNameLbl.BackColor = System.Drawing.Color.Transparent;
            this.hostNameLbl.Location = new System.Drawing.Point(0, 52);
            this.hostNameLbl.Name = "hostNameLbl";
            this.hostNameLbl.Size = new System.Drawing.Size(97, 16);
            this.hostNameLbl.TabIndex = 72;
            this.hostNameLbl.Text = "Hostname :";
            this.hostNameLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // ServerLbl
            // 
            this.ServerLbl.BackColor = System.Drawing.Color.Transparent;
            this.ServerLbl.Location = new System.Drawing.Point(7, 24);
            this.ServerLbl.Name = "ServerLbl";
            this.ServerLbl.Size = new System.Drawing.Size(90, 16);
            this.ServerLbl.TabIndex = 71;
            this.ServerLbl.Text = "Server :";
            this.ServerLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // startTimeLbl
            // 
            this.startTimeLbl.BackColor = System.Drawing.Color.Transparent;
            this.startTimeLbl.Location = new System.Drawing.Point(336, 164);
            this.startTimeLbl.Name = "startTimeLbl";
            this.startTimeLbl.Size = new System.Drawing.Size(86, 16);
            this.startTimeLbl.TabIndex = 70;
            this.startTimeLbl.Text = "Start Date :";
            this.startTimeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // historyStartDateBox
            // 
            this.historyStartDateBox.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.historyStartDateBox.Location = new System.Drawing.Point(446, 161);
            this.historyStartDateBox.Name = "historyStartDateBox";
            this.historyStartDateBox.Size = new System.Drawing.Size(99, 22);
            this.historyStartDateBox.TabIndex = 69;
            // 
            // dataLegendBox
            // 
            this.dataLegendBox.Location = new System.Drawing.Point(103, 102);
            this.dataLegendBox.MaxLength = 50;
            this.dataLegendBox.Name = "dataLegendBox";
            this.dataLegendBox.Size = new System.Drawing.Size(442, 22);
            this.dataLegendBox.TabIndex = 68;
            // 
            // legendLbl
            // 
            this.legendLbl.BackColor = System.Drawing.Color.Transparent;
            this.legendLbl.Location = new System.Drawing.Point(16, 105);
            this.legendLbl.Name = "legendLbl";
            this.legendLbl.Size = new System.Drawing.Size(81, 16);
            this.legendLbl.TabIndex = 67;
            this.legendLbl.Text = "Legend :";
            this.legendLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelEnabledCb
            // 
            this.labelEnabledCb.AutoSize = true;
            this.labelEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.labelEnabledCb.Checked = true;
            this.labelEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.labelEnabledCb.Location = new System.Drawing.Point(103, 186);
            this.labelEnabledCb.Name = "labelEnabledCb";
            this.labelEnabledCb.Size = new System.Drawing.Size(98, 20);
            this.labelEnabledCb.TabIndex = 66;
            this.labelEnabledCb.Text = "Enable Tag";
            this.labelEnabledCb.UseVisualStyleBackColor = false;
            // 
            // dataPointEnabledCb
            // 
            this.dataPointEnabledCb.AutoSize = true;
            this.dataPointEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.dataPointEnabledCb.Checked = true;
            this.dataPointEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.dataPointEnabledCb.Location = new System.Drawing.Point(103, 160);
            this.dataPointEnabledCb.Name = "dataPointEnabledCb";
            this.dataPointEnabledCb.Size = new System.Drawing.Size(131, 20);
            this.dataPointEnabledCb.TabIndex = 65;
            this.dataPointEnabledCb.Text = "Enable Datapoint";
            this.dataPointEnabledCb.UseVisualStyleBackColor = false;
            // 
            // colorPanel
            // 
            this.colorPanel.BackColor = System.Drawing.Color.Transparent;
            this.colorPanel.Location = new System.Drawing.Point(446, 131);
            this.colorPanel.Name = "colorPanel";
            this.colorPanel.Size = new System.Drawing.Size(99, 21);
            this.colorPanel.TabIndex = 64;
            // 
            // cancelCmd
            // 
            this.cancelCmd.BackColor = System.Drawing.Color.Transparent;
            this.cancelCmd.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelCmd.Image = global::TrendViewer.Properties.Resources.Cancel;
            this.cancelCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelCmd.Location = new System.Drawing.Point(352, 234);
            this.cancelCmd.Margin = new System.Windows.Forms.Padding(4);
            this.cancelCmd.Name = "cancelCmd";
            this.cancelCmd.Size = new System.Drawing.Size(114, 33);
            this.cancelCmd.TabIndex = 63;
            this.cancelCmd.Text = "Cancel";
            this.cancelCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.cancelCmd.UseVisualStyleBackColor = false;
            this.cancelCmd.Click += new System.EventHandler(this.cancelCmd_Click);
            // 
            // okCmd
            // 
            this.okCmd.BackColor = System.Drawing.Color.Transparent;
            this.okCmd.Image = global::TrendViewer.Properties.Resources.Yes_v3;
            this.okCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.okCmd.Location = new System.Drawing.Point(231, 234);
            this.okCmd.Name = "okCmd";
            this.okCmd.Size = new System.Drawing.Size(114, 33);
            this.okCmd.TabIndex = 62;
            this.okCmd.Text = "OK";
            this.okCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.okCmd.UseVisualStyleBackColor = false;
            // 
            // lineTypeLbl
            // 
            this.lineTypeLbl.BackColor = System.Drawing.Color.Transparent;
            this.lineTypeLbl.Location = new System.Drawing.Point(2, 133);
            this.lineTypeLbl.Name = "lineTypeLbl";
            this.lineTypeLbl.Size = new System.Drawing.Size(95, 16);
            this.lineTypeLbl.TabIndex = 60;
            this.lineTypeLbl.Text = "Line Type :";
            this.lineTypeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // colorLbl
            // 
            this.colorLbl.BackColor = System.Drawing.Color.Transparent;
            this.colorLbl.Location = new System.Drawing.Point(381, 133);
            this.colorLbl.Name = "colorLbl";
            this.colorLbl.Size = new System.Drawing.Size(65, 16);
            this.colorLbl.TabIndex = 59;
            this.colorLbl.Text = "Colour :";
            this.colorLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // seriesTypeBox
            // 
            this.seriesTypeBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.seriesTypeBox.FormattingEnabled = true;
            this.seriesTypeBox.Location = new System.Drawing.Point(103, 130);
            this.seriesTypeBox.Name = "seriesTypeBox";
            this.seriesTypeBox.Size = new System.Drawing.Size(238, 24);
            this.seriesTypeBox.TabIndex = 61;
            // 
            // dataPointBox
            // 
            this.dataPointBox.Location = new System.Drawing.Point(103, 74);
            this.dataPointBox.Name = "dataPointBox";
            this.dataPointBox.Size = new System.Drawing.Size(442, 22);
            this.dataPointBox.TabIndex = 58;
            // 
            // dataPointLbl
            // 
            this.dataPointLbl.BackColor = System.Drawing.Color.Transparent;
            this.dataPointLbl.Location = new System.Drawing.Point(4, 77);
            this.dataPointLbl.Name = "dataPointLbl";
            this.dataPointLbl.Size = new System.Drawing.Size(93, 16);
            this.dataPointLbl.TabIndex = 57;
            this.dataPointLbl.Text = "Datapoint :";
            this.dataPointLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // HistDataPointData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(696, 285);
            this.Controls.Add(this.browseCmd);
            this.Controls.Add(this.colorCmd);
            this.Controls.Add(this.endTimeLbl);
            this.Controls.Add(this.historyEndDateBox);
            this.Controls.Add(this.opcServerHostBox);
            this.Controls.Add(this.opcServerNameBox);
            this.Controls.Add(this.hostNameLbl);
            this.Controls.Add(this.ServerLbl);
            this.Controls.Add(this.startTimeLbl);
            this.Controls.Add(this.historyStartDateBox);
            this.Controls.Add(this.dataLegendBox);
            this.Controls.Add(this.legendLbl);
            this.Controls.Add(this.labelEnabledCb);
            this.Controls.Add(this.dataPointEnabledCb);
            this.Controls.Add(this.colorPanel);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.okCmd);
            this.Controls.Add(this.lineTypeLbl);
            this.Controls.Add(this.colorLbl);
            this.Controls.Add(this.seriesTypeBox);
            this.Controls.Add(this.dataPointBox);
            this.Controls.Add(this.dataPointLbl);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "HistDataPointData";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Datapoint History";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.AddDataPoint_Paint);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Button browseCmd;
        private System.Windows.Forms.Button colorCmd;
        private System.Windows.Forms.Label endTimeLbl;
        internal System.Windows.Forms.TextBox opcServerHostBox;
        internal System.Windows.Forms.TextBox opcServerNameBox;
        private System.Windows.Forms.Label hostNameLbl;
        private System.Windows.Forms.Label ServerLbl;
        private System.Windows.Forms.Label startTimeLbl;
        internal System.Windows.Forms.TextBox dataLegendBox;
        private System.Windows.Forms.Label legendLbl;
        internal System.Windows.Forms.Panel colorPanel;
        private System.Windows.Forms.Button cancelCmd;
        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.Label lineTypeLbl;
        private System.Windows.Forms.Label colorLbl;
        private System.Windows.Forms.ColorDialog colorDialog2;
        internal System.Windows.Forms.ComboBox seriesTypeBox;
        internal System.Windows.Forms.TextBox dataPointBox;
        private System.Windows.Forms.Label dataPointLbl;
        internal System.Windows.Forms.DateTimePicker historyEndDateBox;
        internal System.Windows.Forms.DateTimePicker historyStartDateBox;
        internal System.Windows.Forms.CheckBox labelEnabledCb;
        internal System.Windows.Forms.CheckBox dataPointEnabledCb;
    }
}

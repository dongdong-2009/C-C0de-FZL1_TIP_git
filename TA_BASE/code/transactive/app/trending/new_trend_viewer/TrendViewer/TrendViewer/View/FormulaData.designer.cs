namespace TrendViewer.View
{
    partial class FormulaData
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
            this.components = new System.ComponentModel.Container();
            this.colorLbl = new System.Windows.Forms.Label();
            this.lineTypeLbl = new System.Windows.Forms.Label();
            this.seriesTypeBox = new System.Windows.Forms.ComboBox();
            this.cancelCmd = new System.Windows.Forms.Button();
            this.okCmd = new System.Windows.Forms.Button();
            this.colorPanel = new System.Windows.Forms.Panel();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.colorCmd = new System.Windows.Forms.Button();
            this.dataPointEnabledCb = new System.Windows.Forms.CheckBox();
            this.labelEnabledCb = new System.Windows.Forms.CheckBox();
            this.dataLegendBox = new System.Windows.Forms.TextBox();
            this.LegendLbl = new System.Windows.Forms.Label();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.checkValidDataPointsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.equationBox = new System.Windows.Forms.TextBox();
            this.exampleLbl = new System.Windows.Forms.Label();
            this.formulaLbl = new System.Windows.Forms.Label();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // colorLbl
            // 
            this.colorLbl.BackColor = System.Drawing.Color.Transparent;
            this.colorLbl.Location = new System.Drawing.Point(384, 43);
            this.colorLbl.Name = "colorLbl";
            this.colorLbl.Size = new System.Drawing.Size(70, 16);
            this.colorLbl.TabIndex = 2;
            this.colorLbl.Text = "Colour :";
            this.colorLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lineTypeLbl
            // 
            this.lineTypeLbl.BackColor = System.Drawing.Color.Transparent;
            this.lineTypeLbl.Location = new System.Drawing.Point(12, 44);
            this.lineTypeLbl.Name = "lineTypeLbl";
            this.lineTypeLbl.Size = new System.Drawing.Size(99, 16);
            this.lineTypeLbl.TabIndex = 3;
            this.lineTypeLbl.Text = "Line Type :";
            this.lineTypeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // seriesTypeBox
            // 
            this.seriesTypeBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.seriesTypeBox.FormattingEnabled = true;
            this.seriesTypeBox.Location = new System.Drawing.Point(117, 40);
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
            this.cancelCmd.Location = new System.Drawing.Point(470, 301);
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
            this.okCmd.Location = new System.Drawing.Point(349, 301);
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
            this.colorPanel.Location = new System.Drawing.Point(460, 41);
            this.colorPanel.Name = "colorPanel";
            this.colorPanel.Size = new System.Drawing.Size(99, 21);
            this.colorPanel.TabIndex = 37;
            // 
            // colorCmd
            // 
            this.colorCmd.BackColor = System.Drawing.Color.Transparent;
            this.colorCmd.Image = global::TrendViewer.Properties.Resources.Color_profile;
            this.colorCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.colorCmd.Location = new System.Drawing.Point(564, 40);
            this.colorCmd.Margin = new System.Windows.Forms.Padding(4);
            this.colorCmd.Name = "colorCmd";
            this.colorCmd.Size = new System.Drawing.Size(118, 24);
            this.colorCmd.TabIndex = 38;
            this.colorCmd.Text = "Select Colour";
            this.colorCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.colorCmd.UseVisualStyleBackColor = false;
            this.colorCmd.Click += new System.EventHandler(this.colorCmd_Click);
            // 
            // dataPointEnabledCb
            // 
            this.dataPointEnabledCb.AutoSize = true;
            this.dataPointEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.dataPointEnabledCb.Checked = true;
            this.dataPointEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.dataPointEnabledCb.Location = new System.Drawing.Point(117, 70);
            this.dataPointEnabledCb.Name = "dataPointEnabledCb";
            this.dataPointEnabledCb.Size = new System.Drawing.Size(131, 20);
            this.dataPointEnabledCb.TabIndex = 42;
            this.dataPointEnabledCb.Text = "Enable Datapoint";
            this.dataPointEnabledCb.UseVisualStyleBackColor = false;
            // 
            // labelEnabledCb
            // 
            this.labelEnabledCb.AutoSize = true;
            this.labelEnabledCb.BackColor = System.Drawing.Color.Transparent;
            this.labelEnabledCb.Checked = true;
            this.labelEnabledCb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.labelEnabledCb.Location = new System.Drawing.Point(460, 71);
            this.labelEnabledCb.Name = "labelEnabledCb";
            this.labelEnabledCb.Size = new System.Drawing.Size(98, 20);
            this.labelEnabledCb.TabIndex = 43;
            this.labelEnabledCb.Text = "Enable Tag";
            this.labelEnabledCb.UseVisualStyleBackColor = false;
            // 
            // dataLegendBox
            // 
            this.dataLegendBox.Location = new System.Drawing.Point(117, 12);
            this.dataLegendBox.Name = "dataLegendBox";
            this.dataLegendBox.Size = new System.Drawing.Size(801, 22);
            this.dataLegendBox.TabIndex = 45;
            // 
            // LegendLbl
            // 
            this.LegendLbl.BackColor = System.Drawing.Color.Transparent;
            this.LegendLbl.Location = new System.Drawing.Point(26, 15);
            this.LegendLbl.Name = "LegendLbl";
            this.LegendLbl.Size = new System.Drawing.Size(85, 16);
            this.LegendLbl.TabIndex = 44;
            this.LegendLbl.Text = "Legend :";
            this.LegendLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.checkValidDataPointsToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(184, 26);
            // 
            // checkValidDataPointsToolStripMenuItem
            // 
            this.checkValidDataPointsToolStripMenuItem.Name = "checkValidDataPointsToolStripMenuItem";
            this.checkValidDataPointsToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
            this.checkValidDataPointsToolStripMenuItem.Text = "Check Valid Datapoints";
            this.checkValidDataPointsToolStripMenuItem.Click += new System.EventHandler(this.checkValidDataPointsToolStripMenuItem_Click);
            // 
            // equationBox
            // 
            this.equationBox.ContextMenuStrip = this.contextMenuStrip1;
            this.equationBox.Location = new System.Drawing.Point(117, 115);
            this.equationBox.Multiline = true;
            this.equationBox.Name = "equationBox";
            this.equationBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.equationBox.Size = new System.Drawing.Size(801, 170);
            this.equationBox.TabIndex = 47;
            // 
            // exampleLbl
            // 
            this.exampleLbl.AutoSize = true;
            this.exampleLbl.BackColor = System.Drawing.Color.Transparent;
            this.exampleLbl.Location = new System.Drawing.Point(114, 96);
            this.exampleLbl.Name = "exampleLbl";
            this.exampleLbl.Size = new System.Drawing.Size(229, 16);
            this.exampleLbl.TabIndex = 49;
            this.exampleLbl.Text = "Eg : 2*::OPCDataPointName::+(100/5)";
            // 
            // formulaLbl
            // 
            this.formulaLbl.BackColor = System.Drawing.Color.Transparent;
            this.formulaLbl.Location = new System.Drawing.Point(23, 118);
            this.formulaLbl.Name = "formulaLbl";
            this.formulaLbl.Size = new System.Drawing.Size(88, 16);
            this.formulaLbl.TabIndex = 50;
            this.formulaLbl.Text = "Formula :";
            this.formulaLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // FormulaData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(932, 347);
            this.Controls.Add(this.dataLegendBox);
            this.Controls.Add(this.exampleLbl);
            this.Controls.Add(this.equationBox);
            this.Controls.Add(this.labelEnabledCb);
            this.Controls.Add(this.formulaLbl);
            this.Controls.Add(this.dataPointEnabledCb);
            this.Controls.Add(this.colorCmd);
            this.Controls.Add(this.colorPanel);
            this.Controls.Add(this.LegendLbl);
            this.Controls.Add(this.seriesTypeBox);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.okCmd);
            this.Controls.Add(this.lineTypeLbl);
            this.Controls.Add(this.colorLbl);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "FormulaData";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Edit Formula";
            this.Load += new System.EventHandler(this.EditDataPointFormula_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.EditDataPoint_Paint);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label colorLbl;
        private System.Windows.Forms.Label lineTypeLbl;
        private System.Windows.Forms.Button cancelCmd;
        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Button colorCmd;
        internal System.Windows.Forms.ComboBox seriesTypeBox;
        internal System.Windows.Forms.Panel colorPanel;
        internal System.Windows.Forms.TextBox dataLegendBox;
        private System.Windows.Forms.Label LegendLbl;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem checkValidDataPointsToolStripMenuItem;
        internal System.Windows.Forms.TextBox equationBox;
        private System.Windows.Forms.Label exampleLbl;
        private System.Windows.Forms.Label formulaLbl;
        internal System.Windows.Forms.CheckBox dataPointEnabledCb;
        internal System.Windows.Forms.CheckBox labelEnabledCb;
    }
}
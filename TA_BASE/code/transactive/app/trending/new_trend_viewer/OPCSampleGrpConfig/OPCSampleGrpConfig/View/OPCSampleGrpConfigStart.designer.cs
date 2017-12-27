namespace OPCSampleGrpConfig.View
{
    partial class OPCSampleGrpConfigStart
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            this.intervalTypeBox = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.intervalUnitBox = new System.Windows.Forms.NumericUpDown();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.deltaValueBox = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.disableCheckBox = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.dateTimePicker = new System.Windows.Forms.DateTimePicker();
            this.cancelMsgCmd = new System.Windows.Forms.Button();
            this.saveMsgCmd = new System.Windows.Forms.Button();
            this.intervalDescTextBox = new System.Windows.Forms.TextBox();
            this.intervalNameTextBox = new System.Windows.Forms.TextBox();
            this.LocationKeyTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.intervalConfigDataGridView = new System.Windows.Forms.DataGridView();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.firstIntervalConfigDataGridView = new System.Windows.Forms.ToolStripButton();
            this.prevIntervalConfigDataGridView = new System.Windows.Forms.ToolStripButton();
            this.toolStripTextBox1 = new System.Windows.Forms.ToolStripTextBox();
            this.nextIntervalConfigDataGridView = new System.Windows.Forms.ToolStripButton();
            this.lastIntervalConfigDataGridView = new System.Windows.Forms.ToolStripButton();
            this.totalIntervalConfigDataGridView = new System.Windows.Forms.ToolStripLabel();
            this.TotalPage_Tlbl = new System.Windows.Forms.ToolStripLabel();
            this.pageNumIntervalConfigDataGridView = new System.Windows.Forms.NumericUpDown();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.intervalUnitBox)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.deltaValueBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.intervalConfigDataGridView)).BeginInit();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageNumIntervalConfigDataGridView)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // intervalTypeBox
            // 
            this.intervalTypeBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.intervalTypeBox.FormattingEnabled = true;
            this.intervalTypeBox.Location = new System.Drawing.Point(542, 73);
            this.intervalTypeBox.Name = "intervalTypeBox";
            this.intervalTypeBox.Size = new System.Drawing.Size(156, 24);
            this.intervalTypeBox.TabIndex = 31;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(423, 77);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(92, 16);
            this.label6.TabIndex = 5;
            this.label6.Text = "Interval Type :";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(18, 74);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(91, 16);
            this.label7.TabIndex = 4;
            this.label7.Text = "Interval Time :";
            // 
            // intervalUnitBox
            // 
            this.intervalUnitBox.Location = new System.Drawing.Point(137, 72);
            this.intervalUnitBox.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.intervalUnitBox.Name = "intervalUnitBox";
            this.intervalUnitBox.Size = new System.Drawing.Size(142, 22);
            this.intervalUnitBox.TabIndex = 21;
            this.intervalUnitBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.deltaValueBox);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.disableCheckBox);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.dateTimePicker);
            this.groupBox1.Controls.Add(this.cancelMsgCmd);
            this.groupBox1.Controls.Add(this.saveMsgCmd);
            this.groupBox1.Controls.Add(this.intervalDescTextBox);
            this.groupBox1.Controls.Add(this.intervalNameTextBox);
            this.groupBox1.Controls.Add(this.LocationKeyTextBox);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.intervalUnitBox);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.intervalTypeBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(1113, 145);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            // 
            // deltaValueBox
            // 
            this.deltaValueBox.DecimalPlaces = 2;
            this.deltaValueBox.Location = new System.Drawing.Point(938, 75);
            this.deltaValueBox.Name = "deltaValueBox";
            this.deltaValueBox.Size = new System.Drawing.Size(155, 22);
            this.deltaValueBox.TabIndex = 41;
            this.deltaValueBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(848, 76);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(84, 16);
            this.label4.TabIndex = 56;
            this.label4.Text = "Delta Value :";
            // 
            // disableCheckBox
            // 
            this.disableCheckBox.AutoSize = true;
            this.disableCheckBox.Checked = true;
            this.disableCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.disableCheckBox.Location = new System.Drawing.Point(265, 105);
            this.disableCheckBox.Name = "disableCheckBox";
            this.disableCheckBox.Size = new System.Drawing.Size(74, 20);
            this.disableCheckBox.TabIndex = 53;
            this.disableCheckBox.Text = "Disable";
            this.disableCheckBox.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(18, 103);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(75, 16);
            this.label3.TabIndex = 54;
            this.label3.Text = "Start Time :";
            // 
            // dateTimePicker
            // 
            this.dateTimePicker.CustomFormat = "HH:mm";
            this.dateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.dateTimePicker.Location = new System.Drawing.Point(137, 103);
            this.dateTimePicker.Name = "dateTimePicker";
            this.dateTimePicker.ShowCheckBox = true;
            this.dateTimePicker.ShowUpDown = true;
            this.dateTimePicker.Size = new System.Drawing.Size(81, 22);
            this.dateTimePicker.TabIndex = 51;
            // 
            // cancelMsgCmd
            // 
            this.cancelMsgCmd.BackColor = System.Drawing.Color.Transparent;
            this.cancelMsgCmd.Image = global::OPCSampleGrpConfig.Properties.Resources.Erase_24;
            this.cancelMsgCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelMsgCmd.Location = new System.Drawing.Point(1001, 101);
            this.cancelMsgCmd.Name = "cancelMsgCmd";
            this.cancelMsgCmd.Size = new System.Drawing.Size(92, 38);
            this.cancelMsgCmd.TabIndex = 57;
            this.cancelMsgCmd.Text = "Cancel";
            this.cancelMsgCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.cancelMsgCmd.UseVisualStyleBackColor = false;
            this.cancelMsgCmd.Click += new System.EventHandler(this.cancelMsgCmd_Click);
            // 
            // saveMsgCmd
            // 
            this.saveMsgCmd.Image = global::OPCSampleGrpConfig.Properties.Resources.Save_file;
            this.saveMsgCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.saveMsgCmd.Location = new System.Drawing.Point(902, 101);
            this.saveMsgCmd.Margin = new System.Windows.Forms.Padding(4);
            this.saveMsgCmd.Name = "saveMsgCmd";
            this.saveMsgCmd.Size = new System.Drawing.Size(92, 38);
            this.saveMsgCmd.TabIndex = 55;
            this.saveMsgCmd.Text = "Save";
            this.saveMsgCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // intervalDescTextBox
            // 
            this.intervalDescTextBox.Location = new System.Drawing.Point(137, 44);
            this.intervalDescTextBox.MaxLength = 100;
            this.intervalDescTextBox.Name = "intervalDescTextBox";
            this.intervalDescTextBox.Size = new System.Drawing.Size(956, 22);
            this.intervalDescTextBox.TabIndex = 11;

            this.LocationKeyTextBox.Text = "0";
            this.LocationKeyTextBox.Location = new System.Drawing.Point(137, 44);
            this.LocationKeyTextBox.Name = "LocationkeyTextBox";
            this.LocationKeyTextBox.Size = new System.Drawing.Size(956, 22);
            this.LocationKeyTextBox.Visible = false;
            // 
            // intervalNameTextBox
            // 
            this.intervalNameTextBox.Location = new System.Drawing.Point(137, 15);
            this.intervalNameTextBox.MaxLength = 50;
            this.intervalNameTextBox.Name = "intervalNameTextBox";
            this.intervalNameTextBox.Size = new System.Drawing.Size(956, 22);
            this.intervalNameTextBox.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(18, 44);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(122, 16);
            this.label2.TabIndex = 9;
            this.label2.Text = "Group Description :";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(18, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(91, 16);
            this.label1.TabIndex = 8;
            this.label1.Text = "Group Name :";
            // 
            // intervalConfigDataGridView
            // 
            this.intervalConfigDataGridView.AllowUserToAddRows = false;
            this.intervalConfigDataGridView.AllowUserToDeleteRows = false;
            this.intervalConfigDataGridView.AllowUserToOrderColumns = true;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            dataGridViewCellStyle1.FormatProvider = new System.Globalization.CultureInfo("en-US");
            this.intervalConfigDataGridView.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.intervalConfigDataGridView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.intervalConfigDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.intervalConfigDataGridView.ColumnHeadersHeight = 24;
            this.intervalConfigDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.intervalConfigDataGridView.Cursor = System.Windows.Forms.Cursors.Arrow;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.intervalConfigDataGridView.DefaultCellStyle = dataGridViewCellStyle2;
            this.intervalConfigDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.intervalConfigDataGridView.Location = new System.Drawing.Point(3, 19);
            this.intervalConfigDataGridView.Margin = new System.Windows.Forms.Padding(6, 4, 6, 4);
            this.intervalConfigDataGridView.MultiSelect = false;
            this.intervalConfigDataGridView.Name = "intervalConfigDataGridView";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.FormatProvider = new System.Globalization.CultureInfo("en-US");
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.intervalConfigDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.intervalConfigDataGridView.RowHeadersVisible = false;
            this.intervalConfigDataGridView.RowTemplate.Height = 23;
            this.intervalConfigDataGridView.Size = new System.Drawing.Size(1101, 312);
            this.intervalConfigDataGridView.TabIndex = 19;
            this.intervalConfigDataGridView.Text = "dataGridView2";
            this.intervalConfigDataGridView.Sorted += new System.EventHandler(this.intervalConfigDataGridView_Sorted);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.firstIntervalConfigDataGridView,
            this.prevIntervalConfigDataGridView,
            this.toolStripTextBox1,
            this.nextIntervalConfigDataGridView,
            this.lastIntervalConfigDataGridView,
            this.totalIntervalConfigDataGridView,
            this.TotalPage_Tlbl});
            this.toolStrip1.Location = new System.Drawing.Point(3, 335);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1107, 25);
            this.toolStrip1.TabIndex = 20;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // firstIntervalConfigDataGridView
            // 
            this.firstIntervalConfigDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.firstIntervalConfigDataGridView.Image = global::OPCSampleGrpConfig.Properties.Resources.PreviousEnd;
            this.firstIntervalConfigDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.firstIntervalConfigDataGridView.Name = "firstIntervalConfigDataGridView";
            this.firstIntervalConfigDataGridView.Size = new System.Drawing.Size(23, 22);
            this.firstIntervalConfigDataGridView.Click += new System.EventHandler(this.firstIntervalConfigDataGridView_Click);
            // 
            // prevIntervalConfigDataGridView
            // 
            this.prevIntervalConfigDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.prevIntervalConfigDataGridView.Image = global::OPCSampleGrpConfig.Properties.Resources.Previous;
            this.prevIntervalConfigDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.prevIntervalConfigDataGridView.Name = "prevIntervalConfigDataGridView";
            this.prevIntervalConfigDataGridView.Size = new System.Drawing.Size(23, 22);
            this.prevIntervalConfigDataGridView.Click += new System.EventHandler(this.prevIntervalConfigDataGridView_Click);
            // 
            // toolStripTextBox1
            // 
            this.toolStripTextBox1.Margin = new System.Windows.Forms.Padding(10, 0, 1, 0);
            this.toolStripTextBox1.Name = "toolStripTextBox1";
            this.toolStripTextBox1.Size = new System.Drawing.Size(80, 25);
            // 
            // nextIntervalConfigDataGridView
            // 
            this.nextIntervalConfigDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.nextIntervalConfigDataGridView.Image = global::OPCSampleGrpConfig.Properties.Resources.Next;
            this.nextIntervalConfigDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.nextIntervalConfigDataGridView.Name = "nextIntervalConfigDataGridView";
            this.nextIntervalConfigDataGridView.Size = new System.Drawing.Size(23, 22);
            this.nextIntervalConfigDataGridView.Click += new System.EventHandler(this.nextIntervalConfigDataGridView_Click);
            // 
            // lastIntervalConfigDataGridView
            // 
            this.lastIntervalConfigDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.lastIntervalConfigDataGridView.Image = global::OPCSampleGrpConfig.Properties.Resources.NextEnd;
            this.lastIntervalConfigDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.lastIntervalConfigDataGridView.Name = "lastIntervalConfigDataGridView";
            this.lastIntervalConfigDataGridView.Size = new System.Drawing.Size(23, 22);
            this.lastIntervalConfigDataGridView.Click += new System.EventHandler(this.lastIntervalConfigDataGridView_Click);
            // 
            // totalIntervalConfigDataGridView
            // 
            this.totalIntervalConfigDataGridView.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.totalIntervalConfigDataGridView.Name = "totalIntervalConfigDataGridView";
            this.totalIntervalConfigDataGridView.Size = new System.Drawing.Size(13, 22);
            this.totalIntervalConfigDataGridView.Text = "0";
            // 
            // TotalPage_Tlbl
            // 
            this.TotalPage_Tlbl.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.TotalPage_Tlbl.Name = "TotalPage_Tlbl";
            this.TotalPage_Tlbl.Size = new System.Drawing.Size(77, 22);
            this.TotalPage_Tlbl.Text = "Total Page :    ";
            // 
            // pageNumIntervalConfigDataGridView
            // 
            this.pageNumIntervalConfigDataGridView.Location = new System.Drawing.Point(64, 336);
            this.pageNumIntervalConfigDataGridView.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumIntervalConfigDataGridView.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumIntervalConfigDataGridView.Name = "pageNumIntervalConfigDataGridView";
            this.pageNumIntervalConfigDataGridView.Size = new System.Drawing.Size(85, 22);
            this.pageNumIntervalConfigDataGridView.TabIndex = 21;
            this.pageNumIntervalConfigDataGridView.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumIntervalConfigDataGridView.ValueChanged += new System.EventHandler(this.pageNumIntervalConfigDataGridView_ValueChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.Color.Transparent;
            this.groupBox2.Controls.Add(this.pageNumIntervalConfigDataGridView);
            this.groupBox2.Controls.Add(this.toolStrip1);
            this.groupBox2.Controls.Add(this.intervalConfigDataGridView);
            this.groupBox2.Location = new System.Drawing.Point(12, 164);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(1113, 363);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            // 
            // OPCSampleGrpConfigStart
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1137, 530);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "OPCSampleGrpConfigStart";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "OPC DataLogger Configuration";
            this.Load += new System.EventHandler(this.AddLoggerInterval_Load);
            ((System.ComponentModel.ISupportInitialize)(this.intervalUnitBox)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.deltaValueBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.intervalConfigDataGridView)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageNumIntervalConfigDataGridView)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox intervalTypeBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown intervalUnitBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox intervalDescTextBox;
        private System.Windows.Forms.TextBox intervalNameTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button cancelMsgCmd;
        internal System.Windows.Forms.Button saveMsgCmd;
        private System.Windows.Forms.DataGridView intervalConfigDataGridView;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton firstIntervalConfigDataGridView;
        private System.Windows.Forms.ToolStripButton prevIntervalConfigDataGridView;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox1;
        private System.Windows.Forms.ToolStripButton nextIntervalConfigDataGridView;
        private System.Windows.Forms.ToolStripButton lastIntervalConfigDataGridView;
        private System.Windows.Forms.ToolStripLabel totalIntervalConfigDataGridView;
        private System.Windows.Forms.ToolStripLabel TotalPage_Tlbl;
        private System.Windows.Forms.NumericUpDown pageNumIntervalConfigDataGridView;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.DateTimePicker dateTimePicker;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox disableCheckBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown deltaValueBox;
        private System.Windows.Forms.TextBox LocationKeyTextBox;
    }
}
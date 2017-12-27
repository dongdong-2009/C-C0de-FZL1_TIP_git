namespace TrainTimeTableViewer.View
{
    partial class TrainTimeTableView
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
            this.timeInterval = new System.Windows.Forms.Label();
            this.datePicker = new System.Windows.Forms.DateTimePicker();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.endTimePicker = new System.Windows.Forms.DateTimePicker();
            this.startTimePicker = new System.Windows.Forms.DateTimePicker();
            this.Date = new System.Windows.Forms.Label();
            this.EndTimeLbl = new System.Windows.Forms.Label();
            this.StrTimeLbl = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.Mintues = new System.Windows.Forms.Label();
            this.timeIntervalUpDown = new System.Windows.Forms.NumericUpDown();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.planned = new System.Windows.Forms.CheckBox();
            this.practical = new System.Windows.Forms.CheckBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.printToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.StartButton = new System.Windows.Forms.Button();
            this.ChartGrpBox = new System.Windows.Forms.GroupBox();
            this.timeTableChartCtrl1 = new TrainTimeTableChartCtrl.TimeTableChartCtrl();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timeIntervalUpDown)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.ChartGrpBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // timeInterval
            // 
            this.timeInterval.AutoSize = true;
            this.timeInterval.Location = new System.Drawing.Point(6, 30);
            this.timeInterval.Name = "timeInterval";
            this.timeInterval.Size = new System.Drawing.Size(55, 13);
            this.timeInterval.TabIndex = 4;
            this.timeInterval.Text = "时间间隔";
            // 
            // datePicker
            // 
            this.datePicker.CustomFormat = "dd/MM/yyyy";
            this.datePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.datePicker.Location = new System.Drawing.Point(42, 30);
            this.datePicker.Name = "datePicker";
            this.datePicker.Size = new System.Drawing.Size(89, 20);
            this.datePicker.TabIndex = 8;
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.SystemColors.Control;
            this.groupBox1.Controls.Add(this.endTimePicker);
            this.groupBox1.Controls.Add(this.startTimePicker);
            this.groupBox1.Controls.Add(this.Date);
            this.groupBox1.Controls.Add(this.EndTimeLbl);
            this.groupBox1.Controls.Add(this.StrTimeLbl);
            this.groupBox1.Controls.Add(this.datePicker);
            this.groupBox1.Location = new System.Drawing.Point(12, 27);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(317, 71);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "时间";
            // 
            // endTimePicker
            // 
            this.endTimePicker.CustomFormat = "HH:mm:ss";
            this.endTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.endTimePicker.Location = new System.Drawing.Point(234, 42);
            this.endTimePicker.Name = "endTimePicker";
            this.endTimePicker.ShowUpDown = true;
            this.endTimePicker.Size = new System.Drawing.Size(77, 20);
            this.endTimePicker.TabIndex = 13;
            // 
            // startTimePicker
            // 
            this.startTimePicker.CustomFormat = "HH:mm:ss";
            this.startTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.startTimePicker.Location = new System.Drawing.Point(234, 16);
            this.startTimePicker.Name = "startTimePicker";
            this.startTimePicker.ShowUpDown = true;
            this.startTimePicker.Size = new System.Drawing.Size(77, 20);
            this.startTimePicker.TabIndex = 12;
            // 
            // Date
            // 
            this.Date.AutoSize = true;
            this.Date.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Date.Location = new System.Drawing.Point(6, 35);
            this.Date.Name = "Date";
            this.Date.Size = new System.Drawing.Size(31, 13);
            this.Date.TabIndex = 11;
            this.Date.Text = "时间";
            // 
            // EndTimeLbl
            // 
            this.EndTimeLbl.AutoSize = true;
            this.EndTimeLbl.Location = new System.Drawing.Point(173, 45);
            this.EndTimeLbl.Name = "EndTimeLbl";
            this.EndTimeLbl.Size = new System.Drawing.Size(55, 13);
            this.EndTimeLbl.TabIndex = 10;
            this.EndTimeLbl.Text = "结束时间";
            // 
            // StrTimeLbl
            // 
            this.StrTimeLbl.AutoSize = true;
            this.StrTimeLbl.Location = new System.Drawing.Point(173, 19);
            this.StrTimeLbl.Name = "StrTimeLbl";
            this.StrTimeLbl.Size = new System.Drawing.Size(55, 13);
            this.StrTimeLbl.TabIndex = 9;
            this.StrTimeLbl.Text = "开始时间";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.Mintues);
            this.groupBox2.Controls.Add(this.timeIntervalUpDown);
            this.groupBox2.Controls.Add(this.timeInterval);
            this.groupBox2.Location = new System.Drawing.Point(385, 27);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(141, 71);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "时间间隔";
            // 
            // Mintues
            // 
            this.Mintues.AutoSize = true;
            this.Mintues.Location = new System.Drawing.Point(118, 30);
            this.Mintues.Name = "Mintues";
            this.Mintues.Size = new System.Drawing.Size(19, 13);
            this.Mintues.TabIndex = 6;
            this.Mintues.Text = "分";
            // 
            // timeIntervalUpDown
            // 
            this.timeIntervalUpDown.Location = new System.Drawing.Point(67, 28);
            this.timeIntervalUpDown.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
            this.timeIntervalUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.timeIntervalUpDown.Name = "timeIntervalUpDown";
            this.timeIntervalUpDown.Size = new System.Drawing.Size(45, 20);
            this.timeIntervalUpDown.TabIndex = 5;
            this.timeIntervalUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.planned);
            this.groupBox3.Controls.Add(this.practical);
            this.groupBox3.Location = new System.Drawing.Point(586, 27);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(147, 71);
            this.groupBox3.TabIndex = 11;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "显示模式";
            // 
            // planned
            // 
            this.planned.AutoSize = true;
            this.planned.Location = new System.Drawing.Point(6, 21);
            this.planned.Name = "planned";
            this.planned.Size = new System.Drawing.Size(110, 17);
            this.planned.TabIndex = 0;
            this.planned.Text = "计划列车时刻表";
            this.planned.UseVisualStyleBackColor = true;
            // 
            // practical
            // 
            this.practical.AutoSize = true;
            this.practical.Location = new System.Drawing.Point(6, 44);
            this.practical.Name = "practical";
            this.practical.Size = new System.Drawing.Size(110, 17);
            this.practical.TabIndex = 1;
            this.practical.Text = "实际列车时刻表";
            this.practical.UseVisualStyleBackColor = true;
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1264, 24);
            this.menuStrip1.TabIndex = 12;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToolStripMenuItem,
            this.printToolStripMenuItem});
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(43, 20);
            this.toolStripMenuItem1.Text = "文件";
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Enabled = false;
            this.saveToolStripMenuItem.Image = global::TrainTimeTableViewer.Properties.Resources.Save_icon;
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(98, 22);
            this.saveToolStripMenuItem.Text = "保存";
            // 
            // printToolStripMenuItem
            // 
            this.printToolStripMenuItem.Enabled = false;
            this.printToolStripMenuItem.Image = global::TrainTimeTableViewer.Properties.Resources.Print;
            this.printToolStripMenuItem.Name = "printToolStripMenuItem";
            this.printToolStripMenuItem.Size = new System.Drawing.Size(98, 22);
            this.printToolStripMenuItem.Text = "打印";
            // 
            // StartButton
            // 
            this.StartButton.Image = global::TrainTimeTableViewer.Properties.Resources.Chart;
            this.StartButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.StartButton.Location = new System.Drawing.Point(838, 42);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(80, 42);
            this.StartButton.TabIndex = 13;
            this.StartButton.Text = "开始";
            this.StartButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.StartButton.UseVisualStyleBackColor = true;
            // 
            // ChartGrpBox
            // 
            this.ChartGrpBox.BackColor = System.Drawing.SystemColors.Control;
            this.ChartGrpBox.Controls.Add(this.timeTableChartCtrl1);
            this.ChartGrpBox.Location = new System.Drawing.Point(12, 104);
            this.ChartGrpBox.Name = "ChartGrpBox";
            this.ChartGrpBox.Size = new System.Drawing.Size(1240, 630);
            this.ChartGrpBox.TabIndex = 15;
            this.ChartGrpBox.TabStop = false;
            this.ChartGrpBox.Text = "列车时刻表";
            // 
            // timeTableChartCtrl1
            // 
            this.timeTableChartCtrl1.BackColor = System.Drawing.SystemColors.Control;
            this.timeTableChartCtrl1.Location = new System.Drawing.Point(9, 17);
            this.timeTableChartCtrl1.Name = "timeTableChartCtrl1";
            this.timeTableChartCtrl1.Size = new System.Drawing.Size(1222, 605);
            this.timeTableChartCtrl1.TabIndex = 14;
            // 
            // TrainTimeTableView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(1264, 742);
            this.Controls.Add(this.ChartGrpBox);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.Name = "TrainTimeTableView";
            this.Text = "列车时刻表管理器";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timeIntervalUpDown)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ChartGrpBox.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label timeInterval;
        private System.Windows.Forms.DateTimePicker datePicker;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label EndTimeLbl;
        private System.Windows.Forms.Label StrTimeLbl;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox planned;
        private System.Windows.Forms.CheckBox practical;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem printToolStripMenuItem;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.Label Date;
        private System.Windows.Forms.NumericUpDown timeIntervalUpDown;
        private System.Windows.Forms.DateTimePicker endTimePicker;
        private System.Windows.Forms.DateTimePicker startTimePicker;
        private TrainTimeTableChartCtrl.TimeTableChartCtrl timeTableChartCtrl1;
        private System.Windows.Forms.Label Mintues;
        private System.Windows.Forms.GroupBox ChartGrpBox;
    }
}
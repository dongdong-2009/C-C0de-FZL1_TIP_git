namespace TrendViewer.View
{
    partial class TrendView
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
            this.realtime_grpbox = new System.Windows.Forms.GroupBox();
            this.mixedModeCb = new System.Windows.Forms.CheckBox();
            this.intervalTypeBox = new System.Windows.Forms.ComboBox();
            this.time_interval_lbl = new System.Windows.Forms.Label();
            this.intervalUnitBox = new System.Windows.Forms.NumericUpDown();
            this.frameTypeBox = new System.Windows.Forms.ComboBox();
            this.realTimeExecuteCmd = new System.Windows.Forms.Button();
            this.realtime3DCb = new System.Windows.Forms.CheckBox();
            this.frameUnitBox = new System.Windows.Forms.NumericUpDown();
            this.time_frame_lbl = new System.Windows.Forms.Label();
            this.opcSrv1ItemTree = new System.Windows.Forms.TreeView();
            this.historical3DCb = new System.Windows.Forms.CheckBox();
            this.endTimeBox = new System.Windows.Forms.DateTimePicker();
            this.startTimeBox = new System.Windows.Forms.DateTimePicker();
            this.endTimeLbl = new System.Windows.Forms.Label();
            this.startTimeLbl = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.displayDataPointsConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.displayDataMarkerConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dataLoggerConfigurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.displayDataHistoryConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.displayDataPointsFormulaConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.changeTrendTitleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.utilityCommandToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportHistoryDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.screenShotTrendToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.printorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.historical_GrpBox = new System.Windows.Forms.GroupBox();
            this.historicalExecuteCmd = new System.Windows.Forms.Button();
            this.chartGrpBox = new System.Windows.Forms.GroupBox();
            this.trendingChartCtl1 = new TrendingChartControl.TrendingChartCtl();
            this.graphModeLabel = new System.Windows.Forms.Label();
            this.printBtn = new System.Windows.Forms.Button();
            this.realtime_grpbox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.intervalUnitBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.frameUnitBox)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.historical_GrpBox.SuspendLayout();
            this.chartGrpBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // realtime_grpbox
            // 
            this.realtime_grpbox.BackColor = System.Drawing.Color.Transparent;
            this.realtime_grpbox.Controls.Add(this.mixedModeCb);
            this.realtime_grpbox.Controls.Add(this.intervalTypeBox);
            this.realtime_grpbox.Controls.Add(this.time_interval_lbl);
            this.realtime_grpbox.Controls.Add(this.intervalUnitBox);
            this.realtime_grpbox.Controls.Add(this.frameTypeBox);
            this.realtime_grpbox.Controls.Add(this.realTimeExecuteCmd);
            this.realtime_grpbox.Controls.Add(this.realtime3DCb);
            this.realtime_grpbox.Controls.Add(this.frameUnitBox);
            this.realtime_grpbox.Controls.Add(this.time_frame_lbl);
            this.realtime_grpbox.Location = new System.Drawing.Point(12, 28);
            this.realtime_grpbox.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.realtime_grpbox.Name = "realtime_grpbox";
            this.realtime_grpbox.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.realtime_grpbox.Size = new System.Drawing.Size(630, 84);
            this.realtime_grpbox.TabIndex = 7;
            this.realtime_grpbox.TabStop = false;
            this.realtime_grpbox.Text = "Realtime Datapoint";
            // 
            // mixedModeCb
            // 
            this.mixedModeCb.AutoSize = true;
            this.mixedModeCb.Location = new System.Drawing.Point(326, 30);
            this.mixedModeCb.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.mixedModeCb.Name = "mixedModeCb";
            this.mixedModeCb.Size = new System.Drawing.Size(101, 20);
            this.mixedModeCb.TabIndex = 37;
            this.mixedModeCb.Text = "Mixed Mode";
            this.mixedModeCb.UseVisualStyleBackColor = true;
            // 
            // intervalTypeBox
            // 
            this.intervalTypeBox.FormattingEnabled = true;
            this.intervalTypeBox.Location = new System.Drawing.Point(186, 20);
            this.intervalTypeBox.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.intervalTypeBox.Name = "intervalTypeBox";
            this.intervalTypeBox.Size = new System.Drawing.Size(117, 24);
            this.intervalTypeBox.TabIndex = 6;
            // 
            // time_interval_lbl
            // 
            this.time_interval_lbl.Location = new System.Drawing.Point(0, 24);
            this.time_interval_lbl.Name = "time_interval_lbl";
            this.time_interval_lbl.Size = new System.Drawing.Size(111, 16);
            this.time_interval_lbl.TabIndex = 4;
            this.time_interval_lbl.Text = "Plotting Interval:";
            this.time_interval_lbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // intervalUnitBox
            // 
            this.intervalUnitBox.Location = new System.Drawing.Point(121, 21);
            this.intervalUnitBox.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.intervalUnitBox.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.intervalUnitBox.Name = "intervalUnitBox";
            this.intervalUnitBox.Size = new System.Drawing.Size(59, 22);
            this.intervalUnitBox.TabIndex = 5;
            this.intervalUnitBox.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // frameTypeBox
            // 
            this.frameTypeBox.FormattingEnabled = true;
            this.frameTypeBox.Location = new System.Drawing.Point(186, 52);
            this.frameTypeBox.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.frameTypeBox.Name = "frameTypeBox";
            this.frameTypeBox.Size = new System.Drawing.Size(117, 24);
            this.frameTypeBox.TabIndex = 14;
            this.frameTypeBox.SelectedIndexChanged += new System.EventHandler(this.frameTypeBox_SelectedIndexChanged);
            // 
            // realTimeExecuteCmd
            // 
            this.realTimeExecuteCmd.Image = global::TrendViewer.Properties.Resources.Chart;
            this.realTimeExecuteCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.realTimeExecuteCmd.Location = new System.Drawing.Point(500, 25);
            this.realTimeExecuteCmd.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.realTimeExecuteCmd.Name = "realTimeExecuteCmd";
            this.realTimeExecuteCmd.Size = new System.Drawing.Size(105, 35);
            this.realTimeExecuteCmd.TabIndex = 9;
            this.realTimeExecuteCmd.Text = "Start";
            this.realTimeExecuteCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.realTimeExecuteCmd.UseVisualStyleBackColor = true;
            // 
            // realtime3DCb
            // 
            this.realtime3DCb.AutoSize = true;
            this.realtime3DCb.Location = new System.Drawing.Point(434, 30);
            this.realtime3DCb.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.realtime3DCb.Name = "realtime3DCb";
            this.realtime3DCb.Size = new System.Drawing.Size(44, 20);
            this.realtime3DCb.TabIndex = 13;
            this.realtime3DCb.Text = "3D";
            this.realtime3DCb.UseVisualStyleBackColor = true;
            this.realtime3DCb.CheckedChanged += new System.EventHandler(this.RealTime3DCb_CheckedChanged);
            // 
            // frameUnitBox
            // 
            this.frameUnitBox.Location = new System.Drawing.Point(121, 53);
            this.frameUnitBox.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.frameUnitBox.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
            this.frameUnitBox.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.frameUnitBox.Name = "frameUnitBox";
            this.frameUnitBox.Size = new System.Drawing.Size(59, 22);
            this.frameUnitBox.TabIndex = 15;
            this.frameUnitBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // time_frame_lbl
            // 
            this.time_frame_lbl.Location = new System.Drawing.Point(21, 56);
            this.time_frame_lbl.Name = "time_frame_lbl";
            this.time_frame_lbl.Size = new System.Drawing.Size(90, 16);
            this.time_frame_lbl.TabIndex = 16;
            this.time_frame_lbl.Text = "Time Frame:";
            this.time_frame_lbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // opcSrv1ItemTree
            // 
            this.opcSrv1ItemTree.Location = new System.Drawing.Point(199, -7);
            this.opcSrv1ItemTree.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.opcSrv1ItemTree.Name = "opcSrv1ItemTree";
            this.opcSrv1ItemTree.Size = new System.Drawing.Size(159, 128);
            this.opcSrv1ItemTree.TabIndex = 9;
            this.opcSrv1ItemTree.Visible = false;
            // 
            // historical3DCb
            // 
            this.historical3DCb.AutoSize = true;
            this.historical3DCb.Location = new System.Drawing.Point(231, 28);
            this.historical3DCb.Name = "historical3DCb";
            this.historical3DCb.Size = new System.Drawing.Size(44, 20);
            this.historical3DCb.TabIndex = 37;
            this.historical3DCb.Text = "3D";
            this.historical3DCb.UseVisualStyleBackColor = true;
            this.historical3DCb.CheckedChanged += new System.EventHandler(this.History3DCb_CheckedChanged);
            // 
            // endTimeBox
            // 
            this.endTimeBox.CustomFormat = "HH:mm:ss";
            this.endTimeBox.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.endTimeBox.Location = new System.Drawing.Point(104, 49);
            this.endTimeBox.Name = "endTimeBox";
            this.endTimeBox.ShowUpDown = true;
            this.endTimeBox.Size = new System.Drawing.Size(78, 22);
            this.endTimeBox.TabIndex = 35;
            // 
            // startTimeBox
            // 
            this.startTimeBox.CustomFormat = "HH:mm:ss";
            this.startTimeBox.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.startTimeBox.Location = new System.Drawing.Point(104, 21);
            this.startTimeBox.Name = "startTimeBox";
            this.startTimeBox.ShowUpDown = true;
            this.startTimeBox.Size = new System.Drawing.Size(78, 22);
            this.startTimeBox.TabIndex = 34;
            // 
            // endTimeLbl
            // 
            this.endTimeLbl.Location = new System.Drawing.Point(-27, 52);
            this.endTimeLbl.Name = "endTimeLbl";
            this.endTimeLbl.Size = new System.Drawing.Size(125, 16);
            this.endTimeLbl.TabIndex = 32;
            this.endTimeLbl.Text = "End Time:";
            this.endTimeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // startTimeLbl
            // 
            this.startTimeLbl.Location = new System.Drawing.Point(-41, 24);
            this.startTimeLbl.Name = "startTimeLbl";
            this.startTimeLbl.Size = new System.Drawing.Size(139, 16);
            this.startTimeLbl.TabIndex = 0;
            this.startTimeLbl.Text = "Start Time:";
            this.startTimeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.Color.Transparent;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.configurationToolStripMenuItem,
            this.utilityCommandToolStripMenuItem,
            this.aboutToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1270, 24);
            this.menuStrip1.TabIndex = 35;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.displayDataPointsConfigToolStripMenuItem,
            this.displayDataMarkerConfigToolStripMenuItem,
            this.dataLoggerConfigurationToolStripMenuItem,
            this.displayDataHistoryConfigToolStripMenuItem,
            this.displayDataPointsFormulaConfigToolStripMenuItem,
            this.changeTrendTitleToolStripMenuItem});
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(84, 20);
            this.configurationToolStripMenuItem.Text = "Configuration";
            // 
            // displayDataPointsConfigToolStripMenuItem
            // 
            this.displayDataPointsConfigToolStripMenuItem.Name = "displayDataPointsConfigToolStripMenuItem";
            this.displayDataPointsConfigToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.displayDataPointsConfigToolStripMenuItem.Text = "Datapoint Configuration";
            // 
            // displayDataMarkerConfigToolStripMenuItem
            // 
            this.displayDataMarkerConfigToolStripMenuItem.Name = "displayDataMarkerConfigToolStripMenuItem";
            this.displayDataMarkerConfigToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.displayDataMarkerConfigToolStripMenuItem.Text = "Marker Data Configuration";
            // 
            // dataLoggerConfigurationToolStripMenuItem
            // 
            this.dataLoggerConfigurationToolStripMenuItem.Name = "dataLoggerConfigurationToolStripMenuItem";
            this.dataLoggerConfigurationToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.dataLoggerConfigurationToolStripMenuItem.Text = "OPC DataLog Configuration";
            this.dataLoggerConfigurationToolStripMenuItem.Visible = false;
            // 
            // displayDataHistoryConfigToolStripMenuItem
            // 
            this.displayDataHistoryConfigToolStripMenuItem.Name = "displayDataHistoryConfigToolStripMenuItem";
            this.displayDataHistoryConfigToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.displayDataHistoryConfigToolStripMenuItem.Text = "Historical Data Configuration";
            // 
            // displayDataPointsFormulaConfigToolStripMenuItem
            // 
            this.displayDataPointsFormulaConfigToolStripMenuItem.Name = "displayDataPointsFormulaConfigToolStripMenuItem";
            this.displayDataPointsFormulaConfigToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.displayDataPointsFormulaConfigToolStripMenuItem.Text = "Formula Configuration";
            this.displayDataPointsFormulaConfigToolStripMenuItem.Visible = false;
            // 
            // changeTrendTitleToolStripMenuItem
            // 
            this.changeTrendTitleToolStripMenuItem.Name = "changeTrendTitleToolStripMenuItem";
            this.changeTrendTitleToolStripMenuItem.Size = new System.Drawing.Size(211, 22);
            this.changeTrendTitleToolStripMenuItem.Text = "Change Trend Title";
            // 
            // utilityCommandToolStripMenuItem
            // 
            this.utilityCommandToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exportHistoryDataToolStripMenuItem,
            this.screenShotTrendToolStripMenuItem,
            this.printorToolStripMenuItem});
            this.utilityCommandToolStripMenuItem.Name = "utilityCommandToolStripMenuItem";
            this.utilityCommandToolStripMenuItem.Size = new System.Drawing.Size(96, 20);
            this.utilityCommandToolStripMenuItem.Text = "Utility Command";
            // 
            // exportHistoryDataToolStripMenuItem
            // 
            this.exportHistoryDataToolStripMenuItem.Name = "exportHistoryDataToolStripMenuItem";
            this.exportHistoryDataToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
            this.exportHistoryDataToolStripMenuItem.Text = "Export History Data";
            // 
            // screenShotTrendToolStripMenuItem
            // 
            this.screenShotTrendToolStripMenuItem.Name = "screenShotTrendToolStripMenuItem";
            this.screenShotTrendToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
            this.screenShotTrendToolStripMenuItem.Text = "Screenshot Trend";
            // 
            // printorToolStripMenuItem
            // 
            this.printorToolStripMenuItem.Name = "printorToolStripMenuItem";
            this.printorToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
            this.printorToolStripMenuItem.Text = "Print";
            this.printorToolStripMenuItem.Visible = false;
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Visible = false;
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            // 
            // historical_GrpBox
            // 
            this.historical_GrpBox.BackColor = System.Drawing.Color.Transparent;
            this.historical_GrpBox.Controls.Add(this.historical3DCb);
            this.historical_GrpBox.Controls.Add(this.endTimeBox);
            this.historical_GrpBox.Controls.Add(this.startTimeBox);
            this.historical_GrpBox.Controls.Add(this.historicalExecuteCmd);
            this.historical_GrpBox.Controls.Add(this.endTimeLbl);
            this.historical_GrpBox.Controls.Add(this.startTimeLbl);
            this.historical_GrpBox.Location = new System.Drawing.Point(648, 30);
            this.historical_GrpBox.Name = "historical_GrpBox";
            this.historical_GrpBox.Size = new System.Drawing.Size(434, 83);
            this.historical_GrpBox.TabIndex = 34;
            this.historical_GrpBox.TabStop = false;
            this.historical_GrpBox.Text = "Historical Datapoint";
            // 
            // historicalExecuteCmd
            // 
            this.historicalExecuteCmd.Image = global::TrendViewer.Properties.Resources.Chart;
            this.historicalExecuteCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.historicalExecuteCmd.Location = new System.Drawing.Point(323, 24);
            this.historicalExecuteCmd.Name = "historicalExecuteCmd";
            this.historicalExecuteCmd.Size = new System.Drawing.Size(105, 35);
            this.historicalExecuteCmd.TabIndex = 33;
            this.historicalExecuteCmd.Text = "Start";
            this.historicalExecuteCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.historicalExecuteCmd.UseVisualStyleBackColor = true;
            // 
            // chartGrpBox
            // 
            this.chartGrpBox.BackColor = System.Drawing.Color.Transparent;
            this.chartGrpBox.Controls.Add(this.trendingChartCtl1);
            this.chartGrpBox.Controls.Add(this.opcSrv1ItemTree);
            this.chartGrpBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chartGrpBox.Location = new System.Drawing.Point(12, 119);
            this.chartGrpBox.Name = "chartGrpBox";
            this.chartGrpBox.Size = new System.Drawing.Size(1246, 633);
            this.chartGrpBox.TabIndex = 37;
            this.chartGrpBox.TabStop = false;
            this.chartGrpBox.Text = "Trend";
            // 
            // trendingChartCtl1
            // 
            this.trendingChartCtl1.ChartTitle = "Data Trend";
            this.trendingChartCtl1.EnableSmartLabel = false;
            this.trendingChartCtl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.trendingChartCtl1.GapDepthFor3D = 100;
            this.trendingChartCtl1.Location = new System.Drawing.Point(6, 16);
            this.trendingChartCtl1.Margin = new System.Windows.Forms.Padding(8);
            this.trendingChartCtl1.Name = "trendingChartCtl1";
            this.trendingChartCtl1.Size = new System.Drawing.Size(1235, 606);
            this.trendingChartCtl1.TabIndex = 0;
            this.trendingChartCtl1.XTitle = "Time";
            this.trendingChartCtl1.YTitle = "Data Value";
            // 
            // graphModeLabel
            // 
            this.graphModeLabel.BackColor = System.Drawing.Color.Transparent;
            this.graphModeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.graphModeLabel.ForeColor = System.Drawing.Color.Red;
            this.graphModeLabel.Location = new System.Drawing.Point(1108, 18);
            this.graphModeLabel.Name = "graphModeLabel";
            this.graphModeLabel.Size = new System.Drawing.Size(145, 33);
            this.graphModeLabel.TabIndex = 0;
            this.graphModeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // printBtn
            // 
            this.printBtn.BackColor = System.Drawing.Color.Transparent;
            this.printBtn.Image = global::TrendViewer.Properties.Resources.Print;
            this.printBtn.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.printBtn.Location = new System.Drawing.Point(1127, 54);
            this.printBtn.Name = "printBtn";
            this.printBtn.Size = new System.Drawing.Size(105, 35);
            this.printBtn.TabIndex = 38;
            this.printBtn.Text = "Print";
            this.printBtn.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.printBtn.UseVisualStyleBackColor = false;
            // 
            // TrendView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(1270, 764);
            this.Controls.Add(this.printBtn);
            this.Controls.Add(this.graphModeLabel);
            this.Controls.Add(this.chartGrpBox);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.realtime_grpbox);
            this.Controls.Add(this.historical_GrpBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.Name = "TrendView";
            this.Text = "OPC TrendViewer";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.TrendViewer_Paint);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TrendView_FormClosed);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TrendViewer_FormClosing);
            this.realtime_grpbox.ResumeLayout(false);
            this.realtime_grpbox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.intervalUnitBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.frameUnitBox)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.historical_GrpBox.ResumeLayout(false);
            this.historical_GrpBox.PerformLayout();
            this.chartGrpBox.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox realtime_grpbox;
        private System.Windows.Forms.CheckBox mixedModeCb;
        private System.Windows.Forms.ComboBox intervalTypeBox;
        private System.Windows.Forms.Label time_interval_lbl;
        private System.Windows.Forms.NumericUpDown intervalUnitBox;
        private System.Windows.Forms.ComboBox frameTypeBox;
        private System.Windows.Forms.NumericUpDown frameUnitBox;
        private System.Windows.Forms.Label time_frame_lbl;
        private System.Windows.Forms.Button realTimeExecuteCmd;
        private System.Windows.Forms.CheckBox realtime3DCb;
        private System.Windows.Forms.TreeView opcSrv1ItemTree;
        private System.Windows.Forms.GroupBox historical_GrpBox;
        private System.Windows.Forms.CheckBox historical3DCb;
        private System.Windows.Forms.DateTimePicker endTimeBox;
        private System.Windows.Forms.DateTimePicker startTimeBox;
        private System.Windows.Forms.Button historicalExecuteCmd;
        private System.Windows.Forms.Label endTimeLbl;
        private System.Windows.Forms.Label startTimeLbl;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem displayDataPointsConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem displayDataMarkerConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem displayDataHistoryConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem displayDataPointsFormulaConfigToolStripMenuItem;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ToolStripMenuItem dataLoggerConfigurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem changeTrendTitleToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem utilityCommandToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exportHistoryDataToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem screenShotTrendToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private TrendingChartControl.TrendingChartCtl trendingChartCtl1;
        private System.Windows.Forms.GroupBox chartGrpBox;
        private System.Windows.Forms.ToolStripMenuItem printorToolStripMenuItem;
        private System.Windows.Forms.Label graphModeLabel;
        private System.Windows.Forms.Button printBtn;

    }
}


namespace TrendViewer.View
{
    partial class MarkerList
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addDataMarkerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadMarkerConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveMarkerConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.TotalPage_Tlbl = new System.Windows.Forms.ToolStripLabel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.pageNumdataMarkerListDataGridView = new System.Windows.Forms.NumericUpDown();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.firstdataMarkerListDataGridView = new System.Windows.Forms.ToolStripButton();
            this.prevdataMarkerListDataGridView = new System.Windows.Forms.ToolStripButton();
            this.toolStripTextBox1 = new System.Windows.Forms.ToolStripTextBox();
            this.nextdataMarkerListDataGridView = new System.Windows.Forms.ToolStripButton();
            this.lastdataMarkerListDataGridView = new System.Windows.Forms.ToolStripButton();
            this.totaldataMarkerListDataGridView = new System.Windows.Forms.ToolStripLabel();
            this.markerListGrpBox = new System.Windows.Forms.GroupBox();
            this.dataMarkerListDataGridViewprogressBar = new System.Windows.Forms.ProgressBar();
            this.dataMarkerListDataGridView = new System.Windows.Forms.DataGridView();
            this.contextMenuStrip1.SuspendLayout();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageNumdataMarkerListDataGridView)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.markerListGrpBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataMarkerListDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addDataMarkerToolStripMenuItem,
            this.loadMarkerConfigToolStripMenuItem,
            this.saveMarkerConfigToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(203, 70);
            // 
            // addDataMarkerToolStripMenuItem
            // 
            this.addDataMarkerToolStripMenuItem.Name = "addDataMarkerToolStripMenuItem";
            this.addDataMarkerToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.addDataMarkerToolStripMenuItem.Text = "Add Data Marker";
            // 
            // loadMarkerConfigToolStripMenuItem
            // 
            this.loadMarkerConfigToolStripMenuItem.Name = "loadMarkerConfigToolStripMenuItem";
            this.loadMarkerConfigToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.loadMarkerConfigToolStripMenuItem.Text = "Load Marker Configuration";
            // 
            // saveMarkerConfigToolStripMenuItem
            // 
            this.saveMarkerConfigToolStripMenuItem.Name = "saveMarkerConfigToolStripMenuItem";
            this.saveMarkerConfigToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.saveMarkerConfigToolStripMenuItem.Text = "Save Marker Configuration";
            // 
            // TotalPage_Tlbl
            // 
            this.TotalPage_Tlbl.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.TotalPage_Tlbl.Name = "TotalPage_Tlbl";
            this.TotalPage_Tlbl.Size = new System.Drawing.Size(77, 22);
            this.TotalPage_Tlbl.Text = "Total Page :    ";
            // 
            // panel3
            // 
            this.panel3.BackColor = System.Drawing.Color.Transparent;
            this.panel3.Controls.Add(this.pageNumdataMarkerListDataGridView);
            this.panel3.Controls.Add(this.toolStrip1);
            this.panel3.Location = new System.Drawing.Point(12, 289);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(1096, 24);
            this.panel3.TabIndex = 58;
            // 
            // pageNumdataMarkerListDataGridView
            // 
            this.pageNumdataMarkerListDataGridView.Location = new System.Drawing.Point(69, 1);
            this.pageNumdataMarkerListDataGridView.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumdataMarkerListDataGridView.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumdataMarkerListDataGridView.Name = "pageNumdataMarkerListDataGridView";
            this.pageNumdataMarkerListDataGridView.Size = new System.Drawing.Size(102, 22);
            this.pageNumdataMarkerListDataGridView.TabIndex = 7;
            this.pageNumdataMarkerListDataGridView.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pageNumdataMarkerListDataGridView.ValueChanged += new System.EventHandler(this.pageNumdataMarkerListDataGridView_ValueChanged);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.firstdataMarkerListDataGridView,
            this.prevdataMarkerListDataGridView,
            this.toolStripTextBox1,
            this.nextdataMarkerListDataGridView,
            this.lastdataMarkerListDataGridView,
            this.totaldataMarkerListDataGridView,
            this.TotalPage_Tlbl});
            this.toolStrip1.Location = new System.Drawing.Point(0, -1);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1096, 25);
            this.toolStrip1.TabIndex = 2;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // firstdataMarkerListDataGridView
            // 
            this.firstdataMarkerListDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.firstdataMarkerListDataGridView.Image = global::TrendViewer.Properties.Resources.PreviousEnd;
            this.firstdataMarkerListDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.firstdataMarkerListDataGridView.Name = "firstdataMarkerListDataGridView";
            this.firstdataMarkerListDataGridView.Size = new System.Drawing.Size(23, 22);
            this.firstdataMarkerListDataGridView.Click += new System.EventHandler(this.firstdataMarkerListDataGridView_Click);
            // 
            // prevdataMarkerListDataGridView
            // 
            this.prevdataMarkerListDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.prevdataMarkerListDataGridView.Image = global::TrendViewer.Properties.Resources.Previous;
            this.prevdataMarkerListDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.prevdataMarkerListDataGridView.Margin = new System.Windows.Forms.Padding(0, 1, 5, 2);
            this.prevdataMarkerListDataGridView.Name = "prevdataMarkerListDataGridView";
            this.prevdataMarkerListDataGridView.Size = new System.Drawing.Size(23, 22);
            this.prevdataMarkerListDataGridView.Click += new System.EventHandler(this.prevdataMarkerListDataGridView_Click);
            // 
            // toolStripTextBox1
            // 
            this.toolStripTextBox1.Margin = new System.Windows.Forms.Padding(10, 0, 1, 0);
            this.toolStripTextBox1.Name = "toolStripTextBox1";
            this.toolStripTextBox1.Size = new System.Drawing.Size(100, 25);
            // 
            // nextdataMarkerListDataGridView
            // 
            this.nextdataMarkerListDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.nextdataMarkerListDataGridView.Image = global::TrendViewer.Properties.Resources.Next;
            this.nextdataMarkerListDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.nextdataMarkerListDataGridView.Margin = new System.Windows.Forms.Padding(5, 1, 0, 2);
            this.nextdataMarkerListDataGridView.Name = "nextdataMarkerListDataGridView";
            this.nextdataMarkerListDataGridView.Size = new System.Drawing.Size(23, 22);
            this.nextdataMarkerListDataGridView.Click += new System.EventHandler(this.nextdataMarkerListDataGridView_Click);
            // 
            // lastdataMarkerListDataGridView
            // 
            this.lastdataMarkerListDataGridView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.lastdataMarkerListDataGridView.Image = global::TrendViewer.Properties.Resources.NextEnd;
            this.lastdataMarkerListDataGridView.ImageTransparentColor = System.Drawing.Color.Transparent;
            this.lastdataMarkerListDataGridView.Name = "lastdataMarkerListDataGridView";
            this.lastdataMarkerListDataGridView.Size = new System.Drawing.Size(23, 22);
            this.lastdataMarkerListDataGridView.Click += new System.EventHandler(this.lastdataMarkerListDataGridView_Click);
            // 
            // totaldataMarkerListDataGridView
            // 
            this.totaldataMarkerListDataGridView.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.totaldataMarkerListDataGridView.Name = "totaldataMarkerListDataGridView";
            this.totaldataMarkerListDataGridView.Size = new System.Drawing.Size(13, 22);
            this.totaldataMarkerListDataGridView.Text = "0";
            // 
            // markerListGrpBox
            // 
            this.markerListGrpBox.BackColor = System.Drawing.Color.Transparent;
            this.markerListGrpBox.Controls.Add(this.dataMarkerListDataGridViewprogressBar);
            this.markerListGrpBox.Controls.Add(this.dataMarkerListDataGridView);
            this.markerListGrpBox.Location = new System.Drawing.Point(12, 12);
            this.markerListGrpBox.Name = "markerListGrpBox";
            this.markerListGrpBox.Size = new System.Drawing.Size(1096, 273);
            this.markerListGrpBox.TabIndex = 57;
            this.markerListGrpBox.TabStop = false;
            this.markerListGrpBox.Text = "Data Marker List";
            // 
            // dataMarkerListDataGridViewprogressBar
            // 
            this.dataMarkerListDataGridViewprogressBar.ForeColor = System.Drawing.Color.Lime;
            this.dataMarkerListDataGridViewprogressBar.Location = new System.Drawing.Point(100, 125);
            this.dataMarkerListDataGridViewprogressBar.Name = "dataMarkerListDataGridViewprogressBar";
            this.dataMarkerListDataGridViewprogressBar.Size = new System.Drawing.Size(897, 23);
            this.dataMarkerListDataGridViewprogressBar.TabIndex = 15;
            // 
            // dataMarkerListDataGridView
            // 
            this.dataMarkerListDataGridView.AllowUserToAddRows = false;
            this.dataMarkerListDataGridView.AllowUserToDeleteRows = false;
            this.dataMarkerListDataGridView.AllowUserToOrderColumns = true;
            this.dataMarkerListDataGridView.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            dataGridViewCellStyle1.FormatProvider = new System.Globalization.CultureInfo("en-US");
            this.dataMarkerListDataGridView.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dataMarkerListDataGridView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataMarkerListDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dataMarkerListDataGridView.ColumnHeadersHeight = 24;
            this.dataMarkerListDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dataMarkerListDataGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataMarkerListDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dataMarkerListDataGridView.Location = new System.Drawing.Point(3, 18);
            this.dataMarkerListDataGridView.MultiSelect = false;
            this.dataMarkerListDataGridView.Name = "dataMarkerListDataGridView";
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F);
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle2.FormatProvider = new System.Globalization.CultureInfo("en-US");
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dataMarkerListDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.dataMarkerListDataGridView.RowHeadersVisible = false;
            this.dataMarkerListDataGridView.RowTemplate.Height = 23;
            this.dataMarkerListDataGridView.Size = new System.Drawing.Size(1090, 252);
            this.dataMarkerListDataGridView.TabIndex = 4;
            this.dataMarkerListDataGridView.Text = "dataGridView2";
            this.dataMarkerListDataGridView.Sorted += new System.EventHandler(this.dataMarkerListDataGridView_Sorted);
            // 
            // MarkerList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(1120, 325);
            this.ContextMenuStrip = this.contextMenuStrip1;
            this.Controls.Add(this.markerListGrpBox);
            this.Controls.Add(this.panel3);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "MarkerList";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Data Marker Viewer";
            this.Load += new System.EventHandler(this.DataMarkerViewer_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.DataMarkerViewer_Paint);
            this.contextMenuStrip1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageNumdataMarkerListDataGridView)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.markerListGrpBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataMarkerListDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem addDataMarkerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadMarkerConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveMarkerConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton nextdataMarkerListDataGridView;
        private System.Windows.Forms.ToolStripButton lastdataMarkerListDataGridView;
        private System.Windows.Forms.ToolStripButton firstdataMarkerListDataGridView;
        private System.Windows.Forms.ToolStripButton prevdataMarkerListDataGridView;
        private System.Windows.Forms.ToolStripLabel TotalPage_Tlbl;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.NumericUpDown pageNumdataMarkerListDataGridView;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox1;
        private System.Windows.Forms.ToolStripLabel totaldataMarkerListDataGridView;
        private System.Windows.Forms.GroupBox markerListGrpBox;
        private System.Windows.Forms.ProgressBar dataMarkerListDataGridViewprogressBar;
        private System.Windows.Forms.DataGridView dataMarkerListDataGridView;
    }
}
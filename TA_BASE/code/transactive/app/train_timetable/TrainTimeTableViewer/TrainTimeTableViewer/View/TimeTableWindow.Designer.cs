namespace TrainTimeTableViewer.View
{
    partial class TimeTableWindow
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
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.timeTableDataGridView = new System.Windows.Forms.DataGridView();
            this.StationName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ArrTime = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DeptTime = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timeTableDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.SystemColors.Control;
            this.groupBox1.Controls.Add(this.timeTableDataGridView);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(331, 441);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "信息";
            // 
            // timeTableDataGridView
            // 
            this.timeTableDataGridView.AllowUserToAddRows = false;
            this.timeTableDataGridView.AllowUserToDeleteRows = false;
            this.timeTableDataGridView.AllowUserToOrderColumns = true;
            this.timeTableDataGridView.AllowUserToResizeRows = false;
            this.timeTableDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.timeTableDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.timeTableDataGridView.ColumnHeadersHeight = 24;
            this.timeTableDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.timeTableDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.StationName,
            this.ArrTime,
            this.DeptTime});
            this.timeTableDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.timeTableDataGridView.Location = new System.Drawing.Point(14, 19);
            this.timeTableDataGridView.MultiSelect = false;
            this.timeTableDataGridView.Name = "timeTableDataGridView";
            this.timeTableDataGridView.ReadOnly = true;
            this.timeTableDataGridView.RightToLeft = System.Windows.Forms.RightToLeft.No;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.timeTableDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.timeTableDataGridView.RowHeadersVisible = false;
            this.timeTableDataGridView.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.timeTableDataGridView.RowTemplate.DefaultCellStyle.BackColor = System.Drawing.Color.White;
            this.timeTableDataGridView.RowTemplate.DefaultCellStyle.ForeColor = System.Drawing.Color.Black;
            this.timeTableDataGridView.RowTemplate.Height = 23;
            this.timeTableDataGridView.Size = new System.Drawing.Size(311, 416);
            this.timeTableDataGridView.TabIndex = 4;
            this.timeTableDataGridView.ScrollBars = System.Windows.Forms.ScrollBars.Vertical; ;
            // 
            // StationName
            // 
            this.StationName.DividerWidth = 1;
            this.StationName.HeaderText = "车站";
            this.StationName.Name = "StationName";
            this.StationName.ReadOnly = true;
            this.StationName.Width = 80;
            // 
            // ArrTime
            // 
            this.ArrTime.DividerWidth = 1;
            this.ArrTime.HeaderText = "列车到站时间";
            this.ArrTime.Name = "ArrTime";
            this.ArrTime.ReadOnly = true;
            this.ArrTime.Width = 110;
            // 
            // DeptTime
            // 
            this.DeptTime.DividerWidth = 1;
            this.DeptTime.HeaderText = "列车离站时间";
            this.DeptTime.Name = "DeptTime";
            this.DeptTime.ReadOnly = true;
            this.DeptTime.Width = 110;
            // 
            // TimeTableWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(355, 463);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "TimeTableWindow";
            this.Text = "TimeTableWindow";
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.timeTableDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.DataGridView timeTableDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn StationName;
        private System.Windows.Forms.DataGridViewTextBoxColumn ArrTime;
        private System.Windows.Forms.DataGridViewTextBoxColumn DeptTime;
    }
}
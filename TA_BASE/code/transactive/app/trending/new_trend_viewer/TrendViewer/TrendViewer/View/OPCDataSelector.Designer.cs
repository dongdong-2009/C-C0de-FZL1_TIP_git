namespace TrendViewer.View
{
    partial class OPCDataSelector
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
            this.dpList = new System.Windows.Forms.GroupBox();
            this.opcItemDataGridView = new System.Windows.Forms.DataGridView();
            this.dataSource = new System.Windows.Forms.GroupBox();
            this.treeOpcItems = new System.Windows.Forms.TreeView();
            this.OPCServerLbl = new System.Windows.Forms.Label();
            this.opcServerHostBox = new System.Windows.Forms.Label();
            this.dpList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.opcItemDataGridView)).BeginInit();
            this.dataSource.SuspendLayout();
            this.SuspendLayout();
            // 
            // dpList
            // 
            this.dpList.BackColor = System.Drawing.Color.Transparent;
            this.dpList.Controls.Add(this.opcItemDataGridView);
            this.dpList.Location = new System.Drawing.Point(397, 36);
            this.dpList.Name = "dpList";
            this.dpList.Size = new System.Drawing.Size(762, 757);
            this.dpList.TabIndex = 31;
            this.dpList.TabStop = false;
            this.dpList.Text = "Datapoint List";
            // 
            // opcItemDataGridView
            // 
            this.opcItemDataGridView.AllowUserToAddRows = false;
            this.opcItemDataGridView.AllowUserToDeleteRows = false;
            this.opcItemDataGridView.AllowUserToOrderColumns = true;
            this.opcItemDataGridView.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            dataGridViewCellStyle1.FormatProvider = new System.Globalization.CultureInfo("en-US");
            this.opcItemDataGridView.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.opcItemDataGridView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.opcItemDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.opcItemDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.opcItemDataGridView.Cursor = System.Windows.Forms.Cursors.Hand;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.opcItemDataGridView.DefaultCellStyle = dataGridViewCellStyle2;
            this.opcItemDataGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.opcItemDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.opcItemDataGridView.Location = new System.Drawing.Point(3, 18);
            this.opcItemDataGridView.Margin = new System.Windows.Forms.Padding(4);
            this.opcItemDataGridView.MultiSelect = false;
            this.opcItemDataGridView.Name = "opcItemDataGridView";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.FormatProvider = new System.Globalization.CultureInfo("en-US");
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.opcItemDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.opcItemDataGridView.RowHeadersVisible = false;
            this.opcItemDataGridView.RowTemplate.Height = 23;
            this.opcItemDataGridView.Size = new System.Drawing.Size(756, 736);
            this.opcItemDataGridView.TabIndex = 7;
            this.opcItemDataGridView.Text = "dataGridView2";
            // 
            // dataSource
            // 
            this.dataSource.BackColor = System.Drawing.Color.Transparent;
            this.dataSource.Controls.Add(this.treeOpcItems);
            this.dataSource.Location = new System.Drawing.Point(24, 36);
            this.dataSource.Name = "dataSource";
            this.dataSource.Size = new System.Drawing.Size(367, 760);
            this.dataSource.TabIndex = 30;
            this.dataSource.TabStop = false;
            this.dataSource.Text = "TransActiveDataSource";
            // 
            // treeOpcItems
            // 
            this.treeOpcItems.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeOpcItems.HideSelection = false;
            this.treeOpcItems.Location = new System.Drawing.Point(3, 18);
            this.treeOpcItems.Name = "treeOpcItems";
            this.treeOpcItems.Size = new System.Drawing.Size(361, 739);
            this.treeOpcItems.TabIndex = 19;
            // 
            // OPCServerLbl
            // 
            this.OPCServerLbl.AutoSize = true;
            this.OPCServerLbl.BackColor = System.Drawing.Color.Transparent;
            this.OPCServerLbl.Location = new System.Drawing.Point(24, 17);
            this.OPCServerLbl.Name = "OPCServerLbl";
            this.OPCServerLbl.Size = new System.Drawing.Size(85, 16);
            this.OPCServerLbl.TabIndex = 29;
            this.OPCServerLbl.Text = "OPC Server :";
            // 
            // opcServerHostBox
            // 
            this.opcServerHostBox.AutoSize = true;
            this.opcServerHostBox.BackColor = System.Drawing.Color.Transparent;
            this.opcServerHostBox.Location = new System.Drawing.Point(150, 17);
            this.opcServerHostBox.Name = "opcServerHostBox";
            this.opcServerHostBox.Size = new System.Drawing.Size(0, 16);
            this.opcServerHostBox.TabIndex = 32;
            // 
            // OPCDataSelector
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(1180, 805);
            this.Controls.Add(this.opcServerHostBox);
            this.Controls.Add(this.OPCServerLbl);
            this.Controls.Add(this.dpList);
            this.Controls.Add(this.dataSource);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "OPCDataSelector";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "OPC Browser";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.OPCDataSelector_Paint);
            this.dpList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.opcItemDataGridView)).EndInit();
            this.dataSource.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox dpList;
        private System.Windows.Forms.DataGridView opcItemDataGridView;
        private System.Windows.Forms.GroupBox dataSource;
        private System.Windows.Forms.TreeView treeOpcItems;
        private System.Windows.Forms.Label OPCServerLbl;
        private System.Windows.Forms.Label opcServerHostBox;
    }
}

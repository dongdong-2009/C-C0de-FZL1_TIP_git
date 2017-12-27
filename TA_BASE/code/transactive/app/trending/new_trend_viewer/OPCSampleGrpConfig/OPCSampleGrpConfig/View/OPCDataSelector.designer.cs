namespace OPCSampleGrpConfig.View
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            this.serverInfoBox = new System.Windows.Forms.GroupBox();
            this.opcItemDataGridViewDataGridViewprogressBar = new System.Windows.Forms.ProgressBar();
            this.opcItemDataGridView = new System.Windows.Forms.DataGridView();
            this.contextMenuStrip2 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.okCmd = new System.Windows.Forms.Button();
            this.serverNameBox = new System.Windows.Forms.GroupBox();
            this.treeOpcItems = new System.Windows.Forms.TreeView();
            this.opcServerHostBox = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.opcServerIPBox = new System.Windows.Forms.ComboBox();
            this.selectDataPointListBox = new System.Windows.Forms.ListBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.intervalNameTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.searchTextBox = new System.Windows.Forms.TextBox();
            this.searchBtn = new System.Windows.Forms.Button();
            this.cancelCmd = new System.Windows.Forms.Button();
            this.serverInfoBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.opcItemDataGridView)).BeginInit();
            this.contextMenuStrip2.SuspendLayout();
            this.serverNameBox.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // serverInfoBox
            // 
            this.serverInfoBox.BackColor = System.Drawing.Color.Transparent;
            this.serverInfoBox.Controls.Add(this.opcItemDataGridViewDataGridViewprogressBar);
            this.serverInfoBox.Controls.Add(this.opcItemDataGridView);
            this.serverInfoBox.Location = new System.Drawing.Point(385, 70);
            this.serverInfoBox.Name = "serverInfoBox";
            this.serverInfoBox.Size = new System.Drawing.Size(583, 710);
            this.serverInfoBox.TabIndex = 31;
            this.serverInfoBox.TabStop = false;
            this.serverInfoBox.Text = "DataPoint List";
            // 
            // opcItemDataGridViewDataGridViewprogressBar
            // 
            this.opcItemDataGridViewDataGridViewprogressBar.ForeColor = System.Drawing.Color.Lime;
            this.opcItemDataGridViewDataGridViewprogressBar.Location = new System.Drawing.Point(3, 409);
            this.opcItemDataGridViewDataGridViewprogressBar.Name = "opcItemDataGridViewDataGridViewprogressBar";
            this.opcItemDataGridViewDataGridViewprogressBar.Size = new System.Drawing.Size(591, 23);
            this.opcItemDataGridViewDataGridViewprogressBar.TabIndex = 104;
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
            this.opcItemDataGridView.ContextMenuStrip = this.contextMenuStrip2;
            this.opcItemDataGridView.Cursor = System.Windows.Forms.Cursors.Hand;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Info;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.opcItemDataGridView.DefaultCellStyle = dataGridViewCellStyle2;
            this.opcItemDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.opcItemDataGridView.Location = new System.Drawing.Point(3, 18);
            this.opcItemDataGridView.Margin = new System.Windows.Forms.Padding(4);
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
            this.opcItemDataGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.opcItemDataGridView.Size = new System.Drawing.Size(556, 685);
            this.opcItemDataGridView.TabIndex = 7;
            this.opcItemDataGridView.Text = "dataGridView2";
            this.opcItemDataGridView.MouseHover += new System.EventHandler(this.opcItemDataGridView_MouseHover);
            this.opcItemDataGridView.MouseEnter += new System.EventHandler(this.opcItemDataGridView_MouseEnter);
            // 
            // contextMenuStrip2
            // 
            this.contextMenuStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addToolStripMenuItem});
            this.contextMenuStrip2.Name = "contextMenuStrip1";
            this.contextMenuStrip2.Size = new System.Drawing.Size(97, 26);
            // 
            // addToolStripMenuItem
            // 
            this.addToolStripMenuItem.Name = "addToolStripMenuItem";
            this.addToolStripMenuItem.Size = new System.Drawing.Size(96, 22);
            this.addToolStripMenuItem.Text = "Add";
            // 
            // okCmd
            // 
            this.okCmd.Image = global::OPCSampleGrpConfig.Properties.Resources.Yes_v3;
            this.okCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.okCmd.Location = new System.Drawing.Point(1063, 744);
            this.okCmd.Margin = new System.Windows.Forms.Padding(4);
            this.okCmd.Name = "okCmd";
            this.okCmd.Size = new System.Drawing.Size(104, 36);
            this.okCmd.TabIndex = 35;
            this.okCmd.Text = "Save";
            this.okCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // serverNameBox
            // 
            this.serverNameBox.BackColor = System.Drawing.Color.Transparent;
            this.serverNameBox.Controls.Add(this.treeOpcItems);
            this.serverNameBox.Location = new System.Drawing.Point(12, 67);
            this.serverNameBox.Name = "serverNameBox";
            this.serverNameBox.Size = new System.Drawing.Size(367, 714);
            this.serverNameBox.TabIndex = 30;
            this.serverNameBox.TabStop = false;
            this.serverNameBox.Text = "serverNameBox";
            // 
            // treeOpcItems
            // 
            this.treeOpcItems.HideSelection = false;
            this.treeOpcItems.Location = new System.Drawing.Point(3, 18);
            this.treeOpcItems.Name = "treeOpcItems";
            this.treeOpcItems.Size = new System.Drawing.Size(364, 690);
            this.treeOpcItems.TabIndex = 19;
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
            // label8
            // 
            this.label8.BackColor = System.Drawing.Color.Transparent;
            this.label8.Location = new System.Drawing.Point(60, 9);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(125, 16);
            this.label8.TabIndex = 34;
            this.label8.Text = "OPC Server Name :";
            this.label8.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // opcServerIPBox
            // 
            this.opcServerIPBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.opcServerIPBox.FormattingEnabled = true;
            this.opcServerIPBox.Location = new System.Drawing.Point(191, 6);
            this.opcServerIPBox.Name = "opcServerIPBox";
            this.opcServerIPBox.Size = new System.Drawing.Size(331, 24);
            this.opcServerIPBox.TabIndex = 33;
            // 
            // selectDataPointListBox
            // 
            this.selectDataPointListBox.ContextMenuStrip = this.contextMenuStrip1;
            this.selectDataPointListBox.Cursor = System.Windows.Forms.Cursors.Default;
            this.selectDataPointListBox.FormattingEnabled = true;
            this.selectDataPointListBox.HorizontalScrollbar = true;
            this.selectDataPointListBox.ItemHeight = 16;
            this.selectDataPointListBox.Location = new System.Drawing.Point(6, 14);
            this.selectDataPointListBox.MaximumSize = new System.Drawing.Size(300, 665);
            this.selectDataPointListBox.Name = "selectDataPointListBox";
            this.selectDataPointListBox.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.selectDataPointListBox.Size = new System.Drawing.Size(300, 644);
            this.selectDataPointListBox.TabIndex = 37;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.removeToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(108, 26);
            // 
            // removeToolStripMenuItem
            // 
            this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            this.removeToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.removeToolStripMenuItem.Text = "Delete";
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.selectDataPointListBox);
            this.groupBox1.Location = new System.Drawing.Point(970, 71);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(309, 665);
            this.groupBox1.TabIndex = 38;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Selected DataPoint";
            // 
            // intervalNameTextBox
            // 
            this.intervalNameTextBox.Enabled = false;
            this.intervalNameTextBox.Location = new System.Drawing.Point(713, 5);
            this.intervalNameTextBox.MaxLength = 30;
            this.intervalNameTextBox.Name = "intervalNameTextBox";
            this.intervalNameTextBox.Size = new System.Drawing.Size(255, 22);
            this.intervalNameTextBox.TabIndex = 39;
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(572, 6);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(141, 16);
            this.label1.TabIndex = 40;
            this.label1.Text = "Sample Group Name :";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(32, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(159, 16);
            this.label2.TabIndex = 41;
            this.label2.Text = "Data Point Name Search:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // searchTextBox
            // 
            this.searchTextBox.Location = new System.Drawing.Point(191, 36);
            this.searchTextBox.MaxLength = 1000;
            this.searchTextBox.Name = "searchTextBox";
            this.searchTextBox.Size = new System.Drawing.Size(331, 22);
            this.searchTextBox.TabIndex = 42;
            // 
            // searchBtn
            // 
            this.searchBtn.Image = global::OPCSampleGrpConfig.Properties.Resources.Search_v2;
            this.searchBtn.Location = new System.Drawing.Point(528, 35);
            this.searchBtn.Name = "searchBtn";
            this.searchBtn.Size = new System.Drawing.Size(36, 23);
            this.searchBtn.TabIndex = 43;
            this.searchBtn.UseVisualStyleBackColor = true;
            // 
            // cancelCmd
            // 
            this.cancelCmd.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelCmd.Image = global::OPCSampleGrpConfig.Properties.Resources.Cancel;
            this.cancelCmd.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancelCmd.Location = new System.Drawing.Point(1175, 744);
            this.cancelCmd.Margin = new System.Windows.Forms.Padding(4);
            this.cancelCmd.Name = "cancelCmd";
            this.cancelCmd.Size = new System.Drawing.Size(104, 36);
            this.cancelCmd.TabIndex = 36;
            this.cancelCmd.Text = "Cancel";
            this.cancelCmd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // OPCDataSelector
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(1284, 793);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.searchBtn);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.searchTextBox);
            this.Controls.Add(this.intervalNameTextBox);
            this.Controls.Add(this.opcServerHostBox);
            this.Controls.Add(this.serverNameBox);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.cancelCmd);
            this.Controls.Add(this.serverInfoBox);
            this.Controls.Add(this.opcServerIPBox);
            this.Controls.Add(this.okCmd);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "OPCDataSelector";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Select OPC Data Point";
            this.Load += new System.EventHandler(this.OPCDataSelector_Load);
            this.serverInfoBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.opcItemDataGridView)).EndInit();
            this.contextMenuStrip2.ResumeLayout(false);
            this.serverNameBox.ResumeLayout(false);
            this.contextMenuStrip1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }



        void opcItemDataGridView_MouseHover(object sender, System.EventArgs e)
        {
            //throw new System.Exception("The method or operation is not implemented.");
            opcItemDataGridView.Focus();
            bool focus = this.opcItemDataGridView.Focused;
        }

        void opcItemDataGridView_MouseEnter(object sender, System.EventArgs e)
        {
            //throw new System.Exception("The method or operation is not implemented.");
            opcItemDataGridView.Focus();
            bool focus = this.opcItemDataGridView.Focused;
        }



        #endregion

        private System.Windows.Forms.GroupBox serverInfoBox;
        private System.Windows.Forms.DataGridView opcItemDataGridView;
        private System.Windows.Forms.GroupBox serverNameBox;
        private System.Windows.Forms.TreeView treeOpcItems;
        private System.Windows.Forms.Label opcServerHostBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox opcServerIPBox;
        internal System.Windows.Forms.Button okCmd;
        private System.Windows.Forms.Button cancelCmd;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox intervalNameTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox searchTextBox;
        private System.Windows.Forms.Button searchBtn;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip2;
        private System.Windows.Forms.ToolStripMenuItem addToolStripMenuItem;
        private System.Windows.Forms.ProgressBar opcItemDataGridViewDataGridViewprogressBar;
        private System.Windows.Forms.ListBox selectDataPointListBox;
    }
}

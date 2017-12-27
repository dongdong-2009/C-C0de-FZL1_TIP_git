using ISCSAlarmNetWrapper;
namespace VASimulator
{
    partial class FormVASimulator
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormVASimulator));
            this.btnSubmitAlarm = new System.Windows.Forms.Button();
            this.btnClearAlarm = new System.Windows.Forms.Button();
            this.txtAlarmRequestInfo = new System.Windows.Forms.TextBox();
            this.btnCleanAckAlarm = new System.Windows.Forms.Button();
            this.txtISCSStartTime = new System.Windows.Forms.TextBox();
            this.lblISCSStartTime = new System.Windows.Forms.Label();
            this.grpbxSubmitAlarm = new System.Windows.Forms.GroupBox();
            this.comboBxState = new System.Windows.Forms.ComboBox();
            this.lblState = new System.Windows.Forms.Label();
            this.txtNoOfAlarm = new System.Windows.Forms.TextBox();
            this.lblNoOfAlarms = new System.Windows.Forms.Label();
            this.txtDescription = new System.Windows.Forms.TextBox();
            this.lblDescription = new System.Windows.Forms.Label();
            this.txtLocation = new System.Windows.Forms.TextBox();
            this.lblLocation = new System.Windows.Forms.Label();
            this.comboBxMMSState = new System.Windows.Forms.ComboBox();
            this.btnDefaultAlarm = new System.Windows.Forms.Button();
            this.txtSystemKey = new System.Windows.Forms.TextBox();
            this.lblSystemKey = new System.Windows.Forms.Label();
            this.txtSubsytemKey = new System.Windows.Forms.TextBox();
            this.lblSubsytemKey = new System.Windows.Forms.Label();
            this.txtAssetName = new System.Windows.Forms.TextBox();
            this.lblAssetName = new System.Windows.Forms.Label();
            this.txtAckBy = new System.Windows.Forms.TextBox();
            this.lblAckBy = new System.Windows.Forms.Label();
            this.txtValue = new System.Windows.Forms.TextBox();
            this.lblAlarmValue = new System.Windows.Forms.Label();
            this.txtSeverity = new System.Windows.Forms.TextBox();
            this.lblAlarmSeverity = new System.Windows.Forms.Label();
            this.txtType = new System.Windows.Forms.TextBox();
            this.lblAlarmType = new System.Windows.Forms.Label();
            this.lblMmsState = new System.Windows.Forms.Label();
            this.grpBxAlarmRequest = new System.Windows.Forms.GroupBox();
            this.lblAlarmID = new System.Windows.Forms.Label();
            this.txtBxAlarmID = new System.Windows.Forms.TextBox();
            this.grpbxSubmitAlarm.SuspendLayout();
            this.grpBxAlarmRequest.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSubmitAlarm
            // 
            this.btnSubmitAlarm.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnSubmitAlarm.Location = new System.Drawing.Point(181, 560);
            this.btnSubmitAlarm.Name = "btnSubmitAlarm";
            this.btnSubmitAlarm.Size = new System.Drawing.Size(86, 38);
            this.btnSubmitAlarm.TabIndex = 0;
            this.btnSubmitAlarm.Text = "Submit Alarm";
            this.btnSubmitAlarm.UseVisualStyleBackColor = true;
            this.btnSubmitAlarm.Click += new System.EventHandler(this.btnSubmitAlarm_Click);
            // 
            // btnClearAlarm
            // 
            this.btnClearAlarm.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnClearAlarm.Location = new System.Drawing.Point(15, 12);
            this.btnClearAlarm.Name = "btnClearAlarm";
            this.btnClearAlarm.Size = new System.Drawing.Size(129, 38);
            this.btnClearAlarm.TabIndex = 1;
            this.btnClearAlarm.Text = "Set StartupTime";
            this.btnClearAlarm.UseVisualStyleBackColor = true;
            this.btnClearAlarm.Click += new System.EventHandler(this.btnClearAlarm_Click);
            // 
            // txtAlarmRequestInfo
            // 
            this.txtAlarmRequestInfo.AllowDrop = true;
            this.txtAlarmRequestInfo.Location = new System.Drawing.Point(6, 19);
            this.txtAlarmRequestInfo.Multiline = true;
            this.txtAlarmRequestInfo.Name = "txtAlarmRequestInfo";
            this.txtAlarmRequestInfo.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtAlarmRequestInfo.Size = new System.Drawing.Size(447, 550);
            this.txtAlarmRequestInfo.TabIndex = 2;
            // 
            // btnCleanAckAlarm
            // 
            this.btnCleanAckAlarm.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCleanAckAlarm.Location = new System.Drawing.Point(418, 575);
            this.btnCleanAckAlarm.Name = "btnCleanAckAlarm";
            this.btnCleanAckAlarm.Size = new System.Drawing.Size(35, 23);
            this.btnCleanAckAlarm.TabIndex = 4;
            this.btnCleanAckAlarm.Text = "cls";
            this.btnCleanAckAlarm.UseVisualStyleBackColor = true;
            this.btnCleanAckAlarm.Click += new System.EventHandler(this.btnCleanAckAlarm_Click);
            // 
            // txtISCSStartTime
            // 
            this.txtISCSStartTime.Location = new System.Drawing.Point(279, 19);
            this.txtISCSStartTime.Name = "txtISCSStartTime";
            this.txtISCSStartTime.Size = new System.Drawing.Size(468, 20);
            this.txtISCSStartTime.TabIndex = 5;
            // 
            // lblISCSStartTime
            // 
            this.lblISCSStartTime.AutoSize = true;
            this.lblISCSStartTime.Location = new System.Drawing.Point(194, 22);
            this.lblISCSStartTime.Name = "lblISCSStartTime";
            this.lblISCSStartTime.Size = new System.Drawing.Size(79, 13);
            this.lblISCSStartTime.TabIndex = 6;
            this.lblISCSStartTime.Text = "ISCS StartTime";
            // 
            // grpbxSubmitAlarm
            // 
            this.grpbxSubmitAlarm.Controls.Add(this.txtBxAlarmID);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAlarmID);
            this.grpbxSubmitAlarm.Controls.Add(this.comboBxState);
            this.grpbxSubmitAlarm.Controls.Add(this.lblState);
            this.grpbxSubmitAlarm.Controls.Add(this.txtNoOfAlarm);
            this.grpbxSubmitAlarm.Controls.Add(this.lblNoOfAlarms);
            this.grpbxSubmitAlarm.Controls.Add(this.txtDescription);
            this.grpbxSubmitAlarm.Controls.Add(this.lblDescription);
            this.grpbxSubmitAlarm.Controls.Add(this.txtLocation);
            this.grpbxSubmitAlarm.Controls.Add(this.lblLocation);
            this.grpbxSubmitAlarm.Controls.Add(this.comboBxMMSState);
            this.grpbxSubmitAlarm.Controls.Add(this.btnDefaultAlarm);
            this.grpbxSubmitAlarm.Controls.Add(this.txtSystemKey);
            this.grpbxSubmitAlarm.Controls.Add(this.lblSystemKey);
            this.grpbxSubmitAlarm.Controls.Add(this.txtSubsytemKey);
            this.grpbxSubmitAlarm.Controls.Add(this.lblSubsytemKey);
            this.grpbxSubmitAlarm.Controls.Add(this.txtAssetName);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAssetName);
            this.grpbxSubmitAlarm.Controls.Add(this.txtAckBy);
            this.grpbxSubmitAlarm.Controls.Add(this.btnSubmitAlarm);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAckBy);
            this.grpbxSubmitAlarm.Controls.Add(this.txtValue);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAlarmValue);
            this.grpbxSubmitAlarm.Controls.Add(this.txtSeverity);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAlarmSeverity);
            this.grpbxSubmitAlarm.Controls.Add(this.txtType);
            this.grpbxSubmitAlarm.Controls.Add(this.lblAlarmType);
            this.grpbxSubmitAlarm.Controls.Add(this.lblMmsState);
            this.grpbxSubmitAlarm.Location = new System.Drawing.Point(16, 65);
            this.grpbxSubmitAlarm.Name = "grpbxSubmitAlarm";
            this.grpbxSubmitAlarm.Size = new System.Drawing.Size(282, 607);
            this.grpbxSubmitAlarm.TabIndex = 7;
            this.grpbxSubmitAlarm.TabStop = false;
            this.grpbxSubmitAlarm.Text = "Alarm Details";
            // 
            // comboBxState
            // 
            this.comboBxState.DataSource = new ISCSAlarmNetWrapper.AlarmState[] {
        ISCSAlarmNetWrapper.AlarmState.AlarmClosed,
        ISCSAlarmNetWrapper.AlarmState.AlarmOpen,
        ISCSAlarmNetWrapper.AlarmState.AlarmReturned,
        ISCSAlarmNetWrapper.AlarmState.AlarmAcknowledged};
            this.comboBxState.FormattingEnabled = true;
            this.comboBxState.Location = new System.Drawing.Point(115, 237);
            this.comboBxState.Name = "comboBxState";
            this.comboBxState.Size = new System.Drawing.Size(121, 21);
            this.comboBxState.TabIndex = 26;
            // 
            // lblState
            // 
            this.lblState.AutoSize = true;
            this.lblState.Location = new System.Drawing.Point(20, 240);
            this.lblState.Name = "lblState";
            this.lblState.Size = new System.Drawing.Size(32, 13);
            this.lblState.TabIndex = 25;
            this.lblState.Text = "State";
            // 
            // txtNoOfAlarm
            // 
            this.txtNoOfAlarm.Location = new System.Drawing.Point(174, 521);
            this.txtNoOfAlarm.Name = "txtNoOfAlarm";
            this.txtNoOfAlarm.Size = new System.Drawing.Size(65, 20);
            this.txtNoOfAlarm.TabIndex = 24;
            // 
            // lblNoOfAlarms
            // 
            this.lblNoOfAlarms.AutoSize = true;
            this.lblNoOfAlarms.Location = new System.Drawing.Point(20, 521);
            this.lblNoOfAlarms.Name = "lblNoOfAlarms";
            this.lblNoOfAlarms.Size = new System.Drawing.Size(136, 13);
            this.lblNoOfAlarms.TabIndex = 23;
            this.lblNoOfAlarms.Text = "Number of Alarm To Submit";
            // 
            // txtDescription
            // 
            this.txtDescription.Location = new System.Drawing.Point(118, 361);
            this.txtDescription.Name = "txtDescription";
            this.txtDescription.Size = new System.Drawing.Size(121, 20);
            this.txtDescription.TabIndex = 21;
            // 
            // lblDescription
            // 
            this.lblDescription.AutoSize = true;
            this.lblDescription.Location = new System.Drawing.Point(20, 361);
            this.lblDescription.Name = "lblDescription";
            this.lblDescription.Size = new System.Drawing.Size(60, 13);
            this.lblDescription.TabIndex = 20;
            this.lblDescription.Text = "Description";
            // 
            // txtLocation
            // 
            this.txtLocation.Location = new System.Drawing.Point(118, 321);
            this.txtLocation.Name = "txtLocation";
            this.txtLocation.Size = new System.Drawing.Size(121, 20);
            this.txtLocation.TabIndex = 19;
            // 
            // lblLocation
            // 
            this.lblLocation.AutoSize = true;
            this.lblLocation.Location = new System.Drawing.Point(20, 321);
            this.lblLocation.Name = "lblLocation";
            this.lblLocation.Size = new System.Drawing.Size(48, 13);
            this.lblLocation.TabIndex = 18;
            this.lblLocation.Text = "Location";
            // 
            // comboBxMMSState
            // 
            this.comboBxMMSState.DataSource = new ISCSAlarmNetWrapper.MmsStateType[] {
        ISCSAlarmNetWrapper.MmsStateType.MMS_NONE,
        ISCSAlarmNetWrapper.MmsStateType.MMS_SEMI,
        ISCSAlarmNetWrapper.MmsStateType.MMS_AUTO,
        ISCSAlarmNetWrapper.MmsStateType.MMS_JR_SENT,
        ISCSAlarmNetWrapper.MmsStateType.MMS_INHIBIT};
            this.comboBxMMSState.FormattingEnabled = true;
            this.comboBxMMSState.Location = new System.Drawing.Point(115, 40);
            this.comboBxMMSState.Name = "comboBxMMSState";
            this.comboBxMMSState.Size = new System.Drawing.Size(121, 21);
            this.comboBxMMSState.TabIndex = 17;
            // 
            // btnDefaultAlarm
            // 
            this.btnDefaultAlarm.Location = new System.Drawing.Point(15, 560);
            this.btnDefaultAlarm.Name = "btnDefaultAlarm";
            this.btnDefaultAlarm.Size = new System.Drawing.Size(84, 38);
            this.btnDefaultAlarm.TabIndex = 16;
            this.btnDefaultAlarm.Text = "Fill Default Alarm";
            this.btnDefaultAlarm.UseVisualStyleBackColor = true;
            this.btnDefaultAlarm.Click += new System.EventHandler(this.btnDefaultAlarm_Click);
            // 
            // txtSystemKey
            // 
            this.txtSystemKey.Location = new System.Drawing.Point(118, 441);
            this.txtSystemKey.Name = "txtSystemKey";
            this.txtSystemKey.Size = new System.Drawing.Size(121, 20);
            this.txtSystemKey.TabIndex = 15;
            // 
            // lblSystemKey
            // 
            this.lblSystemKey.AutoSize = true;
            this.lblSystemKey.Location = new System.Drawing.Point(20, 441);
            this.lblSystemKey.Name = "lblSystemKey";
            this.lblSystemKey.Size = new System.Drawing.Size(62, 13);
            this.lblSystemKey.TabIndex = 14;
            this.lblSystemKey.Text = "System Key";
            // 
            // txtSubsytemKey
            // 
            this.txtSubsytemKey.Location = new System.Drawing.Point(118, 401);
            this.txtSubsytemKey.Name = "txtSubsytemKey";
            this.txtSubsytemKey.Size = new System.Drawing.Size(121, 20);
            this.txtSubsytemKey.TabIndex = 13;
            // 
            // lblSubsytemKey
            // 
            this.lblSubsytemKey.AutoSize = true;
            this.lblSubsytemKey.Location = new System.Drawing.Point(20, 401);
            this.lblSubsytemKey.Name = "lblSubsytemKey";
            this.lblSubsytemKey.Size = new System.Drawing.Size(79, 13);
            this.lblSubsytemKey.TabIndex = 12;
            this.lblSubsytemKey.Text = "Subsystem Key";
            // 
            // txtAssetName
            // 
            this.txtAssetName.Location = new System.Drawing.Point(118, 281);
            this.txtAssetName.Name = "txtAssetName";
            this.txtAssetName.Size = new System.Drawing.Size(121, 20);
            this.txtAssetName.TabIndex = 11;
            // 
            // lblAssetName
            // 
            this.lblAssetName.AutoSize = true;
            this.lblAssetName.Location = new System.Drawing.Point(20, 281);
            this.lblAssetName.Name = "lblAssetName";
            this.lblAssetName.Size = new System.Drawing.Size(64, 13);
            this.lblAssetName.TabIndex = 10;
            this.lblAssetName.Text = "Asset Name";
            // 
            // txtAckBy
            // 
            this.txtAckBy.Location = new System.Drawing.Point(115, 200);
            this.txtAckBy.Name = "txtAckBy";
            this.txtAckBy.Size = new System.Drawing.Size(121, 20);
            this.txtAckBy.TabIndex = 9;
            // 
            // lblAckBy
            // 
            this.lblAckBy.AutoSize = true;
            this.lblAckBy.Location = new System.Drawing.Point(17, 200);
            this.lblAckBy.Name = "lblAckBy";
            this.lblAckBy.Size = new System.Drawing.Size(87, 13);
            this.lblAckBy.TabIndex = 8;
            this.lblAckBy.Text = "Acknowledge By";
            // 
            // txtValue
            // 
            this.txtValue.Location = new System.Drawing.Point(115, 160);
            this.txtValue.Name = "txtValue";
            this.txtValue.Size = new System.Drawing.Size(121, 20);
            this.txtValue.TabIndex = 7;
            // 
            // lblAlarmValue
            // 
            this.lblAlarmValue.AutoSize = true;
            this.lblAlarmValue.Location = new System.Drawing.Point(17, 160);
            this.lblAlarmValue.Name = "lblAlarmValue";
            this.lblAlarmValue.Size = new System.Drawing.Size(34, 13);
            this.lblAlarmValue.TabIndex = 6;
            this.lblAlarmValue.Text = "Value";
            // 
            // txtSeverity
            // 
            this.txtSeverity.Location = new System.Drawing.Point(115, 120);
            this.txtSeverity.Name = "txtSeverity";
            this.txtSeverity.Size = new System.Drawing.Size(121, 20);
            this.txtSeverity.TabIndex = 5;
            // 
            // lblAlarmSeverity
            // 
            this.lblAlarmSeverity.AutoSize = true;
            this.lblAlarmSeverity.Location = new System.Drawing.Point(17, 120);
            this.lblAlarmSeverity.Name = "lblAlarmSeverity";
            this.lblAlarmSeverity.Size = new System.Drawing.Size(45, 13);
            this.lblAlarmSeverity.TabIndex = 4;
            this.lblAlarmSeverity.Text = "Severity";
            // 
            // txtType
            // 
            this.txtType.Location = new System.Drawing.Point(115, 80);
            this.txtType.Name = "txtType";
            this.txtType.Size = new System.Drawing.Size(121, 20);
            this.txtType.TabIndex = 3;
            // 
            // lblAlarmType
            // 
            this.lblAlarmType.AutoSize = true;
            this.lblAlarmType.Location = new System.Drawing.Point(17, 80);
            this.lblAlarmType.Name = "lblAlarmType";
            this.lblAlarmType.Size = new System.Drawing.Size(31, 13);
            this.lblAlarmType.TabIndex = 2;
            this.lblAlarmType.Text = "Type";
            // 
            // lblMmsState
            // 
            this.lblMmsState.AutoSize = true;
            this.lblMmsState.Location = new System.Drawing.Point(17, 40);
            this.lblMmsState.Name = "lblMmsState";
            this.lblMmsState.Size = new System.Drawing.Size(60, 13);
            this.lblMmsState.TabIndex = 0;
            this.lblMmsState.Text = "MMS State";
            // 
            // grpBxAlarmRequest
            // 
            this.grpBxAlarmRequest.Controls.Add(this.txtAlarmRequestInfo);
            this.grpBxAlarmRequest.Controls.Add(this.btnCleanAckAlarm);
            this.grpBxAlarmRequest.Location = new System.Drawing.Point(304, 65);
            this.grpBxAlarmRequest.Name = "grpBxAlarmRequest";
            this.grpBxAlarmRequest.Size = new System.Drawing.Size(459, 607);
            this.grpBxAlarmRequest.TabIndex = 8;
            this.grpBxAlarmRequest.TabStop = false;
            this.grpBxAlarmRequest.Text = "Alarm Request";
            // 
            // lblAlarmID
            // 
            this.lblAlarmID.AutoSize = true;
            this.lblAlarmID.Location = new System.Drawing.Point(20, 481);
            this.lblAlarmID.Name = "lblAlarmID";
            this.lblAlarmID.Size = new System.Drawing.Size(47, 13);
            this.lblAlarmID.TabIndex = 27;
            this.lblAlarmID.Text = "Alarm ID";
            // 
            // txtBxAlarmID
            // 
            this.txtBxAlarmID.Location = new System.Drawing.Point(118, 481);
            this.txtBxAlarmID.Name = "txtBxAlarmID";
            this.txtBxAlarmID.Size = new System.Drawing.Size(121, 20);
            this.txtBxAlarmID.TabIndex = 28;
            // 
            // FormVASimulator
            // 
            this.AcceptButton = this.btnCleanAckAlarm;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(775, 677);
            this.Controls.Add(this.grpbxSubmitAlarm);
            this.Controls.Add(this.lblISCSStartTime);
            this.Controls.Add(this.txtISCSStartTime);
            this.Controls.Add(this.btnClearAlarm);
            this.Controls.Add(this.grpBxAlarmRequest);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "FormVASimulator";
            this.Text = "VA Simulator";
            this.Load += new System.EventHandler(this.FormVASimulator_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormVASimulator_FormClosing);
            this.grpbxSubmitAlarm.ResumeLayout(false);
            this.grpbxSubmitAlarm.PerformLayout();
            this.grpBxAlarmRequest.ResumeLayout(false);
            this.grpBxAlarmRequest.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnSubmitAlarm;
        private System.Windows.Forms.Button btnClearAlarm;
        private System.Windows.Forms.TextBox txtAlarmRequestInfo;
        private System.Windows.Forms.Button btnCleanAckAlarm;
        private System.Windows.Forms.TextBox txtISCSStartTime;
        private System.Windows.Forms.Label lblISCSStartTime;
        private System.Windows.Forms.GroupBox grpbxSubmitAlarm;
        private System.Windows.Forms.TextBox txtSystemKey;
        private System.Windows.Forms.Label lblSystemKey;
        private System.Windows.Forms.TextBox txtSubsytemKey;
        private System.Windows.Forms.Label lblSubsytemKey;
        private System.Windows.Forms.TextBox txtAssetName;
        private System.Windows.Forms.Label lblAssetName;
        private System.Windows.Forms.TextBox txtAckBy;
        private System.Windows.Forms.Label lblAckBy;
        private System.Windows.Forms.TextBox txtValue;
        private System.Windows.Forms.Label lblAlarmValue;
        private System.Windows.Forms.TextBox txtSeverity;
        private System.Windows.Forms.Label lblAlarmSeverity;
        private System.Windows.Forms.TextBox txtType;
        private System.Windows.Forms.Label lblAlarmType;
        private System.Windows.Forms.Label lblMmsState;
        private System.Windows.Forms.GroupBox grpBxAlarmRequest;
        private System.Windows.Forms.Button btnDefaultAlarm;
        private System.Windows.Forms.ComboBox comboBxMMSState;
        private System.Windows.Forms.TextBox txtLocation;
        private System.Windows.Forms.Label lblLocation;
        private System.Windows.Forms.TextBox txtDescription;
        private System.Windows.Forms.Label lblDescription;
        private System.Windows.Forms.Label lblNoOfAlarms;
        private System.Windows.Forms.TextBox txtNoOfAlarm;
        private System.Windows.Forms.Label lblState;
        private System.Windows.Forms.ComboBox comboBxState;
        private System.Windows.Forms.TextBox txtBxAlarmID;
        private System.Windows.Forms.Label lblAlarmID;
    }
}


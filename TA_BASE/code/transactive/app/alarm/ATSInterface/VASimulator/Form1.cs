using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

using ISCSAlarmNetWrapper;
namespace VASimulator
{
    public partial class FormVASimulator : Form
    {
        long AlarmID = 1;
        const uint VASubsystemID = 10001;   //VA subsystem ID
        IExternalAlarmWrapper _AlarmWrapper;
        Thread _retrieveAckAlarmThread; //background thread to retireve alarm request and ISCS Start time
        volatile bool _stopRetrieveAckAlarmThread = true; 

        public FormVASimulator()
        {
            InitializeComponent();

            //get NET wrapper from factory
            _AlarmWrapper = AlarmWrapperFactory.getExternalAlarmWrapper(VASubsystemID);
            //update VA startTime;
            UpdateVAStartTime();
           

            //Start new thread to update Alarm Request Info and ISCS start time
            _retrieveAckAlarmThread = new Thread(new ThreadStart(this.RetrieveAlarmAck));
            _stopRetrieveAckAlarmThread = false;
            _retrieveAckAlarmThread.Start();
        }

        /// <summary>
        /// Retrieves alarm request and updates Request Info text box
        /// </summary>
        private void RetrieveAlarmAck()
        {
            while (!_stopRetrieveAckAlarmThread)
            {
                List<AlarmRequest> AlarmRequestList = new List<AlarmRequest>();
                DateTime startTime = DateTime.Now;
                string ISCSStartTime = "";
                //Get ISCS startTime
                if (_AlarmWrapper.getISCSStartupTime(ref startTime))
                {
                    ISCSStartTime = startTime.ToString();
                }
                updateStartTimeInfo(ISCSStartTime);
                //IMPORT: if newISCSStartTime !=OldISCSStartTime, need resend all VA alarms;

                //Get alarm request
                ulong noOfAck = (ulong)(_AlarmWrapper.retrieveAlarmAckCloseUpdate(AlarmRequestList));
                System.Threading.Thread.Sleep(200);

                foreach (AlarmRequest _alarmReq in AlarmRequestList)
                {
                    updateRequestInfo("Alarm ID = " + _alarmReq.alarmId + ", Operator ID = " + _alarmReq.operatorID
                        + ", Request Time = " + _alarmReq.requestTime.ToString("yyyy/MM/dd-HH:mm:ss") + ", State = " + _alarmReq.alarmStatue + System.Environment.NewLine);
                }

            }

        }

        /// <summary>
        /// To append alarm request info to the Request text box
        /// </summary>
        /// <param name="reqeustInfo"></param>
        private void updateRequestInfo(String reqeustInfo)
        {
            if (this.txtAlarmRequestInfo.InvokeRequired)
            {
                ModifyTextCallback d = new ModifyTextCallback(AppendRequestInfo);
                this.Invoke(d, new object[] { reqeustInfo });
            }
            else
            {
                this.txtAlarmRequestInfo.AppendText(reqeustInfo);
            }
        }

        /// <summary>
        /// To Set the ISCS start time
        /// </summary>
        /// <param name="startTime"></param>
        private void updateStartTimeInfo(String startTime)
        {
            if (this.txtAlarmRequestInfo.InvokeRequired)
            {
                ModifyTextCallback d = new ModifyTextCallback(SetStartTimeInfo);
                this.Invoke(d, new object[] { startTime });
            }
            else
            {
                this.txtISCSStartTime.Text = startTime;
            }
        }

        /// <summary>
        /// Append Request Info text box
        /// </summary>
        /// <param name="requestInfo"></param>
        private void AppendRequestInfo(string requestInfo)
        {
            this.txtAlarmRequestInfo.AppendText(requestInfo);
        }

        /// <summary>
        /// Set value to Start time text box
        /// </summary>
        /// <param name="startTime"></param>
        private void SetStartTimeInfo(string startTime)
        {
            this.txtISCSStartTime.Text = startTime;
        }

        /// <summary>
        /// Submits alarm to ISCS
        /// </summary>
        private void SubmitAlarm()
        {
            
            List<AlarmDetail> submitAlarmDetailList = new List<AlarmDetail>();
            
            //get the number of alarms to be submitted
            int NoOfAlarm;
            bool validInt = int.TryParse(txtNoOfAlarm.Text, out NoOfAlarm);
            if (!validInt)
                NoOfAlarm = VASimualtorConst.DefaultNoOfAlarmToSubmit;

            for (int i = 0; i < NoOfAlarm; i++)
            {
                AlarmDetail _alarmDetail = new AlarmDetail();

                //Set AlarmID if empty
                string _alarmID;
                if (txtBxAlarmID.Text.Trim() == "")
                    _alarmID = AlarmID++.ToString();
                else
                    _alarmID = txtBxAlarmID.Text;

                _alarmDetail.alarmID = _alarmID;
                _alarmDetail.alarmType = txtType.Text;
                if (txtSeverity.Text.Trim() == "")
                    txtSeverity.Text = VASimualtorConst.DefaultAlarmSeverity;
                _alarmDetail.alarmSeverity = Convert.ToUInt32(txtSeverity.Text, 16);
                _alarmDetail.alarmValue = txtValue.Text;
                _alarmDetail.sourceTime = DateTime.Now;
                _alarmDetail.alarmDescription = txtDescription.Text;
                _alarmDetail.alarmAcknowledgeBy = txtAckBy.Text;
                _alarmDetail.assetName = txtAssetName.Text;
                _alarmDetail.locationId = txtLocation.Text;
                _alarmDetail.systemkey = txtSystemKey.Text;
                _alarmDetail.subsytemkey = txtSubsytemKey.Text;
                _alarmDetail.state = (AlarmState)comboBxState.SelectedIndex;
                _alarmDetail.mmsState = (MmsStateType)comboBxMMSState.SelectedIndex;
                submitAlarmDetailList.Add(_alarmDetail);
            }

            try
            {
                _AlarmWrapper.submitAlarmList(submitAlarmDetailList);

            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception: " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void FillDefaultAlarmDetail()
        {
            comboBxMMSState.SelectedItem = MmsStateType.MMS_NONE;
            comboBxState.SelectedItem = AlarmState.AlarmOpen;
            txtValue.Text = VASimualtorConst.DefaultAlarmValue;
            txtSubsytemKey.Text = VASimualtorConst.DefaultSubsystemKey;
            txtSystemKey.Text = VASimualtorConst.DefaultSystemKey;
            txtSeverity.Text = VASimualtorConst.DefaultAlarmSeverity;
            txtType.Text = VASimualtorConst.DefaultAlarmType;
            txtAssetName.Text = VASimualtorConst.DefaultAlarmAssetName;
            txtDescription.Text = VASimualtorConst.DefaultAlarmDescription;
            txtLocation.Text = VASimualtorConst.DefaulLocationId;
            txtNoOfAlarm.Text = VASimualtorConst.DefaultNoOfAlarmToSubmit.ToString();
            
        }


        /// <summary>
        /// Set Start time
        /// </summary>
        private void UpdateVAStartTime()
        {
            DateTime TimeNow = DateTime.Now;
            _AlarmWrapper.setExternalStartupTime(TimeNow);
        }


        private void btnSubmitAlarm_Click(object sender, EventArgs e)
        {
            SubmitAlarm();
        }

        private void btnClearAlarm_Click(object sender, EventArgs e)
        {
            UpdateVAStartTime();

        }

        private void btnCleanAckAlarm_Click(object sender, EventArgs e)
        {
            txtAlarmRequestInfo.Clear();
        }


        private void FormVASimulator_FormClosing(object sender, FormClosingEventArgs e)
        {
            //stop background thread
            _stopRetrieveAckAlarmThread = true;
            _retrieveAckAlarmThread.Abort();
            //release Alarm wrapper
            AlarmWrapperFactory.releaseExternalWrapper(VASubsystemID);
        }

        private void btnDefaultAlarm_Click(object sender, EventArgs e)
        {
            FillDefaultAlarmDetail();
        }

        private void FormVASimulator_Load(object sender, EventArgs e)
        {
            FillDefaultAlarmDetail();
        }


    }

    /// <summary>
    /// Delegate function used to Modify text boxes value
    /// </summary>
    /// <param name="text"></param>
    delegate void ModifyTextCallback(string text);

}


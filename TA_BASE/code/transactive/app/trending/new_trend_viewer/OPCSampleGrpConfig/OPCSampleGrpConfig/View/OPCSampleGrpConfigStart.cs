using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows.Forms;


using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using STEE.ISCS.MVC;
using STEE.ISCS.MulLanguage;
using Entity.Trending;
using DAO.Trending.Helper;
using TrendingHelper;
using OPCSampleGrpConfig.Common;
using OPCSampleGrpConfig.Controller;



namespace OPCSampleGrpConfig.View
{
    /// <summary>
    /// This class is reponsible for display OPCSampleGroupConfig form and 
    /// coordinates between user and its controller.
    /// </summary>
    public partial class OPCSampleGrpConfigStart : BaseForm
    {
        #region Declaration
        private const string CLASS_NAME = "OPCSampleGrpConfig.OPCSampleGrpConfigStart";
        //current selected sample group for editing
        private string m_editIntervalGrpID = "";
       // private string m_localConnectionString = "";

        //current Grid page upper and lower index
        private int m_lowerIndex = 0;
        private int m_upperIndex = 0;

        private bool m_reload = false;

        //Constants
        private const int RECORDSIZEINDEX = 8;
        public const string OPCSAMPLEGRPCONFIGSTARTFRM = "OPCSampleGrpConfigStart";
        public const string OPCSAMPLEGRP_SORT_ASC = "ASCENDING";
        public const string OPCSAMPLEGRP_SORT_DESC = "DESCENDING";

        public const string OPCSAMPLEGRPID_COL_NAME = "PKEY";
        public const string OPCSAMPLEGRPNAME_COL_NAME = "NAME";
        public const string OPCSAMPLEGRPDESC_COL_NAME = "DESCRIPTION";
        public const string OPCSAMPLEINTERVAL_COL_NAME = "SAMPLE_INTERVAL";
        public const string OPCSAMPLEINTERVALTYPE_COL_NAME = "INTERVAL_TYPE";
        public const string OPCSAMPLESTARTTIME_COL_NAME = "START_TIME";
        public const string OPCSAMPLEDELTAVAL_COL_NAME = "DELTA_VALUE";
        public const string OPCSAMPLEDISABLE_COL_NAME = "DISABLE";
        public const string OPCSAMPLELOCATION_COL_NAME = "LOCATION_KEY";


        private string OPCSAMPLEGRPID_COL_TEXT = "Group ID";       

        public const int OPCSAMPLEGRP_IDCOL_INDEX = 0;
        public const int OPCSAMPLEGRP_NAMECOL_INDEX = 1;
        public const int OPCSAMPLEGRP_DESCCOL_INDEX = 2;
        public const int OPCSAMPLEGRP_INTERVALCOL_INDEX = 3;
        public const int OPCSAMPLEGRP_INTERVALTYPECOL_INDEX = 4;
        public const int OPCSAMPLEGRP_STARTTIMECOL_INDEX = 5;
        public const int OPCSAMPLEGRP_DELTAVALCOL_INDEX = 6;

        public const int OPCSAMPLEGRP_LOCATIONNAME_INDEX = 7;


        public const int OPCSAMPLEGRP_DISABLECOL_INDEX = 8;
        public const int OPCSAMPLEGRP_EDITCOLUMN_INDEX = 9;
        public const  int OPCSAMPLEGRP_DELETECOLUMN_INDEX = 10;
        public const int OPCSAMPLEGRP_CONFIGCOLUMN_INDEX = 11;


        //interval const
        public const int INTERVALETYPE_HOUR_INDEX = 0;
        public const int INTERVALETYPE_MINUTE_INDEX = 1;
        public const int INTERVALETYPE_SECOND_INDEX = 2;
        

        private int m_totalGrpCount = 0;
        public int TotalNumberGrp
        {
            set
            {
                m_totalGrpCount = value;
            }
        }

        private delegate void VoidEventHandler();
        private event VoidEventHandler RefreshPageNumber;
        private delegate List<EtyDataLogDPGroupTrend> GridEventHandler(string sortCol, string sortingOrder, bool reload);
        private event GridEventHandler RefreshGridData;

        #endregion

        public OPCSampleGrpConfigStart()
        {
            string Function_Name = "OPCSampleGrpConfigStart";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            System.Diagnostics.Process[] ps = System.Diagnostics.Process.
                GetProcessesByName(System.Diagnostics.Process.GetCurrentProcess().ProcessName);
            if (ps.Length > 1)
            {
                Process thisprocess = Process.GetCurrentProcess();
                thisprocess.Kill();
            }     
            InitializeComponent();

            //m_localConnectionString = ConfigureFileHelper.GetInstance().ConnectionString;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");


        }


        /// <summary>
        /// Transalate the Control caption to chinese language.
        /// </summary>
        public override void TranslateCaption()
        {
            string Function_Name = "TranslateCaption";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            InitializeControlsCaptions();
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");           
        }
        
        /// <summary>
        /// Method through which Form commuicates with its controller.
        /// </summary>
        /// <param name="ctl">Form controller</param>
        public override void  AttachListener(IController ctl)
        {
            string Function_Name = "AttachListener";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            OPCSampleGrpConfigStartController controller = (OPCSampleGrpConfigStartController)ctl;
            //check the Database Connection
            bool bConnectionOpened = controller.GetDatabaseConnection();
            if (!bConnectionOpened)
            {
                LogHelper.Error("No Connection to Database. Closing the Form.");
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_NODATABASECONN_MSG, EnglishString.OPC_MSG_NODB));
                this.Close();
            }

            //set Current location
            LocationKeyHelper.GetInstance().init();
            this.LocationKeyTextBox.Text = LocationKeyHelper.GetInstance().LocationKey.ToString();
            this.saveMsgCmd.Click +=  controller.SaveCommand;
            this.intervalConfigDataGridView.CellDoubleClick += controller.IntervalConfigDataGridView_CellDoubleClick;
            this.RefreshGridData += controller.RetriveAllOPCGrp;
            this.RefreshPageNumber += controller.RetriveTotalCount;
            this.Paint += controller.AddLoggerInterval_Paint_1;
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");           
        } 

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AddLoggerInterval_Load(object sender, EventArgs e)
        {
            string Function_Name = "AddLoggerInterval_Load";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            try
            {
                dateTimePicker.Checked = false;
                   //InitializeControlsCaptions();
                intervalTypeBox.SelectedIndex = INTERVALETYPE_MINUTE_INDEX;

                InitializeIntervalConfigDataGridView();
                PopulateIntervalConfigDataGridView(1);

            }
            catch (Exception localException)
            {
                LogHelper.Error(localException.ToString());
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");                 
        }

        /// <summary>
        /// Initializes all control captions of the form based on current language.
        /// </summary>
        private void InitializeControlsCaptions()
        {
            this.label1.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB1, this.label1.Text);
            this.label2.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB2, this.label2.Text);
            this.label3.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB3, this.label3.Text);
            this.label4.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB4, this.label4.Text);
            this.label6.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB6, this.label6.Text);
            this.label7.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB7, this.label7.Text);  
            this.disableCheckBox.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_DISABLE_CHECKBOX, this.disableCheckBox.Text); 
            this.cancelMsgCmd.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_CANCEL_BUTTON, this.cancelMsgCmd.Text); 
            this.saveMsgCmd.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_SAVE_BUTTON, this.saveMsgCmd.Text); 
            this.TotalPage_Tlbl.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_TOTALPGLBL, this.TotalPage_Tlbl.Text);
            this.Text = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_TITLE, this.Text);

            intervalTypeBox.Items.Clear();
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_HOUR_INTERVALTYPE, EnglishString.HOUR));
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_MINUTE_INTERVALTYPE, EnglishString.MINITE));
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_SECOND_INTERVALTYPE, EnglishString.SECOND));

            
        }

        /// <summary>
        /// Intializes the Sample Group Grid control columns.
        /// </summary>
        private void InitializeIntervalConfigDataGridView()
        {
            string Function_Name = "InitializeIntervalConfigDataGridView";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            intervalConfigDataGridView.Columns.Add(OPCSAMPLEGRPID_COL_NAME,  OPCSAMPLEGRPID_COL_TEXT);
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_IDCOL_INDEX].Width = 50;
            intervalConfigDataGridView.Columns.Add(OPCSAMPLEGRPNAME_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPNAME_COL_TEXT, EnglishString.OPCSAMPLEGRPNAME_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_NAMECOL_INDEX].Width = 250;
            intervalConfigDataGridView.Columns.Add(OPCSAMPLEGRPDESC_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRPDESC_COL_TEXT, EnglishString.OPCSAMPLEGRPDESC_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_DESCCOL_INDEX].Width = 300;
            intervalConfigDataGridView.Columns.Add(OPCSAMPLEINTERVAL_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEINTERVAL_COL_TEXT, EnglishString.OPCSAMPLEINTERVAL_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_INTERVALCOL_INDEX].Width = 60; 
            intervalConfigDataGridView.Columns.Add(OPCSAMPLEINTERVALTYPE_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEINTERVALTYPE_COL_TEXT, EnglishString.OPCSAMPLEINTERVALTYPE_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_INTERVALTYPECOL_INDEX].Width = 95;
            intervalConfigDataGridView.Columns.Add(OPCSAMPLESTARTTIME_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLESTARTTIME_COL_TEXT, EnglishString.OPCSAMPLESTARTTIME_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_STARTTIMECOL_INDEX].Width = 80;
            this.intervalConfigDataGridView.Columns.Add(OPCSAMPLEDELTAVAL_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEDELTAVAL_COL_TEXT, EnglishString.OPCSAMPLEDELTAVAL_COL_TEXT));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_DELTAVALCOL_INDEX].Width = 92;



            this.intervalConfigDataGridView.Columns.Add(OPCSAMPLELOCATION_COL_NAME, StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_LOCATIONCOL_TEXT, EnglishString.OPCSAMPLEGRP_LOCATIONCOL_NAME));
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_LOCATIONNAME_INDEX].Width = 80;
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_LOCATIONNAME_INDEX].Visible = false;
            if (LocationKeyHelper.GetInstance().IsOCC)
            {
                this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_LOCATIONNAME_INDEX].Visible = true;
                this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_DESCCOL_INDEX].Width = 220;
            }

            
            DataGridViewImageColumn column4 = new DataGridViewImageColumn();
            column4.Image = Properties.Resources.blank;
            column4.HeaderText = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEDISABLE_COL_TEXT, OPCSAMPLEDISABLE_COL_NAME)  ;
            this.intervalConfigDataGridView.Columns.Add(column4);
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_DISABLECOL_INDEX].Width = 70;


            DataGridViewImageColumn iconColumn1 = new DataGridViewImageColumn();
            iconColumn1.Image = Properties.Resources.Notes_16_;
            iconColumn1.Name = EnglishString.OPCSAMPLEGRP_EDITCOLUMN_NAME;
            iconColumn1.HeaderText = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_EDITCOLUMN_TEXT, EnglishString.OPCSAMPLEGRP_EDITCOLUMN_NAME)  ;
            intervalConfigDataGridView.Columns.Add(iconColumn1);
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_EDITCOLUMN_INDEX].Width = 50;

            DataGridViewImageColumn iconColumn2 = new DataGridViewImageColumn();
            iconColumn2.Image = Properties.Resources.Erase_16_;
            iconColumn2.Name = EnglishString.OPCSAMPLEGRP_DELETECOL_NAME;
            iconColumn2.HeaderText = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_DELETECOL_TEXT, EnglishString.OPCSAMPLEGRP_DELETECOL_NAME)  ;
            intervalConfigDataGridView.Columns.Add(iconColumn2);
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_DELETECOLUMN_INDEX].Width = 50;

            DataGridViewImageColumn iconColumn3 = new DataGridViewImageColumn();
            iconColumn3.Image = Properties.Resources.nav_icon_insertkey;
            iconColumn3.Name = EnglishString.OPCSAMPLEGRP_CONFIGCOL_NAME;
            iconColumn3.HeaderText = StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLEGRP_CONFIGCOL_TEXT, EnglishString.OPCSAMPLEGRP_CONFIGCOL_NAME)  ;
            intervalConfigDataGridView.Columns.Add(iconColumn3);
            this.intervalConfigDataGridView.Columns[OPCSAMPLEGRP_CONFIGCOLUMN_INDEX].Width = 50;

            intervalConfigDataGridView.Columns[OPCSAMPLEGRPID_COL_NAME].Visible = false;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");   

        }

        /// <summary>
        /// Populates the Sample Group details in Grid control
        /// </summary>
        /// <param name="mode"></param>       
        public void PopulateIntervalConfigDataGridView(int mode)
        {
            string Function_Name = "PopulateIntervalConfigDataGridView";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            
            totalIntervalConfigDataGridView.Text = "1";

            if (mode == 1)
            {
                pageNumIntervalConfigDataGridView.Value = 1;
            }
            
            intervalConfigDataGridView.Rows.Clear();
            
            RefreshPageNumber();
            LoadPageNumber();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");  
        }                    

        /// <summary>
        /// Refresh the Page number related control based on the current total page count.
        /// </summary>
        private void LoadPageNumber()
        {
            string Function_Name = "LoadPageNumber";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            int totalRecord = m_totalGrpCount;
            m_upperIndex = 0;
            m_lowerIndex = 0;
            string sortCol = OPCSAMPLEGRPNAME_COL_NAME;
            string sortingOrder = OPCSAMPLEGRP_SORT_DESC;
            if (totalRecord > 0)
            {
                if (totalRecord % RECORDSIZEINDEX > 0)
                {
                    totalRecord = totalRecord / RECORDSIZEINDEX;
                    totalRecord++;
                }
                else
                {
                    totalRecord = totalRecord / RECORDSIZEINDEX;
                }
                // totalunAckIESDataGridView.Text = totalRecord.ToString();
                totalIntervalConfigDataGridView.Text = totalRecord.ToString();
                pageNumIntervalConfigDataGridView.Maximum = totalRecord;
            }
            else
            {
                pageNumIntervalConfigDataGridView.Maximum = 1;
            }

            m_upperIndex = (int)(RECORDSIZEINDEX * pageNumIntervalConfigDataGridView.Value);
            /*if (pageNumIntervalConfigDataGridView.Value > 1)
                m_lowerIndex = (int)(RECORDSIZEINDEX * (pageNumIntervalConfigDataGridView.Value - 1)) + 1;
            else*/
                m_lowerIndex = (int)(RECORDSIZEINDEX * (pageNumIntervalConfigDataGridView.Value - 1));
            if (intervalConfigDataGridView.SortedColumn != null && intervalConfigDataGridView.SortedColumn.Name.Trim() != "")
            {
                 sortCol =  intervalConfigDataGridView.SortedColumn.Name;
                 sortingOrder = intervalConfigDataGridView.SortOrder.ToString().ToUpper();                
            }

            List<EtyDataLogDPGroupTrend> sampleGrpList = RefreshGridData(sortCol, sortingOrder, m_reload);
            
            m_reload = false;

            LoadGridData(sampleGrpList);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");  

        }

        /// <summary>
        /// Refresh the DataGrid control based on the current sample Group list and page number.
        /// </summary>
        /// <param name="OPCSampleGrpList"></param>
        private void LoadGridData(List<EtyDataLogDPGroupTrend> OPCSampleGrpList)
        {
            intervalConfigDataGridView.Rows.Clear();

            string Function_Name = "LoadGridData";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            
            DataGridViewRow dataGridRow = null;
            int counter = 0;

            if (m_upperIndex > OPCSampleGrpList.Count)
                m_upperIndex = OPCSampleGrpList.Count;

            for(int i = m_lowerIndex;i<m_upperIndex;i++)
            {
                EtyDataLogDPGroupTrend etyDPGrpTrend = OPCSampleGrpList[i];
                if (etyDPGrpTrend != null)
                {
                    intervalConfigDataGridView.Rows.Add();
                    dataGridRow = intervalConfigDataGridView.Rows[counter];
                    counter++;
                    dataGridRow.Cells[OPCSAMPLEGRP_IDCOL_INDEX].Value = etyDPGrpTrend.SampleGrpID;
                    dataGridRow.Cells[OPCSAMPLEGRP_NAMECOL_INDEX].Value = etyDPGrpTrend.SampleGrpName;
                    dataGridRow.Cells[OPCSAMPLEGRP_DESCCOL_INDEX].Value = etyDPGrpTrend.SampleGrpDescription;
                    dataGridRow.Cells[OPCSAMPLEGRP_INTERVALCOL_INDEX].Value = etyDPGrpTrend.Interval;
                    dataGridRow.Cells[OPCSAMPLEGRP_INTERVALTYPECOL_INDEX].Value = GetIntervalTypeString(etyDPGrpTrend.IntervalType);
                    dataGridRow.Cells[OPCSAMPLEGRP_STARTTIMECOL_INDEX].Value = etyDPGrpTrend.StartTime;
                    dataGridRow.Cells[OPCSAMPLEGRP_DELTAVALCOL_INDEX].Value = etyDPGrpTrend.DeltaValue;
                    dataGridRow.Cells[OPCSAMPLEGRP_LOCATIONNAME_INDEX].Value = etyDPGrpTrend.LocationName;
                    if (etyDPGrpTrend.Disabled)
                    {
                        dataGridRow.Cells[OPCSAMPLEGRP_DISABLECOL_INDEX].Value = Properties.Resources.img_checkmark;
                    }
                    else
                    {
                        dataGridRow.Cells[OPCSAMPLEGRP_DISABLECOL_INDEX].Value = Properties.Resources.blank;
                    }
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
        }

        /// <summary>
        /// Returns the Interval type caption based on its index.
        /// </summary>
        /// <param name="intervaltypeID">interval type index</param>
        /// <returns></returns>
        private string GetIntervalTypeString(string intervaltypeID)
        {
            string result = (string)intervalTypeBox.Items[INTERVALETYPE_MINUTE_INDEX];
            if (intervaltypeID == DAOHelper.MINITE)
                result = (string)intervalTypeBox.Items[INTERVALETYPE_MINUTE_INDEX];
            else if (intervaltypeID == DAOHelper.HOUR)
                result = (string)intervalTypeBox.Items[INTERVALETYPE_HOUR_INDEX];
            else if (intervaltypeID == DAOHelper.SECOND)
                result = (string)intervalTypeBox.Items[INTERVALETYPE_SECOND_INDEX];
            return result;
        }

        /// <summary>
        /// Clears the Sample group edit related controls in the Form.
        /// </summary>
        public void Clear()
        {
            string Function_Name = "Clear";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            ResetEditFields();
            m_editIntervalGrpID = "";
            intervalNameTextBox.Enabled = true;

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
        }       

        /// <summary>
        /// Returns the value of the specified row nad column in data grid.
        /// </summary>
        /// <param name="nRow">row number</param>
        /// <param name="nColumn">column number</param>
        /// <returns></returns>
       public string GetDataGridValue(int nRow, int nColumn)
        {
            return intervalConfigDataGridView.Rows[nRow].Cells[nColumn].Value.ToString();
        }

        /// <summary>
        /// Resets value of all edit related controls.
        /// </summary>
        public void ResetEditFields()
        {
            string Function_Name = "ResetEditFields";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            intervalNameTextBox.Text = "";
            intervalDescTextBox.Text = "";
            intervalUnitBox.Value = 1;
            intervalTypeBox.SelectedIndex = INTERVALETYPE_MINUTE_INDEX;
            deltaValueBox.Value = 1;
            dateTimePicker.Checked = false;
            disableCheckBox.Checked = true; // false;
            intervalNameTextBox.Enabled = false;
            LocationKeyTextBox.Text = LocationKeyHelper.GetInstance().LocationKey.ToString();
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
        } 

        /// <summary>
        /// Sets the value of all edit related controls based on sample group entity.
        /// </summary>
        /// <param name="etyOPCSampleGrp">sample group entity</param>
        public void SetEditFields(EtyDataLogDPGroupTrend etyDPGrpTrend)
        {
            string Function_Name = "SetEditFields";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            try
            {
                m_editIntervalGrpID = etyDPGrpTrend.SampleGrpID.ToString();
                intervalNameTextBox.Text = etyDPGrpTrend.SampleGrpName;
                intervalDescTextBox.Text = etyDPGrpTrend.SampleGrpDescription;
                intervalUnitBox.Value = (decimal) etyDPGrpTrend.Interval;
                intervalTypeBox.Text = GetIntervalTypeString(etyDPGrpTrend.IntervalType);
                dateTimePicker.Checked = false;
                if (etyDPGrpTrend.StartTime != "")
                {
                    dateTimePicker.Text = etyDPGrpTrend.StartTime;
                    dateTimePicker.Checked = true;
                }
                disableCheckBox.Checked = etyDPGrpTrend.Disabled;
                deltaValueBox.Value = (decimal) etyDPGrpTrend.DeltaValue;
                LocationKeyTextBox.Text = etyDPGrpTrend.LocationKey.ToString();
            }
            catch(Exception localException)
            {
                LogHelper.Error(localException.ToString());
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
        }

        /// <summary>
        /// Returns sample group entity from edit related controls.
        /// </summary>
        /// <returns></returns>
        public EtyDataLogDPGroupTrend GetEditFields()
        {
            string Function_Name = "GetEditFields";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            EtyDataLogDPGroupTrend etyDPGrpTrend = new EtyDataLogDPGroupTrend();
            if (dateTimePicker.Checked)
            {
                etyDPGrpTrend.StartTime = dateTimePicker.Value.ToString("HH:mm");
            }
            etyDPGrpTrend.IntervalType = DAOHelper.MINITE;

            if (intervalTypeBox.SelectedIndex == INTERVALETYPE_MINUTE_INDEX)
                etyDPGrpTrend.IntervalType = DAOHelper.MINITE;
            else if (intervalTypeBox.SelectedIndex == INTERVALETYPE_HOUR_INDEX)
                etyDPGrpTrend.IntervalType = DAOHelper.HOUR;
            else if (intervalTypeBox.SelectedIndex == INTERVALETYPE_SECOND_INDEX)
                etyDPGrpTrend.IntervalType = DAOHelper.SECOND;
             
           // etyOPCSampleGroup.IntervalType = intervalTypeBox.Text;
            etyDPGrpTrend.SampleGrpName = intervalNameTextBox.Text.Trim();
            etyDPGrpTrend.SampleGrpDescription = intervalDescTextBox.Text;
            etyDPGrpTrend.Disabled = disableCheckBox.Checked;
            etyDPGrpTrend.Interval = (double) intervalUnitBox.Value;
            etyDPGrpTrend.DeltaValue = (double) deltaValueBox.Value;
            etyDPGrpTrend.LocationKey = Convert.ToDouble(LocationKeyTextBox.Text);
            if (intervalNameTextBox.Text == "")
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_GRPNAMEMISSING_MSG, EnglishString.MSG_FOR_GRP_NAME));
                return null;
            }
            /*if (intervalUnitBox.Value <= 0)
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(FormCaptionHelper.OPCSAMPLECONFIG_INTERVALEVALMISS_MSG, EnglishString.MSG_FOR_INTERVAL));
                return null;
            }*/
            if (m_editIntervalGrpID == "")
            {
                etyDPGrpTrend.NewData = true;              
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
            return etyDPGrpTrend;
        }

        /// <summary>
        /// Cancel button handler function which clears edit related controls.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cancelMsgCmd_Click(object sender, EventArgs e)
        {
            Clear();
        }

        /// <summary>
        /// Sets First page as current page of the data grid control.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void firstIntervalConfigDataGridView_Click(object sender, EventArgs e)
        {
            pageNumIntervalConfigDataGridView.Value = 1;
        }

        /// <summary>
        /// Sets the previous page as current page of the datagrid.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void prevIntervalConfigDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumIntervalConfigDataGridView.Value > 1)
                pageNumIntervalConfigDataGridView.Value = pageNumIntervalConfigDataGridView.Value - 1;
        }

        /// <summary>
        /// Sets the next page as current page of the datagrid.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void nextIntervalConfigDataGridView_Click(object sender, EventArgs e)
        {
            if (pageNumIntervalConfigDataGridView.Value < pageNumIntervalConfigDataGridView.Maximum)
                pageNumIntervalConfigDataGridView.Value = pageNumIntervalConfigDataGridView.Value + 1;
        }

        /// <summary>
        /// Sets the last page as the current page of the datagrid.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void lastIntervalConfigDataGridView_Click(object sender, EventArgs e)
        {
            pageNumIntervalConfigDataGridView.Value = pageNumIntervalConfigDataGridView.Maximum;
        }

        /// <summary>
        /// Called when the page control value is changed and refreshs the datagrid based on the page selection.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pageNumIntervalConfigDataGridView_ValueChanged(object sender, EventArgs e)
        {
            PopulateIntervalConfigDataGridView(2);
        }

        /// <summary>
        /// Sorts the DataGrid control column value based on current sort column and order.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void intervalConfigDataGridView_Sorted(object sender, EventArgs e)
        {
            m_reload = true;
            PopulateIntervalConfigDataGridView(2);
        }
    }
}
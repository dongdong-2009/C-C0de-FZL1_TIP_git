using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using STEE.ISCS.GenericGUI;
using STEE.ISCS.MVC;
using TrainTimeTableViewer.Controller;
using TrainTimeTableViewer.Common;
using TrainTimeTable;
using DAO.TimeTable.Common;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.View
{
    public partial class TrainTimeTableView : BaseForm
    {
        private const string CLASS_NAME = "TrainTimeTableViewer.View.TrainTimeTableView";
        private string  m_Exclude_StationId_List = null;

        public System.Diagnostics.Process m_thisprocess;
        private int m_lineThickness = 10;

        public TrainTimeTableView()
        {
            string FUNCTION_NAME = "TrainTimeTableView";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            m_thisprocess = System.Diagnostics.Process.GetCurrentProcess();
            System.Diagnostics.Process[] ps = System.Diagnostics.Process.
               GetProcessesByName(System.Diagnostics.Process.GetCurrentProcess().ProcessName);
            if (ps.Length > 1)
            {
                
                m_thisprocess.Kill();
            }

            InitializeComponent();
            InitializeTimeTableViewer();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Exited.");
            
        }

        public string ExcludeStationId
        {
            get { return m_Exclude_StationId_List; }
        }

        /// <summary>
        /// Initialize TimeTableViewer.
        /// </summary>
        /// <param name="ctl">.</param>

        public void InitializeTimeTableViewer ()
        {
            string FUNCTION_NAME = "InitializeTimeTableViewer";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            DateTime Today = System.DateTime.Today.Date;
            this.datePicker.MaxDate = Today;
            this.datePicker.Value = Today;
            
            //minimum date set to 32 days earlier from current date.
            this.datePicker.MinDate = System.DateTime.Today.Date.AddDays(-TrainTimeTableConst.NUM_EARLIER_DAYS);
            this.startTimePicker.MinDate = Today;
            this.startTimePicker.Value = Today;
            this.startTimePicker.MaxDate = Today.AddHours(23).AddMinutes(58).AddSeconds(59);
            this.endTimePicker.Value = Today.AddHours(23).AddMinutes(59).AddSeconds(59);
            this.endTimePicker.MinDate = System.DateTime.Today.Date.AddMinutes(1);
            this.endTimePicker.MaxDate = Today.AddHours(23).AddMinutes(59).AddSeconds(59);
            
            bool bConnected = false;
            try
            {
                DBConnectionStrings.GetInstance().AddConnectionString(ConfigureFileHelper.GetInstance().DBConnectionString);
                
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                    EDebugLevelManaged.DebugInfo, "Connection String is: " + ConfigureFileHelper.GetInstance().DBConnectionString);
                
                bConnected = SimpleDatabase.GetInstance().OpenConnection();
                
                
            }
            catch (Exception localException)
            {     
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            if (!bConnected)
            {
                MessageBox.Show("Error", "No connection", MessageBoxButtons.OK, MessageBoxIcon.Error);
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "No Connection to Database.");
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Exited");

        }
        public void SetLineThickness(int thickNess)
        {
            m_lineThickness = thickNess;
        }

        /// <summary>
        /// Attach event listener to this UI.
        /// </summary>
        /// <param name="ctl">.</param>
        public override void AttachListener(IController ctrl)
       {
            TimeTableViewController controller = (TimeTableViewController)ctrl;
            this.StartButton.Click += controller.StartDrawTimeTable;
            this.saveToolStripMenuItem.Click += controller.CaptureScreenShoot;
            this.printToolStripMenuItem.Click += controller.PrintScreen;
            this.timeTableChartCtrl1.GetChartMain().MouseDoubleClick += controller.ChartClick;
            this.timeTableChartCtrl1.GetChartMain().MouseWheel += controller.ChartMouseWheel;
            this.startTimePicker.ValueChanged += controller.StartTimeChanged;
            this.timeTableChartCtrl1.GetChartMain().AxisViewChanged += controller.AxisViewChange;
            this.timeTableChartCtrl1.GetChartMain().MouseEnter += controller.ChartMouseEnter;
           
        }

        public void SetParameters(string Exclude_StationId)
        {
            string FUNCTION_NAME = "SetParameters";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered");

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Exclude_StationId - " + Exclude_StationId);
            //verify and set exclude station id list
            string [] ExcludeStationIdList;
            string ResExcludeStationIdList = null;
                   
            ExcludeStationIdList = Exclude_StationId.Split(',');

            foreach (string StationId in ExcludeStationIdList)
            {
                int StationId_Int;
                bool result = Int32.TryParse(StationId, out StationId_Int);

                //add only if valid integer
                if (result && StationId_Int >= 0)
                {
                    if (!String.IsNullOrEmpty(ResExcludeStationIdList))
                        ResExcludeStationIdList += ',';

                    ResExcludeStationIdList += StationId;
                }
            }

            m_Exclude_StationId_List = ResExcludeStationIdList;
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Final Exclude_StationId - " + m_Exclude_StationId_List);

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Exited");

        }
        /// <summary>
        /// Initialise chart
        /// </summary>       
        /// <param name="minValue">the min timestamp </param>
        /// <param name="maxValue">the max timestamp </param>
        /// <param name="TimeInterval">time interval </param>
        public void InitialiseChart(DateTime MinValue, DateTime MaxValue, int TimeInterval) 
            
        {
            string FUNCTION_NAME = "InitialiseChart";
            
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered");

            timeTableChartCtrl1.ClearAllSeriesAndMarkers();
            timeTableChartCtrl1.InitChartBefore(MinValue, MaxValue, TimeInterval, TrainTimeTableConst.MINOR_INTERVAL_NUM, TrainTimeTableConst.MAJOR_GRID_LINES_MAX_NUM);

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Exited.");
        }


        /// <summary>
        /// This function is called for drawing planned chart.
        /// </summary>
        /// <param name="Dictionary<int, List<TrainTimeTableData>>">
        public void DrawPlannedChart(Dictionary<int, List<TrainTimeTableData>> PlannedData)
        {
            string FUNCTION_NAME = "DrawPlannedChart";
           
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            try
            {
                this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                StartButton.Enabled = false;

                foreach (KeyValuePair<int, List<TrainTimeTableData>> plannedTimeTbl in PlannedData)
                {
                    string lblName = plannedTimeTbl.Key.ToString();
                    string seriesName = "Planned" + lblName;

                    timeTableChartCtrl1.AddPlannedSeries(seriesName, "Line", true, lblName, 1, true);

                    foreach (TrainTimeTableData Data in plannedTimeTbl.Value)
                    {
                        
                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                            EDebugLevelManaged.DebugInfo, "Data Point(" + Data.StationID + "," + Data.ArrTime + " Added to Series " + seriesName);

                        timeTableChartCtrl1.AddPoint(seriesName, Data.StationID, Data.StationName, Data.ArrTime);

                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                            EDebugLevelManaged.DebugInfo, "Data Point(" + Data.StationID + "," + Data.DeptTime + " Added to Series " + seriesName);

                        timeTableChartCtrl1.AddPoint(seriesName, Data.StationID, Data.StationName.ToString(), Data.DeptTime);

                    }
                }

                StartButton.Enabled = true;
                this.Cursor = System.Windows.Forms.Cursors.Default;
               
            }

            catch (Exception localException)
            {             
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                    EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            finally
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                    EDebugLevelManaged.DebugInfo, "Function_Exited");
            }
        }

        /// <summary>
        /// This function is called for drawing practical chart.
        /// </summary>
        /// <param name="mode">
        public void DrawPracticalChart(Dictionary<int, List<TrainTimeTableData>> PracticalData)
        {
            string FUNCTION_NAME = "DrawPracticalChart";
           
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            try
            {
                this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                StartButton.Enabled = false;

                foreach (KeyValuePair<int, List<TrainTimeTableData>> practicalTimeTbl in PracticalData)
                {
                    string lblName = practicalTimeTbl.Key.ToString();
                    string seriesName = "Practical" + lblName;

                    timeTableChartCtrl1.AddPracticalSeries(seriesName, "Line", true, lblName, 1, true);

                    foreach (TrainTimeTableData Data in practicalTimeTbl.Value)
                    {
                       
                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                            EDebugLevelManaged.DebugInfo, "Data Point(" + Data.StationID + "," + Data.ArrTime + " Added to Series " + seriesName);

                        timeTableChartCtrl1.AddPoint(seriesName, Data.StationID, Data.StationName, Data.ArrTime);

                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                            EDebugLevelManaged.DebugInfo, "Data Point(" + Data.StationID + "," + Data.ArrTime + " Added to Series " + seriesName);

                        timeTableChartCtrl1.AddPoint(seriesName, Data.StationID, Data.StationName.ToString(), Data.DeptTime);

                    }
                }

                StartButton.Enabled = true;
                this.Cursor = System.Windows.Forms.Cursors.Default;

            }

            catch (Exception localException)
            {              
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                    EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            finally
            {   
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                    EDebugLevelManaged.DebugInfo, "Function Exited");
            }
        }


        /// <summary>
        /// This function is called for Enabling Save and Print Buttons.
        /// </summary>
      
        public void  EnableSavePrint(bool Enable)
        {
            this.saveToolStripMenuItem.Enabled = Enable;
            this.printToolStripMenuItem.Enabled = Enable;
        }

        /// <summary>
        /// This function is called to set Minimum End Time.
        /// </summary>
        public void SetMinEndTime()
        {
            this.endTimePicker.MinDate = GetStartTime().AddMinutes(1);
        }

        public void TrainTimeTableView_FormClosed()
        {
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                    EDebugLevelManaged.DebugInfo, "Form Closed");

            return;
        }
        

#region getData

        public TrainTimeTableChartCtrl.TimeTableChartCtrl GetTimeTableChartCtl()
        {
            return timeTableChartCtrl1;
        }

        public DateTime GetStartTime()
        {
            return startTimePicker.Value;
        }

        public DateTime GetEndTime()
        {
            return endTimePicker.Value;
        }

        public DateTime GetDate()
        {
            return datePicker.Value;

        }

        public int GetTimeInterval()
        {
            return (int)(timeIntervalUpDown.Value);
        }

        public bool IsPlanned()
        {
            return planned.Checked;
        }

        public bool IsPractical()
        {
            return practical.Checked;
        }

#endregion

       

       
    }
}

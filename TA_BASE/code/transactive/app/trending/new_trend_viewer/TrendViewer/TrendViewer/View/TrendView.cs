using System;
using System.Threading;
using System.Text;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
//using System.Diagnostics;
using System.Runtime.InteropServices;

using OPCTrendLib.OPCHeader;
using OPCTrendLib.OPCDataInterface;
using OPCTrendLib.OPCData;

using OPCTrendLib;
using STEE.ISCS.GenericGUI;
using STEE.ISCS.MVC;
using System.Drawing.Drawing2D;
using TrendViewer.Controller;

using STEE.ISCS.Log;
using System.Diagnostics;
using System.Collections.Generic;
using STEE.ISCS.OPCProxy;
using Entity.Trending;
using TrendViewer.Common;
using System.Drawing.Printing;
using STEE.ISCS.MulLanguage;
using DAO.Trending.Common;
using DAO.Trending.Helper;
using System.Drawing.Imaging;
using TrendingHelper;
namespace TrendViewer.View
{
    public partial class TrendView : BaseForm
    {
        private const string CLASS_NAME = "TrendViewer.View.TrendView";

        Evaluator m_localEvaluator = new Evaluator();
        public System.Diagnostics.Process m_thisprocess;
        private bool m_realTimeStartFlag = false;
        private DateTime m_startDateRealTime = DateTime.Now;
        private int m_lineThickness = 10;
        public const int DEFAULT_GAP = 100;

        private PrintDocument printDocument1 = new PrintDocument();

        Bitmap memoryImage;


        /// <summary>
        /// constructor, will initialize OPC server
        /// </summary>
        public TrendView()
        {
            string Function_Name = "TrendView";

            LogHelper.Info(CLASS_NAME, Function_Name, "Come into TrendView().");
            m_thisprocess = Process.GetCurrentProcess(); 
            System.Diagnostics.Process[] ps = System.Diagnostics.Process.
                GetProcessesByName(System.Diagnostics.Process.GetCurrentProcess().ProcessName);
            if (ps.Length > TrendViewConst.INSTANCE_MAX_NUM)
            {
                //MessageBoxDialog.Show("Multiple");
                m_thisprocess.Kill();
            }

            InitializeComponent();
            InitializeIntervalTypBox();
            InitializeFrameTypeBox();

            InitializeTrendViewer();
            LogHelper.Info(CLASS_NAME, Function_Name, "Come out TrendView().");
        }

        public override void TranslateCaption()
        {
            this.realtime_grpbox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_RTDataGrpBox, LanguageHelper.TrendViewer_TrendView_RTDataGrpBox_EN);
            this.historical_GrpBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_HistDataGrpBox, LanguageHelper.TrendViewer_TrendView_HistDataGrpBox_EN);
            this.mixedModeCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MixModeCb, LanguageHelper.TrendViewer_TrendView_MixModeCb_EN);
            this.time_interval_lbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_PlotIntervalLbl, LanguageHelper.TrendViewer_TrendView_PlotIntervalLbl_EN);
            this.time_frame_lbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_TimeFrameLbl, LanguageHelper.TrendViewer_TrendView_TimeFrameLbl_EN);
            if (!m_realTimeStartFlag)
            {
                this.realTimeExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Start, LanguageHelper.TrendViewer_TrendView_Start_EN);
            }
            else
            {
                this.realTimeExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Stop, LanguageHelper.TrendViewer_TrendView_Stop_EN);
            }

            this.historicalExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Start, LanguageHelper.TrendViewer_TrendView_Start_EN);
            this.realtime3DCb.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_3DModeCb, LanguageHelper.TrendViewer_TrendView_3DModeCb_EN);
            this.endTimeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_EndTimeLbl, LanguageHelper.TrendViewer_TrendView_EndTimeLbl_EN);
            this.startTimeLbl.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_StartTimeLbl, LanguageHelper.TrendViewer_TrendView_StartTimeLbl_EN);
            this.utilityCommandToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuUtiCommand, LanguageHelper.TrendViewer_TrendView_MenuUtiCommand_EN);
            this.exportHistoryDataToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuExport, LanguageHelper.TrendViewer_TrendView_MenuExport_EN);
            this.screenShotTrendToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuScreenShort, LanguageHelper.TrendViewer_TrendView_MenuScreenShort_EN);
            this.printorToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuPrint, LanguageHelper.TrendViewer_TrendView_MenuPrint_EN);
            this.printBtn.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuPrint, LanguageHelper.TrendViewer_TrendView_MenuPrint_EN);
            this.configurationToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuConfig, LanguageHelper.TrendViewer_TrendView_MenuConfig_EN);
            this.displayDataPointsConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuDPConfig, LanguageHelper.TrendViewer_TrendView_MenuDPConfig_EN);
            this.displayDataMarkerConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuMarkerConfig, LanguageHelper.TrendViewer_TrendView_MenuMarkerConfig_EN);
            this.dataLoggerConfigurationToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuSampleGrpConfig, LanguageHelper.TrendViewer_TrendView_MenuSampleGrpConfig_EN);
            this.displayDataHistoryConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuHistDPConfig, LanguageHelper.TrendViewer_TrendView_MenuHistDPConfig_EN);
            this.displayDataPointsFormulaConfigToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuFormulaConfig, LanguageHelper.TrendViewer_TrendView_MenuFormulaConfig_EN);
            this.changeTrendTitleToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuChangeChartTitle, LanguageHelper.TrendViewer_TrendView_MenuChangeChartTitle_EN);
            this.aboutToolStripMenuItem.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_MenuAbout, LanguageHelper.TrendViewer_TrendView_MenuAbout_EN);
            this.chartGrpBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_GrpBox, LanguageHelper.TrendViewer_TrendView_GrpBox_EN);
            this.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Title, LanguageHelper.TrendViewer_TrendView_Title_EN);
        }

        /// <summary>
        /// Initialize the interval type box in the UI control.
        /// </summary>
        public void InitializeIntervalTypBox()
        {
            intervalTypeBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Second, LanguageHelper.TrendViewer_TrendView_Second_EN);
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Second, LanguageHelper.TrendViewer_TrendView_Second_EN));
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN));
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Hour, LanguageHelper.TrendViewer_TrendView_Hour_EN));
            intervalTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Day, LanguageHelper.TrendViewer_TrendView_Day_EN));
        }

        /// <summary>
        /// Initialize the time fram type box in the UI control.
        /// </summary>
        public void InitializeFrameTypeBox()
        {
            frameTypeBox.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN);
            frameTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN));
            frameTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Hour, LanguageHelper.TrendViewer_TrendView_Hour_EN));
            frameTypeBox.Items.Add(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Day, LanguageHelper.TrendViewer_TrendView_Day_EN));
        }

        /// <summary>
        /// get OPC server name from config file, connect to it. 
        /// ( If OPC server can't be connected, will not show err message.)
        /// </summary>
        public void InitializeTrendViewer()
        {
            string Function_Name = "InitializeTrendViewer";
            string opcSrv1Name;
            try
            {
                LogHelper.Info(CLASS_NAME, Function_Name,"Come into InitializeTrendViewer()");

                opcSrv1Name = ConfigureFileHelper.GetInstance().OPCServerName; //localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_NAME", "", 255, configFolder + "config.ini");

                if (opcSrv1Name.Trim() != "")
                {
                    OPCServerProxy.GetInstance().InitializeServer(opcSrv1Name, m_thisprocess.Id.ToString());
                    OPCServerProxy.GetInstance().ReconnectAndAddToOPC();
                    if (!OPCServerProxy.GetInstance().IsOPCServerConnected())  //OPC server is not connected
                    {
                        MessageBoxDialog.Show(
                            StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_OPCNOK, LanguageHelper.TrendViewer_Msg_OPCNOK_EN),
                            StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), 
                            MessageBoxButtons.OK, 
                            MessageBoxIcon.Error);
                    }
                }

                DBConnectionStrings.GetInstance().AddConnectionString(ConfigureFileHelper.GetInstance().ConnectionStringViewer);
                LogHelper.Info(CLASS_NAME, Function_Name, "Connection String is: " + ConfigureFileHelper.GetInstance().ConnectionStringViewer);
                bool bConnected = SimpleDatabase.GetInstance().OpenConnection();
                
                if (!bConnected)
                {
                    MessageBoxDialog.Show(
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoDB2, LanguageHelper.TrendViewer_Msg_NoDB2_EN),
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), 
                        MessageBoxButtons.OK, 
                        MessageBoxIcon.Error);
                }
                
                LocationKeyHelper.GetInstance().init();

                trendingChartCtl1.InitChart(DEFAULT_GAP);
                SetLineThickness(ConfigureFileHelper.GetInstance().DefaultLineThickness);
                trendingChartCtl1.EnableSmartLabel = ConfigureFileHelper.GetInstance().EnableSmartLabel;
                LogHelper.Debug(CLASS_NAME, Function_Name,"3D PointGapDepth = " + trendingChartCtl1.GapDepthFor3D);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
            LogHelper.Info(CLASS_NAME, Function_Name,"Come out InitializeTrendViewer()");
        }

        public void SetLineThickness(int thickNess)
        {
            m_lineThickness = thickNess;
        }

        /// <summary>
        /// Attach event listener to this UI.
        /// </summary>
        /// <param name="ctl">A TrendViewController which is connected to this page.</param>
        public override void  AttachListener(IController ctl)
        {
            TrendViewController controller = (TrendViewController)ctl;
            this.aboutToolStripMenuItem.Click += controller.showAbout;
            this.screenShotTrendToolStripMenuItem.Click += controller.CaptureScreenShoot;  //new EventHandler(CaptureScreen1); 
            this.displayDataPointsConfigToolStripMenuItem.Click += controller.ShowDataPointList;
            this.displayDataMarkerConfigToolStripMenuItem.Click += controller.ShowMarkerList;
            this.displayDataPointsFormulaConfigToolStripMenuItem.Click += controller.ShowFormulaList;
            this.displayDataHistoryConfigToolStripMenuItem.Click += controller.ShowHistDataPointList;
            this.changeTrendTitleToolStripMenuItem.Click += controller.ChangeTrendTitle; 
            this.realTimeExecuteCmd.Click += controller.RealTimeExecuteCmd_Click;
            this.dataLoggerConfigurationToolStripMenuItem.Click += controller.DataLoggerConfigurationToolStripMenuItem_Click; 
            this.exportHistoryDataToolStripMenuItem.Click += controller.ExportHistoryDataToolStripMenuItem_Click; 

            this.historicalExecuteCmd.Click += controller.HistoryExecuteCmd_Click;
            this.timer1.Tick += controller.timer1_Tick;
            this.printorToolStripMenuItem.Click += controller.PrintorToolStripMenuItem_Click; 
            //this.printorToolStripMenuItem.Click += new EventHandler(printButton_Click);
            //printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);

            this.printBtn.Click += controller.PrintorToolStripMenuItem_Click; 

        }

       void  CaptureScreen1(object sender, EventArgs e) //for test only: screen shot the whole form.
       {
           Graphics myGraphics = this.CreateGraphics();
           Size s = this.Size;
           memoryImage = new Bitmap(s.Width, s.Height, myGraphics);
           Graphics memoryGraphics = Graphics.FromImage(memoryImage);
           memoryGraphics.CopyFromScreen(this.Location.X, this.Location.Y, 0, 0, s);
           memoryImage.Save("C:\\temp_Pic.jpg", ImageFormat.Jpeg);
       }

        void printButton_Click(object sender, EventArgs e)  //for test only: print the whole form.
        {
            CaptureScreen();
            //printDocument1.Print();

            //
            PrintDialog PrintDialog1 = new PrintDialog();

            // Allow the user to choose the page range he or she would
            // like to print.
            PrintDialog1.AllowSomePages = false;

            // Show the help button.
            PrintDialog1.ShowHelp = true;

            // Set the Document property to the PrintDocument for 
            // which the PrintPage Event has been handled. To display the
            // dialog, either this property or the PrinterSettings property 
            // must be set 
            PrintDialog1.Document = printDocument1;


            DialogResult result = PrintDialog1.ShowDialog();

            // If the result is OK then print the document.
            if (result == DialogResult.OK)
            {
                printDocument1.Print();
            }
        }


        private void CaptureScreen()
        {
            Graphics myGraphics = this.CreateGraphics();
            Size s = this.Size;
            memoryImage = new Bitmap(s.Width, s.Height, myGraphics);
            Graphics memoryGraphics = Graphics.FromImage(memoryImage);
            memoryGraphics.CopyFromScreen(this.Location.X, this.Location.Y, 0, 0, s);
           // memoryImage.Save("C:\temp_Pic.jpg",ImageFormat.Jpeg);
        }

/*        private void printDocument1_PrintPage(System.Object sender,
               System.Drawing.Printing.PrintPageEventArgs e)
        {
            e.Graphics.DrawImage(memoryImage, 0, 0);
        }*/




        /// <summary>
        /// Initialize chart and series, 
        /// </summary>
        /// <param name="dpList"></param>
        /// <param name="formulaList"></param>
        /// <param name="enabledMarkerList"></param>
        public void InitializeChartForRealTime(List<EtyDataPoint> dpList, List<EtyFormula> formulaList)
        {
            string Function_Name = "InitializeChartForRealTime";
            try
            {
                LogHelper.Info(CLASS_NAME, Function_Name,"Come into InitializeChartForRealTime()");

                int frameInterval = GetFrameInterval();

                trendingChartCtl1.InitChartBeforeRealTime(frameInterval, timer1.Interval, realtime3DCb.Checked, DEFAULT_GAP);

                LogHelper.Debug(CLASS_NAME, Function_Name,"3D PointGapDepth = " + trendingChartCtl1.GapDepthFor3D);
                 
               

                AddSeriesForRealTimeChart(dpList, formulaList);
                //EnableZooming();
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            LogHelper.Info(CLASS_NAME, Function_Name,"Come out InitializeChartForRealTime()");

        }


        private void DrawDataMarker(List<EtyMarker> enabledMarkerList)
        {
            string Function_Name = "DrawDataMarker";
            try
            {
                if (enabledMarkerList == null) return;
                foreach(EtyMarker marker in enabledMarkerList)
                {
                    string markerName = marker.MarkerName;
                    double markerWidth = marker.MarkerWidth;
                    double markerValue = marker.MarkerValue;
                    Color bColor = Color.FromArgb(Convert.ToInt32(marker.MarkerBColor));
                    Color fColor = Color.FromArgb(Convert.ToInt32(marker.MarkerFColor)); 

                    if (markerName.Trim() != "")
                    {
                        trendingChartCtl1.AddMarker(markerName, markerWidth, markerValue, bColor, fColor);
                    }
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }


        }

        /// <summary>
        /// add datapoints to opc group
        /// </summary>
        /// <param name="dpList">Enabled dp list for real time charting</param>
        private void AddDPToOPCServer(List<EtyDataPoint> dpList)
        {
            string Function_Name = "AddDPToOPCServer";
            string dpName = "";
             if(dpList!=null)
             {
                  foreach (EtyDataPoint dp in dpList)
                  {
                      dpName = dp.DPName;
                       if (dpName.Trim() != ""  && dp.DPEnabled == true)
                       {
                           try
                           {
                               OPCServerProxy.GetInstance().AddOPCItem(dpName);
                           }
                           catch (System.Exception ex)
                           {
                               LogHelper.Error(CLASS_NAME, Function_Name, "Exception got when add datapoint: "+ dpName + ", exception is: " +ex.ToString() );
                           }
                       }
                  }
             }
        }

        /// <summary>
        /// add series for real time charting
        /// </summary>
        /// <param name="dpList">Enabled dp list for real time charting</param>
        /// <param name="formulaList">Enabled formula list for real time charting</param>
        private void AddSeriesForRealTimeChart(List<EtyDataPoint> dpList, List<EtyFormula> formulaList)
        {
            string Function_Name = "AddSeriesForRealTimeChart";
            try
            {
                string dpName = "";
                string dpType = "";
                Color dpColor = new Color();
                string lblEnabledStr = "";
                string lblName = "";
                bool lblEnabled = false;
                string dpEquation = "";

                LogHelper.Info(CLASS_NAME, Function_Name, "Come into AddSeriesForRealTimeChart()");

                if(dpList!=null)
                {


                    foreach (EtyDataPoint dp in dpList)
                    {
                        //Add a series
                        dpName = dp.DPName;
                        dpType = TrendViewerHelper.ConvertLineTypeFromEnumToEn(dp.DPType);
                        dpColor = Color.FromArgb(Convert.ToInt32(dp.DPColor));
                        lblName = dp.DPLblName;
                        lblEnabled = dp.DPLblEnabled;
                         if (dpName.Trim() != "" && dp.DPVisible == true )
                         {
                             trendingChartCtl1.AddRealTimeSeries(dpName, dpType, dpColor, lblEnabled, lblName, m_lineThickness);
                         }
                    }
                }

                // Formula Data Table
                if(formulaList!=null)
                {
                    foreach (EtyFormula formula in formulaList)
                    {
                        dpType = TrendViewerHelper.ConvertLineTypeFromEnumToEn(formula.DPType);
                        dpColor = Color.FromArgb(Convert.ToInt32(formula.DPColor));
                        lblName = formula.DPLblName;
                        lblEnabled = formula.DPLblEnabled;
                        dpEquation = formula.DPEquation;

                        if (lblName.Trim() != "")
                        {
                            lblName = TrendViewerHelper.getDisplayName(ETrendingChartType.Formula, lblName);
                            trendingChartCtl1.AddFormulaSeries(dpType, dpColor, lblEnabled, lblName, dpEquation, m_lineThickness);
                        }
                     }
                }
                trendingChartCtl1.DisableZooming();

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

            LogHelper.Info(CLASS_NAME, Function_Name,"Come out InitializeChartData()");
        }



        /// <summary>
        /// Update chart in :1. RealTime mode, or, 2.mixed mode
        /// </summary>
        /// <param name="histDPLogMap"> historical dp and log data used in mixed mode.</param>
        public void UpdateChartForRealTimeOrMixedMode(Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap)
        {
            string Function_Name = "UpdateChartForRealTimeOrMixedMode";
            try
            {
                LogHelper.Debug(CLASS_NAME, Function_Name,"Come into UpdateChartForRealTimeOrMixedMode()");   
                DateTime timeStamp = DateTime.Now;

                string tempString = "";
                string[] variableArray = null;

                LogHelper.Debug(CLASS_NAME, Function_Name,"chart count= " + trendingChartCtl1.GetSeriesCount());
                List<string> seriesNames = trendingChartCtl1.GetSeriesNames();

                //check for opcServer Connection
                if (!OPCServerProxy.GetInstance().IsOPCServerConnected())  //OPC server is not connected
                {
                    LogHelper.Debug("During realtime charting, opc server is down.");
                    OPCServerProxy.GetInstance().ReconnectAndAddToOPC();
                    return;
                }
                
               OPCServerProxy.GetInstance().AddFailedOPCItems();
                foreach (string seriesName in seriesNames)
                {
                    LogHelper.Debug(CLASS_NAME, Function_Name,"series name is:" + seriesName);

                    // if this is a real-time type series
                    if (TrendViewerHelper.getETrendingChartType(seriesName.ToString()) == ETrendingChartType.RealTime)
                    {
                        double dpValue = 0.0;
                       if( GetLatestDataPointValue(seriesName,ref dpValue))
                       {
                           trendingChartCtl1.AddRealTimePoint(seriesName, timeStamp, dpValue);
                       }
                        
                    }
                    else if (TrendViewerHelper.getETrendingChartType(seriesName.ToString()) == ETrendingChartType.Formula) // if this is a formula type series
                    {
                        // Formula Line
                        try
                        {
                            double dpValue = 0.0;

                            tempString = trendingChartCtl1.GetFormulaEquation(seriesName);
                            tempString =tempString.Replace("formula=", "");

                            variableArray = TrendViewerHelper.CheckValidVariables(tempString).Split(new string[] { "," }, StringSplitOptions.None);
                            
                            bool addThisNewPoint = true;  //draw the new point only if all the related dp can get value

                            for (int i = 0; i < variableArray.Length; i++)
                            { 
                                string realTimeSeriesName = variableArray[i].Trim();

                                if (GetLatestDataPointValue(realTimeSeriesName, ref dpValue))
                                {
                                    tempString = tempString.Replace("::" + variableArray[i].Trim() + "::", dpValue.ToString());
                                }
                                else
                                {
                                    break;
                                    addThisNewPoint = false;
                                }

                            }
                            if(addThisNewPoint)
                            {
                                object valueObj = m_localEvaluator.Eval(tempString);
                                double value = Double.Parse(valueObj.ToString());
                                trendingChartCtl1.AddFormulaPoint(seriesName, timeStamp, value);
                            }

                        }
                        catch (Exception localException)
                        {
                            LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
                            LogHelper.Debug(CLASS_NAME, Function_Name, "The formulation expression now is :" + tempString);
                            //trendingChartCtl1.AddFormulaPoint(seriesName, timeStamp, 0);
                        }
                    }

                }

                if (mixedModeCb.Checked && m_startDateRealTime.Date != DateTime.Now.Date)
                {
                    m_startDateRealTime = DateTime.Now;
                    DrawHistoryChart(DrawHistoryMode.Mixed, histDPLogMap);
                }

            }
            catch (Exception localException)
            {
                MessageBoxDialog.Show(
                    localException.ToString(),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK, 
                    MessageBoxIcon.Error);
                    
            }

            LogHelper.Debug(CLASS_NAME, Function_Name,"Come out UpdateChartForRealTimeOrMixedMode()");   
        }

        /// <summary>
        /// get new value from OPC server, if no value gotten, return false. if has value gotten, return true
        /// </summary>
        /// <param name="dpName">the datapoint name</param>
        /// <param name="dpValue">the value, to be set.</param>
        /// <returns>true if has dp value gotten, false if not</returns>
        public bool GetLatestDataPointValue(string dpName, ref double dpValue)
        {
            string Function_Name = "GetLatestDataPointValue";
            LogHelper.Debug(CLASS_NAME, Function_Name, "Come into GetLatestDataPointValue(), dp name is: "+ dpName);
            bool res=false;

            try
            {

                Object dpValueObj = OPCServerProxy.GetInstance().GetDataItemValue(dpName);

                if (dpValueObj == null)
                {
                    LogHelper.Warn(CLASS_NAME, Function_Name, "Can't find this datapoint in the group.");

                }
                else
                {
                    LogHelper.Debug(CLASS_NAME, Function_Name, "DP Value string: " + dpValueObj.ToString());
                }
                

                if (dpValueObj != null && dpValueObj.ToString() != "BLANK" && dpValueObj.ToString() != "NOTCONNECTED")
                {
                    
                    if (TrendViewerHelper.isNumeric(dpValueObj.ToString(), System.Globalization.NumberStyles.Number))
                    {
                        dpValue = Convert.ToDouble(dpValueObj);
                        LogHelper.Debug(CLASS_NAME, Function_Name,"DP Value is numeric, value = " + dpValue);
                        res = true;
    
                    }
                    else if (TrendViewerHelper.isBoolean(dpValueObj.ToString()))
                    {
                        if (Convert.ToBoolean(dpValueObj.ToString()))
                        { 
                            dpValue = 1; 
                        } 
                        else 
                        {
                            dpValue = 0;
                        }
                        LogHelper.Debug(CLASS_NAME, Function_Name,"DP Value is boolean, value = " + dpValue);  
                        res = true;
                    }
                    else
                    {
                        LogHelper.Warn(CLASS_NAME, Function_Name,"DP Value is neither numeric nor boolean, will not add this point to chart.");
                    }
                    
                }
                else
                {
                    LogHelper.Warn(CLASS_NAME, Function_Name, "Abnormal DP Value for Datapoint:" + dpName + ", value:" + dpValueObj.ToString());
                }

                LogHelper.Debug(CLASS_NAME, Function_Name, "Come out GetLatestDataPointValue(),return :"+ res);
                return res;

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
                LogHelper.Debug(CLASS_NAME, Function_Name, "Come out GetLatestDataPointValue()");  
                return false;
            }
           
        }


        private void TrendViewer_FormClosing(object sender, FormClosingEventArgs e)
        {
//             string Function_Name = "TrendViewer_FormClosing";
//             try
//             {
// 
// 
// //                 //kill this process:
// //                 Process curr = Process.GetCurrentProcess();
// //                 foreach (Process theprocess in Process.GetProcessesByName(curr.ProcessName))
// //                 {
// //                     if ((theprocess.Id == curr.Id) &&
// //                         (theprocess.MainModule.FileName == curr.MainModule.FileName))
// //                         theprocess.Kill();
// //                 }
// 
// 
// 
//             }
//             catch (Exception localException)
//             {
//                 LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
//             }
        }

        /// <summary>
        /// when user click 3D check box( in Real time part)
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void RealTime3DCb_CheckedChanged(object sender, EventArgs e)
        {
            string Function_Name = "RealTime3DCb_CheckedChanged";
            try
            {
                trendingChartCtl1.ChangeStyle(realtime3DCb.Checked);
                historical3DCb.Checked = realtime3DCb.Checked;
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// when user clicked "historical" start button. This is history mode, (not mix mode).
        /// </summary>
        /// <param name="histDPLogMap"> historical dp and log used in charting.</param>
        /// <param name="markerList">marker configuration list for drawing mark</param>
        /// <param name="minValue">the min timestamp in the dp log</param>
        /// <param name="maxValue">the max timestamp in the dp log</param>
        public void StartHistoryModeCharting(Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap, List<EtyMarker> markerList, DateTime minValue, DateTime maxValue)
        {
            string Function_Name = "StartHistoryModeCharting";

            try
            {
                this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                historicalExecuteCmd.Enabled = false;

                historicalExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Stop, LanguageHelper.TrendViewer_TrendView_Stop_EN);

                trendingChartCtl1.ClearAllSeriesAndMarkers();

                LogHelper.Info(CLASS_NAME, Function_Name, "Going to init chart before historical charting.");
                trendingChartCtl1.InitChartBeforeHistory(minValue, maxValue, historical3DCb.Checked);

                DrawDataMarker(markerList);

                DrawHistoryChart(DrawHistoryMode.NotMixed, histDPLogMap);


                graphModeLabel.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_GraphLblHist, LanguageHelper.TrendViewer_TrendView_GraphLblHist_EN);
                historicalExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Start, LanguageHelper.TrendViewer_TrendView_Start_EN);
                historicalExecuteCmd.Enabled = true;
                this.Cursor = System.Windows.Forms.Cursors.Default;
            }
            catch (System.Exception ex)
            {
                LogHelper.Error(ex.ToString());
            }

        }


        /// <summary>
        /// this function is called for history dp plotting when history mode started, or mixed mode started, or mixed mode updating,
        /// 
        /// </summary>
        /// <param name="mode">
        /// NotMixed: history mode , 
        /// Mixed: Mixed mode, (start or update)
        /// <param name="histDPLogMap"> historical dp and corresponding log used </param>
        private void DrawHistoryChart(DrawHistoryMode mode, Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap)
        {
            string Function_Name = "DrawHistoryChart";
            LogHelper.Info(CLASS_NAME, Function_Name, "Function_Entered");
            try
            {

                string tempString = "";
                EtyHistDataPoint histDP = null;

                foreach (KeyValuePair<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> dpLogList in histDPLogMap)
                {
                    histDP = dpLogList.Key;

                    if (histDP.DPEnabled == false) { continue; }
                    string dpName = histDP.DPName;
                    string lblName = histDP.DPLblName;
                    DateTime startDate = histDP.DPStartDateTime;
                    string dpType = TrendViewerHelper.ConvertLineTypeFromEnumToEn(histDP.DPType); 
                    Color dpColor = Color.FromArgb(Convert.ToInt32(histDP.DPColor));
                    bool lblEnabled = histDP.DPLblEnabled;

                    if (dpName.Trim() != "")
                    {
                        tempString = dpName + lblName + (startDate).ToString("yyMMdd");
                        tempString = TrendViewerHelper.getDisplayName(ETrendingChartType.Historical, tempString);

                        if(mode==DrawHistoryMode.NotMixed)
                        {
                            trendingChartCtl1.ClearSeriesByName(tempString);
                            trendingChartCtl1.AddHistorySeries(tempString, dpType, dpColor, lblEnabled, lblName, m_lineThickness, true);
                            DateTime tempDate = new DateTime();
                            foreach (EtyDataLogDPLogTrend dpLog in dpLogList.Value)
                            {
                                tempDate = dpLog.CreateTime;
                                trendingChartCtl1.AddHistoryPoint(tempString, tempDate, dpLog.Value);
                            }

                        }
                        else //Mixed Mode:
                        {
                            trendingChartCtl1.ClearSeriesByNameStartBy(tempString);  //when the date comes to next day, need to clear the old series.


                            if (dpLogList.Value.Count == 0) return;

                            DateTime currentHistSeriesDateForMixedMode = dpLogList.Value[0].CreateTime.Date;
                            string currentHistSeriesNameForMixedMode = tempString + "-" + currentHistSeriesDateForMixedMode.ToString("yyMMdd");
                            trendingChartCtl1.AddHistorySeries(currentHistSeriesNameForMixedMode, dpType, dpColor, lblEnabled, lblName, m_lineThickness, true);

                            DateTime tempDate = new DateTime();

                            foreach (EtyDataLogDPLogTrend dpLog in dpLogList.Value)
                            {
                                tempDate = dpLog.CreateTime;

                                //if the date is not the same as last one
                                if (tempDate.Date > currentHistSeriesDateForMixedMode)
                                {
                                    currentHistSeriesDateForMixedMode = dpLog.CreateTime.Date;
                                    currentHistSeriesNameForMixedMode = tempString + "-" + currentHistSeriesDateForMixedMode.ToString("yyMMdd");
                                    trendingChartCtl1.AddHistorySeries(currentHistSeriesNameForMixedMode, dpType, dpColor, lblEnabled, lblName, m_lineThickness,false);
                                }
                                
                                tempDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month
                                    , DateTime.Now.Day, tempDate.Hour
                                    , tempDate.Minute, tempDate.Second);

                                trendingChartCtl1.AddHistoryPoint(currentHistSeriesNameForMixedMode, tempDate, dpLog.Value);
                            } 

                        }
                        
                            
                       }

                    }
                }

            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
            finally
            {
                LogHelper.Info(CLASS_NAME, Function_Name, "Function_Exited");
            }
        }


        /// <summary>
        /// update chart title, X Title, Y title.
        /// </summary>
        /// <param name="titles"> the array saved chart title, X title, Y title. </param>
        public void UpdateTitles(string[] titles)
        {
            trendingChartCtl1.ChartTitle = titles[0];
            trendingChartCtl1.XTitle = titles[1];
            trendingChartCtl1.YTitle = titles[2];
        }

        private void TrendViewer_Paint(object sender, PaintEventArgs e)
        {
            string Function_Name = "TrendViewer_Paint";
            try
            {
                Graphics obj_Graph = e.Graphics;

                LinearGradientBrush b = new LinearGradientBrush(new System.Drawing.Rectangle(0, 0, this.Width, this.Height), Color.LightBlue, Color.BlanchedAlmond, LinearGradientMode.Vertical);

                obj_Graph.FillRectangle(b, new System.Drawing.Rectangle(0, 0, this.Width, this.Height));
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// when user click the "3D" check box in historical part.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void History3DCb_CheckedChanged(object sender, EventArgs e)
        {
            string Function_Name = "History3DCb_CheckedChanged";
            try
            {
                trendingChartCtl1.ChangeStyle(historical3DCb.Checked);
                realtime3DCb.Checked = historical3DCb.Checked;

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }


        public void InitializeTimer(ref System.Windows.Forms.Timer timer)
         {
             if (intervalTypeBox.Text == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Second, LanguageHelper.TrendViewer_TrendView_Second_EN))
                timer.Interval = (int)intervalUnitBox.Value * 1000;
            else if (intervalTypeBox.Text == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN))
                timer.Interval = (int)intervalUnitBox.Value * 60 * 1000;
            else if (intervalTypeBox.Text == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Hour, LanguageHelper.TrendViewer_TrendView_Hour_EN))
                timer.Interval = (int)intervalUnitBox.Value * 60 * 60 * 1000;
            else if (intervalTypeBox.Text == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Day, LanguageHelper.TrendViewer_TrendView_Day_EN))
                timer.Interval = (int)intervalUnitBox.Value * 24 * 60 * 60 * 1000;
        }

        /// <summary>
        /// when the user click "start" button in real time part.
        /// </summary>
        /// <param name="dpList">the enabled data point list configured for real time plotting</param>
        /// <param name="formulaList">the enabled formula list</param>
        /// <param name="markerList">the enabled marker list</param>
        /// <param name="histDPLogMap">the enabled historical dp list and corresponding logs</param>
        public void StartRealTimeOrMixedModeCharting(List<EtyDataPoint> dpList, List<EtyFormula> formulaList, List<EtyMarker> markerList, Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap)
        {
            string Function_Name = "StartRealTimeOrMixedModeCharting";
            try
            {    
                LogHelper.Info(CLASS_NAME, Function_Name,"Come into StartRealTimeCharting().");
                this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                realTimeExecuteCmd.Enabled = false;


                //check OPC server connection 
                if (!OPCServerProxy.GetInstance().IsOPCServerConnected())  //OPC server is not connected
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "Realtime charting starting, opc server is not connected.");
                    OPCServerProxy.GetInstance().ReconnectAndAddToOPC();
                    if (!OPCServerProxy.GetInstance().IsOPCServerConnected())
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, "Realtime charting starting, after re-connect, opc server is still not connected.");
                        MessageBoxDialog.Show(
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConn2OPC, LanguageHelper.TrendViewer_Msg_NoConn2OPC_EN),
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error);

                        realTimeExecuteCmd.Enabled = true;
                        this.Cursor = System.Windows.Forms.Cursors.Default;
                        
                        return;
                    }
                    else
                    {
                        LogHelper.Info(CLASS_NAME, Function_Name, "Realtime charting starting, after re-connect, opc server is connected.");
                    }
                }

                // when there is some Exception, e.g. "ComException :The RPC server is unavailable" in OPCServerProxy, it will return false
                try
                {
                    OPCServerProxy.GetInstance().RemoveAllOPCItem();
                }
                catch (System.Runtime.InteropServices.COMException ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "Realtime charting starting, get ComException when remove opc items: " + ex.ToString());
                    realTimeExecuteCmd.Enabled = true;
                    this.Cursor = System.Windows.Forms.Cursors.Default;
                    return;
                }
                catch (System.Exception ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "Realtime charting starting, get System Exception when remove opc items: " + ex.ToString());
                    MessageBoxDialog.Show(
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_RemoveOPCItemErr, LanguageHelper.TrendViewer_Msg_RemoveOPCItemErr_EN),
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    realTimeExecuteCmd.Enabled = true;
                    this.Cursor = System.Windows.Forms.Cursors.Default;
                    return;
                }

                
                trendingChartCtl1.ClearAllSeriesAndMarkers();
                InitializeChartForRealTime(dpList , formulaList);
                AddDPToOPCServer(dpList);
                InitializeTimer(ref timer1);

                DrawDataMarker(markerList);

                if (mixedModeCb.Checked)
                {
                    DrawHistoryChart(DrawHistoryMode.Mixed, histDPLogMap);
                }

                timer1.Start();

                m_startDateRealTime = DateTime.Now;

                System.Threading.Thread.Sleep(1000);

                UpdateChartForRealTimeOrMixedMode(histDPLogMap);

                realTimeExecuteCmd.Image = Properties.Resources.Close;
                m_realTimeStartFlag = true;

                realTimeExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Stop, LanguageHelper.TrendViewer_TrendView_Stop_EN);
                if (mixedModeCb.Checked)
                {
                    graphModeLabel.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_GraphLblMixed, LanguageHelper.TrendViewer_TrendView_GraphLblMixed_EN);
                }
                else
                {
                    graphModeLabel.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_GraphLblRT, LanguageHelper.TrendViewer_TrendView_GraphLblRT_EN);
                }
                realTimeExecuteCmd.Enabled = true;
                this.Cursor = System.Windows.Forms.Cursors.Default;
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// when user click "stop" button in Real Time part.
        /// </summary>
        public void StopRealTimeCharting()
        {
            string Function_Name = "StopRealTimeCharting";
            try
            {
                this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                realTimeExecuteCmd.Enabled = false;
                realTimeExecuteCmd.Image = Properties.Resources.Chart;

                timer1.Stop();

                m_realTimeStartFlag = false;

                realTimeExecuteCmd.Text = StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Start, LanguageHelper.TrendViewer_TrendView_Start_EN);
                graphModeLabel.Text = "";
                realTimeExecuteCmd.Enabled = true;
                this.Cursor = System.Windows.Forms.Cursors.Default;


            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }


#region  getData
        private int GetFrameInterval()
        {
            int frameInterval = 120;

            if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN))
                frameInterval = (int)frameUnitBox.Value * 60;
            else if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Hour, LanguageHelper.TrendViewer_TrendView_Hour_EN))
                frameInterval = (int)frameUnitBox.Value * 60 * 60;
            else if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Day, LanguageHelper.TrendViewer_TrendView_Day_EN))
                frameInterval = (int)frameUnitBox.Value * 24 * 60 * 60;
            return frameInterval;

        }

        public TrendingChartControl.TrendingChartCtl GetTrendingChartCtl()
        {
            return trendingChartCtl1;
        }

        public DateTime GetStartTime()
        {
            return this.startTimeBox.Value;
        }

        public DateTime GetEndTime()
        {
            return this.endTimeBox.Value;
        }


        public bool GetRealTimeStartFlag()
        {
            return m_realTimeStartFlag;
        }

        public string[] GetTitles()
        {
            string[] titles = new string[3];
            titles[0] = trendingChartCtl1.ChartTitle;
            titles[1] = trendingChartCtl1.XTitle;
            titles[2] = trendingChartCtl1.YTitle;
            return titles;

        }

        public bool IsMixMode()
        {
            return mixedModeCb.Checked;
        }

#endregion

        /// <summary>
        /// time frame should be : minute 1-60, hour 1-24, day 1-100
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frameTypeBox_SelectedIndexChanged(object sender, EventArgs e)
        {

            if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Minute, LanguageHelper.TrendViewer_TrendView_Minute_EN))
            {
                frameUnitBox.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
            }
            if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Hour, LanguageHelper.TrendViewer_TrendView_Hour_EN))
            {
                frameUnitBox.Maximum = new decimal(new int[] {
            24,
            0,
            0,
            0});
            }
            if (frameTypeBox.Text.Trim() == StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_TrendView_Day, LanguageHelper.TrendViewer_TrendView_Day_EN))
            {
                frameUnitBox.Maximum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            }
            
        }

        private void TrendView_FormClosed(object sender, FormClosedEventArgs e)
        {
            string Function_Name = "TrendView_FormClosed";
            try
            {
                OPCServerProxy.GetInstance().ReleaseOPC();
            }
            catch(System.Exception ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, "Exception: " + ex.ToString());
            }
        }


}

}

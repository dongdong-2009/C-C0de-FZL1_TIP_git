using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using STEE.ISCS.Log;
using STEE.ISCS.MVC;
using TrendViewer.Common;
using TrendViewer.View;
using TrendViewer.Model;
using Entity.Trending;
using System.Drawing.Printing;
using STEE.ISCS.OPCProxy;
using STEE.ISCS.MulLanguage;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using TrendingHelper;
namespace TrendViewer.Controller
{
   public class TrendViewController : IController,IOPCObserver
    {
        //currently, this variable is not useful.
        private string accessRight="Y";

        private int leftLocation = 0;
        private TrendViewModel m_Model;
        private TrendViewer.View.TrendView m_View;
        private TrendCache m_trendCache = new TrendCache();
        private const string CLASS_NAME = "TrendViewController";

        private delegate DialogResult messageBoxHandler(IWin32Window owner, string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon);
        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (TrendViewModel)model;
            m_View = (TrendViewer.View.TrendView)view;
            OPCServerProxy.GetInstance().AddObserver(this);
        }

        void IOPCObserver.ServerShutdownEvent()
        {
            m_View.BeginInvoke(new messageBoxHandler(MessageBoxDialog.Show), m_View,StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoConn2OPC, LanguageHelper.TrendViewer_Msg_NoConn2OPC_EN),
                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                 MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.
            if (notifyObj.Type == DataNotificaitonConst.DPListClosed)
            {
                m_trendCache.m_dataPointList.Clear();
                KeyValuePair<string, List<EtyDataPoint>> pair = (KeyValuePair<string, List<EtyDataPoint>>)(notifyObj.Body);
                m_trendCache.m_datapointGrpName = pair.Key;
                m_trendCache.m_dataPointList = pair.Value;
            }
            if (notifyObj.Type == DataNotificaitonConst.HistDPListClosed)
            {
                m_trendCache.m_histDataPointList.Clear();
                KeyValuePair<string, List<EtyHistDataPoint>> pair = (KeyValuePair<string, List<EtyHistDataPoint>>)(notifyObj.Body);
                m_trendCache.m_histDatapointGrpName = pair.Key;
                m_trendCache.m_histDataPointList = pair.Value;
            }
            if (notifyObj.Type == DataNotificaitonConst.MarkListClosed)
            {
                m_trendCache.m_markerList.Clear();
                KeyValuePair<string, List<EtyMarker>> pair = (KeyValuePair<string, List<EtyMarker>>)(notifyObj.Body);
                m_trendCache.m_markerGrpName = pair.Key;
                m_trendCache.m_markerList = pair.Value;
            }
            if (notifyObj.Type == DataNotificaitonConst.FormulaListClosed)
            {
                m_trendCache.m_formulaList.Clear();

                KeyValuePair<string, List<EtyFormula>> pair = (KeyValuePair<string, List<EtyFormula>>)(notifyObj.Body);
                m_trendCache.m_formulaGrpName = pair.Key;
                m_trendCache.m_formulaList = pair.Value;
            }

            if (notifyObj.Type == DataNotificaitonConst.ModifyTitles)
            {
                string[] titles = (string[])(notifyObj.Body);
                m_View.UpdateTitles(titles);
            }

        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="paramList"> arg[0]: "view-0/view-edit-0/..", arg[1] datapoint, arg[2] marker,arg[3] history, arg[4] formula
        /// </param>
        public void DrawTrendView(ref Form frm, string paramList)
        {
            //get trending data by model
            //call to view for drawing     
             string[] arg = paramList.Split(new char[] { ',' }, StringSplitOptions.None);
             int screenWidth = ConfigureFileHelper.GetInstance().ScreenWidth;
             const int OFFSET = 200; 

            if (arg.Length > 0)
            {
                if (arg[0].Trim().ToLower() == "view-edit-0")
                {
                    this.accessRight = "Y";
                    this.leftLocation = 0 + OFFSET;
                }
                else if (arg[0].Trim().ToLower() == "view-edit-1")
                {
                    this.accessRight = "Y";
                    this.leftLocation = screenWidth + OFFSET; 
                }
                else if (arg[0].Trim().ToLower() == "view-edit-2")
                {
                    this.accessRight = "Y";
                    this.leftLocation = screenWidth * 2 + OFFSET; 
                }
                else if (arg[0].Trim().ToLower() == "view-0")
                {
                    this.accessRight = "N";
                    this.leftLocation = 0;
                }
                else if (arg[0].Trim().ToLower() == "view-1")
                {
                    this.accessRight = "N";
                    this.leftLocation = screenWidth + OFFSET;
                }
                else
                {
                    this.accessRight = "N";
                    this.leftLocation = screenWidth * 2 + OFFSET;
                }
                frm.StartPosition = FormStartPosition.Manual;
                frm.Location = new Point(leftLocation, 0x9b);

            }
            if (arg.Length > 1)
            {
                if (arg[1].ToString().Trim() != "")
                {
                    DataPointListModel model = new DataPointListModel();
                    m_trendCache.m_dataPointList = model.GetDPListByGrp(arg[1].Trim());
                    m_trendCache.m_datapointGrpName = arg[1].Trim();
                }
            }
            if (arg.Length > 2)
            {
                if (arg[2].ToString().Trim() != "")
                { 
                    MarkerListModel model = new MarkerListModel();
                    m_trendCache.m_markerList = model.GetMarkerListByGrp(arg[2].Trim());
                    m_trendCache.m_markerGrpName = arg[2].Trim();
                }
            }
            if (arg.Length > 3)
            {
                if (arg[3].ToString().Trim() != "")
                { 
                    HistDataPointListModel model = new HistDataPointListModel();
                    m_trendCache.m_histDataPointList = model.GetHistDListByGrp(arg[3].Trim());
                    m_trendCache.m_histDatapointGrpName = arg[3].Trim();
                }
            }
            if (arg.Length > 4)
            {
                if (arg[4].ToString().Trim() != "")
                { 
                    FormulaListModel model = new FormulaListModel();
                    m_trendCache.m_formulaList = model.GetFormulaListByGrp(arg[4].Trim());
                    m_trendCache.m_formulaGrpName = arg[4].Trim();
                }
            }
            object sender = null;
            EventArgs e = null;
            if (arg.Length > 1)
            {
                RealTimeExecuteCmd_Click(sender, e);
            }
            
        }



        public void showAbout(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.About, m_View.ViewID);
            view.ShowView(true);
        }

        public void ShowMarkerList(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.MarkerList, m_View.ViewID);
            MarkerListController ctl = (MarkerListController)(view.getController());
            ctl.initMarkerTable();
            ctl.SetMarkerTable(m_trendCache.m_markerList);
            ctl.SetGrpName(m_trendCache.m_markerGrpName);
            view.ShowView(true);
        }

        public void ShowFormulaList(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.FormulaList, m_View.ViewID);
            FormulaListController ctl = (FormulaListController)(view.getController());
            ctl.initFormulaTable();
            ctl.SetFormulaTable(m_trendCache.m_formulaList);
            ctl.SetGrpName(m_trendCache.m_formulaGrpName);
            view.ShowView(true);
        }


        public void ShowDataPointList(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.DataPointList, m_View.ViewID);
            DataPointListController ctl = (DataPointListController)(view.getController());
            ctl.InitDataPointTableColumn();
            ctl.FillDataPointTable(m_trendCache.m_dataPointList);
            ctl.SetGrpName(m_trendCache.m_datapointGrpName);
            ((DataPointList)(view)).Display(1, ctl.GetDataPointTable());
            view.ShowView(true);
        }

        public void ShowHistDataPointList(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.HistDataPointList, m_View.ViewID);
            HistDataPointListController ctl = (HistDataPointListController)(view.getController());
            ctl.InitHistDataPointTableColumns();
            ctl.FillHistDataPointTable(m_trendCache.m_histDataPointList);
            ctl.SetGrpName(m_trendCache.m_histDatapointGrpName);
            ((HistDataPointList)(view)).Display(1, ctl.GetHistDataPointTable());
            view.ShowView(true);
        }

        public void ChangeTrendTitle(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.ChangeTrendTitle, m_View.ViewID);
            ChangeTrendTitleController ctl = (ChangeTrendTitleController)(view.getController());
            ctl.InitData(m_View.GetTitles());
            ((EditChartTitle)(view)).Display(ctl.GetTitles());
            view.ShowView(true);


        }

        public void CaptureScreenShoot(object sender, EventArgs e)
        {
            string Function_Name = "CaptureScreenShoot";
            try
            {
                // Create a new save file dialog
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();

                // Sets the current file name filter string, which determines 
                // the choices that appear in the "Save as file type" or 
                // "Files of type" box in the dialog box.
                //saveFileDialog1.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                saveFileDialog1.InitialDirectory = ConfigureFileHelper.GetInstance().DefaultFolder;
                
                //hide "emf" format
                //saveFileDialog1.Filter = "Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|EMF (*.emf)|*.emf|PNG (*.png)|*.png|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif";
                saveFileDialog1.Filter = "Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif";
                
                saveFileDialog1.FilterIndex = 2;
                saveFileDialog1.RestoreDirectory = true;

                // Set image file format
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    /*if( saveFileDialog1.FileName.Equals("")) //this if will never come in.
                    {
                        LogHelper.Debug(CLASS_NAME, Function_Name,"***************");
                        MessageBoxDialog.Show("Please specify filename to save.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        return;
                    }*/
                    

                    m_View.GetTrendingChartCtl().SaveAsImage(saveFileDialog1.FileName);
                    MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue((LanguageHelper.TrendViewer_Msg_ScreenShotOK),
                        LanguageHelper.TrendViewer_Msg_ScreenShotOK_EN));
                }

            }
            catch (Exception localException)
            {
                MessageBoxDialog.Show
                    (StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ScreenShotNOK_1, LanguageHelper.TrendViewer_Msg_ScreenShotNOK_1_EN) 
                    + localException.Message.ToString(),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        //when usedInMixedMode is false, will ignore startTime and endTime
        public Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> GetHistDPLogMap(DateTime startTime, DateTime endTime)
        {
            Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap = new Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>>();
           
            List<EtyDataLogDPLogTrend> dpLogList= null;
            foreach (EtyHistDataPoint histDP in m_trendCache.m_histDataPointList)
            {

                if(histDP.DPEnabled == false) continue;
                dpLogList = m_Model.GetHistDPLogList(histDP, startTime, endTime);
                histDPLogMap.Add(histDP, dpLogList);
            }
            return histDPLogMap;
        }

        /// <summary>
        /// when the user clicked "Start/Stop" from the TrendViewer main form, real time district(top left).
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void RealTimeExecuteCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "RealTimeExecuteCmd_Click";
            LogHelper.Info(CLASS_NAME, Function_Name, "Function_Entered");

            m_trendCache.ClearHistLogForMixedMode();

            //if it's a "start" button.
            if (!m_View.GetRealTimeStartFlag())
            {
                //for realtime dp, there are checkbox: "Enable datapoint" and "Display" , 
                //if  "Enabled dp"==true && Display==true: display properly
                //if  Display == false  : not display this series.
                //if  Display == true && Enable DP==false : will display the series name ,but without value.

                List<EtyDataPoint> realTimeDPList = m_trendCache.GetVisibleOrEnabledDPList();

                List<EtyFormula> formulaList = m_trendCache.GetEnabledFormulaList();
                List<EtyMarker> markerList = m_trendCache.GetEnabledMarkerList();

                //if there is no enabled datapoint configured for plotting.
                if (realTimeDPList==null || realTimeDPList.Count < 1)
                {
                    //MessageBoxDialog.Show("Please Select a Configuration to Plot");
                    MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SelectConfirm, LanguageHelper.TrendViewer_Msg_SelectConfirm_EN));
                    LogHelper.Info(CLASS_NAME, Function_Name, "Function_Exited");
                    return;

                }
                else
                {
                    Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap = null;
                    if (m_View.IsMixMode() == true)
                    {
                        DateTime startTime = m_View.GetStartTime();
                        DateTime endTime = m_View.GetEndTime();
                        histDPLogMap = GetHistDPLogMap(startTime, endTime);
                        m_trendCache.SetHistLogForMixedMode(histDPLogMap);
                    }
                    m_View.StartRealTimeOrMixedModeCharting(realTimeDPList,formulaList,markerList,m_trendCache.GetHistLogForMixedMode());
                }
            }
            // If user click "stop"
            else
            {
                m_View.StopRealTimeCharting();
            }

            LogHelper.Info(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void DataLoggerConfigurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Function_Name = "DataLoggerConfigurationToolStripMenuItem_Click";
            try
            {
                System.Diagnostics.Process.Start("OPCSampleGrpConfig.exe");
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void ExportHistoryDataToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Function_Name = "ExportHistoryDataToolStripMenuItem_Click";
            if (m_trendCache.m_histDataPointList == null
                || m_trendCache.m_histDataPointList.Count == 0
                )
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ExportNoData, LanguageHelper.TrendViewer_Msg_ExportNoData_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_WarningTitle, LanguageHelper.TrendViewer_Msg_WarningTitle_EN), 
                    MessageBoxButtons.OK, 
                    MessageBoxIcon.Warning);
                LogHelper.Error(CLASS_NAME, Function_Name, "No Data for current Datapoint History.");
                return;
            }
            try
            {
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();
                //saveFileDialog1.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                saveFileDialog1.InitialDirectory = ConfigureFileHelper.GetInstance().DefaultFolder;//defaultFolder;
                saveFileDialog1.Filter = "csv files (*.csv)|*.csv";
                saveFileDialog1.OverwritePrompt = true;
                saveFileDialog1.FileName = "OPCTrendViewerHistoryData";
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    /*if (saveFileDialog1.FileName.Trim().Equals(""))  //this if will never come in.
                    {
                        MessageBoxDialog.Show("Please specify filename to save.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        return;
                    }*/

                    ExportHistoryData(saveFileDialog1.FileName);
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }

        }

        private void ExportHistoryData(string filename)
        {
            string Function_Name = "ExportHistoryData";

            try
            {
                // Open Stream for Writing

                System.IO.StreamWriter SW;
                SW = System.IO.File.AppendText(filename);

                EtyHistDataPoint histDP = null;
                string originalMsg = "";
                string messageData = "";
                DateTime startTime = m_View.GetStartTime();
                DateTime endTime = m_View.GetEndTime();
                bool hasData = false;

                for (int i = 0; i < m_trendCache.m_histDataPointList.Count; i++)
                {
                    histDP = m_trendCache.m_histDataPointList[i];
                    if (histDP.DPEnabled == false) continue;

                    if (histDP.DPName.Trim() != "")
                    {
                        List<EtyDataLogDPLogTrend> histDPLogList = m_Model.GetHistDPLogList(m_trendCache.m_histDataPointList[i], startTime, endTime);

                        originalMsg = histDP.DPName + "," + histDP.DPServer;
                        foreach (EtyDataLogDPLogTrend dpLog in histDPLogList)
                        {
                            messageData = originalMsg;
                            messageData += "," + dpLog.Value.ToString();
                            messageData += "," + dpLog.CreateTime.ToString("dd'/'MM'/'yyyy hh:mm:ss tt");   // ("dd-MM-yyyy HH:mm:ss");
                            messageData += "," + "127.0.0.1"; //histDP.DPHost.ToString();
                            SW.WriteLine(messageData);
                            hasData = true;
                        }
                    }
                }

                SW.Close();
                if (!hasData)
                {
                    File.Delete(filename);
                    MessageBoxDialog.Show(
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ExportNoData, LanguageHelper.TrendViewer_Msg_ExportNoData_EN),
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_WarningTitle, LanguageHelper.TrendViewer_Msg_WarningTitle_EN),
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    LogHelper.Error(CLASS_NAME, Function_Name, "No Data for current Datapoint History.");
                    return;
                }
                
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ExportOK, LanguageHelper.TrendViewer_Msg_ExportOK_EN));
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }

        }

        /// <summary>
        /// when user clicked "historical" start button. This is history mode, (not mix mode).
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void HistoryExecuteCmd_Click(object sender, EventArgs e)
        {
            string Function_Name = "HistoryExecuteCmd_Click";
            LogHelper.Info(CLASS_NAME, Function_Name, "Function_Entered");
            try
            {
                //check whether real time charting is ON
                if (m_View.GetRealTimeStartFlag())
                {
                    MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_StopRT2Continue, LanguageHelper.TrendViewer_Msg_StopRT2Continue_EN));
                    return;
                }

                if ( m_trendCache.CountEnabledHistDPList()< 1)
                {
                    //MessageBoxDialog.Show("Please Select a Configuration to Plot");
                    MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SelectConfirm, LanguageHelper.TrendViewer_Msg_SelectConfirm_EN));
                    return;
                }

                DateTime minValue = GetMinTimeForHistLog();
                DateTime maxValue = GetMaxTimeForHistLog();

                if (maxValue.ToOADate() < minValue.ToOADate())
                {
                    MessageBoxDialog.Show("Invalid EndTime : EndTime is less than StartTime");
                    return;
                }

                Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap = null;

                LogHelper.Info(CLASS_NAME, Function_Name, "Going to get historical datapoint log from DB ");

                DateTime startTime = m_View.GetStartTime();
                DateTime endTime = m_View.GetEndTime();
                histDPLogMap = GetHistDPLogMap( startTime, endTime);
                //histDPLogMap = GetHistDPLogMap(false, DateTime.Now, DateTime.Now);

                LogHelper.Info(CLASS_NAME, Function_Name, "Hitorical datapoint log gotten for "+ histDPLogMap.Count + " datapoint.");
                foreach (KeyValuePair<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> dplog in histDPLogMap)
                {
                    if( dplog.Key!=null && dplog.Value!=null)
                    {
                        LogHelper.Info(CLASS_NAME, Function_Name, "Hist DP Name:  " + dplog.Key.DPName + 
                            ", from: "+ dplog.Key.DPStartDateTime.ToString("yyyy-MM-dd") +" "+ startTime.ToString("HH:mm:ss") +
                            ", to: "+ dplog.Key.DPEndDateTime.ToString("yyyy-MM-dd") + " "+ endTime.ToString("HH:mm:ss") +
                            " has log: "+ dplog.Value.Count );
                    }
                    
                }
                

                List<EtyMarker> markerList = m_trendCache.GetEnabledMarkerList();

                m_View.StartHistoryModeCharting(histDPLogMap, markerList, minValue, maxValue );
               
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

        DateTime GetMinTimeForHistLog()
        {
              
             DateTime minValue = DateTime.Now;
             minValue = GetMinDateOfEnabledHistDP();
             DateTime startTime = m_View.GetStartTime();
             minValue = new DateTime(minValue.Year, minValue.Month, minValue.Day, startTime.Hour,startTime.Minute, startTime.Second);
             return minValue;

        }
        DateTime GetMaxTimeForHistLog()
        {
            DateTime maxValue = DateTime.Now;
            maxValue = GetMaxDateOfEnabledHistDP();
            DateTime endTime = m_View.GetEndTime();
            maxValue = new DateTime(maxValue.Year, maxValue.Month, maxValue.Day, endTime.Hour, endTime.Minute, endTime.Second);
            return maxValue;
        }

        DateTime GetMinDateOfEnabledHistDP()  // called only when Enabled Hist DP exists.
        {
            DateTime minDate = DateTime.MaxValue;
           
            foreach (EtyHistDataPoint dp in m_trendCache.m_histDataPointList)
            {
                if(dp.DPEnabled == false) continue;
                if(dp.DPStartDateTime < minDate)
                {
                    minDate = dp.DPStartDateTime; 
                }
            }

            if (minDate != DateTime.MaxValue)
            {
                return minDate;
            }
            else
            { 
                //means no enabled dp in history dp list
                return DateTime.Now;
            }
        }

        DateTime GetMaxDateOfEnabledHistDP() // called only when Enabled Hist DP exists.
        {
            DateTime maxDate = DateTime.MinValue;
            foreach (EtyHistDataPoint dp in m_trendCache.m_histDataPointList)
            {
                if(dp.DPEnabled == false) continue;
                if (dp.DPEndDateTime > maxDate)
                {
                    maxDate = dp.DPEndDateTime; 
                }
            }
            if (maxDate != DateTime.MinValue)
            {
                return maxDate;
            }
            else
            {
                //means no enabled dp in history dp list
                return DateTime.Now;
            }
        }

        public void timer1_Tick(object sender, EventArgs e)
        {
            string Function_Name = "timer1_Tick";
            //MessageBoxDialog.Show("timer 1 " + DateTime.Now.ToString("HH:mm:ss"));
            try
            {
                Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histDPLogMap = null;
                    if (m_View.IsMixMode() == true)
                    {
                        histDPLogMap = m_trendCache.GetHistLogForMixedMode();
                    }
                    m_View.UpdateChartForRealTimeOrMixedMode(histDPLogMap);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void PrintorToolStripMenuItem_Click(object sender, EventArgs e)
        {

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
            PrintDocument docToPrint = m_View.GetTrendingChartCtl().GetPrintDoc();
            PrintDialog1.Document = docToPrint;


            DialogResult result = PrintDialog1.ShowDialog();

            // If the result is OK then print the document.
            if (result == DialogResult.OK)
            {
                docToPrint.Print();
            }
        }
    }
}

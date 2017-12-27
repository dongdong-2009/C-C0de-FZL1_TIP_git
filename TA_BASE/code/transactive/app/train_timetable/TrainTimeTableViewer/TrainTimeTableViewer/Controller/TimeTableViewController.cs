using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Printing;
using STEE.ISCS.MVC;
using TrainTimeTableViewer.View;
using TrainTimeTableViewer.Common;
using TrainTimeTable;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.Controller
{
    class TimeTableViewController : IController
    {
        private const string CLASS_NAME = "TimeTableViewController";

        private TrainTimeTableViewer.View.TrainTimeTableView m_View;
        private TrainTimeTableViewer.Model.TimeTableViewModel m_Model;
        private DateTime ChartDate; 

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (TrainTimeTableViewer.Model.TimeTableViewModel)model;
            m_View = (TrainTimeTableViewer.View.TrainTimeTableView)view;
            

        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            return;
        }

        /// <summary>
        ///  Connect to Oracle
        /// </summary>
        /// <param name="connectionString">Oracle connection string</param>
        /// <returns>true - connected , false - failed</returns>
        public bool GetDatabaseConnection()
        {
            return m_Model.OpenConnection();
        }

        public void setExcludeStationList()
        {
            string FUNCTION_NAME = "setExcludeStationList";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            m_Model.ExcludeStationIdList = m_View.ExcludeStationId;

           
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Exited.");
        }

        /// <summary>
        /// Gets start time to the nearest lower bound minutes.
        /// </summary>
        /// <returns> Returns start time</returns>
        public DateTime GetStartTimeRoundDown()
        {
            DateTime SelectedDate = m_View.GetDate().Date;
            DateTime StartDateTime = m_View.GetStartTime();

            return SelectedDate.Date.AddHours(StartDateTime.Hour).AddMinutes(StartDateTime.Minute);
        }

        /// <summary>
        /// Gets The end time to the nearest upper bound minute. 
        /// </summary>
        /// <returns>Returns end time</returns>
        public DateTime GetEndTimeRoundUp()
        {
            DateTime SelectedDate = m_View.GetDate().Date;
            TimeSpan EndTime = m_View.GetEndTime().TimeOfDay;
            DateTime EndDateTime = SelectedDate.Add(EndTime);

            //set Milliseconds to 0;
            EndDateTime = EndDateTime.AddMilliseconds(-EndDateTime.Millisecond);

            //if time is 23:59, round up to 23:59:59
            if (EndDateTime.Hour == 23 && EndDateTime.Minute == 59)
            {
                return (SelectedDate.AddHours(23).AddMinutes(59).AddSeconds(59));
            }

            return (EndDateTime.AddSeconds((60 - EndDateTime.Second) % 60));
            
        }

        public void StartDrawTimeTable(object sender, EventArgs e)
        {   
            string FUNCTION_NAME = "StartDrawTimeTable";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            //set Exclude Station List
            setExcludeStationList();

            //clear Data cache
            m_Model.ClearDataCache();
            
            try
            {
                

                ChartDate = m_View.GetDate().Date; ;
                DateTime minDateTime = GetStartTimeRoundDown();
                DateTime maxDateTime = GetEndTimeRoundUp();

                int timeInterval = m_View.GetTimeInterval();

                //Check if Planned or Practical Option is Selected

                if (!m_View.IsPlanned() && !m_View.IsPractical())
                {
                    MessageBox.Show("请选择显示模式", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), 
                        EDebugLevelManaged.DebugInfo, "Please Select Planned Or Practical TimeTable Option");
                    return;
                }
                 
                //Check start Time to be less than end Time
                if (minDateTime.CompareTo(maxDateTime) >= 0)
                {
                    MessageBox.Show("结束时间不能小于开始时间", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Start Time Has To Be Less Than End Time");
                    return;
                }

                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Initilise Chart Before Drawing.");
                
                m_View.InitialiseChart(minDateTime, maxDateTime, timeInterval);

                Dictionary<int, List<TrainTimeTableData>> practicalData = null;
                Dictionary<int, List<TrainTimeTableData>> plannedData = null;

                bool DataAvailable = false;

                //check the Database Connection
                bool bConnectionOpened = GetDatabaseConnection();
                if (!bConnectionOpened)
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                    EDebugLevelManaged.DebugInfo, "No Database Connection");

                    MessageBox.Show("No Database Connection", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Error);

                }

                else
                {

                    //Get Data
                    if (m_View.IsPlanned())
                    {
                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Planned Option Selected.");

                        plannedData = m_Model.GetTimeTableData(true, minDateTime, maxDateTime);
                        m_View.DrawPlannedChart(plannedData);

                        if (plannedData != null && plannedData.Count > 0)
                        {
                            DataAvailable = true;
                        }

                    }

                    if (m_View.IsPractical())
                    {
                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Practical Option Selected.");

                        practicalData = m_Model.GetTimeTableData(false, minDateTime, maxDateTime);
                        m_View.DrawPracticalChart(practicalData);

                        if (practicalData != null && practicalData.Count > 0)
                        {
                            DataAvailable = true;
                        }

                    }

                    //setLabel
                    m_View.GetTimeTableChartCtl().SetSeriesLabel();

                    if (!DataAvailable)
                    {
                        MessageBox.Show("所选时间没有找到相关记录.", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        m_View.EnableSavePrint(false);

                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                            EDebugLevelManaged.DebugInfo, "No date Avaiable For Selected Time");
                    }
                    else
                    {
                        m_View.EnableSavePrint(true);

                        //scroll to plot area with series
                        m_View.GetTimeTableChartCtl().ScrollToPlotArea();
                    }

                }
               
            }
            catch (Exception localException)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited");
        }

        public void CaptureScreenShoot(object sender, EventArgs e)
        {
            string FUNCTION_NAME = "CaptureScreenShoot";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");
            try
            {
                // Create a new save file dialog
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();

                // Sets the current file name filter string, which determines 
                // the choices that appear in the "Save as file type" or 
                // "Files of type" box in the dialog box.
          
                saveFileDialog1.InitialDirectory = ConfigureFileHelper.GetInstance().DefaultFolder;
                
                saveFileDialog1.Filter = "Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif";
                saveFileDialog1.FilterIndex = 2;
                saveFileDialog1.RestoreDirectory = true;

                // Set image file format
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    m_View.GetTimeTableChartCtl().SaveAsImage(saveFileDialog1.FileName);
                    MessageBox.Show("图象保存成功", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Image Saved Successfully");
                }

            }
            catch (Exception localException)
            {
                MessageBox.Show("图象保存失败", "列车时刻表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited");
        }

        public void PrintScreen(object sender, EventArgs e)
        {

            string FUNCTION_NAME = "PrintScreen";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

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
            PrintDocument docToPrint = m_View.GetTimeTableChartCtl().GetPrintDoc();
            PrintDialog1.Document = docToPrint;

            DialogResult result = PrintDialog1.ShowDialog();

            // If the result is OK then print the document.
            if (result == DialogResult.OK)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Printing Document");
                docToPrint.Print();
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited");
        }

        public void ChartClick(object sender, MouseEventArgs e)
        {
            string FUNCTION_NAME = "ChartClick";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

            //if chart is empty return
            if (m_View.GetTimeTableChartCtl().IsChartEmpty())
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited, Chart Is Empty");
                return;
            }

            String SeriesName = "";

            SeriesName = m_View.GetTimeTableChartCtl().GetSeriesName(e);

            if (SeriesName.Equals(""))
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Chart Area Clicked. But, Not on Series.");
                return;
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Chart Area Clicked. Series - " + SeriesName);

            IView view = ViewManager.GetInstance().GetView(TrainTimeTableConst.TimeTableWindowView, m_View.ViewID);
            TimeTableWindowController Ctrl = (TimeTableWindowController)(view.getController());
            bool IsPlanned;
            int TrainServiceId;
            if(SeriesName.StartsWith("Planned"))
            {
                IsPlanned = true;
                TrainServiceId = Convert.ToInt32(SeriesName.Substring(7));
                
            }
            else
            {
                IsPlanned = false;
                TrainServiceId = Convert.ToInt32(SeriesName.Substring(9));
                    
            }
            //get date from chart
            //DateTime date = m_View.GetDate().Date;
           

            //hightlight selected series
            m_View.GetTimeTableChartCtl().HightlightSeries(SeriesName);

            
            Ctrl.ShowTimeTable(TrainServiceId, IsPlanned, ChartDate);
            view.ShowView(true);

            //remove hightlight on selected series
            m_View.GetTimeTableChartCtl().RemoveHightlightSeries(SeriesName);

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited");

        }

        public void ChartMouseWheel(object sender, MouseEventArgs e)
        {

            string FUNCTION_NAME = "ChartMouseWheel";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

            //if chart is empty return
            if (m_View.GetTimeTableChartCtl().IsChartEmpty())
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited Without Zoom Operation, Chart Is Empty");
                return;
            }
            //only zoom if mouse position within chartArea
            float XMin, XMax, YMin, YMax;
            m_View.GetTimeTableChartCtl().GetChartAreaBoundary(out XMin, out XMax, out YMin, out YMax);


            if (e.X < XMin || e.X > XMax || e.Y < YMin || e.Y > YMax)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited Without Zoom Operation, Mouse Outside ChartArea");
                return;
            }
            try
            {
                if (e.Delta < 0)
                {
                    m_View.GetTimeTableChartCtl().ZoomOut(m_View.GetTimeInterval() * TrainTimeTableConst.MINOR_INTERVAL_NUM * TrainTimeTableConst.MAJOR_GRID_LINES_MAX_NUM);
                }

                if (e.Delta > 0)
                {
                    m_View.GetTimeTableChartCtl().Zoom(e.Location.X, e.Location.Y);

                }
            }
            catch (Exception localException)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, localException.ToString());
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited");

        }

      

        public void AxisViewChange(object sender, System.Windows.Forms.DataVisualization.Charting.ViewEventArgs e)
        {
            //SetLabel
             m_View.GetTimeTableChartCtl().SetSeriesLabel();
             m_View.GetTimeTableChartCtl().SetScale();
        }

        public void ChartMouseEnter(object sender, EventArgs e)
        {
            m_View.GetTimeTableChartCtl().SetChartFocus();
        }

        public void StartTimeChanged(object sender, EventArgs e)
        {
            string FUNCTION_NAME = "StartTimeChanged";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

            m_View.SetMinEndTime();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Function Exited");
            
        }

       
    }

}

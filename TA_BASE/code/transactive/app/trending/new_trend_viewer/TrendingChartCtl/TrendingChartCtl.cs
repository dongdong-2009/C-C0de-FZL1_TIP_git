using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;

using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing.Printing;


namespace TrendingChartControl
{
    public partial class TrendingChartCtl : UserControl
    {
    
        //used for realtime - 3D effect issue
        // change chart PointDepth value after first set of datapoints(first datapoint of all series)
        int m_realtimeCount = 0;
        bool m_enableSmartLabel = false;

        public TrendingChartCtl()
        {
            InitializeComponent();
        }

        [Browsable(true)]
        public string XTitle
        {
            get
            {
               // throw new System.NotImplementedException();
                return this.chartMain.ChartAreas["Default"].AxisX.Title;
            }
            set
            {
                this.chartMain.ChartAreas["Default"].AxisX.Title = value;
            }
        }

        public string YTitle
        {
            get
            {
                // throw new System.NotImplementedException();
                return this.chartMain.ChartAreas["Default"].AxisY.Title;
            }
            set
            {
                this.chartMain.ChartAreas["Default"].AxisY.Title = value;
            }
        }

        public string ChartTitle
        {
            get
            {
                return chartMain.Titles[0].Text.ToString();
            }
            set
            {
                chartMain.Titles[0].Text = value;
            }
        }

        public int GapDepthFor3D
        {
            get
            {
                return this.chartMain.ChartAreas["Default"].Area3DStyle.PointGapDepth;
            }
            set
            {
                this.chartMain.ChartAreas["Default"].Area3DStyle.PointGapDepth = value;
            }
        }

        public bool EnableSmartLabel
        {
            get 
            {
                return m_enableSmartLabel;
            }
            set 
            {
                m_enableSmartLabel = value;
            }
        }
        public void ClearSeries()
        {
            this.chartMain.Series.Clear(); 
        }

        public void ClearSeriesByName(string seriesName)
        {
            int index = GetSeriesIndex(seriesName);
            if (index>=0)
            {
                this.chartMain.Series.RemoveAt(index);
            }
        }

        public void ClearSeriesByNameStartBy(string nameStartBy)
        {
            if (chartMain.Series == null || chartMain.Series.Count == 0) return;
            string seriesName;
            List<string> namesToBeDeleted = new List<string>();
            for(int i=0; i<chartMain.Series.Count; i++)
            {
                seriesName = chartMain.Series[i].Name;
                if (seriesName.StartsWith(nameStartBy))
                {
                    namesToBeDeleted.Add(seriesName);
                }
            }

            foreach (string name in namesToBeDeleted)
            {
                ClearSeriesByName(name);
            }
           
        }

        private void TrendingChartUserCtl_Resize(object sender, EventArgs e)
        {
            this.chartMain.Left = 0;
            this.chartMain.Top = 0;            
            this.chartMain.Width=this.Width;
            this.chartMain.Height= this.Height;            
            
        }

   
        /// <summary>
        /// initialize cursor X/Y, pointGapDepth before any chart.
        /// </summary>
        /// <param name="gapDepth">pointGapDepth for 3D</param>
        public void InitChart(int gapDepth)  
        {
	        chartMain.ChartAreas["Default"].CursorX.LineColor = Color.Black;
            chartMain.ChartAreas["Default"].CursorX.LineWidth = 0;
            chartMain.ChartAreas["Default"].CursorX.LineDashStyle = ChartDashStyle.DashDot;

            chartMain.ChartAreas["Default"].CursorY.LineColor = Color.Black;
            chartMain.ChartAreas["Default"].CursorY.LineWidth = 0;
            chartMain.ChartAreas["Default"].CursorY.LineDashStyle = ChartDashStyle.DashDot;

            chartMain.ChartAreas["Default"].CursorX.SelectionColor = Color.FromArgb(120, 253, 215, 34);
            chartMain.ChartAreas["Default"].CursorY.SelectionColor = Color.FromArgb(120, 253, 215, 34);

            chartMain.ChartAreas["Default"].CursorX.Interval = 0;
            chartMain.ChartAreas["Default"].CursorX.IntervalOffset = 0;
            /*chartMain.ChartAreas["Default"].CursorX.Interval = 1;  
            chartMain.ChartAreas["Default"].CursorX.IntervalType = DateTimeIntervalType.Seconds;  
           
            chartMain.ChartAreas["Default"].CursorX.IntervalOffsetType = DateTimeIntervalType.Seconds;  
	        */

            

            chartMain.ChartAreas["Default"].Area3DStyle.PointGapDepth = gapDepth;
        }

        /// <summary>
        ///  initialization before real time charting
        /// </summary>
        /// <param name="frameInterval">second, the frame of X axis.</param>
        /// <param name="timeInterval">ms, the time interval for plotting</param>
        /// <param name="enable3D">whether 3D is enabled</param>
        /// <param name="defaultGapDepth">PointGapDepth for 3D</param>
       public void InitChartBeforeRealTime(int frameInterval, int timeInterval, bool enable3D, int defaultGapDepth )
       {
           chartMain.ChartAreas["Default"].AxisX.ScaleView.ZoomReset(0);
           chartMain.ChartAreas["Default"].AxisY.ScaleView.ZoomReset(0);
            chartMain.ChartAreas["Default"].Area3DStyle.Enable3D = enable3D;
            chartMain.ChartAreas[0].Area3DStyle.PointDepth = 100;

            chartMain.ChartAreas["Default"].AxisX.MinorGrid.Enabled = true;
            chartMain.ChartAreas["Default"].AxisX.LabelStyle.Format = "HH:mm:ss";
            chartMain.ChartAreas["Default"].AxisX.MajorTickMark.LineWidth = 20;
            chartMain.ChartAreas["Default"].AxisX.MinorTickMark.Enabled = true;
            chartMain.ChartAreas["Default"].AxisX.MinorTickMark.TickMarkStyle = System.Windows.Forms.DataVisualization.Charting.TickMarkStyle.OutsideArea;
            chartMain.ChartAreas[0].AxisY.LabelStyle.Format = "0.000";
                
		    DateTime minValue = DateTime.Now;
            DateTime maxValue = minValue.AddSeconds(frameInterval);

            chartMain.ChartAreas[0].AxisX.Minimum = minValue.ToOADate();
            chartMain.ChartAreas[0].AxisX.Maximum = maxValue.ToOADate();
            chartMain.ChartAreas["Default"].Area3DStyle.PointDepth = 1;

            m_realtimeCount = 0;
            if (frameInterval * 5000 / (timeInterval * 60) >10) 
            {
                chartMain.ChartAreas["Default"].Area3DStyle.PointGapDepth = 1000;
            }else 
            {
                chartMain.ChartAreas["Default"].Area3DStyle.PointGapDepth = defaultGapDepth * (frameInterval * 5000 / (timeInterval * 60));
            }

            //this is to make the axis show even when the series has no datapoint added, but no need to change now.
            //chartMain.ChartAreas[0].AxisY.Enabled = AxisEnabled.True;
            //chartMain.ChartAreas[0].AxisX.Enabled = AxisEnabled.True;

            chartMain.Series.Clear();
        }

        /// <summary>
        /// enable zooming of the chart
        /// </summary>
        public void EnableZooming()
        {

            chartMain.ChartAreas["Default"].AxisX.ScaleView.Zoomable = true;
            chartMain.ChartAreas["Default"].AxisY.ScaleView.Zoomable = true;
            // Enable range selection and cursor user interface
            chartMain.ChartAreas["Default"].CursorX.IsUserEnabled = true;
            chartMain.ChartAreas["Default"].CursorX.IsUserSelectionEnabled = true;
            chartMain.ChartAreas["Default"].CursorY.IsUserEnabled = true;
            chartMain.ChartAreas["Default"].CursorY.IsUserSelectionEnabled = true;


        }

       /// <summary>
       /// disable zooming
       /// </summary>
       public void DisableZooming() 
       {
           chartMain.ChartAreas["Default"].AxisX.ScaleView.Zoomable = false;
           chartMain.ChartAreas["Default"].AxisY.ScaleView.Zoomable = false;
           chartMain.ChartAreas["Default"].CursorX.IsUserEnabled = false;
            chartMain.ChartAreas["Default"].CursorX.IsUserSelectionEnabled = false;
            chartMain.ChartAreas["Default"].CursorY.IsUserEnabled = false;
            chartMain.ChartAreas["Default"].CursorY.IsUserSelectionEnabled = false;
       }

        /// <summary>
        /// clear all the series and markers on the chart
        /// </summary>
       public void ClearAllSeriesAndMarkers()
        {
            chartMain.ChartAreas["Default"].AxisY.StripLines.Clear();
            chartMain.Series.Clear();

           //should recalculate axis scales, otherwise the old axis min/max value will used in next time charting
           chartMain.ChartAreas["Default"].RecalculateAxesScale();
        }

        /// <summary>
        /// add a marker to the chart
        /// </summary>
        /// <param name="markerName">marker title </param>
        /// <param name="markerWidth">marker width</param>
        /// <param name="markerValue">marker start value</param>
        /// <param name="bColor">background color</param>
        /// <param name="fColor">marker title color</param>
       public void AddMarker(string markerName, double markerWidth,double markerValue, Color bColor, Color fColor)
        {
            StripLine stripLine = new StripLine();

            stripLine.Interval = 0;
            stripLine.BorderColor = Color.Black;
            stripLine.BorderWidth = 0;
            stripLine.TextLineAlignment = StringAlignment.Far;
            stripLine.TextLineAlignment = StringAlignment.Far;
            //stripLine.TitleFont = new Font("Microsoft Sans Serif", 10, FontStyle.Bold | FontStyle.Italic);
            stripLine.Font = new Font("Microsoft Sans Serif", 10, FontStyle.Regular);
            stripLine.TextOrientation = 0;

            stripLine.Text = markerName;

            stripLine.StripWidth = markerWidth;
            stripLine.IntervalOffset = markerValue;

            stripLine.BackColor = bColor;
            stripLine.ForeColor = fColor;

            chartMain.ChartAreas["Default"].AxisY.StripLines.Add(stripLine);

            chartMain.AntiAliasing = AntiAliasingStyles.Graphics;
     /*       if (! (markerValue > chartMain.ChartAreas[0].AxisY.Minimum ))
            {
                chartMain.ChartAreas[0].AxisY.Minimum = markerValue;
            }
            if (! (markerValue + markerWidth <chartMain.ChartAreas[0].AxisY.Maximum) )
            {
                chartMain.ChartAreas[0].AxisX.Maximum = markerValue + markerWidth;
            }*/
            
        }

        /// <summary>
        /// add a series to a chart
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="dpType">line type: "Line" or "Spline"</param>
        /// <param name="dpColor">series color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="lineThickness">point size in the series</param>
        /// <param name="equation">formula equation if this series is a formula. if not a formula, it's an empty string</param>
       private void AddSeries(string seriesName, string dpType, Color dpColor, bool lblEnabled, string lblName, int lineThickness, string equation, bool visibleInLegend)
        {
            Series s = new Series();

            s.Name = seriesName;

            switch (dpType)
            {
                case "Area":
                    s.ChartType = SeriesChartType.Area;
                    break;
                case "Bar":
                    s.ChartType = SeriesChartType.Bar;
                    break;
                case "Bubble":
                    s.ChartType = SeriesChartType.Bubble;
                    break;
                case "Column":
                    s.ChartType = SeriesChartType.Column;
                    break;
                case "Line":
                    s.ChartType = SeriesChartType.Line;
                    break;
                case "Spline":
                    s.ChartType = SeriesChartType.Spline;
                    break;
                default:
                    s.ChartType = SeriesChartType.Line;
                    break;
            }

            s.Color = dpColor;
            s.CustomProperties = "LabelStyle=Top";
            s.LabelForeColor = System.Drawing.Color.Blue;
            s.IsValueShownAsLabel = lblEnabled;

            s.BorderWidth = 2;
            s.MarkerSize = lineThickness;
            s.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.DateTime;
            s.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            s.MarkerStyle = System.Windows.Forms.DataVisualization.Charting.MarkerStyle.Diamond;

            //DATA_PT_DT
            s.LegendText = lblName;
            s.IsVisibleInLegend = visibleInLegend;

            //for formula only:
            if(!equation.Equals(""))
            {
                s.CustomProperties = "formula=" + equation;
            }

            chartMain.Series.Add(s);
            chartMain.Series[seriesName].ToolTip = "#VALY,#VALX{dd-MM-yyyy HH:mm:ss}";
            chartMain.Series[seriesName].SmartLabelStyle.Enabled = m_enableSmartLabel;

        }

        /// <summary>
        /// set tooltip for a series
        /// </summary>
        /// <param name="seriesName">the series name to be set tooltip</param>
        /// <param name="tooltip">the tooltip format to be set</param>
        public void SetToolTip(string seriesName, string tooltip)
        {
            chartMain.Series[seriesName].ToolTip = tooltip; // "#VALY,#VALX"; 
        }   

        /// <summary>
        /// add a history series
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="dpType">line type, "Line" or "Spline"</param>
        /// <param name="dpColor">line color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="lineThickness">point size</param>
        public void AddHistorySeries(string seriesName, string dpType, Color dpColor, bool lblEnabled, string lblName, int lineThickness, bool visibleInLegend)
        {
            AddSeries(seriesName, dpType, dpColor, lblEnabled, lblName, lineThickness, "", visibleInLegend);

        }

        /// <summary>
        /// add a point to a historical series
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="timestamp">the timestamp of the new point</param>
        /// <param name="value">the value of the new point</param>
        public void AddHistoryPoint(string seriesName, DateTime timestamp, double value)
        {
           chartMain.Series[seriesName].Points.AddXY(timestamp.ToOADate(), value);  
        }

        /// <summary>
        /// add a realtime series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <param name="dpType">series line type</param>
        /// <param name="dpColor">series color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="lineThickness">point size</param>
        public void AddRealTimeSeries(string seriesName, string dpType, Color dpColor, bool lblEnabled, string lblName, int lineThickness)
        {
            AddSeries(seriesName, dpType, dpColor, lblEnabled, lblName, lineThickness, "", true);
        }

        /// <summary>
        /// add a formula series
        /// </summary>
        /// <param name="dpType">series line type</param>
        /// <param name="dpColor">series color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="dpEquation">formula equation</param>
        /// <param name="lineThickness">point size</param>
        public void AddFormulaSeries(string dpType, Color dpColor, bool lblEnabled, string lblName, string dpEquation, int lineThickness)
        {
            AddSeries(lblName, dpType, dpColor, lblEnabled, lblName, lineThickness, dpEquation, true);
        }

        /// <summary>
        /// number of current series
        /// </summary>
        /// <returns></returns>
        public int GetSeriesCount()
        {
            return chartMain.Series.Count;
        }

        /// <summary>
        /// get all the series names
        /// </summary>
        /// <returns>the series name list</returns>
        public List<string> GetSeriesNames ()
        {
            List<string> seriesNames = new List<string>();
            foreach (Series ptSeries in chartMain.Series)
            {
                seriesNames.Add(ptSeries.Name);
            }

            return seriesNames;
        }

        /// <summary>
        ///  add a point to real time series or formula series.
        /// </summary>
        /// <param name="seriesName">the name of the series to add point</param>
        /// <param name="timeStamp">the timestamp of the new point</param>
        /// <param name="dpValue">the value of the new point</param>
        private void AddPointAndAdjustAxisX(string seriesName, DateTime timeStamp, double dpValue)
        {
            m_realtimeCount++;

            if (m_realtimeCount > GetSeriesCount())
            {
                chartMain.ChartAreas[0].Area3DStyle.PointDepth = 100;
            }
            double frameInterval=0;

            frameInterval = (chartMain.ChartAreas[0].AxisX.Maximum - chartMain.ChartAreas[0].AxisX.Minimum) * 24 * 60 * 60;  //seconds

            if (chartMain.Series.IndexOf(seriesName) <= -1) return;

            Series ptSeries = chartMain.Series[seriesName];//findSeriesByName(seriesName);

            ptSeries.Points.AddXY(timeStamp, dpValue);

            double removeBefore = timeStamp.AddSeconds((double)(frameInterval) * (-1)).ToOADate();

            if (ptSeries.Points.Count > 0)
            {
                
                //we should delete all the old ones(time<new_time - timeframe), instead of the first one. 
                int i = 0;
                int oldCount = ptSeries.Points.Count;
                while (i < oldCount )
                {
                    if (ptSeries.Points[0].XValue < removeBefore)
                    {
                        ptSeries.Points.RemoveAt(0);
                    }else
                    {
                        break;
                    }
                    i++;
                }

//                //remove oldest values to maintain a constant number of data points
//                 if (ptSeries.Points[0].XValue < removeBefore)
//                 {
//                     ptSeries.Points.RemoveAt(0);
//                 }
            }

            chartMain.ChartAreas[0].AxisX.Minimum = ptSeries.Points[0].XValue;
            chartMain.ChartAreas[0].AxisX.Maximum = DateTime.FromOADate(ptSeries.Points[0].XValue).AddSeconds(frameInterval).ToOADate();

            chartMain.Invalidate();
            chartMain.ChartAreas["Default"].RecalculateAxesScale();
        }

        /// <summary>
        /// add a point to a realtime series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <param name="timeStamp">the timestamp of the new point</param>
        /// <param name="dpValue">value of the new point</param>
        public void AddRealTimePoint(string seriesName, DateTime timeStamp, double dpValue)
        {
            AddPointAndAdjustAxisX(seriesName, timeStamp, dpValue);

        }

        /// <summary>
        /// get the formula equation of the series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <returns>return the formula equation if the series is a formula series, else return "".</returns>
        public string GetFormulaEquation(string seriesName)
        {
            if (chartMain.Series.IndexOf(seriesName) > -1)
            {
                return chartMain.Series[seriesName].CustomProperties.ToString();
            }
            else return "";
            
        }


        /// <summary>
        /// add a point to a formula series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <param name="timeStamp">the time stamp of the new point</param>
        /// <param name="value">value of the new point</param>
        public void AddFormulaPoint(string seriesName,DateTime timeStamp, double value )
        {
            AddPointAndAdjustAxisX(seriesName, timeStamp, value);
        }

        /// <summary>
        /// get the index of the series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <returns>the index</returns>
        public int GetSeriesIndex (string seriesName)
        {
            return chartMain.Series.IndexOf(seriesName);
        }

        /// <summary>
        /// change style to 3D or 2D
        /// </summary>
        /// <param name="enable3D">whether to enable 3D style</param>
        public void ChangeStyle(bool enable3D)
        {
            //chartMain.ChartAreas[0].AxisX.View.ZoomReset();
            //chartMain.ChartAreas[0].AxisY.View.ZoomReset();
            chartMain.ChartAreas[0].Area3DStyle.Enable3D = enable3D;
        }

        /// <summary>
        /// init chart before historical charting
        /// </summary>
        /// <param name="xMinValue">minimum value of the x axis</param>
        /// <param name="xMaxValue">maximum value of the y axis</param>
        /// <param name="enable3D">whether to enable 3D</param>
        public void InitChartBeforeHistory(DateTime xMinValue, DateTime xMaxValue, bool enable3D)
        {

            chartMain.ChartAreas[0].AxisX.ScaleView.ZoomReset(0);
            chartMain.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
            chartMain.ChartAreas[0].Area3DStyle.Enable3D = enable3D;
            chartMain.ChartAreas[0].Area3DStyle.PointDepth = 100;

            chartMain.ChartAreas[0].AxisX.LabelStyle.Format = "dd-MM-yyyy HH:mm:ss";
            chartMain.ChartAreas[0].AxisY.LabelStyle.Format = "0.000";
           // chartMain.ChartAreas[0].AxisX.Interval = 1;
           // chartMain.ChartAreas[0].AxisX.IntervalType = DateTimeIntervalType.Seconds;
            chartMain.ChartAreas[0].AxisX.IntervalAutoMode = IntervalAutoMode.VariableCount;


            chartMain.ChartAreas[0].AxisX.Minimum = xMinValue.ToOADate();

            double diff = ( xMaxValue.ToOADate()) - (xMinValue.ToOADate());
            if (diff == 0)
            {
                chartMain.ChartAreas[0].Area3DStyle.PointDepth = 1;
            }
            chartMain.ChartAreas[0].AxisX.Maximum = xMaxValue.ToOADate();

            chartMain.ChartAreas["Default"].AxisX.ScaleView.SmallScrollSize = 2;
            chartMain.ChartAreas["Default"].AxisX.ScaleView.SmallScrollSizeType = DateTimeIntervalType.Seconds;
            EnableZooming();

            //this is to make the axis show even when the series has no datapoint added.
            //chartMain.ChartAreas[0].AxisY.Enabled = AxisEnabled.True;
            //chartMain.ChartAreas[0].AxisX.Enabled = AxisEnabled.True;

            chartMain.Series.Clear();
        }

        /// <summary>
        /// get print document for printing
        /// </summary>
        /// <returns>PrintDocument</returns>
        public PrintDocument GetPrintDoc()
        {
            return chartMain.Printing.PrintDocument;
        }

        /// <summary>
        /// save chart image to file
        /// </summary>
        /// <param name="fileName">the file name with path and the format.</param>
        public void SaveAsImage(string fileName)
        {
            ChartImageFormat format = ChartImageFormat.Bmp;

            if (fileName.EndsWith("bmp"))
            {
                format = ChartImageFormat.Bmp;
            }
            else if (fileName.EndsWith("jpg"))
            {
                format = ChartImageFormat.Jpeg;
            }
            else if (fileName.EndsWith("emf"))
            {
                format = ChartImageFormat.Emf;
            }
            else if (fileName.EndsWith("gif"))
            {
                format = ChartImageFormat.Gif;
            }
            else if (fileName.EndsWith("png"))
            {
                format = ChartImageFormat.Png;
            }
            else if (fileName.EndsWith("tif"))
            {
                format = ChartImageFormat.Tiff;
            }

            // Save image
            chartMain.SaveImage(fileName, format);
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing.Printing;


namespace TrainTimeTableChartCtrl
{
    public partial class TimeTableChartCtrl : UserControl
    {

        Color PlannedColor = Color.Green;
        Color PracticalColor = Color.Purple;
        Color HightlightColor = Color.Blue;

        public TimeTableChartCtrl()
        {
            InitializeComponent();
            InitChart();
          
        }

        public Chart GetChartMain()
        {
            return this.chartMain;
        }

        public void ClearSeries()
        {
            this.chartMain.Series.Clear();
        }

        public void GetChartAreaBoundary(out float XMin, out float XMax, out float YMin, out float YMax)
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = chartMain.ChartAreas["Default"];

            XMin = chartArea1.Position.X;
            XMax = chartArea1.Position.X + chartArea1.Position.Width * chartMain.Width / 100;
            YMin = chartArea1.Position.Y;
            YMax = chartArea1.Position.Y + chartArea1.Position.Height * chartMain.Height / 100;

            return;
        }

        public bool IsChartEmpty()
        {
            SeriesCollection ChartSeries = chartMain.Series;
            if (ChartSeries != null && ChartSeries.Count > 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        /// <summary>
        /// initialize cursor X/Y, pointGapDepth before any chart.
        /// </summary>
        /// <param name="gapDepth">pointGapDepth for 3D</param>
        public void InitChart()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = chartMain.ChartAreas["Default"];

            chartArea1.BackColor = SystemColors.ControlLightLight;
            //chartArea1.CursorX.LineColor = Color.Black;
            //chartArea1.CursorX.LineWidth = 0;
            //chartArea1.CursorX.LineDashStyle = ChartDashStyle.DashDot;

            //chartArea1.CursorY.LineColor = Color.Black;
            //chartArea1.CursorY.LineWidth = 0;
            //chartArea1.CursorY.LineDashStyle = ChartDashStyle.DashDot;

            //chartArea1.CursorX.SelectionColor = Color.FromArgb(120, 253, 215, 34);
            //chartArea1.CursorY.SelectionColor = Color.FromArgb(120, 253, 215, 34);

            chartArea1.Position.Height = 100;
            chartArea1.Position.Width = 100;

            chartArea1.AxisX.IntervalType = DateTimeIntervalType.Number;
            chartArea1.AxisX.Interval = 1;
            chartArea1.AxisX.LabelStyle.IntervalType = DateTimeIntervalType.Number;
            chartArea1.AxisX.LabelStyle.Interval = 1;
            chartArea1.AxisX.LabelStyle.Angle = -90 ;
            //chartArea1.AxisX.IntervalAutoMode = IntervalAutoMode.VariableCount;

            chartArea1.AxisX.ScrollBar.IsPositionedInside = false;
            chartArea1.AxisX.ScrollBar.ButtonColor = SystemColors.ControlLight;
            chartArea1.AxisX.ScrollBar.BackColor = SystemColors.Menu;
            chartArea1.AxisX.ScrollBar.LineColor = SystemColors.ControlDark;
            chartArea1.AxisX.ScrollBar.ButtonStyle = ScrollBarButtonStyles.SmallScroll;
            chartArea1.AxisY2.ScrollBar.IsPositionedInside = false;
            chartArea1.AxisY2.ScrollBar.ButtonColor = SystemColors.ControlLight;
            chartArea1.AxisY2.ScrollBar.BackColor = SystemColors.Menu;
            chartArea1.AxisY2.ScrollBar.LineColor = SystemColors.ControlDark;
            chartArea1.AxisY2.ScrollBar.ButtonStyle = ScrollBarButtonStyles.SmallScroll;
            


        }

        /// <summary>
        /// clear all the series and markers on the chart
        /// </summary>
        public void ClearAllSeriesAndMarkers()
        {
            chartMain.ChartAreas["Default"].AxisY2.StripLines.Clear();
            chartMain.Series.Clear();
           
            //should recalculate axis scales, otherwise the old axis min/max value will used in next time charting
            chartMain.ChartAreas["Default"].RecalculateAxesScale();
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
            s.CustomProperties = "LabelStyle=Right";
            
            s.LabelForeColor = System.Drawing.Color.Blue;
            s.IsValueShownAsLabel = false;

            s.BorderWidth = 2;
            s.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.DateTime;
            s.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.String;
                    
            s.LegendText = lblName;
            s.IsVisibleInLegend = visibleInLegend;
            s.YValuesPerPoint =  32;
            s.SmartLabelStyle.Enabled = true;
            //s.SmartLabelStyle.IsMarkerOverlappingAllowed = false;
            
            s.YAxisType = AxisType.Secondary;
        
            //for formula only:
            if (!equation.Equals(""))
            {
                s.CustomProperties = "formula=" + equation;
            }

            chartMain.Series.Add(s);
            chartMain.Series[seriesName].ToolTip = "#VALY{HH:mm:ss},#VALX";
        }

        /// <summary>
        /// add a planned series
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="dpType">line type, "Line" or "Spline"</param>
        /// <param name="dpColor">line color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="lineThickness">point size</param>
        public void AddPlannedSeries(string seriesName, string dpType, bool lblEnabled, string lblName, int lineThickness, bool visibleInLegend)
        {
            AddSeries(seriesName, dpType, PlannedColor, lblEnabled, lblName, lineThickness, "", visibleInLegend);

        }

        /// <summary>
        /// add a practical series
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="dpType">line type, "Line" or "Spline"</param>
        /// <param name="dpColor">line color</param>
        /// <param name="lblEnabled">whether to show label as value</param>
        /// <param name="lblName">series legend name</param>
        /// <param name="lineThickness">point size</param>
        public void AddPracticalSeries(string seriesName, string dpType,  bool lblEnabled, string lblName, int lineThickness, bool visibleInLegend)
        {
            AddSeries(seriesName, dpType, PracticalColor, lblEnabled, lblName, lineThickness, "", visibleInLegend);

        }

        /// <summary>
        /// add a point to a series
        /// </summary>
        /// <param name="seriesName">series name</param>
        /// <param name="timestamp">the timestamp of the new point</param>
        /// <param name="value">the value of the new point</param>
        public void AddPoint(string seriesName, int stationID,string stationName, DateTime timestamp)
        {
   
            DataPoint dp = new DataPoint(stationID,timestamp.ToOADate());
            
            dp.AxisLabel = stationName;
           
            chartMain.Series[seriesName].Points.Add(dp);
         
        }

        /// <summary>
        /// Scroll Chart to Plot Area With Series
        /// </summary>
        public void ScrollToPlotArea()
        {
            Double MinSeriesYValue = chartMain.ChartAreas["Default"].AxisY2.Maximum;
            //Find the Minimum Y Value of All the Series
            foreach (Series S in chartMain.Series)
            {
                foreach (DataPoint Dp in S.Points)
                {
                    if (MinSeriesYValue > Dp.YValues[0])
                    {
                        MinSeriesYValue = Dp.YValues[0];
                    }
                }
            }

            chartMain.ChartAreas[0].AxisY2.ScaleView.Scroll(MinSeriesYValue);
            SetScale();
        }

        /// <summary>
        /// Set Label on the fist and last data point within time range.
        /// </summary>
        public void SetSeriesLabel()
        {
            DateTime StartTime = DateTime.FromOADate(chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum);
            DateTime EndTime = DateTime.FromOADate(chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum);

           foreach( Series s in this.chartMain.Series)
           {
                
                String SeriesLabel;
                Color LabelColor;
                //set label on datapoint
                if (s.Name.StartsWith("Planned"))
                {              
                    SeriesLabel = s.Name.Substring(7);
                    LabelColor = PlannedColor;
                }
                else
                {
                    SeriesLabel = s.Name.Substring(9);
                    LabelColor = PracticalColor;
                }
               //remove any existing data point labels
                foreach (DataPoint DP in s.Points)
                {
                    DP.Label = "";
                }

                //set first label, within time range
                bool firstLblSet = false;
                int dpIndex = 0;
                
                while(!firstLblSet && s.Points.Count > dpIndex)
                {   
                    DataPoint DP = s.Points[dpIndex];
                    DateTime YValue =  DateTime.FromOADate(DP.YValues[0]);
                    if(YValue.CompareTo(StartTime) == 1)
                    {   
                        if (dpIndex == 0 && s.Points.Count >= 2 )                        
                        {
                            //check if second datapoint is within time range
                            DataPoint DP2 = s.Points[1];
                            DateTime YValue2 =  DateTime.FromOADate(DP2.YValues[0]);
                            if (YValue2.CompareTo(StartTime) == 1 && YValue2.CompareTo(EndTime) == -1)
                            {
                                DP2.Label = SeriesLabel;
                                DP2.LabelForeColor = LabelColor;
                            }
                            //label first
                            else
                            {
                                DP.Label = SeriesLabel;
                                DP.LabelForeColor = LabelColor;
                            }
                        }
                        else
                        {
                            DP.Label = SeriesLabel;
                            DP.LabelForeColor = LabelColor;
                        }
                        
                        firstLblSet = true;
                    }
                    dpIndex++;
                }

                //set last label, within time range
                bool lastLblSet = false;
                dpIndex = s.Points.Count - 1;
                while (!lastLblSet && dpIndex >= 0)
                {
                    DataPoint DP = s.Points[dpIndex];
                    DateTime YValue = DateTime.FromOADate(DP.YValues[0]);
                    if (YValue.CompareTo(EndTime) == -1)
                    {

                        if (dpIndex == s.Points.Count - 1 && s.Points.Count >= 2)
                        {
                            //check if last but one DP is within time range
                            DataPoint DPLastButOne = s.Points[dpIndex - 1];
                            DateTime YValueLastButOne = DateTime.FromOADate(DPLastButOne.YValues[0]);
                            if (YValueLastButOne.CompareTo(StartTime) == 1 && YValueLastButOne.CompareTo(EndTime) == -1)
                            {
                                DPLastButOne.Label = SeriesLabel;
                                DPLastButOne.LabelForeColor = LabelColor;
                            }
                            //label first
                            else
                            {
                                DP.Label = SeriesLabel;
                                DP.LabelForeColor = LabelColor;
                            }
                        }
                        else
                        {
                            DP.Label = SeriesLabel;
                            DP.LabelForeColor = LabelColor;
                        }
                        lastLblSet = true;
                    }
                    dpIndex--;
                }

           }
        }

        public void SetScale()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = chartMain.ChartAreas["Default"];

            //round down view minimum and round up view maximum to the nearest minute

            DateTime StartTime = DateTime.FromOADate(chartArea1.AxisY2.ScaleView.ViewMinimum);
            DateTime EndTime = DateTime.FromOADate(chartArea1.AxisY2.ScaleView.ViewMaximum);

            StartTime = StartTime.Date.AddHours(StartTime.Hour).AddMinutes(StartTime.Minute);            
            EndTime = (EndTime.AddSeconds((60 - EndTime.Second) % 60).AddMilliseconds(-EndTime.Millisecond));

            //calculate Viewsize after rounding
            double viewSize = (EndTime - StartTime).TotalSeconds;

            //if view size after Rounding is more than scaleView size, set view size to scaleView Size
            if (viewSize > chartArea1.AxisY2.ScaleView.Size)
                viewSize = chartArea1.AxisY2.ScaleView.Size;

            //set chart to the new view size
            chartArea1.AxisY2.ScaleView.Zoom(StartTime.ToOADate(), viewSize, DateTimeIntervalType.Seconds, false);

            //obtain the new scale view minimum and maximum
            StartTime = DateTime.FromOADate(chartArea1.AxisY2.ScaleView.ViewMinimum);
            EndTime = DateTime.FromOADate(chartArea1.AxisY2.ScaleView.ViewMaximum);


            //if EndTime is 23:59:59 round EndTime to 0:0:0 for calculation purposes
            if (EndTime.Hour == 23 && EndTime.Minute == 59 && EndTime.Second == 59)
            {
                EndTime = EndTime.AddSeconds(1);
            }

            DateTime ChartStartTime = DateTime.FromOADate(chartArea1.AxisY2.Minimum);
            double Interval = chartArea1.AxisY2.MajorTickMark.Interval;
            double MajorInterval = chartArea1.AxisY2.MajorGrid.Interval;

            //calculate interval offset for the new scale view
            double TimeDiff = (StartTime - ChartStartTime).TotalSeconds;
            double IntervalOffset = (MajorInterval - TimeDiff) % MajorInterval;

            chartArea1.AxisY2.LabelStyle.IntervalOffset = IntervalOffset;
            chartArea1.AxisY2.MajorTickMark.IntervalOffset = (IntervalOffset % (double)Interval);
            chartArea1.AxisY2.MajorGrid.IntervalOffset = IntervalOffset;
        }

     
        public void SetChartFocus()
        {
            chartMain.Focus();
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
        public List<string> GetSeriesNames()
        {
            List<string> seriesNames = new List<string>();
            foreach (Series ptSeries in chartMain.Series)
            {
                seriesNames.Add(ptSeries.Name);
            }

            return seriesNames;
        }

        /// <summary>
        /// get the index of the series
        /// </summary>
        /// <param name="seriesName">the series name</param>
        /// <returns>the index</returns>
        public int GetSeriesIndex(string seriesName)
        {
            return chartMain.Series.IndexOf(seriesName);
        }

        /// <summary>
        /// init chart before historical charting
        /// </summary>
        /// <param name="xMinValue">minimum value of the x axis</param>
        /// <param name="xMaxValue">maximum value of the y axis</param>
        /// <param name="enable3D">whether to enable 3D</param>
        public void InitChartBefore(DateTime yMinValue, DateTime yMaxValue, int Interval, int NumMinorGridInterval, int NumMajorGridInterval)
        {   
           
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = chartMain.ChartAreas["Default"];


            while (chartArea1.AxisX.ScaleView.IsZoomed)
                chartArea1.AxisX.ScaleView.ZoomReset();

            while (chartArea1.AxisY2.ScaleView.IsZoomed)
                chartArea1.AxisY2.ScaleView.ZoomReset();

            //set minimum time, round down to the nearest minute
            DateTime ChartYMin = yMinValue.Date.AddHours(yMinValue.Hour).AddMinutes(yMinValue.Minute);
            chartArea1.AxisY2.Minimum = ChartYMin.ToOADate();
            //set maximum time, round up to the nearest minute
            DateTime ChartYMax = (yMaxValue.AddSeconds((60 - yMaxValue.Second)%60).AddMilliseconds(-yMaxValue.Millisecond));
            
            //if end time is 23:59 set maxium value of y axis to 23:59:59
            if (yMaxValue.Hour == 23 && yMaxValue.Minute == 59)
            {
                chartArea1.AxisY2.Maximum = (yMaxValue.Date.AddHours(23).AddMinutes(59).AddSeconds(59)).ToOADate();
            }
            else
            {
                chartArea1.AxisY2.Maximum = ChartYMax.ToOADate();
            }

            chartArea1.AxisY2.IntervalType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.IntervalOffsetType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.LabelStyle.IntervalType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.LabelStyle.IntervalOffsetType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.MajorTickMark.IntervalType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.MajorTickMark.IntervalType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.MajorGrid.IntervalType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.MajorGrid.IntervalOffsetType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.ScaleView.SizeType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.ScaleView.MinSizeType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.ScaleView.SmallScrollSizeType = DateTimeIntervalType.Seconds;
            chartArea1.AxisY2.ScaleView.SmallScrollMinSizeType = DateTimeIntervalType.Seconds;

            //convert Interval to seconds
            Interval *= 60;
                          
            int TimeRange = (int)((ChartYMax - ChartYMin).TotalSeconds);

            int MajorInterval = NumMinorGridInterval * Interval;
            
            chartArea1.AxisY2.MajorTickMark.Interval = Interval;
            chartArea1.AxisY2.LabelStyle.Interval = MajorInterval;
            chartArea1.AxisY2.MajorGrid.Interval = MajorInterval;
            chartArea1.AxisY2.ScaleView.MinSize = MajorInterval;

            chartArea1.AxisY2.MajorTickMark.IntervalOffset = 0.0;
            chartArea1.AxisY2.LabelStyle.IntervalOffset = 0.0;
            chartArea1.AxisY2.MajorGrid.IntervalOffset = 0.0;
           
           
            if (TimeRange >= (MajorInterval * NumMajorGridInterval))
            {
                chartArea1.AxisY2.ScaleView.Size = MajorInterval * NumMajorGridInterval;
            }

            else
            {
                chartArea1.AxisY2.ScaleView.Size = TimeRange;
            }
         
            chartArea1.AxisY2.ScaleView.SmallScrollMinSize = Interval;
            chartArea1.AxisY2.ScaleView.SmallScrollSize = MajorInterval;
           
          
            chartArea1.AxisY2.IsStartedFromZero = false;

            chartMain.Series.Clear();
        }


        public void DisableScrollBar()
        {
            chartMain.ChartAreas["Default"].AxisX.ScrollBar.Enabled = false;
            chartMain.ChartAreas["Default"].AxisY2.ScrollBar.Enabled = false;
        }

        public void EnableScrollBar()
        {
            chartMain.ChartAreas["Default"].AxisX.ScrollBar.Enabled = true;
            chartMain.ChartAreas["Default"].AxisY2.ScrollBar.Enabled = true;
        }
        
        /// <summary>
        /// get print document for printing
        /// </summary>
        /// <returns>PrintDocument</returns>
        public PrintDocument GetPrintDoc()
        {   
            DisableScrollBar();
            PrintDocument printDoc =  chartMain.Printing.PrintDocument;
            EnableScrollBar();
            return printDoc;
        }

        /// <summary>
        /// save chart image to file
        /// </summary>
        /// <param name="fileName">the file name with path and the format.</param>
        public void SaveAsImage(string fileName)
        {
            DisableScrollBar();
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

            EnableScrollBar();


        }

        private void TrainTimeTableChartCtrl_Resize(object sender, EventArgs e)
        {
            this.chartMain.Left = 0;
            this.chartMain.Top = 0;
            this.chartMain.Width = this.Width;
            this.chartMain.Height = this.Height;

        }

        ///<summary>
        /// Get Series Name
        /// </summary>
        public string GetSeriesName(System.Windows.Forms.MouseEventArgs e)
        {
            // Call Hit Test Method
            HitTestResult result = this.chartMain.HitTest(e.X, e.Y);
            Series s = result.Series;
            if(s != null)
            {
                return s.Name;
            }

            return "";
            
        }

        ///<summary>
        /// Hightlight Sereis
        /// </summary>
        /// /// <param name="Series Name">Series Name to Highlight.</param>
        public void HightlightSeries(String SeriesName)
        {
            Series s = this.chartMain.Series.FindByName(SeriesName);
            if (s != null)
            {
                s.Color = HightlightColor;
            }

        }

        ///<summary>
        /// RemoveHightlight Sereis
        /// </summary>
        /// <param name="Series Name">Series Name to Remove Highlight.</param>
        public void RemoveHightlightSeries(String SeriesName)
        {
            Series s = this.chartMain.Series.FindByName(SeriesName); 
            if(s != null)
            {
                if (s.Name.StartsWith("Planned"))
                {
                    s.Color = PlannedColor;
                }
                else
                {
                    s.Color = PracticalColor;
                }
            }

        }

        ///<summary>
        /// Zoom in
        /// </summary>
        /// <param name="X">the X coordinates.</param>
        /// <param name="Y">the Y coordinates.</param>
        public void Zoom(double X, double Y)
        {

            double xMin = chartMain.ChartAreas["Default"].AxisX.ScaleView.ViewMinimum;
            double xMax = chartMain.ChartAreas["Default"].AxisX.ScaleView.ViewMaximum;
            double yMin = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum;
            double yMax = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum;

            double posXStart = (int)(chartMain.ChartAreas["Default"].AxisX.PixelPositionToValue(X)) - (xMax - xMin) / 4;
            double posXFinish = (int)(chartMain.ChartAreas["Default"].AxisX.PixelPositionToValue(X)) + (xMax - xMin) / 4;
            double posYStart = chartMain.ChartAreas["Default"].AxisY2.PixelPositionToValue(Y) - (yMax - yMin) / 4;
            double posYFinish = chartMain.ChartAreas["Default"].AxisY2.PixelPositionToValue(Y) + (yMax - yMin) / 4;

            //set view start to nearest min.
            DateTime posYStartDate = (DateTime.FromOADate(posYStart));
            posYStartDate = posYStartDate.Date.AddHours(posYStartDate.Hour).AddMinutes(posYStartDate.Minute);
            posYStart = posYStartDate.ToOADate();

            //set view end to nearest min.
            DateTime posYFinishDate = (DateTime.FromOADate(posYFinish));
            posYFinishDate = posYFinishDate.Date.AddHours(posYFinishDate.Hour).AddMinutes(posYFinishDate.Minute);
            posYFinish = posYFinishDate.ToOADate();

            double viewSize = (DateTime.FromOADate(posYFinish) - DateTime.FromOADate(posYStart)).TotalSeconds;
            //Zoom in 
            chartMain.ChartAreas["Default"].AxisX.ScaleView.Zoom(Math.Round(posXStart), Math.Round(posXFinish - posXStart), DateTimeIntervalType.Number, true);
            chartMain.ChartAreas["Default"].AxisY2.ScaleView.Zoom(posYStart, viewSize, DateTimeIntervalType.Seconds, true);

            double yMinNew = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum;
            double yMaxNew = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum;

            DateTime minDateTime = DateTime.FromOADate(yMinNew);
            DateTime maxDateTime = DateTime.FromOADate(yMaxNew);
            //SetLabel
            SetSeriesLabel();
            SetScale();

        }

       
        ///<summary>
        /// Zoom in
        /// </summary>
        public void Zoom()
        {

            //double xMin = chartMain.ChartAreas["Default"].AxisX.ScaleView.ViewMinimum;
            //double xMax = chartMain.ChartAreas["Default"].AxisX.ScaleView.ViewMaximum;
            double yMin = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum;
            double yMax = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum;

            //double posXStart = chartMain.ChartAreas["Default"].AxisX.PixelPositionToValue(X) - (xMax - xMin) / 4;
            //double posXFinish = chartMain.ChartAreas["Default"].AxisX.PixelPositionToValue(X) + (xMax - xMin) / 4;

            double Center = (yMax + yMin) / 2.0;
            double posYStart =  Center  - (yMax - yMin) / 4.0;
            double posYFinish = Center  + (yMax - yMin) / 4.0;


            double viewSize = (DateTime.FromOADate(posYFinish) - DateTime.FromOADate(posYStart)).TotalMinutes;
            //chartMain.ChartAreas["Default"].AxisY2.ScaleView.Zoom(posYStart, posYFinish);
            chartMain.ChartAreas["Default"].AxisY2.ScaleView.Zoom(posYStart, viewSize, DateTimeIntervalType.Minutes, true);
        }

        ///<summary>
        /// Zoom out
        /// </summary>
        /// <param name="MaxZooInterval">Maximum Zoom out Level.</param>
        public void ZoomOut(int MaxZoomInterval)
        {
            //Reset X Axis
            chartMain.ChartAreas["Default"].AxisX.ScaleView.ZoomReset(1);  


            //Reset Y Axis
            double yMin = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum;
            double yMax = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum;

           
            double ViewSize = (DateTime.FromOADate(yMax) - DateTime.FromOADate(yMin)).TotalMinutes;


            if (ViewSize < MaxZoomInterval)
            {
                chartMain.ChartAreas["Default"].AxisY2.ScaleView.ZoomReset(1);

                
            }

            double yMinNew = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMinimum;
            double yMaxNew = chartMain.ChartAreas["Default"].AxisY2.ScaleView.ViewMaximum;

            DateTime minDateTime = DateTime.FromOADate(yMinNew);
            DateTime maxDateTime = DateTime.FromOADate(yMaxNew);
            //SetLabel
            SetSeriesLabel();
            SetScale();

        }


          

    }
}

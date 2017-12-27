using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using STEE.ISCS.GenericGUI;
using TrainTimeTableViewer.Common;
using TrainTimeTable;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.View
{
    public partial class TimeTableWindow : BaseForm
    {

        private const string CLASS_NAME = "TrainTimeTableViewer.View.TimeTableWindow";
        public TimeTableWindow()
        {
            InitializeComponent();
            InitializeRowHeight();
            DisableColumnSorting();
        }

        /// <summary>
        /// Initialise Row Height Based on the Number Of Rows to be Displayed;
        /// </summary>
        public void InitializeRowHeight()
        {
            
            int NumOfRows = TrainTimeTableConst.NUM_ROWS_TRAINTIMETABLE_WINDOW;   
            int DataGridHeight = timeTableDataGridView.Height;
            int DisplayAreaHeight = DataGridHeight - timeTableDataGridView.ColumnHeadersHeight;

            timeTableDataGridView.RowTemplate.Height = DisplayAreaHeight / NumOfRows;
        
        }

        /// <summary>
        /// Disable Column Sorting
        /// </summary>
        public void DisableColumnSorting()
        {
            foreach (DataGridViewColumn column in timeTableDataGridView.Columns)
            {
                column.SortMode = DataGridViewColumnSortMode.NotSortable;
            }
        }

        public void Initialise(String SeriesName, bool Planned)
        {
            string FUNCTION_NAME = "Initialise";
            
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            this.Text = SeriesName;

            if (Planned)
            {
                this.ArrTime.HeaderText = "计划列车到站时间";
                this.DeptTime.HeaderText = "计划列车离站时间";
            }
            else
            {
                this.ArrTime.HeaderText = "实际列车到站时间";
                this.DeptTime.HeaderText = "实际列车离站时间";
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Exited.");

        }

       
        public void AddTrainTimings(List<TrainTimeTableData> TimeTableData)
        {
            string FUNCTION_NAME = "AddTrainTimings";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Entered.");

            foreach (TrainTimeTableData data in TimeTableData)
            {
                this.timeTableDataGridView.Rows.Add(data.StationName, data.ArrTime.TimeOfDay, data.DeptTime.TimeOfDay);
            }


            //add empty columns if need
            int NumOfRows = TrainTimeTableConst.NUM_ROWS_TRAINTIMETABLE_WINDOW;         
            int TotalRows = this.timeTableDataGridView.Rows.Count;
            
            if (NumOfRows > TotalRows)
            {
                this.timeTableDataGridView.Rows.Add(NumOfRows - TotalRows);
            }

          
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(), EDebugLevelManaged.DebugInfo, "Function Exited.");

        }


    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using STEE.ISCS.MVC;
using TrainTimeTable;
using DAO.TimeTable;
using DAO.TimeTable.Common;
using NET_ISCS_Bridge;

namespace TrainTimeTableViewer.Model 
{
    class TimeTableViewModel : IModel
    {
        private const string CLASS_NAME = "TimeTableViewModel";
        private string  m_Exclude_StationId_List = null;
        /// <summary>
        /// Returns whether oracle connection is opened or not
        /// </summary>
        /// <param name="connectionString">Connection string</param>
        /// <returns>true - connected, false - failed</returns>
        public bool OpenConnection()
        {
            // m_locationConnectionString = connectionString;
            return SimpleDatabase.GetInstance().IsDatabaseConnectable();
        }

        public string ExcludeStationIdList
        {
            get { return m_Exclude_StationId_List; }
            set { m_Exclude_StationId_List = value; }
        }

        public void ClearDataCache()
        {
            string FUNCTION_NAME = "ClearDataCache";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            TrainTimeTableDAO.ClearCache();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Exited.");

        }

        public Dictionary<int,List<TrainTimeTableData>> GetTimeTableData(bool plannedData, DateTime minTime, DateTime maxTime)
        {
            string FUNCTION_NAME = "GetTimeTableData";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Entered.");

            TrainTimeTableDAO timetableDAO = TrainTimeTableDAO.GetInstance();

            //set exclude station id list
            TrainTimeTableDAO.SetExcludeStationList(this.ExcludeStationIdList);
            
            Dictionary<int,List<TrainTimeTableData>> timeTableDataRes=  new Dictionary<int,List<TrainTimeTableData>>();
            Dictionary<int, List<TrainTimeTableData>> ResWithinTimeRange = new Dictionary<int, List<TrainTimeTableData>>();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
              EDebugLevelManaged.DebugInfo, "Getting Data, StarTime " + minTime.ToShortDateString() + " End Time " + maxTime.ToShortDateString());

            //get all train schedule for the particular day
            timeTableDataRes = timetableDAO.GetTimeTableData(plannedData, minTime.Date);

            if (timeTableDataRes.Count == 0)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "No Data Avaible for the Specified Date");
            }
            //remove TrainServiceIds not within time range          
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Remove trains Ids not with the time range.");

            foreach (KeyValuePair<int, List<TrainTimeTableData>> TimeTblData in timeTableDataRes)
            {   
                //only add if the first arrival time is less than maxTime and 
                // the last depature time is more than minTime
                if (((TimeTblData.Value.First().ArrTime.CompareTo(maxTime) <= 0) &&
                    (TimeTblData.Value.Last().DeptTime.CompareTo(minTime) >= 0)))
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Train Id" + TimeTblData.Key.ToString() + "added");

                    ResWithinTimeRange.Add(TimeTblData.Key, TimeTblData.Value);
                }

                else
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Train Id" + TimeTblData.Key.ToString() + "Removed");
                }
               
            }

            if (ResWithinTimeRange.Count == 0)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
               EDebugLevelManaged.DebugInfo, "No Train Within the Time Range.");
            }
            
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME +"."+ FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function Exited.");
           

            return ResWithinTimeRange;
            
        }

    }
}

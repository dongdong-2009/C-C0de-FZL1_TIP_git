using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using DAO.TimeTable.Common;
using DAO.TimeTable.Helper;
using TrainTimeTable;
using NET_ISCS_Bridge;

namespace DAO.TimeTable
{
    public class TrainTimeTableDAO
    {
        private static TrainTimeTableDAO m_instance = null;
        private static string m_ExcludeStationList = "-1";
        private static Dictionary<int, List<TrainTimeTableData>> m_TrainDataCachePlanned = null;
        private static Dictionary<int, List<TrainTimeTableData>> m_TrainDataCachePractical = null;

        private const string CLASS_NAME = "DAO.TimeTable.TrainTimeTableDAO";

        public static TrainTimeTableDAO GetInstance()
        {
            string FUNCTION_NAME = "GetInstance";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Entered"); 

            if (m_instance == null)
            {
                m_instance = new TrainTimeTableDAO();
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited"); 
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            string FUNCTION_NAME = "ReleaseInstance";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 
            if (m_instance != null)
            {
                m_instance = null;
            }
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited"); 
        }

        public static void ClearCache()
        {
            string FUNCTION_NAME = "ClearCache";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 

            m_TrainDataCachePractical = null;
            m_TrainDataCachePlanned = null;

           
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 
        }

        public static void SetExcludeStationList(string ExcludeStationId)
        {
            string FUNCTION_NAME = "SetExcludeStationList";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 

            if(String.IsNullOrEmpty(ExcludeStationId))
            {
                m_ExcludeStationList = "-1";

                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Exclude Station List is empty"); 
            }
            else
            {
                m_ExcludeStationList = ExcludeStationId;

                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Exclude Station List - " + m_ExcludeStationList); 
            }

          
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 
            
        }

        /// <summary>
        /// get timetable data
        /// </summary>    
        /// <param name="planned">planned</param>
        /// <param name="Date">date</param>
        /// <returns>train time table</returns>

        public Dictionary<int, List<TrainTimeTableData>> GetTimeTableData(bool isPlanned, DateTime Date)
        {
            string FUNCTION_NAME = "GetTrainTimeTableData";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Entered"); 

            List<TrainTimeTableData> res = new List<TrainTimeTableData>();
            string localSQL;


           //clear cache
            if (isPlanned)
            {
                m_TrainDataCachePlanned = null;
                
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Clear Planned Cache"); 
            }
            else
            {
                m_TrainDataCachePractical = null;

                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Clear Practical Cache"); 
            }


            if (isPlanned)
            {
                localSQL = " SELECT SERVICE_ID,ORDER_ID as STATION_ORDER_ID,A_TIME,D_TIME, DISPLAY_NAME " +
                       " FROM ATS_PLANNED_TRAIN_DATA, LOCATION WHERE " +
                       " D_TIME >= @StartDateValue" +
                       " AND A_TIME <= @EndDateValue" +
                       " AND STATION_ID = PKEY" +
                       " AND STATION_ID NOT IN ( " + m_ExcludeStationList +
                       " ) ORDER BY SERVICE_ID, A_TIME";
            }
            else
            {
                localSQL = " SELECT SERVICE_ID,ORDER_ID as STATION_ORDER_ID,A_TIME,D_TIME, DISPLAY_NAME" +
                      " FROM ATS_PRACTICAL_TRAIN_DATA, LOCATION WHERE " +                
                      " D_TIME >= @StartDateValue" +
                      " AND A_TIME <= @EndDateValue" +
                      " AND STATION_ID = PKEY" +
                      " AND STATION_ID NOT IN ( " + m_ExcludeStationList +
                      " ) ORDER BY SERVICE_ID, A_TIME";
            }
            


            List<SqlParameter> parameters = new List<SqlParameter>();
          
            SqlParameter parameter1 = new SqlParameter();
            parameter1.ParameterName = "StartDateValue";
            parameter1.DbType = DbType.DateTime;

            parameter1.Value = Date;
            parameter1.Direction = System.Data.ParameterDirection.Input;
            parameters.Add(parameter1);

            SqlParameter parameter2 = new SqlParameter();
            parameter2.ParameterName = "EndDateValue";
            parameter2.DbType = DbType.DateTime;

            parameter2.Value = Date.AddHours(23).AddMinutes(59).AddSeconds(59);
            parameter2.Direction = System.Data.ParameterDirection.Input;
            parameters.Add(parameter2);

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQueryWithParams(localSQL, parameters);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    TrainTimeTableData timeTableData = new TrainTimeTableData();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("A_TIME")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["A_TIME"].ToString(), out dt))
                            timeTableData.ArrTime = dt;
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("D_TIME")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["D_TIME"].ToString(), out dt))
                            timeTableData.DeptTime = dt;
                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("SERVICE_ID")))
                    {
                        timeTableData.TrainServiceId = Convert.ToInt32(drReader["SERVICE_ID"]);
                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("STATION_ORDER_ID")))
                    {
                        timeTableData.StationID = Convert.ToInt32(drReader["STATION_ORDER_ID"]);

                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DISPLAY_NAME")))
                    {
                        timeTableData.StationName = Convert.ToString(drReader["DISPLAY_NAME"]);

                    }

                    res.Add(timeTableData);
                }
            }
            catch (System.Exception ex)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, ex.ToString()); 
            }

            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            //Store in Cache
            if (isPlanned)
            {
                m_TrainDataCachePlanned = CreateTrainDataDictionary(res);
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Store Planned Cache"); 
            }
            else
            {
                m_TrainDataCachePractical = CreateTrainDataDictionary(res);
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Store Practical Cache"); 
            }


           LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 

            //return a another dictionary  copy
           return CreateTrainDataDictionary(res);
        }
        /// <summary>
        /// get train timetable data
        /// </summary>

        /// <param name="train id">train id</param>
        /// <param name="isPlannedData">is planned data</param>
        /// <returns>train time table</returns>
        public List<TrainTimeTableData> GetTrainTimeTableData(bool isPlanned, int TrainServiceId, DateTime Date)
        {
            string FUNCTION_NAME = "GetTrainTimeTableData";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Function Entered");

            List<TrainTimeTableData> res;

            res = GetCacheTrainTimeTableData(isPlanned, TrainServiceId);
            if (res != null)
            {   
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Returning Cache Data");
                return res;
            }

            //if  data returned is null, get data from database
            res = new List<TrainTimeTableData>();

            string localSQL;

            if (isPlanned)
            {
                localSQL = " SELECT SERVICE_ID,ORDER_ID as STATION_ORDER_ID,A_TIME,D_TIME, DISPLAY_NAME " +
                       " FROM ATS_PLANNED_TRAIN_DATA, LOCATION WHERE " +
                       " SERVICE_ID = @TrainServiceId" +
                       " AND D_TIME >= @StartDateValue" +
                       " AND A_TIME <= @EndDateValue" +
                       " AND STATION_ID = PKEY" +
                       " AND STATION_ID NOT IN ( " + m_ExcludeStationList +
                       " ) ORDER BY SERVICE_ID, A_TIME";
            }
            else
            {
                localSQL = " SELECT SERVICE_ID,ORDER_ID as STATION_ORDER_ID,A_TIME,D_TIME, DISPLAY_NAME" +
                      " FROM ATS_PRACTICAL_TRAIN_DATA, LOCATION WHERE " +
                      " SERVICE_ID = @TrainServiceId" +
                      " AND D_TIME >= @StartDateValue" +
                      " AND A_TIME <= @EndDateValue" +
                      " AND STATION_ID = PKEY" +
                      " AND STATION_ID NOT IN ( " + m_ExcludeStationList +
                      " ) ORDER BY SERVICE_ID, A_TIME";
                     
            }


            SqlParameter parameter1 = new SqlParameter();
            parameter1.ParameterName = "TrainServiceId";
            parameter1.DbType = DbType.Int32;

            parameter1.Value = TrainServiceId;
            parameter1.Direction = System.Data.ParameterDirection.Input;
            List<SqlParameter> parameters = new List<SqlParameter>();
            parameters.Add(parameter1);

            SqlParameter parameter2 = new SqlParameter();
            parameter2.ParameterName = "StartDateValue";
            parameter2.DbType = DbType.DateTime;

            parameter2.Value = Date;
            parameter2.Direction = System.Data.ParameterDirection.Input;
            parameters.Add(parameter2);

            SqlParameter parameter3 = new SqlParameter();
            parameter3.ParameterName = "EndDateValue";
            parameter3.DbType = DbType.DateTime;

            parameter3.Value = Date.AddHours(23).AddMinutes(59).AddSeconds(59);
            parameter3.Direction = System.Data.ParameterDirection.Input;
            parameters.Add(parameter3);


            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQueryWithParams(localSQL, parameters);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    TrainTimeTableData timeTableData = new TrainTimeTableData();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("A_TIME")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["A_TIME"].ToString(), out dt))
                            timeTableData.ArrTime = dt;
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("D_TIME")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["D_TIME"].ToString(), out dt))
                            timeTableData.DeptTime = dt;
                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("SERVICE_ID")))
                    {
                        timeTableData.TrainServiceId = Convert.ToInt32(drReader["SERVICE_ID"]);
                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("STATION_ORDER_ID")))
                    {
                        timeTableData.StationID = Convert.ToInt32(drReader["STATION_ORDER_ID"]);

                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DISPLAY_NAME")))
                    {
                        timeTableData.StationName = Convert.ToString(drReader["DISPLAY_NAME"]);

                    }

                    res.Add(timeTableData);
                }
            }
            catch (System.Exception ex)
            {
                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, ex.ToString()); 
            }

            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 
            return res;
        }

        public List<TrainTimeTableData> GetCacheTrainTimeTableData(bool isPlanned, int TrainServiceId)
        {
            string FUNCTION_NAME = "GetCacheTrainTimeTableData";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Function Entered");

            //if data available in cache return cache data
            if (isPlanned && m_TrainDataCachePlanned != null)
            {
                if (m_TrainDataCachePlanned.ContainsKey(TrainServiceId))
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Return Cache Planned Data");

                    return m_TrainDataCachePlanned[TrainServiceId];
                }
                else
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                         EDebugLevelManaged.DebugInfo, "No Cache Planned Data");

                    return null;
                }

            }
            else if (!isPlanned && m_TrainDataCachePractical != null)
            {

                if (m_TrainDataCachePractical.ContainsKey(TrainServiceId))
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Return Cache Practical Data");

                    return m_TrainDataCachePractical[TrainServiceId];
                }
                else
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                         EDebugLevelManaged.DebugInfo, "No Cache Practical Data");

                    return null;
                }
            }
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited");

            return null;
        }

        public Dictionary<int, List<TrainTimeTableData>> CreateTrainDataDictionary(List<TrainTimeTableData> DataList)
        {

            string FUNCTION_NAME = "CreateTrainDataDictionary";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

            Dictionary<int, List<TrainTimeTableData>> timeTableDataRes = new Dictionary<int, List<TrainTimeTableData>>();
            
            //go through all the records, and form a hash table,
            //with train id as key and train timetable data list as value
            foreach (TrainTimeTableData record in DataList)
            {
                List<TrainTimeTableData> timeTableDataList;
                if (timeTableDataRes.ContainsKey(record.TrainServiceId))
                {
                    timeTableDataList = timeTableDataRes[record.TrainServiceId];
                    timeTableDataList.Add(record);
                }
                else
                {
                    timeTableDataList = new List<TrainTimeTableData>();
                    timeTableDataList.Add(record);
                    timeTableDataRes[record.TrainServiceId] = timeTableDataList;
                }
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited");

            return timeTableDataRes;

        }
    }
}


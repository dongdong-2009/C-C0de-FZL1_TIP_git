using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Entity.Trending;
using STEE.ISCS.Log;
using System.Data;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    public class DataLogDPLogTrendDAO
    {
        private static DataLogDPLogTrendDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.DataLogDPLogTrendDAO";

        public static DataLogDPLogTrendDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                m_instance = new DataLogDPLogTrendDAO();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (m_instance != null)
            {
                m_instance = null;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

//         /// <summary>
//         /// insert a record into TRENDVIEWER_LOG
//         /// </summary>
//         /// <param name="etyTrendLog">a record of trending log</param>
//         /// <returns>true/false(successful/failed)</returns>
//         public bool InsertTrendViewerLog(EtyTrendLog etyTrendLog)
//         {
//             string Function_Name = "InsertTrendViewerLog";
//             LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
//             bool result = false;
//             /*string localSQL = " INSERT INTO TRENDVIEWER_LOG(DATA_PT_HOST," +
//                                        "DATA_PT_SERVER,DATA_PT_NAME,DATA_PT_VALUE,DATA_PT_DATE) " +
//                                         " VALUES( '" + etyTrendLog.Data_PT_Host + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Server + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Name + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Value + "'" +
//                                         " , to_date('" + etyTrendLog.Data_PT_Time.ToString("yyyyMMddHHmmss") + "','YYYYMMDDHH24MISS') " +
//                                         " )";
//             result = SimpleDatabase.getInstance().ExecuteAndReturnNonQuery(localSQL);*/
//             string localSQL = " INSERT INTO TRENDVIEWER_LOG(DATA_PT_HOST," +
//                                        "DATA_PT_SERVER,DATA_PT_NAME,DATA_PT_VALUE,DATA_PT_DATE) " +
//                                         " VALUES( '" + etyTrendLog.Data_PT_Host + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Server + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Name + "'" +
//                                         " ,'" + etyTrendLog.Data_PT_Value + "'" +
//                                         " ," + SimpleDatabase.GetInstance().GetParameterDelimiter() + "DateValue)";
//             SqlParameter parameter = new SqlParameter();
//             parameter.ParameterName = "DateValue";
//             parameter.DbType = DbType.DateTime;
//             parameter.Value = etyTrendLog.Data_PT_Time;
//             parameter.Direction = System.Data.ParameterDirection.Input;
//             List<SqlParameter> parameters = new List<SqlParameter>();
//             parameters.Add(parameter);
//             result = SimpleDatabase.GetInstance().ExecuteNonQueryWithParams(localSQL, parameters);
// 
//             LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
//             return result;
//         }

        /// <summary>
        /// get dp log list for a historical dp to plot
        /// </summary>
        /// <param name="histDP">the given historical dp for charting</param>
        /// <param name="startTime">start time</param>
        /// <param name="endTime">end time</param>
        /// <returns>trend log list</returns>
        public List<EtyDataLogDPLogTrend> GetHistDPLogList(EtyHistDataPoint histDP, DateTime startTime, DateTime endTime)
        {
            string Function_Name = "GetHistDPLogList";
            List<EtyDataLogDPLogTrend> res = new List<EtyDataLogDPLogTrend>();
            string localSQL;
            //changed by luxiangmei, to make the logic for historical mode the same as mixed mode.
            //                 if (usedInMixMode)  //used in mixed mode
            //                 {
            localSQL = " SELECT CREATETIME,VALUE " +
                   " FROM DATALOG_DP_LOG_TREND WHERE ENTITY_KEY = " + histDP.EntityKey  +
                   " AND CREATETIME >= @StartDateValue" +
                   " AND CREATETIME <= @EndDateValue" +
                   " ORDER BY CREATETIME";


            SqlParameter parameter1 = new SqlParameter();
            parameter1.ParameterName = "StartDateValue";
            parameter1.DbType = DbType.DateTime;
            DateTime dtStart = new DateTime(histDP.DPStartDateTime.Year, histDP.DPStartDateTime.Month, histDP.DPStartDateTime.Day,
                startTime.Hour, startTime.Minute, startTime.Second);
            parameter1.Value = dtStart;
            parameter1.Direction = System.Data.ParameterDirection.Input;
            List<SqlParameter> parameters = new List<SqlParameter>();
            parameters.Add(parameter1);


            SqlParameter parameter2 = new SqlParameter();
            parameter2.ParameterName = "EndDateValue";
            parameter2.DbType = DbType.DateTime;
            DateTime dtEnd = new DateTime(histDP.DPEndDateTime.Year, histDP.DPEndDateTime.Month, histDP.DPEndDateTime.Day,
                endTime.Hour, endTime.Minute, endTime.Second);
            parameter2.Value = dtEnd;
            parameter2.Direction = System.Data.ParameterDirection.Input;
            parameters.Add(parameter2);

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQueryWithParams(localSQL, parameters);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyDataLogDPLogTrend entity = new EtyDataLogDPLogTrend();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("CREATETIME")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["CREATETIME"].ToString(), out dt))
                            entity.CreateTime = dt;
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("VALUE")))
                    {
                        entity.Value = Convert.ToDouble(drReader["VALUE"]);
                    }

                    res.Add(entity);
                }
            }
            catch (System.Exception ex)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
            }

            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            return res;
        }

    }
}

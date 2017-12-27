using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using System.Data;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    /// <summary>
    /// This class is responsible for accessing table TRENDVIEWER_LOG, which is saving dp value by OPCDataLogger.
    /// </summary>
    public class TrendLogDAO
    {
        private static TrendLogDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.TrendLogDAO";

        public static TrendLogDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                m_instance = new TrendLogDAO();
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

        /// <summary>
        /// insert a record into TRENDVIEWER_LOG
        /// </summary>
        /// <param name="etyTrendLog">a record of trending log</param>
        /// <returns>true/false(successful/failed)</returns>
        public bool InsertTrendViewerLog(EtyTrendLog etyTrendLog)
        {
            string Function_Name = "InsertTrendViewerLog";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool result = false;
            /*string localSQL = " INSERT INTO TRENDVIEWER_LOG(DATA_PT_HOST," +
                                       "DATA_PT_SERVER,DATA_PT_NAME,DATA_PT_VALUE,DATA_PT_DATE) " +
                                        " VALUES( '" + etyTrendLog.Data_PT_Host + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Server + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Name + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Value + "'" +
                                        " , to_date('" + etyTrendLog.Data_PT_Time.ToString("yyyyMMddHHmmss") + "','YYYYMMDDHH24MISS') " +
                                        " )";
            result = SimpleDatabase.getInstance().ExecuteAndReturnNonQuery(localSQL);*/
            //SimpleDatabase.GetInstance().BeginTransaction();
            string localSQL = " INSERT INTO TRENDVIEWER_LOG(DATA_PT_HOST," +
                                       "DATA_PT_SERVER,DATA_PT_NAME,DATA_PT_VALUE,DATA_PT_DATE) " +
                                        " VALUES( '" + etyTrendLog.Data_PT_Host + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Server + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Name + "'" +
                                        " ,'" + etyTrendLog.Data_PT_Value + "'" +
                                        " ,@DateValue)";
            SqlParameter parameter = new SqlParameter();
            parameter.ParameterName = "DateValue";
            parameter.DbType = DbType.DateTime;
            parameter.Value = etyTrendLog.Data_PT_Time;
            parameter.Direction = System.Data.ParameterDirection.Input;
            List<SqlParameter> parameters = new List<SqlParameter>();
            parameters.Add(parameter);
            result = SimpleDatabase.GetInstance().ExecuteNonQueryWithParams(localSQL, parameters);
           /* if (result)
                result  = SimpleDatabase.GetInstance().ExecuteNonQuery("INSERT INTO TRENDVIEWR_LOG(DATA_PT_HOST,DATA_PT_SERVER,DATA_PT_NAME,DATA_PT_VALUE,DATA_PT_DATE) VALUES('xxx','ddd','dpw','1','20130506190000')");
            if (result)
                SimpleDatabase.GetInstance().CommitTransaction();
            else
                SimpleDatabase.GetInstance().RollbackTransaction();*/

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return result;
        }

        /// <summary>
        /// get dp log list for a historical dp to plot
        /// </summary>
        /// <param name="histDP">the given historical dp for charting</param>
        /// <param name="startTime">start time</param>
        /// <param name="endTime">end time</param>
        /// <returns>trend log list</returns>
        public List<EtyTrendLog> GetHistDPLogList(EtyHistDataPoint histDP, DateTime startTime, DateTime endTime)
        {
            string Function_Name = "GetHistDPLogList";
            List<EtyTrendLog> res = new List<EtyTrendLog>();
            string localSQL;
            //changed by luxiangmei, to make the logic for historical mode the same as mixed mode.
            //                 if (usedInMixMode)  //used in mixed mode
            //                 {
            localSQL = " SELECT DATA_PT_DATE,DATA_PT_VALUE " +
                   " FROM TRENDVIEWER_LOG WHERE DATA_PT_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(histDP.DPName) + "' " +
                   " AND DATA_PT_SERVER = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(histDP.DPServer) + "' " +
                   " AND DATA_PT_HOST = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(histDP.DPHost) + "' " +
                   " AND DATA_PT_DATE >= @StartDateValue" +
                   " AND DATA_PT_DATE <= @EndDateValue" +
                   " ORDER BY DATA_PT_DATE";
            //                 }
            //                 else  // used in history mode, will ignore "startTime" and "endTime"
            //                 {
            //                     localSQL = " SELECT DATA_PT_DATE,DATA_PT_VALUE " +
            //                          " FROM TRENDVIEWER_LOG WHERE DATA_PT_NAME = '" + DAOHelper.convertGB2312To8859P1(histDP.DPName) + "' " +
            //                          " AND DATA_PT_SERVER = '" + DAOHelper.convertGB2312To8859P1(histDP.DPServer) + "' " +
            //                          " AND DATA_PT_HOST = '" + DAOHelper.convertGB2312To8859P1(histDP.DPHost) + "' " +
            //                          " AND DATA_PT_DATE >= to_date(" + histDP.DPStartDateTime.ToString("yyyyMMdd") +
            //                          "000000,'YYYYMMDDHH24MISS') " +
            //                          " AND DATA_PT_DATE <= to_date(" + histDP.DPEndDateTime.ToString("yyyyMMdd") +
            //                           "235959,'YYYYMMDDHH24MISS')  " +
            //                          " ORDER BY DATA_PT_DATE";
            //                 }

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

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQueryWithParams(localSQL,parameters);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyTrendLog entity = new EtyTrendLog();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_DATE")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["DATA_PT_DATE"].ToString(), out dt))
                            entity.Data_PT_Time = dt;
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_VALUE")))
                    {
                        entity.Data_PT_Value = Convert.ToDouble(drReader["DATA_PT_VALUE"]);
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
            }

            return res;
        }
    }
}

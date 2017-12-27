using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using System.Data;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    public class DatalogDPGroupTrendDAO
    {
        private static DatalogDPGroupTrendDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.DatalogDPGroupTrendDAO";
        private const string COLUMN_PKEY = "PKEY";
        private const string COLUMN_NAME = "NAME";
        private const string COLUMN_DESCRIPTION = "DESCRIPTION";
        private const string COLUMN_SAMPLE_INTERVAL = "SAMPLE_INTERVAL";
        private const string COLUMN_INTERVAL_TYPE = "INTERVAL_TYPE";
        private const string COLUMN_START_TIME = "START_TIME";
        private const string COLUMN_ENABLED = "ENABLED";
        private const string COLUMN_DELTA_VALUE = "DELTA_VALUE";
        private const string COLUMN_LOCATION_KEY = "LOCATION_KEY";
        private const string LOCATION_NAME = "LOCATION_NAME";

        public EtyDataLogDPGroupTrend CreateOPCSampleGrp()  //todo
        {
            return null;
        }

        public static DatalogDPGroupTrendDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Creating static DatalogDPGroupTrendDAO instance!");
                m_instance = new DatalogDPGroupTrendDAO();
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
        /// Returns the total sample group records count in table
        /// </summary>
        /// <returns>No of Sample group configured</returns>
        public int GetRowCount()
        {
            string Function_Name = "GetRowCount";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string localSQL;
            int nRowCount = 0;

            localSQL = "SELECT COUNT(NAME) AS MAXROWS FROM DATALOG_DP_GROUP_TREND";

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATION_KEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " WHERE" + LocationClauseStr;
            }


            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(0))
                    {
                        LogHelper.Debug(CLASS_NAME, Function_Name, "Function_Exited");
                        nRowCount = Convert.ToInt32(drReader["MAXROWS"]);
                    }
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

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return nRowCount;
        }

        /// <summary>
        /// Returns Sample group indexed between lowerbound and upperbound.
        /// </summary>
        /// <param name="lowerRecord">lowerbound - starting index</param>
        /// <param name="upperRecord">upperbound - ending index</param>
        /// <returns></returns>
        public List<EtyDataLogDPGroupTrend> GetAllOPCGrpsByPage(int lowerRecord, int upperRecord)
        {
            return GetAllOPCGrpsBySortNPage(COLUMN_NAME, "descending", lowerRecord, upperRecord);
        }

        /// <summary>
        /// Returns sorted Sample Group indexed between lowerbound and upperbound. 
        /// </summary>
        /// <param name="sortColumn">sorting based on this column</param>
        /// <param name="sortOrder">sorting order</param>
        /// <param name="lowerRecord">starting index</param>
        /// <param name="upperRecord">ending index</param>
        /// <returns></returns>
        public List<EtyDataLogDPGroupTrend> GetAllOPCGrpsBySortNPage(string sortColumn, string sortOrder, int lowerRecord, int upperRecord)
        {
            string Function_Name = "GetAllOPCGrpsBySortNPage";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0},{1},{2},{3}", sortColumn, sortOrder, lowerRecord, upperRecord));
            List<EtyDataLogDPGroupTrend> etyDPGrpTrendList = new List<EtyDataLogDPGroupTrend>();
            string localSQL;

            localSQL = " SELECT PKEY,NAME,DESCRIPTION,SAMPLE_INTERVAL,INTERVAL_TYPE,START_TIME,DELTA_VALUE,ENABLED,LOCATION_KEY "
                      + " FROM DATALOG_DP_GROUP_TREND";

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATION_KEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " WHERE" + LocationClauseStr;
            }

            localSQL += " ORDER BY " + sortColumn;
            sortOrder = sortOrder.ToUpper();
            if (sortOrder.Equals("ASCENDING") || sortOrder.Equals("ASC"))
                localSQL += " ASC ";
            else
                localSQL += " DESC ";
            localSQL = PreparePagingStatement(localSQL, lowerRecord, upperRecord);

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyDataLogDPGroupTrend etyDPGroupTrend = new EtyDataLogDPGroupTrend();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                        etyDPGroupTrend.SampleGrpID = Convert.ToDouble(drReader[COLUMN_PKEY]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                        etyDPGroupTrend.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                        etyDPGroupTrend.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());

                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_INTERVAL)))
                        etyDPGroupTrend.Interval = Convert.ToDouble(drReader[COLUMN_SAMPLE_INTERVAL]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_INTERVAL_TYPE)))
                    {
                        etyDPGroupTrend.IntervalType = drReader[COLUMN_INTERVAL_TYPE].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_START_TIME)))
                        etyDPGroupTrend.StartTime = drReader[COLUMN_START_TIME].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DELTA_VALUE)))
                    {
                        etyDPGroupTrend.DeltaValue = Convert.ToDouble(drReader[COLUMN_DELTA_VALUE]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_ENABLED)))
                    {
                        if (drReader[COLUMN_ENABLED].ToString() == "Y")
                        {
                            etyDPGroupTrend.Disabled = false;
                        }
                        else
                        {
                            etyDPGroupTrend.Disabled = true;
                        }
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_LOCATION_KEY)))
                    {
                        etyDPGroupTrend.LocationKey = Convert.ToDouble(drReader[COLUMN_LOCATION_KEY]);
                    }
                    etyDPGrpTrendList.Add(etyDPGroupTrend);
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
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDPGrpTrendList;
        }

        public List<EtyDataLogDPGroupTrend> GetAllOPCGrpsBySort(string sortColumn, string sortOrder)
        {
            string Function_Name = "GetAllOPCGrpsBySort";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0},{1}", sortColumn, sortOrder));
            List<EtyDataLogDPGroupTrend> etyDPGrpTrendList = new List<EtyDataLogDPGroupTrend>();
            string localSQL;

            localSQL = " SELECT DP.PKEY,DP.NAME,DP.DESCRIPTION,DP.SAMPLE_INTERVAL,DP.INTERVAL_TYPE,DP.START_TIME,DP.DELTA_VALUE,DP.ENABLED,DP.LOCATION_KEY,LOC.NAME AS LOCATION_NAME "
                      + " FROM DATALOG_DP_GROUP_TREND DP, LOCATION LOC WHERE DP.LOCATION_KEY = LOC.PKEY";

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATION_KEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }
            localSQL += " ORDER BY " + sortColumn;
            sortOrder = sortOrder.ToUpper();
            if (sortOrder.Equals("ASCENDING") || sortOrder.Equals("ASC"))
                localSQL += " ASC ";
            else
                localSQL += " DESC ";

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyDataLogDPGroupTrend etyDPGroupTrend = new EtyDataLogDPGroupTrend();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                        etyDPGroupTrend.SampleGrpID = Convert.ToDouble(drReader[COLUMN_PKEY]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                        etyDPGroupTrend.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                        etyDPGroupTrend.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());

                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_INTERVAL)))
                        etyDPGroupTrend.Interval = Convert.ToDouble(drReader[COLUMN_SAMPLE_INTERVAL]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_INTERVAL_TYPE)))
                    {
                        etyDPGroupTrend.IntervalType = drReader[COLUMN_INTERVAL_TYPE].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_START_TIME)))
                        etyDPGroupTrend.StartTime = drReader[COLUMN_START_TIME].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DELTA_VALUE)))
                    {
                        etyDPGroupTrend.DeltaValue = Convert.ToDouble(drReader[COLUMN_DELTA_VALUE]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_ENABLED)))
                    {
                        if (drReader[COLUMN_ENABLED].ToString() == "Y")
                        {
                            etyDPGroupTrend.Disabled = false;
                        }
                        else
                        {
                            etyDPGroupTrend.Disabled = true;
                        }
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_LOCATION_KEY)))
                    {
                        etyDPGroupTrend.LocationKey = Convert.ToDouble(drReader[COLUMN_LOCATION_KEY]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(LOCATION_NAME)))
                    {
                        etyDPGroupTrend.LocationName = DAOHelper.convert8859P1ToGB2312(drReader[LOCATION_NAME].ToString());
                    }
                    etyDPGrpTrendList.Add(etyDPGroupTrend);
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

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDPGrpTrendList;
        }

        /// <summary>
        /// Prepare SQL statement with paging details
        /// </summary>
        /// <param name="queryString">SQL statement to be paged</param>
        /// <param name="lowerRecord">starting index of the page</param>
        /// <param name="upperRecord">ending index of the page</param>
        /// <returns></returns>
        public string PreparePagingStatement(string queryString, int lowerRecord, int upperRecord)
        {
            //todo change for mysql
            string Function_Name = "PreparePagingStatement";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0},{1},{2}", queryString, lowerRecord, upperRecord));

            queryString = " SELECT * FROM ( SELECT a.*, ROWNUM rnum " +
                            " FROM ( " + queryString + " ) a ) WHERE rnum <= " + upperRecord.ToString() +
                            " AND rnum >= " + lowerRecord.ToString();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return queryString;
        }

        /// <summary>
        /// Returns all OPC Sample Group key and Name map
        /// </summary>
        /// <returns>Sample group key-Name map</returns>
        public Dictionary<string, string> GetAllOPCGrpIdAndName(bool checkLoc)
        {
            string Function_Name = "GetAllOPCGrpIdAndName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            Dictionary<string, string> opcGrpIDToNameDic = new Dictionary<string, string>();
            string localSQL;
            localSQL = "SELECT PKEY, NAME FROM  DATALOG_DP_GROUP_TREND";

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATION_KEY);
            if (LocationClauseStr.Length != 0 && checkLoc)
            {
                localSQL += " WHERE" + LocationClauseStr;
            }

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    string grpID = "", grpName = "";
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                        grpID = drReader[COLUMN_PKEY].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                        grpName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    opcGrpIDToNameDic.Add(grpID, grpName);
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
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return opcGrpIDToNameDic;
        }

        /// <summary>
        /// Returns Sample Group details of the specified Sample group Key.
        /// </summary>
        /// <param name="opcGrpID">Sample group key</param>
        /// <returns>Sample Group entity</returns>
        public EtyDataLogDPGroupTrend GetOPCGrpByID(double opcGrpID)
        {
            string Function_Name = "GetOPCGrpByID";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0}", opcGrpID));
            EtyDataLogDPGroupTrend etyDpGroupTrend = new EtyDataLogDPGroupTrend();

            string localSQL = " SELECT DP.NAME,DP.DESCRIPTION,DP.SAMPLE_INTERVAL,DP.INTERVAL_TYPE,DP.START_TIME,DP.DELTA_VALUE,DP.ENABLED,DP.LOCATION_KEY,LOC.NAME AS LOCATION_NAME"
                + " FROM DATALOG_DP_GROUP_TREND DP,LOCATION LOC WHERE DP.PKEY = " + opcGrpID.ToString() + "AND LOC.PKEY = DP.LOCATION_KEY";

           /* string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATION_KEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }*/


            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                        etyDpGroupTrend.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                        etyDpGroupTrend.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_INTERVAL)))
                        etyDpGroupTrend.Interval = Convert.ToInt32(drReader[COLUMN_SAMPLE_INTERVAL]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_INTERVAL_TYPE)))
                    {
                        etyDpGroupTrend.IntervalType = drReader[COLUMN_INTERVAL_TYPE].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_START_TIME)))
                    {
                        etyDpGroupTrend.StartTime = drReader[COLUMN_START_TIME].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DELTA_VALUE)))
                        etyDpGroupTrend.DeltaValue = Convert.ToDouble(drReader[COLUMN_DELTA_VALUE]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_ENABLED)))
                        etyDpGroupTrend.Disabled = !(DAOHelper.ChangeStrToBool(drReader[COLUMN_ENABLED].ToString()));
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_LOCATION_KEY)))
                    {
                        etyDpGroupTrend.LocationKey = Convert.ToDouble(drReader[COLUMN_LOCATION_KEY]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(LOCATION_NAME)))
                    {
                        etyDpGroupTrend.LocationName = DAOHelper.convert8859P1ToGB2312(drReader[LOCATION_NAME].ToString());
                    }
                    
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
            
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDpGroupTrend;
        }

        /// <summary>
        /// Updates the given sample group entity.
        /// </summary>
        /// <param name="etyDPGroupTrend">sample group entity</param>
        public bool UpdateOPCGrpData(EtyDataLogDPGroupTrend etyDPGroupTrend)
        {
            string Function_Name = "UpdateOPCGrpData";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - SampleGroup Name = {0}", etyDPGroupTrend.SampleGrpName));

            SimpleDatabase.GetInstance().BeginTransaction();

            string enabled = DAOHelper.GetEnabledStr(DAOHelper.ChangeBoolToStr(etyDPGroupTrend.Disabled));             

            string startTime = "";
            if (etyDPGroupTrend.StartTime.Length != 0)
            {
                startTime = DateTime.Parse(etyDPGroupTrend.StartTime).ToString("dd/MM/yyyy HH:mm:ss");                
            }
            string sample_grp_name = DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpName);
            
            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string date_func = (i==0)?DAOHelper.ORACLE_DATE_FUNCTION :DAOHelper.MYSQL_DATE_FUNCTION;
                string date_format = (i==0)?DAOHelper.ORACLE_DATE_FORMAT :DAOHelper.MYSQL_DATE_FORMAT;
                if (startTime.Trim().Length != 0)
                {
                    string sql = " UPDATE DATALOG_DP_GROUP_TREND "
                                 + " SET DESCRIPTION = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpDescription) + "'"
                                 + ",SAMPLE_INTERVAL = " + etyDPGroupTrend.Interval
                                 + ",INTERVAL_TYPE = '" + etyDPGroupTrend.IntervalType + "'"
                                 + ",START_TIME = " + date_func
                                 + "('" + startTime + "','" + date_format
                                 + "') ,DELTA_VALUE = " + etyDPGroupTrend.DeltaValue
                                 + ",ENABLED = '" + enabled + "'"
                                 + " WHERE NAME = '" + sample_grp_name + "' AND LOCATION_KEY = "
                                 + etyDPGroupTrend.LocationKey.ToString();
                    sqlStrings.Add(sql);
                }
                else
                {
                    string sql = " UPDATE DATALOG_DP_GROUP_TREND "
                        + " SET DESCRIPTION = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpDescription) + "'"
                        + ",SAMPLE_INTERVAL = " + etyDPGroupTrend.Interval
                        + ",INTERVAL_TYPE = '" + etyDPGroupTrend.IntervalType + "'"
                        + ",START_TIME = null, DELTA_VALUE = " + etyDPGroupTrend.DeltaValue
                        + ",ENABLED = '" + enabled + "'"
                        + " WHERE NAME = '" + sample_grp_name + "' AND LOCATION_KEY = "
                        + etyDPGroupTrend.LocationKey.ToString();
                    sqlStrings.Add(sql);
                }
            }
            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            bool rRes = true;        
            rRes = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);

            if (rRes)
            {
                rRes = DatalogConfigSettingsDAO.GetInstance().UpdateTrendVersionNum();
            }
            if (!rRes)
            {
                SimpleDatabase.GetInstance().RollbackTransaction();
            }
            else
            {
                SimpleDatabase.GetInstance().CommitTransaction();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return rRes;
        }

 
        /// <summary>
        ///  Inserts new sample group into database
        /// </summary>
        /// <param name="etyOPCGrp">new sample group entity</param>
        public bool InsertOPCGrpData(EtyDataLogDPGroupTrend etyDPGroupTrend)
        {
            string Function_Name = "InsertOPCGrpData";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup Name  = {0}", etyDPGroupTrend.SampleGrpName));
           
            SimpleDatabase.GetInstance().BeginTransaction();

            // Get The max pkey from the database first and then insert the group with pkey value.
            // To avoid having incorrect pkey value between stations , hence avoid concurrency problem
            SqlStatements sqlQueries = new SqlStatements();
            sqlQueries.OracleSql = DAOHelper.ORACLE_SEQ_FUNC_SQL;
            sqlQueries.MySqlStr = DAOHelper.MYSQL_SEQ_FUNC_SQL;
            System.Data.IDataReader drReader  = SimpleDatabase.GetInstance().ExecuteQueryWithSqls(sqlQueries);
            double nextPkey = 1;
            if (drReader != null && drReader.Read())
            {
                if (drReader.FieldCount == 1)
                {
                    nextPkey = Convert.ToDouble(drReader[0]);
                }
            }
            if (drReader != null)
            {
                drReader.Close();
                drReader.Dispose();
            }
            //
            //
            string enabled = DAOHelper.GetEnabledStr(DAOHelper.ChangeBoolToStr(etyDPGroupTrend.Disabled));
            string startTime = "";
            if (etyDPGroupTrend.StartTime.Length != 0)
            {
                startTime = DateTime.Parse(etyDPGroupTrend.StartTime).ToString("dd/MM/yyyy HH:mm:ss");
            }
            string sample_grp_name = DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpName);

            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string date_func = (i == 0) ? DAOHelper.ORACLE_DATE_FUNCTION : DAOHelper.MYSQL_DATE_FUNCTION;
                string date_format = (i == 0) ? DAOHelper.ORACLE_DATE_FORMAT : DAOHelper.MYSQL_DATE_FORMAT;
                if (startTime.Trim().Length != 0)
                {
                    string sql = "INSERT INTO DATALOG_DP_GROUP_TREND "
                                    + " (PKEY,NAME,DESCRIPTION,SAMPLE_INTERVAL,START_TIME,DELTA_VALUE,ENABLED,INTERVAL_TYPE,LOCATION_KEY)"
                                    + " VALUES"
                                    + " ("+ nextPkey+",'" + sample_grp_name + "'"
                                    + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpDescription) + "'"
                                    + "," + etyDPGroupTrend.Interval
                                    + "," + date_func + "('" + startTime + "','" + date_format
                                    + "')," + etyDPGroupTrend.DeltaValue
                                    + ",'" + enabled + "'"
                                    + ",'" + etyDPGroupTrend.IntervalType + "',"
                                    + LocationKeyHelper.GetInstance().LocationKey.ToString() + ")";
                    sqlStrings.Add(sql);
                }
                else
                {
                    string sql = "INSERT INTO DATALOG_DP_GROUP_TREND "
                                   + " (PKEY,NAME,DESCRIPTION,SAMPLE_INTERVAL,DELTA_VALUE,ENABLED,INTERVAL_TYPE,LOCATION_KEY)"
                                   + " VALUES"
                                   + " ("+nextPkey+",'" + sample_grp_name + "'"
                                   + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPGroupTrend.SampleGrpDescription) + "'"
                                   + "," + etyDPGroupTrend.Interval                         
                                   + "," + etyDPGroupTrend.DeltaValue
                                   + ",'" + enabled + "'"
                                   + ",'" + etyDPGroupTrend.IntervalType + "',"
                                   + LocationKeyHelper.GetInstance().LocationKey.ToString() + ")";
                    sqlStrings.Add(sql);
                }
            }

            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            bool rRes =  true;          
            rRes = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);

            if (rRes)
            {
                rRes = DatalogConfigSettingsDAO.GetInstance().UpdateTrendVersionNum();
            }
            if (!rRes)
            {
                SimpleDatabase.GetInstance().RollbackTransaction();
            }
            else
            {
                SimpleDatabase.GetInstance().CommitTransaction();
            }
            
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return rRes;
        }


        /// <summary>
        /// Delete  opc Sample Group by key
        /// </summary>
        /// <param name="opcGrpId">Sample group key</param>
        public bool DeleteOPCSampleGroupById(string opcGrpId)
        {
            string Function_Name = "DeleteOPCSampleGroupById";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup ID  = {0}", opcGrpId));
            bool result = true;
            SimpleDatabase.GetInstance().BeginTransaction();
            try
            {
                List<string> sqlStrings = new List<string>();
                for (int i = 0; i < 2; i++)
                {
                    string sql = "DELETE FROM DATALOG_DP_GROUP_TREND WHERE PKEY = " + opcGrpId;
                    sqlStrings.Add(sql);
                }
                List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
                result = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }
            if (result)
            {
                //update datapoint table also.
                result = DatalogDPTrendDAO.GetInstance().DisableGrpDataPoint(Convert.ToDouble(opcGrpId));
            }
            if (result)
            {
                result = DatalogConfigSettingsDAO.GetInstance().UpdateTrendVersionNum();
            }
            if (!result)
            {
                SimpleDatabase.GetInstance().RollbackTransaction();
            }
            else
            {
                SimpleDatabase.GetInstance().CommitTransaction();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return result;
        }
    }
}

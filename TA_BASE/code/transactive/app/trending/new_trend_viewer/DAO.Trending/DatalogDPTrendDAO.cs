using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    public class DatalogDPTrendDAO
    {
        private static DatalogDPTrendDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.DatalogDPTrendDAO";
        private const string COLUMN_PKEY = "PKEY";
        private const string COLUMN_NAME = "DP_NAME";
        private const string COLUMN_ENTITY_KEY = "ENTITY_KEY";
        private const string COLUMN_ENABLED = "ENABLED";
        private const string COLUMN_SAMPLE_GRP_ID = "DP_GRP_ID";
        private const string COLUMN_ENTITY_LOCATIONKEY = "LOCATIONKEY";


        public static DatalogDPTrendDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Creating static DatalogDPTrendDAO instance!");
                m_instance = new DatalogDPTrendDAO();
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
        /// Returns the DataPoints based on the filterstring for specified server name.
        /// e.g. for filterstring can include substring datapoint name to search. 
        /// </summary>
        /// <param name="opcServerName">datapoint server name</param>
        /// <param name="filterString">Where clause query string</param>
        /// <returns>Datapoints</returns>
        public List<EtyDataLogDPTrend> GetOPCDataPoint(string opcServerName, string filterString)
        {
            const string Function_Name = "GetOPCDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            List<EtyDataLogDPTrend> etyDPTrendList = new List<EtyDataLogDPTrend>();

            string localSQL = "SELECT et.PKEY AS ENTITY_KEY,CONCAT(et.NAME,'.Value') as DP_NAME ,DP.DP_GRP_ID as DP_GRP_ID FROM ENTITY et LEFT JOIN DATALOG_DP_TREND DP"
                + " ON et.PKEY = DP.ENTITY_KEY WHERE et.TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')";

            if (filterString.Trim() != "")
            {
                localSQL += filterString;
            }
            localSQL += " ORDER BY DP_NAME ";


            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            if (drReader != null)
            {
                try
                {
                    while (drReader.Read())
                    {
                        EtyDataLogDPTrend etyDPTrend = new EtyDataLogDPTrend();
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_ENTITY_KEY)))
                            etyDPTrend.EntityKey = Convert.ToDouble(drReader[COLUMN_ENTITY_KEY]);
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                            etyDPTrend.OPCDataPointName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_GRP_ID)))
                            etyDPTrend.OPCSampleGrpId = Convert.ToDouble(drReader[COLUMN_SAMPLE_GRP_ID]);
                        etyDPTrendList.Add(etyDPTrend);
                    }

                }
                catch (System.Exception ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                }

                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDPTrendList;
        }

        /// <summary>
        /// Returns the DataPoints based specified datapoint name substring.
        /// </summary>
        /// <param name="opcServerName">Datapoint server name</param>
        /// <param name="DataPointNameSubstr"> Datapoint name substring to be queried</param>
        /// <returns>Datapoints</returns>
        public List<EtyDataLogDPTrend> GetDataPointByName(string opcServerName, string DataPointNameSubstr)
        {
            const string Function_Name = "GetDataPointByName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            List<EtyDataLogDPTrend> etyDPTrendList = new List<EtyDataLogDPTrend>();

            string localSQL = "SELECT et.PKEY AS ENTITY_KEY,CONCAT(et.NAME,'.Value')as DP_NAME ,DP.DP_GRP_ID AS DP_GRP_ID FROM ENTITY et LEFT JOIN DATALOG_DP_TREND DP" +
                   " ON et.PKEY = DP.ENTITY_KEY WHERE TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')" +
                   " AND UPPER(et.NAME) LIKE '%" + DAOHelper.convertEscapeStringAndGB2312To8859P1(DataPointNameSubstr) + "%'";

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_ENTITY_LOCATIONKEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }

            localSQL += " ORDER BY DP_NAME ";

            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            if (drReader != null)
            {
                try
                {
                    while (drReader.Read())
                    {
                        EtyDataLogDPTrend etyDPTrend = new EtyDataLogDPTrend();
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_ENTITY_KEY)))
                            etyDPTrend.EntityKey = Convert.ToDouble(drReader[COLUMN_ENTITY_KEY]);
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                            etyDPTrend.OPCDataPointName = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                        if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_GRP_ID)))
                            etyDPTrend.OPCSampleGrpId = Convert.ToDouble(drReader[COLUMN_SAMPLE_GRP_ID]);
                        etyDPTrendList.Add(etyDPTrend);
                    }
                }
                catch (System.Exception ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                }
                drReader.Close();
                drReader.Dispose();
                //SimpleDatabase.GetInstance().CloseCurrentSession();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDPTrendList;
        }

        /// <summary>
        /// Returns Datapoint belonging to specified OPC Sample Group.
        /// </summary>
        /// <param name="sampleGrpId">Sample Group key</param>
        /// <returns>DataPoints</returns>
        public bool GetOPCDataPointByGrpId(double sampleGrpId)
        {
            string Function_Name = "GetOPCDataPointByGrpId";

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            bool res = false;

            string localSQL = "SELECT DP_NAME FROM DATALOG_DP_TREND WHERE  DP_GRP_ID = " + sampleGrpId;
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(0))
                    {
                        if (drReader.Read())
                        {
                            LogHelper.Debug(CLASS_NAME, Function_Name, "Function_Exited with value true");
                            res = true;
                        }

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
            return res;
        }

        /// <summary>
        /// Retrieves the sampleGrpId to which specified data points belongs to 
        /// </summary>
        /// <param name="dataPoint"></param>
        /// <returns></returns>
        public double GetGrpIDByDPName(string dataPoint)
        {
            string Function_Name = "GetSampleGrpIdFromDP";

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            double sampleGrpId = -1;

            string localSQL = "SELECT DP_GRP_ID FROM DATALOG_DP_TREND WHERE DP_NAME = '"+dataPoint+"'";
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                if (drReader != null && drReader.Read())
               {
                   if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_SAMPLE_GRP_ID)))
                       sampleGrpId = Convert.ToDouble(drReader[COLUMN_SAMPLE_GRP_ID]);                    
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
            return sampleGrpId;
        }

        public bool GetOPCDataPointByName(string dataPoint)
        {
            string Function_Name = "GetOPCDataPointByName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string localSQL = "SELECT PKEY FROM DATALOG_DP_TREND WHERE DP_NAME = '" + dataPoint + "'";
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            bool bFound = false;
            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                        bFound = true;
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
            return bFound;
        }


        /// <summary>
        /// Updates the DataPoints of specified sampleGroup.
        /// </summary>
        /// <param name="disabled">true - disabled, false - enabled</param>
        /// <param name="sampleGrpId">datapoint Sample group key</param>
        public bool UpdateOPCDataPointByGrpId(bool disabled, double sampleGrpId)
        {
            const string Function_Name = "UpdateOPCDataPointByGrpId";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string sEnabled = DAOHelper.GetEnabledStr(DAOHelper.ChangeBoolToStr(disabled));
            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string sql = " UPDATE DATALOG_DP_TREND "
                         + " SET DP_GRP_ID = ''"
                         + ",ENABLED = '" +sEnabled + "'"
                         + " WHERE DP_GRP_ID = " + sampleGrpId.ToString();
                sqlStrings.Add(sql);
            }
            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            bool bRes = true;
            bRes = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);       
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return bRes;
        }

        /// <summary>
        /// Updates the datapoints of specified name
        /// </summary>
        /// <param name="EtyDataLogDPTrend"></param>
        public bool UpdateOPCDataPointByName(EtyDataLogDPTrend etyDPTrend) 
        {
            const string Function_Name = "UpdateOPCDataPointByName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string sEnabled = DAOHelper.GetEnabledStr(DAOHelper.ChangeBoolToStr(etyDPTrend.Disabled));
            if (GetOPCDataPointByName(etyDPTrend.OPCDataPointName))
            {

                List<string> sqlStrings = new List<string>();
                for (int i = 0; i < 2; i++)
                {
                    string sql = " UPDATE DATALOG_DP_TREND "
                               + " SET DP_GRP_ID = " + etyDPTrend.OPCSampleGrpId.ToString()
                               + ",ENABLED =  '" + sEnabled + "'"
                               + " WHERE DP_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(etyDPTrend.OPCDataPointName.Trim()) + "'";
                    sqlStrings.Add(sql);
                }
                List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
                return SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);   
            }
            else
            {
                return  InsertOPCDataPoint(etyDPTrend);
            }
        }

        public bool InsertOPCDataPoint(EtyDataLogDPTrend etyDPTrend)
        {
            const string Function_Name = "InsertOPCDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string sEnabled = DAOHelper.GetEnabledStr(DAOHelper.ChangeBoolToStr(etyDPTrend.Disabled));
            string dpName = etyDPTrend.OPCDataPointName.Trim();
            if (dpName.Contains(".Value"))
            {
                dpName = dpName.Remove(dpName.Length - 6);
            }
            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string sql = "INSERT INTO DATALOG_DP_TREND (PKEY, ENTITY_KEY, DP_NAME, DP_GRP_ID,ENABLED)"
                    + " (SELECT PKEY,PKEY,CONCAT(NAME,'.Value'),"
                    + etyDPTrend.OPCSampleGrpId.ToString()
                    + ",'" + sEnabled + "'"
                    + " FROM ENTITY  WHERE TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint' )"
                    + " AND NAME = '" + dpName + "')";                  
                sqlStrings.Add(sql);
            }
            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            return SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);   
        }

        /// <summary>
        /// Disables and clears the sample group link of the datapoints belonging to specified sample group
        /// </summary>
        /// <param name="sampleGrpId">datapoint sample group key</param>
        public bool DisableGrpDataPoint(double dpGrpId)
        {
            const string Function_Name = "DisableGrpDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string sql = "UPDATE DATALOG_DP_TREND SET ENABLED = 'N',DP_GRP_ID = '' WHERE DP_GRP_ID = " + dpGrpId;
                sqlStrings.Add(sql);
            }
            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            bool bRes = true;
            bRes = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);          
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return bRes;
        }

        public void UpdateDataPoints(EtyDataLogDPTrend sampleGrpEty, List<EtyDataLogDPTrend> selectedDataPoints)
        {
            SimpleDatabase.GetInstance().BeginTransaction();
            bool bResult = UpdateOPCDataPointByGrpId(sampleGrpEty.Disabled,sampleGrpEty.OPCSampleGrpId);
            if (bResult)
            {
                for (int i = 0; i < selectedDataPoints.Count; i++)
                {
                    bResult = UpdateOPCDataPointByName(selectedDataPoints[i]);
                }
            }               
            if(bResult)
               bResult =  DatalogConfigSettingsDAO.GetInstance().UpdateTrendVersionNum();
            if (bResult)
                SimpleDatabase.GetInstance().CommitTransaction();
            else
                SimpleDatabase.GetInstance().RollbackTransaction();
            return;
        }
    }
}

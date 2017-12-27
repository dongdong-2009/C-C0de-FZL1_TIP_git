 using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    /// <summary>
    /// This class is responsible for updating, deleting and 
    /// selecting records of opc_dt_sample_grp
    /// </summary>
    public class OPCSampleGroupDAO
    {
        private static OPCSampleGroupDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.OPCSampleGroupDAO";

        public EtyOPCSampleGroup CreateOPCSampleGrp()  //todo
        {
            return null;
        }

        public static OPCSampleGroupDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Creating static OPCSampleGroupDAO instance!");
                m_instance = new OPCSampleGroupDAO();
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

                localSQL = "SELECT COUNT(SAMPLE_GRP_NAME) AS MAXROWS FROM OPC_DT_SAMPLE_GRP";
                System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
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
        public List<EtyOPCSampleGroup> GetAllOPCGrpsByPage(int lowerRecord, int upperRecord)
        {
            return GetAllOPCGrpsBySortNPage("SAMPLE_GRP_NAME", "descending", lowerRecord, upperRecord);
        }

        /// <summary>
        /// Returns sorted Sample Group indexed between lowerbound and upperbound. 
        /// </summary>
        /// <param name="sortColumn">sorting based on this column</param>
        /// <param name="sortOrder">sorting order</param>
        /// <param name="lowerRecord">starting index</param>
        /// <param name="upperRecord">ending index</param>
        /// <returns></returns>
        public List<EtyOPCSampleGroup> GetAllOPCGrpsBySortNPage(string sortColumn, string sortOrder, int lowerRecord, int upperRecord)
        {
            string Function_Name = "GetAllOPCGrpsBySortNPage";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0},{1},{2},{3}", sortColumn, sortOrder, lowerRecord, upperRecord));
            List<EtyOPCSampleGroup> etyOPCGrpList = new List<EtyOPCSampleGroup>();
            string localSQL;

                localSQL = " SELECT SAMPLE_GRP_ID,SAMPLE_GRP_NAME,SAMPLE_GRP_DESC,SAMPLE_INTERVAL,INTERVAL_TYPE,START_TIME,DELTA_VALUE,DISABLE "
                          + " FROM OPC_DT_SAMPLE_GRP";
                localSQL += " ORDER BY " + sortColumn;
                sortOrder = sortOrder.ToUpper();
                if (sortOrder.Equals("ASCENDING") || sortOrder.Equals("ASC"))
                    localSQL += " ASC ";
                else
                    localSQL += " DESC ";
                localSQL = PreparePagingStatement(localSQL, lowerRecord, upperRecord);

                System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                try
                {
                    while (drReader != null && drReader.Read())
                    {
                        EtyOPCSampleGroup etyOPCSampleGrp = new EtyOPCSampleGroup();
                        if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                            etyOPCSampleGrp.SampleGrpID = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                        if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_NAME")))
                            etyOPCSampleGrp.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_NAME"].ToString());
                        if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_DESC")))
                            etyOPCSampleGrp.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_DESC"].ToString());

                        if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_INTERVAL")))
                            etyOPCSampleGrp.Interval = Convert.ToDouble(drReader["SAMPLE_INTERVAL"]);
                        if (!drReader.IsDBNull(drReader.GetOrdinal("INTERVAL_TYPE")))
                        {
                            etyOPCSampleGrp.IntervalType = drReader["INTERVAL_TYPE"].ToString();
                        }
                        if (!drReader.IsDBNull(drReader.GetOrdinal("START_TIME")))
                            etyOPCSampleGrp.StartTime = drReader["START_TIME"].ToString();
                        if (!drReader.IsDBNull(drReader.GetOrdinal("DELTA_VALUE")))
                        {
                            etyOPCSampleGrp.DeltaValue = Convert.ToDouble(drReader["DELTA_VALUE"]);
                        }
                        if (!drReader.IsDBNull(drReader.GetOrdinal("DISABLE")))
                        {
                            if (drReader["DISABLE"].ToString() == "Y")
                            {
                                etyOPCSampleGrp.Disabled = true;
                            }
                            else
                            {
                                etyOPCSampleGrp.Disabled = false;
                            }
                        }
                        etyOPCGrpList.Add(etyOPCSampleGrp);
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
            return etyOPCGrpList;
        }


        public List<EtyOPCSampleGroup> GetAllOPCGrpsBySort(string sortColumn, string sortOrder)
        {
            string Function_Name = "GetAllOPCGrpsBySort";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0},{1}", sortColumn, sortOrder));
            List<EtyOPCSampleGroup> etyOPCGrpList = new List<EtyOPCSampleGroup>();
            string localSQL;

            localSQL = " SELECT SAMPLE_GRP_ID,SAMPLE_GRP_NAME,SAMPLE_GRP_DESC,SAMPLE_INTERVAL,INTERVAL_TYPE,START_TIME,DELTA_VALUE,DISABLE "
                      + " FROM OPC_DT_SAMPLE_GRP";
            localSQL += " ORDER BY " + sortColumn;
            sortOrder = sortOrder.ToUpper();
            if (sortOrder.Equals("ASCENDING") || sortOrder.Equals("ASC"))
                localSQL += " ASC ";
            else
                localSQL += " DESC ";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyOPCSampleGroup etyOPCSampleGrp = new EtyOPCSampleGroup();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                        etyOPCSampleGrp.SampleGrpID = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_NAME")))
                        etyOPCSampleGrp.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_DESC")))
                        etyOPCSampleGrp.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_DESC"].ToString());

                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_INTERVAL")))
                        etyOPCSampleGrp.Interval = Convert.ToDouble(drReader["SAMPLE_INTERVAL"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("INTERVAL_TYPE")))
                    {
                        etyOPCSampleGrp.IntervalType = drReader["INTERVAL_TYPE"].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("START_TIME")))
                        etyOPCSampleGrp.StartTime = drReader["START_TIME"].ToString();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DELTA_VALUE")))
                    {
                        etyOPCSampleGrp.DeltaValue = Convert.ToDouble(drReader["DELTA_VALUE"]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DISABLE")))
                    {
                        if (drReader["DISABLE"].ToString() == "Y")
                        {
                            etyOPCSampleGrp.Disabled = true;
                        }
                        else
                        {
                            etyOPCSampleGrp.Disabled = false;
                        }
                    }
                    etyOPCGrpList.Add(etyOPCSampleGrp);
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
            return etyOPCGrpList;
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
        public Dictionary<string, string> GetAllOPCGrpIdAndName()
        {
            string Function_Name = "GetAllOPCGrpIdAndName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            Dictionary<string, string> opcGrpIDToNameDic = new Dictionary<string, string>();

            string localSQL;

            localSQL = "SELECT SAMPLE_GRP_ID, SAMPLE_GRP_NAME FROM  OPC_DT_SAMPLE_GRP";
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    string grpID = "", grpName = "";
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                        grpID = drReader["SAMPLE_GRP_ID"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_NAME")))
                        grpName = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_NAME"].ToString());
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
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return opcGrpIDToNameDic;
        }

        /// <summary>
        /// Returns Sample Group details of the specified Sample group Key.
        /// </summary>
        /// <param name="opcGrpID">Sample group key</param>
        /// <returns>Sample Group entity</returns>
        public EtyOPCSampleGroup GetOPCGrpByID(string opcGrpID)
        {
            string Function_Name = "GetOPCGrpByID";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - {0}", opcGrpID));
            EtyOPCSampleGroup etyOPCSampleGrp = new EtyOPCSampleGroup();

            string localSQL = " SELECT SAMPLE_GRP_NAME,SAMPLE_GRP_DESC,SAMPLE_INTERVAL,INTERVAL_TYPE,START_TIME,DELTA_VALUE,DISABLE "
                + " FROM OPC_DT_SAMPLE_GRP WHERE SAMPLE_GRP_ID = " + opcGrpID;                
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);         
            try
            {
                while (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_NAME")))
                        etyOPCSampleGrp.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_DESC")))
                        etyOPCSampleGrp.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_DESC"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_INTERVAL")))
                        etyOPCSampleGrp.Interval = Convert.ToInt32(drReader["SAMPLE_INTERVAL"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("INTERVAL_TYPE")))
                    {
                        etyOPCSampleGrp.IntervalType = drReader["INTERVAL_TYPE"].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("START_TIME")))
                    {
                        etyOPCSampleGrp.StartTime = drReader["START_TIME"].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DELTA_VALUE")))
                        etyOPCSampleGrp.DeltaValue = Convert.ToDouble(drReader["DELTA_VALUE"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DISABLE")))
                        etyOPCSampleGrp.Disabled = DAOHelper.ChangeStrToBool(drReader["DISABLE"].ToString());
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
            return etyOPCSampleGrp;
        }

        /// <summary>
        /// Returns all Enabled Sample group Entities.
        /// </summary>
        /// <returns>Sample group entity list</returns>
        public List<EtyOPCSampleGroup> GetAllEnabledSampleGrp()
        {
            string Function_Name = "GetAllEnabledSampleGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<EtyOPCSampleGroup> etyOPCGrpList = new List<EtyOPCSampleGroup>();
            string localSQL;

            localSQL = " SELECT SAMPLE_GRP_ID,SAMPLE_GRP_NAME,SAMPLE_GRP_DESC,SAMPLE_INTERVAL,INTERVAL_TYPE,START_TIME,DELTA_VALUE,DISABLE "
                      + " FROM OPC_DT_SAMPLE_GRP WHERE DISABLE = 'N' ";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyOPCSampleGroup etyOPCSampleGrp = new EtyOPCSampleGroup();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                        etyOPCSampleGrp.SampleGrpID = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_NAME")))
                        etyOPCSampleGrp.SampleGrpName = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_DESC")))
                        etyOPCSampleGrp.SampleGrpDescription = DAOHelper.convert8859P1ToGB2312(drReader["SAMPLE_GRP_DESC"].ToString());

                    if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_INTERVAL")))
                        etyOPCSampleGrp.Interval = Convert.ToDouble(drReader["SAMPLE_INTERVAL"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("INTERVAL_TYPE")))
                    {
                        etyOPCSampleGrp.IntervalType = drReader["INTERVAL_TYPE"].ToString();
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("START_TIME")))
                        etyOPCSampleGrp.StartTime = drReader["START_TIME"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DELTA_VALUE")))
                    {
                        etyOPCSampleGrp.DeltaValue = Convert.ToDouble(drReader["DELTA_VALUE"]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DISABLE")))
                    {
                        if (drReader["DISABLE"].ToString() == "Y")
                        {
                            etyOPCSampleGrp.Disabled = true;
                            //TODO CHECK ERROR
                        }
                        else
                        {
                            etyOPCSampleGrp.Disabled = false;
                        }
                    }
                    etyOPCGrpList.Add(etyOPCSampleGrp);
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
            return etyOPCGrpList;
        }

        /// <summary>
        /// Updates specified sample groups in database
        /// </summary>
        /// <param name="etyOPCGrpsList">sample group entities list</param>
        public void UpdateOPCGrps(List<EtyOPCSampleGroup> etyOPCGrpsList)
        {
            foreach (EtyOPCSampleGroup etyOPCGrp in etyOPCGrpsList)
            {
                UpdateOPCGrpData(etyOPCGrp);
            }
        }

        /// <summary>
        /// Updates the given sample group entity.
        /// </summary>
        /// <param name="etyOPCGrp">sample group entity</param>
        public void UpdateOPCGrpData(EtyOPCSampleGroup etyOPCGrp)
        {
            string Function_Name = "UpdateOPCGrpData";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params - SampleGroup Name = {0}", etyOPCGrp.SampleGrpName));

            string localSQL;
            string sample_grp_name = DAOHelper.convertGB2312To8859P1(etyOPCGrp.SampleGrpName);
            localSQL = " UPDATE OPC_DT_SAMPLE_GRP "
                         + " SET SAMPLE_GRP_DESC = '" + DAOHelper.convertGB2312To8859P1(etyOPCGrp.SampleGrpDescription) + "'"
                         + ",SAMPLE_INTERVAL = " + etyOPCGrp.Interval
                         + ",INTERVAL_TYPE = '" + etyOPCGrp.IntervalType + "'"
                         + ",START_TIME = '" + etyOPCGrp.StartTime + "'"
                         + ",DELTA_VALUE = " + etyOPCGrp.DeltaValue
                         + ",DISABLE = '" + DAOHelper.ChangeBoolToStr(etyOPCGrp.Disabled) + "'"
                         + " WHERE SAMPLE_GRP_NAME ='" + sample_grp_name + "'";
            SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        ///  Inserts new sample group into database
        /// </summary>
        /// <param name="etyOPCGrp">new sample group entity</param>
        public void InsertOPCGrpData(EtyOPCSampleGroup etyOPCGrp)
        {
            string Function_Name = "InsertOPCGrpData";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup Name  = {0}", etyOPCGrp.SampleGrpName));

            string localSQL;

            string sample_grp_name = DAOHelper.convertGB2312To8859P1(etyOPCGrp.SampleGrpName);
            localSQL = "INSERT INTO OPC_DT_SAMPLE_GRP "
                        + " (SAMPLE_GRP_NAME,SAMPLE_GRP_DESC,SAMPLE_INTERVAL,START_TIME,DELTA_VALUE,DISABLE,INTERVAL_TYPE)"
                        + " VALUES"
                        + " ('" + sample_grp_name + "'"
                        + ",'" + DAOHelper.convertGB2312To8859P1(etyOPCGrp.SampleGrpDescription) + "'"
                        + "," + etyOPCGrp.Interval
                        + ",'" + etyOPCGrp.StartTime + "'"
                        + "," + etyOPCGrp.DeltaValue
                        + ",'" + DAOHelper.ChangeBoolToStr(etyOPCGrp.Disabled) + "'"
                        + ",'" + etyOPCGrp.IntervalType + "')";
            SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Returns whether specified sample group name exists in the table 
        /// </summary>
        /// <param name="OPCSampleGrpName">sample group name to be searched</param>
        /// <returns>true/false</returns>
        /* public bool IsOPCSampleGroupExist(string OPCSampleGrpName)
         {
             string Function_Name = "IsOPCSampleGroupExist";
             LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup Name  = %s", OPCSampleGrpName));

             OracleConnection dbConn = DBConnection.getInstance().getConn();
             if (dbConn != null && OPCSampleGrpName.Trim() != "")
             {
                 try
                 {
                     string localSQL = "SELECT SAMPLE_GRP_NAME FROM OPC_DT_SAMPLE_GRP WHERE SAMPLE_GRP_NAME = '" + OPCSampleGrpName.Trim() + "'";

                     OracleDataReader drOracle = DBConnection.getInstance().ExecuteOracleQuery(localSQL, dbConn);

                     if (drOracle != null)
                     {
                         if (!drOracle.IsDBNull(0))
                         {
                             LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Function_Exited with value %d", true));
                             return true;
                         }
                     }

                     if (drOracle != null)
                     {
                         drOracle.Close();
                         drOracle.Dispose();
                     }
                     /*string temp = DBConnection.getInstance().GetStringFromQuery(localSQL, dbConn);
                     if (temp != "")
                     {
                         LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Function_Exited with value %d",true));
                         return true;
                     }
                 }
                 catch (Exception localException)
                 {
                     LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
                 }
             }
             LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Function_Exited with value %d",false));
             return false;
         }*/

        /// <summary>
        /// Delete  opc Sample Group by key
        /// </summary>
        /// <param name="opcGrpId">Sample group key</param>
        public bool DeleteOPCSampleGroupById(string opcGrpId)
        {
            string Function_Name = "DeleteOPCSampleGroupById";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup ID  = {0}", opcGrpId));
            bool result = false;

            try
            {
                string localSQL = "DELETE FROM OPC_DT_SAMPLE_GRP WHERE SAMPLE_GRP_ID = " + opcGrpId;
                result = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return result;
        }

        /// <summary>
        /// Updates HAS_DT_PT flag of the specified sample Group
        /// </summary>
        /// <param name="opcGrpId">sample group key</param>
        /// <param name="hasDP">flag value</param>
        public void UpdateGrpDataPointFlag(double opcGrpId, bool hasDP)
        {
            string Function_Name = "UpdateGrpDataPointFlag";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params -  SampleGroup ID  = {0}, Flag = {1}", opcGrpId, hasDP));


            string localSQL = "UPDATE OPC_DT_SAMPLE_GRP SET HAS_DT_PT = '" + DAOHelper.ChangeBoolToStr(hasDP) + "'"
                         + " WHERE SAMPLE_GRP_ID = " + opcGrpId.ToString();
            SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

    }
}

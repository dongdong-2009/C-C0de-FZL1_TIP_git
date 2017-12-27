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
    /// This class is responsible for accessing table TRENDVIEWER_CONFIG, 
    /// which saves configuration for realtime datapoint charting.
    /// </summary>
    public class DataPointDAO
    {
        private const string CLASS_NAME = "DAO.Trending.DataPointDAO";

        /// <summary>
        /// retrieve all the configuration name for real-time charting.
        /// </summary>
        /// <returns></returns>
        public List<string> GetAllDPGrpNames()
        {
            string Function_Name = "GetAllDPGrpNames";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<string> dpGrpNames = new List<string>();        
            string localSQL = " SELECT DISTINCT CONFIG_NAME FROM TRENDVIEWER_CONFIG order by config_name ";
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            if (drReader != null)
            {
                try
                {
                    while (drReader.Read())
                    {
                        if (!drReader.IsDBNull(0))
                            dpGrpNames.Add(DAOHelper.convert8859P1ToGB2312(drReader["CONFIG_NAME"].ToString()));
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
            return dpGrpNames;

        }

        public bool SaveDPListToGrp(List<EtyDataPoint> datapointList, string grpName)
        {
            SimpleDatabase.GetInstance().BeginTransaction();
            if( DeleteAllDPInGrp(grpName) )
            {
                if(InsertDPListToGrp(datapointList, grpName))
                {
                    SimpleDatabase.GetInstance().CommitTransaction();
                    return true;
                }
            }
            SimpleDatabase.GetInstance().RollbackTransaction();
            return false;
 
        }

        /// <summary>
        /// insert a new configuration into TRENDVIEWER_CONFIG.
        /// </summary>
        /// <param name="datapointList">realtime dp list in a configuration</param>
        /// <param name="grpName">configuration name</param>
        public bool InsertDPListToGrp(List<EtyDataPoint> datapointList, string grpName)
        {
            string Function_Name = "InsertDPListToGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            bool executeOK = false;

                string localSQL = " INSERT INTO TRENDVIEWER_CONFIG(CONFIG_NAME,DATA_PT_NAME,DATA_PT_TYPE, " +
                            " DATA_PT_COLOR,DATA_PT_SERVER,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME,DATA_PT_HOST,DATA_PT_VISIBLE) " +
                            " VALUES( '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
                string fullSQL;
                foreach (EtyDataPoint datapoint in datapointList)
                {
                    fullSQL = localSQL
                        + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(datapoint.DPName) + "'"
                        + ",'" + DAOHelper.ConvertLineTypeToDBString(datapoint.DPType) + "'"
                        + ",'" + datapoint.DPColor + "'"
                        + ",'" + datapoint.DPServer + "'"
                        + ",'" + DAOHelper.ChangeBoolToStr(datapoint.DPEnabled) + "'"
                        + ",'" + DAOHelper.ChangeBoolToStr(datapoint.DPLblEnabled) + "'"
                        + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(datapoint.DPLblName) + "'"
                        + ",'" + datapoint.DPHost + "'"
                        + ",'" + DAOHelper.ChangeBoolToStr(datapoint.DPVisible) + "'"
                        + " ) ";
                    executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(fullSQL);
                    fullSQL = "";
                    if (executeOK == false)
                    {
                        break;
                    }
                }
            
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }

        /// <summary>
        /// delete a configuration in  TRENDVIEWER_CONFIG
        /// </summary>
        /// <param name="grpName">configuration name</param>
        public bool DeleteAllDPInGrp(string grpName)
        {
            string Function_Name = "DeleteAllDPInGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK = false;

            {
                string localSQL = " DELETE FROM TRENDVIEWER_CONFIG WHERE CONFIG_NAME ='" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
                executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }

        /// <summary>
        /// retrieve a configuration from TRENDVIEWER_CONFIG by configuration name.
        /// </summary>
        /// <param name="configName">configuration name</param>
        /// <returns></returns>
        public List<EtyDataPoint> GetDPByCfgName(string configName)
        {
            string Function_Name = "GetDPByCfgName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<EtyDataPoint> dpList = new List<EtyDataPoint>();

                string localSQL = " SELECT DATA_PT_NAME,DATA_PT_TYPE,DATA_PT_COLOR,DATA_PT_SERVER,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME,DATA_PT_HOST,DATA_PT_VISIBLE  " +
                            " FROM TRENDVIEWER_CONFIG WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(configName) + "'";

                //            string localSQL = " SELECT c.DATA_PT_NAME,c.DATA_PT_TYPE,c.DATA_PT_COLOR,c.DATA_PT_SERVER,c.DATA_PT_ENABLED,c.DATA_PT_LBL_ENABLED, c.DATA_PT_LBL_NAME, c.DATA_PT_HOST,c.DATA_PT_VISIBLE, "
                //                            + "o.keyid from trendviewer_config c, opc_dt_pt o where c.data_pt_name = o.data_pt_name " +
                //           " and c.CONFIG_NAME = '" + DAOHelper.convertGB2312To8859P1(configName) + "'";

                System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                EtyDataPoint newEtyDP = null;
            try
            {
                while (drReader != null && drReader.Read())
                {
                    newEtyDP = new EtyDataPoint();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_NAME")))
                        newEtyDP.DPName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_TYPE")))
                        newEtyDP.DPType = DAOHelper.ConvertLineTypeToEnum(drReader["DATA_PT_TYPE"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_COLOR")))
                        newEtyDP.DPColor = drReader["DATA_PT_COLOR"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_SERVER")))
                        newEtyDP.DPServer = drReader["DATA_PT_SERVER"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_ENABLED")))
                        newEtyDP.DPEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_ENABLED")))
                        newEtyDP.DPLblEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_LBL_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_NAME")))
                        newEtyDP.DPLblName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_LBL_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_HOST")))
                        newEtyDP.DPHost = drReader["DATA_PT_HOST"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_VISIBLE")))
                        newEtyDP.DPVisible = DAOHelper.ChangeStrToBool(drReader["DATA_PT_VISIBLE"].ToString());

                    // if (!drOracle.IsDBNull(9))
                    //     newEtyDP.PKey = (ulong)(drOracle.GetOracleDecimal(9).ToInt64());

                    dpList.Add(newEtyDP);
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
            return dpList;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;
using System.Data;

namespace DAO.Trending
{
    /// <summary>
    /// This class is responsible for accessing table TRENDVIEWER_HIST_CONFIG
    /// </summary>
    public class HistDataPointDAO
    {
        private const string CLASS_NAME = "DAO.Trending.HistDataPointDAO";

        /// <summary>
        /// Get all the historical datapoint configuration names
        /// </summary>
        /// <returns></returns>
        public List<string> GetAllHistDPGrpNames()
        {
            string Function_Name = "GetAllHistDPGrpNames";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            List<string> dpGrpNames = new List<string>();



            string localSQL = " SELECT DISTINCT CONFIG_NAME FROM TRENDVIEWER_HIST_CONFIG order by config_name ";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                while (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(0))
                        dpGrpNames.Add(DAOHelper.convert8859P1ToGB2312(drReader.GetString(0).ToString()));

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
            return dpGrpNames;

        }

        public bool SaveHistDPListToGrp(List<EtyHistDataPoint> histDPList, string grpName)
        {
            SimpleDatabase.GetInstance().BeginTransaction();
            if(DeleteAllHistDPInGrp(grpName))
            {
                if(InsertHistDPListToGrp(histDPList, grpName))
                {
                    SimpleDatabase.GetInstance().CommitTransaction();
                    return true;
                }
            }

            SimpleDatabase.GetInstance().RollbackTransaction();
            return false;
        }

        /// <summary>
        /// insert a historical datapoint configuration into table TRENDVIEWER_HIST_CONFIG, 
        /// if histDP.pkey=0, insert null into entitykey column (means no entity key is related to this datapoint)
        /// </summary>
        /// <param name="histDPList">the historical datapoint list included in a configuration</param>
        /// <param name="grpName">the historical configuration name</param>
        public bool InsertHistDPListToGrp(List<EtyHistDataPoint> histDPList, string grpName)
        {
            string Function_Name = "InsertHistDPListToGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK = false;

            string localSQL = " INSERT INTO TRENDVIEWER_HIST_CONFIG(CONFIG_NAME,DATA_PT_NAME,DATA_PT_TYPE,DATA_PT_COLOR,DATA_PT_SERVER " +
                        " ,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME,DATA_PT_DT,DATA_PT_HOST,DATA_PT_DT_END, ENTITYKEY) " +
                        " VALUES( '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";

            string fullSQL;
            foreach (EtyHistDataPoint datapoint in histDPList)
            {
                fullSQL = localSQL
                        + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(datapoint.DPName) + "'"
                        + ",'" + DAOHelper.ConvertLineTypeToDBString(datapoint.DPType) + "'"
                        + ",'" + datapoint.DPColor + "'"
                        + ",'" + datapoint.DPServer + "'"
                        + ",'" + DAOHelper.ChangeBoolToStr(datapoint.DPEnabled) + "'"
                        + ",'" + DAOHelper.ChangeBoolToStr(datapoint.DPLblEnabled) + "'"
                        + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(datapoint.DPLblName) + "'"
                        + ", @StartDateValue"
                        + ", '" + datapoint.DPHost + "'"
                        + ", @EndDateValue"
                        + ","  ;
                if(datapoint.EntityKey != 0)
                {
                    fullSQL = fullSQL + datapoint.EntityKey.ToString() + ")";
                }else
                {
                    fullSQL = fullSQL + "NULL )";
                }

                SqlParameter parameter1 = new SqlParameter();
                parameter1.ParameterName = "StartDateValue";
                parameter1.DbType = DbType.DateTime;
                DateTime dtStart = new DateTime(datapoint.DPStartDateTime.Year, datapoint.DPStartDateTime.Month, datapoint.DPStartDateTime.Day,
                    0, 0, 0);
                parameter1.Value = dtStart;
                parameter1.Direction = System.Data.ParameterDirection.Input;
                List<SqlParameter> parameters = new List<SqlParameter>();
                parameters.Add(parameter1);

                SqlParameter parameter2 = new SqlParameter();
                parameter2.ParameterName = "EndDateValue";
                parameter2.DbType = DbType.DateTime;
                DateTime dtEnd = new DateTime(datapoint.DPEndDateTime.Year, datapoint.DPEndDateTime.Month, datapoint.DPEndDateTime.Day,
                   23, 59, 59);
                parameter2.Value = dtEnd;
                parameter2.Direction = System.Data.ParameterDirection.Input;
                parameters.Add(parameter2);

                executeOK = SimpleDatabase.GetInstance().ExecuteNonQueryWithParams(fullSQL, parameters);
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
        /// delete the historical datapoint configuration by the configuration name
        /// </summary>
        /// <param name="grpName">historical datapoint configuration name</param>
        public bool DeleteAllHistDPInGrp(string grpName)
        {
            string Function_Name = "DeleteAllHistDPInGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK = false;

            string localSQL = " DELETE FROM TRENDVIEWER_HIST_CONFIG WHERE CONFIG_NAME ='" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
            executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }

        /// <summary>
        /// get historical datapoint configuration by config name
        /// </summary>
        /// <param name="configName">hist dp config name</param>
        /// <returns>hist dp list in one group</returns>
        public List<EtyHistDataPoint> GetHistDPByCfgName(string configName)
        {
            string Function_Name = "GetHistDPByCfgName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<EtyHistDataPoint> histDPList = new List<EtyHistDataPoint>();


            string localSQL = " SELECT DATA_PT_NAME,DATA_PT_TYPE,DATA_PT_COLOR," +
                    " DATA_PT_SERVER,DATA_PT_ENABLED,DATA_PT_LBL_ENABLED,DATA_PT_LBL_NAME,DATA_PT_DT," +
                    " DATA_PT_HOST,DATA_PT_DT_END ,ENTITYKEY " +
                    " FROM TRENDVIEWER_HIST_CONFIG WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(configName) + "'";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            EtyHistDataPoint newEtyHistDP = null;

            try
            {
                while (drReader != null && drReader.Read())
                {
                    newEtyHistDP = new EtyHistDataPoint();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_NAME")))
                        newEtyHistDP.DPName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_TYPE")))
                        newEtyHistDP.DPType = DAOHelper.ConvertLineTypeToEnum(drReader["DATA_PT_TYPE"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_COLOR")))
                        newEtyHistDP.DPColor = drReader["DATA_PT_COLOR"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_SERVER")))
                        newEtyHistDP.DPServer = drReader["DATA_PT_SERVER"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_ENABLED")))
                        newEtyHistDP.DPEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_ENABLED")))
                        newEtyHistDP.DPLblEnabled = DAOHelper.ChangeStrToBool(drReader["DATA_PT_LBL_ENABLED"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_LBL_NAME")))
                        newEtyHistDP.DPLblName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_LBL_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_DT")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["DATA_PT_DT"].ToString(), out dt))
                            newEtyHistDP.DPStartDateTime = dt;

                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_HOST")))
                        newEtyHistDP.DPHost = drReader["DATA_PT_HOST"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_DT_END")))
                    {
                        DateTime dt;
                        if (DateTime.TryParse(drReader["DATA_PT_DT_END"].ToString(), out dt))
                            newEtyHistDP.DPEndDateTime = dt;
                    }
                    if(!drReader.IsDBNull((drReader.GetOrdinal("ENTITYKEY"))))
                    {
                        newEtyHistDP.EntityKey = Convert.ToUInt64(drReader["ENTITYKEY"]);
                    }

                    histDPList.Add(newEtyHistDP);
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
            return histDPList;
        }
    }
}

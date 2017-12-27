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
    /// This class is responsible for selecting, modifying and deleting records in
    /// opc_dt_pt and opc_dt_node table.
    /// </summary>
    public class OPCNodeNDataPointDAO
    {
        private const string OPC_DT_NodeTableName = "OPC_DT_NODE";
        private const string OPC_DT_PTTableName = "OPC_DT_PT";
        private const string CLASS_NAME = "DAO.Trending.OPCNodeNDataPointDAO";

        private static OPCNodeNDataPointDAO m_instance = null;

        public static OPCNodeNDataPointDAO GetInstance()
        {
            if (m_instance == null)
            {
                m_instance = new OPCNodeNDataPointDAO();
            }
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            if (m_instance != null)
            {
                m_instance = null;
            }
        }

        /// <summary>
        /// Returns all the Root Data Nodes.
        /// </summary>
        /// <returns>Root Data Nodes</returns>
        public List<EtyOPCDataNode> GetAllOPCDataNode(string serverRootName, string opcServerName )
        {
            const string Function_Name = "GetAllOPCDataNode";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

          
            List<EtyOPCDataNode> OPCDataNodeList = new List<EtyOPCDataNode>();

            string localSQL = " SELECT KEYID,DT_NODE_NAME,DT_NODE_DESC " +
                    " FROM OPC_DT_NODE WHERE PKEYID  IN (SELECT DISTINCT PKEY FROM ENTITY WHERE NAME = '" + serverRootName + "') AND DISABLE = 'N' AND DATA_PT_SERVER = '" + opcServerName
                    + "' ORDER BY DT_NODE_NAME";

            //sql for getting from entity table
            /*string localSQL = " SELECT PKEY,NAME,DESCRIPTION" +
                                    " FROM ENTITY WHERE TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataNode')"+
                                    " AND PARENTKEY  IN (SELECT DISTINCT PKEY FROM ENTITY WHERE TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'SCADAROOT'))"+
                                    " ORDER BY NAME";*/

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            if (drReader != null)
            {
                try
                {
                    while (drReader.Read())
                    {
                        EtyOPCDataNode etyOPCDataNode = new EtyOPCDataNode();
                        if (!drReader.IsDBNull(drReader.GetOrdinal("KEYID")))
                            etyOPCDataNode.OPCDataNodeId = Convert.ToDouble(drReader["KEYID"]);
                        if (!drReader.IsDBNull(drReader.GetOrdinal("DT_NODE_NAME")))
                            etyOPCDataNode.OPCDataNodeName = DAOHelper.convert8859P1ToGB2312(drReader["DT_NODE_NAME"].ToString());
                        if (!drReader.IsDBNull(drReader.GetOrdinal("DT_NODE_DESC")))
                            etyOPCDataNode.OPCDataNodeDesc = DAOHelper.convert8859P1ToGB2312(drReader["DT_NODE_DESC"].ToString());
                        OPCDataNodeList.Add(etyOPCDataNode);

                    }  
                }
                catch (System.Exception ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                }
                drReader.Close();
                drReader.Dispose();
            }     


            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return OPCDataNodeList;
        }

        /// <summary>
        /// Returns the Child node of the given parent node.
        /// </summary>
        /// <param name="parentKeyId">parent node key</param>
        /// <param name="opcServerName">OPC Server name</param>
        /// <returns>Child Data nodes</returns>
        public List<EtyOPCDataNode> GetChildOPCDataNode(string parentKeyId, string opcServerName)
        {
            const string Function_Name = "GetChildOPCDataNode";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            List<EtyOPCDataNode> OPCDataNodeList = new List<EtyOPCDataNode>();        
            string localSQL = " SELECT KEYID,DT_NODE_NAME,DT_NODE_DESC FROM OPC_DT_NODE " +
                        " WHERE PKEYID = " + parentKeyId.ToString() + " AND DISABLE = 'N' AND DATA_PT_SERVER = '" + opcServerName
                        + "' ORDER BY DT_NODE_NAME";

                /*string localSQL = " SELECT PKEY,NAME,DESCRIPTION" +
                                        " FROM ENTITY WHERE TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataNode')"+
                                        " AND PARENTKEY = " + parentKeyId.ToString() +
                                        " ORDER BY NAME";*/
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            if (drReader != null)
            {
               try
               {
                   while (drReader.Read())
                   {
                       EtyOPCDataNode etyOPCDataNode = new EtyOPCDataNode();
                       if (!drReader.IsDBNull(drReader.GetOrdinal("KEYID")))
                           etyOPCDataNode.OPCDataNodeId = Convert.ToDouble(drReader["KEYID"]);
                       if (!drReader.IsDBNull(drReader.GetOrdinal("DT_NODE_NAME")))
                           etyOPCDataNode.OPCDataNodeName = DAOHelper.convert8859P1ToGB2312(drReader["DT_NODE_NAME"].ToString());
                       if (!drReader.IsDBNull(drReader.GetOrdinal("DT_NODE_DESC")))
                           etyOPCDataNode.OPCDataNodeDesc = DAOHelper.convert8859P1ToGB2312(drReader["DT_NODE_DESC"].ToString());
                       OPCDataNodeList.Add(etyOPCDataNode);
                   }
               }
               catch (System.Exception ex)
               {
                   LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
               }
               drReader.Close();
               drReader.Dispose();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return OPCDataNodeList;
        }


       /* public int GetOPCDataPointCount(string opcServerName, string filterString)
        {
            OracleConnection dbConn = DBConnection.getInstance().getConn();
            int nCount = 0;
            if (dbConn != null)
            {
                string localSQL = " SELECT COUNT(KEYID) " +
                                          " FROM OPC_DT_PT " +
                                          " WHERE  DATA_PT_SERVER = '" + opcServerName + "'";

                if (filterString.Trim() != "")
                {
                    localSQL += filterString;
                }

                OracleDataReader drOracle = DBConnection.getInstance().ExecuteOracleQuery(localSQL, dbConn);
                if (drOracle != null && drOracle.Read())
                {
                    if (!drOracle.IsDBNull(0))
                        nCount = drOracle.GetOracleDecimal(0).ToInt32();
                }

                if (drOracle != null)
                {
                    drOracle.Close();
                    drOracle.Dispose();
                }
            }
            return nCount;
        }*/

        /// <summary>
        /// Returns the DataPoints based on the filterstring for specified server name.
        /// e.g. for filterstring can include substring datapoint name to search. 
        /// </summary>
        /// <param name="opcServerName">datapoint server name</param>
        /// <param name="filterString">Where clause query string</param>
        /// <returns>Datapoints</returns>
        public List<EtyOPCDataPoint> GetOPCDataPoint(string opcServerName, string filterString)
        {
            const string Function_Name = "GetOPCDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            List<EtyOPCDataPoint> etyDataPointList = new List<EtyOPCDataPoint>();

                /*string localSQL = " SELECT KEYID,PKEYID,DATA_PT_NAME,DATA_PT_DESC,SAMPLE_GRP_ID " +
                        " FROM OPC_DT_PT " +
                        " WHERE  DATA_PT_SERVER = '" + TrendingHelper.convertGB2312To8859P1(opcServerName.Trim()) + "'";*/

                /*string localSQL = " SELECT et.PKEY,et.PARENTKEY as PARENTKEY,CONCAT(et.NAME,'.Value') as DATA_PT_NAME ,et.DESCRIPTION, dt.SAMPLE_GRP_ID as SAMPLE_GRP_ID FROM ENTITY et, OPC_DT_PT dt" +
                    " WHERE et.TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')" +
                    " AND et.PKEY = dt.KEYID(+)";*/

                string localSQL = " SELECT et.PKEY,et.PARENTKEY as PARENTKEY,CONCAT(et.NAME,'.Value') as DATA_PT_NAME ,et.DESCRIPTION, dt.SAMPLE_GRP_ID as SAMPLE_GRP_ID FROM ENTITY et LEFT JOIN OPC_DT_PT dt" +
                       " ON et.PKEY = dt.KEYID WHERE et.TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')";       

                if (filterString.Trim() != "")
                {
                    localSQL += filterString;
                }
                localSQL += " ORDER BY DATA_PT_NAME ";


                System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                if (drReader != null)
                {
                    try
                    {
                        while (drReader.Read())
                        {
                            EtyOPCDataPoint etyDataPoint = new EtyOPCDataPoint();
                            if (!drReader.IsDBNull(drReader.GetOrdinal("PKEY")))
                                etyDataPoint.OPCDataPointId = Convert.ToDouble(drReader["PKEY"]);
                            if (!drReader.IsDBNull(drReader.GetOrdinal("PARENTKEY")))
                                etyDataPoint.OPCParentDataPointId = Convert.ToDouble(drReader["PARENTKEY"]);
                            if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_NAME")))
                                etyDataPoint.OPCDataPointName = DAOHelper.convert8859P1ToGB2312(drReader["DATA_PT_NAME"].ToString());
                            if (!drReader.IsDBNull(drReader.GetOrdinal("DESCRIPTION")))
                                etyDataPoint.OPCDataPointDesc = DAOHelper.convert8859P1ToGB2312(drReader["DESCRIPTION"].ToString());
                            if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                                etyDataPoint.OPCSampleGrpId = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                            etyDataPointList.Add(etyDataPoint);
                        }

                    }
                    catch (System.Exception ex)
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                    }

                    drReader.Close();
                    drReader.Dispose();
                }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDataPointList;
        }

        /// <summary>
        /// Returns the DataPoints based specified datapoint name substring.
        /// </summary>
        /// <param name="opcServerName">Datapoint server name</param>
        /// <param name="DataPointNameSubstr"> Datapoint name substring to be queried</param>
        /// <returns>Datapoints</returns>
        public List<EtyOPCDataPoint> GetDataPointByName(string opcServerName, string DataPointNameSubstr)
        {
            const string Function_Name = "GetDataPointByName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            List<EtyOPCDataPoint> etyDataPointList = new List<EtyOPCDataPoint>();

               /* string localSQL = " SELECT KEYID,PKEYID,DATA_PT_NAME,DATA_PT_DESC,SAMPLE_GRP_ID " +
                        " FROM OPC_DT_PT " +
                        " WHERE  DATA_PT_SERVER = '" + TrendingHelper.convertGB2312To8859P1(opcServerName.Trim()) + "'" +
                        " And DATA_PT_NAME LIKE '%" + DAO.Trending.TrendingHelper.convertGB2312To8859P1(DataPointNameSubstr) + "%'";

                localSQL += " ORDER BY DATA_PT_NAME ";*/


                /*string localSQL = " SELECT et.PKEY,et.PARENTKEY,CONCAT(et.NAME,'.Value')as NAME ,et.DESCRIPTION, dt.SAMPLE_GRP_ID FROM ENTITY et, OPC_DT_PT dt" +
                    " WHERE TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')" +
                    " AND UPPER(NAME) LIKE '%" + DAOHelper.convertGB2312To8859P1(DataPointNameSubstr) +"%'"+
                    " AND et.PKEY = dt.KEYID(+)";*/

            string localSQL = " SELECT et.PKEY,et.PARENTKEY,CONCAT(et.NAME,'.Value')as NAME ,et.DESCRIPTION, dt.SAMPLE_GRP_ID FROM ENTITY et LEFT JOIN OPC_DT_PT dt" +
                   " ON et.PKEY = dt.KEYID WHERE TYPEKEY = (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint')" +
                   " AND UPPER(NAME) LIKE '%" + DAOHelper.convertGB2312To8859P1(DataPointNameSubstr) + "%'";                 

                localSQL += " ORDER BY NAME ";

                 System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                if (drReader != null)
                {
                    try
                    {
                        while (drReader.Read())
                        {
                            EtyOPCDataPoint etyDataPoint = new EtyOPCDataPoint();
                            if (!drReader.IsDBNull(drReader.GetOrdinal("PKEY")))
                                etyDataPoint.OPCDataPointId = Convert.ToDouble(drReader["PKEY"]);
                            if (!drReader.IsDBNull(drReader.GetOrdinal("PARENTKEY")))
                                etyDataPoint.OPCParentDataPointId = Convert.ToDouble(drReader["PARENTKEY"]);
                            if (!drReader.IsDBNull(drReader.GetOrdinal("NAME")))
                                etyDataPoint.OPCDataPointName = DAOHelper.convert8859P1ToGB2312(drReader["NAME"].ToString());
                            if (!drReader.IsDBNull(drReader.GetOrdinal("DESCRIPTION")))
                                etyDataPoint.OPCDataPointDesc = DAOHelper.convert8859P1ToGB2312(drReader["DESCRIPTION"].ToString());
                            if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                                etyDataPoint.OPCSampleGrpId = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                            etyDataPointList.Add(etyDataPoint);
                        }
                    }
                    catch (System.Exception ex)
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                    }
                    drReader.Close();
                    drReader.Dispose();
                }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDataPointList;
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

            string localSQL = "SELECT DATA_PT_NAME FROM OPC_DT_PT WHERE  SAMPLE_GRP_ID = " + sampleGrpId ;
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            
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
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return res;
        }

        /// <summary>
        /// Returns Unique Enabled Datapoints of specified servername and hostname,
        /// which belongs to enabled OPC sample group
        /// </summary>
        /// <param name="hostName">DataPoint Hostname</param>
        /// <param name="serverName">DataPoint ServerName</param>
        /// <returns>DataPoint</returns>
        public List<EtyOPCDataPoint> GetUniqueEnabledOPCDataPoint(string hostName, string serverName)
        {
            const string Function_Name = "GetUniqueEnabledOPCDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            List<EtyOPCDataPoint> etyDataPointList = new List<EtyOPCDataPoint>();

                string localSQL = "SELECT DISTINCT dt.DATA_PT_NAME,dt.SAMPLE_GRP_ID " +
                            " FROM OPC_DT_PT dt, OPC_DT_SAMPLE_GRP grp " +
                            " WHERE dt.DATA_PT_HOST = '" + hostName.Trim() + "' AND dt.DATA_PT_SERVER = '" + serverName.Trim() + "' AND dt.DISABLE = 'N' AND dt.SAMPLE_GRP_ID = grp.SAMPLE_GRP_ID and grp.DISABLE = 'N' ";

                System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
                if (drReader != null)
                {
                    try
                    {
                        while (drReader.Read())
                        {
                            EtyOPCDataPoint etyDataPoint = new EtyOPCDataPoint();
                            if (!drReader.IsDBNull(drReader.GetOrdinal("DATA_PT_NAME")))
                                etyDataPoint.OPCDataPointName = drReader["DATA_PT_NAME"].ToString();
                            if (!drReader.IsDBNull(drReader.GetOrdinal("SAMPLE_GRP_ID")))
                                etyDataPoint.OPCSampleGrpId = Convert.ToDouble(drReader["SAMPLE_GRP_ID"]);
                            etyDataPointList.Add(etyDataPoint);
                        }
                    }
                    catch (System.Exception ex)
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                    }
                    drReader.Close();
                    drReader.Dispose();
                }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return etyDataPointList;
        }

        /// <summary>
        /// Updates the DataPoints of specified sampleGroup.
        /// </summary>
        /// <param name="disabled">true - disabled, false - enabled</param>
        /// <param name="opcServerName">datapoint server name</param>
        /// <param name="sampleGrpId">datapoint Sample group key</param>
        public void UpdateOPCDataPointByGrpId(bool disabled, string opcServerName, double sampleGrpId)
        {
            const string Function_Name = "UpdateOPCDataPointByGrpId";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


                string localSQL = " UPDATE OPC_DT_PT "
                             + " SET SAMPLE_GRP_ID = ''"
                             + ",DISABLE = '" + DAOHelper.ChangeBoolToStr(disabled) + "'"
                             + " WHERE DATA_PT_SERVER = '" + DAOHelper.convertGB2312To8859P1(opcServerName.Trim()) + "'"
                             + " AND SAMPLE_GRP_ID = " + sampleGrpId.ToString();

                SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Updates the datapoints of specified name
        /// </summary>
        /// <param name="disabled">true-disabled , false - enabled</param>
        /// <param name="opcServerName">datapoint server name</param>
        /// <param name="sampleGrpId">sample group key</param>
        /// <param name="DataPointName">datapoint name to be updated</param>
        public void UpdateOPCDataPointByName(EtyOPCDataPoint etyDataPoint ) //bool disabled, string opcServerName, double sampleGrpId,string DataPointName)
        {
            const string Function_Name = "UpdateOPCDataPointByName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

                string localSQL = " UPDATE OPC_DT_PT "
                               + " SET SAMPLE_GRP_ID = " + etyDataPoint.OPCSampleGrpId.ToString()
                               + ",DISABLE =  '" + DAOHelper.ChangeBoolToStr(etyDataPoint.Disabled) + "'"
                               + " WHERE DATA_PT_NAME = '" + DAOHelper.convertGB2312To8859P1(etyDataPoint.OPCDataPointName.Trim()) + "'"
                               + " AND DATA_PT_SERVER = '" + DAOHelper.convertGB2312To8859P1(etyDataPoint.OPCDataPointServer.Trim()) + "'";
                
                //DBConnection.getInstance().ExecuteOracleNonQuery(localSQL, dbConn);
                if (!SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL))
                {
                    string dpName = etyDataPoint.OPCDataPointName.Trim();
                    if (dpName.Contains(".Value"))
                    {
                        dpName = dpName.Remove(dpName.Length - 6);
                    }
                    //insert the datapoint into the OPC_dt_pt table.
                    localSQL = "INSERT INTO OPC_DT_PT (DATA_PT_HOST, DATA_PT_SERVER, KEYID, DATA_PT_NAME, DATA_PT_DESC, PKEYID,SAMPLE_GRP_ID,DISABLE)"
                        + " (SELECT '127.0.0.1', 'TransActiveDataSource', PKEY,CONCAT(NAME,'.Value'),DESCRIPTION , PARENTKEY,"
                        + etyDataPoint.OPCSampleGrpId.ToString()
                        + ",'" + DAOHelper.ChangeBoolToStr(etyDataPoint.Disabled) +"'"
                        +" FROM ENTITY  WHERE TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint' )"
                        + " AND NAME = '" + dpName + "')";

                    SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);
                }


            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        /// <summary>
        /// Disables and clears the sample group link of the datapoints belonging to specified sample group
        /// </summary>
        /// <param name="sampleGrpId">datapoint sample group key</param>
        public void DisableGrpDataPoint(double sampleGrpId)
        {
            const string Function_Name = "DisableGrpDataPoint";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


                string localSQL = "UPDATE OPC_DT_PT SET DISABLE = 'Y',SAMPLE_GRP_ID = '' WHERE SAMPLE_GRP_ID = " + sampleGrpId;              
                
                SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }
    }
}

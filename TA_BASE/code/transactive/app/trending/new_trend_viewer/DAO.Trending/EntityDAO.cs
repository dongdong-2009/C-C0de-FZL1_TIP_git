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
    /// this class is responsible for accessing entity
    /// </summary>
    public class EntityDAO
    {
        private const string CLASS_NAME = "DAO.Trending.EntityDAO";
        private const string COLUMN_PKEY = "PKEY";
        private const string COLUMN_LOCATIONKEY = "LOCATIONKEY";
        private const string COLUMN_NAME = "NAME";
        private const string COLUMN_DESCRIPTION = "DESCRIPTION";
        private const string COLUMN_LONGNAME = "LONGNAME";



        //find a datapoint entity's pkey by the given entity name, if not found, return 0.
        public ulong FindDataPointEtyKeyByName(string etyName)
        {
            string Function_Name = "GetLocationKeyByEtyName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            ulong etyPkey = 0;
            string localSQL = "select PKEY from entity where name= '"+ etyName 
                + "' AND TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint' ) " 
                + " AND DELETED = 0 ";
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                    {
                        etyPkey = Convert.ToUInt64(drReader[COLUMN_PKEY]);
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
            return etyPkey;
        }

        public ulong GetLocationKeyByEtyName(string etyName)
        {
            string Function_Name = "GetLocationKeyByEtyName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            ulong locKey = 0;
            etyName = etyName.ToUpper();
            string localSQL = "select LOCATIONKEY from entity where UPPER(name)= '" + etyName + "'";
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_LOCATIONKEY)))
                    {
                        locKey = Convert.ToUInt64(drReader[COLUMN_LOCATIONKEY]);
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
            return locKey;
        }

        public string GetLocationNameFromKey(double locationKey)
        {
            string Function_Name = "GetLocationNameFromKey";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string locationName = "";           
            string localSQL = "SELECT NAME FROM LOCATION WHERE PKEY = " + locationKey.ToString();
            System.Data.IDataReader drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                if (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(drReader.GetOrdinal("NAME")))
                    {
                        locationName = DAOHelper.convert8859P1ToGB2312(drReader["NAME"].ToString());
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
            return locationName;

        }

        /// <summary>
        /// Retrieve all data node from entity table which is the child data node of a given Entity.
        /// </summary>
        /// <param name="serverRootName">The given entity, whose child data node will be gotten.</param>
        /// <returns></returns>
        public Dictionary<ulong, EtyEntity> GetAllDataNodesByServerRootName(string serverRootName)
        {
            string Function_Name = "GetAllDataNodesByServerRootName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            Dictionary<ulong, EtyEntity> res = new Dictionary<ulong, EtyEntity>();

            string localSQL = " SELECT PKEY,NAME,DESCRIPTION " +
                " FROM ENTITY " +
                " WHERE PARENTKEY IN (SELECT DISTINCT PKEY FROM ENTITY WHERE NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(serverRootName) + "')" +
                " AND TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataNode' ) " +
                " AND DELETED = 0 ";
           /* if (checkLoc)
            {
                //localSQL += " AND LOCATIONKEY=";// + locKey.ToString;
                localSQL = string.Concat(localSQL, " AND LOCATIONKEY=", locKey);
            }*/

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATIONKEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }

            localSQL +=" ORDER BY NAME";
                

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyEntity entity = new EtyEntity();

                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                    {
                        entity.Pkey = Convert.ToUInt64(drReader[COLUMN_PKEY]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                    {
                        entity.Name = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                    {
                        entity.Description = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());
                    }

                    res.Add(entity.Pkey, entity);
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
        /// Get all the child data node by an entity pkey
        /// </summary>
        /// <param name="pkey">the parent entity pkey</param>
        /// <returns></returns>
        public Dictionary<ulong, EtyEntity> GetDataNodeChildrenByPkey(ulong pkey)
        {
            string Function_Name = "GetDataNodeChildrenByPkey";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            Dictionary<ulong, EtyEntity> res = new Dictionary<ulong, EtyEntity>();
            
            string localSQL = " SELECT PKEY,NAME,DESCRIPTION " +
                        " FROM ENTITY " +
                        " WHERE PARENTKEY = " + pkey.ToString() +
                        " AND TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataNode' ) " +
                        " AND DELETED = 0 ";

           /* if (checkLoc)
            {
                //localSQL = localSQL + " AND LOCATIONKEY=" + locKey.ToString;
                localSQL = string.Concat(localSQL, " AND LOCATIONKEY=", locKey);
            }*/

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATIONKEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }
             
            localSQL = localSQL + " ORDER BY NAME";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyEntity entity = new EtyEntity();
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                    {
                        entity.Pkey = Convert.ToUInt64(drReader[COLUMN_PKEY]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_NAME)))
                    {
                        entity.Name = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_NAME].ToString());
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                    {
                        entity.Description = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());
                    }

                    res.Add(entity.Pkey, entity);
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
        /// Get All child data point by a given entity pkey
        /// </summary>
        /// <param name="pkey">the parent entity pkey</param>
        /// <returns></returns>
        public Dictionary<ulong, EtyEntity> GetDataPointByDNPkey(ulong pkey)
        {
            string Function_Name = "GetDataPointByDNPkey";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");


            Dictionary<ulong, EtyEntity> res = new Dictionary<ulong, EtyEntity>();


            //                 localSQL = " SELECT KEYID,DATA_PT_NAME,DATA_PT_DESC,DISABLE " +
            //    " FROM OPC_DT_PT " +
            //    " WHERE PKEYID = " + parentKeyID +
            //    " AND DATA_PT_HOST = '" + opcServerHostBox.Text.Trim() + "' AND DATA_PT_SERVER = '" + serverNameBox.Text.Trim() + "'  " +
            //    " ORDER BY DATA_PT_NAME";


            string localSQL = " SELECT PKEY,CONCAT(NAME,'.Value') as LONGNAME,DESCRIPTION " +
                        " FROM ENTITY " +
                        " WHERE PARENTKEY = " + pkey.ToString() +
                        " AND TYPEKEY IN (SELECT PKEY FROM ENTITYTYPE WHERE NAME = 'DataPoint' ) " +
                        " AND DELETED = 0 ";
            /*if (checkLoc)
            {
                localSQL = string.Concat(localSQL, " AND LOCATIONKEY=", locKey);
            }*/

            string LocationClauseStr = DAOHelper.CheckLocationAndAddSQL(COLUMN_LOCATIONKEY);
            if (LocationClauseStr.Length != 0)
            {
                localSQL += " AND" + LocationClauseStr;
            }

            localSQL = localSQL + " ORDER BY NAME";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            try
            {
                while (drReader != null && drReader.Read())
                {
                    EtyEntity entity = new EtyEntity();

                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_PKEY)))
                    {
                        entity.Pkey = Convert.ToUInt64(drReader[COLUMN_PKEY]);
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_LONGNAME)))
                    {
                        entity.Name = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_LONGNAME].ToString());
                    }
                    if (!drReader.IsDBNull(drReader.GetOrdinal(COLUMN_DESCRIPTION)))
                    {
                        entity.Description = DAOHelper.convert8859P1ToGB2312(drReader[COLUMN_DESCRIPTION].ToString());
                    }

                    res.Add(entity.Pkey, entity);
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

    }
}

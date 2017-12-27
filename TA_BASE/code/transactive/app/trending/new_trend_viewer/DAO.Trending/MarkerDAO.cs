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
    /// this class is responsible for accessing table TRENDVIEWER_MARKER
    /// </summary>
    public class MarkerDAO
    {
        public const string CLASS_NAME = "MarkerDAO";

        /// <summary>
        /// get all the marker group names
        /// </summary>
        /// <returns>the marker group name list</returns>
        public List<string> GetAllMarkGrpNames()
        {
            const string Function_Name = "GetAllMarkGrpNames";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<string> markGrpNames = new List<string>();

            string localSQL = " SELECT DISTINCT CONFIG_NAME FROM TRENDVIEWER_MARKER order by config_name ";
            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);

            try
            {
                while (drReader != null && drReader.Read())
                {
                    if (!drReader.IsDBNull(0))
                        markGrpNames.Add(DAOHelper.convert8859P1ToGB2312(drReader.GetString(0).ToString()));
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
            return markGrpNames;

        }

        public bool SaveMarkerListToGrp(List<EtyMarker> markerList, string grpName)
        {
            SimpleDatabase.GetInstance().BeginTransaction();
            if (DeleteAllMarkerInGrp(grpName))
            {
                if (InsertMarkerListToGrp(markerList, grpName))
                {
                    SimpleDatabase.GetInstance().CommitTransaction();
                    return true;
                }
            }

            SimpleDatabase.GetInstance().RollbackTransaction();
            return false;
        }

        /// <summary>
        /// insert marker list to a marker group
        /// </summary>
        /// <param name="markerList">marker list</param>
        /// <param name="grpName">marker group name</param>
        public bool InsertMarkerListToGrp(List<EtyMarker> markerList, string grpName)
        {
            const string Function_Name = "InsertMarkerListToGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool executeOK = false;

            string localSQL = " INSERT INTO TRENDVIEWER_MARKER(CONFIG_NAME,MARKER_NAME,MARKER_WIDTH,MARKER_BCOLOR,MARKER_VALUE,MARKER_ENABLED,MARKER_FCOLOR) " +
                        " VALUES( '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
            foreach (EtyMarker marker in markerList)
            {
                string markerEnabledStr = "Y";
                if (!marker.MarkerEnabled)
                {
                    markerEnabledStr = "N";
                }

               executeOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL
                    + ",'" + DAOHelper.convertEscapeStringAndGB2312To8859P1(marker.MarkerName) + "'"
                    + ", " + marker.MarkerWidth.ToString()
                    + ",'" + marker.MarkerBColor + "'"
                    + ", " + marker.MarkerValue.ToString()
                    + ",'" + markerEnabledStr + "'"
                    + ",'" + marker.MarkerFColor + "'"
                    + " ) ");
               if (executeOK == false)
               {
                   break;
               }
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return executeOK;
        }

        /// <summary>
        /// delete a marker group
        /// </summary>
        /// <param name="grpName">the group name to be deleted</param>
        public bool DeleteAllMarkerInGrp(string grpName)
        {
            const string Function_Name = "DeleteAllMarkerInGrp";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            bool excuteOK = false;

            string localSQL = " DELETE FROM TRENDVIEWER_MARKER WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(grpName) + "'";
            excuteOK = SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);


            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return excuteOK;
        }

        /// <summary>
        /// get marker list by a group name
        /// </summary>
        /// <param name="configName">the marker group name</param>
        /// <returns>the marker list in the group</returns>
        public List<EtyMarker> GetMarkersByCfgName(string configName)
        {
            const string Function_Name = "GetMarkersByCfgName";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            List<EtyMarker> markerList = new List<EtyMarker>();


            string localSQL = " SELECT CONFIG_NAME, MARKER_NAME,MARKER_WIDTH,MARKER_BCOLOR,MARKER_VALUE,MARKER_ENABLED,MARKER_FCOLOR" +
                        " FROM TRENDVIEWER_MARKER WHERE CONFIG_NAME = '" + DAOHelper.convertEscapeStringAndGB2312To8859P1(configName) + "'";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            EtyMarker newEtyMarker = null;
            try
            {
                while (drReader != null && drReader.Read())
                {
                    newEtyMarker = new EtyMarker();

                    if (!drReader.IsDBNull(drReader.GetOrdinal("CONFIG_NAME")))
                        newEtyMarker.ConfigName = DAOHelper.convert8859P1ToGB2312(drReader["CONFIG_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_NAME")))
                        newEtyMarker.MarkerName = DAOHelper.convert8859P1ToGB2312(drReader["MARKER_NAME"].ToString());
                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_WIDTH")))
                        newEtyMarker.MarkerWidth = Convert.ToDouble(drReader["MARKER_WIDTH"]);  //need test
                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_BCOLOR")))
                        newEtyMarker.MarkerBColor = drReader["MARKER_BCOLOR"].ToString();
                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_VALUE")))
                        newEtyMarker.MarkerValue = Convert.ToDouble(drReader["MARKER_VALUE"]);
                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_ENABLED")))
                    {
                        string markerEnabled = drReader["MARKER_ENABLED"].ToString();
                        if (markerEnabled == "Y")
                        {
                            newEtyMarker.MarkerEnabled = true;
                        }
                        else
                        {
                            newEtyMarker.MarkerEnabled = false;
                        }
                    }

                    if (!drReader.IsDBNull(drReader.GetOrdinal("MARKER_FCOLOR")))
                        newEtyMarker.MarkerFColor = drReader["MARKER_FCOLOR"].ToString();

                    markerList.Add(newEtyMarker);
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
            return markerList;
        }
    }
}

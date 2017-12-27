using System;
using System.Collections.Generic;
using System.Text;


using STEE.ISCS.Log;
using STEE.ISCS.GenericGUI;
using Entity.Trending;
using OPCSampleGrpConfig.Common;
using OPCSampleGrpConfig.Controller;
using STEE.ISCS.MVC;
using DAO.Trending;
using DAO.Trending.Helper;

namespace OPCSampleGrpConfig.Model
{
    /// <summary>
    /// This class acts as Model for the OPCDataSelector View 
    /// and its interface between DAO Layer and Controller.
    /// </summary>
    public class OPCDataSelectorModel : IModel
    {
        /// <summary>
        /// Returns all the Sample Group in the Database in ID_Name map format
        /// </summary>
        /// <returns>Sample Group IdToName map</returns>
        public Dictionary<string, string> GetSampleGrps()
        {
            Dictionary<string, string> opcSampleGrpDic = new Dictionary<string, string>();
            Dictionary<string, string> opcDic = DatalogDPGroupTrendDAO.GetInstance().GetAllOPCGrpIdAndName(false);
            foreach (KeyValuePair<string, string> pair in opcDic)
            {
                if (!opcSampleGrpDic.ContainsKey(pair.Key))
                {
                    opcSampleGrpDic.Add(pair.Key, pair.Value);
                }
            }
            return opcSampleGrpDic;
        }

        /*public int GetTotalDataPointCount(string opcServerName, string filterString)
        {
            return OPCNodeNDataPointDAO.GetInstance().GetOPCDataPointCount(opcServerName, filterString);
        }*/

        /// <summary>
        /// Get all the DataNode from the Database of specified server root (Parent Entity)name .
        /// </summary>
        /// <param name="serverRootName">Root entity name</param>
        /// <param name="opcServerName">Server name</param>
        /// <returns>DataNode Entity List</returns>
        /*public List<EtyOPCDataNode> GetAllOPCDataNodes(string serverRootName, string opcServerName)
        {
            return OPCNodeNDataPointDAO.GetInstance().GetAllOPCDataNode(serverRootName, opcServerName);
        }*/

        public List<EtyOPCDataNode> GetAllOPCDataNodes(string serverRootName, string opcServerName)
        {
            List<EtyOPCDataNode> etyDataNodeList = new List<EtyOPCDataNode>();
            EntityDAO entityDao = new EntityDAO();
            Dictionary<ulong,EtyEntity>  dataNodelist = entityDao.GetAllDataNodesByServerRootName(serverRootName); 
            foreach (KeyValuePair<ulong, EtyEntity> pair in dataNodelist)
            {
                EtyOPCDataNode etyDataNode = new EtyOPCDataNode();
                etyDataNode.OPCDataNodeId = pair.Key;
                EtyEntity etyEntity = pair.Value;
                etyDataNode.OPCDataNodeName = etyEntity.Name;
                etyDataNode.OPCDataNodeDesc = etyEntity.Description;
                etyDataNodeList.Add(etyDataNode);
            }
            return etyDataNodeList;
        }

        /// <summary>
        /// Returns all DataPoints of specified server name matched to filterstring
        /// </summary>
        /// <param name="opcServerName">Server name</param>
        /// <param name="filterString">Filter string to be matached</param>
        /// <returns>DataPoint Entity List</returns>
        public List<EtyDataLogDPTrend> GetAllOPCDataPoints(string opcServerName, string filterString)
        {
            return DatalogDPTrendDAO.GetInstance().GetOPCDataPoint(opcServerName, filterString);
        }

        /// <summary>
        /// Returns all Child Datanode of the specified parent Datanode
        /// </summary>
        /// <param name="parentNode">parent datanode name</param>
        /// <param name="opcServerName">server name</param>
        /// <returns>Datanode entity List</returns>
        public List<EtyOPCDataNode> GetChildOPCDataNodes(string parentNode, string opcServerName)
        {
           // return OPCNodeNDataPointDAO.GetInstance().GetChildOPCDataNode(parentNode, opcServerName);
            ulong parentkey = Convert.ToUInt64(parentNode); 
            List<EtyOPCDataNode> etyDataNodeList = new List<EtyOPCDataNode>();
            EntityDAO entityDao = new EntityDAO();
            Dictionary<ulong, EtyEntity> dataNodelist = entityDao.GetDataNodeChildrenByPkey(parentkey); 
            foreach (KeyValuePair<ulong, EtyEntity> pair in dataNodelist)
            {
                EtyOPCDataNode etyDataNode = new EtyOPCDataNode();
                etyDataNode.OPCDataNodeId = pair.Key;
                EtyEntity etyEntity = pair.Value;
                etyDataNode.OPCDataNodeName = etyEntity.Name;
                etyDataNode.OPCDataNodeDesc = etyEntity.Description;
                etyDataNodeList.Add(etyDataNode);
            }

            return etyDataNodeList;

        }

        /// <summary>
        /// Updates DataPoint values in database based on either name or sample group id.
        /// </summary>
        /// <param name="etyDataPoint">DataPoint entity with updated values</param>
        public bool UpdateDataPointDB(EtyDataLogDPTrend etyDataPoint)
        {
            bool bResult = true;
            if (etyDataPoint.OPCDataPointName == null)
            {
                //clears the sample group and datapoint link for specified sample group
               bResult = DatalogDPTrendDAO.GetInstance().UpdateOPCDataPointByGrpId(etyDataPoint.Disabled, etyDataPoint.OPCSampleGrpId);
            }
            else
            {
                bResult = DatalogDPTrendDAO.GetInstance().UpdateOPCDataPointByName(etyDataPoint);
            }
            return bResult;
        }

        /// <summary>
        /// Updates the specified Sample Group Datapoint Flag in database.
        /// </summary>
        /// <param name="opcSampleGrpId">Sample Group id</param>
        /// <param name="hasDataPoint">flag value to be updated</param>
       /* public void UpdateSampleGrpFlag(string opcSampleGrpId, bool hasDataPoint)
        {
            //hasDataPoint = DatalogDPTrendDAO.GetInstance().GetOPCDataPointByGrpId(Convert.ToDouble(opcSampleGrpId));
            //DatalogDPGroupTrendDAO.GetInstance().UpdateGrpDataPointFlag(Convert.ToDouble(opcSampleGrpId), hasDataPoint);
        }*/

        /// <summary>
        /// Returns the datapoints whose name contains specified
        /// DataPtNameSubStr and belongs to specified server name.
        /// </summary>
        /// <param name="opcServerName">Server name</param>
        /// <param name="DataPtNameSubStr">DataPtNameSubStr to be matched</param>
        /// <returns></returns>
        public List<EtyDataLogDPTrend> GetDataPointsByName(string opcServerName, string DataPtNameSubStr)
        {
            List<EtyDataLogDPTrend> etyOPCDataPointList = DatalogDPTrendDAO.GetInstance().GetDataPointByName(opcServerName, DataPtNameSubStr);
            //if exceeded 2000 datapoint, return first 2000 DataPoint only.
            int totalCount = etyOPCDataPointList.Count;
            int maximumAllowed = 2000;
            if (totalCount > maximumAllowed)
            {
                etyOPCDataPointList.RemoveRange(maximumAllowed, totalCount - maximumAllowed);
            }
            return etyOPCDataPointList;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="dataPoint"></param>
        /// <returns></returns>
        public bool CheckConfiguredByOcc(string dataPoint)
        {
            bool bConfigured = false;
            double sampleGrpID = DatalogDPTrendDAO.GetInstance().GetGrpIDByDPName(dataPoint);
            if (sampleGrpID != -1)
            {
                EtyDataLogDPGroupTrend etySampleGrp = DatalogDPGroupTrendDAO.GetInstance().GetOPCGrpByID(sampleGrpID);
                if (etySampleGrp.LocationName.ToUpper() == DAOHelper.OCC_LOCATIONNAME)
                {
                    bConfigured = true;
                }
            }
            return bConfigured;
        }

        public bool IsOccLocation()
        {
            return LocationKeyHelper.GetInstance().IsOCC;          
        }

        public bool CheckStationGrp(string dataPoint, string currentGrpLocation)
        {
            bool bConfigured = true;
            if (currentGrpLocation.ToUpper() != DAOHelper.OCC_LOCATIONNAME)
            {
                if (dataPoint.Contains(".Value"))
                {
                    dataPoint = dataPoint.Remove(dataPoint.Length - 6);
                }
                EntityDAO entityDAO = new EntityDAO();
                double locationkey = entityDAO.GetLocationKeyByEtyName(dataPoint);
                string locationName = entityDAO.GetLocationNameFromKey(locationkey);
                if (locationName.ToUpper() != currentGrpLocation.ToUpper())
                {
                    bConfigured = false;
                }
            }
            return bConfigured;
        }

        public void UpdateDataPoints(EtyDataLogDPTrend sampleGrpEty, List<EtyDataLogDPTrend> selectedDataPoints)
        {
            DatalogDPTrendDAO.GetInstance().UpdateDataPoints(sampleGrpEty, selectedDataPoints);
        }

 



        
    }
}

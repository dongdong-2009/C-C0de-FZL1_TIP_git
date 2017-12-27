using System;
using System.Collections.Generic;
using System.Text;

using STEE.ISCS.MVC;
using Entity.Trending;
using DAO.Trending;
using DAO.Trending.Common;

namespace OPCSampleGrpConfig.Model
{
    /// <summary>
    /// This class acts as Model for the OPCSampleGrpConfigStart View 
    /// and its interface between DAO Layer and Controller.
    /// </summary>
    public class OPCSampleGrpConfigStartModel:IModel
    {
        //current oracle connection string
        //private string m_locationConnectionString = null;

        /// <summary>
        /// Returns whether oracle connection is opened or not
        /// </summary>
        /// <param name="connectionString">Connection string</param>
        /// <returns>true - connected, false - failed</returns>
        public bool OpenConnection()
        {
           // m_locationConnectionString = connectionString;
            return SimpleDatabase.GetInstance().IsDatabaseConnectable();//.OpenConnection(connectionString);
        }

        /// <summary>
        /// Returns the Sample Group Entity List which is sorted and indexed.
        /// </summary>
        /// <param name="sortCol">column used for sorting</param>
        /// <param name="sortingOrder">sorting order</param>
        /// <param name="lowerRecord">starting index</param>
        /// <param name="upperRecord">end index</param>
        /// <returns>Sample Groups Entity List</returns>
        public List<EtyDataLogDPGroupTrend> GetAllOPCSampelGrp(string sortCol, string sortingOrder, int lowerRecord, int upperRecord)
        {
            return DatalogDPGroupTrendDAO.GetInstance().GetAllOPCGrpsBySortNPage(sortCol, sortingOrder, lowerRecord, upperRecord);    
        }

        /// <summary>
        /// Returns all Sample Group Entity List which is sorted.
        /// </summary>
        /// <param name="sortCol"></param>
        /// <param name="sortingOrder"></param>
        /// <returns></returns>
        public List<EtyDataLogDPGroupTrend> GetAllOPCSampelGrp(string sortCol, string sortingOrder)
        {
            return DatalogDPGroupTrendDAO.GetInstance().GetAllOPCGrpsBySort(sortCol, sortingOrder);
        }


        /*public EtyOPCSampleGroup GetOPCSampleGrp(string opcSampleGrpId)
        {
            return OPCSampleGroupDAO.GetInstance().GetOPCGrpByID(opcSampleGrpId);
        }

        public bool OPCSampleGrpExist(string sampleGrpName)
        {
            return OPCSampleGroupDAO.GetInstance().IsOPCSampleGroupExist(sampleGrpName);
        }*/

        /// <summary>
        /// Insert new Sample Group entity into the database
        /// </summary>
        /// <param name="etyOPCSampleGroup">New Sampel Group entity</param>
        public void InsertOPCSampleGrp(EtyDataLogDPGroupTrend etyOPCSampleGroup)
        {
            DatalogDPGroupTrendDAO.GetInstance().InsertOPCGrpData(etyOPCSampleGroup);
        }

        /// <summary>
        /// Updates the value of the existing Sample Group in Database.
        /// </summary>
        /// <param name="etyOPCSampleGroup">Sample group entity with updated values</param>
        public void UpdateOPCSampleGrp(EtyDataLogDPGroupTrend etyOPCSampleGroup)
        {
            DatalogDPGroupTrendDAO.GetInstance().UpdateOPCGrpData(etyOPCSampleGroup);
        }

        /// <summary>
        /// Deletes Sample Group from the Database based on the ID.
        /// </summary>
        /// <param name="opcSampelGrpId"> Sample Group Id</param>
        public bool DeleteOPCSampleGrp(string opcSampelGrpId)
        {
            bool result = DatalogDPGroupTrendDAO.GetInstance().DeleteOPCSampleGroupById(opcSampelGrpId);           
            return result;
        }

        /// <summary>
        /// Returns the total Count of the Sample Group in the Database.
        /// </summary>
        /// <returns>Total count</returns>
        public int GetOPCGrpCount()
        {
            return DatalogDPGroupTrendDAO.GetInstance().GetRowCount();
        }

    }
}

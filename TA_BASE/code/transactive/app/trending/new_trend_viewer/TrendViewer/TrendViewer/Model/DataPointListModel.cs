using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using Entity.Trending;
using DAO.Trending;

namespace TrendViewer.Model
{
    public class DataPointListModel : IModel
    {
        public List<EtyDataPoint> GetDPListByGrp(string grpName)
        {
            DataPointDAO dpDAO = new DataPointDAO();
            return dpDAO.GetDPByCfgName(grpName);
        }

        public void SaveDPListToGroup(List<EtyDataPoint> dpList, string grpName)
        {
            DataPointDAO dpDAO = new DataPointDAO();

            //do this as a transaction:
            dpDAO.SaveDPListToGrp(dpList, grpName);

           // dpDAO.DeleteAllDPInGrp(grpName);
           // dpDAO.InsertDPListToGrp(dpList, grpName);

        }
    }
}

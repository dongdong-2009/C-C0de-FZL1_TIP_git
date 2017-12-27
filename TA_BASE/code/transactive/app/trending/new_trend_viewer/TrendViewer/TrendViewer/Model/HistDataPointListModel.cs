using System;
using System.Collections.Generic;
using System.Text;
using Entity.Trending;
using STEE.ISCS.MVC;
using DAO.Trending;

namespace TrendViewer.Model
{
   public class HistDataPointListModel:IModel
    {
        public List<EtyHistDataPoint> GetHistDListByGrp(string grpName)
        {
            HistDataPointDAO dpDAO = new HistDataPointDAO();
            return dpDAO.GetHistDPByCfgName(grpName);
        }

        public void SaveHistDPListToGroup(List<EtyHistDataPoint> dpList, string grpName)
        {
            //This dpList contains EtyHistDataPoint with EntityKey already
            
//             EntityDAO entityDAO = new EntityDAO();
// 
//             foreach (EtyHistDataPoint dp in dpList)
//             {
//                 string dpNameWithoutSuffix = "";
//                 if (dp.DPName.Contains(".Value"))
//                 {
//                     dpNameWithoutSuffix = dp.DPName.Remove(dp.DPName.Length - 6);
//                 }
//                 //use long, so if there is no entity found, return 0. 
//                 //There is no entity which (pkey=0 and type is DataPoint)
//                 ulong entityKey = entityDAO.FindDataPointEtyKeyByName(dpNameWithoutSuffix);
//                 
//                 dp.PKey = entityKey;  //if not found in entity table, pkey=0
//                 
//             }

            HistDataPointDAO dpDAO = new HistDataPointDAO();

            //make it as a transaction
            dpDAO.SaveHistDPListToGrp(dpList, grpName);

            //dpDAO.DeleteAllHistDPInGrp(grpName);
            //dpDAO.InsertHistDPListToGrp(dpList, grpName); //if dp.pkey=0, insert null into this column, 

        }
    }
}

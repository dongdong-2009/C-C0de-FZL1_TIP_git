using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using Entity.Trending;
using DAO.Trending;
using TrendViewer.View;

namespace TrendViewer.Model
{
   public class MarkerListModel: IModel
    {
        public List<EtyMarker> GetMarkerListByGrp(string grpName)
        {
            MarkerDAO markerDAO = new MarkerDAO(); 
            return markerDAO.GetMarkersByCfgName(grpName);
        }

        public void SaveMarkerListToGroup(List<EtyMarker> markerList, string grpName)
        {
            MarkerDAO markerDAO = new MarkerDAO();

            //do it in a transaction:
            markerDAO.SaveMarkerListToGrp(markerList, grpName);

            //markerDAO.DeleteAllMarkerInGrp(grpName);
            //markerDAO.InsertMarkerListToGrp(markerList, grpName);

        }
    }
}

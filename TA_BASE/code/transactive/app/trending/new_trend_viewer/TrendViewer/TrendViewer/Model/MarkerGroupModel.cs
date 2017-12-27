using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;

namespace TrendViewer.Model
{
    class MarkerGroupModel:IModel
    {
        public List<string> GetAllMarkGrp()
        {
            MarkerDAO markerDAO = new MarkerDAO(); ;
            return markerDAO.GetAllMarkGrpNames();
        }
    }
}

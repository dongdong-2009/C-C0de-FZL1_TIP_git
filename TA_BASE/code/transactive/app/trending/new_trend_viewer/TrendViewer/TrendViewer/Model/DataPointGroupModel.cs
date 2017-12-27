using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;

namespace TrendViewer.Model
{
    class DataPointGroupModel:IModel
    {
        public List<string> GetAllDPGrp()
        {
            DataPointDAO markerDAO = new DataPointDAO(); ;
            return markerDAO.GetAllDPGrpNames();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;

namespace TrendViewer.Model
{
    class HistDataPointGroupModel:IModel
    {
        public List<string> GetAllHistDPGrp()
        {
            HistDataPointDAO histDPDAO = new HistDataPointDAO(); ;
            return histDPDAO.GetAllHistDPGrpNames();
        }
    }
}

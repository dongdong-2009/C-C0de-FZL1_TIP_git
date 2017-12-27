using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;
using Entity.Trending;
using TrendViewer.View;

namespace TrendViewer.Model
{
   public  class TrendViewModel:IModel
    {
        public List<EtyDataLogDPLogTrend> GetHistDPLogList(EtyHistDataPoint histDP,DateTime startTime, DateTime endTime)
        {
            DataLogDPLogTrendDAO dao = new DataLogDPLogTrendDAO();
            return dao.GetHistDPLogList(histDP, startTime, endTime);
        }

    }
}

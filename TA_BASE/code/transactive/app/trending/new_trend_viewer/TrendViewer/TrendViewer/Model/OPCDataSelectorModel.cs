using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;
using DAO.Trending.Helper;
using Entity.Trending;

namespace TrendViewer.Model
{
    class OPCDataSelectorModel:IModel
    {
       public Dictionary<ulong, EtyEntity> GetDataNodeListByServerRootName(string servreRootName)
        {
            EntityDAO dao = new EntityDAO();

//            bool checkLoc = !(LocationKeyHelper.GetInstance().IsOCC);
//            ulong locKey = LocationKeyHelper.GetInstance().LocationKey;

            return dao.GetAllDataNodesByServerRootName(servreRootName);

        }

        public Dictionary<ulong, EtyEntity> GetDataNodeChildrenByPkey(ulong pkey)
        {
            EntityDAO dao = new EntityDAO();

//            bool checkLoc = !(LocationKeyHelper.GetInstance().IsOCC);
//            ulong locKey = LocationKeyHelper.GetInstance().LocationKey;

            return dao.GetDataNodeChildrenByPkey(pkey);
        }

        public Dictionary<ulong, EtyEntity> GetDataPointByDNPkey(ulong pkey)
        {
            EntityDAO dao = new EntityDAO();

//            bool checkLoc = !(LocationKeyHelper.GetInstance().IsOCC);
//            ulong locKey = LocationKeyHelper.GetInstance().LocationKey;

            return dao.GetDataPointByDNPkey(pkey);
        }

        
    }
}

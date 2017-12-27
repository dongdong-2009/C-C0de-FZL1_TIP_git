using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using DAO.Trending;

namespace TrendViewer.Model
{
    public class HistDataPointDataModel:IModel
    {
       public ulong GetEntityKeyByName(string entityName)
       {
           string etyNameWithoutSuffix = entityName;

           EntityDAO entityDAO = new EntityDAO();
           //if this dpname
           if (entityName.Contains(".Value"))
           {
               etyNameWithoutSuffix = entityName.Remove(entityName.Length - 6);
           }
           //use long, so if there is no entity found, return 0. 
           //There is no entity which (pkey=0 and type is DataPoint)
           ulong entityKey = entityDAO.FindDataPointEtyKeyByName(etyNameWithoutSuffix);

           return entityKey;  //if not found in entity table, pkey=0
       }
    }
}

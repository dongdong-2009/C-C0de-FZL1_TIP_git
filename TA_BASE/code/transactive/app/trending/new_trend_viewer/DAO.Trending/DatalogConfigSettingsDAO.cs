using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using System.Data;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    public class DatalogConfigSettingsDAO
    {
        private static DatalogConfigSettingsDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.DatalogConfigSettings";
        public static DatalogConfigSettingsDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                LogHelper.Debug(CLASS_NAME, Function_Name, "Creating static DatalogConfigSettings instance!");
                m_instance = new DatalogConfigSettingsDAO();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (m_instance != null)
            {
                m_instance = null;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public bool UpdateTrendVersionNum()
        {
            string Function_Name = "UpdateTrendVersionNum";
            List<string> sqlStrings = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                string sql = "UPDATE DATALOG_CONFIGSETTINGS SET VERSION_NUM = (VERSION_NUM+1) WHERE CONFIG_TYPE = 'Trending'";
                sqlStrings.Add(sql);
            }
            List<SqlParameter> parameters = DAOHelper.CreateEnqueneParameters(sqlStrings);
            bool bRes = true;
            bRes = SimpleDatabase.GetInstance().ExecuteEnqueneProcedure(parameters);      
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return bRes;
        }

    }
}

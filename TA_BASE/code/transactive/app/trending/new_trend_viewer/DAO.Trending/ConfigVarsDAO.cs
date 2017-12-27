using System;
using System.Collections.Generic;
using System.Text;

using Entity.Trending;
using STEE.ISCS.Log;
using DAO.Trending.Common;
using DAO.Trending.Helper;

namespace DAO.Trending
{
    /// <summary>
    /// This class is reponsible for modifying records in
    /// Config_vars table.
    /// </summary>
    public class ConfigVarsDAO
    {
        private static ConfigVarsDAO m_instance = null;
        private const string CLASS_NAME = "DAO.Trending.ConfigVarsDAO";

        public static ConfigVarsDAO GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_instance == null)
            {
                m_instance = new ConfigVarsDAO();
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

        /// <summary>
        /// Returns the value of the specified variable name
        /// </summary>
        /// <param name="varName">Variable name</param>
        /// <returns>variable value</returns>
        public string GetVarValue(string varName)
        {
            string Function_Name = "GetVarValue";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            string varValue = "null";
            string localSQL = "SELECT VAR_VALUE FROM CONFIG_VARS WHERE VAR_NAME = '" + varName + "'";

            System.Data.IDataReader  drReader = SimpleDatabase.GetInstance().ExecuteQuery(localSQL);
            if (drReader != null)
            {
                try
                {
                    while (drReader.Read())
                    {
                        if (!drReader.IsDBNull(0))
                        {
                            varValue = drReader.GetString(0).ToString();
                        }
                    }
                }
                catch (System.Exception ex)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, ex.ToString());
                }

                drReader.Close();
                drReader.Dispose();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return varValue;
        }

        /// <summary>
        /// Updates the value of the variable name.
        /// </summary>
        /// <param name="VarName">varaible name </param>
        /// <param name="VarValue">varaible value</param>
        public void UpdateValue(string VarName, string VarValue)
        {
            string Function_Name = "UpdateValue";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string localSQL = "UPDATE CONFIG_VARS SET VAR_VALUE = '" + VarValue + "' WHERE VAR_NAME = '" + VarName + "'";
            SimpleDatabase.GetInstance().ExecuteNonQuery(localSQL);
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }
    }
}

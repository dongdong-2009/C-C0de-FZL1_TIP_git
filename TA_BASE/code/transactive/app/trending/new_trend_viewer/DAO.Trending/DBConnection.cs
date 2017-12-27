using System;
using System.Collections.Generic;
using System.Text;
using Oracle.DataAccess.Client;
using Oracle.DataAccess.Types;
using System.Data;
using STEE.ISCS.Log;

namespace DAO.Trending
{
    public class DBConnection
    {
        private const string CLASS_NAME = "DAO.Trending.DBConnection";
        private OracleConnection m_conn=null;

        private static DBConnection s_DBConnection = null;

        public static DBConnection getInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (s_DBConnection==null) 
            {
                s_DBConnection = new DBConnection();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return s_DBConnection;
        }

        private DBConnection()
        {
        }

        public static void ReleaseInstance()
        {
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (s_DBConnection != null)
            {
                s_DBConnection = null;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }
        
        public bool OpenConnection(string localConnectionString)
        {
            string Function_Name = "OpenConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Entered with params: {0}", localConnectionString));

            try
            {
                 int connectionCounter = 0;
                bool boolConnected = true;
                while (boolConnected && connectionCounter < 3)
                {
                    try
                    {
                        m_conn = new OracleConnection(localConnectionString);
                        m_conn.Open();
                        boolConnected = false;
                    }
                    catch (Exception localException)
                    {
                        LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                    }
                    connectionCounter++;
                }
                if (boolConnected)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, "No Connection to Oracle Database");
                    LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
                    return false;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException);
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return true;
        }

        internal OracleConnection getConn()
        {
            return m_conn;
        }

        public void ExecuteOracleNonQuery(string query, OracleConnection localConnection)
        {
            string Function_Name = "ExecuteOracleNonQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name,"Function_Entered");

            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
            int connectionCounter = 0;
            bool boolUpdated = true;
            while (boolUpdated && connectionCounter < 3)
            {
                try
                {

                    OracleCommand cmdOracle = new OracleCommand(query, localConnection);
                    cmdOracle.ExecuteNonQuery();

                    if (cmdOracle != null)
                        cmdOracle.Dispose();

                    boolUpdated = false;

                }
                catch (Exception localException)
                {
                    LogHelper.Debug(CLASS_NAME,Function_Name,query);
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }
                connectionCounter++;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }



        public string GetStringFromQuery(string cmdQuery, OracleConnection localConnection)
        {
            string Function_Name = "GetStringFromQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Query = {0}", cmdQuery));

            string tempString = "";

            OracleCommand cmdOracle = new OracleCommand(cmdQuery, localConnection);
            OracleDataReader drOracle = null;

            try
            {
                drOracle = cmdOracle.ExecuteReader();

                while (drOracle.Read())
                {
                    if (!drOracle.IsDBNull(0))
                        tempString += drOracle.GetString(0) + ",";
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());

            }
            finally
            {
                if (cmdOracle != null)
                    cmdOracle.Dispose();
                if (drOracle != null)
                {
                    drOracle.Close();
                    drOracle.Dispose();
                }
            }
            if (tempString.Trim() != "" && tempString.LastIndexOf(",") + 1 == tempString.Length)
            {
                tempString = tempString.Substring(0, tempString.Length - 1);
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Exited with value {0}", tempString));
            return tempString;
        }


        public OracleDataReader ExecuteOracleQuery(string query, OracleConnection localConnection)
        {
            string Function_Name = "ExecuteOracleQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));

            int connectionCounter = 0;
            bool boolUpdated = true;
            while (boolUpdated && connectionCounter < 3)
            {
                try
                {

                    OracleCommand cmdOracle = new OracleCommand(query, localConnection);
                    cmdOracle.CommandType = CommandType.Text;

                    OracleDataReader drOracle = cmdOracle.ExecuteReader();

                    if (cmdOracle != null)
                        cmdOracle.Dispose();

                    boolUpdated = false;
                    LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
                    return drOracle;

                }
                catch (Exception localException)
                {

                    LogHelper.Debug(CLASS_NAME, Function_Name, query);
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }
                connectionCounter++;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return null;
        }

        public bool ExecuteAndReturnOracleNonQuery(string query, OracleConnection localConnection)
        {
            string Function_Name = "ExecuteAndReturnOracleNonQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
            int connectionCounter = 0;
            bool boolUpdated = true;
            int recordUpdated = 0;
            while (boolUpdated && connectionCounter < 3)
            {
                try
                {

                    OracleCommand cmdOracle = new OracleCommand(query, localConnection);
                    recordUpdated = cmdOracle.ExecuteNonQuery();

                    if (cmdOracle != null)
                        cmdOracle.Dispose();

                    boolUpdated = false;

                }
                catch (Exception localException)
                {
                    LogHelper.Debug(CLASS_NAME, Function_Name, query);
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }
                connectionCounter++;
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

            if (recordUpdated <= 0)
            {
                return false;
            }
            return !boolUpdated;
        }

         

    }
}

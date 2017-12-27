using System;
using System.Collections.Generic;
using System.Collections;
using STEE.ISCS.Log;
using System.Threading;
using DAO.Trending.Helper;

namespace DAO.Trending.Common
{
    public class DBConnectionStrings
    {
        private const string CLASS_NAME = "DAO.Trending.Common.DBConnectionStrings";
        private static Object s_Singleton = new Object();
        private static DBConnectionStrings s_DBConnectionStrings = null;
        private Dictionary<string,DBStatus> m_connectionStrings = null;
        private DbStatusMonitorThread m_monitorthread = null;
        private Object m_lockObj = new Object();
        private const int DATASOURCE_COUNT = 14;


        public static DBConnectionStrings GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (s_Singleton)
            {
                if (s_DBConnectionStrings == null)
                {
                    s_DBConnectionStrings = new DBConnectionStrings();
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return s_DBConnectionStrings;
        }

        private DBConnectionStrings()
        {
            m_connectionStrings = new Dictionary<string, DBStatus>();
            m_monitorthread = new DbStatusMonitorThread();
            m_monitorthread.UpdateStatusHandler += this.UpdateDBStatus;

            //thread for monitoring the offline databases.
            /*ThreadStart threadStartFunction = new ThreadStart(m_monitorthread.Run);
            Thread workerThread = new Thread(threadStartFunction);            
            workerThread.Start();
             * */
        }

        ~DBConnectionStrings()
        {
            m_connectionStrings = null;
            m_monitorthread.TerminateThread();
            m_monitorthread = null;
        }

        public static void ReleaseInstance()
        {
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            lock (s_Singleton)
            {
                if (s_DBConnectionStrings != null)
                {
                    s_DBConnectionStrings = null;
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void TerminateMonitorThread()
        {
            lock (m_lockObj)
            {
                m_monitorthread.TerminateThread();
            }
        }

        public void AddConnectionString(string connectionStr )
        {
            lock (m_lockObj)
            {
                if (connectionStr.Trim().Length != 0 && !m_connectionStrings.ContainsKey(connectionStr))
                {
                    m_connectionStrings.Add(connectionStr, DBStatus.DB_ONLINE);
                }
            }
        }

        public List<string> GetConnectionStrings()
        {
            List<string> connectionStr = new List<string>();
            lock (m_lockObj)
            {
                foreach (KeyValuePair<string, DBStatus> pair in m_connectionStrings)
                {
                    connectionStr.Add(pair.Key);
                }
            }
            return connectionStr;
        }

        public string GetConnectionString()
        {
            lock (m_lockObj)
            {
                foreach (KeyValuePair<string, DBStatus> pair in m_connectionStrings)
                {
                    if (pair.Value == DBStatus.DB_ONLINE)
                    {
                        return pair.Key;
                    }
                }
            }
            return null;
        }

        public void UpdateDBStatus(string connectionString, DBStatus dbStatus)
        {
            lock (m_lockObj)
            {
                UpdateDBStatusWithoutMonitoring(connectionString, dbStatus);

                //add to dbstatusmonitor thread.
                if (dbStatus == DBStatus.DB_OFFLINE)
                {
                    m_monitorthread.AddItem(connectionString);
                }
            }

        }

        public void UpdateDBStatusWithoutMonitoring(string connectionString, DBStatus dbStatus)
        {
            DBStatus value;
            m_connectionStrings.TryGetValue(connectionString, out value);
            if (value != dbStatus)
            {
                m_connectionStrings[connectionString] = dbStatus;
            }         
        }

        public string GetDatabaseName(string ConnectionString)
        {
            string databaseName = "";
            //remove db_type from connection string
            databaseName = ConnectionString.Substring(ConnectionString.IndexOf(DAOHelper.DB_Delimiter) + 1);
            //extract name alone
            //databaseName = databaseName.Substring(DATASOURCE_COUNT, databaseName.IndexOf(DAOHelper.DB_Delimiter) - DATASOURCE_COUNT);
            int index = databaseName.IndexOf("=") + 1;
            databaseName = databaseName.Substring(index, databaseName.IndexOf(DAOHelper.DB_Delimiter) - index);
            return databaseName;
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DAO.TimeTable.Helper;
using NET_ISCS_Bridge;

namespace DAO.TimeTable.Common
{
    internal class DatabaseSessionFactory
    {
        //todo check multiple threads
        private const string CLASS_NAME = "DAO.TimeTable.Common.DatabaseSessionFactory";
        private SessionStore m_sessionStore = null;       
        const int MAX_TIMES = 3;
       // private string m_DBSwitched = "";


        public DatabaseSessionFactory()
        {
            m_sessionStore = new SessionStore();
        }

        ~DatabaseSessionFactory()
        {
            m_sessionStore = null;
        }

        public DatabaseSession GetDatabaseSession()
        {
            string FUNCTION_NAME = "GetDatabaseSession";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 

            bool bConnected = false;
            int nTrytimes = 0;
            while (!bConnected && nTrytimes++ <= MAX_TIMES)
            {
                DatabaseSession currentSession = m_sessionStore.GetCurrentSession();
                if (currentSession != null && currentSession.IsTransactionOpen())
                {

                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Transaction open in current Session");

                    if (currentSession.CheckDatabaseConnection())
                    {
                        LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 
                        return currentSession;
                    }
                }
                else
                {
                    string connectionString = DBConnectionStrings.GetInstance().GetConnectionString();
                    if (connectionString != null)
                    {
                        DatabaseSession session = m_sessionStore.GetSession(connectionString);
                        if (session == null)
                        {
                            CreateDatabaseSession(connectionString);
                            session = m_sessionStore.GetSession(connectionString);
                        }
                        if (session.CheckDatabaseConnection())
                        {
                            /*if (m_DBSwitched.Length != 0 && (m_DBSwitched == connectionString))
                            {
                                m_DBSwitched = "";
                            }*/
                            m_sessionStore.StoreCurrentSession(session);
                            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                                EDebugLevelManaged.DebugInfo, "Function Exited"); 
                            return session;
                        }
                        else
                        {
                           // m_DBSwitched = connectionString;
                           // DBConnectionStrings.GetInstance().UpdateDBStatus(connectionString, DBStatus.DB_OFFLINE);
                         }
                    }
                }
            }
            throw new DatabaseConnectionException("No Database Connection");
        }

        public void CreateDatabaseSession(string ConnectionString)
        {
            string FUNCTION_NAME = "CreateDatabaseSession";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                 EDebugLevelManaged.DebugInfo, "Function_Entered");

            DatabaseSession session = new DatabaseSession(ConnectionString);
            m_sessionStore.Store(ConnectionString, session);

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function_Exited");
        }

        public DatabaseSession GetDatabaseSession(string connectionString)
        {
            string FUNCTION_NAME = "GetDatabaseSession";

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function_Entered");

            if (connectionString != null)
            {
                DatabaseSession session = m_sessionStore.GetSession(connectionString);
                if (session == null)
                {
                    CreateDatabaseSession(connectionString);
                    session = m_sessionStore.GetSession(connectionString);
                }
                m_sessionStore.StoreCurrentSession(session);

                LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                    EDebugLevelManaged.DebugInfo, "Function_Exited");
                return session;
            }

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                EDebugLevelManaged.DebugInfo, "Function_Exited");
            return null;
        }

        /*public string DBSwitched()
        {
            return m_DBSwitched;
        }*/

        /*public DatabaseSession GetCurrentSession()
        {
            return m_sessionStore.GetCurrentSession(); 
        }*/


    }
}

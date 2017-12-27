using System;
using System.Collections.Generic;
using System.Data.Common;
using System.Data;
using System.Runtime.Remoting.Messaging;
using DAO.TimeTable.Helper;
using NET_ISCS_Bridge;

namespace DAO.TimeTable.Common
{
    public class SimpleDatabase
    {
        private const string CLASS_NAME = "DAO.TimeTable.Common.SimpleDatabase";
        private DatabaseSessionFactory m_sessionFactory = null;

        private static SimpleDatabase s_SimpleDatabase = null;
        //static lock for instance variable todo
       
        public static SimpleDatabase GetInstance()
        {
            string FUNCTION_NAME = "GetInstance";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                         EDebugLevelManaged.DebugInfo, "Function Entered"); 

            if (s_SimpleDatabase == null)
            {
                s_SimpleDatabase = new SimpleDatabase();
            }
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Function Exited"); 
            return s_SimpleDatabase;
        }

        private SimpleDatabase()
        {
            m_sessionFactory = new DatabaseSessionFactory();
        }

        ~SimpleDatabase()
        {
            m_sessionFactory = null;
        }

        public static void ReleaseInstance()
        {
            string FUNCTION_NAME = "ReleaseInstance";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 
            if (s_SimpleDatabase != null)
            {
                s_SimpleDatabase = null;
            }
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                         EDebugLevelManaged.DebugInfo, "Function Exited"); 
        }

        public bool IsDatabaseConnectable()
        {
            try
            {
                DatabaseSession session = m_sessionFactory.GetDatabaseSession();
                return session.CheckDatabaseConnection();
            }
            catch (DatabaseConnectionException e)
            {

            }
            return false;            
        }

        public bool IsDatabaseConnectable(string ConnectionString)
        {
            DatabaseSession session = m_sessionFactory.GetDatabaseSession(ConnectionString);
            return session.CheckDatabaseConnection();
        }

        public bool OpenConnection()
        {
            string FUNCTION_NAME = "OpenConnection";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, "Function Entered"); 

            bool bRet = m_sessionFactory.GetDatabaseSession().OpenConnection();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 
            return bRet;
        }

        public bool OpenConnection(string ConnectionString)
        {
            string FUNCTION_NAME = "OpenConnection";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered"); 

            bool bRet = m_sessionFactory.GetDatabaseSession(ConnectionString).OpenConnection();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Exited"); 
            return bRet;
        }

        public void CloseConnection()
        {
            string FUNCTION_NAME = "CloseConnection";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Entered");

            m_sessionFactory.GetDatabaseSession().CloseConnection();

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited"); 
            
        }

        //public void BeginTransaction()
        //{
        //    string FUNCTION_NAME = "BeginTransaction";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    try
        //    {
        //        if (session != null)
        //        {                   
        //            session.OpenConnection();
        //            session.BeginTransaction();
        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //        throw localException;
        //    }
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");           
        //}

        //public void CommitTransaction()
        //{
        //    string FUNCTION_NAME = "CommitTransaction";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    try
        //    {
        //        if (session != null)
        //        {
        //            session.CommitTransaction();
        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //        throw localException;
        //    }
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //}


        //public void RollbackTransaction()
        //{
        //    string FUNCTION_NAME = "RollbackTransaction";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    try
        //    {
        //        if (session != null)
        //        {
        //            session.RollBackTransaction();
        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //        throw localException;
        //    }
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //}



        //public string GetStringFromQuery(string cmdQuery)
        //{
        //    string FUNCTION_NAME = "GetStringFromQuery";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Query = {0}", cmdQuery));

        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    bool bLocalSession = false;
        //    string tempString = "";
        //    try
        //    {
        //        if (session != null)
        //        {
        //            if (!session.IsConnectionOpen())
        //            {
        //                session.OpenConnection();
        //                bLocalSession = true;
        //            }
        //            IDbCommand cmd = session.CreateCommand();
        //            cmd.CommandText = cmdQuery;
        //            IDataReader drOracle = null;
        //            drOracle = cmd.ExecuteReader();
        //            while (drOracle.Read())
        //            {
        //                if (!drOracle.IsDBNull(0))
        //                    tempString += drOracle.GetString(0) + ",";
        //            }
        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //    }
        //    finally
        //    {
        //        if (bLocalSession)
        //        {
        //            session.CloseConnection();
        //        }
        //    }

        //    if (tempString.Trim() != "" && tempString.LastIndexOf(",") + 1 == tempString.Length)
        //    {
        //        tempString = tempString.Substring(0, tempString.Length - 1);
        //    }
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, string.Format("Function_Exited with value {0}", tempString));
        //    return tempString;
        //}

        //public IDataReader ExecuteQuery(string query)
        //{
        //    string FUNCTION_NAME = "ExecuteQuery";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Query = {0}", query));
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    bool bLocalSession = false;
        //    IDataReader dr = null;
        //    if (session != null)
        //    {
        //        try
        //        {
        //            if (!session.IsConnectionOpen())
        //            {
        //                session.OpenConnection();
        //                bLocalSession = true;
        //            }

        //            IDbCommand cmd = session.CreateCommand();
        //            cmd.CommandText = query;
        //            dr = cmd.ExecuteReader();

        //            LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //             return dr;
        //        }
        //        catch (Exception localException)
        //        {
        //            LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //            todo
        //             throw exception
        //        }
        //    }
        //    /*if (bLocalSession)
        //    {
        //        session.CloseConnection();
        //    }*/
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //    return dr;
        //}        

        public IDataReader ExecuteQueryWithParams(string query, List<SqlParameter> parameters)
        {
            string FUNCTION_NAME = "ExecuteQueryWithParams";
            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, "Function Entered");

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                       EDebugLevelManaged.DebugInfo, string.Format("Query = {0}", query));
           

            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            bool bLocalSession = false;
            IDataReader dr = null;
            try
                {
                     if (session != null)
                     {
                         if (!session.IsConnectionOpen())
                         {
                             session.OpenConnection();
                             bLocalSession = true;
                         }
                         query = session.ReplaceParamelimiterString(query);
                         IDbCommand cmd = session.CreateCommand();
                         cmd.CommandText = query;
                         foreach (var item in parameters)
                         {
                             IDbDataParameter parameter = cmd.CreateParameter();
                             parameter.ParameterName = item.ParameterName;
                             parameter.DbType = item.DbType;
                             parameter.Direction = item.Direction;
                             parameter.Value = item.Value;
                             cmd.Parameters.Add(parameter);
                             //cmd.Parameters.Add(item);
                         }
                         dr = cmd.ExecuteReader();


                         LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited"); 
                         //return dr;
                     }
                }
                catch (Exception localException)
                {
                    LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                        EDebugLevelManaged.DebugInfo, localException.ToString()); 
                }


            /*if (bLocalSession)
            {
                session.CloseConnection();
            }*/

            LogHelperCli.GetInstance().Log_Generic(CLASS_NAME + "." + FUNCTION_NAME, LogHelperCli.GetInstance().GetLineNumber(),
                      EDebugLevelManaged.DebugInfo, "Function Exited"); 
            return dr;
        }

        //public IDataReader ExecuteQueryWithSqls(SqlStatements queries)
        //{
        //    string FUNCTION_NAME = "ExecuteQuery";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    string query;
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    bool bLocalSession = false;
        //    IDataReader dr = null;
        //    if (session != null)
        //    {
        //        try
        //        {
        //            if (!session.IsConnectionOpen())
        //            {
        //                session.OpenConnection();
        //                bLocalSession = true;
        //            }

        //            IDbCommand cmd = session.CreateCommand();
        //            if (session.GetDBType() == DBType.Oracle)
        //                query = queries.OracleSql;
        //            else
        //                query = queries.MySqlStr;
        //            LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Query = {0}", query));
        //            cmd.CommandText = query;
        //            dr = cmd.ExecuteReader();

        //             LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //             return dr;
        //        }
        //        catch (Exception localException)
        //        {
        //            LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //            todo
        //             throw exception
        //        }
        //    }
        //    /*if (bLocalSession)
        //    {
        //        session.CloseConnection();
        //    }*/
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");
        //    return dr;
        //}        


        //public bool ExecuteNonQuery(string query)
        //{
        //    string FUNCTION_NAME = "ExecuteNonQuery";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");
        //    LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Query = {0}", query));
        //    int connectionCounter = 0;
        //    bool bLocalSession = false;
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    int recordUpdated = 0;
        //    bool boolUpdated = true;

        //    try
        //    {
        //        if (session != null)
        //        {
        //            if (!session.IsConnectionOpen())
        //            {
        //                session.OpenConnection();
        //                bLocalSession = true;
        //            }
        //            IDbCommand cmd = session.CreateCommand();
        //            cmd.CommandText = query;
        //            recordUpdated = cmd.ExecuteNonQuery();
        //            boolUpdated = false;

        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //    }
        //    if (bLocalSession)
        //    {
        //        session.CloseConnection();
        //    }

        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");

        //    if (recordUpdated <= 0)
        //    {
        //        return false;
        //    }

        //    return !boolUpdated;
        //}

        //public bool ExecuteNonQueryWithParams(string query, List<SqlParameter> parameters)
        //{
        //    string FUNCTION_NAME = "ExecuteNonQueryWithParams";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");

        //    LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Query = {0}", query));
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    bool bLocalSession = false;
        //    int recordUpdated = 0;
        //    bool boolUpdated = true;

        //        try
        //        {
        //             if (session != null)
        //             {
        //                 if (!session.IsConnectionOpen())
        //                 {
        //                     session.OpenConnection();
        //                     bLocalSession = true;
        //                 }
        //                 IDbCommand cmd = session.CreateCommand();
        //                 Replace Parameter delimiter based on the type
        //                 query = session.ReplaceParamelimiterString(query);
        //                 cmd.CommandText = query;
        //                 foreach (var item in parameters)
        //                 {
        //                     IDbDataParameter parameter = cmd.CreateParameter();
        //                     parameter.ParameterName = item.ParameterName;
        //                     parameter.DbType = item.DbType;
        //                     parameter.Direction = item.Direction;
        //                     parameter.Value = item.Value;
        //                     cmd.Parameters.Add(parameter);
        //                     cmd.Parameters.Add(item);
        //                 }
        //                 recordUpdated = cmd.ExecuteNonQuery();
        //                 boolUpdated = false;

        //             }
        //        }
        //        catch (Exception localException)
        //        {
        //            LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //        }

        //    if (bLocalSession)
        //    {
        //        session.CloseConnection();
        //    }
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");

        //     if (recordUpdated <= 0)
        //     {
        //         return false;
        //     }
        //    return !boolUpdated;
        //}

        //public bool ExecuteEnqueneProcedure(List<SqlParameter> parameters)
        //{
        //    string FUNCTION_NAME = "ExecuteProcedure";
        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");

        //    bool bLocalSession = false;
        //    DatabaseSession session = m_sessionFactory.GetDatabaseSession();
        //    bool boolUpdated = true;

        //    try
        //    {
        //        if (session != null)
        //        {
        //            if (!session.IsConnectionOpen())
        //            {
        //                session.OpenConnection();
        //                bLocalSession = true;
        //            }
        //            IDbCommand cmd = session.CreateCommand();
        //            cmd.CommandType = CommandType.StoredProcedure;
        //            string procName;
        //            if (session.GetDBType() == DBType.Oracle)
        //                procName = DAOHelper.ORACLE_AUDIT_PROCEDURE_NAME;
        //            else
        //                procName = DAOHelper.MYSQL_AUDIT_PROCEDURE_NAME;

        //            cmd.CommandText = procName;
        //            LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, "SQL Parameters for Enquene procedure are : ");

        //            foreach (var item in parameters)
        //            {
        //                IDbDataParameter parameter = cmd.CreateParameter();
        //                parameter.ParameterName = item.ParameterName;
        //                parameter.Direction = item.Direction;
        //                parameter.Value = item.Value;
        //                LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, String.Format("{0}\n", item.Value));
        //                cmd.Parameters.Add(parameter);
        //            }

        //            LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, String.Format("Command Text: {0}", cmd.CommandText));

        //            cmd.ExecuteNonQuery();
        //            boolUpdated = false;
        //        }
        //    }
        //    catch (Exception localException)
        //    {
        //        LogHelper.Error(CLASS_NAME, FUNCTION_NAME, localException.ToString());
        //    }
        //    if (bLocalSession)
        //    {
        //        session.CloseConnection();
        //    }

        //    LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Exited");

        //    return !boolUpdated;
        //}

    }
}

using System;
using System.Collections.Generic;
using System.Data.Common;
using STEE.ISCS.Log;
using System.Data;
using System.Runtime.Remoting.Messaging;
using DAO.Trending.Helper;

namespace DAO.Trending.Common
{
    public class SimpleDatabase
    {
        private const string CLASS_NAME = "DAO.Trending.Common.SimpleDatabase";
        private DatabaseSessionFactory m_sessionFactory = null;

        private static SimpleDatabase s_SimpleDatabase = null;
        //static lock for instance variable todo
       
        public static SimpleDatabase GetInstance()
        {
            string Function_Name = "GetInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (s_SimpleDatabase == null)
            {
                s_SimpleDatabase = new SimpleDatabase();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
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
            string Function_Name = "ReleaseInstance";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (s_SimpleDatabase != null)
            {
                s_SimpleDatabase = null;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
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
            string Function_Name = "OpenConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            bool bRet = m_sessionFactory.GetDatabaseSession().OpenConnection();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return bRet;
        }

        public bool OpenConnection(string ConnectionString)
        {
            string Function_Name = "OpenConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            bool bRet = m_sessionFactory.GetDatabaseSession(ConnectionString).OpenConnection();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return bRet;
        }

        public void CloseConnection()
        {
            string Function_Name = "CloseConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            m_sessionFactory.GetDatabaseSession().CloseConnection();

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            
        }

        public void BeginTransaction()
        {
            string Function_Name = "BeginTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            try
            {
                if (session != null)
                {                   
                    session.OpenConnection();
                    session.BeginTransaction();
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                throw localException;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");           
        }

        public void CommitTransaction()
        {
            string Function_Name = "CommitTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            try
            {
                if (session != null)
                {                    
                    session.CommitTransaction();
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                throw localException;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");         
        }


        public void RollbackTransaction()
        {
            string Function_Name = "RollbackTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            try
            {
                if (session != null)
                {
                    session.RollBackTransaction();
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                throw localException;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }



        public string GetStringFromQuery(string cmdQuery)
        {
            string Function_Name = "GetStringFromQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", cmdQuery));

            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            bool bLocalSession = false;
            string tempString = "";             
            try
            {
                if (session != null)
                {
                    if (!session.IsConnectionOpen())
                    {
                        session.OpenConnection();
                        bLocalSession = true;
                    }
                    IDbCommand cmd = session.CreateCommand();
                    cmd.CommandText = cmdQuery;
                    IDataReader drOracle = null;
                    drOracle = cmd.ExecuteReader();
                    while (drOracle.Read())
                    {
                        if (!drOracle.IsDBNull(0))
                            tempString += drOracle.GetString(0) + ",";
                    }
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }
            finally
            {
                if (bLocalSession)
                {
                    session.CloseConnection();
                }
            }

            if (tempString.Trim() != "" && tempString.LastIndexOf(",") + 1 == tempString.Length)
            {
                tempString = tempString.Substring(0, tempString.Length - 1);
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, string.Format("Function_Exited with value {0}", tempString));
            return tempString;
        }

        public IDataReader ExecuteQuery(string query)
        {
            string Function_Name = "ExecuteQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
           
            IDataReader dr = null;
            if (session != null)
            {
                try
                {
                    if (!session.IsConnectionOpen())
                    {
                        session.OpenConnection();
                        
                    }

                    IDbCommand cmd = session.CreateCommand();
                    cmd.CommandText = query;
                    dr = cmd.ExecuteReader();

                    // LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
                    // return dr;
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                    //todo
                    // throw exception
                }
            }
            /*if (bLocalSession)
            {
                session.CloseConnection();
            }*/
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return dr;
        }        

        public IDataReader ExecuteQueryWithParams(string query, List<SqlParameter> parameters)
        {
            string Function_Name = "ExecuteQueryWithParams";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));

            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
          
            IDataReader dr = null;
            try
                {
                     if (session != null)
                     {
                         if (!session.IsConnectionOpen())
                         {
                             session.OpenConnection();
                          
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


                         //LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
                         //return dr;
                     }
                }
                catch (Exception localException)
                {                  
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }


            /*if (bLocalSession)
            {
                session.CloseConnection();
            }*/

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return dr;
        }

        public IDataReader ExecuteQueryWithSqls(SqlStatements queries)
        {
            string Function_Name = "ExecuteQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            string query;
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            
            IDataReader dr = null;
            if (session != null)
            {
                try
                {
                    if (!session.IsConnectionOpen())
                    {
                        session.OpenConnection();
                       
                    }

                    IDbCommand cmd = session.CreateCommand();
                    if (session.GetDBType() == DBType.Oracle)
                        query = queries.OracleSql;
                    else
                        query = queries.MySqlStr;
                    LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
                    cmd.CommandText = query;
                    dr = cmd.ExecuteReader();

                    // LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
                    // return dr;
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                    //todo
                    // throw exception
                }
            }
            /*if (bLocalSession)
            {
                session.CloseConnection();
            }*/
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return dr;
        }        


        public bool ExecuteNonQuery(string query)
        {
            string Function_Name = "ExecuteNonQuery";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
           // int connectionCounter = 0;
            bool bLocalSession = false;
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            int recordUpdated = 0;
            bool boolUpdated = true;

                try
                {
                     if (session != null)
                     {
                         if (!session.IsConnectionOpen())
                         {
                             session.OpenConnection();
                             bLocalSession = true;
                         }
                        IDbCommand cmd = session.CreateCommand();
                         cmd.CommandText = query;
                         recordUpdated = cmd.ExecuteNonQuery();
                         boolUpdated = false;

                     }
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }
            if (bLocalSession)
            {
                session.CloseConnection();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

//             if (recordUpdated <= 0)
//             {
//                 return false;
//             }

            return !boolUpdated;
        }

        public bool ExecuteNonQueryWithParams(string query, List<SqlParameter> parameters)
        {
            string Function_Name = "ExecuteNonQueryWithParams";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Query = {0}", query));
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            bool bLocalSession = false;
            int recordUpdated = 0;
            bool boolUpdated = true;

                try
                {
                     if (session != null)
                     {
                         if (!session.IsConnectionOpen())
                         {
                             session.OpenConnection();
                             bLocalSession = true;
                         }
                         IDbCommand cmd = session.CreateCommand();
                         //Replace Parameter delimiter based on the type
                         query = session.ReplaceParamelimiterString(query);
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
                         recordUpdated = cmd.ExecuteNonQuery();
                         boolUpdated = false;

                     }
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }

            if (bLocalSession)
            {
                session.CloseConnection();
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

//             if (recordUpdated <= 0)
//             {
//                 return false;
//             }
            return !boolUpdated;
        }

        public bool ExecuteEnqueneProcedure(List<SqlParameter> parameters)
        {
            string Function_Name = "ExecuteProcedure";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
        
            bool bLocalSession = false;
            DatabaseSession session = m_sessionFactory.GetDatabaseSession();
            bool boolUpdated = true;

            try
            {
                if (session != null)
                {
                    if (!session.IsConnectionOpen())
                    {
                        session.OpenConnection();
                        bLocalSession = true;
                    }
                    IDbCommand cmd = session.CreateCommand();
                    cmd.CommandType = CommandType.StoredProcedure;
                    string procName;
                    if (session.GetDBType() == DBType.Oracle)
                        procName = DAOHelper.ORACLE_AUDIT_PROCEDURE_NAME;
                    else
                        procName = DAOHelper.MYSQL_AUDIT_PROCEDURE_NAME;

                    cmd.CommandText = procName;
                    LogHelper.Debug(CLASS_NAME, Function_Name, "SQL Parameters for Enquene procedure are : ");

                    foreach (var item in parameters)
                    {
                        IDbDataParameter parameter = cmd.CreateParameter();
                        parameter.ParameterName = item.ParameterName;
                        parameter.Direction = item.Direction;
                        parameter.Value = item.Value;
                        LogHelper.Debug(CLASS_NAME, Function_Name, String.Format("{0}\n", item.Value));
                        cmd.Parameters.Add(parameter);
                    }

                    LogHelper.Debug(CLASS_NAME, Function_Name, String.Format("Command Text: {0}", cmd.CommandText));

                    cmd.ExecuteNonQuery();
                    boolUpdated = false;
                }
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
            }
            if (bLocalSession)
            {
                session.CloseConnection();
            }

            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");

            return !boolUpdated;
        }

    }
}

using System;
using System.Collections.Generic;
using System.Data;
using STEE.ISCS.Log;

using DAO.Trending.Helper;

namespace DAO.Trending.Common
{
    public class DatabaseSession
    {
        private IDbConnection m_connection = null;
        private IDbTransaction m_transaction = null;
        //private bool m_IsExplicitOpen = false;
        private const string CLASS_NAME = "DAO.Trending.Common.DatabaseSession";
        private int m_transactionRefCnt = 0;
        private DBType m_dbType = DBType.Oracle;
        private string m_ConnectionString = null;
        private const string CONNECTION_POOLING_STRING = "Pooling=true;Max Pool Size=1;";

        public DatabaseSession(string connectionString)
        {
            string FUNCTION_NAME = "DatabaseSession";
            LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");

            LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Database Connection String = {0}",connectionString));

            m_dbType = GetDatabaseTypeFromString(connectionString);
            m_ConnectionString = connectionString.Substring(connectionString.IndexOf(DAOHelper.DB_Delimiter) + 1) + CONNECTION_POOLING_STRING;
        }

        public DBType GetDBType()
        {
            return m_dbType;
        }

        private System.Data.IDbConnection GetConnection(DBType dbType)
        {
            if (dbType == DBType.Oracle)
            {
                return new Oracle.DataAccess.Client.OracleConnection();
            }
            else
            {
                return new MySql.Data.MySqlClient.MySqlConnection();
            }
        }

        private DBType GetDatabaseTypeFromString(string databaseConnectionString)
        {
            string FUNCTION_NAME = "GetDatabaseTypeFromString";
            LogHelper.Trace(CLASS_NAME, FUNCTION_NAME, "Function_Entered");

            //connection string will have dbtype at the first of the string with ';' as delimiter.
            string dbTypeString = databaseConnectionString.Substring(0, databaseConnectionString.IndexOf(DAOHelper.DB_Delimiter));
            DBType dbType = DAOHelper.GetDbType(dbTypeString);
            LogHelper.Debug(CLASS_NAME, FUNCTION_NAME, string.Format("Database Type = {0}",dbType.ToString()));
            return dbType;
        }

        public bool CheckDatabaseConnection()
        {
            string Function_Name = "CheckDatabaseConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
                        
           // int connectionCounter = 0;
            bool boolConnected = false;
            //while (!boolConnected && connectionCounter < 3)
            //{
                try
                {
                    if (m_connection == null)
                    {
                        m_connection = GetConnection(m_dbType);
                        //set the connection string
                        m_connection.ConnectionString = m_ConnectionString;
                        //open the connection
                        m_connection.Open();
                        m_connection.Close();
                    }
                    else
                    {
                        if (m_connection.State != ConnectionState.Open)
                        {
                            m_connection.Open();
                            m_connection.Close();
                        }
                        else
                        {
                            if (m_transaction == null)
                            {
                                m_connection.Close();
                                m_connection.Open();
                                m_connection.Close();
                            }
                        }
                    }
                    boolConnected = true;
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());
                }
             //   connectionCounter++;
            //}
            
            LogHelper.Debug(CLASS_NAME, Function_Name, string.Format("Database {0} Status = {1}", m_ConnectionString, boolConnected ? "ONLINE" : "OFFLINE"));
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return boolConnected;
        }

        public bool OpenConnection()
        {
            string Function_Name = "OpenConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            int connectionCounter = 0;
            bool boolConnected = false;
            while (!boolConnected && connectionCounter < 3)
            {
                try
                {
                    if (m_connection == null)
                    {
                        m_connection = GetConnection(m_dbType);
                        //set the connection string
                        m_connection.ConnectionString = m_ConnectionString;
                        //open the connection
                        m_connection.Open();                      
                    }
                    else
                    {
                        if (m_connection.State != ConnectionState.Open)
                        {
                            m_connection.Open();
                        }
                    }
                    //m_IsExplicitOpen = true;
                    boolConnected = true;
                }
                catch (Exception localException)
                {
                    LogHelper.Error(CLASS_NAME, Function_Name, localException.ToString());                    
                }
                connectionCounter++;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
            return boolConnected;
        }


        public void CloseConnection()
        {
            string Function_Name = "CloseConnection";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");
            if (m_transaction == null)
            {
                if ((m_connection != null) && (m_connection.State != ConnectionState.Closed))
                {
                    m_connection.Close();
                    m_connection.Dispose();
                }
                m_connection = null;
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");
        }

        public void BeginTransaction()
        {
            string Function_Name = "BeginTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_transaction != null)
            {
                m_transactionRefCnt++;
            }
            else
            {
                if (m_connection != null)
                {
                    m_transaction = m_connection.BeginTransaction();
                    m_transactionRefCnt++;
                }
                else
                {
                    throw new Exception("No Connection object created");
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");            
        }

        public void CommitTransaction()
        {
            string Function_Name = "CommitTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_transaction == null)
            {
                throw new Exception("Tracsaction cannot be null");
            }
            else
            {
                m_transactionRefCnt--;
                if (m_transactionRefCnt == 0)
                {
                    m_transaction.Commit();
                    m_transaction.Dispose();
                    m_transaction = null;
                    if (m_connection.State != ConnectionState.Closed)
                    {
                        CloseConnection();
                    }
                }               
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited");        
        }

        public void RollBackTransaction()
        {
            string Function_Name = "RollBackTransaction";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_transaction == null)
            {
                throw new Exception("Tracsaction cannot be null");
            }
            else
            {
                m_transactionRefCnt--;
                if (m_transactionRefCnt == 0)
                {
                    m_transaction.Rollback();
                    m_transaction.Dispose();
                    m_transaction = null;
                    if (m_connection.State != ConnectionState.Closed)
                    {
                        CloseConnection();
                    }
                }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 
        }


        public IDbCommand CreateCommand()
        {
            string Function_Name = "CreateCommand";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            IDbCommand command = m_connection.CreateCommand();
            // Assign transaction
            if (m_transaction != null)
            {
                try
                {
                    command.Transaction = m_transaction;
                }
                catch
                { }
            }
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Exited"); 

            return command;
        }



        public bool IsTransactionOpen()
        {
            string Function_Name = "IsTransactionOpen";
            LogHelper.Trace(CLASS_NAME, Function_Name, "Function_Entered");

            if (m_transaction != null)
                return true;
            else
                return false;
        }


        /*public bool IsExplicitOpen()
        {
            return m_IsExplicitOpen;
        }*/

        public bool IsConnectionOpen()
        {
            return m_connection.State == ConnectionState.Open ? true : false;
        }


        public string GetParameterDelimiter()
        {
            if (m_dbType == DBType.Oracle)
                return DAOHelper.DB_Oracle_ParameterDelimiter;
            else
                return DAOHelper.DB_Mysql_ParameterDelimiter;
        }

        public string ReplaceParamelimiterString(string OriginalQuery)
        {
            string replaceQuery = "";
            replaceQuery =  OriginalQuery.Replace("@", GetParameterDelimiter());
            return replaceQuery;
        }


    }
}

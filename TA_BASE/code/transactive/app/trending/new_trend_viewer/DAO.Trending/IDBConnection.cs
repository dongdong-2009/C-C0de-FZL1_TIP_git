using System;
using System.Data;
using System.Data.Common;
using System.Collections.Generic;

namespace DAO.Trending
{
    public  abstract class  IDBConnection
    {
        //public abstract void ExecuteNonQuery(string query);
        public abstract string GetStringFromQuery(string cmdQuery);
        public abstract IDataReader ExecuteQuery(string query);
        public abstract IDataReader ExecuteQueryWithParams(string query, List<SqlParameter> parameters);
        public abstract bool ExecuteNonQuery(string query);
        public abstract bool ExecuteNonQueryWithParams(string query, List<SqlParameter> parameters);

    }
}

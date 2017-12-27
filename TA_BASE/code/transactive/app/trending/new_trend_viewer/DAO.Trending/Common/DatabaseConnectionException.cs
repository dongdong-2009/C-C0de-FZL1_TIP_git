using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DAO.Trending.Common
{
    public class DatabaseConnectionException : Exception
    {
        //Constructors. 
        public DatabaseConnectionException() : base() { }
        public DatabaseConnectionException(string message) : base(message) { }
        public DatabaseConnectionException(string message, Exception e) : base(message, e) { }
        //If there is extra error information that needs to be captured
        //create properties for them.
        private string strExtraInfo;
        public string ExtraErrorInfo
        {
            get
            {
                return strExtraInfo;
            }

            set
            {
                strExtraInfo = value;
            }
        }
    }
}

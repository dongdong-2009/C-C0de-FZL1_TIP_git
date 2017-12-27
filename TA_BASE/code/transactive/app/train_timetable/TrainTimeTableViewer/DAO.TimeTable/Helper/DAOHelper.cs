using System;
using System.Collections.Generic;
using System.Text;

namespace DAO.TimeTable.Helper
{
    public enum DBType // database type
    {
        Oracle,
        MySql
    }

    public enum DBStatus //database status
    {
        DB_ONLINE,
        DB_OFFLINE
    }
    public class SqlParameter
    {
        public string ParameterName;
        public System.Data.DbType DbType;
        public object Value;
        public System.Data.ParameterDirection Direction;

        public SqlParameter()
        {
        }

        public SqlParameter(string parameterName, System.Data.ParameterDirection direction, object value, System.Data.DbType type)
        {
            this.ParameterName = parameterName;
            this.Direction = direction;
            this.DbType = type;
            this.Value = value;
        }

        public SqlParameter(string parameterName, System.Data.ParameterDirection direction, object value)
        {
            this.ParameterName = parameterName;
            this.Direction = direction;
            this.Value = value;
        }
    }

    public class SqlStatements
    {
        public string OracleSql;
        public string MySqlStr;
    }
    
    public class DAOHelper
    {
        private const string CLASS_NAME = "DAO.TimeTable.DAOHelper";

        public const string SECOND = "S";
        public const string MINITE = "MI";
        public const string HOUR = "H";


        public const string DB_LineType_Line = "Line";  //Line type (Line) string saved in DB
        public const string DB_LineType_Spline = "Spline"; //Line type (Spline) string saved in DB


        public const string DB_Type_Oracle = "ORACLE";  
        public const string DB_Type_MySql = "MYSQL";
        public const char DB_Delimiter = ';';
        public const string DB_Oracle_ParameterDelimiter = ":";
        public const string DB_Mysql_ParameterDelimiter = "@";

        public const string ORACLE_AUDIT_PROCEDURE_NAME = "audit_data_queue_pkg.enqueue_audit_data";
        public const string MYSQL_AUDIT_PROCEDURE_NAME = "prc_enqueue_audit_data";

        public const string ORACLE_SEQ_FUNC_SQL = "SELECT DATALOG_DP_GRP_TREND_PKEY.NEXTVAL FROM DUAL";
        public const string MYSQL_SEQ_FUNC_SQL = "SELECT func_seq_get_next_number('DATALOG_DP_GRP_TREND_SEQ')";

        public const string AUDIT_PROC_PARAMETER_NAME1 = "@p_location";
        public const string AUDIT_PROC_PARAMETER_NAME2 = "@p_message_qualifier";
        public const string AUDIT_PROC_PARAMETER_NAME3 = "@p_audit_data_Oracle";
        public const string AUDIT_PROC_PARAMETER_NAME4 = "@p_audit_data_MySQL";

        public const string ORACLE_DATE_FORMAT = "DD/MM/YYYY HH24:MI:SS";
        public const string MYSQL_DATE_FORMAT = "%d/%m/%Y %H:%i:%S";

        public const string ORACLE_DATE_FUNCTION = "TO_DATE";
        public const string MYSQL_DATE_FUNCTION = "STR_TO_DATE";

        public const string MESSAGE_QUALIFIER_CONST = "PUBLIC";
        
        public const string OCC_LOCATIONNAME = "OCC";


        private static bool s_EncodingChange = false;

        public static void SetEncodingChange (bool encodingChange)
        {
            s_EncodingChange = encodingChange;
        }
        public static bool GetEncodingChange ()
        {
            return s_EncodingChange;
        }

        ///// <summary>
        ///// convert LineType from enum LineType to string format for DB saving.
        ///// </summary>
        ///// <param name="lineType"></param>
        ///// <returns></returns>
        //public static string ConvertLineTypeToDBString(LineType lineType)
        //{
        //    if (lineType == LineType.Line)
        //    {
        //        return DB_LineType_Line;
        //    }
        //    if (lineType == LineType.Spline)
        //    {
        //        return DB_LineType_Spline;
        //    }
        //    else return DB_LineType_Line;
        //}

        ///// <summary>
        ///// convert line type from db saving format to enum LineType
        ///// </summary>
        ///// <param name="lineType"></param>
        ///// <returns></returns>
        //public static LineType ConvertLineTypeToEnum (String lineType)
        //{
        //    if(lineType == DB_LineType_Line)
        //    {
        //        return LineType.Line;
        //    }
        //    if(lineType == DB_LineType_Spline)
        //    {
        //        return LineType.Spline;
        //    }
        //    return LineType.Line;
        //}

        /// <summary>
        /// execute non-query sql
        /// </summary>
        /// <param name="sql">sql statement</param>
       /* public static void ExecuteNonQuery(string sql)
        {
            string FUNCTION_NAME = "ExecuteNonQuery";
            OracleConnection cnn = DBConnection.getInstance().getConn();
            OracleCommand cmdOracle = new OracleCommand(sql, cnn);
            try
            {
                cmdOracle.ExecuteNonQuery();
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME,FUNCTION_NAME ,localException.ToString());
            }
            finally
            {
                if (cmdOracle != null)
                    cmdOracle.Dispose();
            }
        }*/

        
        /// <summary>
        /// if s_EncodingChange is true, convert encoding of a given string from "iso-8859-1" to  "gb2312"
        ///  else, just return the same string
        /// </summary>
        /// <param name="s">the string to be converted</param>
        /// <returns></returns>
        public static string convert8859P1ToGB2312(string s)
        {
            if(s_EncodingChange)
            {
                //return System.Text.Encoding.Default.GetString(System.Text.Encoding.GetEncoding("iso-8859-1").GetBytes(s));
                return System.Text.Encoding.GetEncoding("gb2312").GetString(System.Text.Encoding.GetEncoding("iso-8859-1").GetBytes(s));
            }
            else
            {
                return s;
            }
            
        }

        /// <summary>
        /// if s_EncodingChange is true, convert encoding of a given string from "gb2312" to "iso-8859-1" 
        /// else, just return the same string
        /// in 3001, no need to convert. so just return the old string
        /// </summary>
        /// <param name="str">the string to be converted</param>
        /// <returns></returns>
        public static string convertEscapeStringAndGB2312To8859P1(string str)
        {
            str = convertEscapeString(str);
            if(s_EncodingChange == true)
            {
                //Encoding gbk_encoder = Encoding.Default; //GBK 
                Encoding gbk_encoder = Encoding.GetEncoding("gb2312");
                byte[] bs = gbk_encoder.GetBytes(str);
                return Encoding.GetEncoding("iso-8859-1").GetString(bs);
            }
            else
            {
                return str;
            }            
        }

        /// <summary>
        /// This function converts "'" Escape string found in the input string to "''"
        /// </summary>
        /// <param name="str">input string</param>
        /// <returns>converted string</returns>
        public static string convertEscapeString(string str)
        {
            string escapedString = str;
            string singleQuote = "'";
            if (escapedString.Length != 0)
            {
                int pos = escapedString.IndexOf(singleQuote);
                while (-1 != pos)
                {
                    escapedString = escapedString.Insert(pos + 1, singleQuote);
                    if ((pos + 2) < escapedString.Length)
                        pos = escapedString.IndexOf("'", pos + 2);
                    else
                        pos = -1;
                }
            }
            return escapedString;
        }

        /// <summary>
        /// change string "Y"/"N" to boolean true/false
        /// </summary>
        /// <param name="str">"Y" or "N"</param>
        /// <returns>boolean value</returns>
        public static bool ChangeStrToBool(string str)
        {
            if ("Y" == str || "y"==str)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// change boolean value to string "Y"/"N"
        /// </summary>
        /// <param name="b">boolean value</param>
        /// <returns>"Y" or "N"</returns>
        public static string ChangeBoolToStr(bool b)
        {
            if (b == true)
            {
                return "Y";
            }
            else
            {
                return "N";
            }
        }

        public static DBType GetDbType(string dbType)
        {
            DBType ret = DBType.MySql;
            if (dbType.ToUpper().Equals(DB_Type_Oracle))
            {
                ret = DBType.Oracle;
            }
            return ret;                                   
        }

        public static string CheckLocationAndAddSQL(string LocationColName)
        {
            string locationClauseStr = "";
            if (!LocationKeyHelper.GetInstance().IsOCC)
            {
                locationClauseStr = " "+LocationColName+ "= " + LocationKeyHelper.GetInstance().LocationKey;
            }
            return locationClauseStr;
        }


        public static string GetEnabledStr(string ChangedBoolToStr)
        {
            string sEnabled = ChangedBoolToStr;
            if (sEnabled == "Y")
                sEnabled = "N";
            else
                sEnabled = "Y";
            return sEnabled;
        }

        public static List<SqlParameter> CreateEnqueneParameters(List<string> parameterValues)
        {
            if (parameterValues.Count != 2)
            {
                throw new Exception("invalid Enquene parameters count");
            }
            List<SqlParameter> parameters = new List<SqlParameter>();
            SqlParameter parameter1 = new SqlParameter(DAOHelper.AUDIT_PROC_PARAMETER_NAME1, System.Data.ParameterDirection.Input, DAO.TimeTable.Common.DBConnectionStrings.GetInstance().GetDatabaseName(DAO.TimeTable.Common.DBConnectionStrings.GetInstance().GetConnectionString()));
            parameters.Add(parameter1);
            SqlParameter parameter2 = new SqlParameter(DAOHelper.AUDIT_PROC_PARAMETER_NAME2, System.Data.ParameterDirection.Input, DAOHelper.MESSAGE_QUALIFIER_CONST);
            parameters.Add(parameter2);
            SqlParameter parameter3 = new SqlParameter(DAOHelper.AUDIT_PROC_PARAMETER_NAME3, System.Data.ParameterDirection.Input, parameterValues[0]);
            parameters.Add(parameter3);
            SqlParameter parameter4 = new SqlParameter(DAOHelper.AUDIT_PROC_PARAMETER_NAME4, System.Data.ParameterDirection.Input, parameterValues[1]);
            parameters.Add(parameter4);
            return parameters;
        }



    }
}

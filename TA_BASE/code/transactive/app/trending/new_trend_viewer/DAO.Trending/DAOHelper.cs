using System;
using System.Collections.Generic;
using System.Text;
using Oracle.DataAccess.Client;
using STEE.ISCS.Log;
using Entity.Trending;

namespace DAO.Trending
{


    public class DAOHelper
    {
        private const string CLASS_NAME = "DAO.Trending.DAOHelper";

        public const string SECOND = "S";
        public const string MINITE = "M";
        public const string HOUR = "H";


        public const string DB_LineType_Line = "Line";  //Line type (Line) string saved in DB
        public const string DB_LineType_Spline = "Spline"; //Line type (Spline) string saved in DB

        private static bool s_EncodingChange = false;

        public static void SetEncodingChange (bool encodingChange)
        {
            s_EncodingChange = encodingChange;
        }
        public static bool GetEncodingChange ()
        {
            return s_EncodingChange;
        }

        /// <summary>
        /// convert LineType from enum LineType to string format for DB saving.
        /// </summary>
        /// <param name="lineType"></param>
        /// <returns></returns>
        public static string ConvertLineTypeToDBString(LineType lineType)
        {
            if (lineType == LineType.Line)
            {
                return DB_LineType_Line;
            }
            if (lineType == LineType.Spline)
            {
                return DB_LineType_Spline;
            }
            else return DB_LineType_Line;
        }

        /// <summary>
        /// convert line type from db saving format to enum LineType
        /// </summary>
        /// <param name="lineType"></param>
        /// <returns></returns>
        public static LineType ConvertLineTypeToEnum (String lineType)
        {
            if(lineType == DB_LineType_Line)
            {
                return LineType.Line;
            }
            if(lineType == DB_LineType_Spline)
            {
                return LineType.Spline;
            }
            return LineType.Line;
        }

        /// <summary>
        /// execute non-query sql
        /// </summary>
        /// <param name="sql">sql statement</param>
       /* public static void ExecuteNonQuery(string sql)
        {
            string Function_Name = "ExecuteNonQuery";
            OracleConnection cnn = DBConnection.getInstance().getConn();
            OracleCommand cmdOracle = new OracleCommand(sql, cnn);
            try
            {
                cmdOracle.ExecuteNonQuery();
            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME,Function_Name ,localException.ToString());
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
        public static string convertGB2312To8859P1(string str)
        {
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
    }
}

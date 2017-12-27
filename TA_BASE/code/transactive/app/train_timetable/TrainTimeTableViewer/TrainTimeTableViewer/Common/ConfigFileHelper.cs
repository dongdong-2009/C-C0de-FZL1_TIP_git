using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using DAO.TimeTable.Helper;

namespace TrainTimeTableViewer.Common
{
    /// <summary>
    /// This class is responsible for parsing INI file and 
    /// loads the key-value.
    /// </summary>
    public class ConfigureFileHelper
    {
        private static ConfigureFileHelper m_instance = null;
        private bool m_inited = false;

        [DllImport("kernel32")]
        public static extern int GetPrivateProfileString(string section,
                                                        string key, string def, StringBuilder retVal,
                                                        int size, string filePath);

        //  <summary>
        //  Return string value based on entryCategory and entryKey of the specified ini configuration file
        //  </summary>
        public static string GetINIDataString(string entryCategory, string entryKey, string entryDefault, int entrySize, string entryFile)
        {
            int returnValue;

            StringBuilder tempStringBuilder = new StringBuilder(entrySize);

            returnValue = GetPrivateProfileString(entryCategory, entryKey, entryDefault, tempStringBuilder, entrySize, entryFile);

            return tempStringBuilder.ToString().Trim();
        }

        public static ConfigureFileHelper GetInstance()
        {
            if (m_instance == null)
            {
                m_instance = new ConfigureFileHelper();
            }
            return m_instance;


        }
        public void init()
        {
            if (m_inited == false)
            {

                string configFile = "./TrainTimetableViewerConfig.ini";

                m_DBConnectionString = getConnectionStringBasedType(configFile, "DB"); //Get DB connection string
                     
                m_DefaultFolder = GetINIDataString("DIRECTORY", "DEFAULT_FOLDER", "", 255, configFile);

                //get Language string from config file, default is "english"
                //m_LanguageStr = GetINIDataString("LANGUAGE", "LANGUAGE", STEE.ISCS.MulLanguage.LanguageStr.ENGLISH, 255, configFile);

                //get encoding_change parameter from config file, default is "false".
                // if it's true, we need to change encoding when read or save string from DB. Else, we don't change the encoding when read or save string.
                //string encodingChange_str = GetINIDataString("ENCODING_CHANGE", "ENCODING_CHANGE", "false", 255, configFile);
                //if (encodingChange_str.ToLower() == "true")
                //{
                //    m_EncodingChange = true;
                //}
                //else
                //{
                //    m_EncodingChange = false;
                //}


            }
        }

        private string getConnectionStringBasedType(string configFile, string dbLabel)
        {
            string connectionString;
            DBType dbType = DAOHelper.GetDbType(GetINIDataString(dbLabel, "DB_TYPE", "", 255, configFile));
            if (dbType == DBType.Oracle)
            {
                connectionString = GetINIDataString(dbLabel, "DB_TYPE", "", 255, configFile) + ";Data Source = " + GetINIDataString(dbLabel, "SERVICE_NAME", "", 255, configFile) + ";" +
                "User Id = " + GetINIDataString(dbLabel, "USER_ID", "", 255, configFile) + "; " +
                "Password = " + GetINIDataString(dbLabel, "USER_PASSWORD", "", 255, configFile) + "; ";
            }
            else
            {
                connectionString = GetINIDataString(dbLabel, "DB_TYPE", "", 255, configFile) + ";Database = " + GetINIDataString(dbLabel, "SERVICE_NAME", "", 255, configFile) + ";" +
                "User Id = " + GetINIDataString(dbLabel, "USER_ID", "", 255, configFile) + "; " +
                "Password = " + GetINIDataString(dbLabel, "USER_PASSWORD", "", 255, configFile) + ";Host = " + GetINIDataString(dbLabel, "HOST_NAME", "", 255, configFile) + ";Port=3306;";
            }
            return connectionString;
        }

        
        private string m_DefaultFolder;
        public string DefaultFolder
        {
            get { return m_DefaultFolder; }
            set { m_DefaultFolder = value; }
        }

        private string m_DBConnectionString;
        public string DBConnectionString
        {
            get { return m_DBConnectionString; }
            set { m_DBConnectionString = value; }
        }


        private string m_AboutMessage;
        public string AboutMessage
        {
            get { return m_AboutMessage; }
            set { m_AboutMessage = value; }
        }
        private bool m_EnableSmartLabel;
        public bool EnableSmartLabel
        {
            get { return m_EnableSmartLabel; }
            set { m_EnableSmartLabel = value; }
        }

        private int m_ScreenWidth;
        public int ScreenWidth
        {
            get { return m_ScreenWidth; }
            set { m_ScreenWidth = value; }
        }
        //private string m_LanguageStr;
        //public string LanguageStr
        //{
        //    get { return m_LanguageStr; }
        //    set { m_LanguageStr = value; }
        //}

        private bool m_EncodingChange;
        public bool EncodingChange
        {
            get { return m_EncodingChange; }
            set { m_EncodingChange = value; }
        }
    }
}

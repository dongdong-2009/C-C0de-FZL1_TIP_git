using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace OPCDataLogger
{
    class ConfigureFileHelper
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

                string configFile = "./config.ini";
                
                m_ConnectionString = "Data Source = " + GetINIDataString("DATABASE_SERVER", "SERVICE_NAME", "", 255, configFile) + ";" +
                "User Id = " + GetINIDataString("DATABASE_SERVER", "USER_ID", "", 255, configFile) + "; " +
                "Password = " + GetINIDataString("DATABASE_SERVER", "USER_PASSWORD", "", 255, configFile) + "; ";
                
                m_OPCServerName = GetINIDataString("OPC_CLIENT", "SERVER1_NAME", "", 255, configFile);

                m_OPCServerRootName = GetINIDataString("OPC_CLIENT", "SERVER_ROOT_NAME", "", 255, configFile);

                m_DefaultLineThickness = Convert.ToInt32(GetINIDataString("OPC_CLIENT", "LINE_THICKNESS", "", 255, configFile));

                m_AboutMessage = GetINIDataString("OPC_CLIENT", "ABOUT_MSG", "", 255, configFile);

                //opcSrv1Root = localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_ENTRY_TAG", "", 255, configFile);
                //opcSrv1ForbiddenName = ";" + localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_EXCLUDE_TAG", "", 255, configFile) + ";";

                m_opcSrv1Host = GetINIDataString("OPC_DT_LOG", "SERVER1_MACHINE", "", 255, configFile);
                m_opcSrv1Name = GetINIDataString("OPC_DT_LOG", "SERVER1_NAME", "", 255, configFile);
                m_opcSrv2Host = GetINIDataString("OPC_DT_LOG", "SERVER2_MACHINE", "", 255, configFile);
                m_opcSrv2Name = GetINIDataString("OPC_DT_LOG", "SERVER2_NAME", "", 255, configFile);
                m_DefaultFolder = GetINIDataString("DIRECTORY", "DEFAULT_FOLDER", "", 255, configFile);

                m_DTTimeInterval = GetINIDataString("OPC_DT_LOG", "TIME_INTERVAL", "", 255, configFile);

                m_DTTimeIntervalMS = GetINIDataString("OPC_DT_LOG", "TIME_INTERVAL_MS", "", 255, configFile);

                //get Language string from config file, default is "english"
                m_LanguageStr = GetINIDataString("LANGUAGE", "LANGUAGE", STEE.ISCS.MulLanguage.LanguageStr.ENGLISH, 255, configFile);

                //get encoding_change parameter from config file, default is "false".
                // if it's true, we need to change encoding when read or save string from DB. Else, we don't change the encoding when read or save string.
                string encodingChange_str = GetINIDataString("ENCODING_CHANGE", "ENCODING_CHANGE", "false", 255, configFile);
                if (encodingChange_str.ToLower() == "true")
                {
                    m_EncodingChange = true;
                }
                else
                {
                    m_EncodingChange = false;
                }
            }
        }

        private string m_OPCServerName;
        public string OPCServerName
        {
            get { return m_OPCServerName; }
            set { m_OPCServerName = value; }
        }

        private string m_OPCServerRootName;
        public string OPCServerRootName
        {
            get { return m_OPCServerRootName; }
            set { m_OPCServerRootName = value; }
        }


        private string m_DefaultFolder;
        public string DefaultFolder
        {
            get { return m_DefaultFolder; }
            set { m_DefaultFolder = value; }
        }

        private string m_ConnectionString;
        public string ConnectionString
        {
            get { return m_ConnectionString; }
            set { m_ConnectionString = value; }
        }

        private int m_DefaultLineThickness;
        public int DefaultLineThickness
        {
            get { return m_DefaultLineThickness; }
            set { m_DefaultLineThickness = value; }
        }


        private string m_AboutMessage;
        public string AboutMessage
        {
            get { return m_AboutMessage; }
            set { m_AboutMessage = value; }
        }

        private string m_opcSrv1Host;
        public string OPCDTSever1Host
        {
            get { return m_opcSrv1Host; }
            set { m_opcSrv1Host = value; }
        }

        private string m_opcSrv1Name;
        public string OPCDTSever1Name
        {
            get { return m_opcSrv1Name; }
            set { m_opcSrv1Name = value; }
        }

        private string m_opcSrv2Host;
        public string OPCDTSever2Host
        {
            get { return m_opcSrv2Host; }
            set { m_opcSrv2Host = value; }
        }

        private string m_opcSrv2Name;
        public string OPCDTSever2Name
        {
            get { return m_opcSrv2Name; }
            set { m_opcSrv2Name = value; }
        }
        private string m_DTTimeInterval;
        public string DTTimeInteval
        {
            get { return m_DTTimeInterval; }
            set { m_DTTimeInterval = value; }
        }

        private string m_DTTimeIntervalMS;
        public string TimeIntevalMS
        {
            get { return m_DTTimeIntervalMS; }
            set { m_DTTimeIntervalMS = value; }
        }

        private string m_LanguageStr;
        public string LanguageStr
        {
            get { return m_LanguageStr; }
            set { m_LanguageStr = value; }
        }

        private bool m_EncodingChange;
        public bool EncodingChange
        {
            get { return m_EncodingChange; }
            set { m_EncodingChange = value; }
        }
    }
}
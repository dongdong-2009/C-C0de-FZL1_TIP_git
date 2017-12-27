using System;
using System.Collections.Generic;
using System.Text;
using OPCTrendLib;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Common
{
    class ConfigureFileHelper
    {
        private static ConfigureFileHelper m_instance = null;
        private bool m_inited = false;
        
        private ConfigureFileHelper()
        {

        }
        public static ConfigureFileHelper GetInstance()
        {
            if (m_instance == null)
            {
                m_instance = new ConfigureFileHelper();
            }
            return m_instance;
        }

        /// <summary>
        /// get information from config.ini and save them into member.
        /// </summary>
        public void init()
        {
            if (m_inited==false)
            {

                string configFile = "./config.ini";
                 
                GeneralFunction localFunction = new OPCTrendLib.GeneralFunction();


                m_ConnectionString = "Data Source = " + localFunction.GetINIDataString("DATABASE_SERVER", "SERVICE_NAME", "", 255, configFile) + ";" +
                "User Id = " + localFunction.GetINIDataString("DATABASE_SERVER", "USER_ID", "", 255, configFile) + "; " +
                "Password = " + localFunction.GetINIDataString("DATABASE_SERVER", "USER_PASSWORD", "", 255, configFile) + "; ";


                m_OPCServerName = localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_NAME", "", 255, configFile);
                m_OPCServerRootName = localFunction.GetINIDataString("OPC_CLIENT", "SERVER_ROOT_NAME", "", 255, configFile);
                m_DefaultLineThickness = Convert.ToInt32(
                    localFunction.GetINIDataString("OPC_CLIENT", "LINE_THICKNESS", "", 255, configFile)
                    );     
                m_AboutMessage = localFunction.GetINIDataString("OPC_CLIENT", "ABOUT_MSG", "", 255, configFile);
                m_ScreenWidth = Convert.ToInt32(
                    localFunction.GetINIDataString("OPC_CLIENT", "SCREEN_WIDTH", "1680", 255, configFile)
                    );

                string enableSmartLabel_str = localFunction.GetINIDataString("OPC_CLIENT", "ENABLE_SMART_LABEL", "false", 255, configFile);
                if (enableSmartLabel_str.ToLower() == "true")
                { m_EnableSmartLabel = true; }
                else 
                { m_EnableSmartLabel = false; }
                //opcSrv1Root = localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_ENTRY_TAG", "", 255, configFile);
                //opcSrv1ForbiddenName = ";" + localFunction.GetINIDataString("OPC_CLIENT", "SERVER1_EXCLUDE_TAG", "", 255, configFile) + ";";
                m_hostIPAddress = "127.0.0.1";

                m_DefaultFolder = localFunction.GetINIDataString("DIRECTORY", "DEFAULT_FOLDER", "", 255, configFile);

                //get Language string from config file, default is "english"
                m_LanguageStr = localFunction.GetINIDataString("LANGUAGE", "LANGUAGE", STEE.ISCS.MulLanguage.LanguageStr.ENGLISH, 255, configFile);

                //get encoding_change parameter from config file, default is "false".
                // if it's true, we need to change encoding when read or save string from DB. Else, we don't change the encoding when read or save string.
                string encodingChange_str = localFunction.GetINIDataString("ENCODING_CHANGE", "ENCODING_CHANGE", "false", 255, configFile);
                if(encodingChange_str.ToLower() == "true")
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

        private bool m_EnableSmartLabel;
        public bool EnableSmartLabel
        {
            get { return m_EnableSmartLabel; }
            set { m_EnableSmartLabel = value; }
        }

        private string m_hostIPAddress;
        public string HostIPAddress
        {
            get { return m_hostIPAddress; }
            set { m_hostIPAddress = value; }
        }

        private int m_ScreenWidth;
        public int ScreenWidth
        {
            get { return m_ScreenWidth; }
            set { m_ScreenWidth = value; }
        }

        private string m_LanguageStr;
        public string LanguageStr
        {
            get { return m_LanguageStr; }
            set { m_LanguageStr = value;}
        }

        private bool m_EncodingChange;
        public bool EncodingChange
        {
            get { return m_EncodingChange; }
            set { m_EncodingChange = value; }
        }
    }
}

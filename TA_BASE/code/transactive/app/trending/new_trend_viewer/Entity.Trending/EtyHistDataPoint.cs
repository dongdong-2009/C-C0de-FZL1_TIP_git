using System;
using System.Collections.Generic;
using System.Text;

namespace Entity.Trending
{
    public class EtyHistDataPoint
    {

        ulong m_Pkey;
        string m_Description;
        string m_DisplayName;
        string m_DPName;
        LineType m_DPType;
        string m_DPColor;
        string m_DPServer;
        bool m_DPEnabled;
        bool m_DPLblEnabled;
        string m_DPLblName;
        DateTime m_DPStartDateTime;
        DateTime m_DPEndDateTime;
        string m_DPHost;
        string m_ConfigName;
        ulong m_EntityKey;

        public EtyHistDataPoint()
        {
            m_Pkey = 0;
            m_Description = "";
            m_DisplayName = "";
            m_DPName = "";
            m_DPType = LineType.Line;
            m_DPColor = "";
            m_DPServer = "";
            m_DPEnabled = true;
            m_DPLblEnabled = true;
            m_DPLblName = "";
            m_DPStartDateTime = new DateTime();
            m_DPEndDateTime = new DateTime();
            m_DPHost = "";
            m_ConfigName = "";
            m_EntityKey = 0;
        }



        public ulong PKey
        {
            get { return m_Pkey; }
            set { m_Pkey = value; }
        }

        public string Description
        {
            get { return m_Description; }
            set { m_Description = value; }
        }

        public string DisplayName
        {
            get { return m_DPName; }  //temporary return DPName
            set { m_DisplayName = value; }
        }
	
        public string DPName
        {
            get { return m_DPName; }
            set { m_DPName = value; }
        }

        public LineType DPType
        {
            get { return m_DPType; }
            set { m_DPType = value; }
        }

        public string DPColor
        {
            get { return m_DPColor; }
            set { m_DPColor = value; }
        }

        public string DPServer
        {
            get { return m_DPServer; }
            set { m_DPServer = value; }
        }

        public bool DPEnabled
        {
            get { return m_DPEnabled; }
            set { m_DPEnabled = value; }
        }

        public bool DPLblEnabled
        {
            get { return m_DPLblEnabled; }
            set { m_DPLblEnabled = value; }
        }
 
        public string DPLblName
        {
            get { return m_DPLblName; }
            set { m_DPLblName = value; }
        }

        public DateTime DPStartDateTime
        {
            get { return m_DPStartDateTime; }
            set { m_DPStartDateTime = value; }
        }

        public DateTime DPEndDateTime
        {
            get { return m_DPEndDateTime; }
            set { m_DPEndDateTime = value; }
        }

        public string DPHost
        {
            get { return m_DPHost; }
            set { m_DPHost = value; }
        }

        public string ConfigName
        {
            get { return m_ConfigName; }
            set { m_ConfigName = value; }
        }
        public ulong EntityKey
        {
            get { return m_EntityKey; }
            set { m_EntityKey = value; }
        }
    }
}

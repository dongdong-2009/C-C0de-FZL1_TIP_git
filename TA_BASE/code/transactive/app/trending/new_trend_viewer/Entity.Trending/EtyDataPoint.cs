using System;
using System.Collections.Generic;
using System.Text;

namespace Entity.Trending
{
    public enum LineType  //LineType for Datapoint
    {
        Line,
        Spline
    }

    public class EtyDataPoint
    {
        ulong m_pkey;
        string m_Description;
        string m_DisplayName;
        string m_DPName;
		LineType m_DPType;
        string m_DPColor;
        string m_DPServer;
        bool m_DPEnabled;
        bool m_DPLblEnabled;
        string m_DPLblName;
        string m_DPHost;
        bool m_DPVisible;
        string m_ConfigName;

        public EtyDataPoint()
        {
            m_pkey =0;
            m_Description = "";
            m_DisplayName = "";
            m_DPName = "";
            m_DPType = LineType.Line;
            m_DPColor = System.Drawing.Color.Transparent.ToArgb().ToString();
            m_DPServer = "";
            m_DPEnabled = true;
            m_DPLblEnabled = true;
            m_DPLblName = "";
            m_DPHost = "";
            m_DPVisible = true;
            m_ConfigName = "";

        }

        public string DisplayName
        {
            get { return m_DPName; }   //temporary return DPName
            set { m_DisplayName = value; }
        }
	
         public ulong  PKey
         {
             get { return m_pkey; }
             set { m_pkey = value; }
         }

         public string Description
         {
             get { return m_Description; }
             set { m_Description = value; }
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

         public string DPHost
         {
             get { return m_DPHost; }
             set { m_DPHost = value; }
         }

         public bool DPVisible
         {
             get { return m_DPVisible; }
             set { m_DPVisible = value; }
         }

         public string ConfigName
         {
             get { return m_ConfigName; }
             set { m_ConfigName = value; }
         }
	
	 
    }
}

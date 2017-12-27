using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Entity.Trending
{
    public class EtyMarker
    {
        ulong m_pkey; 
        string m_ConfigName;
		string m_MarkerName;
        double m_MarkerWidth;
        double m_MarkerValue;
        bool m_MarkerEnabled;
        string m_MarkerBColor;
        string m_MarkerFColor;

        public EtyMarker()
        {
            m_pkey =0;
            m_ConfigName = "";
            m_MarkerName = "";
            m_MarkerWidth = 1;
            m_MarkerValue = 1;
            m_MarkerEnabled = true;
            m_MarkerBColor = System.Drawing.Color.Transparent.ToArgb().ToString();
            m_MarkerFColor = System.Drawing.Color.Transparent.ToArgb().ToString();
        }

        public ulong PKey
        {
            get { return m_pkey; }
            set { m_pkey = value; }
        }

         public string  ConfigName
         {
             get { return m_ConfigName; }
             set { m_ConfigName = value; }
         }
         public string MarkerName
         {
             get { return m_MarkerName; }
             set { m_MarkerName = value; }
         }
	
          public double MarkerWidth
         {
             get { return m_MarkerWidth; }
             set { m_MarkerWidth = value; }
         }
         public double MarkerValue
         {
             get { return m_MarkerValue; }
             set { m_MarkerValue = value; }
         }
        public bool MarkerEnabled
        {
            get { return m_MarkerEnabled; }
             set { m_MarkerEnabled = value; }
        }

        public string MarkerBColor
        {
            get { return m_MarkerBColor; }
             set { m_MarkerBColor = value; }
        }
         public string MarkerFColor
        {
            get { return m_MarkerFColor; }
             set { m_MarkerFColor = value; }
        }

    }
}

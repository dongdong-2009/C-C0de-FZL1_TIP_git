using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Entity.Trending
{
    public class EtyFormula
    {
        ulong m_PKey;
        string m_ConfigName;
        string m_DPEquation;
        LineType m_DPType;
        string m_DPColor;
        bool m_DPEnabled;
        bool m_DPLblEnabled;
        string m_DPLblName;

        public EtyFormula()
        {
            m_PKey = 0;
            m_ConfigName = "";
            m_DPEquation = "";
            m_DPType = LineType.Line;
            m_DPColor = System.Drawing.Color.Transparent.ToArgb().ToString();
            m_DPEnabled = true;
            m_DPLblEnabled = true;
            m_DPLblName = "";

        }
        public ulong PKey
        {
            get { return m_PKey; }
            set { m_PKey = value; }
        }
        public string ConfigName
        {
            get { return m_ConfigName; }
            set { m_ConfigName = value; }
        }
        public string DPEquation
        {
            get { return m_DPEquation; }
            set { m_DPEquation = value; }
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
    }
}

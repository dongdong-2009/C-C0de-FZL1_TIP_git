using System;
using System.Collections.Generic;
using System.Text;

namespace Entity.Trending
{
    public class EtyTrendLog
    {
        string m_DataPointName = "";
        string m_DataPointServerName = "";
        double m_DataPointValue;
        //todo set default value for time
        DateTime m_DataPointTime;
        string m_DataPointHost = "";

        public string Data_PT_Name
        {
            get { return m_DataPointName; }
            set { m_DataPointName = value; }
        }

        public string Data_PT_Server
        {
            get { return m_DataPointServerName; }
            set { m_DataPointServerName = value; }
        }

        public string Data_PT_Host
        {
            get { return m_DataPointHost; }
            set { m_DataPointHost = value; }
        }

        public double Data_PT_Value
        {
            get { return m_DataPointValue; }
            set { m_DataPointValue = value; }
        }

        public DateTime Data_PT_Time
        {
            get { return m_DataPointTime; }
            set { m_DataPointTime = value; }
        }
    }
}

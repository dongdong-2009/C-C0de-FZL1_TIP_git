using System;
using System.Collections.Generic;
using System.Text;

namespace OPCDataLogger
{
    class OPCDPGrpDetails
    {
        private string m_DataPointName ="";
        private string m_Value = "0";
        private string m_OldValue = "null";
        private double m_interval = 10;
        private double m_deltaValue = 0;
        private DateTime? m_nextTime =  null;
       

        public string DT_PT_Name
        {
            get { return m_DataPointName; }
            set { m_DataPointName = value; }
        }

        public string Value
        {
            get { return m_Value; }
            set { m_Value = value; }
        }

        public string OldValue
        {            
            get { return m_OldValue; }
            set { m_OldValue = value; }
        }

        public double Interval
        {
            get { return m_interval; }
            set { m_interval = value; }
        }

        public double Delta
        {
            get { return m_deltaValue; }
            set { m_deltaValue = value; }
        }

        public DateTime? NextSampleTime
        {
            get { return m_nextTime; }
            set { m_nextTime = value; }
        }

    }
}

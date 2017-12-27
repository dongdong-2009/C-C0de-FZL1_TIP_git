using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Entity.Trending
{
    public class EtyDataLogDPGroupTrend
    {
        double m_SampleGrpID;
        string m_SampleGrpName = "";
        string m_SampleGrpDescription = "";
        double m_Interval = 1;
        string m_IntervalType = "";
        string m_StartTime = "";
        bool m_Disbaled = false;
        double m_DeltaValue = 0;
        double m_locationkey;
        string m_locationName = "";
        bool m_isNew = false;

        public double SampleGrpID
        {
            get { return m_SampleGrpID; }
            set { m_SampleGrpID = value; }
        }

        public double LocationKey
        {
            get { return m_locationkey; }
            set { m_locationkey = value; }
        }

        public string LocationName
        {
            get { return m_locationName; }
            set { m_locationName = value; }
        }

        public string SampleGrpName
        {
            get { return m_SampleGrpName; }
            set { m_SampleGrpName = value; }
        }

        public string SampleGrpDescription
        {
            get { return m_SampleGrpDescription; }
            set { m_SampleGrpDescription = value; }
        }

        public double Interval
        {
            get { return m_Interval; }
            set { m_Interval = value; }
        }

        public string IntervalType
        {
            get { return m_IntervalType; }
            set { m_IntervalType = value; }
        }

        public string StartTime
        {
            get { return m_StartTime; }
            set { m_StartTime = value; }
        }


        public bool Disabled
        {
            get { return m_Disbaled; }
            set { m_Disbaled = value; }
        }

        public double DeltaValue
        {
            get { return m_DeltaValue; }
            set { m_DeltaValue = value; }
        }

        public bool NewData
        {
            get { return m_isNew; }
            set { m_isNew = value; }
        }

        //todo add field to check whether the data is updated.
    }
}

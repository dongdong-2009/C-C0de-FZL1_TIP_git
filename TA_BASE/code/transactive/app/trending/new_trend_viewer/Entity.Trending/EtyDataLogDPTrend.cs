using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Entity.Trending
{
    public class EtyDataLogDPTrend
    {
        double m_DataPointId;
        double m_EntityKey;
        double m_SampleGrpId = -1;
        string m_DataPtName = null;
        bool m_Disable = true;
        string m_DPDesc = null;

        public double Pkey
        {
            get { return m_DataPointId; }
            set { m_DataPointId = value; }
        }

        public double EntityKey
        {
            get { return m_EntityKey; }
            set { m_EntityKey = value; }
        }

        public double OPCSampleGrpId
        {
            get { return m_SampleGrpId; }
            set { m_SampleGrpId = value; }
        }

        public string OPCDataPointName
        {
            get { return m_DataPtName; }
            set { m_DataPtName = value; }
        }

        public bool Disabled
        {
            get { return m_Disable; }
            set { m_Disable = value; }
        }

        public string OPCDataPointDesc
        {
            get { return m_DPDesc; }
            set { m_DPDesc = value; }
        }

    }
}

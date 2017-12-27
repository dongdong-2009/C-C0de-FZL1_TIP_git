using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Entity.Trending
{
    public class EtyDataLogDPLogTrend
    {


        ulong m_Pkey = 0;
        ulong m_EntityKey = 0;
        DateTime m_CreateTime ;
        double m_Value = 0;
        // ulong m_LocationKey = 0;  don't care this column in TrendViewer
        // DateTime m_PlanLogTime = null; don't care this column in TrendViewer

        public ulong Pkey
        {
            get { return m_Pkey; }
            set { m_Pkey = value;}
        }

        public ulong EntityKey
        {
            get { return m_EntityKey; }
            set { m_EntityKey = value; }
        }
//         public long LocationKey
//         {
//             get { return m_LocationKey; }
//             set { m_LocationKey = value; }
//         }
        public DateTime CreateTime
        {
            get { return m_CreateTime; }
            set { m_CreateTime = value; }
        }
        public double Value
        {
            get { return m_Value; }
            set { m_Value = value; }
        }


    }
}

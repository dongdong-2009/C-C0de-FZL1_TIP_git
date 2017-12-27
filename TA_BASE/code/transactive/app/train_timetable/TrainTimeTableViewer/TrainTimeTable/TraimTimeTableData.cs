using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrainTimeTable
{
    public class TrainTimeTableData
    {
        int m_TrainServiceId;
        int m_StationID;
        string m_StationName;
        DateTime m_ArrTime;
        DateTime m_DeptTime;
        //DateTime m_Date;

        public int TrainServiceId
        {
            get { return m_TrainServiceId; }
            set { m_TrainServiceId = value; }
        }

        public int StationID
        {
            get { return m_StationID; }
            set { m_StationID = value; }

        }

        public string StationName
        {
            get { return m_StationName; }
            set { m_StationName = value; }

        }

        public DateTime ArrTime
        {
            get { return m_ArrTime; }
            set { m_ArrTime = value; }
        }

        public DateTime DeptTime
        {
            get { return m_DeptTime; }
            set { m_DeptTime = value; }
        }

        //public DateTime Date
        //{
        //    get { return m_Date; }
        //    set { m_Date = value; }
        //}


    }

    
}

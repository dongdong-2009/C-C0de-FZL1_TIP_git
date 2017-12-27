using System;
using System.Collections.Generic;
using System.Text;
using Entity.Trending;

namespace TrendViewer.Common
{
    /// <summary>
    /// This class is for saving these cache: current real-time datapoint config name and list, current history datapoint config name and  list,
    /// current marker config name and list, current formula config name and list
    /// </summary>
   public class TrendCache
    {
        public List<EtyDataPoint> m_dataPointList = null;
        public List<EtyHistDataPoint> m_histDataPointList = null;
        public List<EtyMarker> m_markerList = null;
        public List<EtyFormula> m_formulaList = null;
        
        public string m_datapointGrpName = "";
        public string m_histDatapointGrpName = "";
        public string m_markerGrpName = "";
        public string m_formulaGrpName = "";

        private Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> m_histLogForMixedMode = null;

        public TrendCache()
        {
            m_dataPointList = new List<EtyDataPoint>();
            m_histDataPointList = new List<EtyHistDataPoint>();

            m_markerList = new List<EtyMarker>();

            m_formulaList = new List<EtyFormula>();

            m_histLogForMixedMode = new Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>>();
        }

        public Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> GetHistLogForMixedMode()
        {
            return m_histLogForMixedMode;
        }
        public void ClearHistLogForMixedMode()
        {
            foreach (KeyValuePair<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> pair in m_histLogForMixedMode)
            {
                if(pair.Value != null )
                {
                    pair.Value.Clear();
                }
            }
            m_histLogForMixedMode.Clear();
        }

        public void SetHistLogForMixedMode(Dictionary<EtyHistDataPoint, List<EtyDataLogDPLogTrend>> histLogMap)
        {
            m_histLogForMixedMode = histLogMap;
        }

        /// <summary>
        /// get the enabled dp list from the current dp list
        /// </summary>
        /// <returns>the enabled dp list</returns>
        public List<EtyDataPoint> GetVisibleOrEnabledDPList()
        {
            if (m_dataPointList == null) return null;
            List<EtyDataPoint> dpList = new List<EtyDataPoint>();
            for (int i = 0; i < m_dataPointList.Count; i++)
            {
                if (m_dataPointList[i].DPVisible == true || m_dataPointList[i].DPEnabled == true)
                {
                    dpList.Add(m_dataPointList[i]);
                }
            }
            return dpList;
        }

        /// <summary>
        /// get the enabled formula list from the current formula list
        /// </summary>
        /// <returns>the enabled formula list</returns>
        public List<EtyFormula> GetEnabledFormulaList()
        {
            if (m_formulaList == null) return null;
            List<EtyFormula> formulaList = new List<EtyFormula>();
            for (int i = 0; i < m_formulaList.Count; i++)
            {
                if (m_formulaList[i].DPEnabled == true)
                {
                    formulaList.Add(m_formulaList[i]);
                }
            }
            return formulaList;
        }

        /// <summary>
        /// get the enabled marker list from the current marker list
        /// </summary>
        /// <returns>the enabled marker list</returns>
        public List<EtyMarker> GetEnabledMarkerList()
        {
            if (m_markerList == null) return null;
            List<EtyMarker> enabledMarkerList = new List<EtyMarker>();
            for (int i = 0; i < m_markerList.Count; i++)
            {
                if (m_markerList[i].MarkerEnabled == true)
                {
                    enabledMarkerList.Add(m_markerList[i]);
                }
            }
            return enabledMarkerList;
        }

        /// <summary>
        /// get the count of the enabled historical dp list
        /// </summary>
        /// <returns></returns>
        public int CountEnabledHistDPList()
        {
            if (m_histDataPointList == null) return 0;
            int count = 0;
            for (int i = 0; i < m_histDataPointList.Count; i++)
            {
                if (m_histDataPointList[i].DPEnabled == true)
                {
                    count++;
                }
            }
            return count;
        }
    }
}

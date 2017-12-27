/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPDataUpdater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include <set>
#include <math.h>
#include "time.h"
#include "ace/OS.h"

#include "SNMPDataUpdater.h"
#include "DataTranslater.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SNMPDataUpdater::SNMPDataUpdater(std::string name)
		:
		ProtocolUpdater(name),
		m_preBestQuality(QUALITY_GOOD_NO_SPECIFIC_REASON)
	{
       
	}

	SNMPDataUpdater::~SNMPDataUpdater()
	{
		std::map<std::string, DataPointList*>::const_iterator it = m_oidToDataPointsMap.begin();
		while (m_oidToDataPointsMap.end() != it)
		{
			delete it->second;
			++it;
		}
		m_oidToDataPointsMap.clear();
    }

	bool SNMPDataUpdater::isDataPointMatched(TA_Base_Bus::DataPoint * dp )
	{
		bool ret = SNMPGlobalHelper::isNameMatchedDeviceName(m_name, dp->getDataPointName());
		return ret;
    }

	void SNMPDataUpdater::finishedAddDataPoint()
	{
		m_oidToDataPointsMap.clear();
		std::map<std::string, DataPointList*>::const_iterator it = m_oidToDataPointsMap.begin();
		DataPointList*	dps = 0;
		for (int i = 0; i < m_dataPoints.size(); ++i)
		{
			it = m_oidToDataPointsMap.find(m_dataPoints[i]->getDataPointAddress());
			if (m_oidToDataPointsMap.end() == it)
			{
				dps = new DataPointList();
				m_oidToDataPointsMap[m_dataPoints[i]->getDataPointAddress()] = dps;
				dps->quality = QUALITY_BAD_NO_SPECIFIC_REASON;
			}
			else
			{
				dps = it->second;
			}
			if (dps)
			{
				dps->dataPoints.push_back(m_dataPoints[i]);
			}
		}
	}

	void SNMPDataUpdater::submitDeviceData(IDeviceData* deviceData)
	{
		if (0 == deviceData)
		{
			return;
		}
		SNMPData* data = dynamic_cast<SNMPData*> (deviceData);
		if (0 == data)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "updater %s can not cast data from %s to SNMPData type",
					m_name.c_str(), deviceData->getStaticType().c_str());
			return;
		}
		int processState = 1;
		TA_Base_Core::ThreadGuard guard(m_lock);
		if (false == data->isGoodQuality && m_preBestQuality <= QUALITY_BAD_LAST_KNOWN_VALUE)
		{
			processState = 2;
		}
		else if (false == data->isGoodQuality && false == data->isPortionDevice)
		{
			updateAllDataPointToBad();
			processState = 3;
		}
		else
		{
			/*Snmp_pp::*/Pdu& pdu = data->pdu;
			/*Snmp_pp::*/Vb vb;
			for ( int i = 0; i < pdu.get_vb_count(); ++i)
			{
				pdu.get_vb(vb, i);
				processVbData(vb, data->isGoodQuality);
			}
			processState = 4;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updater %s finished to process SNMPData, within state[%d]",
					m_name.c_str(), processState);
	}

	void SNMPDataUpdater::updateAllDataPointToBad()
	{
		std::map<std::string, DataPointList*>::const_iterator it = m_oidToDataPointsMap.begin();
		while (m_oidToDataPointsMap.end() != it)
		{
			updateDataPointsToBad(it->second);
			++it;
		}
		m_preBestQuality = QUALITY_BAD_LAST_KNOWN_VALUE;
	}

	void SNMPDataUpdater::updateDataPointsToBad(DataPointList* dataPoints)
	{
		EQualityStatus newQuality = QUALITY_BAD_NOT_CONNECTED;
		if (QUALITY_BAD_LAST_KNOWN_VALUE <= dataPoints->quality )
		{
			newQuality = QUALITY_BAD_LAST_KNOWN_VALUE;
		}
		if (newQuality == dataPoints->quality)
		{
			return;	// bad quality to bad quality, no need to update.
		}
		int nSize = dataPoints->dataPoints.size();
		DataPoint* dp = 0;
		timeb currentTimeStamp;
		ftime( &currentTimeStamp );
		for (int i = 0; i < nSize; ++i)
		{
			dp = dataPoints->dataPoints[i];
			boost::shared_ptr<TA_Base_Bus::DataPointValue> rawValue = dp->DataPoint::getRawValue();
			TA_Base_Bus::DpValue dpValue(*rawValue, dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			TA_Base_Bus::DataPointState newState(dpValue, currentTimeStamp, newQuality);

			updateDPFieldValue(dp, newState);
		}

		dataPoints->quality = newQuality;
	}

	void SNMPDataUpdater::processVbData(const /*Snmp_pp::*/Vb& vb, bool isGoodQuality)
	{
		const char* POid = vb.get_printable_oid();
		if (0 == POid)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "can not get oid from /*Snmp_pp::*/Vb value");
			return;
		}
		std::string oid(POid);
		std::map<std::string, DataPointList*>::const_iterator it = m_oidToDataPointsMap.find(oid);
		if (m_oidToDataPointsMap.end() == it)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "updater %s can not find DataPoint for oid[%s]",
				m_name.c_str(), POid);
			return;
		}
		if (false == isGoodQuality)
		{
			updateDataPointsToBad(it->second);
			return;
		}
		// just have good quality.
		timeb currentTimeStamp;
		ftime( &currentTimeStamp );
		DataPointList* dataPoints = it->second;
		EQualityStatus newQuality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		for (int i = 0; i < dataPoints->dataPoints.size(); ++i)
		{
			DataPoint* dp = dataPoints->dataPoints[i];
			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "start update datapoint:%s with value:%s, the oid is:%s",
				dp->getDataPointName().c_str(), vb.get_printable_value(), POid);
			try
			{
				TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
				DataTranslater::translateToDpValue(dpValue, vb);
				TA_Base_Bus::DataPointState newState(dpValue, currentTimeStamp, newQuality);
				updateDPFieldValue(dp, newState);
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "exception happened when process snmp vb data for datapoint:%s",
					dp->getDataPointName().c_str());
			}
		}

		dataPoints->quality = newQuality;
		if (newQuality > m_preBestQuality)
		{
			m_preBestQuality = newQuality;
		}
	}

}

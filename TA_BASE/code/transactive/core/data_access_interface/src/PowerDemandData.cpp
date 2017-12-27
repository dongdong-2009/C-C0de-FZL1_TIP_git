/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/PowerDemandData.cpp $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/PowerDemandData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{

	PowerDemandData::PowerDemandData()
        : m_meterName(""),
          m_interval(0),
          m_recordedValue(0),
          m_updatedValue(0)
    {
    }


    PowerDemandData::PowerDemandData(const std::string& meterName, 
									 time_t interval, 
		                             unsigned long recordedValue, 
		                             unsigned long updatedValue)
        : m_meterName(meterName),
          m_interval(interval),
          m_recordedValue(recordedValue),
          m_updatedValue(updatedValue)
    {
    }


    PowerDemandData::~PowerDemandData()
    {

    }


    std::string PowerDemandData::getMeterName() const
    {
        return m_meterName;
    }


    time_t PowerDemandData::getInterval() const
    {
        return m_interval;
    }


    unsigned long PowerDemandData::getRecordedValue() const
    {
        return m_recordedValue;
    }

    
    unsigned long PowerDemandData::getUpdatedValue() const
    {
        return m_updatedValue;
    }


	void PowerDemandData::setRecordedValue(unsigned long recordedValue)
	{
		m_recordedValue = recordedValue;
	}


	void PowerDemandData::setUpdatedValue(unsigned long updatedValue)
	{
		m_updatedValue = updatedValue;
	}

} // closes TA_Base_Core

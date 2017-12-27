/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/DataLogAgentEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/DataLogAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	
    const std::string DataLogAgentEntityData::ENTITY_TYPE = "DataLogAgent";
   
	const std::string DataLogAgentEntityData::DATA_MAX_TIMEOUT = "DataMaxTimeout";

	const std::string DataLogAgentEntityData::TIMEOUT_DELAY = "TimeoutDelay";

	const std::string DataLogAgentEntityData::BATCH_UPDATE_SIZE = "BatchUpdateSize";

	const std::string DataLogAgentEntityData::MAX_DELAY_TIMES = "MaxDelayTimes";
	//++TD16491

    DataLogAgentEntityData::DataLogAgentEntityData()
    {}

    DataLogAgentEntityData::DataLogAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

    void DataLogAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    DataLogAgentEntityData::~DataLogAgentEntityData()
    {
    }

    
    std::string DataLogAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string DataLogAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

     
	int  DataLogAgentEntityData::getDataMaxTimeout()
	{
		int  dataMaxTimeout = -1;
		try
		{
			std::string value = getHelper()->getParameter(DATA_MAX_TIMEOUT);
			dataMaxTimeout = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return dataMaxTimeout;
	}

	int  DataLogAgentEntityData::getTimeoutDelay()
	{
		int  timeoutDelay = -1;
		try
		{
			std::string value = getHelper()->getParameter(TIMEOUT_DELAY);
			timeoutDelay = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return timeoutDelay;
	}

	int  DataLogAgentEntityData::getMaxDelayTimes()
	{
		int  maxDelayTimes = -1;
		try
		{
			std::string value = getHelper()->getParameter(MAX_DELAY_TIMES);
			maxDelayTimes = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return maxDelayTimes;

	}

	int  DataLogAgentEntityData::getBatchUpdateSize()
	{
		int  batchUpdateSize = -1;
		try
		{
			std::string value = getHelper()->getParameter(BATCH_UPDATE_SIZE);
			batchUpdateSize = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return batchUpdateSize;
	}

	//TD16491++
    TA_Base_Core::IEntityData* DataLogAgentEntityData::clone(unsigned long key)
    {
        return( new DataLogAgentEntityData(key) );
    }

}

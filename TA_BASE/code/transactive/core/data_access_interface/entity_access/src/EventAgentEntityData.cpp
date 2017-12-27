/**
  * The source code in this file is the property of 
  * Combuilder PTE LTD and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/EventAgentEntityData.cpp $
  * @author Noel R. Tajanlangit
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/24 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Class implementation for EventEntityData.
  */

#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	const std::string EventAgentEntityData::ENTITY_TYPE = "EventAgent";
	const std::string EventAgentEntityData::ASSET_NAME = "AssetName";
	const std::string EventAgentEntityData::DATABASE_REFRESH_INTERVAL = "DbRefreshInterval";
	const std::string EventAgentEntityData::EVENT_SUBMIT_INTERVAL = "SubmitInterval";
	const std::string EventAgentEntityData::MAX_CACHE_SIZE = "MaxCacheSize";
	const std::string EventAgentEntityData::AGENT_REFRESH_INTERVAL = "AgentRefreshInterval";
	const std::string EventAgentEntityData::PROPAGATE_BATCH_SIZE = "PropagateBatchSize";

	EventAgentEntityData::EventAgentEntityData(){}

	EventAgentEntityData::EventAgentEntityData(unsigned long ulkey) 
		: EntityData(ulkey, getStaticType())
	{
	}

	EventAgentEntityData::~EventAgentEntityData()
	{
	}

	void EventAgentEntityData::invalidate()
	{
		getHelper()->invalidate();
	}

	std::string EventAgentEntityData::getType()
	{
		return getStaticType();
	}

	std::string EventAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

	IEntityData* EventAgentEntityData::clone(unsigned long key)
	{
		return( new EventAgentEntityData(key) );
	}

	std::string EventAgentEntityData::getAssetName()
	{
		std::string assetName("");

		try
		{
			return getHelper()->getParameter(ASSET_NAME);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return assetName;

	}
	
	unsigned long EventAgentEntityData::getDbRefreshInterval()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string dbRefreshInterval = getHelper()->getParameter(DATABASE_REFRESH_INTERVAL);
			return EntityHelper::getUnsignedLongData(dbRefreshInterval);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	unsigned long EventAgentEntityData::getEVSubmitInterval()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string evSubmitInterval = getHelper()->getParameter(EVENT_SUBMIT_INTERVAL);
			return EntityHelper::getUnsignedLongData(evSubmitInterval);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	unsigned long EventAgentEntityData::getMaxCacheSize()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string evCacheSize = getHelper()->getParameter(MAX_CACHE_SIZE);
			return EntityHelper::getUnsignedLongData(evCacheSize);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	unsigned long EventAgentEntityData::getAgentRefreshInterval()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string evDbRefreshInterval = getHelper()->getParameter(AGENT_REFRESH_INTERVAL);
			return EntityHelper::getUnsignedLongData(evDbRefreshInterval);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}
	unsigned long EventAgentEntityData::getPropagationBatchSize()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string evProgBatchSize = getHelper()->getParameter(PROPAGATE_BATCH_SIZE);
			return EntityHelper::getUnsignedLongData(evProgBatchSize);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}
}

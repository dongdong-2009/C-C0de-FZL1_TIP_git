/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ECSManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * ECSManager holds the data specific to a ECSManager entity from the
  * database and allows read-only access to that data. 
  *
  * IMPORTANT: To ensure this object remains thread-safe, any added accessors must be
  * ThreadGuarded as they potentially modify state of object (i.e. if !validData).
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	// set constant static Parameter Names
    const std::string ECSManager::ENTITY_TYPE                            = "ECSManager";
	const std::string ECSManager::STATION_MODE_STATUS_TOKEN_PARAMETER_NAME = "StationModeStatusToken";
	const std::string ECSManager::STATION_MODE_IN_PROGRESS_TOKEN_PARAMETER_NAME = "StationModeInProgressToken";
    //devender++ WP-4677M51520006  09-Sep-2005
	//const std::string ECSManager::STATION_MODE_CONTROL_TOKEN_PARAMETER_NAME = "StationModeControlToken";
	//const std::string ECSManager::PROPOSED_STATION_MODE_TOKEN_PARAMETER_NAME = "ProposedStationModeToken";
	//const std::string ECSManager::OVERRIDING_PROPOSED_STATION_MODE_1_TOKEN_PARAMETER_NAME = "OverridingProposedStationMode1Token"; 
	//const std::string ECSManager::OVERRIDING_PROPOSED_STATION_MODE_2_TOKEN_PARAMETER_NAME = "OverridingProposedStationMode2Token"; 
    //++devender WP-4677M51520006 09-Sep-2005
	const std::string ECSManager::CENTRAL_CONTROL_LOCATION_PARAMETER_NAME = "CentralControlLocation";
    //devender++ WP-4677M51520006 09-Sep-2005
	//const std::string ECSManager::PROPOSED_MASTER_MODE_1_TOKEN_PARAMETER_NAME = "ProposedMasterMode1Token";   
	//const std::string ECSManager::PROPOSED_MASTER_MODE_2_TOKEN_PARAMETER_NAME = "ProposedMasterMode2Token";   
	//const std::string ECSManager::CURRENT_MASTER_MODE_TOKEN_PARAMETER_NAME = "CurrentMasterModeToken";        
    //++devender WP-4677M51520006 09-Sep-2005
    const std::string ECSManager::DATAPOINT_HEALTH_POLL_INTERVAL_PARAMETER_NAME = "DataPointHealthPollInterval";
	const std::string ECSManager::STATION_VISUAL_LOCK_PARAMETER_NAME = "StationVisualLockToken";
	const std::string ECSManager::AUTOMODE_ON_TOKEN_PARAMETER_NAME = "AutoModeOnToken";
    const std::string ECSManager::CURRENT_TIMETABLE_PARAMETER_NAME = "CurrentTimeTableToken";
    const std::string ECSManager::SELECT_TIMETABLE_PARAMETER_NAME = "SelectTimeTableToken";


    ECSManager::ECSManager(unsigned long key) 
        : EntityData(key, getStaticType())
    {
    }


    ECSManager::~ECSManager()
    { 
        // Nothing to clean up
    }


    IEntityData* ECSManager::clone(unsigned long key)
    {
        return new ECSManager(key);        
    }


    void ECSManager::invalidate()
    {
        getHelper()->invalidate();
    }


    std::string ECSManager::getType()
    {
        return getStaticType();
    }


    std::string ECSManager::getStaticType()
    {

        return ENTITY_TYPE;
    }

	std::string ECSManager::getStationModeStatusToken()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(STATION_MODE_STATUS_TOKEN_PARAMETER_NAME);
	}


	std::string ECSManager::getStationModeInProgressToken()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(STATION_MODE_IN_PROGRESS_TOKEN_PARAMETER_NAME);
	}


    //devender++ WP-4677M51520006  09-Sep-2005
	//std::string ECSManager::getStationModeControlToken()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_stationModeControlToken;
	//}


	//std::string ECSManager::getProposedStationModeToken()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_proposedStationModeToken;
	//}
    //++devender WP-4677M51520006 09-Sep-2005


	std::string ECSManager::getStationVisualLockToken()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(STATION_VISUAL_LOCK_PARAMETER_NAME);
	}

    //devender WP-4677M51520006  09-Sep-2005
	//std::string ECSManager::getOverridingProposedStationMode1Token()
	//{/
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_overridingProposedStationMode1Token;
	//}
    
	//std::string ECSManager::getOverridingProposedStationMode2Token()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_overridingProposedStationMode2Token;
	//}
    //++devender WP-4677M51520006  09-Sep-2005

  
	unsigned long ECSManager::getCentralControlLocationKey()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);

		unsigned long centralControlLocationKey = 0;
		std::string value = getHelper()->getParameter(CENTRAL_CONTROL_LOCATION_PARAMETER_NAME);
		if (!value.empty())
		{
			centralControlLocationKey = EntityHelper::getUnsignedLongData(value);
		}
		return centralControlLocationKey;
	}


    int ECSManager::getDataPointHealthPollInterval()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
        int dataPointHealthPollInterval = 0;

		std::string value = getHelper()->getParameter(DATAPOINT_HEALTH_POLL_INTERVAL_PARAMETER_NAME);
		if (!value.empty())
		{
			dataPointHealthPollInterval = EntityHelper::getUnsignedLongData(value);
		}

		return dataPointHealthPollInterval;
	}


    //devender WP-4677M51520006 09-Sep-2005
	//std::string ECSManager::getProposedMasterMode1Token()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_proposedMasterMode1Token;
	//}


	//std::string ECSManager::getProposedMasterMode2Token()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_proposedMasterMode2Token;
	//}


	//std::string ECSManager::getCurrentMasterModeToken()
	//{
	//	//
	//	// Guard the method to ensure access by client code is synchronous
	//	//
	//	ThreadGuard guard(m_threadLock);
    //
	//	validateData(); // ensure its up to date
	//	return m_currentMasterModeToken;
	//}
    //++devender WP-4677M51520006 09-Sep-2005

    std::string ECSManager::getAutoModeOnToken()
	{
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(AUTOMODE_ON_TOKEN_PARAMETER_NAME);
	}

    std::string ECSManager::getCurrentTimeTableToken()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(CURRENT_TIMETABLE_PARAMETER_NAME);
	}


	std::string ECSManager::getSelectTimeTableToken()
	{
		//
		// Guard the method to ensure access by client code is synchronous
		//
		ThreadGuard guard(m_threadLock);
		return getHelper()->getParameter(SELECT_TIMETABLE_PARAMETER_NAME);
	}

} // namespace TA_Base_Core


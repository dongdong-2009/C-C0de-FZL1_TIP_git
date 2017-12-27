/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyDbCacheUpdater.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 * Last modification: $DateTime: 2015/01/21 10:20:13 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Implements DutyDbCacheUpdater.
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyDbCacheUpdater::DutyDbCacheUpdater( DutyDbCacheImpl& dbCache ) :
		m_dbCache(dbCache)
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::LOCATION , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::SUBSYSTEM, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::PROFILE  , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::SECURITY  , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::OPERATOR , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerInterest(
			TA_Base_Core::ENTITY   , *this, dbCache.m_controlStationKey );
	}

	DutyDbCacheUpdater::~DutyDbCacheUpdater()
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::LOCATION , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::SUBSYSTEM, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::PROFILE  , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::SECURITY  , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::OPERATOR , *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::ENTITY   , *this );
	}


	void DutyDbCacheUpdater::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		switch( updateEvent.getType() )
		{
			case TA_Base_Core::LOCATION:
			{
				switch( updateEvent.getModifications() )
				{
					case TA_Base_Core::Create:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Attempt to add a location while Duty Agent is running: location=%lu", updateEvent.getKey() );
					}
					break;
						

					case TA_Base_Core::Update:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Update received for location %lu", updateEvent.getKey() );

						{
							ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_locationLock);
							DutyDbCacheImpl::LocationMap::iterator it;

							if( (it=m_dbCache.m_locationMap.find(updateEvent.getKey())) != m_dbCache.m_locationMap.end() )
							{
								delete it->second;
								it->second = NULL;
								m_dbCache.m_locationMap.erase(it);
							}
						}

						{
							ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_regionLock);
							DutyDbCacheImpl::RegionMap::iterator it;

							if( (it=m_dbCache.m_regionMap.find(updateEvent.getKey())) != m_dbCache.m_regionMap.end() )
							{
								delete it->second;
								it->second = NULL;
								m_dbCache.m_regionMap.erase(it);
							}
						}
					}
					break;

					case TA_Base_Core::Delete:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempt to remove a location while Duty Agent is running: location=%lu", updateEvent.getKey() );
					}
					break;

					default:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unexpected location update type: %lu",  updateEvent.getModifications() );
					}
					break;
				}
			}
			break;

			case TA_Base_Core::SUBSYSTEM:
			{

				switch( updateEvent.getModifications() )
				{
					case TA_Base_Core::Create:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Attempt to add a subsystem while Duty Agent is running: subsystem=%lu", updateEvent.getKey() );
					}
					break;
						

					case TA_Base_Core::Update:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Update received for location %lu", updateEvent.getKey() );

						ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_subsystemLock);
						DutyDbCacheImpl::SubsystemMap::iterator it;

						if( (it=m_dbCache.m_subsystemMap.find(updateEvent.getKey())) != m_dbCache.m_subsystemMap.end() )
						{
							delete it->second;
							it->second = NULL;
							m_dbCache.m_subsystemMap.erase(it);
						}
					}
					break;

					case TA_Base_Core::Delete:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempt to remove a subsystem while Duty Agent is running: subsystem=%lu", updateEvent.getKey() );
					}
					break;

					default:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unexpected subsystem update type: %lu",  updateEvent.getModifications() );
					}
					break;
				}
			}
			break;

			case TA_Base_Core::SECURITY:
				// don't break
			case TA_Base_Core::PROFILE:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Update received for security/profile %lu", updateEvent.getKey() );

				ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_profileLock);
				DutyDbCacheImpl::ProfileMap::iterator it;

				if( (it=m_dbCache.m_profileMap.find(updateEvent.getKey())) != m_dbCache.m_profileMap.end() )
				{
					delete it->second;
					it->second = NULL;
					m_dbCache.m_profileMap.erase(it);
				}
			}
			break;
		
			case TA_Base_Core::OPERATOR:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Update received for operator %lu", updateEvent.getKey() );
				
				ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_operatorLock);
				DutyDbCacheImpl::OperatorMap::iterator it;

				if( (it=m_dbCache.m_operatorMap.find(updateEvent.getKey())) != m_dbCache.m_operatorMap.end() )
				{
					delete it->second;
					it->second = NULL;
					m_dbCache.m_operatorMap.erase(it);
				}
			}
			break;

			case TA_Base_Core::ENTITY:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Update received for entity %lu", updateEvent.getKey() );
				
				ACE_Guard<ACE_Thread_Mutex> guard(m_dbCache.m_entityLock);
				DutyDbCacheImpl::EntityMap::iterator it;

				if( (it=m_dbCache.m_entityMap.find(updateEvent.getKey())) != m_dbCache.m_entityMap.end() )
				{
					delete it->second;
					it->second = NULL;
					m_dbCache.m_entityMap.erase(it);
				}
			}
			break;

			default:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unexpected update type %lu", updateEvent.getType() );
			}
			break;
		}
	}
}


/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/xacml_factory/src/ContextOnlineUpdatable.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 10:20:13 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Implements the ContextOnlineUpdatable interface.
 *
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{
	
	ContextOnlineUpdatable::ContextOnlineUpdatable( RightsType::ProfileMap& profileMap,
		ACE_RW_Mutex& profileLock, RightsType::ActionMap& actionMap,
		ACE_RW_Mutex& actionLock, RightsType::ResourceMap& resourceMap,
		ACE_RW_Mutex& resourceLock, RightsType::SubsystemMap& subsystemMap,
		ACE_RW_Mutex& subsystemLock, RightsType::ActionGroupMap& actionGroupMap,
		ACE_RW_Mutex& dutyLock ) :
		m_profileMap(profileMap),
		m_profileLock(profileLock),
		m_actionMap(actionMap),
		m_actionLock(actionLock),
		m_resourceMap(resourceMap),
		m_resourceLock(resourceLock),
		m_subsystemMap(subsystemMap),
		m_subsystemLock(subsystemLock),
		m_actionGroupMap(actionGroupMap),
		m_dutyLock(dutyLock)
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::PROFILE, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::SECURITY, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::ACTION_GROUP, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::LOCATION, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::SUBSYSTEM, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
			TA_Base_Core::ENTITY, *this );
	}

	ContextOnlineUpdatable::~ContextOnlineUpdatable()
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::PROFILE, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::SECURITY, *this );		
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::ACTION_GROUP, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::LOCATION, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::SUBSYSTEM, *this );
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
			TA_Base_Core::ENTITY, *this );
	}

	void ContextOnlineUpdatable::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		switch( updateEvent.getType() )
		{
			case TA_Base_Core::PROFILE:
				// don't break
			case TA_Base_Core::SECURITY:
			{
				switch( updateEvent.getModifications() )
				{
					case TA_Base_Core::Create: // we'll find out in good time
					{
						// do nothing
					}
					break;

					case TA_Base_Core::Update:
					// don't break
					case TA_Base_Core::Delete:
					{
						ACE_Write_Guard<ACE_RW_Mutex> guard(m_profileLock);
						
						RightsType::ProfileMap::iterator it;

						it = m_profileMap.find(updateEvent.getKey());

						if( it != m_profileMap.end() )
						{
							delete it->second;
							it->second = NULL;
							m_profileMap.erase(it);
						}
					}
					break;

					default:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unknown online update mod type: %lu", updateEvent.getModifications() );
					}
					break;
				}
			}
			break;
		
			case TA_Base_Core::ACTION_GROUP:
			{
				switch( updateEvent.getModifications() )
				{
					case TA_Base_Core::Create: // actions may be added
					// don't break
					case TA_Base_Core::Update: // actions may have been added or removed
					// don't break
					case TA_Base_Core::Delete: // actions may be removed
					{
						{
							ACE_Write_Guard<ACE_RW_Mutex> guard(m_actionLock);

							RightsType::ActionMap::iterator it;

							for( it = m_actionMap.begin(); it != m_actionMap.end(); it++ )
							{
								delete it->second;
								it->second = NULL;
							}

							m_actionMap.clear();
						}
						{
							ACE_Write_Guard<ACE_RW_Mutex> guard(m_dutyLock);

							RightsType::ActionGroupMap::iterator it;

							for( it = m_actionGroupMap.begin(); it != m_actionGroupMap.end(); it++ )
							{
									delete it->second;
									it->second = NULL;
							}

							m_actionGroupMap.clear();
						}
					}
					break;
						
					default:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unknown online update mod type: %lu", updateEvent.getModifications() );
					}
					break;
				}
			}
			break;

			case TA_Base_Core::LOCATION:
			// don't break
			case TA_Base_Core::SUBSYSTEM:
			// don't break
			case TA_Base_Core::ENTITY:
			{
				switch( updateEvent.getModifications() )
				{
					case TA_Base_Core::Create:
					{
						// do nothing
					}
					break;

					case TA_Base_Core::Update:
					// don't break
					case TA_Base_Core::Delete:
					{
						// TODO: this is inefficient
						// however, prototype before polish 
						{
							ACE_Write_Guard<ACE_RW_Mutex> guard(m_resourceLock);

							RightsType::ResourceMap::iterator it;

							for( it = m_resourceMap.begin(); it != m_resourceMap.end(); it++ )
							{
								delete it->second;
								it->second = NULL;
							}

							m_resourceMap.clear();
						}
						{
							ACE_Write_Guard<ACE_RW_Mutex> guard(m_subsystemLock);

							RightsType::SubsystemMap::iterator it;

							for( it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++ )
							{
								delete it->second;
								it->second = NULL;
							}

							m_subsystemMap.clear();
						}

					}
					break;

					default:
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unknown online update mod type: %lu", updateEvent.getModifications() );
					}
					break;
				}
			}
			break;

			default:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received config update with unexpected type %lu", updateEvent.getType() );
			}
			break;
		}
	}
}


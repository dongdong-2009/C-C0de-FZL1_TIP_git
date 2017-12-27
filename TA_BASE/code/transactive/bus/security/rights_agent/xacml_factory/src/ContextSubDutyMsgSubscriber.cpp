/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/xacml_factory/src/ContextSubDutyMsgSubscriber.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Implements the ContextSubDutyMsgSubscriber interface.
 *
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{

    ContextSubDutyMsgSubscriber::ContextSubDutyMsgSubscriber( RightsType::LocSubDutyMap& locSubDutyMap,
		ACE_RW_Mutex& locSubDutyLock ) :
		m_locSubDutyMap(locSubDutyMap),
		m_locSubDutyLock(locSubDutyLock),
		MAX_LSDM_SIZE(5000),
		m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
		getCommsMessageSender(TA_Base_Core::DutyAgentComms::Context)),
		m_pUpdate(NULL),
		m_version(0)
    {
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);
		
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
            TA_Base_Core::DutyAgentBroadcastComms::SubsubsystemDutyChangeNotification, this, NULL);

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification, this, NULL);

		m_locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );
		std::stringstream ss;
		ss << m_locationKey;
		m_locationStr = ss.str();

		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 5000, false );
    }

    ContextSubDutyMsgSubscriber::~ContextSubDutyMsgSubscriber()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this);
    }

    void ContextSubDutyMsgSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {     
		std::string msgTypeKey(message.messageTypeKey);

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"ContextSubDutyMsgSubscriber::receiveSpecialisedMessage received message %s ", msgTypeKey.c_str());

		if( 0 == msgTypeKey.compare(TA_Base_Core::DutyAgentBroadcastComms::SubsubsystemDutyChangeNotification.getTypeKey()) )
        {
			DutyAgentTypeCorbaDef::SubsystemDutyChanges* data = NULL;
            if (0 != ( message.messageState >>= data ) )
            {
				update(data->add, true);
				update(data->remove,false);

// 				TA_Base_Core::FilterData filterData;
// 				TA_Base_Core::NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, m_locationStr );
// 				filterData.push_back(&regionFilter);

// 				CORBA::Any commsData;
// 				commsData <<= data->rightsUpdate;
// 				//m_commsSender->sendBroadcastCommsMessage(TA_Base_Core::DutyAgentBroadcastComms::RightsLibraryCacheUpdate,commsData,&filterData);
// 				m_commsSender->sendCommsMessage(TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate,
// 					TA_Base_Bus::RightsAgentInfo::m_entity,commsData,
// 					TA_Base_Bus::RightsAgentInfo::m_subsystem,TA_Base_Bus::RightsAgentInfo::m_location,NULL);

				this->accumulateUpdates(data->rightsUpdate);				

            }
        }
        else if( 0 == TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty.getTypeKey().compare(message.messageTypeKey) )
        {
			DutyAgentTypeCorbaDef::SubsystemDutySequence* subsystemDutyUpdate = NULL;

			if((message.messageState >>= subsystemDutyUpdate) != 0)
		    {
                LOG(SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received subsystem update (add)");

                update(*subsystemDutyUpdate, true);
            }
			return;
        }
		else if(0 == TA_Base_Core::DutyAgentBroadcastComms::RemoveSubsystemDuty.getTypeKey().compare(message.messageTypeKey))
        {
            DutyAgentTypeCorbaDef::SubsystemDutySequence* subsystemDutyUpdate = NULL;
            
		    if((message.messageState >>= subsystemDutyUpdate) != 0)
		    {
                LOG(SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received subsystem update (remove)");

			    update(*subsystemDutyUpdate, false);
            }
        }
		else if( 0 == TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage.getTypeKey().compare(message.messageTypeKey) )
		{
			DutyAgentTypeCorbaDef::Key locationKey = 0;
			if((message.messageState >>= locationKey) != 0)
			{
                LOG(SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, 
					"Received Duty Agent switch over message for location %d. Will clean the duty cache", 
					locationKey);

				ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
				m_locSubDutyMap.clear();
            }
		}
		else if ( 0 == TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification.getTypeKey().compare(message.messageTypeKey) )
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::StationStatus* data = NULL;
			DutyAgentTypeCorbaDef::Key locationKey = 0;
			if ( 0 != (message.messageState >>= data) )
			{
				locationKey = data->regionKey;

				LOG_GENERIC(SOURCE_INFO, TA_Base_Core::DebugUtil::DebugInfo, 
					"Received Duty Agent Station OnLine/OffLine message for location %d. RightsAgent will clean the duty cache at this location.", 
					locationKey);

				// Clean this specified location's cache.
				this->resetLocationCache(locationKey);
			}
		}

    }

	void ContextSubDutyMsgSubscriber::update(const DutyAgentTypeCorbaDef::SubsystemDutySequence& update, bool isAdd )
    {
		if( m_locSubDutyMap.size() > MAX_LSDM_SIZE )
		{
			ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
			m_locSubDutyMap.clear();
		}

		for( int i=0; i< update.length(); i++ )
		{
			LocSubSesProf lssp( update[i].regionKey, update[i].subsystemKey,
				update[i].session.in(), update[i].profileKey );

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "loc: %lu, sub: %lu, session:%s, profile:%lu, duty: %lu add:%d",
				lssp.m_location, lssp.m_subsystem, lssp.m_sessionId.c_str(), lssp.m_profile,  update[i].actionGroupKey , isAdd);
            // TD10259: Remove update not correctly handled. If the update isn't add it must be remove, see
            //          receiveSpecialisedMessage above.
			if( isAdd )
			{
			    unsigned long newDuty;
				newDuty = update[i].actionGroupKey;
				RightsType::LocSubDutyMap::iterator it;
				{
					ACE_Read_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);				
					it = m_locSubDutyMap.find( lssp );
				}
				
				if( it == m_locSubDutyMap.end() )
				{
					ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
					m_locSubDutyMap.insert( RightsType::LocSubDutyMap::value_type(lssp, newDuty) );
				}
				else
				{
					it->second = newDuty;
			    }
			}
            else
            {
                ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);

                // The update was a removal, so find the item to remove.
                RightsType::LocSubDutyMap::iterator it = m_locSubDutyMap.find( lssp );
                if ( it != m_locSubDutyMap.end() )
                {
                    // Item found so remove it.
					ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
                    m_locSubDutyMap.erase( it );
                }
            }
		}
    }

	void ContextSubDutyMsgSubscriber::resetLocationCache( unsigned long locationKey )
	{
		RightsType::LocSubDutyMap::iterator iter;
		iter = m_locSubDutyMap.begin();

		RightsType::LocSubDutyMap::const_iterator iterEnd = m_locSubDutyMap.end();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Location-subystem Duty Map have size %d before reset location %d cache.", m_locSubDutyMap.size(), locationKey);

		while ( iter != iterEnd )
		{
			if ( iter->first.m_location == locationKey )
			{
				ACE_Write_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
				m_locSubDutyMap.erase( iter );
			}

			++iter;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Location-subystem Duty Map have size %d after reset location %d cache.", m_locSubDutyMap.size(), locationKey);
	}

	void ContextSubDutyMsgSubscriber::threadedTimerExpired(long timerId, void* userData)
	{
		ACE_Guard<ACE_Mutex> guard(m_mutex);

		if(0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL))
		{
		
			if(m_pUpdate != NULL && m_pUpdate->rightsUpdate.length() > 0 )
			{
// 				commsData <<= (m_pUpdate->rightsUpdate);
// 				//m_commsSender->sendBroadcastCommsMessage(TA_Base_Core::DutyAgentBroadcastComms::RightsLibraryCacheUpdate,commsData,&filterData);
// 				m_commsSender->sendCommsMessage(TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate,
// 					TA_Base_Bus::RightsAgentInfo::m_entity,commsData,
// 					TA_Base_Bus::RightsAgentInfo::m_subsystem,TA_Base_Bus::RightsAgentInfo::m_location,NULL);

				
				//start from zero if the version number reach the max value
				if( m_version == ULONG_MAX)
				{
					m_version =0;
				}			
				else
				{
					++m_version ;
				}

				CORBA::Any commsDataVer;
				
				m_pUpdate->versionNum = m_version;
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "update message version number is : %lu", m_version);
				commsDataVer <<= (*m_pUpdate);
				
				m_commsSender->sendCommsMessage(TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate,
					TA_Base_Bus::RightsAgentInfo::m_entity,commsDataVer,
					TA_Base_Bus::RightsAgentInfo::m_subsystem,TA_Base_Bus::RightsAgentInfo::m_location,NULL);

				CORBA::Any commsData;
				commsData <<= (m_pUpdate->rightsUpdate);
				m_commsSender->sendCommsMessage(TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate,
					TA_Base_Bus::RightsAgentInfo::m_entity,commsData,
					TA_Base_Bus::RightsAgentInfo::m_subsystem,TA_Base_Bus::RightsAgentInfo::m_location,NULL);
			}
			delete m_pUpdate;
			m_pUpdate = NULL;
		}
		
	}
	void ContextSubDutyMsgSubscriber::accumulateUpdates(TA_Base_Bus::DutyAgentTypeCorbaDef::RightsLibraryUpdateSequence& updates)
	{
		ACE_Guard<ACE_Mutex> guard(m_mutex);

		if(0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL))
		{	
			if(m_pUpdate == NULL)
			{
				m_pUpdate = new TA_Base_Bus::DutyAgentTypeCorbaDef::VersionedRightsLibraryUpdate();
			}	
			int end = updates.length() ;
			if(end > 0)
			{
				int startIdx = m_pUpdate->rightsUpdate.length();
				m_pUpdate->rightsUpdate.length(startIdx + end);
				for(int i=0; i< end; i ++)
				{
					(m_pUpdate->rightsUpdate)[startIdx + i] = updates[i];
				}
			}
		}
	}

} // TA_Base_App

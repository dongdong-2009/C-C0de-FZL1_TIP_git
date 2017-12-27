/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/DutyCommsSubscriber.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/02/11 18:28:17 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Implements the DutyCommsSubscriber interface.
 *
 */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "bus/security/rights_library/src/RightsLibraryInternal.h"
using namespace TA_Base_Core;
namespace TA_Base_Bus
{

    DutyCommsSubscriber::DutyCommsSubscriber( RightsCache& rightsCache ) :
		m_rightsCache(rightsCache),
		m_firstUpdate(true),
		m_version(0)
    {
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );


// 		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
// 			TA_Base_Core::DutyAgentBroadcastComms::RightsLibraryCacheUpdate, this, &filterData);

		IEntityDataList dataList =  EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(RightsAgentEntityData::getStaticType(),locationKey);
		if(dataList.size()==0)
		{
			std::ostringstream message;
			message << "There is no RightsAgent at location '" << locationKey << "'";
			
			const char* fld = "RightsLibrary";
			const char* msg = message.str().c_str();
			
			TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
		}
		
		TA_Base_Core::IEntityData* data = dataList[0];
		unsigned long entityKey = data->getKey();
		unsigned long  subsystemKey = data->getSubsystem();

		delete data;
		std::stringstream ss;
		ss << locationKey;
		std::string locationStr = ss.str();
		//here should only subscribe the messages from corresponding rightsagent		
		TA_Base_Core::FilterData filterData;
		TA_Base_Core::NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, locationStr );
		filterData.push_back(&regionFilter);

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"SubscribeToCommsMessage to DutyAgentComms::RightsLibraryCacheUpdate message: EntityKey:%lu, SubsystemKey:%lu, LocationKey:%lu",
			entityKey,subsystemKey,locationKey);
		
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate,
			this,entityKey,subsystemKey,locationKey,NULL);

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate, this, &filterData);

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);

		// FZL1 don't need StationOnLineOffLineNotification message, because this message is for DutyCenter(C955)
		//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		//	TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification, this, NULL);

    }

    DutyCommsSubscriber::~DutyCommsSubscriber()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }

    void DutyCommsSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");
    
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DutyCommsSubscriber::receiveSpecialisedMessage messagetype: %s",
			message.messageTypeKey.in());
		DutyAgentTypeCorbaDef::VersionedRightsLibraryUpdate_var versionedUpdates(NULL);
		DutyAgentTypeCorbaDef::RightsLibraryUpdateSequence_var updateSeq(NULL);
		bool bRightsChanged = false;
	
		std::string session = m_rightsCache.getSession();
		bool validSession = (session.length() > 0);

		std::list<unsigned long> emptySubsystems;

		DutyChanges dutyChanges;
		SessionDutyChanges sessionDutyChanges; 
		try
		{
			if( 0 == TA_Base_Core::DutyAgentComms::RightsLibraryCacheUpdate.getTypeKey().compare(message.messageTypeKey) )
			{
				if((message.messageState >>= versionedUpdates.out()) != 0)
				{

					updateSeq = &(versionedUpdates->rightsUpdate);

					if(m_firstUpdate)
					{
						m_firstUpdate=false;
						m_version = versionedUpdates->versionNum;
					}
					else
					{
						CORBA::ULong newVersion = versionedUpdates->versionNum;
						CORBA::ULong expectedVersion = 0;
						if(m_version != ULONG_MAX)
						{
							expectedVersion = m_version +1;
						}
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"version: %lu, expected version:%lu, arriving version:%lu",
								m_version, expectedVersion, newVersion);

						if(newVersion != expectedVersion)
						{
							//recalculate the cache
							m_rightsCache.reload();
							LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Before release corba var");
							updateSeq._retn();
							versionedUpdates._retn();
							LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end release corba var");
							m_version= newVersion;
							return;
						}						
						m_version = expectedVersion;

					}
					CORBA::ULong regIdx = 0;
					CORBA::ULong subIdx = 0;
					CORBA::ULong sesIdx = 0;

					for( regIdx=0; regIdx<updateSeq->length(); regIdx++ )
					{
						for( subIdx=0; subIdx<updateSeq[regIdx].duty.length(); subIdx++ )
						{
							for( sesIdx=0; sesIdx<updateSeq[regIdx].duty[subIdx].sessions.length(); sesIdx++ )
							{
								bool changed = false;
								changed = m_rightsCache.processDutyUpdate( updateSeq[regIdx].duty[subIdx].sessions[sesIdx].in(),
									updateSeq[regIdx].region,
									updateSeq[regIdx].duty[subIdx].subsystem );

								if(changed )
								{								
									std::pair<SessionDutyChanges::iterator, bool> sesDutyChangesResult =	
										sessionDutyChanges.insert(SessionDutyChanges::value_type(updateSeq[regIdx].duty[subIdx].sessions[sesIdx].in(),
																	dutyChanges));
									std::pair<DutyChanges::iterator, bool> dutyChangesResult =
										sesDutyChangesResult.first->second.insert(DutyChanges::value_type(updateSeq[regIdx].region,emptySubsystems));

									dutyChangesResult.first->second.push_back(updateSeq[regIdx].duty[subIdx].subsystem );
									bRightsChanged = true;
								}								
							}
						}
					}
				}				
			}
			else if(0 == TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage.getTypeKey().compare(message.messageTypeKey) )
			{
				DutyAgentTypeCorbaDef::Key locationKey = 0;
				if((message.messageState >>= locationKey) != 0)
				{
					LOG_GENERIC(SOURCE_INFO, TA_Base_Core::DebugUtil::DebugInfo, 
						"Received Duty Agent switch over message for location %d. Will clean the duty cache", 
						locationKey);
					
					m_rightsCache.reset(locationKey); // reset will notify the changes
					//bRightsChanged = true;
				}
			}
			else if (0 == TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification.getTypeKey().compare(message.messageTypeKey) )
			{
				TA_Base_Bus::DutyAgentTypeCorbaDef::StationStatus* data = NULL;
				DutyAgentTypeCorbaDef::Key locationKey = 0;
				if ( 0 != (message.messageState >>= data) )
				{
					locationKey = data->regionKey;

					LOG_GENERIC(SOURCE_INFO, TA_Base_Core::DebugUtil::DebugInfo, 
						"Received Duty Agent Station OnLine/OffLine message for location %d. RightsLibrary will clean the duty cache at this location.", 
						locationKey);

					m_rightsCache.reset(locationKey);
				}
			}
			if(bRightsChanged)
			{
				SessionDutyChanges::const_iterator end = sessionDutyChanges.end();
				for(SessionDutyChanges::iterator itr = sessionDutyChanges.begin(); itr != end; ++itr)
				{
					std::string session = itr->first;
					m_rightsCache.notifyRightsChange(session, RC_DutyUpdate,itr->second);
				}
				
			}
			
		}
		catch(...)
		{
			updateSeq._retn();
			versionedUpdates._retn();
			throw;
		}

		updateSeq._retn();
		versionedUpdates._retn();
		FUNCTION_EXIT;
    }

} // TA_Base_App

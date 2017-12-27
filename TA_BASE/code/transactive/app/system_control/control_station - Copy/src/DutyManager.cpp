/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyManager.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages all duty related tasks such as popping out the 
  * duty request and notification dialogs
  */

#include "app/system_control/control_station/src/ControlStationRootDialog.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/ProcessManager.h"
#include "app/system_control/control_station/src/DutyManager.h"
#include "core\data_access_interface\entity_access/src/IEntityData.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\data_access_interface\src\ILocation.h"
#include "core\data_access_interface\src\IProfile.h"
#include "core\data_access_interface\src\ISubsystem.h"
#include "core\data_access_interface\src\Profile.h"
#include "core\data_access_interface\src\SessionAccessFactory.h"
#include "core\data_access_interface\src\ProfileAccessFactory.h"
#include "core\data_access_interface\src\RegionAccessFactory.h"
#include "core\data_access_interface\src\SubsystemAccessFactory.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core\message\src\MessageSubscriptionManager.h"
#include "core\message\IDL\src\CommsMessageCorbaDef.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "core\corba\src\CorbaUtil.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/authentication_agent/idl/src/SessionInfoCorbaDef.h"

#include "app/system_control/control_station/src/ExclusiveDutyDialog.h"
#include "app/system_control/control_station/src/OverRideExclusiveDuty.h" // TD19409 yg

#include <algorithm> // TD19075

#include <memory>

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::Profile;
using TA_Base_Core::IProfile;
using TA_Base_Core::ProfileAccessFactory;
using TA_Base_Core::SessionAccessFactory;
using TA_Base_Core::RegionAccessFactory;
using TA_Base_Core::SubsystemAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::OperatorAccessFactory;
using namespace TA_Base_Core::DutyAgentBroadcastComms;
using TA_Base_Core::TransactiveException;

DutyManager::DutyManager()
{
	FUNCTION_ENTRY("Constructor");
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		DutyChangedNotification, this, NULL);
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		DutyRequest, this, NULL);

    // Need to get the name of the duty agent so we can communicate with it when the user logs out.
    TA_Base_Core::CorbaNameList dutyAgentName;
    try
    {
		TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "RPARAM_LOCATIONKEY unset" );

		// Get the entity name based on the locationKey
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );

		std::string agentType = TA_Base_Core::DutyAgentEntityData::getStaticType();

		// this will only return a single entry
		dutyAgentName = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, locationKey,true );

        // If more than one name was returned for the duty agent, then that's a bad thing (it's a theoretically
        // impossible situation, but as there is little we can do to resolve it, we'll just use the first one 
        // on the list. Need to log the problem though.
		if(dutyAgentName.size() != 1)
        { 
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"More than one duty agent name resolved for location key %d.",
                locationKey);
            LOGMORE(SourceInfo,"Will use first name provided: %s",dutyAgentName[0]);
        }
    }
    catch(...)
    {
        // Okay, if the name of the duty agent can't be found in the database, then the processing shouldn't really
        // reach hear (the Auth Agent won't run without a duty agent at the very least), however, if it DOES then
        // just log it (the duty agent object is only used for informational purposes, and so the system can 
        // still run without it.
        LOG_EXCEPTION_CATCH(SourceInfo,"General Excpetion",
            "Exception caught while retrieving database information for the Duty Agent.");
    }

    try
    {
        // Set the agent name - this is the name of the agent to contact.
        m_dutyAgentQuery.setCorbaName( dutyAgentName[0] );
    }
    catch(...)
    {
        // As with the database exception, just log that it happened and move on.
        LOG_EXCEPTION_CATCH(SourceInfo,"General Excpetion",
            "Exception caught while attempting to set the duty agent named object.");
    }
}


DutyManager::~DutyManager()
{
    FUNCTION_ENTRY("~DutyManager");
    try
    {
        // Unsubscribe the subscriber
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }
    catch (...)
    {
        // Catch all exceptions so we don't get errors
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of DutyManager");
    }
    FUNCTION_EXIT;
}



void DutyManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");
    try
    {
        //
        // message from the duty agent
        //
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Got a Comms message.");

        std::string mesgTypeKey = message.messageTypeKey;
        if (0 == mesgTypeKey.compare(DutyChangedNotification.getTypeKey()))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Processing duty change message.");
            // need to display notification dialog
            processDutyChange(message);
        }
        else if (0 == mesgTypeKey.compare(DutyRequest.getTypeKey()))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Processing duty update message.");
            // need to display request duty dialog
            processDutyRequest(message);
        }
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught some error when processing specialised message");
    }
    FUNCTION_EXIT;
}


void DutyManager::respond(const std::string& uniqueId, const TA_Base_App::TA_ControlStation::EDutyResponse response)
{
    ThreadGuard guard(m_callbackLock);
    // see if it is already in the map, if it is, then we've already done the request so just ignore
    if (m_callback.find(uniqueId) == m_callback.end())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Unique Id not in map.  Ignore.");
        return;
    }

    // match our internal EDutyResponse to TA_Base_Bus::IDutyAgentRespCorbaDef::EDutyResponse rather 
    // than typecasting it just in case it's in a different order
    using namespace TA_Base_App::TA_ControlStation;
    TA_Base_Bus::IDutyAgentRespCorbaDef::EDutyResponse resp;
    switch (response)
    {
    case ACCEPT:
        resp = TA_Base_Bus::IDutyAgentRespCorbaDef::DR_ACCEPT;
        break;
    case DENY:
        resp = TA_Base_Bus::IDutyAgentRespCorbaDef::DR_DENY;
        break;
    case TIMEOUT:
        resp = TA_Base_Bus::IDutyAgentRespCorbaDef::DR_TIMEOUT;
        break;
    default:
        TA_ASSERT(false, "Unknown response");
    }
    m_callback[uniqueId]->respResponse(uniqueId.c_str(), resp);
    CORBA::release(m_callback[uniqueId]);
    m_callback.erase(uniqueId);
    
}
    

// TD19075
void DutyManager::processDutyChange(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("processDutyChange");
    std::string currentId = SessionManager::getInstance().getSessionDetails().getSessionId();
	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
		"[processDutyChange]1 Get in ProcessDutyChange, SessionID:%s", currentId.c_str());

	TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup* pDutyPopup;
	TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopupSequence* pDutySeq;
    // process and repackage the data so that the GUI could use it straightaway
    DutyNotificationDetail* det = new struct DutyNotificationDetail;
	bool isDutyUpdated;

	try
	{
		if ( (message.messageState >>= pDutyPopup) != 0 ) // for DutyPopup
		{
			if (currentId.compare(pDutyPopup->targetSession) != 0)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
					"Received a DutyChangedNotification for DutyPopup, but doesn't affect current session.  Ignore[%s]", pDutyPopup->targetSession.in());
				FUNCTION_EXIT;
				return;
			}

			// repackage data
			repackageDutyData(pDutyPopup->gained, det->gained);
			repackageDutyData(pDutyPopup->denied, det->denied);
			repackageDutyData(pDutyPopup->lost, det->lost);
			det->changed.clear();

			isDutyUpdated = ((det->gained.size() > 0) || (det->denied.size() > 0) 
				|| (det->lost.size() > 0) || (det->changed.size() > 0));
			if (isDutyUpdated)
			{
				ControlStationRootDialog::getInstance().PostMessage(WM_DUTY_CHANGE, reinterpret_cast<WPARAM>(det), 0);
			}
		}
		else if ( (message.messageState >>= pDutySeq) != 0 ) // for DutySeq
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopupSequence& dutySeq = *pDutySeq;

			int nDutyDetails = 0;
			int iIndex = 0;
			for( iIndex = 0; iIndex < dutySeq.length(); ++iIndex)
			{
				if (currentId.compare(dutySeq[iIndex].targetSession) == 0)
				{
					++nDutyDetails;
				}
			}
			if (0 == nDutyDetails)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
					"Received a DutyChangedNotification for DutySequence, but doesn't affect current session.  Ignore[%s]", dutySeq[iIndex].targetSession.in());
				FUNCTION_EXIT;
				return;
			}
			else
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
					"[processDutyChange]3 get DutyDetail:%d", nDutyDetails);
			}

			// check session ID and repackage data
			for (unsigned int i=0; i < dutySeq.length(); ++i)
			{
				if(currentId.compare(dutySeq[i].targetSession) != 0)
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
						"One of DutyDetail is on concern with current session, index:%d, sessionID:%s",
						i, dutySeq[i].targetSession.in());
					continue;
				}
				repackageDutyData(dutySeq[i].gained, det->gained);
				repackageDutyData(dutySeq[i].denied, det->denied);
				repackageDutyData(dutySeq[i].lost, det->lost);
			}
			// get duty changed data
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"[processDutyChange] before getDutyChanged, Gained:%d   Lost:%d   Deined:%d   Changed:%d",
				det->gained.size(), det->lost.size(), det->denied.size(), det->changed.size() );
			getDutyChanged(det);
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"[processDutyChange] after getDutyChanged, Gained:%d   Lost:%d   Deined:%d   Changed:%d",
				det->gained.size(), det->lost.size(), det->denied.size(), det->changed.size() );

			isDutyUpdated = ((det->gained.size() > 0) || (det->denied.size() > 0) 
				|| (det->lost.size() > 0) || (det->changed.size() > 0));

			if (isDutyUpdated)
			{
				ControlStationRootDialog::getInstance().PostMessage(WM_DUTY_CHANGE, reinterpret_cast<WPARAM>(det), 1);
			}
		}
		else
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Received a DutyChangedNotification but couldn't extract DutyPopup or DutySequence. Ignore");
			FUNCTION_EXIT;
			return;
		}
	}
	catch(TA_Base_Core::TransactiveException& err)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());

		TA_Base_Bus::TransActiveMessage userMsg;
		const std::string errorMsg = "未能显示权限消息。原因：" +  std::string(err.what());
		userMsg<<errorMsg;
		userMsg.showMsgBox(IDS_UW_050018, "ControlStation");
	}

    // doesn't affects the current session, ignore
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
        "Received a DutyChangedNotification message");


    if (isDutyUpdated)
    {
        
		if(SessionManager::getInstance().getProcessManager())
		{
			// Force the applications to update since we've gained or lost duty
			dynamic_cast<ProcessManager*>(SessionManager::getInstance().getProcessManager())->updateSessionId(currentId);

			// There is now a dedicated call for duty updates, so will remove the above call come IT21
			dynamic_cast<ProcessManager*>(SessionManager::getInstance().getProcessManager())->dutyChanged();
		}
    }
	else
	{
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
			"Received Duty Change DutyChangedNotification, but no data exist!");
	}
    FUNCTION_EXIT;
}
// ++TD19075


void DutyManager::repackageDutyData(TA_Base_Bus::DutyAgentTypeCorbaDef::DutyTreeSequence& source, std::vector<DutyDetail>& destination)
{
    for (unsigned int i = 0; i < source.length(); i++)
    {
        DutyDetail duty;
        duty.operatorName = "";
        duty.profileName = "";
        std::string sessionId(source[i].session);

        if (!sessionId.empty())
        {
            try
            {        
                std::auto_ptr<TA_Base_Core::ISession> session(SessionAccessFactory::getInstance().getSession(sessionId));
                duty.operatorName = session->getOperatorName();
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error retrieving session from database");
				TA_THROW( TransactiveException("从数据库中读取session信息产生异常") );
            }
        }

        if (source[i].profile != ULONG_MAX)
        {
            try
            {
                std::auto_ptr<TA_Base_Core::IProfile> profile(ProfileAccessFactory::getInstance().getProfile(source[i].profile));
                duty.profileName = profile->getName();
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error retrieving profile from database");
				TA_THROW( TransactiveException("从数据库中读取profile信息产生异常") );
            }
        }

        for (unsigned int j = 0; j < source[i].subsystems.length(); j++)
        {
            try
            {
                SubsystemDetail subsystem;
                std::auto_ptr<TA_Base_Core::IRegion> region(RegionAccessFactory::getInstance().getRegionByKey(source[i].subsystems[j].region));
                subsystem.regionName = region->getDisplayName();
                for (unsigned int k = 0; k < source[i].subsystems[j].subsystems.length(); k++)
                {
                    std::auto_ptr<TA_Base_Core::ISubsystem> subsystemData(SubsystemAccessFactory::getInstance().getSubsystemByKey(source[i].subsystems[j].subsystems[k]));
                    subsystem.subsystemName.push_back(subsystemData->getName());
                }
                duty.subsystem.push_back(subsystem);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error retrieving region/subsystem data from database");
				TA_THROW( TransactiveException("从数据库中读取位置或子系统信息产生异常") );
            }
        }
        destination.push_back(duty);
    }
}


void DutyManager::processDutyRequest(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("processDutyRequest");

    std::string currentId = SessionManager::getInstance().getSessionDetails().getSessionId();
    TA_Base_Bus::DutyAgentTypeCorbaDef::DutyRequest* data;

    // can't extract to the DutyRequest, ignore
	if ( ( message.messageState >>= data ) == 0)
	{
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Received a DutyRequest but couldn't extract DutyRequest.  Ignore");
        FUNCTION_EXIT;
        return;
    }

    // doesn't affects the current session, ignore
    if (currentId.compare(data->targetSession) != 0)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Received a DutyRequest but doesn't affect current session.  Ignore");
        FUNCTION_EXIT;
        return;
    }

    if (data->duty.subsystems.length() == 0)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Received a DutyRequest message but doesn't have subsystem details.  Ignore.");
        FUNCTION_EXIT;
        return;
    }

    // set up the map for the callback
    std::string uniqueId(data->uniqueId);
    {
        ThreadGuard guard(m_callbackLock);
        // see if it is already in the map, if it is, then we've already done the request so just ignore
        if (m_callback.find(uniqueId) != m_callback.end())
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Received a DutyRequest message but we've already received a message with the same unique id.  Ignore.");
            return;
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Received a DutyRequest message");

        // add it to the map
        m_callback.insert(CallbackMap::value_type(uniqueId, TA_Base_Bus::IDutyAgentRespCorbaDef::_duplicate(data->dutyAgent)));
    }

    try
    {
        // process and repackage the data so that the GUI could use it straightaway

        //std::auto_ptr<TA_Base_Core::ISession> session(SessionAccessFactory::getInstance().getSession(std::string(data->duty.session)));
		std::string sessionId(std::string(data->duty.session));
		TA_Base_Bus::SessionInfo  sessionInfo = SessionManager::getInstance().getSessionInfo(sessionId);

        std::auto_ptr<TA_Base_Core::IProfile> profile(ProfileAccessFactory::getInstance().getProfile(data->duty.profile));
        // TD14986 should not use session location instead using console location
        std::auto_ptr<TA_Base_Core::IEntityData> consoleEntity(EntityAccessFactory::getInstance().getEntity(sessionInfo.WorkstationId));
        std::auto_ptr<TA_Base_Core::ILocation> location(LocationAccessFactory::getInstance().getLocationByKey(consoleEntity->getLocation()));
		std::auto_ptr<TA_Base_Core::IOperator> oper(OperatorAccessFactory::getInstance().getOperator(sessionInfo.OperatorId[0],false));
        DutyRequestDetail* det = new struct DutyRequestDetail;
        det->uniqueId = uniqueId;
        det->duty.operatorName = oper->getName();
        det->duty.profileName = profile->getName();
        det->duty.locationName = location->getName();
        for (unsigned int j = 0; j < data->duty.subsystems.length(); j++)
        {
            SubsystemDetail subsystem;
            std::auto_ptr<TA_Base_Core::IRegion> region(RegionAccessFactory::getInstance().getRegionByKey(data->duty.subsystems[j].region));
            subsystem.regionName = region->getDisplayName();
            for (unsigned int k = 0; k < data->duty.subsystems[j].subsystems.length(); k++)
            {
                std::auto_ptr<TA_Base_Core::ISubsystem> subsystemData(SubsystemAccessFactory::getInstance().getSubsystemByKey(data->duty.subsystems[j].subsystems[k]));
                subsystem.subsystemName.push_back(subsystemData->getName());
            }
            det->duty.subsystem.push_back(subsystem);
        }

        ControlStationRootDialog::getInstance().PostMessage(WM_DUTY_REQUEST, reinterpret_cast<WPARAM>(det));
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error accessing database");
    }
    FUNCTION_EXIT;
}

bool DutyManager::loseExclusiveDuty()
{
    TA_Base_Bus::DutyAgentTypeCorbaDef::SessionId_var sessionId(
        CORBA::string_dup(SessionManager::getInstance().getSessionDetails().getSessionId().c_str()));
    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_var exclusiveSubsystems;

	TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = SessionManager::getInstance().getSessionDetails().toCorbaSessionInfo();
	bool hasDuty = false;
    try
    {
		CORBA_CALL_RETURN( hasDuty,
		                   m_dutyAgentQuery,
		                   queryHasExclusiveDuty,
		                   ( corbaSession, exclusiveSubsystems ) );
	}
    catch(...)
    {     
        // If there is an error, then the duty agent is unavailable, and the duties cannot be determined, 
        // so just log out
        LOG_EXCEPTION_CATCH(SourceInfo,"General Exception",
            "An exception was caught while attempting to retrieve duty information from the Duty Agent.");
        return true;
    }

    if (!hasDuty)
    {
        // There are no exclusive subsystem duties held by this session, so can exit immediately
        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Session %s has no exclusive duties.",sessionId);
        return true;
    }
    else
    {
        // The tree contains exclusive subsystems, so need to notify the user.
        std::vector<SubsystemDetail> subsystems;
        for (unsigned int j = 0; j < exclusiveSubsystems->length(); j++)
        {
			// Ignore empty entries
			if( exclusiveSubsystems[j].subsystems.length() == 0 )
			{
				continue;
			}

            try
            {
                SubsystemDetail subsystemDetails;
                std::auto_ptr<TA_Base_Core::IRegion> region(RegionAccessFactory::getInstance().getRegionByKey(exclusiveSubsystems[j].region));
                subsystemDetails.regionName = region->getName();
                for (unsigned int k = 0; k < exclusiveSubsystems[j].subsystems.length(); k++)
                {
                    std::auto_ptr<TA_Base_Core::ISubsystem> subsystem(SubsystemAccessFactory::getInstance().getSubsystemByKey(exclusiveSubsystems[j].subsystems[k]));
                    subsystemDetails.subsystemName.push_back(subsystem->getName());
                }

                subsystems.push_back(subsystemDetails);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error retrieving region/subsystem data from database");
            }
        }

        CString subsystemsAtLocations, locationStr, subsystemStr;
        TA_VERIFY(locationStr.LoadString(IDS_LOCATION)!=0, "Unable to load IDS_LOCATION");
        TA_VERIFY(subsystemStr.LoadString(IDS_SUBSYSTEM)!=0, "Unable to load IDS_SUBSYSTEM");

        for (unsigned int i = 0;i < subsystems.size();i++)
        {
            subsystemsAtLocations += _T("\n")+locationStr + _T(":\t"); 
            subsystemsAtLocations += subsystems[i].regionName.c_str();
            subsystemsAtLocations += _T("\n") + subsystemStr + _T("\t");  
            for (unsigned int j = 0;j < subsystems[i].subsystemName.size();j++)
            {
                subsystemsAtLocations += subsystems[i].subsystemName[j].c_str();
                subsystemsAtLocations += _T("\n\t\t");
            }
            subsystemsAtLocations += _T("\n");
        }

		// TD19409 yg++
		if (SessionManager::getInstance().getSessionDetails().isOperatorOverridden())
		{
			
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"yg0606 isOverride has no exclusive duties.");
			OverRideExclusiveDuty exclusiveDutyDialog(subsystemsAtLocations);
			if(exclusiveDutyDialog.DoModal() == IDCANCEL)
			{
				return false;
			}
			return true;
		}
		else // ++yg
		{
		
			//TD15531++
			ExclusiveDutyDialog exclusiveDutyDialog(subsystemsAtLocations);
			if(exclusiveDutyDialog.DoModal() == IDCANCEL)
			{
				return false;
			}
			return true;
		}	
        /*TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << subsystemsAtLocations;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_020008);

        if ( selectedButton == IDNO )
        {
            return false;
        }
        return true;*/
		//++TD15531
    }
    return false;
}


// TD19075++
inline bool IsSubsystemEmpty(DutyManager::SubsystemDetail sd)
{
	return (0 == sd.subsystemName.size());
}

inline bool IsDutyDetailEmpty(DutyManager::DutyDetail dd)
{
	return (0 == dd.subsystem.size());
}


void DutyManager::getDutyChanged(DutyNotificationDetail *det)
{
	DutyDetailList &gained = det->gained;
	DutyDetailList &lost = det->lost;
	DutyDetailList &changed = det->changed;
	
	for(DutyDetailIter iterGain = gained.begin(); iterGain != gained.end(); ++iterGain)
	{
		for(DutyDetailIter iterLost = lost.begin(); iterLost != lost.end(); ++iterLost)
		{
			DutyDetail details;
			details.locationName = iterGain->locationName;
			details.operatorName = iterGain->operatorName;
			details.profileName = iterGain->profileName;
			//if (iterGain->operatorName == iterLost->operatorName)
			{
				for (SubsystemDetailIter iterGainSub = iterGain->subsystem.begin(); 
				iterGainSub != iterGain->subsystem.end(); ++iterGainSub)
				{
					for (SubsystemDetailIter iterLostSub = iterLost->subsystem.begin(); 
					iterLostSub != iterLost->subsystem.end(); ++iterLostSub)
					{

						// Handle SubsystemDetail(region, subsystems<set>)
						SubsystemDetail sDetail;
						sDetail.regionName = iterLostSub->regionName;
						if (iterGainSub->regionName == iterLostSub->regionName)
						{
							std::vector<std::string>::iterator iterLostSubsysEnd = iterLostSub->subsystemName.end();
							std::vector<std::string>::iterator iterLostSubsysEndOld = iterLostSub->subsystemName.end();
							for (std::vector<std::string>::iterator iterGainSubsys = iterGainSub->subsystemName.begin(); 
							iterGainSubsys != iterGainSub->subsystemName.end(); ++iterGainSubsys)
							{
								iterLostSubsysEnd = std::remove(iterLostSub->subsystemName.begin(), iterLostSubsysEnd, *iterGainSubsys);
								if (iterLostSubsysEnd != iterLostSubsysEndOld)
								{
									LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
										"Duty Change at:%s-->%s", 
										sDetail.regionName.c_str(),
										iterGainSubsys->c_str());
									sDetail.subsystemName.push_back(*iterGainSubsys);
								}
							}
							// do delete from Lost and Gain
							LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
								"Delete all Lost SubSytem");
							iterLostSub->subsystemName.erase(iterLostSubsysEnd, iterLostSub->subsystemName.end());
							// do delete from Gain
							LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
								"Delete all Gain SubSytem");
							for (std::vector<std::string>::iterator iterChangeds = sDetail.subsystemName.begin();
							iterChangeds != sDetail.subsystemName.end(); ++iterChangeds)
							{
// 								LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
// 									"Delete gaind SubSytem %s", iterChangeds->c_str());
								iterGainSub->subsystemName.erase(
									std::remove(iterGainSub->subsystemName.begin(),
									iterGainSub->subsystemName.end(), *iterChangeds), 
									iterGainSub->subsystemName.end());
							}
						}
						if (sDetail.subsystemName.size()> 0)
						{
							LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
								"Add change details subsystem for region:%s   systemcount:%d", 
								sDetail.regionName.c_str(), sDetail.subsystemName.size());
							details.subsystem.push_back(sDetail);
						}
					}
				}
			}// end if
			// delete all empty subsystem
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"Del all empty gained subsystem");
			SubsystemDetailIter gainSubDetailEnd = std::remove_if(
				iterGain->subsystem.begin(), iterGain->subsystem.end(), 
				IsSubsystemEmpty);
			iterGain->subsystem.erase(gainSubDetailEnd, iterGain->subsystem.end());

			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"Del all empty lost subsystem");
			SubsystemDetailIter lostSubDetailEnd = std::remove_if(
				iterLost->subsystem.begin(), iterLost->subsystem.end(), 
				IsSubsystemEmpty);
			iterLost->subsystem.erase(lostSubDetailEnd, iterLost->subsystem.end());
			

			// add change detail
			if (details.subsystem.size() > 0)
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"Add changed for:Loc:%s\tUser:%s\tProFile:%s", 
					details.locationName.c_str(), details.operatorName.c_str(), 
					details.profileName.c_str());
				changed.push_back(details);
			}
		}
	}

	LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
		"Del all empty gained dutydetail");
	DutyDetailIter GainDutyDetail = std::remove_if(gained.begin(), gained.end(), 
		IsDutyDetailEmpty);
	gained.erase(GainDutyDetail, gained.end());

	LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
		"Del all empty lost dutydetail");
	DutyDetailIter LostDutyDetail = std::remove_if(lost.begin(), lost.end(), 
		IsDutyDetailEmpty);
	lost.erase(LostDutyDetail, lost.end());
}
// ++TD19075


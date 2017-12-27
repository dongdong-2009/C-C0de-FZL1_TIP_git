/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/CCTVRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786 4503 4250)
#endif

#include "app/alarm/alarm_agent/src/CCTVRule.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/AlarmAgentAlarm_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/data_access_interface/entity_access/src/Console.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::RunParams;


namespace TA_Base_App
{
	const std::string CCTV_ALARM_SUBMITTER = "CctvAlarmSubmitter";


    //
    // Constructor
    //
    CCTVRule::CCTVRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                     TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key )
    : AbstractRule(entityKey, entityType, alarmTypeKey, trigger, key)
    {
		std::string locationKeyString = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		unsigned long locationKey = atoi(locationKeyString.c_str());
		TA_Base_Core::ILocation* agentLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
		if(agentLocation!=NULL)
		{
			try
			{
				std::string locationName = agentLocation->getName();
				TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationName(locationName);
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "set location name to %s", locationName.c_str() );
			}
			catch( const TA_Base_Bus::VideoSwitchAgentException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught VideoSwitchAgent exception %s", ex._name() );
			}
			catch( const TA_Base_Core::DataException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught unknown data exception ", ex.what());
			}
			catch( const TA_Base_Core::DatabaseException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught unknown database exception ", ex.what());
			}
			catch( ... )
			{
        		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught unknown exception");
			}	
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "failed to set location name from key %d", locationKey );
		}

	}


    //
    // Destructor
    //
    CCTVRule::~CCTVRule()
    {
    }


    //
    // process
    //
    void CCTVRule::process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                           TA_Base_App::AbstractRule::ETriggerType trigger)
    {
		if( doesRuleApply(message, trigger) )
        {
			try
			{
				std::string alarmid = message.alarmID.in();
				if(trigger == ACK)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "[TD15853] Executing acknowledge cctv rule for alarm id %s", alarmid.c_str() );
					TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(std::string(message.alarmAcknowledgedSession));
					unsigned long consoleKey = console->getKey();
					//TD15853++
					CORBA_CALL( TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(consoleKey), acknowledgeAlarm, ( message, consoleKey ) );//limin
					//++TD15853
				}
				else if(trigger == CLOSE)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "[TD15853] Executing close cctv rule for alarm id %s", alarmid.c_str() );
					//TD15853++
					CORBA_CALL( TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(), closeAlarm, ( message ) );//limin
					//++TD15853
				}
			}
			catch( const TA_Base_Bus::VideoSwitchAgentException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught VideoSwitchAgent exception %s", ex._name() );
			}
			catch( const TA_Base_Core::ObjectResolutionException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "failed to resolve agent ", ex.what());
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught unknown corba exception ", ex._name());
			}
			catch( ... )
			{
        		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "caught unknown exception");
			}	
		}
    }
};

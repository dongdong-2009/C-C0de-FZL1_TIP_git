/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/MMSRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/alarm/alarm_agent/src/MMSRule.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/AlarmAgentAlarm_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
//TD16491++
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "bus/maintenance_management/mmsHelper/src/MMSSubmitManager.h"
//++TD16491
using TA_Base_Core::RunParams;


namespace TA_Base_App
{
    //Mintao++ TD15936
	const std::string MMS_ALARM_SUBMITTER = "MmsAlarmSubmitter";
	const std::string MIS_ALARM_SUBMITTER = "MisAlarmSubmitter";
    //Mintao++ TD15936


    //
    // Constructor
    //
    MMSRule::MMSRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                     TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key )
    : AbstractRule(entityKey, entityType, alarmTypeKey, trigger, key)//, m_mmsAgent(NULL)

    {
    
        //Mintao++ TD15936
		//TD16491++
// 		if (TA_Base_Core::RunParams::getInstance().isSet(TA_Base_Core::AlarmRuleAccessFactory::RPARAM_MMS_SUBMITTER_TYPE.c_str()))
// 		//++TD16491
// 		{
//             m_mmsAgentSubmitterType = MIS_ALARM_SUBMITTER;
// 		}
// 		else
// 		{
//             m_mmsAgentSubmitterType = MMS_ALARM_SUBMITTER;
// 		}
        //Mintao++ TD15936
    
    }


    //
    // Destructor
    //
    MMSRule::~MMSRule()
    {
//         if( m_mmsAgent != NULL )
//         {
//             delete m_mmsAgent;
//             m_mmsAgent = 0;
//         }
    }


    //
    // process
    //
    void MMSRule::process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                           TA_Base_App::AbstractRule::ETriggerType trigger)
    {
        try
        {
			if( message.mmsState == TA_Base_Core::MMS_AUTO && doesRuleApply(message, trigger) )
            {
//                 if( m_mmsAgent == NULL )
//                 {
//                     //Mintao++ TD15936
//                     TA_Base_Core::CorbaNameList names = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType( m_mmsAgentSubmitterType, false );
// 
// 					// Only one CorbaName should have been returned. If the EntityAccessFactory returned no names,
// 					// the call to at() will throw an exception.
//                     m_mmsAgent = new MMSNamedObject(names.at(0));
//                 }

				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Submitting mms alarm message for alarm %s",
				//	message.alarmID.in());
				LOG ( SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, " MMSRule::process - received alarm id - %s",message.alarmID.in());
				
				//Push the mms Service Request in AlarmMMSSubmitter's quene
				TA_Base_Core::AlarmDetailCorbaDef* pAlarm = new TA_Base_Core::AlarmDetailCorbaDef();
				*pAlarm = message;
				AlarmMMSSubmitter::getInstance()->push(pAlarm);

                //CORBA_CALL( (*m_mmsAgent), submitMmsAlarmMessage, (message) );
            }
        }
		// TD18552++ 
		// has resolved MMSAgent and connect to it, but fail when connect from MMSAgent to MMSServer
		catch ( const TA_Base_Bus::MmsConnectionFailure& ex ) 
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "fail to connect to MMSServer: %s, errorCode: %d ", std::string(ex.errorMessage).c_str(), ex.errorCode );
			//AlarmCache::getInstance()->updateMMSState(message.alarmID.in(), TA_Base_Core::MMS_AUTO); //DTL-134, remove the event based on requirement
		}

		// could not resolve MMSAgent, set to degrade mode
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException when submitMmsAlarmMessage() ", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			//AlarmCache::getInstance()->updateMMSState(message.alarmID.in(), TA_Base_Core::MMS_FAILED);
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException when submitMmsAlarmMessage(): ", "Could not resolve the mms agent ");
			//AlarmCache::getInstance()->updateMMSState(message.alarmID.in(), TA_Base_Core::MMS_FAILED);
		}
		catch ( ... ) 
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "fail to resolve MMSAgent when submitMmsAlarmMessage(): Unknown reason " );
			//AlarmCache::getInstance()->updateMMSState(message.alarmID.in(), TA_Base_Core::MMS_FAILED);
		}
		// ++TD18552
    }
};

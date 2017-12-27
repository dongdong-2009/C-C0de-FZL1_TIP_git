/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/alarm/alarm_agent/src/AlarmAgentImpl.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/RuleRepository.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/exceptions/src/AlarmAgentException.h"
#include "core/exceptions/src/AlarmCacheCapacityExceededException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/threads/src/Thread.h"
#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"
#include <vector>

#include "core/threads/src/ThreadMemberFunction.h"
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmAgentException;
using TA_Base_Core::AlarmCacheCapacityExceededException;
using TA_Base_Bus::RightsLibraryFactory;
using TA_Base_Bus::RightsLibrary;
using TA_Base_Core::RightsException;

namespace TA_Base_App
{

    //
    // Constructor
    //
    AlarmAgentImpl::AlarmAgentImpl()
    : //m_terminated(false), 
	m_haveReceivedAlarmBatch(false), 
	m_rightsLibrary(0)
    {
        LOG0(SourceInfo,DebugUtil::DebugDebug,"constructor in")

        RightsLibraryFactory rlf;

        m_rightsLibrary = rlf.buildRightsLibrary();

        m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
                            TA_Base_Core::AlarmAudit::Context);
		LOG0(SourceInfo,DebugUtil::DebugDebug,"constructor out")
    }


    //
    // Destructor
    //
    AlarmAgentImpl::~AlarmAgentImpl()
    {
        LOG0(SourceInfo,DebugUtil::DebugDebug,"destructor in")

        AlarmCache::getInstance()->terminate();
		//release AlarmCache
		AlarmCache::releaseInstance();

        if( m_rightsLibrary != NULL )
        {
            delete m_rightsLibrary;
            m_rightsLibrary = 0;
        }

        if( m_auditSender != NULL )
        {
            delete m_auditSender;
            m_auditSender = 0;
        }

        LOG0(SourceInfo,DebugUtil::DebugDebug,"destructor out")
    }


	void AlarmAgentImpl::EnsureInControlState()
	{
		if( 0 != TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{
			
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                     "Alarm Agent is not in control mode");
			throw(TA_Base_Core::OperationModeException(RPARAM_MONITOR));
		}
	}


    //
    // loadAlarmsAndRules
    //
    void AlarmAgentImpl::loadAlarmsAndRules(bool bSkipLoadAlarms)
    {
        FUNCTION_ENTRY("loadAlarmsAndRules");

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        // signal loading threads to start
        AlarmCache::getInstance()->loadAlarms(bSkipLoadAlarms);
        RuleRepository::getInstance()->loadRules();
        
        // now wait for the loading to finish

        while( (!AlarmCache::getInstance()->hasDataLoaded()) || 
			(!RuleRepository::getInstance()->hasDataLoaded()) )
        {
            TA_Base_Core::Thread::sleep(50);
        }

		// Reset Data loader to prepare for next loading when
		// changing state from standby -> monitor or control
		AlarmCache::getInstance()->resetDataLoader();

		//start in new thread in order to avoid alarm agent to become slow
		typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::AlarmCache > CacheThreadedMemberFunction;
		CacheThreadedMemberFunction& thrd = CacheThreadedMemberFunction::alloc(*(AlarmCache::getInstance()) );
        thrd.add( &AlarmCache::loadMMSAlarmFromDatabase );
        thrd.start();
		

        activateServantWithName(RunParams::getInstance().get(RPARAM_ENTITYNAME));
        
        FUNCTION_EXIT;
    }

	void AlarmAgentImpl::unloadAlarmsAndRules()
	{
		FUNCTION_ENTRY("unloadAlarmsAndRules");

		AlarmCache::getInstance()->unloadAlarms();
		RuleRepository::getInstance()->unloadRules();
		
		FUNCTION_EXIT;
	}

    //
    // submitAlarm
    //
    void AlarmAgentImpl::submitAlarm(CORBA::Boolean p_multipleOccurancesAllowed,
                                     const TA_Base_Core::AlarmDetailCorbaDef& p_alarm)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "submitAlarm");
		EnsureInControlState();
        try
        {
            TA_Base_Core::ThreadGuard guard( m_preCacheLock );

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

            if( RuleRepository::getInstance()->isAlarmSuppressed(p_alarm) )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                     "Alarm with ID %s is suppressed", std::string(p_alarm.alarmID).c_str());

				// check if alarm is avalanche child, log only if not
				if (!p_alarm.isChildOfAvalanche)
				{
					// If alarm is suppressed or inhibited, do nothing and not add it to cache or DB
					// But send an audit message to log the suppression
					TA_Base_Core::DescriptionParameters dp;
					CORBA::String_member alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( p_alarm.messageTypeKey,  std::string(p_alarm.alarmParameterList.in())).c_str());
					TA_Base_Core::NameValuePair alarmDesc("alarmDescription", alarmDescription.in() );
					dp.push_back(&alarmDesc);

					timeb sourceTime = {p_alarm.sourceTime.time,p_alarm.sourceTime.millitm,-1,-1};
					std::string session = RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str());
					//CL-21508 begin
// 					m_auditSender->sendAuditMessageWithAsset(
// 						TA_Base_Core::AlarmAudit::AlarmSupressed, p_alarm.assocEntityKey, dp, 
// 						"", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
// 					AlarmAgentQueueItemCorbaDef* queueItem=new AlarmAgentQueueItemCorbaDef();
					m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmSupressed,
                        p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(),
                        alarmDescription.in(), p_alarm.alarmSeverity, p_alarm.alarmValue.in(), 
                        p_alarm.messageTypeKey, p_alarm.alarmComments.in(),AlarmQueueWrapper::getMMSStateDBString(p_alarm), 
                        AlarmQueueWrapper::getPlanStateDBString(p_alarm), 
						AlarmQueueWrapper::getAVLStateDBString(p_alarm), p_alarm.alarmAcknowledgedBy.in(),
						AlarmQueueWrapper::getAlarmStateDBString(p_alarm),AlarmQueueWrapper::getOMFlagString(p_alarm),  0, sourceTime ); 
 					//CL-21508 end
				}
				// TD 9606 Disable this audit message as it will be sent by Alarm Cache instead
				// TD #10752
				// Send the Alarm ID of the head alarm, not the child alarm
                //m_auditSender->sendAuditMessageWithAsset(
                //        TA_Base_Core::AlarmAudit::AlarmSupressed,
                //        p_alarm.assocEntityKey, dp, "", session, RuleRepository::getInstance()->getLastAvalancheHead(), "", "", p_alarm.assetName.in());
            }
            else
            {
				// DTL-Alarm-DuplicatedSubmission - start
				// Check alarm insertion if successful into cache before processing alarm with rules,
				// to avoid alarm re-submission in case rules will cause a timeout.
				TA_Base_Core::AlarmDetailCorbaDef* alarm =new TA_Base_Core::AlarmDetailCorbaDef(p_alarm);
				std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> apAlarm= std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(alarm);
                if (AlarmCache::getInstance()->insert(*alarm, p_multipleOccurancesAllowed))
				{
					RuleRepository::getInstance()->processAlarm(*alarm, TA_Base_App::AbstractRule::SUBMIT);
				}
				// DTL-Alarm-DuplicatedSubmission - end
            }
        }
        catch(const AlarmAgentException& aae)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup(aae.what());
            throw(ae);
        }
        catch(const AlarmCacheCapacityExceededException& cee)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmCapacityExceededException ex;
            ex.reason = CORBA::string_dup(cee.what());
            throw(ex);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            std::string msg = "Unexpected exception raised when attempting to submit alarm";
            ae.reason = CORBA::string_dup(msg.c_str());
            throw(ae);
        }

        AlarmCache::getInstance()->printCacheSize();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "submitAlarm");
    }


    //
    // updateComments
    //
    void AlarmAgentImpl::updateComments(const char* p_description, const char* p_alarmID, const char* p_sessionID)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "updateComments");
		EnsureInControlState();

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

		std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> alarm = 
			std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(AlarmCache::getInstance()->getAlarmById(std::string(p_alarmID)));

        if( alarm.get() == NULL )
        {
            std::string error = "Request to add comment to an alarm failed - the alarm does not exist";
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = error.c_str();
            throw(ae);
        }

        TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                     TA_Base_Bus::AlarmActions::COMMENT_ALARM);
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        TA_Base_Core::IResource* r = 
	        TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( alarm->assocEntityKey );
        unsigned long resourceID = r->getKey();

        delete r;

        bool isAllowed = m_rightsLibrary->isActionPermittedOnResource(std::string(p_sessionID), resourceID,
                                                                      action, reason, decisionModule);
        
        if( !isAllowed )
        {
            std::string error = "Request to add a comment to an alarm was denied by access control for the following reason ";
                        error+= reason;
            TA_Base_Core::AccessDeniedException ade;
            ade.reason = error.c_str();
            throw(ade);
        }

        try
        {
            AlarmCache::getInstance()->updateAlarmComment(p_alarmID, p_description, p_sessionID);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup("Unable to update alarm comments");
            throw(ae);
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "updateComments");
    }


    //
    // acknowledge
    //
    void AlarmAgentImpl::acknowledge(const char* p_alarmID, const char* p_sessionId)
    {
        FUNCTION_ENTRY("acknowledge");
		EnsureInControlState();

		operatorAcknowledge(p_alarmID, p_sessionId, false);

        FUNCTION_EXIT;
    }
    
    //
    // acknowledgeSeq
    //
    void AlarmAgentImpl::acknowledgeSeq(const TA_Base_Core::StringSequence& p_alarms,
                                    const char* p_sessionId)
    {
        FUNCTION_ENTRY("acknowledgeSeq");
		EnsureInControlState();

		acknowledgeAlarmSeq(p_alarms, p_sessionId, false);

        FUNCTION_EXIT;

    }

	//
	// operatorAcknowledgeSeq
	//
	void AlarmAgentImpl::operatorAcknowledgeSeq(const TA_Base_Core::StringSequence& p_alarms,
									const char* p_sessionId)
    {
        FUNCTION_ENTRY("operatorAcknowledgeSeq");
		EnsureInControlState();

		acknowledgeAlarmSeq(p_alarms, p_sessionId, true);

        FUNCTION_EXIT;
    }

	//
	// acknowledgeAlarmSeq
	//
	TA_Base_Core::StringSequence* AlarmAgentImpl::operatorAcknowledgeSeqEx(const TA_Base_Core::StringSequence& p_alarms,
			 const char* p_sessionId)
	{
		FUNCTION_ENTRY("operatorAcknowledgeSeqEx");
		EnsureInControlState();
		bool acknowledgedAlarms = true;

		bool acknowledgedAlarm=true;  //use to record whether alarm has been acknowledged successfully.

		TA_Base_Core::StringSequence* cachedAlarms = new TA_Base_Core::StringSequence();

		std::vector<std::string> alarmIDlist;
		unsigned int i=0;
        for( i = 0; i < p_alarms.length(); i++ )
        {
            std::string alarmID(p_alarms[i]);
			acknowledgedAlarm=true;
            try 
            {			 
			   operatorAcknowledge(alarmID.c_str(), p_sessionId);			 
            }
            catch(const AlarmAgentException& aae)
            {
                
				std::string msg = "Alarm Agent exception caught with following message - ";
				msg+= aae.what();
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmAgentException", msg.c_str());
				
                acknowledgedAlarms = false;
				acknowledgedAlarm=false;
            }
            catch(const TA_Base_Core::AccessDeniedException& ade)
            {
                std::string msg = "Access denied exception caught with following message - ";
				msg+= std::string(ade.reason);
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AccessDeniedException", msg.c_str());
				
                acknowledgedAlarms = false;
				acknowledgedAlarm=false;
            }
			catch(const TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ade)
            {
                std::string msg = "Access denied exception caught with following message - ";
				msg+= std::string(ade.reason);
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmException", msg.c_str());
				
                acknowledgedAlarms = false;
				acknowledgedAlarm=false;
            }
			catch(const TA_Base_Core::AlarmAgentCorbaDef::NoAlarmException& aena)
            {
                std::string msg = "Alarm Agent exception caught with following message - ";
				msg+= std::string(aena.reason);
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "NoAlarmException", msg.c_str());		 
                acknowledgedAlarms = false;
				acknowledgedAlarm=false;				 
				alarmIDlist.push_back(alarmID.c_str());
            }
            catch( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
					"Unknown exception caught when trying to acknowledge alarm.");
				
                acknowledgedAlarms = false;
				acknowledgedAlarm=false;
            }
			 
        }
		
        if( !acknowledgedAlarms )
        {
            if (alarmIDlist.size()==0)
			{
				TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
				ae.reason = CORBA::string_dup("Unable to acknowledge some alarms - see Alarm Agent log for details");
				throw(ae);
			}
        }
        
		//copy into return list
		cachedAlarms->length(alarmIDlist.size());
		std::vector<std::string>::iterator  iter=alarmIDlist.begin();
		i=0;
		for (;iter!=alarmIDlist.end();iter++)
		{
			std::string str=*iter;
			(*cachedAlarms)[i] = CORBA::string_dup(str.c_str());
			i++;
		}


		
        FUNCTION_EXIT;
		return cachedAlarms;

	}
	void AlarmAgentImpl::acknowledgeAlarmSeq(const TA_Base_Core::StringSequence& p_alarms, const char* p_sessionId, bool isOperatorTriggered )
	{
        FUNCTION_ENTRY("acknowledgeAlarmSeq");

        bool acknowledgedAlarms = true;

        for( unsigned int i = 0; i < p_alarms.length(); i++ )
        {
            std::string alarmID(p_alarms[i]);
            try 
            {
				if (isOperatorTriggered)
				{
					operatorAcknowledge(alarmID.c_str(), p_sessionId);
				}
				else
				{
					acknowledge(alarmID.c_str(), p_sessionId);
				}
            }
            catch(const AlarmAgentException& aae)
            {
                std::string msg = "Alarm Agent exception caught with following message - ";
                            msg+= aae.what();
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmAgentException", msg.c_str());

                acknowledgedAlarms = false;
            }
            catch(const TA_Base_Core::AccessDeniedException& ade)
            {
                std::string msg = "Access denied exception caught with following message - ";
                            msg+= std::string(ade.reason);
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AccessDeniedException", msg.c_str());

                acknowledgedAlarms = false;
            }
            catch( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
                     "Unknown exception caught when trying to acknowledge alarm.");

                acknowledgedAlarms = false;
            }
        }

        if( !acknowledgedAlarms )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup("Unable to acknowledge some alarms - see Alarm Agent log for details");
            throw(ae);
        }

        FUNCTION_EXIT;
	}

    //
    // operatorAcknowledge
    //
    void AlarmAgentImpl::operatorAcknowledge(const char* p_alarmID, const char* p_sessionId, bool isOperatorTriggered /*=true*/)
    {
        FUNCTION_ENTRY("operatorAcknowledge");
        EnsureInControlState();
        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

		std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> alarm = 
			std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(AlarmCache::getInstance()->getAlarmById(std::string(p_alarmID)));

        if( alarm.get() == NULL )
        {
            std::string error = "Request to acknowledge alarm failed - the alarm does not exist";
			//this exception will be using by outside to determine whether alarm exist or not.  
            TA_Base_Core::AlarmAgentCorbaDef::NoAlarmException ae;
            ae.reason = error.c_str();
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "ack cannot find alarm id=%s", p_alarmID );
            throw(ae);
        }

        TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                     TA_Base_Bus::AlarmActions::ACK_ALARM);
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        TA_Base_Core::IResource* r = 
	        TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( alarm->assocEntityKey );
        unsigned long resourceID = r->getKey();

        delete r;

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Requesting permission to ack alarm" );

        bool isAllowed = false;

        try
        {
            isAllowed = m_rightsLibrary->isActionPermittedOnResource(std::string(p_sessionId), resourceID,
                                                                      action, reason, decisionModule);
        }
        catch(...)
        {
            isAllowed = false;
            reason = "Call to isActionPermittedOnResource threw an exception";
        }

        if( !isAllowed )
        {
            std::string error = "Request to acknowledge alarm was denied by access control for the following reason ";
                        error+= reason;
            TA_Base_Core::AccessDeniedException ade;
            ade.reason = error.c_str();

            LOG( SourceInfo, DebugUtil::ExceptionConstruction,"AlarmException", error.c_str() );

            throw(ade);
        }

        AlarmCache::getInstance()->updateAlarmAcknowledgeStateNonReentrant(p_alarmID, 
                                               AlarmCache::getInstance()->getOperatorNameFromSession(p_sessionId), 
                                               p_sessionId, isOperatorTriggered);
        FUNCTION_EXIT;
    }

    //
    // getAlarm
    //
	TA_Base_Core::AlarmDetailCorbaDef* AlarmAgentImpl::getAlarm(const char* alarmId)
	{
		EnsureInControlState();
        try
        {
            TA_Base_Core::AlarmDetailCorbaDef* alarm = AlarmCache::getInstance()->getAlarmById(alarmId);
            if( alarm != NULL )
            {
                return alarm;
            }
            else
            {
                TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
                ae.reason = CORBA::string_dup("No alarm with this ID exists.");
                throw(ae);
            }
        }
        catch(const AlarmAgentException& aae)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup(aae.what());
            throw(ae);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            std::string msg = "Failed to return most recent active alarm";
            ae.reason = CORBA::string_dup(msg.c_str());
            throw(ae);
        }
    }

    //
    // closeAlarmById
    //
    void AlarmAgentImpl::closeAlarmById(const char* p_alarmID, const char* p_closeValue, CORBA::ULong p_closeTime)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "closeAlarmById");
		EnsureInControlState();

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        AlarmCache::getInstance()->closeAlarm(p_alarmID, false , "", p_closeValue, p_closeTime);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "closeAlarmById");
    }


    //
    // closeAlarmByTypeAndKey
    //
    void AlarmAgentImpl::closeAlarmByTypeAndKey(CORBA::ULong p_alarmType, CORBA::ULong p_entityKey, const char* p_closeValue, CORBA::ULong p_closeTime)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "closeAlarmByTypeAndKey");
		EnsureInControlState();

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        AlarmCache::getInstance()->closeAlarm(p_entityKey, p_alarmType, false, p_closeValue, p_closeTime);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "closeAlarmByTypeAndKey");
    }


    //
    // operatorCloseAlarm
    //
    void AlarmAgentImpl::operatorCloseAlarm(const char* p_alarmID, const char* p_sessionId)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "operatorCloseAlarm");
		EnsureInControlState();
		try
		{
			operatorCloseAlarmInternal(p_alarmID,p_sessionId);
		}
		catch(const AlarmAgentException& aae)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup(aae.what());
            throw(ae);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            std::string msg = "Failed to return most recent active alarm";
            ae.reason = CORBA::string_dup(msg.c_str());
            throw(ae);
        }
		
			
        LOG ( SourceInfo, DebugUtil::FunctionExit, "operatorCloseAlarm");
	}

    //
    // operatorCloseAlarm
    //
    void AlarmAgentImpl::operatorCloseAlarmInternal(const char* p_alarmID, const char* p_sessionId)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "operatorCloseAlarmInternal");

		EnsureInControlState();

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        //TA_Base_Core::AlarmDetailCorbaDef* alarm = AlarmCache::getInstance()->getAlarmById(std::string(p_alarmID));
		std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> alarm = 
			std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(AlarmCache::getInstance()->getAlarmById(std::string(p_alarmID)));
         
        if( alarm.get() == NULL )
        {
            std::string error = "Request to close alarm failed - the alarm does not exist";
			//this exception will be using by outside to determine whether alarm exist or not.  
            TA_Base_Core::AlarmAgentCorbaDef::NoAlarmException ae;
            ae.reason = error.c_str();
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "close cannot find alarm id=%s", p_alarmID );
            throw(ae);
        }

        TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                     TA_Base_Bus::AlarmActions::CLOSE_ALARM);
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        TA_Base_Core::IResource* r = 
			TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( alarm->assocEntityKey );
        unsigned long resourceID = r->getKey();
		
        delete r;

        bool isAllowed = m_rightsLibrary->isActionPermittedOnResource(std::string(p_sessionId), resourceID,
                                                                      action, reason, decisionModule);
        if( !isAllowed )
        {
            std::string error = "Request to close alarm was denied by access control for the following reason ";
                        error+= reason;
            TA_Base_Core::AccessDeniedException ade;
            ade.reason = error.c_str();
            throw(ade);
        }

        AlarmCache::getInstance()->closeAlarm(p_alarmID, true, p_sessionId, "", 0);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "operatorCloseAlarmInternal");
    }

	//
	// operatorCloseAlarmWithValueAndTime
	//
	void AlarmAgentImpl::operatorCloseAlarmWithValueAndTime(const std::string alarmID, const std::string sessionId, 
			const std::string closeValue, const TA_Base_Core::DateTime closeTime)
	{
		FUNCTION_ENTRY("operatorCloseAlarmWithValueAndTime");

        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        //TA_Base_Core::AlarmDetailCorbaDef* alarm = AlarmCache::getInstance()->getAlarmById(alarmID);
		std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> alarm = 
			std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(AlarmCache::getInstance()->getAlarmById(std::string(alarmID)));


        if( alarm.get() == NULL )
        {
            std::string error = "Request to close alarm failed - the alarm does not exist";
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = error.c_str();
            throw(ae);
        }

        TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                     TA_Base_Bus::AlarmActions::CLOSE_ALARM);
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        TA_Base_Core::IResource* r = 
	        TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( alarm->assocEntityKey );
        unsigned long resourceID = r->getKey();

        delete r;

        bool isAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceID,
                                                                      action, reason, decisionModule);
       
        if( !isAllowed )
        {
            std::string error = "Request to close alarm was denied by access control for the following reason ";
                        error+= reason;
            TA_Base_Core::AccessDeniedException ade;
            ade.reason = error.c_str();
            throw(ade);
        }

        AlarmCache::getInstance()->closeAlarm(alarmID, true, sessionId, closeValue, closeTime);

        FUNCTION_EXIT;	
	}

    //
    // operatorCloseAlarmSequence
    //
    void AlarmAgentImpl::operatorCloseAlarmSequence(const TA_Base_Core::StringSequence& p_alarms,
                                                    const char* p_sessionId )
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "operatorCloseAlarmSequence");
		EnsureInControlState();

        bool allAlarmsClosed = true;

        for( unsigned int i = 0; i < p_alarms.length(); i++ )
        {
            std::string alarmID(p_alarms[i]);
            try 
            {
				operatorCloseAlarmInternal(alarmID.c_str(), p_sessionId);
            }
            catch(const AlarmAgentException& aae)
            {
                std::string msg = "Alarm Agent exception caught with following message - ";
                            msg+= aae.what();
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmAgentException", msg.c_str());
                allAlarmsClosed = false;
            }
            catch(const TA_Base_Core::AccessDeniedException& ade)
            {
                std::string msg = "Access denied exception caught with following message - ";
                            msg+= std::string(ade.reason);
                allAlarmsClosed = false;
            }
            catch( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
                     "Unknown exception caught when trying to close alarm.");
                allAlarmsClosed = false;
            }
        }

        if( !allAlarmsClosed )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup("Unable to close some alarms - see Alarm Agent log for details");
            throw(ae);
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "operatorCloseAlarmSequence");
    }


	TA_Base_Core::StringSequence* AlarmAgentImpl::operatorCloseAlarmSequenceEx( const TA_Base_Core::StringSequence& p_alarms, const char* p_sessionId )
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "operatorCloseAlarmSequenceEx");
		
		EnsureInControlState();
		
        bool allAlarmsClosed = true;

		bool alarmsClosed=true;  //use to record whether alarm has been acknowledged successfully.
		
		
		TA_Base_Core::StringSequence* cachedAlarms = new TA_Base_Core::StringSequence();
		
		std::vector<std::string> alarmIDlist;
		unsigned int i=0;
        for( i = 0; i < p_alarms.length(); i++ )
        {
            std::string alarmID(p_alarms[i]);
			alarmsClosed=true;
            try 
            {
				operatorCloseAlarmInternal(alarmID.c_str(), p_sessionId);
            }
			catch(const TA_Base_Core::AlarmAgentCorbaDef::AlarmException aae)
			{
				std::string msg = "Alarm Agent exception caught with following message - ";
				msg+= aae.reason.in();
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmAgentException", msg.c_str());
                allAlarmsClosed = false;
				alarmsClosed=false;				 	 
			}
			catch(const TA_Base_Core::AlarmAgentCorbaDef::NoAlarmException noAlarmEx)
			{
				alarmIDlist.push_back(alarmID.c_str());			 
			}
            catch(const AlarmAgentException& aae)
            {
                std::string msg = "Alarm Agent exception caught with following message - ";
				msg+= aae.what();
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "AlarmAgentException", msg.c_str());
                allAlarmsClosed = false;
				alarmsClosed=false;
            }
            catch(const TA_Base_Core::AccessDeniedException& ade)
            {
                std::string msg = "Access denied exception caught with following message - ";
				msg+= std::string(ade.reason);
                allAlarmsClosed = false;
				alarmsClosed=false;
            }
            catch( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
					"Unknown exception caught when trying to close alarm.");
                allAlarmsClosed = false;
				alarmsClosed=false;
            }
			 
        }
		
        if( !allAlarmsClosed )
        {            
			if (alarmIDlist.size()==0)
			{
				TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
				ae.reason = CORBA::string_dup("Unable to acknowledge some alarms - see Alarm Agent log for details");
				throw(ae);
			}
// 			TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
//             ae.reason = CORBA::string_dup("Unable to close some alarms - see Alarm Agent log for details");
//             throw(ae);
        }
		//copy into return list
		cachedAlarms->length(alarmIDlist.size());
		std::vector<std::string>::iterator  iter=alarmIDlist.begin();
		i=0;
		for (;iter!=alarmIDlist.end();iter++)
		{
			std::string str=*iter;
			(*cachedAlarms)[i] = CORBA::string_dup(str.c_str());;
			i++;
		}

		LOG ( SourceInfo, DebugUtil::FunctionExit, "operatorCloseAlarmSequence");
		return cachedAlarms;
	}


    //
    // getAlarms
    //
    TA_Base_Core::AlarmSequence* AlarmAgentImpl::getAlarms()
    {
		EnsureInControlState();
        return AlarmCache::getInstance()->getAll();
    }

    /** TD 14082
      * getQueueUpdates
      *
      * This method returns all of the currently items in the alarm agent's queue for updating
      *
      * @return All currently queue item
      */
	TA_Base_Core::AlarmAgentQueueItemSequence* AlarmAgentImpl::getQueueUpdates()
	{
		EnsureInControlState();
		return AlarmCache::getInstance()->getBatchQueueUpdate();
	}

    /** TD 14082
      * isLoading
      *
      * This method provides an indication whether the cache is still loading the data
      *
      * @return bool Cache is still loading data
      */
	bool AlarmAgentImpl::isLoading()
	{
		FUNCTION_ENTRY("isLoading");
				
		if ( AlarmCache::getInstance()->hasDataLoaded()==false )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Alarm Cache is still loading alarms");
		}
		
		FUNCTION_EXIT;
		return !( AlarmCache::getInstance()->hasDataLoaded() );
	}

    //
    // getEntityAlarms
    //
    TA_Base_Core::AlarmSequence* AlarmAgentImpl::getEntityAlarms(unsigned long p_entityKey)
    {
		EnsureInControlState();
        return AlarmCache::getInstance()->getByEntity(p_entityKey);
    }


    //
    // getMostRecentActiveAlarmForEntity
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmAgentImpl::getMostRecentActiveAlarmForEntity( 
                                                         unsigned long p_entityKey )
    {
		EnsureInControlState();
        try
        {
            TA_Base_Core::AlarmDetailCorbaDef* alarm = AlarmCache::getInstance()->getMostRecentByEntity(p_entityKey);
            if( alarm != NULL )
            {
                return alarm;
            }
            else
            {
                TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
                ae.reason = CORBA::string_dup("No alarms exist for the supplied entity");
                throw(ae);
            }
        }
        catch(const AlarmAgentException& aae)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup(aae.what());
            throw(ae);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            std::string msg = "Failed to return most recent active alarm";
            ae.reason = CORBA::string_dup(msg.c_str());
            throw(ae);
        }
    }

	//
	// getActiveAlarmForEntityOfType
	// 
    TA_Base_Core::AlarmDetailCorbaDef* AlarmAgentImpl::getActiveAlarmForEntityOfType(CORBA::ULong  p_alarmTypeKey, unsigned long p_entityKey)
	{
		FUNCTION_ENTRY("getActiveAlarmForEntityOfType");
		TA_Base_Core::AlarmDetailCorbaDef* retAlarm = NULL;
		EnsureInControlState();
        try
        {
            retAlarm = AlarmCache::getInstance()->getActiveAlarmForEntityOfType(p_alarmTypeKey, p_entityKey);

            TA_ASSERT((retAlarm != NULL), "Alarm cannot be NULL");

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "getActiveAlarmForEntityOfType: Requested Alarm obtained from Cache");
		}
        catch(const AlarmAgentException& aae)
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup(aae.what());
            throw(ae);
        }
        catch( ... )
        {
            TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            std::string msg = "Failed to return most recent active alarm";
            ae.reason = CORBA::string_dup(msg.c_str());
            throw(ae);
        }
		FUNCTION_EXIT;
		return retAlarm;
	}

    //
    // setMmsState
    //
    void AlarmAgentImpl::setMmsState( const char* p_alarmId, TA_Base_Core::MmsStateType p_mmsState)
    {
		EnsureInControlState();
        AlarmCache::getInstance()->updateMMSState(p_alarmId, p_mmsState);
    }

    //
    // planAssociatedWithAlarmWasRun
    //
    void AlarmAgentImpl::planAssociatedWithAlarmWasRun(const char* p_alarmId, const char* p_sessionID)
    {
		EnsureInControlState();
        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        AlarmCache::getInstance()->updateDecisionSupportState(p_alarmId, TA_Base_Core::RelatedPlanHasBeenRun, p_sessionID);
    }


    //
    // planAssociatedWithAlarmWasPostponed
    //
    void AlarmAgentImpl::planAssociatedWithAlarmWasPostponed(const char* p_alarmId, const char* p_sessionID)
    {
		EnsureInControlState();
        TA_Base_Core::ThreadGuard guard( m_preCacheLock );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got the pre-cache lock");

        AlarmCache::getInstance()->updateDecisionSupportState(p_alarmId, TA_Base_Core::RelatedPlanHasBeenPostponed, p_sessionID);
    }

};

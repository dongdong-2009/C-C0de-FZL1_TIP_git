/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */

#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/utilities/src/DataConversion.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/message/types/PlanAgentAlarms_MessageTypes.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"

namespace TA_Base_App
{
    AAMessageSenderImpl::AAMessageSenderImpl()
		//TD18093,jianghp:m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
    {
		m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::PlanAgentAudit::Context );
    }

	AAMessageSenderImpl::~AAMessageSenderImpl()
	{
		deleteMessageSenders ();
	}

    void AAMessageSenderImpl::deleteMessageSenders(void)
    {
		if (m_auditMessageSender != NULL)
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    }

    void AAMessageSenderImpl::sendAuditMessage(const std::string& session, const TA_Base_Core::MessageType& auditMessageType, const std::vector<TA_Base_Core::NameValuePair*>& description)
    {
        if ( !m_bEnable )
        {
            return;
        }

        static const unsigned long ulAgentEntityKey = PlanAgentUser::getInstance()->getAgentEntityKey();

		m_auditMessageSender->sendAuditMessage	( auditMessageType, ulAgentEntityKey, description, "", session, "", "", "" );
    }

	std::string AAMessageSenderImpl::raiseAlarm(const TA_Base_Core::MessageType& alarmType, const std::vector<TA_Base_Core::NameValuePair*>& description)
    {
        if ( !m_bEnable )
        {
            return "";
        }

        return TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
                                             alarmType,								//alarm message type
                                             PlanAgentUser::getInstance()->getAgentEntityKey(),		//
                                             PlanAgentUser::getInstance()->getAgentEntityTypeKey(),	//
                                             description,					//
                                             PlanAgentUser::getInstance()->getAgentEntityName(),	//
                                             PlanAgentUser::getInstance()->getAgentLocationKey(),		//regionkey does not exists, check!
                                             PlanAgentUser::getInstance()->getAgentSubsystemKey(),	//
											 PlanAgentUser::getInstance()->getAgentAssetName(),
											 TA_Base_Core::AlarmConstants::defaultTime
                                         );

    }


    void AAMessageSenderImpl::closeAlarm(const std::string& alarmId)
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmId, PlanAgentUser::getInstance()->getAgentLocationKey());
    }

    void AAMessageSenderImpl::enable( const bool bEnable )
    {
        m_bEnable = bEnable;
    }

    void AAMessageSenderImpl::submitAuditOperatorLog( const std::string& strSession, const std::string& strPlanName, const std::string& strMessage, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirMessage( "LogMessage", strMessage );
        vecParameters.push_back( &pirMessage );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanOperatorsLog, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditStepExecutedFailure( const std::string& strSession, const std::string& strPlanName, 
        const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirStepNum( "StepNumber", TA_Base_Core::DataConversion::toString( ulStepNum ));
        vecParameters.push_back( &pirStepNum );

        TA_Base_Core::NameValuePair pirReason( "Reason", strReason );
        vecParameters.push_back( &pirReason );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanStepExecFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditAutoGrcFailed( const std::string& strSession, const std::string& strPlanName, 
        const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirStepNum( "StepNumber", TA_Base_Core::DataConversion::toString( ulStepNum ));
        vecParameters.push_back( &pirStepNum );

        TA_Base_Core::NameValuePair pirReason( "Reason", strReason );
        vecParameters.push_back( &pirReason );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanAutoGRCUnsuccessful, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditManualGrcFailed( const std::string& strSession, const std::string& strPlanName, 
        const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirStepNum( "StepNumber", TA_Base_Core::DataConversion::toString( ulStepNum ));
        vecParameters.push_back( &pirStepNum );

        TA_Base_Core::NameValuePair pirReason( "Reason", strReason );
        vecParameters.push_back( &pirReason );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanManualGRCUnsuccessful, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanCompleted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL");
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanComplete, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditManualGrcSucceed( const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( "", TA_Base_Core::PlanAgentAudit::PlanManualGRCSuccessful, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditAutoGrcSucceed( const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL");
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( "", TA_Base_Core::PlanAgentAudit::PlanAutoGRCSuccessful, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditManualGrcBadLaunch( const std::string& strEquipDesc, const std::string& strLcc )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirEquipDescA( "EquipmentDescription", strEquipDesc);
        vecParameters.push_back ( &pirEquipDescA );
        TA_Base_Core::NameValuePair pirEquipDescB( "EquipmentDescription", strEquipDesc );
        vecParameters.push_back ( &pirEquipDescB );
        TA_Base_Core::NameValuePair pirLcc( "LaunchingConditionString", strLcc );
        vecParameters.push_back ( &pirLcc );

        sendAuditMessage( "", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadLaunch, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditManualGrcBadReturn( const std::string& strEquipDesc, const std::string& strRcc )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirEquipDesc( "EquipmentDescription", strEquipDesc);
        vecParameters.push_back ( &pirEquipDesc );
        TA_Base_Core::NameValuePair pirRcc( "LaunchingConditionString", strRcc );
        vecParameters.push_back ( &pirRcc );

        sendAuditMessage( "", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadReturn, vecParameters );
    }

    std::string AAMessageSenderImpl::raiseAlarmStepExecutedFailure( const std::string& strPlanName, 
        const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum )
    {
        if ( !m_bEnable )
        {
            return "";
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirStepNum( "StepNumber", TA_Base_Core::DataConversion::toString( ulStepNum ));
        vecParameters.push_back( &pirStepNum );

        TA_Base_Core::NameValuePair pirReason( "Reason", strReason );
        vecParameters.push_back( &pirReason );

        return raiseAlarm( TA_Base_Core::PlanAgentAlarms::PlanStepExecutionFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanExecution( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanExecution, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanPending( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( "", TA_Base_Core::PlanAgentAudit::PlanPending, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditAutoGrcStarted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "STARTED" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanAutoGRCStarted, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditManualGrcStarted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "STARTED" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanManualGRCStarted, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanStop( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanStop, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanPause( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanPause, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditStepCustomize( const std::string& strSession, 
        const std::string& strPlanName, const unsigned long ulInstanceNum, const unsigned long ulStepNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirStepNum( "StepNumber", TA_Base_Core::DataConversion::toString( ulStepNum ));
        vecParameters.push_back( &pirStepNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanStepCustomisation, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditControlAcquire( const std::string& strSession, 
        const std::string& strPlanName, const std::string& strOperator, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        TA_Base_Core::NameValuePair pirOperator( "OperatorName", strOperator );
        vecParameters.push_back( &pirOperator );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanControlAcquire, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanPauseFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanPauseFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanResume( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanResume, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanResumeFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanResumeFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanStopFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirInstanceNum( "InstanceNumber", TA_Base_Core::DataConversion::toString( ulInstanceNum ));
        vecParameters.push_back( &pirInstanceNum );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanStopFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanUpdate( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanUpdate, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanCopyFailure( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirNewName( "NewPlanName", strNewName );
        vecParameters.push_back ( &pirNewName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanCopyFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanDelete( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanDelete, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanCopy( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirNewName( "NewPlanName", strNewName );
        vecParameters.push_back ( &pirNewName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanCopy, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanInsert( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanInsert, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanMove( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        TA_Base_Core::NameValuePair pirNewName( "NewPlanName", strNewName );
        vecParameters.push_back ( &pirNewName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanMove, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanDeleteFailure( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirPlanName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirPlanName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanDeleteFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditCategoryUpdate( const std::string& strSession, const std::string& strName, const std::string& strNewName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "CategoryName", strName );
        vecParameters.push_back ( &pirName );

        TA_Base_Core::NameValuePair pirNewName( "NewCategoryName", strNewName );
        vecParameters.push_back ( &pirNewName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanCategoryUpdate, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditCategoryInsert( const std::string& strSession, const std::string& strName, const std::string& strParentName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "CategoryName", strName );
        vecParameters.push_back ( &pirName );

        TA_Base_Core::NameValuePair pirParentName( "ParentCategory", strParentName );
        vecParameters.push_back ( &pirParentName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanCategoryInsert, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditCategoryDelete( const std::string& strSession, const std::string& strName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "CategoryName", strName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanCategoryDelete, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanApprovalFailure( const std::string& strSession, const std::string& strName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanApprovalFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanUnapprovalFailure( const std::string& strSession, const std::string& strName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "UNSUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanUnApprovalFailure, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanApproval( const std::string& strSession, const std::string& strName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanApproval, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditPlanUnapproval( const std::string& strSession, const std::string& strName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanUnapproval, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditEditLocklAcquire( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanEditLockAcquire, vecParameters );
    }

    void AAMessageSenderImpl::submitAuditEditLocklRelease( const std::string& strSession, const std::string& strPlanName )
    {
        if ( !m_bEnable )
        {
            return;
        }

        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue( "DataPointValue", "SUCCESSFUL" );
        vecParameters.push_back ( &pirValue );

        TA_Base_Core::NameValuePair pirName( "PlanName", strPlanName );
        vecParameters.push_back ( &pirName );

        sendAuditMessage( strSession, TA_Base_Core::PlanAgentAudit::PlanEditLockRelease, vecParameters );
    }
}



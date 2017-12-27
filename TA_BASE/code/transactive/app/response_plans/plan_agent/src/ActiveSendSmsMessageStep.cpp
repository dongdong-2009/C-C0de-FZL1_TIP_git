/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bradley Cleaver
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is responsible for sending an SMS message as part of
  * a plans execution.
  *
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

// namespace alias for Corba structures/types

#if defined ( STEPTYPE_SEND_SMS_MESSAGE )

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/SmsAgentEntityData.h"
#include "app/messaging/sms_agent/IDL/src/ISmsAgentCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActiveSendSmsMessageStep.h"

namespace
{
    const std::string IDENTIFIER_RECIPIENT("recipient");
    const std::string IDENTIFIER_GROUP("group");
    const std::string IDENTIFIER_UNLISTED("unlisted");
}

using namespace TA_Base_App;


ActiveSendSmsMessageStep::ActiveSendSmsMessageStep( const TA_Base_Core::StepDetail& stepDetail, 
    const TA_Base_Core::SendSmsMessageParameters& params, IActivePlanFlow& flowControl ) : ActiveStep( stepDetail, flowControl ),
m_params( params )
{
}

TA_Base_Core::EActiveStepState ActiveSendSmsMessageStep::executeImpl( void )
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = RemarkPlanStepExecuted;

        // Create a SMS Agent object reference.
        TA_Base_Core::NamedObject< ISmsAgentCorbaDef, ISmsAgentCorbaDef_ptr, ISmsAgentCorbaDef_var> smsAgent; 
        TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance()
            .getCorbaNameOfEntity( TA_Base_Core::SmsAgentEntityData::getStaticType() );
        smsAgent.setCorbaName( name );

        try
        {
            ISmsAgentCorbaDef::ESmsMessageType smsType;

            // Extract the type and recipient from the recipient identifier.
            std::string recipientIdentifier( m_params.recipientIdentifier );
            std::string type("");
            std::string identifier("");

            unsigned int index = recipientIdentifier.find( ":", 0 );
            if ( std::string::npos != index )
            {
                type = recipientIdentifier.substr( 0, index );
                identifier = recipientIdentifier.substr( index+1 );

                if ( IDENTIFIER_RECIPIENT == type )
                {
                    smsType = ISmsAgentCorbaDef::SmsMessageToRecipient;
                }
                else if ( IDENTIFIER_GROUP == type )
                {
                    smsType = ISmsAgentCorbaDef::SmsMessageToGroup;
                }
                else if ( IDENTIFIER_UNLISTED == type )
                {
                    smsType = ISmsAgentCorbaDef::SmsMessageToUnlisted;
                }
                else
                {
                    identifier = "";
                }
            }

            // Determine the plan information.
            std::stringstream planInfo;
            try
            {
                planInfo << TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan);
            }    
            catch (...)
            {
                planInfo << "ID " << m_flowControl.getPlan().plan;
            }
            planInfo << " " << dumpContext();
            
            // Make sure the recipient identifier was valid.
            if ( identifier.empty() )
            {
                // Log the error.
                std::stringstream msg;
                msg << "Invalid recipient identifier '" << recipientIdentifier << "' provided for plan step '"
                    << planInfo.str() << "'";
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str() );

                // Set a meaningful remark.
                m_remark ="Invalid recipient identifier '" + recipientIdentifier + "'";

                // Fail the step.
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                return stepState;
            }

            // Fill in the SmsMessageInfo structure.
            ISmsAgentCorbaDef::SmsMessageInfo smsInfo;
            smsInfo.type = smsType;
            smsInfo.recipient = CORBA::string_dup( identifier.c_str() );
            smsInfo.message = CORBA::string_dup( m_params.textMessage );
            smsInfo.plan = CORBA::string_dup( planInfo.str().c_str() );

            // Determine the session ID.
            std::string sessionId = ActivePlanBulletinBoard::instance()->getSession( m_flowControl.getPlan() );

			// Send the SMS.
			CORBA_CALL( smsAgent,
			            sendSmsMessage,
			            ( sessionId.c_str(), smsInfo ) );
            // Its all good! Move on to the next step.
            m_flowControl.setNextStep();
            return stepState;
        }
        catch ( TA_Base_Core::DatabaseException& ex )
        {
            // Log the exception.
			std::ostringstream msg;
			msg << dumpContext() << ": " << ex.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", msg.str().c_str() );

            // Set a meaningful remark.
            m_remark = "A TA_Base_Core::DatabaseException was caught while attempting to send the SMS.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( ISmsAgentCorbaDef::InvalidGroupException& )
        {
            // Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ISmsAgentCorbaDef::InvalidGroupException", dumpContext().c_str() );

            // Set a meaningful remark.
            m_remark = "An invalid group was specified.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( ISmsAgentCorbaDef::InvalidRecipientException& )
        {
            // Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ISmsAgentCorbaDef::InvalidRecipientException", dumpContext().c_str() );

            // Set a meaningful remark.
            m_remark = "An invalid recipient was specified.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( ISmsAgentCorbaDef::MessageTooLongException& )
        {
            // Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ISmsAgentCorbaDef::MessageTooLongException", dumpContext().c_str() );

            // Set a meaningful remark.
            m_remark = "The specified message is too long.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( ISmsAgentCorbaDef::EmptyGroupException& )
        {
            // This exception is safe to ignore - there is no need to stop the plan
            // execution if a group is empty.

            m_flowControl.setNextStep();
        }
        catch ( ISmsAgentCorbaDef::CapacityReachedException& )
        {
            // Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ISmsAgentCorbaDef::CapacityReachedException", dumpContext().c_str() );

            // Set a meaningful remark.
            m_remark = "The SMS Agent has reached its capacity.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( TA_Base_Core::ObjectResolutionException& ex )
        {
            // Log the exception.
			std::ostringstream msg;
			msg << dumpContext() << ": " << ex.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );

            // Set a meaningful remark.
            m_remark = "Failed to resolve the SMS Agent.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( CORBA::TRANSIENT& ex )
        {
            // Log the exception.
			std::ostringstream msg;
			msg << dumpContext() << ": " << ex.NP_minorString();
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", msg.str().c_str() );

            // Set a meaningful remark.
            m_remark = "A CORBA::TRANSIENT was caught while attempting to send the SMS.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( CORBA::Exception& ex )
        {
            // Log the exception.
            std::string exMsg = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex );
			std::ostringstream msg;
			msg << dumpContext() << ": " << exMsg;
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", msg.str().c_str() );

            // Set a meaningful remark.
            m_remark = "A CORBA::Exception was caught while attempting to send the SMS.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		catch( const ActiveStepParamException& e )
		{
            // Log the exception.
			std::ostringstream msg;
			msg << dumpContext() << ": " << e.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );

            // Set a meaningful remark.
            m_remark = "A TA_Base_App::ActiveStepParamException was caught while attempting to send the SMS.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
        catch ( ... )
        {
            // Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", dumpContext().c_str() );

            // Set a meaningful remark.
            m_remark = "An unknown exception was encountered while attempting to send the SMS.";

            // Fail the step.
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

		return stepState;
}

#endif //  STEPTYPE_SEND_SMS_MESSAGE

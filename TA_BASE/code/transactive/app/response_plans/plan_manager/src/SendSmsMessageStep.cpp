/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/SendSmsMessageStep.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the information required by the Plan Manager
  * for the Send SMS Message plan step.
  *
  */

#include "stdafx.h"

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include "SendSmsMessageStep.h"
#include "core/utilities/src/TAAssert.h"
#include "InvalidStepParameterException.h"
#include "SmsRecipientStepParameter.h"
#include "TextualStepParameter.h"
#include "resource.h"
#include "PlanStepEditSendSmsMessagePage.h"

namespace
{
    const std::string DEFAULT_SMS_RECIPIENT("");
    const std::string DEFAULT_TEXT_MESSAGE("");
}

using namespace TA_Base_App;

SendSmsMessageStep::SendSmsMessageStep( PlanNode* parent, const TA_Base_Core::StepDetail& stepDetail )
    : PlanStep( parent, stepDetail ),
      m_recipientIdentifierParameter( NULL ),
      m_textMessageParameter( NULL )
{
    FUNCTION_ENTRY( "SendSmsMessageStep" );
    FUNCTION_EXIT;
}


SendSmsMessageStep::SendSmsMessageStep( PlanNode* parent, PlanStepId stepId )
    : PlanStep( parent, stepId, TA_Base_Core::SEND_SMS_MESSAGE_STEP ),
      m_recipientIdentifierParameter( NULL ),
      m_textMessageParameter( NULL )
{
    FUNCTION_ENTRY( "SendSmsMessageStep" );
    FUNCTION_EXIT;
}


SendSmsMessageStep::SendSmsMessageStep( const SendSmsMessageStep& planStep )
    : PlanStep( planStep ),
      m_recipientIdentifierParameter( NULL ),
      m_textMessageParameter( NULL )
{
    FUNCTION_ENTRY( "SendSmsMessageStep" );

    m_recipientIdentifierParameter = new SmsRecipientStepParameter( *planStep.m_recipientIdentifierParameter );
    m_textMessageParameter = new TextualStepParameter( *planStep.m_textMessageParameter );

    addPlanStepParameter( *m_recipientIdentifierParameter );
    addPlanStepParameter( *m_textMessageParameter );

    FUNCTION_EXIT;
}


SendSmsMessageStep::~SendSmsMessageStep()
{
    FUNCTION_ENTRY( "~SendSmsMessageStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* SendSmsMessageStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure the parameters are loaded.
    PlanStepParameterList stepParameters;
    getPlanStepParameters( stepParameters );

    // Create a new SendSmsMessageStep.
    FUNCTION_EXIT;
    return new SendSmsMessageStep( *this );
}


void SendSmsMessageStep::createPlanStepParameters( PlanStepParameterList& stepParameters )
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    m_recipientIdentifierParameter = new SmsRecipientStepParameter( *this, getParameterName(IDS_STEP_PARAMETER_SMS_RECIPIENT), DEFAULT_SMS_RECIPIENT );
    m_textMessageParameter = new TextualStepParameter( *this, getParameterName(IDS_STEP_PARAMETER_SMS_MESSAGE), DEFAULT_TEXT_MESSAGE );

    stepParameters.clear();
    stepParameters.push_back( m_recipientIdentifierParameter );
    stepParameters.push_back( m_textMessageParameter );

    FUNCTION_EXIT;
}


void SendSmsMessageStep::createPlanStepParameters( const TA_Base_Core::StepParameters& parameterSet,
                                                   PlanStepParameterList& stepParameters )
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if ( parameterSet._d() != TA_Base_Core::SEND_SMS_MESSAGE_STEP )
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for send SMS message step.";
        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::SEND_SMS_MESSAGE_STEP));
    }

    m_recipientIdentifierParameter = new SmsRecipientStepParameter( *this, getParameterName(IDS_STEP_PARAMETER_SMS_RECIPIENT), std::string(parameterSet.sendSmsMessage().recipientIdentifier) );
    m_textMessageParameter = new TextualStepParameter( *this, getParameterName(IDS_STEP_PARAMETER_SMS_MESSAGE), std::string(parameterSet.sendSmsMessage().textMessage) );

    stepParameters.clear();
    stepParameters.push_back( m_recipientIdentifierParameter );
    stepParameters.push_back( m_textMessageParameter );

    FUNCTION_EXIT;
}


void SendSmsMessageStep::getUpdatedParameters( TA_Base_Core::Step& step )
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::SendSmsMessageParameters sendSmsMessageParameters;
    sendSmsMessageParameters.recipientIdentifier = CORBA::string_dup( m_recipientIdentifierParameter->getRecipientIdentifier().c_str() );
    sendSmsMessageParameters.textMessage = CORBA::string_dup( m_textMessageParameter->getMessageText().c_str() );
    step.parameters.sendSmsMessage( sendSmsMessageParameters );

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* SendSmsMessageStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditSendSmsMessagePage( this );
}


#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

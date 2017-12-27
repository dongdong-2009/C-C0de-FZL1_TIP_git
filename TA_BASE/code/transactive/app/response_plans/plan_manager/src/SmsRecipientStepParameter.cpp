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
  * This class stores the recipient identifier parameter information.
  *
  */

#include "stdafx.h"

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include "SmsRecipientStepParameter.h"
#include "PlanStepParameterEditDlg.h"

using namespace TA_Base_App;

SmsRecipientStepParameter::SmsRecipientStepParameter( PlanStep& parent, std::string name, std::string recipientIdentifier )
    : PlanStepParameter( parent, name ),
      m_recipientIdentifier( recipientIdentifier )
{
    FUNCTION_ENTRY( "SmsRecipientStepParameter" );
    FUNCTION_EXIT;
}


SmsRecipientStepParameter::SmsRecipientStepParameter( PlanStep& parent, const SmsRecipientStepParameter& parameter )
    : PlanStepParameter( parent, parameter.getName() ),
      m_recipientIdentifier( parameter.getRecipientIdentifier() )
{
    FUNCTION_ENTRY( "SmsRecipientStepParameter" );
    FUNCTION_EXIT;
}


SmsRecipientStepParameter::SmsRecipientStepParameter( const SmsRecipientStepParameter& parameter )
    : PlanStepParameter( *parameter.getParentStep(), parameter.getName() ),
      m_recipientIdentifier( parameter.getRecipientIdentifier() )
{
    FUNCTION_ENTRY( "SmsRecipientStepParameter" );
    FUNCTION_EXIT;
}


SmsRecipientStepParameter::~SmsRecipientStepParameter()
{
    FUNCTION_ENTRY( "~SmsRecipientStepParameter" );
    FUNCTION_EXIT;
}


void SmsRecipientStepParameter::addToParameterList( CPlanStepParameterListCtrl& listCtrl )
{
    FUNCTION_ENTRY( "addToParameterList" );

    std::string type("");
    std::string identifier("");
    std::string finalText("");

    // Format the identifier as required.
    std::string::size_type index = m_recipientIdentifier.find( ":", 0 );
    if ( std::string::npos != index )
    {
        type = m_recipientIdentifier.substr( 0, index );
        identifier = m_recipientIdentifier.substr( index+1 );
        if ( "recipient" == type )
        {
            finalText = "Recipient '"+identifier+"'";
        }
        else if ( "group" == type )
        {
            finalText = "Group '"+identifier+"'";
        }
        else if ( "unlisted" == type )
        {
            finalText = "Unlisted Number '"+identifier+"'";
        }
    }

    // Add the parameter to the control.
    listCtrl.AddParameter( getName().c_str(), finalText.c_str(), this );

    FUNCTION_EXIT;
}


void SmsRecipientStepParameter::setRecipientIdentifier( const std::string& recipientIdentifier )
{
    FUNCTION_ENTRY( "setRecipientIdentifier" );

    m_recipientIdentifier = recipientIdentifier;

    FUNCTION_EXIT;
}


CPlanStepParameterEditDlg* SmsRecipientStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg( *this, canEdit );
}


#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

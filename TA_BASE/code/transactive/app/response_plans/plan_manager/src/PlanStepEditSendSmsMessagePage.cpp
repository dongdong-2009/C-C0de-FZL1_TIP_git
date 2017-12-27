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
  * This class is responsible for allowing the user to specify the
  * configuration required for the Send SMS Message plan step.
  *
  */

#include "stdafx.h"

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include <sstream>
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/SmsAgentEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanStepEditSendSmsMessagePage.h"
#include "SendSmsMessageStep.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const std::string DEFAULT_RECIPIENT("<No Recipient Selected>");
    const std::string DEFAULT_GROUP("<No Group Selected>");

    const std::string IDENTIFIER_RECIPIENT("recipient");
    const std::string IDENTIFIER_GROUP("group");
    const std::string IDENTIFIER_UNLISTED("unlisted");
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepEditSendSmsMessagePage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditSendSmsMessagePage, CPlanStepEditParameterPage)

CPlanStepEditSendSmsMessagePage::CPlanStepEditSendSmsMessagePage( SendSmsMessageStep* step )
    : CPlanStepEditParameterPage( CPlanStepEditSendSmsMessagePage::IDD, step ),
      m_recipientIdentifierParameter( *step->getRecipientIdentifierParameter() ),
      m_textMessageParameter( *step->getTextMessageParameter() ),
      m_numRecipients(0),
      m_numGroups(0),
      m_charsRemaining(0)
{
    FUNCTION_ENTRY( "CPlanStepEditSendSmsMessagePage" );

    // {{AFX_DATA_INIT(CPlanStepEditSendSmsMessagePage)
    m_sRecipientPhone = _T("");
    m_sMessage = _T("");
    // }}AFX_DATA_INIT

    // Set the SMS Agent details.
    TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance()
        .getCorbaNameOfEntity( TA_Base_Core::SmsAgentEntityData::getStaticType() );
    m_smsAgent.setCorbaName( name );

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditSendSmsMessagePage)
    DDX_Control(pDX, IDC_STATICMSGFRAME, m_bCharCount);
    DDX_Control(pDX, IDC_EDIT2, m_cMessage);
    DDX_Control(pDX, IDC_RADIORECIPIENT, m_bRadioRecipient);
    DDX_Control(pDX, IDC_RADIOGROUP, m_bRadioGroup);
    DDX_Control(pDX, IDC_RADIONUMBER, m_bRadioUnlisted);
    DDX_Control(pDX, IDC_EDIT4, m_cRecipientPhone);
    DDX_Control(pDX, IDC_COMBO2, m_cGroupsList);
    DDX_Control(pDX, IDC_COMBO1, m_cRecipientsList);
    DDX_Control(pDX, IDC_STATICLISTNAME, m_sListName);
    DDX_Text(pDX, IDC_EDIT4, m_sRecipientPhone);
    DDX_Text(pDX, IDC_EDIT2, m_sMessage);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditSendSmsMessagePage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditSendSmsMessagePage)
    ON_BN_CLICKED(IDC_RADIORECIPIENT, OnRadioRecipient)
    ON_BN_CLICKED(IDC_RADIOGROUP, OnRadioGroup)
    ON_BN_CLICKED(IDC_RADIONUMBER, OnRadioUnlisted)
    ON_CBN_SELCHANGE(IDC_COMBO1, OnRecipientSelChange)
    ON_CBN_SELCHANGE(IDC_COMBO2, OnGroupSelChange)
    ON_EN_CHANGE(IDC_EDIT4, OnUnlistedChange)
    ON_EN_CHANGE(IDC_EDIT2, OnMessageChange)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditSendSmsMessagePage message handlers

BOOL CPlanStepEditSendSmsMessagePage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    // Populate the recipients combo box.
    GetRecipientsList();

    // Populate the groups combo box.
    GetGroupsList();

    // Find out how many characters a message can have.
    GetMaxMsgLength();

    // Set the initial character count.
    UpdateCharCount();

    // If there are no recipients, disable the option.
    if ( 0 == m_numRecipients )
    {
        m_bRadioRecipient.EnableWindow( false );
    }

    // If there are no groups, disable the option.
    if ( 0 == m_numGroups )
    {
        m_bRadioGroup.EnableWindow( false );
    }

    // Now set the default selection.
    if ( 0 < m_numRecipients )
    {
        CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIORECIPIENT );
        OnRadioRecipient();
    }
    else if ( 0 < m_numGroups )
    {
        CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIOGROUP );
        OnRadioGroup();
    }
    else
    {
        CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIONUMBER );
        OnRadioUnlisted();
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditSendSmsMessagePage::OnRadioRecipient()
{
    FUNCTION_ENTRY( "OnRadioRecipient" );

    // Change the list name.
    m_sListName.SetWindowText("Recipient name:");

    // Shows corresponding list and hides the others
    m_cRecipientsList.ShowWindow(SW_SHOWNORMAL);
    m_cRecipientPhone.ShowWindow(SW_HIDE);
    m_cGroupsList.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnRadioGroup()
{
    FUNCTION_ENTRY( "OnRadioGroup" );

    // Change the list name.
    m_sListName.SetWindowText("Recipient group name:");

    // Shows corresponding list and hides the others
    m_cGroupsList.ShowWindow(SW_SHOWNORMAL);
    m_cRecipientsList.ShowWindow(SW_HIDE);
    m_cRecipientPhone.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnRadioUnlisted()
{
    FUNCTION_ENTRY( "OnRadioUnlisted" );

    // Change the list name.
    m_sListName.SetWindowText("Recipient number:");

    // Shows corresponding list and hides the others
    m_cRecipientPhone.ShowWindow(SW_SHOWNORMAL);
    m_cRecipientsList.ShowWindow(SW_HIDE);
    m_cGroupsList.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnRecipientSelChange()
{
    FUNCTION_ENTRY( "OnRecipientSelChange" );

    // Determine the selection.
    int index = m_cRecipientsList.GetCurSel();

    if ( CB_ERR == index )
    {
//        MessageBox::error( "Failed to identify the selected recipient." );
        m_recipientIdentifierParameter.setRecipientIdentifier( "" );
    }
    else
    {
        // Extract the selected string.
        CString recipient;
        m_cRecipientsList.GetLBText( index, recipient );

        // Make sure its not the default entry.
        if ( recipient == DEFAULT_RECIPIENT.c_str() )
        {
            m_recipientIdentifierParameter.setRecipientIdentifier( "" );
        }
        else
        {
            // Set the new identifier.
            std::string identifier = IDENTIFIER_RECIPIENT + ":" + std::string(recipient);
            m_recipientIdentifierParameter.setRecipientIdentifier( identifier );
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnGroupSelChange()
{
    FUNCTION_ENTRY( "OnGroupSelChange" );

    // Determine the selection.
    int index = m_cGroupsList.GetCurSel();

    if ( CB_ERR == index )
    {
//        MessageBox::error( "Failed to identify the selected group." );
        m_recipientIdentifierParameter.setRecipientIdentifier( "" );
    }
    else
    {
        // Extract the selected string.
        CString group;
        m_cGroupsList.GetLBText( index, group );

        // Make sure its not the default entry.
        if ( group == DEFAULT_GROUP.c_str() )
        {
            m_recipientIdentifierParameter.setRecipientIdentifier( "" );
        }
        else
        {
            // Set the new identifier.
            std::string identifier = IDENTIFIER_GROUP + ":" + std::string(group);
            m_recipientIdentifierParameter.setRecipientIdentifier( identifier );
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnUnlistedChange()
{
    FUNCTION_ENTRY( "OnUnlistedChange" );

    // Saves the previous number text and cursor position
    BOOL isPhoneNumber = TRUE;
    int charIndex = 0;
    int startChar = 0, endChar = 0;
    m_cRecipientPhone.GetSel(startChar, endChar);
    CString tempPhone = m_sRecipientPhone;

    UpdateData();

    // Checks to validate the phone number mask (only 0-9 or '+' or ' ")
    while ( isPhoneNumber && charIndex < m_sRecipientPhone.GetLength())
    {
        if (m_sRecipientPhone[charIndex] != '+' &&    m_sRecipientPhone[charIndex] != ' ' &&
            !( m_sRecipientPhone[charIndex] >= '0' && m_sRecipientPhone[charIndex] <= '9' ))
        {
            isPhoneNumber = FALSE;
        }
        charIndex++;
    }

    // If is not a valid phone number, produces a rollback
    if (!isPhoneNumber)
    {
        m_cRecipientPhone.SetWindowText(tempPhone) ;
        m_cRecipientPhone.SetSel(startChar-1, endChar-1);
    }
    else
    {
        // Update the recipient identifier.
        std::string identifier = IDENTIFIER_UNLISTED + ":" + std::string(m_sRecipientPhone);
        m_recipientIdentifierParameter.setRecipientIdentifier( identifier );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::OnMessageChange()
{
    FUNCTION_ENTRY( "OnMessageChange" );

    // Saves the previous message text and cursor position
    CString tempMsg = m_sMessage;
    int startChar = 0, endChar = 0;
    m_cMessage.GetSel(startChar, endChar);
    UpdateData();

    // If this update will produce a valid message length then allows the changes
    if ( static_cast<int>(m_maxMsgLength) >= m_sMessage.GetLength() )
    {
        // Update the counters.
        m_charsRemaining = m_maxMsgLength - m_sMessage.GetLength();
        UpdateCharCount();

        // Update the text message parameter.
        m_textMessageParameter.setText( std::string(m_sMessage) );
    }

    // If this update will produce an invalid message length then doesn't
    // allow the changes and rollback to previous message string
    else
    {
        m_sMessage = tempMsg ;
        m_cMessage.SetWindowText(tempMsg);
        m_cMessage.SetSel(startChar-1, endChar-1);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


/********************************************************************************/

void CPlanStepEditSendSmsMessagePage::GetRecipientsList()
{
    FUNCTION_ENTRY( "GetRecipientsList" );

    std::string errorMsg("");

    try
    {
        // Get the maximum message length.
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        ISmsAgentCorbaDef::SmsRecipientsSequence_var recipients = m_smsAgent->getRecipientsList( sessionId.c_str() );

        // Iterate through, adding the recipients to our list.
        for ( unsigned int i=0; i<recipients->length(); i++ )
        {
            int index = m_cRecipientsList.AddString( recipients[i] );
            if ( CB_ERR == index )
            {
                // If it failed to add, make sure nothing is left behind.
                m_cRecipientsList.DeleteString( m_cRecipientsList.FindStringExact( 0, recipients[i] ) );
            }
        }

        // Store the number of recipients retrieved.
        m_numRecipients = recipients->length();
    }
    catch ( ISmsAgentCorbaDef::SmsAgentException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ISmsAgentCorbaDef::SmsAgentException", ex.reason );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_RECIPIENT_LIST);
        agent.LoadString(IDS_SMS_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch ( TA_Base_Core::ObjectResolutionException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ex.what());
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_RECIPIENT_LIST);
        agent.LoadString(IDS_SMS_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch ( CORBA::TRANSIENT& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", ex.NP_minorString());
        MessageBox::error(IDS_RETRIEVE_RECIPIENT_LIST);
    }
    catch ( CORBA::Exception& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ));
        MessageBox::error(IDS_RETRIEVE_RECIPIENT_LIST);
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "GetGroupsList: An unknown exception was thrown.");
        MessageBox::error(IDS_RETRIEVE_RECIPIENT_LIST);
    }

    FUNCTION_EXIT;
}


    // Add the default entry.
    int index = m_cRecipientsList.AddString( DEFAULT_RECIPIENT.c_str() );
    if ( CB_ERR == index )
    {
        // If it failed to add, make sure nothing is left behind.
        m_cRecipientsList.DeleteString( m_cRecipientsList.FindStringExact( 0, DEFAULT_RECIPIENT.c_str() ) );
    }
    else
    {
        // Select it.
        m_cRecipientsList.SetCurSel( index );
    }
}

void CPlanStepEditSendSmsMessagePage::GetGroupsList()
{
    FUNCTION_ENTRY( "GetGroupsList" );

    std::string errorMsg("");

    try
    {
        // Get the maximum message length.
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        ISmsAgentCorbaDef::SmsRecipientsSequence_var groups = m_smsAgent->getGroupsList( sessionId.c_str() );

        // Iterate through, adding the recipients to our list.
        for ( unsigned int i=0; i<groups->length(); i++ )
        {
            int index = m_cGroupsList.AddString( groups[i] );
            if ( CB_ERR == index )
            {
                // If it failed to add, make sure nothing is left behind.
                m_cGroupsList.DeleteString( m_cGroupsList.FindStringExact( 0, groups[i] ) );
            }
        }
    }
    catch ( ISmsAgentCorbaDef::SmsAgentException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ISmsAgentCorbaDef::SmsAgentException", ex.reason );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_GROUP_LIST);
        agent.LoadString(IDS_SMS_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch ( TA_Base_Core::ObjectResolutionException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ex.what());
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_GROUP_LIST);
        agent.LoadString(IDS_SMS_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch ( CORBA::TRANSIENT& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", ex.NP_minorString());
        MessageBox::error(IDS_RETRIEVE_GROUP_LIST);
    }
    catch ( CORBA::Exception& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ));
        MessageBox::error(IDS_RETRIEVE_GROUP_LIST);
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "GetGroupsList: An unknown exception was thrown.");
        MessageBox::error(IDS_RETRIEVE_GROUP_LIST);
    }

    // Add the default entry.
    int index = m_cGroupsList.AddString( DEFAULT_GROUP.c_str() );
    if ( CB_ERR == index )
    {
        // If it failed to add, make sure nothing is left behind.
        m_cGroupsList.DeleteString( m_cGroupsList.FindStringExact( 0, DEFAULT_GROUP.c_str() ) );
    }
    else
    {
        // Select it.
        m_cGroupsList.SetCurSel( index );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::GetMaxMsgLength()
{
    FUNCTION_ENTRY( "GetMaxMsgLength" );

    // Set the default value in case an exception occurs.
    m_maxMsgLength = 160;
    m_charsRemaining = m_maxMsgLength;

    std::string errorMsg("");

    try
    {
        // Get the maximum message length.
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        m_maxMsgLength = m_smsAgent->getMaxMessageLength( sessionId.c_str() );
    }
    catch ( ISmsAgentCorbaDef::SmsAgentException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ISmsAgentCorbaDef::SmsAgentException", ex.reason );
    }
    catch ( TA_Base_Core::ObjectResolutionException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ex.what() );
    }
    catch ( CORBA::TRANSIENT& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", ex.NP_minorString());
    }
    catch ( CORBA::Exception& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ));
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "GetMaxMsgLength: An unknown exception was thrown.");
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::UpdateCharCount()
{
    FUNCTION_ENTRY( "UpdateCharCount" );

    std::stringstream newCount;
    newCount << "Message (Remaining character count: ";
    newCount << m_charsRemaining;
    newCount << ")";
    m_bCharCount.SetWindowText( newCount.str().c_str() );

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::EnableParameterControls( bool enable )
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    // Disable all radio buttons if required, otherwise leave them
    // as they are.
    if ( !enable )
    {
        m_bRadioRecipient.EnableWindow( enable );
        m_bRadioGroup.EnableWindow( enable );
        m_bRadioUnlisted.EnableWindow( enable );
    }

    // Set recipient control states.
    m_cRecipientsList.EnableWindow( enable );
    m_cGroupsList.EnableWindow( enable );
    m_cRecipientPhone.EnableWindow( enable );

    // Set message control state.
    m_cMessage.EnableWindow( enable );

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    // Extract the type information from the recipient identifier.
    std::wstring recipientIdentifier( m_recipientIdentifierParameter.getRecipientIdentifier() );
    std::wstring type("");
    std::wstring identifier("");

    std::string::size_type index = recipientIdentifier.find( ":", 0 );
    if ( std::wstring::npos != index )
    {
        type = recipientIdentifier.substr( 0, index );
        identifier = recipientIdentifier.substr( index+1 );

        if ( IDENTIFIER_RECIPIENT == type )
        {
            // Select the recipient.
            int result = m_cRecipientsList.SelectString( 0, identifier.c_str() );
            if ( CB_ERR == result )
            {
                // Display an error message.
                CString itemType;
                itemType.LoadString(IDS_RECIPIENT);
                TA_Bus::TransActiveMessage userMsg;
                userMsg << itemType << identifier;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_210086);
                // Reset the recipient.
                m_recipientIdentifierParameter.setRecipientIdentifier( "" );

                // Don't set default settings - this should have been done in
                // OnInitDialog.
            }
            else
            {
                // Set the correct state.
                CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIORECIPIENT );
                OnRadioRecipient();
            }
        }
        else if ( IDENTIFIER_GROUP == type )
        {
            // Set the group.
            int result = m_cGroupsList.SelectString( 0, identifier.c_str() );
            if ( CB_ERR == result )
            {
                // Display an error message.
                CString itemType;
                itemType.LoadString(IDS_GROUP);
                TA_Bus::TransActiveMessage userMsg;
                userMsg << itemType << identifier;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_210086);

                // Reset the recipient.
                m_recipientIdentifierParameter.setRecipientIdentifier( "" );

                // Don't set default settings - this should have been done in
                // OnInitDialog.
            }
            else
            {
                // Set the correct state.
                CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIOGROUP );
                OnRadioGroup();
            }
        }
        else if ( IDENTIFIER_UNLISTED == type )
        {
            // Set the text.
            m_cRecipientPhone.SetWindowText( identifier.c_str() );

            // Set the correct state.
            CheckRadioButton( IDC_RADIORECIPIENT, IDC_RADIONUMBER, IDC_RADIONUMBER );
            OnRadioUnlisted();
        }
        else
        {
            // Reset the identifier.
            m_recipientIdentifierParameter.setRecipientIdentifier( "" );

            // Don't set default settings - this should have been done in
            // OnInitDialog.
        }
    }
    else
    {
        // Reset the identifier.
        m_recipientIdentifierParameter.setRecipientIdentifier( "" );

        // Don't set default settings - this should have been done in
        // OnInitDialog.
    }

    // Update the character counters.
    m_cMessage.SetWindowText( m_textMessageParameter.getMessageText().c_str() );
    m_charsRemaining = m_maxMsgLength - m_textMessageParameter.getMessageText().length();
    UpdateCharCount();

    FUNCTION_EXIT;
}


void CPlanStepEditSendSmsMessagePage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    SendSmsMessageStep* sendSmsMessageStep = dynamic_cast<SendSmsMessageStep*>( GetPlanStep() );
    TA_ASSERT( sendSmsMessageStep!=NULL, "Incorrect step type for the Send SMS Message page" );

    SmsRecipientStepParameter* recipientIdentifierParameter = sendSmsMessageStep->getRecipientIdentifierParameter();
    TA_ASSERT( recipientIdentifierParameter!=NULL, "Invalid Recipient Identifier parameter for the Send SMS Message step" );
    recipientIdentifierParameter->setRecipientIdentifier( m_recipientIdentifierParameter.getRecipientIdentifier() );

    TextualStepParameter* textMessageParameter = sendSmsMessageStep->getTextMessageParameter();
    TA_ASSERT( textMessageParameter!=NULL, "Invalid Recipient Identifier parameter for the Send SMS Message step" );
    textMessageParameter->setText( m_textMessageParameter.getMessageText() );

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

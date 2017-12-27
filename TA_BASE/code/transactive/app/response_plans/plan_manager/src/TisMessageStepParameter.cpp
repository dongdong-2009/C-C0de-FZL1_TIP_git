/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisMessageStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the Message parameter of a Station/Train Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include "TisMessageStepParameter.h"
#include "TisConfigAccess.h"
#include "PlanStepParameterTisMessageEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisMessageStepParameter

TisMessageStepParameter::TisMessageStepParameter(PlanStep& parent, const std::string& name, EMessageType messageType) :
    PlanStepParameter(parent, name),
    m_message(),
    m_messageType(messageType),
    m_messageText("")
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );

    resetMessage();

    FUNCTION_EXIT;
}


TisMessageStepParameter::TisMessageStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TISMessage& message, EMessageType messageType) :
    PlanStepParameter(parent, name),
    m_message(),
    m_messageType(messageType)
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );

    setMessage(message);

    FUNCTION_EXIT;
}


TisMessageStepParameter::TisMessageStepParameter(PlanStep& parent, const TisMessageStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_message(parameter.getMessage()),
    m_messageType(parameter.m_messageType),
    m_messageText(parameter.getMessageText())
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );
    FUNCTION_EXIT;
}


TisMessageStepParameter::TisMessageStepParameter(const TisMessageStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_message(parameter.getMessage()),
    m_messageType(parameter.m_messageType),
    m_messageText(parameter.getMessageText())
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );
    FUNCTION_EXIT;
}


TisMessageStepParameter::~TisMessageStepParameter()
{
    FUNCTION_ENTRY( "~TisMessageStepParameter" );
    FUNCTION_EXIT;
}


void TisMessageStepParameter::setMessage(const TA_Base_Core::TISMessage& message)
{
    FUNCTION_ENTRY( "setMessage" );

    m_message = message;

    switch (m_messageType)
    {
        case MT_STATION:
            m_messageText = TisConfigAccess::getSTisMessageText(m_message);
            break;

        case MT_TRAIN:
            m_messageText = TisConfigAccess::getTTisMessageText(m_message);
            break;

        default:
            m_messageText = "";
    }

    FUNCTION_EXIT;
}


void TisMessageStepParameter::resetMessage()
{
    FUNCTION_ENTRY( "resetMessage" );

    m_message.libraryVersion = 0;
    m_message.librarySection = 0;
    m_message.messageId = TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID;

    m_messageText = "";

    FUNCTION_EXIT;
}


void TisMessageStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_messageText.c_str(), this);

    FUNCTION_EXIT;
}


bool TisMessageStepParameter::editParameter(unsigned short libraryVersion, TisConfigAccess::PredefinedTisMessage& selectedMessage)
{
    FUNCTION_ENTRY( "editParameter" );

    CPlanStepParameterTisMessageEditDlg editDlg(libraryVersion, *this, true);

    if (editDlg.DoModal() == IDOK)
    {
        selectedMessage = editDlg.GetSelectedMessage();

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


CPlanStepParameterEditDlg* TisMessageStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );

    TA_ASSERT(false, "Function not supported, use editParameter(unsigned short libraryVersion) instead");

    FUNCTION_EXIT;
    return NULL;
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

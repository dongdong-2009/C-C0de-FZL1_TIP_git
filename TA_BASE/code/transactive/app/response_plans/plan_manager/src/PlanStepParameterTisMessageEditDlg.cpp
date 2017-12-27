/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTisMessageEditDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog for selecting a TIS message for the Station/Train TIS steps.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepParameterTisMessageEditDlg.h"
#include "TisMessageStepParameter.h"
#include "Utilities.h"
#include "LabelMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    static const enum
    {
        TYPE_COLUMN_INDEX = 0,
        ID_COLUMN_INDEX,
        MESSAGE_COLUMN_INDEX,
        PRIORITY_COLUMN_INDEX,
    };

    static const int LV_ERR(-1);
    static const int LV_GLOBAL_SEARCH(-1);
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTisMessageEditDlg dialog

CPlanStepParameterTisMessageEditDlg::CPlanStepParameterTisMessageEditDlg(unsigned short libraryVersion, TisMessageStepParameter& parameter, bool canEdit) :
    CPlanStepParameterEditDlg(parameter, CPlanStepParameterTisMessageEditDlg::IDD, canEdit),
    m_messageParameter(parameter),
    m_parameterName(_T(parameter.getName().c_str())),
    m_libraryType(_T(Utilities::convertToString(parameter.getMessageType()).c_str())),
    m_destinationLibraryVersion(libraryVersion),
    m_messageLibraryVersion(_T(""))
{
    FUNCTION_ENTRY( "CPlanStepParameterTisMessageEditDlg" );

    TA_ASSERT(parameter.getMessageType() != TisMessageStepParameter::MT_UNDEFINED, "Invalid message type");

    TA_Base_Core::TISMessage message(parameter.getMessage());

    if (message.messageId == TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
    {
        m_messageLibraryVersion.LoadString(IDS_UNDEFINED_TIS_MESSAGE_LIBRARY);
    }
    else
    {
        m_messageLibraryVersion = TA_Base_Bus::StringUtilities::convertToString(message.libraryVersion);
    }

    FUNCTION_EXIT;
}


CPlanStepParameterTisMessageEditDlg::~CPlanStepParameterTisMessageEditDlg()
{
    FUNCTION_ENTRY( "~CPlanStepParameterTisMessageEditDlg" );
    FUNCTION_EXIT;
}


void CPlanStepParameterTisMessageEditDlg::SetMsgListItemData(int item, const TisConfigAccess::PredefinedTisMessage& message)
{
    FUNCTION_ENTRY( "SetMsgListItemData" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");
    TA_ASSERT(item >= 0 && item < m_messageList.GetItemCount(), "Item index out of range");

    m_messageList.SetItemData(item, reinterpret_cast<DWORD>(&message));

    FUNCTION_EXIT;
}


const TisConfigAccess::PredefinedTisMessage& CPlanStepParameterTisMessageEditDlg::GetMsgListItemData(int item) const
{
    FUNCTION_ENTRY( "GetMsgListItemData" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");
    TA_ASSERT(item >= 0 && item < m_messageList.GetItemCount(), "Item index out of range");

    const TisConfigAccess::PredefinedTisMessage* message =
        reinterpret_cast<const TisConfigAccess::PredefinedTisMessage*>(m_messageList.GetItemData(item));

    FUNCTION_EXIT;
    return *message;
}


void CPlanStepParameterTisMessageEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterTisMessageEditDlg)
    DDX_Control(pDX, IDOK, m_okBtn);
    DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    DDX_Text(pDX, IDC_LIBRARY_TYPE, m_libraryType);
    DDX_Text(pDX, IDC_DEST_LIBRARY_VERSION, m_destinationLibraryVersion);
    DDX_Text(pDX, IDC_MSG_LIBRARY_VERSION, m_messageLibraryVersion);
    DDX_Control(pDX, IDC_MESSAGE_LIST, m_messageList);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTisMessageEditDlg message handlers

BEGIN_MESSAGE_MAP(CPlanStepParameterTisMessageEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterTisMessageEditDlg)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_MESSAGE_LIST, OnMessageListItemChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPlanStepParameterTisMessageEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    CWaitCursor waitCursor;

    // Retrieve from the database the messages matching the library version of the target station/train.
    bool isSTisLibrary(m_messageParameter.getMessageType() == TisMessageStepParameter::MT_STATION);

    m_predefinedMessages = (isSTisLibrary? TisConfigAccess::getSTisMessages(m_destinationLibraryVersion)
                                         : TisConfigAccess::getTTisMessages(m_destinationLibraryVersion));

    if (m_predefinedMessages.empty())
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << m_destinationLibraryVersion;
        userMsg.showMsgBox(isSTisLibrary? IDS_UE_210112 : IDS_UE_210113);

        EndDialog(IDABORT);

        FUNCTION_EXIT;
        return TRUE;
    }

    m_okBtn.EnableWindow(FALSE); // Enabled only when there is a message selected in the list.

    // Initialise message list columns.
    m_messageList.SetExtendedStyle(m_messageList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_messageList.InsertColumn(TYPE_COLUMN_INDEX, _T(LabelMaker::getTisMessageListTypeColumnLabel()), LVCFMT_LEFT, 90);
    m_messageList.InsertColumn(ID_COLUMN_INDEX, _T(LabelMaker::getTisMessageListIdColumnLabel()), LVCFMT_LEFT, 40);
    m_messageList.InsertColumn(MESSAGE_COLUMN_INDEX, _T(LabelMaker::getTisMessageListMessageColumnLabel()), LVCFMT_LEFT, 410);
    m_messageList.InsertColumn(PRIORITY_COLUMN_INDEX, _T(LabelMaker::getTisMessageListPriorityColumnLabel()), LVCFMT_LEFT, 60);

    // Populate the message list.
    TA_Base_Core::TISMessage currentMessage(m_messageParameter.getMessage());
    std::string currentMessageText(m_messageParameter.getMessageText());
    int currentMessageIndex(LV_ERR);

    for (TisConfigAccess::PredefinedTisMessageList::const_iterator mIt(m_predefinedMessages.begin());
         mIt != m_predefinedMessages.end(); mIt++)
    {
        int item(m_messageList.InsertItem(m_messageList.GetItemCount(), _T("")));

        if (item == LV_ERR)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to add a new row to TIS message list");
            continue;
        }

        TA_Base_Core::ELibrarySection librarySection(static_cast<TA_Base_Core::ELibrarySection>(mIt->librarySection));

        m_messageList.SetItemText(item, TYPE_COLUMN_INDEX, _T(Utilities::convertToString(librarySection).c_str()));
        m_messageList.SetItemText(item, ID_COLUMN_INDEX, _T(TA_Base_Bus::StringUtilities::convertToString(mIt->messageTag)));
        m_messageList.SetItemText(item, MESSAGE_COLUMN_INDEX, _T(mIt->message.c_str()));
        m_messageList.SetItemText(item, PRIORITY_COLUMN_INDEX, _T(TA_Base_Bus::StringUtilities::convertToString(mIt->priority)));

        SetMsgListItemData(item, *mIt);

        // See if the given predefined message matches the last configured message for the step.
        // Use library section, message ID and message content as the basis of the comparison.
        if (currentMessageIndex == LV_ERR && currentMessage.messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
        {
            if (currentMessageText == mIt->message)
            {
                currentMessageIndex = item;
            }
        }

    }

    if (currentMessageIndex == LV_ERR)
    {
        if (currentMessage.messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
        {
            // We have an existing message that could not be located based on library section, message ID
            // and message content. The target train/stations possibly has been upgraded to a message library
            // that does not contain a message that matches these three attributes.

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UW_210024);
        }
    }
    else
    {
        // Auto-select the appropriate message. This will also enable the OK button.
        m_messageList.SetItemState(currentMessageIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

        if (currentMessage.libraryVersion != m_destinationLibraryVersion)
        {
            // Message was found based on library section, message ID and message content, however, the
            // original library version does not match what the target train/station is now running.

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(isSTisLibrary? IDS_UW_210025 : IDS_UW_210026);
        }
    }

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterTisMessageEditDlg::OnMessageListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnMessageListItemChanged" );

    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

    // Enable the OK button when there is a single message selected in the list
    if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        m_okBtn.EnableWindow(m_messageList.GetSelectedCount() == 1);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterTisMessageEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    TA_ASSERT(m_messageList.GetSelectedCount() == 1, "Exactly 1 message must be selected in the list");

    int selectedItem(m_messageList.GetNextItem(LV_GLOBAL_SEARCH, LVNI_SELECTED));
    m_selectedMessage = GetMsgListItemData(selectedItem);

    CPlanStepParameterEditDlg::OnOK();

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/CommentDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  * 
  * This class holds the comment dialog that allows the user to view comments (read-only)
  * The user can use this dialog to launch another dialog to edit comments.
  * There is also a button to allow users with appropriate privileges to delete comments.
  */


#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/CommentDialog.h"
#include "bus/alarm/alarm_list_control/src/EditCommentDialog.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "bus\mfc_extensions\src\coding\Encoder.h"

#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::AlarmActionException;
using TA_Base_Bus::Encoder;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_Bus
{
    // Ctor
	CommentDialog::CommentDialog(IAlarmRetriever& alarmRetriever, CWnd* pParent /*=NULL*/)
	    : CDialog(CommentDialog::IDD, pParent),
		  m_alarmRetriever(alarmRetriever)
    {
	    //{{AFX_DATA_INIT(CommentDialog)
	    //}}AFX_DATA_INIT
	    m_comment = "";
    }


	// Dtor
	CommentDialog::~CommentDialog()
	{
		if (m_selectedAlarmDetails.size() > 0) DeleteAllAlarmsInVector();
	}

	void CommentDialog::DeleteAllAlarmsInVector()
	{
		// Cleanup alarm details in the vector
		std::vector<TA_Base_Bus::AlarmDataItem*>::iterator itr = m_selectedAlarmDetails.begin();
		for (itr; itr != m_selectedAlarmDetails.end(); itr++)
		{
			delete (*itr);
			(*itr) = NULL;
		}
		m_selectedAlarmDetails.clear();
	}

    void CommentDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CommentDialog)
	    DDX_Control(pDX, IDC_EDIT_ALARM_COMMENT, m_commentEdit);
	    DDX_Text(pDX, IDC_EDIT_ALARM_COMMENT, m_comment);
	//}}AFX_DATA_MAP
    }


	BEGIN_MESSAGE_MAP(CommentDialog, CDialog)
		//{{AFX_MSG_MAP(CommentDialog)
		ON_BN_CLICKED(IDCANCEL, onCancel)
		ON_BN_CLICKED(ID_BTN_EDIT, onEdit)
		ON_BN_CLICKED(ID_BTN_DELETE, onDelete)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CommentDialog message handlers
    /////////////////////////////////////////////////////////////////////////////

    BOOL CommentDialog::OnInitDialog() 
    {
		FUNCTION_ENTRY("OnInitDialog");
	    CDialog::OnInitDialog();

		RetrieveAlarms();
		EnableButtons();
        UpdateData(FALSE);

		FUNCTION_EXIT;

		return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    /////////////////////////////////////////////////////////////////////////////
	// Handle OK button press
	void CommentDialog::onCancel()
    {
		FUNCTION_ENTRY("onCancel");

        CDialog::OnCancel();
		
		FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Handle Edit button press
	void CommentDialog::onEdit()
    {
		FUNCTION_ENTRY("onEdit");

		if (m_selectedAlarmDetails.size() > 1)
		{
            CString msg;
            CString size;
			size.Format("%d", m_selectedAlarmDetails.size());
            AfxFormatString1(msg, IDS_EDIT_COMMENT_FOR_ALARMS, size);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << msg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);
            
            if(IDNO == selectedButton )
				return; // user canceled action
		}

		// Launch Edit Comment Dialog and update alarm comments if necessary
		EditCommentDialog editDlg;
		if (m_selectedAlarmDetails.size() == 1) // only one alarm is selected
		{
			editDlg.m_comment = this->m_comment;
		}
		else if (m_selectedAlarmDetails.size() > 1) //multiple alarms selected
		{
			editDlg.m_comment.Empty();
			editDlg.m_comment = "";
		}

		if( IDOK == editDlg.DoModal() )
		{
			// Update the alarm in the agent
			CWaitCursor wait;

			try
			{
				this->m_comment = editDlg.m_comment;

				UpdateData(FALSE);

				std::stringstream so;
				so << "Going to update alarm with comment: " << std::string(m_comment);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, so.str().c_str() );

				UpdateAlarmComments(std::string(m_comment));
			}
			catch(const TA_Base_Core::NoAlarmAgentException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "CommentDialogException", "Could not update the alarm's comment");

				//UE-040023
                CString actionName;
                actionName.LoadString(IDS_UPDATE_COMMENT);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_040023);

				FUNCTION_EXIT;				
				OnCancel(); // Close the dialog because we now don't really know the state of the alarms
			}
		}

		FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Handle Delete button press
	void CommentDialog::onDelete()
    {
		FUNCTION_ENTRY("onDelete");

		// No comment means there is no comment to delete
		// No action required
		if (this->m_comment.IsEmpty() || this->m_comment == "") return;

		if(m_selectedAlarmDetails.size() >= 1)
		{
            CString msg;
            CString size;
            size.Format("%d", m_selectedAlarmDetails.size());
            AfxFormatString1(msg, IDS_DELETE_COMMENT_FOR_ALARMS, size);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << msg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);

			if( IDNO == selectedButton )
			{
				FUNCTION_EXIT;
				return; // user canceled action
			}
		}

		// Delete alarm comments
		CWaitCursor wait;
		try
		{
			this->m_comment.Empty();
			this->m_comment = "";
			UpdateAlarmComments(std::string(m_comment));
		}
		catch(const TA_Base_Core::NoAlarmAgentException&)
		{
            CString actionName;
            actionName.LoadString(IDS_UPDATE_COMMENT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_040023);

			FUNCTION_EXIT;
			onCancel(); // close the dialog
		}
 
		FUNCTION_EXIT;
		onCancel(); // close the dialog
    }


    /////////////////////////////////////////////////////////////////////////////
    // CommentDialog message helper functions
    /////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////
	// Retrieve the alarms and get the comment for the first alarm selected
	void CommentDialog::RetrieveAlarms()
	{
		FUNCTION_ENTRY("RetrieveAlarms");

        std::vector<std::string> selectedAlarmStrings = m_alarmRetriever.getSelectedAlarms();

		// One or Multiple alarms selected
		if (selectedAlarmStrings.size() >= 1)
		{
			// empty the selected alarm details before adding new alarm, if it is not empty
			if (m_selectedAlarmDetails.size() > 0)	DeleteAllAlarmsInVector();

			// Retrieve the alarm details for each of the alarms and add them to a vector
			for(std::vector<std::string>::iterator iter = selectedAlarmStrings.begin(); iter != selectedAlarmStrings.end(); ++iter)
			{
				try
				{
					TA_Base_Bus::AlarmDataItem* pAlarm = m_alarmRetriever.getAlarmItem(*iter);
					if (pAlarm != NULL)
					{
						m_selectedAlarmDetails.push_back(pAlarm);
					}
				}
				catch(...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AlarmListCtrlException, Alarm comments could not be retrieved");
				}
			}
			if(m_selectedAlarmDetails.size() != selectedAlarmStrings.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to be retrieve some or all of the alarm details");

				//IDS_UE_040027 //CANNOT_FIND_ALARM
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg.showMsgBox(IDS_UE_040027); 

				//TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ALARM) );
				CDialog::OnCancel();
			}

		}
		// Either the selectedAlarmStrings size is 0 or something wrong with the retriever
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to be retrieve some or all of the alarm details");

			//IDS_UE_040027 //CANNOT_FIND_ALARM
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_040027); 

			//TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ALARM) );
			CDialog::OnCancel();
		}

		FUNCTION_EXIT;
	}

    /////////////////////////////////////////////////////////////////////////////
	// Is the user permitted to modify this alarm?
	bool CommentDialog::IsActionPermitted(const TA_Base_Bus::AlarmDataItem* alarm) const
	{
        // Get the action we should be checking
        unsigned long action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                    TA_Base_Bus::AlarmActions::COMMENT_ALARM);
        // Can we modify this alarm?
		return m_alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, action);
	}

    /////////////////////////////////////////////////////////////////////////////
	// Enable/disable the "edit" and "delete" buttons depending on the user's access
	void CommentDialog::EnableButtons()
	{
		FUNCTION_ENTRY("EnableButtons");

		// for one alarm selection
		if(m_selectedAlarmDetails.size() == 1)
		{
			bool editEnabled = IsActionPermitted(m_selectedAlarmDetails[0]);

			static_cast<CButton*>(GetDlgItem(ID_BTN_EDIT))->EnableWindow(editEnabled);
			static_cast<CButton*>(GetDlgItem(ID_BTN_DELETE))->EnableWindow(editEnabled);
		}
		// for multiple alarm selection
		// all alarms need to be check. throw the user out if any one of the alarm is not accessible
		else if (m_selectedAlarmDetails.size() > 1)// Error - no alarm details for selected alarms
		{
			bool editEnabled = false;

			for (int i=0; i < static_cast<int> (m_selectedAlarmDetails.size()) ; ++i)
			{
				editEnabled = IsActionPermitted(m_selectedAlarmDetails[i]);

				if (editEnabled == false) {
					// Display a message to inform operator of his access limit
					//CString tempString = "You have no access to change comment for alarm: \r\n Alarm ID = ";
					//tempString += m_selectedAlarmDetails[i]->alarmID;
					//tempString += "\r\n Description = ";
					//tempString += m_selectedAlarmDetails[i]->alarmDescription;
					//tempString += "\r\n Edit and Delete is denied.";
					CString tempString = "你没有权限改变此警报的注释: \r\n 警报编号 = ";
					tempString += m_selectedAlarmDetails[i]->alarmID.c_str();
					tempString += "\r\n 描述 = ";
					tempString += m_selectedAlarmDetails[i]->alarmDescription.c_str();
					tempString += "\r\n 禁止编辑或删除.";
					
					// TD14164 azenitha++
					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << tempString;
					CString errMsg = userMsg.constructMessage(IDS_UW_050018);

					/*AfxMessageBox(tempString, MB_OK);*/
					AfxMessageBox(errMsg, MB_OK);
					// TD14164 ++azenitha

					break;
				}
			}

			static_cast<CButton*>(GetDlgItem(ID_BTN_EDIT))->EnableWindow(editEnabled);
			static_cast<CButton*>(GetDlgItem(ID_BTN_DELETE))->EnableWindow(editEnabled);

		}
		// either no alarm selected or some thing is wrong with the retriever
		else
		{
			static_cast<CButton*>(GetDlgItem(ID_BTN_EDIT))->EnableWindow(false);
			static_cast<CButton*>(GetDlgItem(ID_BTN_DELETE))->EnableWindow(false);
		}

		FUNCTION_EXIT;
	}

    /////////////////////////////////////////////////////////////////////////////
	// Update the alarm comments for the selected alarms (obtained using the alarm
	// retriever). Set them to newCommentStr
	void CommentDialog::UpdateAlarmComments(const std::string& newCommentStr)
	{
		FUNCTION_ENTRY("UpdateAlarmComments");

		TA_ASSERT(m_selectedAlarmDetails.size() >= 1, "Attempted to update alarm comments but don't have any alarm details.")

		TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper();
		
		// Update the comments
		bool failureHappened = false;

		std::stringstream ss;
		std::vector<TA_Base_Core::AlarmModifyItem*> alarmModifyItems = m_alarmRetriever.getSelectedModItems();
		ss << "No of selected / retrieved alarm to update:" << alarmModifyItems.size();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str() );

		std::auto_ptr<char> pStr = Encoder::AnsiToUTF8(newCommentStr.c_str());
		std::string strCommentUTF8 = pStr.get();
		failureHappened = TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().updateComments( alarmModifyItems, strCommentUTF8 );

		m_alarmRetriever.freeAlarmModifyItems(alarmModifyItems);
		// Display message to inform user of failure to update comments
		if (failureHappened == true)
		{

			//UE-040023
            CString actionName;
            actionName.LoadString(IDS_UPDATE_COMMENT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_040023);

			LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not update one or more alarms' comment");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_UPDATE_ALARM_COMMENTS) );
		}

		FUNCTION_EXIT;
	}

}

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/CommentDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds the comment dialog that allows the user to view and add/modify comments
  * The user can use this dialog to launch another dialog to edit comments.
  * There is also a button to allow users with appropriate privileges to delete comments.
  */

#if !defined(AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)
#define AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_

#include "bus/alarm/alarm_list_control/src/resource.h"
#include "bus/alarm/alarm_list_control/src/IAlarmRetriever.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class CommentDialog : public CDialog
    {
    // Construction
    public:
	    CommentDialog(IAlarmRetriever& alarmRetriever, CWnd* pParent = NULL);   // standard constructor
		~CommentDialog();
		
    // Dialog Data
	    //{{AFX_DATA(CommentDialog)
		enum { IDD = IDD_VIEW_COMMENT_DLG };
		CString m_comment; // This is the comment shown
		CEdit m_commentEdit; // This is the edit box control
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CommentDialog)

	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CommentDialog)
	    afx_msg void onCancel();
	    afx_msg void onEdit();
	    afx_msg void onDelete();
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
		// Get the alarm details for the selected alarms.
		void RetrieveAlarms();

		// Is the user permitted to modify this alarm?
		bool IsActionPermitted(const TA_Base_Bus::AlarmDataItem* alarm) const;

		// Enable/disable the "edit" and "delete" buttons depending on the user's access
		void EnableButtons();

		// Update the alarm comments for the selected alarms (obtained using the alarm
		// retriever). Set them to newCommentStr
		void UpdateAlarmComments(const std::string& newCommentStr);

		// Delete all alarms that are currently stored to release the memory (obtained using the alarm
		// retriever).
		void DeleteAllAlarmsInVector();

		// The alarm retriever is used to extract the actual alarms from the DB
		IAlarmRetriever& m_alarmRetriever;

        // Vector of corba alarm defs for the selected alarms
		std::vector<TA_Base_Bus::AlarmDataItem*> m_selectedAlarmDetails;

        // This is the maximum length of the comment that can be stored in the database.
        static const long MAXIMUM_COMMENT_LENGTH;

    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // !defined(AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)

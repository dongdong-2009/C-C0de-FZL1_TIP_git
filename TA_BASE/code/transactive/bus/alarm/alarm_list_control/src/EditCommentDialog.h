/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/EditCommentDialog.h $
  * @author:  Ben Robbins
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds the comment dialog that allows the user to edit alarm comments
  */

#if !defined(AFX_EDITCOMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)
#define AFX_EDITCOMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_

#include "bus/alarm/alarm_list_control/src/resource.h"
#include <iostream>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class EditCommentDialog : public CDialog
    {
    // Construction
    public:
	    EditCommentDialog(CWnd* pParent = NULL);   // standard constructor

		/*
		 * This is to set the comment edit box to the param comment
		 *
		 * @param comment string to be shown
		 * @return void
		 *
		 */
		void setEditComment(std::string comment);

		/*
		 * This is to get the comment edit box to the param comment
		 *
		 * @param none
		 * @return string of the edited comment
		 *
		 */
		std::string getEditComment() 
		{
			return std::string(m_comment);
		}

    // Dialog Data
	    //{{AFX_DATA(EditCommentDialog)
		enum { IDD = IDD_EDIT_ALARM_COMMENT_DLG };
	    CString	m_comment;
		CEdit	m_commentControl;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EditCommentDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(EditCommentDialog)
	    afx_msg void onOK();
	    afx_msg void onCancel();
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // This is the maximum length of the comment that can be stored in the database.
        static const long MAXIMUM_COMMENT_LENGTH;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // !defined(AFX_EDITCOMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)

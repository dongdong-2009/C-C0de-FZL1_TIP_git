#if !defined(AFX_SELECTDELETIONDATEDLG_H__9D324B18_6440_483F_A2E5_433FD3A56E2E__INCLUDED_)
#define AFX_SELECTDELETIONDATEDLG_H__9D324B18_6440_483F_A2E5_433FD3A56E2E__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/SelectDeletionDateDlg.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Allows the operator to change the deletion date for a set of restored data.
  * Added for PW #3133.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <utility>
#include <afx.h>
#include <afxdisp.h>

#include "app/data_management/archive_manager/src/resource.h"

namespace TA_Base_App
{
    class SelectDeletionDateDlg : public CDialog
    {
    public:
        /**
          * SelectDeletionDateDlg
          *
          * Standard constructor.
          *
          * @param      CWnd* pParent
          *             The parent window of this dialog.
          */

	    SelectDeletionDateDlg(CWnd* pParent = NULL);

        /**
          * ~SelectDeletionDateDlg
          *
          * Standard destructor.
          */

        ~SelectDeletionDateDlg() {};

        /**
          * getDeletionDueDate
          *
          * @return     time_t
          *             The date that was selected in the dialog.
          *
          * @exception  ArchiveException
          *             Thrown if the date cannot be converted to a time_t.
          */

        time_t getDeletionDueDate();


		/**
          * OnDateChanged
		  *
          */

		void OnDateChanged(NMHDR *,LRESULT *);

    // Dialog Data
	    //{{AFX_DATA(SelectDeletionDateDlg)
	    enum { IDD = IDD_SELECT_DATE_DIALOG };
	    COleDateTime	m_date;   
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SelectDeletionDateDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(SelectDeletionDateDlg)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        SelectDeletionDateDlg( const SelectDeletionDateDlg& );
        SelectDeletionDateDlg& operator=( const SelectDeletionDateDlg& );
    };

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDELETIONDATEDLG_H__9D324B18_6440_483F_A2E5_433FD3A56E2E__INCLUDED_)

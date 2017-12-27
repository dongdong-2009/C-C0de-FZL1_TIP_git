/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/DeleteArchiveDlg.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog to list archives that have been restored and allow the operator
  * to select restorations to be removed from the active data.
  *
  */

#if !defined(AFX_DELETEARCHIVEDLG_H__324F7047_C8FA_444F_B692_49E1770E767D__INCLUDED_)
#define AFX_DELETEARCHIVEDLG_H__324F7047_C8FA_444F_B692_49E1770E767D__INCLUDED_

#include <sstream>

#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/DeleteArchiveHelper.h"
#include "app/data_management/archive_manager/src/StateManager.h"
#include "app/data_management/archive_manager/src/RestoredArchiveListComponent.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // DeleteArchiveDlg dialog
    #define WM_FORCE_CANCEL    (WM_USER + 111)
    class DeleteArchiveDlg : public CDialog, public TA_Base_Core::Thread
    {
		typedef HANDLE (WINAPI*OpenThread)(DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwThreadId);
    public:
        /** 
          * DeleteArchiveDlg
          *
          * Standard constructor.
          *
          * @param      CWnd* pParent
          *             The parent window.
          */
		
        DeleteArchiveDlg(CWnd* pParent = NULL);


        /**
          * ~DeleteArchiveDlg
          *
          * Standard destructor.
          */

        virtual ~DeleteArchiveDlg();
        
        /**
          * run
          * 
          * Inherited from Thread.  This performs the deletion when requested in OnRemoveData().
          * By having a separate thread do this, we can allow user interaction to continue with
          * the GUI.  If Cancel is pressed, the thread will finish removing the data for the
          * current date and then terminate.
          *
          * This class inherits from Thread instead of having a separate class for it, because
          * the deletion process has lots of user/GUI interaction throughout.
          */

        virtual void run();

        /**
          * terminate
          *
          * If the thread is still running, it tells the thread to stop after it has finished
          * removing the current date's data.
          */
        
        virtual void terminate();


        // Dialog Data
        //{{AFX_DATA(DeleteArchiveDlg)
	enum { IDD = IDD_DELETE_ARCHIVE_DIALOG };
	CButton	m_modifyButton;
	    CButton	m_cancelButton;
	    CButton	m_closeButton;
	    CButton	m_removeButton;
        //CListCtrl	m_archiveList;
		RestoredArchiveListComponent m_archiveList;
	//}}AFX_DATA
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DeleteArchiveDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(DeleteArchiveDlg)
        virtual BOOL OnInitDialog();
	    afx_msg void OnButtonRemoveData();
	    virtual void OnCancel();
	    afx_msg void OnItemChangedArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnModifyButton();
		
	//}}AFX_MSG
		afx_msg LRESULT OnForceCancel(WPARAM, LPARAM);
        DECLARE_MESSAGE_MAP()

    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        DeleteArchiveDlg( const DeleteArchiveDlg& );
        DeleteArchiveDlg& operator=( const DeleteArchiveDlg& );

        /**
          * populateList
          *
          * Populates the archive list from the AR_INVENTORY database table.
          */
        
        void populateList();

        /**
          * enableCloseButton
          *
          * Disables and hides the RemoveData and Cancel buttons and 
          * enables and shows the Close button.
          */

        void enableCloseButton();
        

        /**
          * setEnabledProperty
          *
          * Enables/disables the Change Date, Remove Data, Cancel buttons
          * and the Archive list.
          *
          * @param      bool isEnabled
          *             True => Enable the above items.
          *             False => Disable the above items.
          */

        void setEnabledProperty( bool isEnabled );

        /** 
          * compareItems
          *
          * Callback used by list control to sort items in the file list.
          *
          * @return int     Zero if items are sort-key identical
          *                 < 0  if item 1 is to be sequenced before item 2
          *                 > 0  if item 1 is after item 2
          *
          * @param      LPARAM lParam1       
          *             Archive Inventory pkey from first item for comparison.
          * @param      LPARAM lParam2       
          *             Archive Inventory pkey from second item for comparison.
          * @param      LPARAM lParamSort    
          *             Map of archives in the list.
          */
        static int CALLBACK compareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

        //
        // Will be populated with the map of archives from the inventory.
        //
        
        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap m_archives;

        //
        // The object that does all the back end work.
        //

        DeleteArchiveHelper m_deleteArchiveHelper;

        //
        // Manages the state of the GUI.
        //
        
        StateManager m_stateManager;
        
        //
        // The value (and the associated message) which records whether:
        // 1. The user has cancelled, or 
        // 2. If the process failed, or 
        // 3. If the process was successful.
        //

        int m_status;
        std::stringstream m_statusMessage;
		bool m_deletedCompleted;
    };
    
} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEARCHIVEDLG_H__324F7047_C8FA_444F_B692_49E1770E767D__INCLUDED_)

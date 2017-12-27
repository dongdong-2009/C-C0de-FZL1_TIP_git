/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Alan Brims
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Dialog for operator to select the archive files to be restored.
  *
  */

#if !defined(AFX_RESTOREARCHIVEDLG_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_)
#define AFX_RESTOREARCHIVEDLG_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_

#include <map>
#include <string>

#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/RestoreArchiveHelper.h"
#include "app/data_management/archive_manager/src/StateManager.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "app/data_management/archive_manager/src/RestoredArchiveListComponent.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declaration

namespace TA_Base_Bus
{
    class IGUIAccess;
}
    
namespace TA_Base_App
{
    class RestoreArchiveDlg : public CDialog, public TA_Base_Core::Thread
    {
    public:
        /**
          * RestoreArchiveDlg
          *
          * Standard constructor.
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             Used to access the ArchiveManager entity data.
          * @param      CWnd* pParent
          *             The parent window.
          */

        RestoreArchiveDlg(TA_Base_Bus::IGUIAccess& guiAccessor, CWnd* pParent = NULL);

        /**
          * ~RestoreArchiveDlg
          *
          * Standard destructor.
          */

        virtual ~RestoreArchiveDlg();
        
        /**
          * run
          * 
          * Inherited from Thread.  This performs the restoration when requested in OnRestore().
          * By having a separate thread do this, we can allow user interaction to continue with
          * the GUI.  If Cancel is pressed, the thread will finish restoring the data for the
          * current date and then terminate.
          *
          * This class inherits from Thread instead of having a separate class for it, because
          * the restoration process has lots of user/GUI interaction throughout.
          */

        virtual void run();

        /**
          * terminate
          *
          * If the thread is still running, it tells the thread to stop after it has finished
          * importing the current date's data.
          */
        
        virtual void terminate();


        // Dialog Data
        //{{AFX_DATA(RestoreArchiveDlg)
	enum { IDD = IDD_RESTORE_ARCHIVE_DIALOG };
	    CComboBox	m_selectedDriveCombo;
	    CButton	m_closeButton;
	    CButton	m_restoreButton;
	    CButton	m_cancelButton;
	    CButton	m_selectAllButton;
        //CListCtrl	m_archiveList;
		RestoredArchiveListComponent m_archiveList;
		CFont m_listFont;
	//}}AFX_DATA
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(RestoreArchiveDlg)
	protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(RestoreArchiveDlg)
        virtual BOOL OnInitDialog();
	    afx_msg void OnButtonSelectAll();
	    afx_msg void OnRestore();
	    virtual void OnCancel();
	    afx_msg void OnItemChangedArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnSelchangeDriveCombo();
	//}}AFX_MSG
	    afx_msg LRESULT OnShowCompleteDlg(WPARAM, LPARAM);
		afx_msg LRESULT onTerminateThreadCall(WPARAM, LPARAM);
        DECLARE_MESSAGE_MAP()

    private:
        //
        // Map of the archive date to the file name (including the full path).
        //

        typedef std::multimap< std::string, std::string > DateToFileMap;

        //
        // Disable copy constructor and assignment operator.
        //

		RestoreArchiveDlg( const RestoreArchiveDlg& );
		RestoreArchiveDlg& operator=( const RestoreArchiveDlg& );
	    
        /**
          * populateList
          *
          * Populates the archive list from the CD.
          */

        void populateList();

        /**
          * disableAllButCancelButton
          *
          * Disables everything except the Cancel button and
          * sets the focus to the Cancel button.
          */

        void disableAllButCancelButton();
    
        /**
          * resetButtons
          *
          * Resets the buttons to their original configuration.
          */

        void resetButtons();

        /**
          * enableCloseButton
          *
          * Hides all the other buttons and shows and enables
          * the Close button.
          */

        void enableCloseButton();

        /**
          * getTickedItems
          *
          * Gets the ticked items from the list.  These will determine
          * which dates are restored and the volume label on the CD
          * that will be used in the AR_INVENTORY table.
          *
          * @return     bool
          *             True if the process runs to completion.
          *             False if the user cancels midway through.
          *
          * @param      RestoreArchiveHelper::DatesVector& restoredDates
          *             Will be populated with the dates to restore in chronological order.
          * @param      std::string& volumeLabel
          *             Will be populated with the volume label to use in the AR_INVENTORY table.
          */
           
        bool getTickedItems( RestoreArchiveHelper::DatesVector& restoredDates, std::string& volumeLabel );

        /**
          * restoreFiles
          *
          * Restores the files listed in the vector.  This includes adding an entry
          * to the AR_INVENTORY table.  The archives are restored in date order.
          * If the user cancels during the operation, the rest of the files
          * for the date that is being restored will be restored and then this method
          * will return.  This prevents data corruption.
          *
          * @return     bool
          *             True if the process runs to completion.
          *             False if the user cancels midway through.
          *
          * @param      const RestoreArchiveHelper::DatesVector& restoredDates
          *             The dates to restore in chronological order.
          * @param      const std::string& volumeLabel
          *             The volume label to use in the AR_INVENTORY table.
          */

        bool restoreFiles( const RestoreArchiveHelper::DatesVector& restoredDates, const std::string& volumeLabel );

        /**
          * processLogFiles
          *
          * If the log files from the restoration process indicate that
          * the process was successful, this method deletes the log files.
          * Otherwise it displays a Restore Complete dialog listing all
          * the files that contain problems.  The user is then
          * responsible for deleting the log files.
          *
          * Any attempts to Cancel by the user during this method will be
          * ignored (or at least a message box will be displayed, and then
          * the method will continue).
          */

        void processLogFiles();

        //
        // The object that does all the back end work.
        //

        RestoreArchiveHelper m_restoreArchiveHelper;
        
        //
        // Will be populated with the archive files and their status'.
        //

        RestoreArchiveHelper::FileDetailsVector m_fileDetails;

        //
        // Will be populated with the status and log file for each
        // set of data that is restored.
        //

        TA_ArchiveManager::FileStatusMap m_fileStatusMap;

        //
        // Used to disable the list ctrl updating until after the list has been populated.
        //

        bool m_isListPopulated;

		bool m_isRestoreInProgress;
        
        //
        // Manages the state of the GUI.
        //
        
        StateManager m_stateManager;

        //
        // The value to return which records whether the user has
        // cancelled or if the process failed or if the process was successful.
        //

        int m_returnValue;
    };
    
} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESTOREARCHIVEDLG_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_)

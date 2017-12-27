#if !defined(AFX_RETRIEVINGDATADLG_H__5D02C1E1_7FC9_445D_B12F_70E2F6D1CF90__INCLUDED_)
#define AFX_RETRIEVINGDATADLG_H__5D02C1E1_7FC9_445D_B12F_70E2F6D1CF90__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RetrievingDataDlg.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Retrieves the data from the database for a manual archive.
  */

#include <string>
#include <afx.h>

#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/WriteArchiveHelper.h"
#include "app/data_management/archive_manager/src/StateManager.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"

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
    class RetrievingDataDlg : public CDialog, public TA_Base_Core::Thread
    {
    public:
        /**
          * RetrievingDataDlg
          *
          * Standard constructor.
          *
          * Preconditions:  fromDate <= toDate
          *                 toDate < todaysDate
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             Used to access the ArchiveManager entity data.
          * @param      const time_t& fromDate
          *             The first date in the range to archive.
          * @param      const time_t& toDate
          *             The last date in the range to archive.
          * @param      CWnd* pParent
          *             The parent window.
          */

        RetrievingDataDlg(TA_Base_Bus::IGUIAccess& guiAccessor, const time_t& fromDate, const time_t& toDate, CWnd* pParent = NULL);

        /**
          * ~RetrievingDataDlg
          *
          * Standard destructor.
          */

        virtual ~RetrievingDataDlg();


        /**
          * getToDate
          *
          * Gets the last date for which data was retrieved before
          * the retrieval process completed or was stopped.
          *
          * @return     time_t
          *             The last date for which data was retrieved.
          */

        time_t getToDate() { return m_toDate; };

        /**
          * run
          * 
          * Inherited from Thread.  This performs the retrieval of the data from the database.
          * By having a separate thread do this, we can allow user interaction to continue with
          * the GUI.  If Stop is pressed, the thread will finish retrieving the data for the
          * current date and then terminate.
          */

        virtual void run();

        /**
          * terminate
          *
          * If the thread is still running, it tells the thread to stop after it has finished
          * retrieving the current date's data.
          */
    
        virtual void terminate();

        // Dialog Data
	    //{{AFX_DATA(RetrievingDataDlg)
	    enum { IDD = IDD_RETRIEVING_DATA_DIALOG };
	    CStatic	m_date;
	    CButton	m_stopButton;
	    CString	m_range;
	    //}}AFX_DATA


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RetrievingDataDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

        // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(RetrievingDataDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnStop();
        //}}AFX_MSG
        afx_msg void OnClose(); 
	    afx_msg LRESULT OnCloseDlg(WPARAM, LPARAM);
        afx_msg LRESULT OnDisableDlg(WPARAM, LPARAM lparam);
	    afx_msg LRESULT OnShowCompleteDlg(WPARAM, LPARAM);
        DECLARE_MESSAGE_MAP()

    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        RetrievingDataDlg( const RetrievingDataDlg& );
        RetrievingDataDlg& operator=( const RetrievingDataDlg& );

        /**
          * processLogFiles
          *
          * If the log files from the retrieval process indicate that
          * the process was successful, this method deletes the log files.
          * Otherwise it displays a Retrieve Complete dialog listing all
          * the files that contain problems.  The user is then
          * responsible for deleting the log files.
          *
          * Any attempts to Cancel by the user during this method will be
          * ignored (or at least a message box will be displayed, and then
          * the method will continue).
          */

        void processLogFiles();

        //
        // The object used to access the ArchiveManager entity data.
        //

        TA_Base_Bus::IGUIAccess& m_guiAccessor;

        //
        // The object that does all the back end work.
        //

        WriteArchiveHelper m_writeArchiveHelper;
        
        //
        // Will be populated with the status and log file for each
        // set of data that is retrieved.
        //

        TA_ArchiveManager::FileStatusMap m_fileStatusMap;

        //
        // The start and end dates of the data to retrieve.
        //

        time_t m_fromDate;
        time_t m_toDate;
        
        //
        // Manages the state of the GUI.
        //
        
        StateManager m_stateManager;

        //
        // The list of archive files (including the full path) that are generated.  
        // These are stored so that if the user cancels, we can delete the files.
        //

        WriteArchiveHelper::FilePathsVector m_archiveFiles;
    };

}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RETRIEVINGDATADLG_H__5D02C1E1_7FC9_445D_B12F_70E2F6D1CF90__INCLUDED_)

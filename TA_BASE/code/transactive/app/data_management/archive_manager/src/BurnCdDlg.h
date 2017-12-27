#if !defined(AFX_BURNCDDLG_H__1468A462_430F_4418_BC92_334001E5F340__INCLUDED_)
#define AFX_BURNCDDLG_H__1468A462_430F_4418_BC92_334001E5F340__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/BurnCdDlg.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog box to handle the CD Burn process.
  * This dialog is specific to CD-R and should be replaced with
  * a similar dialog if an alternative media is required.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "app/data_management/archive_manager/src/WriteArchiveHelper.h"
#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/StateManager.h"
#include "app/data_management/archive_manager/src/IMediaAccess.h"
#include "app/data_management/archive_manager/src/ICallbackBurnStatus.h"
#include "core/threads/src/Thread.h"


// Forward declaration

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
	class BurnCdDlg : public CDialog, public TA_Base_Core::Thread, public ICallbackBurnStatus
    {
    public:
        /**
          * BurnCdDlg
          *
          * Standard constructor.
          *
          * @param      const std::string& volumeLabel
          *             The label to apply to the CD.  The user will also
          *             be told to write this label on the CD once the write is complete.
          * @param      const std::string& cdLabel
          *             The label to apply to the CD electronically.
          *             KT 10 Mar 04: PW #3136 Added reformatted volume label for CD (cdLabel).
          * @param      const WriteArchiveHelper::FileDetailsVector& files
          *             The names (incl. full paths) of the files to write to CD.
          *             These are in the order in which they will be written to the CD.
          * @param      CWnd* pParent
          *             The parent of this dialog.
          */

        BurnCdDlg( const std::string& volumeLabel, const std::string& cdLabel, 
            const WriteArchiveHelper::FileDetailsVector& files, CWnd* pParent = NULL);
           
        /**
          * ~BurnCdDlg
          *
          * Standard destructor that calls terminateAndWait().
          */

        virtual ~BurnCdDlg();

        /**
          * run
          * 
          * Inherited from Thread.  This calls the initialisation and write methods.
          * By having a separate thread do this, we can allow user interaction to continue with
          * the GUI.  If Cancel is pressed, the thread will wait until it is safe to cancel,
          *  then the process will be cancelled.
          *
          * This class inherits from Thread instead of having a separate class for it, because
          * the write process has lots of GUI interaction throughout.
          */

        virtual void run();

        /**
          * terminate
          *
          * If the thread is still running, it tells the thread to stop when Media access API decides
          * it is safe to do so.
          */
        
        virtual void terminate();
		virtual void BurnStatusCallBack (long totalTime, long timeLeft, BurnStatus burnStatus) ;


	    
        // Dialog Data
        //{{AFX_DATA(BurnCdDlg)
	    enum { IDD = IDD_BURN_CD_DIALOG };
	    CButton	m_buttonCancel;
	    CStatic	m_stageStatic;
	    CEdit	m_editLog;
        CProgressCtrl	m_writeProgress;
        CString	m_csStage;
        CString	m_csLog;
	    //}}AFX_DATA
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(BurnCdDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(BurnCdDlg)
        virtual BOOL OnInitDialog();
	    afx_msg void OnDestroy();
	    virtual void OnCancel();
	    //}}AFX_MSG
	    afx_msg LRESULT OnCloseDlg(WPARAM, LPARAM);
        DECLARE_MESSAGE_MAP()

    private:

        //
        // Disable the copy constructor and assignment operator.
        //

        BurnCdDlg( const BurnCdDlg& );
        BurnCdDlg& operator=( const BurnCdDlg& );


        void addFile( const std::string& fileNameWithPath, const std::string& fileName, bool isDirectory = false );

        /**
          * appendStringToLogDisplay
          *
          * Adds a string to the log displayed on the dialog.
          *
          * @param      const std::string& sAdd
          *             The string to add.
          */

        void appendStringToLogDisplay( const std::string& sAdd );

        //
        // The files to write to CD in write order as passed to
        // this dialog
        //

        const WriteArchiveHelper::FileDetailsVector& m_files;
        

        //
        // The label to write on the CD manually (m_volumeLabel) and 
        // electronically (m_cdLabel) containing the archive date and part information.
        // KT 10 Mar 04: PW #3136 Added reformatted volume label for CD (m_cdLabel).
        // 

        std::string m_volumeLabel;
        std::string m_cdLabel;
		std::string m_chosenDrive;
        
        //
        // Manages the state of the GUI.
        //
        
        StateManager m_stateManager;

		IMediaAccess* iMediaAccess;
		std::vector<FilePathName> m_FilePathNameVector;

    };    
    
} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BURNCDDLG_H__1468A462_430F_4418_BC92_334001E5F340__INCLUDED_)

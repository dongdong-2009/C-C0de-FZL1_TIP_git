/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/WriteArchiveDlg.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog box to list the files that will be written to archive media,
  * and prompt the operator to begin the write process.
  *
  */

#if !defined(AFX_WRITEARCHIVEDLG_H__7F0333C5_A4F1_4DE2_A786_D061681B411E__INCLUDED_)
#define AFX_WRITEARCHIVEDLG_H__7F0333C5_A4F1_4DE2_A786_D061681B411E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <afx.h>

#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/WriteArchiveHelper.h"
#include "app/data_management/archive_manager/src/WriteArchiveListComponent.h"
// Forward declaration

namespace TA_Base_Bus
{
    class IGUIAccess;
}
    
namespace TA_Base_App
{   
    class WriteArchiveDlg : public CDialog
    {
    public:

        /**
          * WriteArchiveDlg
          *
          * Constructor used for automatic/scheduled archives.
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             Used to access the ArchiveManager entity data.
          * @param      CWnd* pParent
          *             The parent window.
          */

        WriteArchiveDlg(TA_Base_Bus::IGUIAccess& guiAccessor, CWnd* pParent = NULL);
        
        /**
          * WriteArchiveDlg
          *
          * Constructor used for manual archives.
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             Used to access the ArchiveManager entity data.
          * @param      const time_t& fromDate
          *             The first date in the range to write.
          * @param      const time_t& toDate
          *             The last date in the range to write.
          * @param      CWnd* pParent
          *             The parent window.
          */

        WriteArchiveDlg(TA_Base_Bus::IGUIAccess& guiAccessor, const time_t& fromDate,
            const time_t& toDate, CWnd* pParent = NULL);  

        /**
          * ~WriteArchiveDlg
          *
          * Standard destructor.
          */

        virtual ~WriteArchiveDlg() {};
        
        // Dialog Data
        //{{AFX_DATA(WriteArchiveDlg)
	    enum { IDD = IDD_WRITE_ARCHIVE_DIALOG };
		CStatic	m_insertCDDVD;
	    CStatic	m_numCds;
	    CButton	m_okButton;
		WriteArchiveListComponent	m_fileList;
    	//}}AFX_DATA
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(WriteArchiveDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(WriteArchiveDlg)
        virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    afx_msg void OnItemChangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	    virtual void OnCancel();
	    afx_msg void OnClose();
	    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        WriteArchiveDlg( const WriteArchiveDlg& );
        WriteArchiveDlg& operator=( const WriteArchiveDlg& );

        /**
          * populateList
          *
          * Populates the file list with the requested archive files.
          */

        void populateList();
            
        /**
          * cleanUp
          *
          * Deletes the archive files and export logs.  Used if the
          * operation was successful.
          */
        
        void cleanUp();

        /**
          * showCdBreakUp
          *
          * THIS IS A TEST METHOD ONLY.
          * It displays a message box showing how the files have
          * been split up into CDs.
          */
        
        void showCdBreakUp();

        //
        // The object that does all the back end work.
        //

        WriteArchiveHelper m_writeArchiveHelper;
        
        //
        // Stores the file names including the full path grouped by CD
        // referenced by the volume label.
        //
       
        WriteArchiveHelper::CdsMap m_cdsMap;

        //
        // Only display/write files with dates in this range.
        // The date is extracted from the filenames, which must be in the format
        // AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp.  
        // If no dates are supplied, all the files in the archive directory that 
        // are in the above format will be displayed/written.
        //
        
        time_t m_fromDate;
        time_t m_toDate;

        // 
        // The date range is only limited when manually archiving.
        //

        bool m_isManualArchive;

		//
		// The width of column name of m_fileList;
		//
		UINT m_fileListColNameWidth;
    };
    
} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITEARCHIVEDLG_H__7F0333C5_A4F1_4DE2_A786_D061681B411E__INCLUDED_)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// crutilDlg.h : header file
//
//{{AFX_INCLUDES()
#include "crystalreportviewer9.h"
#include "webbrowser2.h"
#include <vector>
#include <map>
//}}AFX_INCLUDES

#if !defined(AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_)
#define AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CrystalReportsHelper.hpp"

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "afxcmn.h"

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
    namespace TA_Reporting
    {


#endif // TRANSACTIVE_BUILD

typedef struct 
{
	unsigned long report_file_id;
	std::string report_file_name;
	std::string report_file_physical_path;
	unsigned long report_directory_id;
} REPORT_FILE;

typedef struct
{
	unsigned long report_directory_id;
	std::string report_directory_name;
	unsigned long report_parent_dir_id;
} REPORT_DIRECTORY;

		/////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg dialog

		#ifdef TRANSACTIVE_BUILD
		class CCrutilDlg : public TA_Base_Bus::TransActiveDialog
		#else
		class CCrutilDlg : public CDialog
		#endif
		{
		// Construction
		public:
			bool m_bViewHistory;
			#ifdef TRANSACTIVE_BUILD
			CCrutilDlg(TA_Base_Bus::IGUIAccess* genericGUICallback, CWnd* pParent = NULL);	// standard constructor
			#else
			CCrutilDlg(CWnd* pParent = NULL);	// standard constructor
			#endif
			~CCrutilDlg();
			CString m_cstrReportName;
			CrystalReportsHelper* m_pCrystalReports;
			bool m_bShowMaxWindow;

			//TD18095, jianghp, to fix the performance of showing manager application
			virtual void init();
		// Dialog Data
			//{{AFX_DATA(CCrutilDlg)
			enum { IDD = IDD_CRUTIL_DIALOG };
			CCrystalReportViewer9	m_ReportViewer;
			CWebBrowser2	m_HistoryReportViewer;

			//Report Tree Selector
			CTreeCtrl m_ReportTree;
			//}}AFX_DATA

			//Use for Report Tree Selector at the left
			//CString m_StrPath;
			CBitmap m_BitmapFolder;
			CBitmap m_BitmapFile;
			CImageList m_ImageList;

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CCrutilDlg)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation


		protected:
			HICON m_hIcon;

			// Generated message map functions
			//{{AFX_MSG(CCrutilDlg)
			virtual BOOL OnInitDialog();
			afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
			afx_msg void OnPaint();
			afx_msg HCURSOR OnQueryDragIcon();
            afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnFileExit();
			afx_msg void OnHelpAbout();
			afx_msg void OnFileOpen();
			afx_msg void OnDestroy();
			afx_msg void OnViewHistory();
			afx_msg void OnViewRefresh();
			afx_msg void OnClose();
	afx_msg void OnPrintButtonClicked(BOOL FAR* UseDefault);
	afx_msg void OnRefreshButtonClicked(BOOL FAR* UseDefault);//cl-18123--lkm
	afx_msg void OnUpdateViewHistory(CCmdUI* pCmdUI);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
			afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);	
			DECLARE_MESSAGE_MAP()
		private:
			void saveWindowSize();
			void restoreWindowSize();
			void displayAboutBox();

		public:			            
          /**
            * createReportTreeListFromDB
            *
            * This create report tree for CTreeList control from Database. It contains two step to 
            * create the report tree. Firstly, create Folder tree: the Directory data comes from table 
            * RM_REPORT_DIRECTORY and stores in vector variable -- m_reportDirVec. If the Directory
            * node's parent_dir_id equal to 0, it means that this is a root Directory node; if larger than
            * 0, then this node is a common node and its parent node is 'parent_dir_id'; if less than 0,
            * then throw an exception. Secondly, after create Directory node for report tree, create File
			*  node(s) for it. The File node data comes from table RM_REPORT_FILE and stores in map
			* variable -- m_reportFileMap.
			*
            * @exceptions ReportManagerException 
            *             It must throw this if the parent node in Directory node does not contain the correct
            *             information for the report tree to insert it.
            */
			void createReportTreeListFromDB();
			            
          /**
            * repaintDlgAfterOperate
            *
            * This is called when user operated the ReportManager, like use the original way to open report
            * file from ReportManager's menu, or double click the report file node on the report tree, etc.
            * For that Crysta lReport Viewer or Web Viewer will mess up ReportManager's HMI after these 
			* actions, we need to call repaintDlgAfterOperate after that.
            */
			void repaintDlgAfterOperate();
            
          /**
            * findItemWithDirId
            *
            * This will be called when a node was inserted to the report tree, whether the node is
            * Directory node or File node, unless the node is a root-Directory node(parenet_dir_id 
			* equal to 0). This function find a Directory node according to the specified Directory ID.
            *
            * @param CtreeCtrl& treeCtrl - Indicate which report tree you want to find
			* @param HTREEITEM item - The node you want to find is under this item, for root item,
			*										use the GetRootItem() of CTreeCtrl
			* @param int dirId - The ID of the Directory node you want to find out
			* @param BOOL findChild - TRUE means recursive to search the node under the child
			*   									node of the 'item', FALSE for not.
            *
            */			
			HTREEITEM findItemWithDirId(CTreeCtrl& treeCtrl,HTREEITEM  item, int dirId, BOOL findChild);
			            
          /**
            * initializeReportDirectoryVec
            * 
            * This function should be called before/during initialize ReportManager. It will initialize the vector
			* variable -- m_reportDirVec, which contains members of struct REPORT_DIRECTORY.
            * It use ReportDirectoryAccessFactory to connect to DB and get the data from the table 
			* RM_REPORT_DIRECTORY. The REPORT_DIRECTORY member(s) contains Directory ID, 
			* Directory name(display on the report tree), and parent Directory ID.
            */
			void initializeReportDirectoryVec();
						            
          /**
            * initializeReportFileMap
            *
			* This function should be called before/during initialize ReportManager. It will initialize the map
			* variable -- m_reportFileMap, which map the File ID and members of struct REPORT_FILE.
			* It use ReportFileAccessFactory to connect to DB and get the data from the table 
			* RM_REPORT_FILE. The REPORT_FILE member(s) contains File ID, File name(display on 
			* the report tree), File physical path, and File's directory ID.
            */
			void initializeReportFileMap();
			            
          /**
            * getReportDirectoriesUseParentId
            *
			* This function get the report Directories (in type REPORT_DIRECTORY) from Database using 
			*  the specified parent Directory ID, and return the Directories under the parent Directory.
            *
            * @param unsigned long parentId - Indicate the parent Directory ID want to find
			* @return std::vector<REPORT_DIRECTORY> representing the report Directories got from Database.
            *
            */			
			std::vector<REPORT_DIRECTORY> getReportDirectoriesUseParentId(unsigned long parentId);
			            
          /**
            * getReportFilesUseDirId
            *
            * This function get the report Files (in type REPORT_FILE) from Database using the specified
			*  Directory ID, and return the Files under the Directory.
            *
            * @param unsigned long dirId - Indicate the Directory ID want to find
			* @return std::vector<REPORT_FILE> representing the report Files got from Database.
            *
            */			
			std::vector<REPORT_FILE> getReportFilesUseDirId(unsigned long dirId);

			afx_msg void OnReportSourceErrorCrviewer1(LPCTSTR errorMsg, long errorCode, BOOL* UseDefault);

		public:
			//When double click Report Tree Selector, open the specified file
			afx_msg void OnNMDblclkTreeSel(NMHDR *pNMHDR, LRESULT *pResult);

		private:
			// The vector variable stores the report file data get from Database
			std::vector<REPORT_DIRECTORY> m_reportDirVec;
			// The map variable stores the report directory data get from Database
			std::map<unsigned long,REPORT_FILE> m_reportFileMap;

			unsigned long m_maxOrRestore;

			// restore the value of report tree border
			int m_treeListBorder;
		};

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_)

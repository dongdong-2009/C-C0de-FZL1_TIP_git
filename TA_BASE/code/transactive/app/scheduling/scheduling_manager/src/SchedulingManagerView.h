/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerView.h $
  * @author:  San Teo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/03 17:02:56 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Displays the list of jobs that are known to the Scheduling Agent.  It will pass user requests related to 
  * jobs to the document object.
  */

#if !defined(AFX_SCHEDULINGMANAGERVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)
#define AFX_SCHEDULINGMANAGERVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scheduling/scheduling_manager/src/Job.h"
#include <vector>
#include <string>
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_Base_Core
{
    class IProfile;
    class ILocation;
}

namespace TA_Base_App
{
    class JobDialog;
    class Job;

    class CSchedulingManagerView :  public CListView,
                                    public TA_Base_Bus::ISortingCallback
    {
    protected: // create from serialization only
	    CSchedulingManagerView();
	    DECLARE_DYNCREATE(CSchedulingManagerView)

    // Attributes
    public:
	    CSchedulingManagerDoc* GetDocument();

    // Operations
    public:

        /** 
          * getSelections
          *
          * Returns all jobs that are selected on the view
          *
          *
          * @return all jobs that are selected on the view
          */
        void getSelections(JobPtrVector& selections);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSchedulingManagerView)
	    public:
	    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	    virtual void OnInitialUpdate();
	    protected:
	    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CSchedulingManagerView();
        void initialise(const std::vector<std::string>& jobTypes);

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

    protected:

    // Generated message map functions
    protected:
	    //{{AFX_MSG(CSchedulingManagerView)
	    afx_msg void onViewJobdetails();
	    afx_msg void onJobAdd();
	    afx_msg void onJobDelete();
	    afx_msg void onJobDisable();
	    afx_msg void onJobEdit();
	    afx_msg void onJobEnable();
	    afx_msg void onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void onFilePrint();
	    //}}AFX_MSG
        afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onCreateJob(WPARAM theJob, LPARAM);
        afx_msg LRESULT onUpdateJobList(WPARAM, LPARAM);
        afx_msg LRESULT onJobDeleted(WPARAM, LPARAM);
        afx_msg LRESULT onJobUpdated(WPARAM, LPARAM);
        afx_msg LRESULT onShowExceedMaxJobDlg(WPARAM, LPARAM);
	    DECLARE_MESSAGE_MAP()

    private: // methods
		
		//TD18095,jianghp
		void initRightLib();
        /**
         * sortCallback
         *
         * User defined sorting function
         *
         * @return positive value if item lParam2 before lParam1, negative if lParam1 should
         *      come first
         *
         * @param lParam1, lParam2 user defined values passed in from sort operation (app specific)
         * @param columnIndex the index of the column being sorted
         * @param currentSort defines the type of sorting specified (ie ascending or descending)
         */
        virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort);

		//TD16352++
		/** 
          * isJobEditLockedForSelection
          *
          * Checks if current selected job is edit locked
          *
          * @param jobId 
          */
        bool isJobEditLockedForSelection(const std::string& jobId);

		/**
		  * updateEditLock
		  *
          * Locks or releases job for edit preventing edits or removal while another operator is editing
          *
          * @param jobId
		  * @param lock 
          */
		void updateEditLock(const std::string& jobId, bool lock);
		//++TD16352


        /** 
          * enable
          *
          * Common method that will be used to enable/disable the selected items
          *
          * @param isEnable true if intended to enable the selected items
          */
        void enable(const bool isEnable);


        /** 
          * updateNumJobsProcessed
          *
          * Update the status on the jobs that being processed
          *
          * @param resourceId
          * @param numProcessed
          * @param totalNum
          *
          */
        void updateNumJobsProcessed(int resourceId, int numProcessed, int totalNum);


        /** 
          * updateNumJobsStatus
          *
          * Update the status on the number of jobs left in the list
          *
          */
        void updateNumJobsStatus();


        /** 
          * populateItem
          *
          * Populates the specified row with the details of the given job.
          * NOTE: This method doesn't insert the row, it only populates it.  
          *
          * @param index Where it is going to be inserted
          * @param job The job that is going to be displayed here
          *
          */
        void populateItem(int index, Job& job);


		/**
		 * isRightsAgentContactable
		 *
		 * Attempts to check rights for RUN_APPLICATION on the Scheduling Manager
		 * to see if the Rights Agent is indeed contactable.
		 * 
		 * @return True if the Rights Agent was able to be contacted, false otherwise
		 */
		bool isRightsAgentContactable();

		bool canLoadDLL(); //TD16070
		//jinhua++ TD20487
		// C830 agreed to use geographical location as the primary sort followed by status, type and schedule start.
		int compareStringContent( int rowIndex1, int rowIndex2, int columnIndex );
		int compareDateContent( Job* job1, Job* job2 );
		int sortByPrimaryAndSecondary( int rowIndex1, int rowIndex2 );
		//++jinhua TD20487

        // A reference to the list control for efficiency so that we don't have to call 
        // GetListCtrl() each time it's needed
	    CListCtrl& m_listCtrl;
		int * m_printoutColWidths;//cl-16406-lkm
        std::vector<std::string> m_jobTypes;

        // Maintains the current sort column and order
        TA_Base_Bus::AutoSortListCtrl::SortInfo m_sortInfo;

        enum ColumnType
        {
            Status,
            Type, 
            Description,
            ScheduleStart,
            Recurrence,
            ScheduleEnd,
            Profile,
            Location,
            NumColumnType
        };

        // Maintains the information for each column
        struct ColumnInfo
        {
            ColumnType type;        // Column Type
            unsigned int width;     // The width of the column
            unsigned int name;      // The string Id in the resource
            TA_Base_Bus::AutoSortListCtrl::ESortType sortBy; // The way that the column is sorted by
        };

        // Contains the information regarding all columns
        static const ColumnInfo COLUMN_INFO[NumColumnType];

        // Using this flag to indicate whether we're in the process of updating the view
        // If it is, then don't tell the main frame of the change (otherwise onItemchange will
        // keep calling on the main frame and crashes)
        bool m_isUpdating;

        // Holds the job types that do not need specialised plugins for configuring that type of job
        std::vector<std::string> m_typesWithoutPlugin;

        // The tooltip control so we can set the display time of the tooltip
        CToolTipCtrl m_toolTip;

        // The common strings to be displayed on the list
        CString INHIBITED_STRING; 
        CString UNINHIBITED_STRING;
        CString RECUR_NONE_STRING;
        CString RECUR_DAILY_STRING;
        CString RECUR_WEEKLY_STRING;
        CString RECUR_MONTHLY_STRING;
        CString RECUR_YEARLY_STRING;

		// Make a local rights library to do Rights Agent communication testing
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;
		unsigned long m_resourceKey;
    };

    #ifndef _DEBUG  // debug version in SchedulingManagerView.cpp
    inline CSchedulingManagerDoc* CSchedulingManagerView::GetDocument()
       { return (CSchedulingManagerDoc*)m_pDocument; }
    #endif

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULINGMANAGERVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)

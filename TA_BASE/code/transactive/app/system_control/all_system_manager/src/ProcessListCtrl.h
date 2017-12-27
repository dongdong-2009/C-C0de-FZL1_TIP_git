/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ProcessListCtrl.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * This class is a specialised List View control that
  * displays process information. It is derived from
  * MFC's CListCtrl class.
  *
  */

#if !defined(AFX_PROCESSLISTCTRL_H__679184B7_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_PROCESSLISTCTRL_H__679184B7_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "AdminManager.h"

namespace TA_Base_App
{
    struct SortParams
    {
        HWND  hwnd;          // The handle of the list control
        int   colIndex;      // The index of the column clicked
        bool  sortAscending; // True if the sort should be in ascending order,
                             // and false otherwise.
    };

    class ProcessListCtrl : public CListCtrl
    {

    public:

        /**
          * Constructor
          */
        ProcessListCtrl();

        /**
          * Destructor
          */
	    virtual ~ProcessListCtrl();


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ProcessListCtrl)
// 	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL

        /**
          * populate
          * 
          * Populates the "list view" control with processes from a vector
          * of ProcessData structures.
          *
          * @param processData  The vector of ProcessData structures
          */
        virtual void populate(const std::vector<ProcessExactData>& processData);

        /**
          * update
          * 
          * Calls populate() with the received vector of ProcessData structs
          * and reselect the previously selected processes if they are still
          * available.
          *
          * @param processData  The vector of ProcessData structures
          */
        void update(const std::vector<ProcessExactData>&);

        /**
          * getProcessName
          * 
          * @return A string containing the identifier of the process corresponding 
          *         to the given index. Empty if not found.
          *
          * @param Index  The process's index.
          */
        virtual std::string getProcessName(unsigned int index);
    
        /**
          * getStatus
          * 
          * @return A string containing the status of the process corresponding 
          *         to the given index. Empty if not found.
          *
          * @param Index  The process's index.
          */
        virtual std::string getStatus(unsigned int index);
	    
        /**
          * select
          * 
          * Selects the requested processes if they are available.
          *
          * @return true  if it worked
          *         false if none of the requested processes was found.
          *
          * @param A reference to a CStringList object containing the entity names
          *        of the processes.
          */
        bool select(CStringList& i_Processes);
	    
        /**
          * freeze
          * 
          * Sets a boolean variable to true. This flag prevents the processes list 
          * to be altered by other threads using populate() or update().
          */
        void freeze();
	    
        /**
          * unfreeze 
          *
          * Sets the m_isFrozen boolean to false.
          */
        void unfreeze();

        void init( const std::string& strLocationName );

        std::string& getLocationName();

    private:


        /**
         * populateProcessState
         *
         * This will populate the Process State and Time Started columns for the specific position
         *
         * @param insertposition The row in the list we are modifying
         * @param state The state of the process that we must convert to text
         * @param processStartTime The time we should convert to string and populate with
         * @param isManagedProcess This indicates if the process is a managed process or not
         *                         so this tells us what string to show for each state
         */
        void populateProcessState(int insertPosition, TA_Base_Core::EProcessStatus state,
                                  const CTime& processStartTime,
                                  bool isManagedProcess);

        /**
         * populateProcessOperationMode
         *
         * This will populate the OperationMode column for the specific position
         *
         * @param insertposition The row in the list we are modifying
         * @param operationMode The operation mode of the process that we must convert to text
         */
        void populateProcessOperationMode(int insertPosition,
                                          TA_Base_Core::EOperationMode operationMode);
        
        /**
         * populateProcessNewState
         *
         * This will populate the Requested State columns for the specific position
         *
         * @param insertposition The row in the list we are modifying
         * @param state The state of the process that we must convert to text
         * @param isManagedProcess This indicates if the process is a managed process or not
         *                         so this tells us what string to show for each state
         */
        void populateProcessNewState(int insertPosition,
                                     TA_Base_Core::EProcessStatus state,
                                     bool isManagedProcess);

        /**
          * setupHeader 
          *
          * Sets up the columns headers corresponding to the process information.
          */
	    virtual void setupHeader();

        /**
          * OnHeaderClicked
          *
          * Called when the user clicks on the header of a column. Causes the
          * column to be sorted.
          */
        void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);

		/**
		  * Sorts the list control.
		  */
		void sortListControl();

        /** 
          * compareStrings
          *
          * Compares two strings in the list control so they can be sorted.
          */
        static int CALLBACK compareStrings(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

        // TD16592 ++
        /** 
          * compareStrings2
          *
          * Compares two strings in the list control so they can be sorted according to specific rule.
          */
        static int CALLBACK compareStrings2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        // ++ TD16592

        /** 
          * compareIntegers
          *
          * Compares two integers in the list control so they can be sorted.
          */
        static int CALLBACK compareIntegers(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

        /** 
          * compareDates
          *
          * Compares two dates in the list control so they can be sorted.
          */
        static int CALLBACK compareDates(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

        /** 
          * updateSortArrow
          *
          * Updates the direction and location of the sort arrow. The
          * arrow will be removed from its current location and placed
          * on the column indicated by the parameter, pointing in the
          * right direction.
          *
          * @param newColumnNum  The column number to place the arrow on.
          */
        void updateSortArrow(int newColumnNum);

        /** 
          * removeSortArrow
          *
          * Removes the sort arrow from its current location.
          *
          */
        void removeSortArrow();

	    // Generated message map functions
	    //{{AFX_MSG(ProcessListCtrl)
// 	    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
// 	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	    //}}AFX_MSG

        /**
          * True if the process list is frozen, and false otherwise.
          */
	    bool m_isFrozen;

        /**
          * The index of the column that is sorted.
          */
        int m_sortedCol; 

        /**
          * True if the column is sorted in ascending order, and false otherwise.
          */
        bool m_sortAscending; 

        // TD16592 ++
        /**
          * The locationname of the System Controller login.
          */
        std::string m_hostName;
        // ++ TD16592

        /**
          * The list control's columns, in the order they initially appear.
          */
        enum ColumnNames
        {
            PROCESS_ID_COL = 0,
            HOST_COL,
            OPERATION_MODE_COL,
            STATUS_COL,
            NEW_STATUS_COL,
            RUNNING_SINCE_COL,
            RESTARTS_COL,
			DEBUG_LEVEL_COL
        };

	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PROCESSLISTCTRL_H__679184B7_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)

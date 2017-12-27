/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Defines a list control that performs automatic sorting (string by default) - can customise
  * to sort numerically for specific columns etc
  *
  * Static definitions are provided so that customised list control can use
  * the sorting functionality without actually having to derive from AutoSortListCtrl
  * (because often we want to derive from a different base list control, and multiple
  *  inheritance is not suitable for these cases)
  * 
  * Classes not wanting to inherit from this class need to maintain a SortInfo structure locally
  *      (call defineSortingSemantic for each column, otherwise defaults to sorting by string)
  *
  * they should declare in the header
    
    protected:
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        
  * and forward the notification on to the static function in the source as follows:   

    LRESULT DerivedClass::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {     
        AutoSortListCtrl::WindowProc(message,
                                     wParam.
                                     lParam,
                                     *this, 
                                     m_colSortingInfo);
        return BaseClass::WindowProc(message, wParam, lParam);
    }
  */

#pragma once

#include <vector>
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

namespace TA_Base_Bus
{
    class ISortingCallback;

    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) AutoSortListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    #else
    class AFX_EXT_CLASS AutoSortListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    #endif
    {
    public:
        
        /**
         * AutoSortListCtrl
         *
         * @param maxColumns defines the maximum number of columns this control supports
         */
        AutoSortListCtrl(int maxColumns);
                
        // Defines kind of sorting for each column, by default we sort by string
        enum ESortType
        {
            BY_STRING,
            BY_INTEGER,
            BY_CALLBACK,     // Only changes in ItemData will cause a re-sort
			BY_MIXED
        };
        
        // Initially will be unset, in which case resolves to ascending order on first request
        enum ECurrentSort
        {
            UNKNOWN,        // Mixed state (i.e. unsorted currently)
            ASCENDING,
            DESCENDING
        };

        
        #ifdef IMPORT_EXTENSIONS
        class __declspec(dllimport) SortInfo
        #else
        class AFX_EXT_CLASS SortInfo
        #endif
        {
            friend class AutoSortListCtrl;

        public:            
            /**
             * @param colCount defines number of columns support by this data structure
             * @param autoResort defines whether the list control should be re-sorted whenever
             *          there is any state change that could break the ordering sequence.  
             *          The list control will not automatically re-sort when a modal dialog
             *          is being displayed, to avoid invalidating item indices unexpectedly if the 
             *          client code displays an error message between setting item data calls.
             *          It detects if a modal dialog is present, by using AfxGetMainWnd()->IsWindowEnabled(), 
             *          therefore when parent window disabled will not automatically re-apply the sort (can be done
             *          explicitly if a sort is required)
             */
            SortInfo(int colCount, bool autoResort = true);
            
            /// If specifying BY_CALLBACK, user must specify the callback object with setCallback
            /// @exception std::out_of_range exception if column out of range
            void defineSortingSemantic(int column, ESortType sortType);
            /// @exception std::out_of_range exception if column out of range
            void setCurrentSort(int column, ECurrentSort sort);
            /// Activates sorting for a column (now reApplyCurrentSort will base sort
            /// on this column)
            void activateSort(int column);            
            /// Defines the callback to be used if any column is sorted using BY_CALLBACK
            void setCallback(ISortingCallback* callback);
            
        private:
            
            // Can't use maps here (which would be nicer) - because allocations of heap memory
            // won't work over DLL boundary..

            // Define what kind of sort to perform on a column by column basis, defaults to BY_STRING
            std::vector<ESortType>  m_sortingSemantics;
            // Current sort state, if currently undefined (as in startup), will default to UNKNOWN
            std::vector<ECurrentSort>  m_sortState;

            int m_currentSortColumn;
            bool m_reSortRequired;
            bool m_repostRefreshMessageOnEnable;
            bool m_automaticResort;

            // Temporary internal state variables used while a sort is taking place
            mutable bool m_isCurrentlySorting;
            mutable std::map<int, DWORD> m_itemDataMap;

            ISortingCallback* m_pSortingCallback;
        };            

       
        /**
         * Instanced AutoSortListCtrl WindowProc handler (forwards to static implementation)
         */
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        /**
         * Provides entry point for windows message notification forwarding
         *
         * @param listControl the list control that is to be sorted
         * @param sortData [IN] stores the current sorting state for each column,
         */
        static void WindowProc(UINT message, 
                                 WPARAM wParam, 
                                 LPARAM lParam,
                                 CListCtrl& listControl,
                                 SortInfo& sortData);

        /** 
          * setSortInfo
          *
          * Replaces the default sort info with the new one
          *
          * @param sortData
          */
        void setSortInfo(const SortInfo& sortData)
        {
            m_colSortingInfo = sortData;
        }


    //protected:            
	public:	
		 /**
         * reApplyCurrentSort
         *
         * Called to update the refresh the list ordering (upon a change
         *  which could have invalidated the sort)
         *
         * @param listControl the list control that is to be sorted
         * @param sortData [IN] stores the current sorting state for each column,
         */
        static void reApplyCurrentSort(CListCtrl& listControl,
                                        const SortInfo& sortData);
    
		

    private:

        /**
         * onItemTextChanged
         *
         * Called whenever there is a change to the text in a row of the list control
         * (this means the sorting may need to be re-done)
         *
         * @param changeDetails defines the item text change that has taken place
         * @param listControl the list control that is to be sorted
         * @param [IN/OUT] sortData stores the current sorting state for each column
         */
        static void onItemTextChanged(const LVITEM& changeDetails, 
                                                CListCtrl& listControl,
                                                SortInfo& sortData);

        /**
         * onItemDataChanged
         *
         * Called whenever there is a change to the item data of the specified item
         *
         * @param changeDetails defines the item text change that has taken place
         * @param listControl the list control that is to be sorted
         * @param [IN/OUT] sortData stores the current sorting state for each column
         */
        static void onItemDataChanged(const LVITEM& changeDetails, 
                                                CListCtrl& listControl,
                                                SortInfo& sortData);

        /**
         * requestColumnSort
         *
         * Requests that the specified column be sorted
         *
         * @pre If sort method is BY_CALLBACK, the callback must be defined
         *
         * @param listControl the list control that is to be sorted
         * @param columnIndex defines the zero based column index (this index
         *              is stored in sortData, so we know which column is currently sorted)
         * @param sortData [IN/OUT] stores the current sorting state for each column,
         *              this structure will be updated to reflect the new sorted state
         *              for a column
         */
        static void requestColumnSort(DWORD columnIndex, CListCtrl& listControl, SortInfo& sortData);
        
        /**
         * requestColumnSort
         *
         * Same as other requestColumnSort, except does not cycle the current sorting mode
         *      for the column index provided in 'sortData'
         */
        static void requestColumnSort(CListCtrl& listControl, const SortInfo& sortData);
       
        /**
         * SortOnColumnClick
         *
         * Processes the column click message, which originated from the specified list control
         *
         * @return TRUE, always (indicating forward notification to parent window, allowing
         *              it to also process the message)
         * @param listControl the list control that is to be sorted
         * @param pNMHDR, pResult parameters passed from the LVN_COLUMNCLICK message
         * @param sortData [IN/OUT] stores the current sorting state for each column,
         *              this structure will be updated to reflect the new sorted state
         *              for a column
         */
        static BOOL SortOnColumnClick(CListCtrl& listControl, 
                                        NMHDR* pNMHDR, 
                                        LRESULT* pResult,
                                        SortInfo& sortData);
        
        /**
         * setToCustomSort
         *
         * @return true if the current sorting column defined by sortData is
         *          being sorted using a custom sort
         */
        static bool isSetToCustomSort(const SortInfo& sortData);

        /**
         * reApplyCurrentSort
         *
         * Local implementation of static counterpart
         */
        void reApplyCurrentSort();

        // Local container for sorting information
        SortInfo m_colSortingInfo;
        
        DECLARE_MESSAGE_MAP()
    };

} // end namespace TA_Base_Bus

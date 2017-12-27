/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/filter_list_ctrl/FilterListCtrl.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * A list control that performs sorting and filtering based on the content of the
  * associated edit box.
  *
  * To initialise this control with appropriate column settings and data, just call:
  * listCtrl.initialise(editBox, columnAttribute);
  * listCtrl.setData(data);
  * listCtrl.refresh();
  *
  * Create a message handler ON_EN_CHANGE for the edit box, so that the refresh()
  * method could be called from within that method to refresh the list when the 
  * text changes.
  */

#if !defined(FilterListCtrl_A2661072_968F_48e4_8EC3_62FF73A7A9CC__INCLUDED_)
#define FilterListCtrl_A2661072_968F_48e4_8EC3_62FF73A7A9CC__INCLUDED_

#pragma once

#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) FilterListCtrl : public AutoSortListCtrl
    #else
    class AFX_EXT_CLASS FilterListCtrl : public AutoSortListCtrl
    #endif
    {
    public:
	    virtual ~FilterListCtrl();
	    FilterListCtrl(int maxColumns = 2);

    public:
        /**
         * Contains the information for setting up a column.
         * @version 1.0
         * @created 11-May-2005 02:10:46 PM
         */
        struct RowData
        {
            // The data in string vector.
	        std::vector<CString> data;

            // The pointer to the callback object.
            LPARAM callback;
        };


	    /**
	     * Just maintains all the information required to set up a column of a list
	     * control.
	     * @version 1.0
	     * @created 11-May-2005 02:10:41 PM
	     */
	    struct ColumnAttributes
	    {
            // The name of the column.
		    CString name;

            // The width of the column
		    int width;

            // How the column is going to be sorted
		    AutoSortListCtrl::ESortType sortBy;

            // true if this column is to be filtered.
            bool isFilterable;

	    };


	    /**
	     * Constructs the list control with the given attributes and associate it with the
	     * given edit control.
	     * @param editBox The edit box associated with this list control from which the list control
         *                will obtain the filter text from.
	     * @param colAttribs The details of the column
         * @param caseSensitive Default to case insensitive.
	     * 
	     */
	    void initialise(CEdit& editBox, const std::vector<ColumnAttributes>& colAttribs, const bool caseSensitive = false);


	    /**
	     * Refreshes the list.  It gets the content of the associated edit box and see if
	     * it's content has changed since last refresh.  If it has, it will clear the list
	     * and filter the content of the list based on the text.
	     */
	    void refresh();


	    /**
	     * Set data in the list control.
	     * @param data
	     * 
	     */
	    void setData(const std::vector<RowData>& data);


	    /**
	     * Add data to the list control.
	     * @param data
	     * 
	     */
	    void addData(const RowData& data);

		/**
		 * Get Column Number.
		 */
		virtual unsigned int getColumnNum();

    protected:
        /** 
          * addRow
          *
          * Adds a row of data to the list control
          *
          * @param index
          * @param rowData
          *
          */
        void addRow(const int index, RowData& rowData);


        /**
         * Instanced AutoSortListCtrl WindowProc handler (forwards to static implementation)
         */
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        // Maintains the data.
		std::vector<RowData> m_data;

        // Maintains the current sort column and order
        AutoSortListCtrl::SortInfo m_sortInfo;

        // True if it is case sensitive
        bool m_isCaseSensitive;

    private:
	    // References the actual filter edit box
	    CEdit* m_filterEdit;

        // Contains the attributes of the column
        std::vector<ColumnAttributes> m_columnAttribs;
    };
};
#endif // !defined(FilterListCtrl_A2661072_968F_48e4_8EC3_62FF73A7A9CC__INCLUDED_)
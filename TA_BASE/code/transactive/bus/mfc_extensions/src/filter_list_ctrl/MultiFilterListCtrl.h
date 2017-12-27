/**
  * The source code in this file is the property of
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:  $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.h $
  * @author:  Huangxin He
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * A list control that performs sorting and filtering based on the content of the
  * edit boxs associated with each column. Every column has a filter, and only items 
  * in which text of each column matches the associated filter would be display.
  *
  * To initialise this control with appropriate column settings and data, just call:
  * listCtrl.initialise(columnAttribute, caseSensitive);
  * listCtrl.setData(data);
  * listCtrl.refresh();
  *
  * Create a message handler ON_EN_CHANGE for each edit box, so that the refresh()
  * method could be called from within that method to refresh the list when the 
  * text changes.
  */

#if !defined(AFX_MULTIFILTERLISTCTRL_H__F7C21A9C_7994_4642_93D6_783CA68A4E76__INCLUDED_)
#define AFX_MULTIFILTERLISTCTRL_H__F7C21A9C_7994_4642_93D6_783CA68A4E76__INCLUDED_

#pragma once

#include "bus/mfc_extensions/src/filter_list_ctrl/FilterListCtrl.h"
namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) MultiFilterListCtrl : public FilterListCtrl
    #else
    class AFX_EXT_CLASS MultiFilterListCtrl : public FilterListCtrl
    #endif
	{
	public:
		MultiFilterListCtrl(int maxColumns = 2);
		virtual ~MultiFilterListCtrl();

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
			
			// the filtering edit box associated with each column
			CEdit* editFilter;
	    };

	public:
	
		/**
	     * Constructs the list control with the given attributes
	     * @param colAttribs The details of the column, and the corresponding edit box
         * @param caseSensitive Default to case insensitive.
	     */

		void initialise(const std::vector<ColumnAttributes>& colAttribs, const bool caseSensitive = false);

		/**
	     * Refreshes the list.  It gets the content of the associated edit box, clears the list
	     * and filter the content of the list based on the text. only those items in which text 
		 * of each column matches the associated filter would be display.
	     */
		void refresh();

		/**
		 * Get Column Number.
		 */
		virtual unsigned int getColumnNum();
		
	private:

		/**
		 * update the filtering text for each column.
		 */
		void updateFilterText();

	private:
        std::vector<ColumnAttributes> m_columnAttribs;
		std::vector<CString> m_filterText;
		bool m_isfilterEmpty;
	};
};
#endif // !defined(AFX_MULTIFILTERLISTCTRL_H__F7C21A9C_7994_4642_93D6_783CA68A4E76__INCLUDED_)

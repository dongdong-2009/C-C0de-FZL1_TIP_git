/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/filter_list_ctrl/FilterListCtrl.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * A list control that performs sorting and filtering based on the content of the
  * associated edit box.
  *
  */

#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/Resource.h"

#pragma warning(disable:4786)
#include "bus/mfc_extensions/src/filter_list_ctrl/FilterListCtrl.h"


namespace TA_Base_Bus
{
    FilterListCtrl::FilterListCtrl(int maxColumns /*=2*/)
        : AutoSortListCtrl(maxColumns),
          m_sortInfo(maxColumns)
    {
    }


    FilterListCtrl::~FilterListCtrl()
    {
        // nothing to destroy
    }


    void FilterListCtrl::initialise(CEdit& editBox, const std::vector<ColumnAttributes>& colAttribs, const bool caseSensitive /*= false*/)
    {
        ASSERT(!colAttribs.empty());

        m_filterEdit = &editBox;

        m_columnAttribs = colAttribs;
		
        // Initialise the headers with the given attributes
        for (unsigned int i = 0; i < colAttribs.size(); i++)
        {
            InsertColumn (i, colAttribs[i].name, LVCFMT_LEFT, colAttribs[i].width);
            m_sortInfo.defineSortingSemantic(i, colAttribs[i].sortBy);
        }

        m_isCaseSensitive = caseSensitive;
    }


    void FilterListCtrl::refresh()
    {
        // get the text to search from the text box
        ASSERT(m_filterEdit != NULL); // make sure the filter edit box exists
        ASSERT(!m_columnAttribs.empty());

        CString searchText;
        m_filterEdit->GetWindowText(searchText);

        // Remove all items from the list so that we could rebuilt it again
        DeleteAllItems();

        // rebuilding the list, only add the ones that 
        unsigned int numCols = m_columnAttribs.size();
        unsigned int numRows = m_data.size();

        // if there is no filter, just display the entire list
        if (searchText.IsEmpty())
        {
            for (unsigned int i = 0; i < numRows; i++)
            {
                addRow(i, m_data[i]);
            }
            RedrawItems(0, numRows);
            return;
        }

        // otherwise, we need to filter the list using the given filter
        if (!m_isCaseSensitive)
        {
            searchText.MakeLower();
        }

        int index = 0;
        for (unsigned int i = 0; i < numRows; i++)
        {
            RowData& rowData = m_data[i];
            // if the row does not have enough columns, just ignore it
            if (rowData.data.size() != numCols)
            {
                continue;
            }

            for (unsigned int j = 0; j < numCols; j++)
            {
                // if found a match in the filterable column
                if (!m_columnAttribs[j].isFilterable) 
                {
                    continue;
                }

                CString temp(rowData.data[j]);
                if (!m_isCaseSensitive)
                {
                    temp.MakeLower();
                }

                if (temp.Find(searchText) != -1)
                {
                    addRow(index++, rowData);
                    break;
                }
            }
        }
        RedrawItems(0, index);
    }


    void FilterListCtrl::addRow(const int index, RowData& rowData)
    {
        // add the entire row into the list control
        InsertItem(index, "");   
        for (unsigned int k = 0; k < getColumnNum(); k++)
        {
            SetItemText(index, k, rowData.data[k]);
        }
        SetItemData(index, rowData.callback);
    }



    void FilterListCtrl::setData(const std::vector<RowData>& data)
    {
        m_data = data;
    }


    void FilterListCtrl::addData(const RowData& data)
    {
        m_data.push_back(data);
    }


    LRESULT FilterListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }
	
	unsigned int FilterListCtrl::getColumnNum()
	{
		return m_columnAttribs.size();
	}

};


/**
  * The source code in this file is the property of
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * A list control that performs sorting and filtering based on the content of the
  * edit boxs associated with each column.
  *
  */

#include "bus/mfc_extensions/src/StdAfx.h"

#pragma warning(disable:4786)
#include "bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.h"


namespace TA_Base_Bus
{
	MultiFilterListCtrl::MultiFilterListCtrl(int maxColumns /* = 2 */)
		: FilterListCtrl(maxColumns),m_isfilterEmpty(true)
	{
	}

	MultiFilterListCtrl::~MultiFilterListCtrl()
	{
	}

	void MultiFilterListCtrl::initialise(const std::vector<ColumnAttributes>& colAttribs, const bool caseSensitive /*= false*/)
	{
		ASSERT(!colAttribs.empty());
		
		m_columnAttribs = colAttribs;

		// Initialise the headers with the given attributes
        for(unsigned int i = 0; i < colAttribs.size(); i++)
        {
            InsertColumn (i, colAttribs[i].name, LVCFMT_LEFT, colAttribs[i].width);
            m_sortInfo.defineSortingSemantic(i, colAttribs[i].sortBy);
			m_filterText.push_back("");
        }

        m_isCaseSensitive = caseSensitive;

	}

	void MultiFilterListCtrl::refresh()
	{
		// get the text to search from the text box
		ASSERT(!m_columnAttribs.empty());
		
		// Remove all items from the list so that we could rebuilt it again
		DeleteAllItems();

		// rebuilding the list, only add the ones that 
		unsigned int numRows = m_data.size();
		unsigned int numCols = getColumnNum();
		updateFilterText();

		// if there is no filter, just display the entire list
		if (m_isfilterEmpty)
		{
			for (unsigned int i = 0; i < numRows; i++)
			{
				addRow(i, m_data[i]);
			}
			RedrawItems(0, numRows);
			return;
		}

		int index = 0;
		unsigned int i = 0;
		unsigned int j = 0;
		for (i = 0; i < numRows; i++)
		{
			RowData& rowData = m_data[i];
			// accepted when the row does not have enough columns, but it cannot have more.
			if (rowData.data.size() > numCols)
			{
				continue;
			}

			for (j = 0; j < rowData.data.size(); j++)
			{
				// if found a match in the filterable column
				if (!m_columnAttribs[j].isFilterable || m_filterText[j].IsEmpty()) 
				{
					continue;
				}

				CString temp(rowData.data[j]);
				if (!m_isCaseSensitive)
				{
					temp.MakeLower();
				}
				if (temp.Find(m_filterText[j]) == -1)
				{
					break;
				}
			}
			if(j == rowData.data.size())
				addRow(index++, rowData);
		}
		RedrawItems(0, index);
	}

	void MultiFilterListCtrl::updateFilterText()
	{
		m_isfilterEmpty = true;
		for(unsigned int i=0; i<getColumnNum(); i++)
		{
			m_columnAttribs[i].editFilter->GetWindowText(m_filterText[i]);
			if(!m_filterText[i].IsEmpty())
			{
				if(m_isfilterEmpty)
					m_isfilterEmpty = false;
				if(!m_isCaseSensitive)
					m_filterText[i].MakeLower();
			}
		}
	}

	unsigned int MultiFilterListCtrl::getColumnNum()
	{
		return m_columnAttribs.size();
	}

};
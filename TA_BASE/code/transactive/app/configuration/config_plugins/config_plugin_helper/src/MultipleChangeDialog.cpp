/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Class is very similar (and based upon) ChangeDialog, except that it
  * uses a list control instead of a list box, and works with multiple selections
  * 
  */

#pragma warning(disable:4786)

#include <algorithm>

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TaAssert.h"

#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    MultipleChangeDialog::MultipleChangeDialog(ItemsForSelection& itemsForSelection,
                                        const CString& title,
                                        const std::vector<HeaderProperties>& columnHeaders,
                                        const std::list<unsigned long>& initialSelection,
                                        CWnd* pParent,
                                        bool useSingleSel,
                                        DWORD initialStyle)
	    : m_items(itemsForSelection),
          m_title(title),
          m_headerProperties(columnHeaders),
          m_initialSelection(initialSelection),
          m_initialStyle(initialStyle),
          m_useSingleSel(useSingleSel), 
          CDialog(MultipleChangeDialog::IDD, pParent),
          m_compareFunc(0),
          m_compareData(0)
    {
        FUNCTION_ENTRY("constructor");

        FUNCTION_EXIT;
    }


    void MultipleChangeDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(MultipleChangeDialog)
	        DDX_Control(pDX, IDOK, m_okButton);
	        DDX_Control(pDX, IDC_SEL_LIST, m_itemList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(MultipleChangeDialog, CDialog)
	//{{AFX_MSG_MAP(MultipleChangeDialog)
	ON_BN_CLICKED(IDOK, onOK)
    ON_NOTIFY(NM_DBLCLK, IDC_SEL_LIST, onDblclkList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SEL_LIST, onSelchangedList)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL MultipleChangeDialog::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");

	    CDialog::OnInitDialog();

        CFont* pMyFont = GetFont();
        if ( NULL != pMyFont )
        {
            LOGFONT stFontSetting;
            pMyFont->GetLogFont( &stFontSetting );
            // Make the font smaller 1 point size than main dialog. Why should be addition?
            stFontSetting.lfHeight += 1;
            stFontSetting.lfCharSet = GB2312_CHARSET;
            m_fntListCtrl.CreateFontIndirect( &stFontSetting );
            m_itemList.SetFont( &m_fntListCtrl );
        }
	    
        // Set the title of the dialog to the title the passed to the constructor
        SetWindowText(m_title);
        
        m_itemList.SetExtendedStyle(m_initialStyle);
        
        if (m_useSingleSel)
        {
            // Adjust window style to only allow single selection
            long p = GetWindowLong(m_itemList.m_hWnd, GWL_STYLE);
            p = p | LVS_SINGLESEL;
            SetWindowLong(m_itemList.m_hWnd, GWL_STYLE, p);
        }
    
        // Set up the columns
        for (int i = 0; i < (int)m_headerProperties.size(); i ++)
        {
            int nIndex = m_itemList.InsertColumn(i, 
                                m_headerProperties[i].label.c_str(),
                                m_headerProperties[i].format,
                                m_headerProperties[i].width);

            // Must re-apply the width parameter in certain cases
            if (LVSCW_AUTOSIZE_USEHEADER == m_headerProperties[i].width)
            {
                m_itemList.SetColumnWidth(nIndex, m_headerProperties[i].width);
                // Must account for the scroll bar manually
                int width = m_itemList.GetColumnWidth(nIndex) - GetSystemMetrics(SM_CXVSCROLL);
                m_itemList.SetColumnWidth(nIndex, width);
            }

            TA_ASSERT(i == nIndex, "MultipleChangeDialog: Invalid internal state!");
        }
    
        // Populate the list control with all the items passed
        for (ItemsForSelection::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            std::vector<std::string>& entries = iter->second;
            // Insert item at end of list
            int insertPoint = m_itemList.GetItemCount() + 1;
            
            int index = -1;

            if (entries.size() == 0)
            {
                // Empty item
                index = m_itemList.InsertItem(insertPoint, "");
            }
            else
            {
                index = m_itemList.InsertItem(insertPoint, entries[0].c_str());
            }
            
            // Fill in rest of column data
            for (int col = 1; col < (int) entries.size(); col ++)
            {
                if (col >= (int)m_headerProperties.size())
                {
                    // This row data set has more column data than the actual number of
                    // columns, discard the additional data
                    break;
                }
                m_itemList.SetItemText(index, col, entries[col].c_str());
            }

            // Set up key data
            m_itemList.SetItemData(index, iter->first);

            // Check if this item is selected
            KeySelection::iterator sel = std::find(m_initialSelection.begin(),
                                                   m_initialSelection.end(),
                                                   iter->first);
            
            if (sel != m_initialSelection.end())
            {
                // Set state to selected initially
                m_itemList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
        
        if (0 != m_compareFunc)
        {
            m_itemList.SortItems(m_compareFunc, m_compareData);
        }

        // Called to update state of OK/Cancel buttons based on current selection
        onSelchangedList();

        FUNCTION_EXIT;
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void MultipleChangeDialog::onSelchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        onSelchangedList();
        *pResult = 0;
    }


    void MultipleChangeDialog::onSelchangedList()
    {
        int selectionCount = m_itemList.GetSelectedCount();
	   
        if (0 == selectionCount)
        {
            m_okButton.EnableWindow(FALSE);
        }
        else
        {
            m_okButton.EnableWindow(TRUE);
        }

    }


    void MultipleChangeDialog::onOK() 
    {
        FUNCTION_ENTRY("onOK");

	    std::vector<unsigned int> selIndices = 
                TA_Base_Bus::SelectionHelper::getSelectedItemIndices(m_itemList);

        if (selIndices.size() > 0)
        {
            // Map the item selection into appropriate array of keys
            for (UINT i = 0; i < selIndices.size(); i ++)
            {
                m_keySelection.push_back(m_itemList.GetItemData(selIndices[i]));
            }

            CDialog::OnOK();
        }

        FUNCTION_EXIT;
    }


    void MultipleChangeDialog::onDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // No behaviour defined..
	    // onOK();
    }

    const MultipleChangeDialog::KeySelection& MultipleChangeDialog::getSelectedKeys() const
    {
        return m_keySelection;
    }
}

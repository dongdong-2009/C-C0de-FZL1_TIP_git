#if !defined(AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ItemSelectDialog.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ItemSelectDialog.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This dialog is launched by the Trend Properties dialog. It lists the items
  * that can be added for trending.
  *
  */

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/ItemSelectDialog.h"
#include "app/trending/trend_viewer/src/Globals.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
    //
    // CItemSelectDialog
    //
    CItemSelectDialog::CItemSelectDialog(const std::string& itemName, const std::map<std::string, TA_Base_Core::ItemInfo>& selectedItems, IItemStore& itemStore, CWnd* pParent /*=NULL*/)
        : CDialog(CItemSelectDialog::IDD, pParent),
          m_itemName(itemName),
          m_itemStore(itemStore)
    {
        //{{AFX_DATA_INIT(CItemSelectDialog)
        m_searchString = _T("<sub-string>");
        m_itemNameString = _T(itemName.c_str());
        //}}AFX_DATA_INIT

        std::map<std::string, TA_Base_Core::ItemInfo>::const_iterator it = selectedItems.begin();
        for(; it != selectedItems.end(); ++it)
        {
            m_selectedItems[it->first] = it->second;
        }
    }


    //
    // DoDataExchange
    //
    void CItemSelectDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CItemSelectDialog)
        DDX_Control(pDX, IDC_ITEM_LIST, m_itemList);
        DDX_Text(pDX, IDC_SEARCH_EDIT, m_searchString);
        DDX_Text(pDX, IDC_ITEM_NAME, m_itemNameString);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CItemSelectDialog, CDialog)
        //{{AFX_MSG_MAP(CItemSelectDialog)
    ON_BN_CLICKED(IDC_FIND_BUTTON, OnFindButton)
    ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
    ON_BN_CLICKED(IDC_RESET_BUTTON, OnResetButton)
    ON_NOTIFY(NM_CLICK, IDC_ITEM_LIST, OnClickItemList)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CItemSelectDialog message handlers


    //
    // OnInitDialog
    //
    BOOL CItemSelectDialog::OnInitDialog() 
    {
        CDialog::OnInitDialog();

        // Configure the list and load the datapoints
        m_itemList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
        initialiseItemList();

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    //
    // initialiseItemList
    //
    void CItemSelectDialog::initialiseItemList()
    {
        FUNCTION_ENTRY("InitialiseItemList");

        // Display a wait cursor while the list is being loaded
        CWaitCursor wait;

        // Insert columns
        LV_COLUMN lvc;

        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        // Insert Name column
        lvc.iSubItem = 0;
        lvc.pszText = "Name";
        lvc.cx = 300;
        m_itemList.InsertColumn( 0, &lvc );

        // Go through the map and put the data into the list
        loadItemList();

        FUNCTION_EXIT;
    }


    //
    // OnOK
    //
    void CItemSelectDialog::OnOK() 
    {
        int iItem = -1;
        int selCount = m_itemList.GetSelectedCount();

        if(selCount > 0)
        {
            iItem = m_itemList.GetNextItem(iItem, LVNI_SELECTED);
            TA_ASSERT(iItem != -1,  "Item selected does not have valid index");
            m_itemName = m_itemList.GetItemText(iItem, 0);
        }

        CDialog::OnOK();
    }


    //
    // OnFindButton
    //
    void CItemSelectDialog::OnFindButton() 
    {
        UpdateData(TRUE);
        m_itemList.DeleteAllItems();

        // Convert whole search string to upper case as we want the find to be
        // context insensitive.
        m_searchString.MakeUpper();

        std::vector<std::string> items = m_itemStore.getAllItems();

        // Go through the map and put the data into the list
        std::vector<std::string>::iterator it = items.begin();
        int i;
        for(i=0; it != items.end(); it++, i++)
        {
            CString name = (*it).c_str();

            // Convert whole string to upper case as we want the find to be
            // case insensitive.
            name.MakeUpper();
            
            if(name.Find(m_searchString) != -1)
            {
                m_itemList.InsertItem(i, (*it).c_str());
            }
       }

        // Sort list by Enabled...
        sortItemList();
    }
        
    
    //
    // OnClearButton
    //
    void CItemSelectDialog::OnClearButton() 
    {
        m_searchString = _T("<sub-string>");
        UpdateData(false);
        
        // 
        // Reload the list
        //
        loadItemList();
    }


    //
    // OnResetButton
    //
    void CItemSelectDialog::OnResetButton() 
    {
        m_itemNameString = _T(EMPTY_STRING);
        m_itemName = m_itemNameString;
        UpdateData(false);
    }

    
    //
    // OnClickItemList
    //
    void CItemSelectDialog::OnClickItemList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        int iItem = -1;
        int selCount = m_itemList.GetSelectedCount();

        if(selCount > 0)
        {
            iItem = m_itemList.GetNextItem(iItem, LVNI_SELECTED);
            TA_ASSERT(iItem != -1,  "Item selected does not have valid index");
            m_itemName = m_itemList.GetItemText(iItem, 0);
            m_itemNameString = m_itemName.c_str();
            UpdateData(false);
        }

        *pResult = 0;
    }


    //
    // SortDatapointList
    //
    void CItemSelectDialog::sortItemList()
    {
        FUNCTION_ENTRY("SortItemList");

        m_itemList.SortItems(compareItems, reinterpret_cast <LPARAM> (this));

        FUNCTION_EXIT;
    }


    //
    // CompareEvents
    //
    int CALLBACK CItemSelectDialog::compareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        CItemSelectDialog* pDlg = reinterpret_cast <CItemSelectDialog*> (lParamSort);

        LVFINDINFO info1;
        LVFINDINFO info2;

        info1.flags = LVFI_PARAM;
        info2.flags = LVFI_PARAM;
        info1.lParam = lParam1;
        info2.lParam = lParam2;

        int index1 = pDlg->m_itemList.FindItem(&info1);
        int index2 = pDlg->m_itemList.FindItem(&info2);

        int returnVal = 0; 
        CString string1;
        CString string2;
        int shortestString(0);

        string1 = pDlg->m_itemList.GetItemText(index1,0);
        string2 = pDlg->m_itemList.GetItemText(index2,0);
        returnVal = string1.CompareNoCase(string2);

        return returnVal;
    }


    //
    // LoadDataPointList
    //
    void CItemSelectDialog::loadItemList()
    {
        FUNCTION_ENTRY("LoadItemList");

        std::vector<std::string> items = m_itemStore.getAllItems();

        // Go through the map and put the data into the list
        std::vector<std::string>::iterator it = items.begin();
        int i = 0;
        for(; it != items.end(); it++)
        {
            std::string name = *it;

            // Don't insert if the item has already been selected
            std::map<std::string, TA_Base_Core::ItemInfo>::iterator selIt = m_selectedItems.find(name);
            if(selIt != m_selectedItems.end())
            {
                continue;
            }

            m_itemList.InsertItem(i, name.c_str());
            ++i;
        }

        // Sort list by Enabled...
        sortItemList();
        
        FUNCTION_EXIT;
    }
}


#endif // !defined(AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)






/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change an aspect of an entity. This could
  * be the subsystem, parent or location.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ChangeDialog::ChangeDialog(ItemsForSelection& itemsForSelection,
                               const CString& title,
                               const CString& currentItem,
                               CWnd* pParent /*=NULL*/)
	    : m_items(itemsForSelection),
          m_title(title),
          m_name(""),
          m_key(0),
          m_nameOfCurrentItem(currentItem),
          m_sortItems(true),
          CDialog(ChangeDialog::IDD, pParent)
    {
        FUNCTION_ENTRY("constructor");
	    //{{AFX_DATA_INIT(ChangeDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
        FUNCTION_EXIT;
    }


    void ChangeDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ChangeDialog)
	    DDX_Control(pDX, IDOK, m_okButton);
	    DDX_Control(pDX, IDC_ITEM_LIST, m_itemList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ChangeDialog, CDialog)
	//{{AFX_MSG_MAP(ChangeDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_LBN_DBLCLK(IDC_ITEM_LIST, onDblclkList)
	ON_LBN_SELCHANGE(IDC_ITEM_LIST, onSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL ChangeDialog::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");

	    CDialog::OnInitDialog();

        // Set the title of the dialog to the title the passed to the constructor
        SetWindowText(m_title);

        // Populate the list box with all the items passed
        for (ItemsForSelection::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
	        int index = 0;

            // Use a different method for inserting the string depending on whether sorting is
            // required. Ideally the style should be changed so that AddString can always be
            // used, however the style can't be changed once the list control is created
            // unless it is completely reconstructed.
            if ( m_sortItems )
            {
                // The list control has the LBS_SORT style, so calling AddString
                // will result in the items being added in alphabetical order.
                index = m_itemList.AddString( iter->second.c_str() );
            }
            else
            {
                // InsertString ignores LBS_SORT, so placing each element at the end of
                // the list in turn will result in items retaining their original order.
                index = m_itemList.InsertString( -1, iter->second.c_str() );
            }
            m_itemList.SetItemData(index, iter->first);
        }

        // Select the current item so the user knows what they had previously
        m_itemList.SelectString(-1,m_nameOfCurrentItem);
	    onSelchangeList();

        FUNCTION_EXIT;
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void ChangeDialog::onSelchangeList() 
    {
	    int selected = m_itemList.GetCurSel();
        if (selected == LB_ERR)
        {
            m_okButton.EnableWindow(FALSE);
        }
        else
        {
            m_okButton.EnableWindow(TRUE);
        }
    }


    void ChangeDialog::onOK() 
    {
        FUNCTION_ENTRY("onOK");

	    int selected = m_itemList.GetCurSel();
        if (selected != LB_ERR)
        {
            // If an item is selected then we want to set our member variables indicating
            // what the user chose.
            m_itemList.GetText(selected,m_name);
            m_key = m_itemList.GetItemData(selected);
    
            CDialog::OnOK();
        }

        FUNCTION_EXIT;
    }


    void ChangeDialog::onDblclkList() 
    {
	    onOK();
    }
}

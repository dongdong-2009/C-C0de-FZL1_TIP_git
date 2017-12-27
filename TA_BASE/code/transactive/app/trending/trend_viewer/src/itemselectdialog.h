#if !defined(AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ItemSelectDialog.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/itemselectdialog.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>

#include "app/trending/trend_viewer/src/resource.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"

#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

namespace TA_Base_App
{
    //
    // CItemSelectDialog
    //
    class CItemSelectDialog : public CDialog
    {
    public:

        /**
          * CItemSelectDialog
          * 
          * Constructor.
          *
          */
        CItemSelectDialog(const std::string& itemName, const std::map<std::string, TA_Base_Core::ItemInfo>& selectedItems, IItemStore& itemStore, CWnd* pParent = NULL);

        std::string getItemName() const
        {
            return m_itemName;
        }

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CItemSelectDialog)
    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CItemSelectDialog)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        afx_msg void OnFindButton();
        afx_msg void OnClearButton();
        afx_msg void OnResetButton();
        afx_msg void OnClickItemList(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        void initialiseItemList();
        void sortItemList();
        void loadItemList();
        static int CALLBACK compareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    public:

    public:

        // Dialog Data
        //{{AFX_DATA(CItemSelectDialog)
        enum { IDD = IDD_ITEM_SELECT };
        CListCtrl    m_itemList;
        CString    m_searchString;
        CString    m_itemNameString;
        //}}AFX_DATA

    private:

        std::string m_itemName;                             // Name of item selected
        IItemStore& m_itemStore;                            // Object storing all the trendable items
        std::map<std::string, TA_Base_Core::ItemInfo> m_selectedItems;    // Items that have been selected
        
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)





/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/object_tree_control/ObjectTreeCtrl.h $
  * @author Andy Parker
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * 
  * This class provides an dynamically loading object based tree control.  The items in the tree 
  * must implement the interface defined in the CObjectTreeItemClass.  The tree conttrol will use 
  * root items to load children items as they are expanded by the user.  The control can optionally
  * own the items, in which case it will clean them up when finished.  The class automatically 
  * manages the icon images.
  */
#if !defined(OBJECTTREECTRL_H_INCLUDED)
#define OBJECTTREECTRL_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\mfc_extensions\src\tree_ctrl_multi_sel\MltiTree.h"

#include <vector>
#include <map>


// ObjectTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectTreeCtrl window
class CObjectTreeItem;

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CObjectTreeCtrl : public TA_Base_Bus::CMultiTree
#else
class AFX_EXT_CLASS CObjectTreeCtrl : public TA_Base_Bus::CMultiTree
#endif
{
	friend class CObjectTreeItem;  // Permit it to access InsertItem

	// Construction
	public:

		CObjectTreeCtrl();

	// Attributes
	public:

	// Operations
	public:

		 /**
         * AddRootItem
         *
         * Adds an item to the root of the tree control.
         */ 
 		 void AddRootItem(CObjectTreeItem& item,bool deleteItem);


		// Get the currently selected item in the tree
		CObjectTreeItem* getSelectedItem();

        /// For multi selection tree controls
        /// @return selected items as vector array
        std::vector<CObjectTreeItem*>   getSelectedItems();

		// Search the root items for the item with the matching key
		CObjectTreeItem* findSelectedRootItem(std::string Key);

		CImageList& getImgList() { return m_imageList; }
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectTreeCtrl)
		//}}AFX_VIRTUAL

	// Implementation
	public:
		virtual ~CObjectTreeCtrl();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CObjectTreeCtrl)
	    afx_msg void onGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg BOOL onItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG

	private:

		 /**
         * InsertItem
         *
         * This method is used by the CObjectTreeItems to insert themselves
		 * in the tree control.
         */ 
		 HTREEITEM InsertItem(CObjectTreeItem& item,HTREEITEM parent);

		void deleteItems();
		int  getImageIndex(HICON icon);

		std::vector<CObjectTreeItem*>  m_rootItems;
		CObjectTreeItem*               m_selectedItem;

		CImageList                     m_imageList;
		std::map<HICON,int>            m_imageMap;

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(OBJECTTREECTRL_H_INCLUDED)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(OBJECTTREEITEM_H_INCLUDED)
#define OBJECTTREEITEM_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

// ObjectTreeItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectTreeItem window
class CObjectTreeCtrl;

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CObjectTreeItem : public CObject
#else
class AFX_EXT_CLASS CObjectTreeItem : public CObject
#endif
{
	public:

		CObjectTreeItem();
		virtual ~CObjectTreeItem();

		void expandItem();
		void selectItem();

        typedef enum
                {
                    IST_BY_KEY,
                    IST_BY_LABEL,
                    IST_UNDEFINED
                } 
                EItemSearchType;

		CObjectTreeItem* findSelectedItem(const std::string& item, EItemSearchType searchType = IST_BY_KEY);

		operator HTREEITEM() { return m_hItem; }
		CObjectTreeCtrl* getTreeCtrl() { return m_treeCtrl; }

		// Operations called by CObjectTreeCtrl
		virtual bool createChildItems();
		virtual const char * getItemLabelText();
		virtual bool hasChildren();
		virtual const std::string getKey();
        virtual HICON getItemImage(bool itemExpanded); 

		// Called by the TreeControl to insert the intem into the tree
		void insertItem(CObjectTreeCtrl& treeCtrl,HTREEITEM parent);


	protected:
	
		// Called by createChildItems to add new children.
		void AddChildItem(CObjectTreeItem& childItem,bool showExpanded,bool deleteItem);

        // Called to sort all the children of this item
        void SortChildren();


	private:

		// Delete any owned child items
		void deleteItems();

		std::vector<CObjectTreeItem*>  m_childItems;
		HTREEITEM                      m_hItem;
		CObjectTreeCtrl*               m_treeCtrl;

};

#endif // !defined(OBJECTTREEITEM_H_INCLUDED)

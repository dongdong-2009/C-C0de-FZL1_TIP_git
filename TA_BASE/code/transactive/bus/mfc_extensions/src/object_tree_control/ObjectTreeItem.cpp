/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/mfc_extensions/src/StdAfx.h"
#include "ObjectTreeCtrl.h"
#include "ObjectTreeItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ObjectTreeItem.cpp : implementation file
//

CObjectTreeItem::CObjectTreeItem() : m_treeCtrl(NULL), m_hItem(NULL)
{
}
		
CObjectTreeItem::~CObjectTreeItem()
{
	// Delete any items that we own
	deleteItems();
}

void CObjectTreeItem::expandItem()
{
	m_treeCtrl->Expand(m_hItem,TVE_EXPAND);
}

void CObjectTreeItem::selectItem()
{
	BOOL selected = m_treeCtrl->SelectItem(m_hItem);
}

CObjectTreeItem* CObjectTreeItem::findSelectedItem(const std::string& item, EItemSearchType searchType)
{
	// Get the first child item
	HTREEITEM hItem = m_treeCtrl->GetNextItem(m_hItem, TVGN_CHILD);

	while (hItem != NULL)
	{
		CObjectTreeItem* treeItem = reinterpret_cast<CObjectTreeItem*>(m_treeCtrl->GetItemData(hItem));
        std::string treeItemId;
         
        switch (searchType)
        {
            case IST_BY_KEY:
                treeItemId = treeItem->getKey();
                break;

            case IST_BY_LABEL:
                treeItemId = treeItem->getItemLabelText();
                break;

            default:
                ASSERT(false);
        }

        if (treeItemId == item)
        {
            return treeItem;
        }

		// Try next child item
		hItem = m_treeCtrl->GetNextItem(hItem, TVGN_NEXT);
	}

	return NULL;
}



bool CObjectTreeItem::hasChildren()
{
	// Assume we have
	return true;
}

bool CObjectTreeItem::createChildItems()
{
	// AJPTODO: Demo for now, make pure
	AddChildItem(*(new CObjectTreeItem()),false,true);
	AddChildItem(*(new CObjectTreeItem()),false,true);
	AddChildItem(*(new CObjectTreeItem()),false,true);

	return true;
}

const char* CObjectTreeItem::getItemLabelText()
{
	// AJPTODO: Demo for now, make pure
	return "Test";
}

const std::string CObjectTreeItem::getKey()
{
	// AJPTODO: Demo for now, make pure
	return "1234";
}


HICON CObjectTreeItem::getItemImage(bool itemExpanded)
{
	// No icons by default
	return NULL;
}

void CObjectTreeItem::insertItem(CObjectTreeCtrl& treeCtrl,HTREEITEM parent)
{
	// Insert ourself into tree
	m_treeCtrl = &treeCtrl;
	m_hItem = treeCtrl.InsertItem(*this,parent);
}

//virtual CIcon* CObjectTreeItem::getImage(bool itemSelected); 

//**********************
// Protected Functions *
//**********************
void CObjectTreeItem::AddChildItem(CObjectTreeItem& childItem,
								   bool showExpanded,
								   bool deleteItem)
{
	// Place into tree control
	childItem.insertItem(*m_treeCtrl,m_hItem);

	// Add to delete list 
	if (deleteItem)
	{
		m_childItems.push_back(&childItem);
	}
}


void CObjectTreeItem::SortChildren()
{
    m_treeCtrl->SortChildren(m_hItem);
}



//*********************
// Private functions  *
//*********************

void CObjectTreeItem::deleteItems()
{
	std::vector<CObjectTreeItem*>::iterator  iter;
	for (iter=m_childItems.begin();iter!=m_childItems.end();iter++)
	{
		delete *iter;
	}
	m_childItems.clear();
}



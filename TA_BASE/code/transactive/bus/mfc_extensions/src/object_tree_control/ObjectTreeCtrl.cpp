/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/object_tree_control/ObjectTreeCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ObjectTreeCtrl.cpp : implementation file
//

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

using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// CObjectTreeCtrl

CObjectTreeCtrl::CObjectTreeCtrl() : m_selectedItem(NULL)
{
    // Default to single selection (normal tree control behaviour)
    m_bMulti = FALSE;

	m_imageList.Create(16, 16, ILC_COLOR8, 0, 4);
}

CObjectTreeCtrl::~CObjectTreeCtrl()
{
	// Delete any owned root items
	deleteItems();
}


BEGIN_MESSAGE_MAP(CObjectTreeCtrl, CMultiTree)
	//{{AFX_MSG_MAP(CObjectTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, onGetdispinfo)
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, onItemExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectTreeCtrl message handlers

BOOL CObjectTreeCtrl::onItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    CWaitCursor wait;

    // If item is being expanded for the first time
    if (pNMTreeView->action == TVE_EXPAND && (pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE) != TVIS_EXPANDEDONCE)
    {
		CObjectTreeItem*  treeItem = reinterpret_cast<CObjectTreeItem*>(pNMTreeView->itemNew.lParam);
		if (treeItem)
		{
            try
            {
			    if (!treeItem->createChildItems())
			    {
				    // No children created so disable the expand button
				    this->SetItemState(pNMTreeView->itemNew.hItem,0,TVIF_CHILDREN);
			    }
            }
            catch(...)
            {
                // Caught an exception when attempting to create child items. Need to be able to stop it here...
            }
		}
	}

    wait.Restore();

	*pResult = 0;

    // Allow processing by parent class (because parent 
    // implementation won't get called unless explicitly invoked here)
    return CMultiTree::OnItemExpanding(pNMHDR, pResult);
}


void CObjectTreeCtrl::onGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//do not sort the items if the window is disabled
	//NOTE: the calling app should disable / enable the tree window
	//		when required
	if (this->IsWindowEnabled())
	{
		TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

		if ((pTVDispInfo->item.mask & TVIF_TEXT) == TVIF_TEXT)
		{
			CObjectTreeItem* treeItem = reinterpret_cast<CObjectTreeItem*>(pTVDispInfo->item.lParam);
			//TA_ASSERT(treeItem, "Tree node pointer is null");

			pTVDispInfo->item.pszText = (char*)treeItem->getItemLabelText();
		}

		if ((pTVDispInfo->item.mask & TVIF_IMAGE) == TVIF_IMAGE ||
			(pTVDispInfo->item.mask & TVIF_SELECTEDIMAGE) == TVIF_SELECTEDIMAGE)
		{
			CObjectTreeItem* treeItem = reinterpret_cast<CObjectTreeItem*>(pTVDispInfo->item.lParam);
			//TA_ASSERT(treeNode, "Tree node pointer is null");

			bool isExpanded = (pTVDispInfo->item.state & TVIS_EXPANDED) == TVIS_EXPANDED;
			pTVDispInfo->item.iImage = getImageIndex(treeItem->getItemImage(isExpanded));
			pTVDispInfo->item.iSelectedImage = pTVDispInfo->item.iImage;
		}

		if ((pTVDispInfo->item.mask & TVIF_CHILDREN) == TVIF_CHILDREN)
		{
			CObjectTreeItem* treeItem = reinterpret_cast<CObjectTreeItem*>(pTVDispInfo->item.lParam);
			//TA_ASSERT(treeNode, "Tree node pointer is null");

			pTVDispInfo->item.cChildren = treeItem->hasChildren()?1:0;
		}
	}
	
	*pResult = 0;
}


//*****************
// Public Methods *
//*****************

void CObjectTreeCtrl::AddRootItem(CObjectTreeItem& treeItem,bool deleteItem)
{
   //  Add to delete list
   if (deleteItem)
   {
		m_rootItems.push_back(&treeItem);
   }

   // Add item to tree
   treeItem.insertItem(*this,NULL);
}


CObjectTreeItem* CObjectTreeCtrl::getSelectedItem()
{
	HTREEITEM hItem = GetSelectedItem();
	if (hItem==NULL) return NULL;

	return reinterpret_cast<CObjectTreeItem*>(GetItemData(hItem));
}

CObjectTreeItem* CObjectTreeCtrl::findSelectedRootItem(std::string Key)
{
	HTREEITEM			hItem=NULL;
	CObjectTreeItem*	treeItem=NULL;

	// Get the first root item
	hItem = GetNextItem(TVI_ROOT,TVGN_ROOT);
	while (hItem!=NULL)
	{
		treeItem = reinterpret_cast<CObjectTreeItem*>(GetItemData(hItem));
		if (treeItem->getKey()==Key) break;

		// Try next root item
		hItem = GetNextItem(hItem,TVGN_NEXT);
		treeItem = NULL;
	}

	return treeItem;
}

//******************
// Private Methods *
//******************


void CObjectTreeCtrl::deleteItems()
{
	std::vector<CObjectTreeItem*>::iterator  iter;
	for (iter=m_rootItems.begin();iter!=m_rootItems.end();iter++)
	{
		delete *iter;
	}
	m_rootItems.clear();
}


int CObjectTreeCtrl::getImageIndex(HICON icon)
{
	if (icon==NULL) return -1;

	std::map<HICON,int>::iterator itor;
	int	iImage=0;

	// Check if icon is already in the image list
	itor = m_imageMap.find(icon);
	if (itor!= m_imageMap.end())
	{
		iImage = (*itor).second;
	}
	else
	{
		// Add the icon to the image list
		iImage = m_imageList.Add(icon);

		// Assign the image list to the control the first time
		SetImageList(&m_imageList, LVSIL_NORMAL);

		// Update the map
        m_imageMap.insert(std::map<HICON,int>::value_type(icon,iImage));
	}

	return iImage;
}

HTREEITEM CObjectTreeCtrl::InsertItem(CObjectTreeItem& item,HTREEITEM parent)
{
	// Add the item to the tree
    TVINSERTSTRUCT tvis;

    tvis.hParent      = parent;
	if (parent==NULL)
	    tvis.hInsertAfter = TVI_LAST;
	else
	    tvis.hInsertAfter = TVI_ROOT;
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE |TVIF_SELECTEDIMAGE; 
    tvis.item.pszText = LPSTR_TEXTCALLBACK;
    tvis.item.iImage = I_IMAGECALLBACK;
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
    tvis.item.cChildren = I_CHILDRENCALLBACK;
    tvis.item.lParam = (LPARAM)&item;

    return CMultiTree::InsertItem(&tvis);
}


std::vector<CObjectTreeItem*> CObjectTreeCtrl::getSelectedItems()
{
    std::vector<CObjectTreeItem*> result;

	HTREEITEM hItem = GetFirstSelectedItem();

	while (hItem) 
    {
        CObjectTreeItem* item = reinterpret_cast<CObjectTreeItem*>(GetItemData(hItem));

        result.push_back(item);
	    
        hItem = GetNextSelectedItem(hItem);
	}

    return result;
}
         

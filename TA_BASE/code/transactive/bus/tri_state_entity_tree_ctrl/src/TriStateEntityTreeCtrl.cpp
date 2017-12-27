/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/tri_state_entity_tree_ctrl/src/TriStateEntityTreeCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// TriStateEntityTreeCtrl.cpp : implementation file
//

#pragma warning(disable:4786)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls

#include "bus\tri_state_entity_tree_ctrl\src\TriStateEntityTreeItem.h"
#include "bus\tri_state_entity_tree_ctrl\src\TriStateEntityTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTriStateEntityTreeCtrl

CTriStateEntityTreeCtrl::CTriStateEntityTreeCtrl() : m_pOrigEntityMap(0), m_hUnCheckedIcon(0),
	m_hCheckedIcon(0), m_hIndeterminantIcon(0)
{
	// create our own image list
	if( getImgList().GetSafeHandle() != NULL )
		getImgList().DeleteImageList();

	getImgList().Create(16, 16, ILC_MASK, 0, 4);
}

CTriStateEntityTreeCtrl::~CTriStateEntityTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CTriStateEntityTreeCtrl, CObjectTreeCtrl)
	//{{AFX_MSG_MAP(CTriStateEntityTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriStateEntityTreeCtrl message handlers

void CTriStateEntityTreeCtrl::AddRootItem( CTriStateEntityTreeItem& treeItem, bool deleteItem )
{
	treeItem.setIcon( m_hUnCheckedIcon );
	CObjectTreeCtrl::AddRootItem( treeItem, deleteItem );
}

void CTriStateEntityTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if( (hItem != NULL) && (TVHT_ONITEMICON & uFlags) )
	{
		// first ensure the item is expanded
		
		if( ! (GetItemState( hItem, TVIS_EXPANDED ) & TVIS_EXPANDED) )
			Expand( hItem, TVE_EXPAND );

		// change the image
		CTriStateEntityTreeItem *pTreeItem = reinterpret_cast<CTriStateEntityTreeItem*>(GetItemData(hItem)),
			*pChildTreeItem=NULL;

		// does this node have children?
		HTREEITEM hChildItem = GetChildItem(hItem);

		// if its a child item then check or uncheck it and set the parent to the correct state

		if( hChildItem == NULL )
		{
			// toggle the check
			pTreeItem->setIcon( pTreeItem->getItemImage(false) == m_hUnCheckedIcon? m_hCheckedIcon:m_hUnCheckedIcon );

			// now see if the parent should be in the indeterminant state
			hItem = GetParentItem( hItem );
			
			//defect 3159
			if (( hItem != NULL ) && 
				( hItem != TVI_ROOT ))
			{
				HICON hIcon=NULL;
				pTreeItem = reinterpret_cast<CTriStateEntityTreeItem*>(GetItemData(hItem));
				hChildItem = GetChildItem(hItem);
				
				while( hChildItem != NULL )
				{
					pChildTreeItem = reinterpret_cast<CTriStateEntityTreeItem*>(GetItemData(hChildItem));

					if( hIcon == NULL )
						hIcon = pChildTreeItem->getItemImage(false);
					else
					{
						if( hIcon != pChildTreeItem->getItemImage(false) )
						{
							// OK, at least two child items are in a different checked state
							// so set the parent to indeterminant and break out
							pTreeItem->setIcon( m_hIndeterminantIcon );
							goto REDRAW;
						}
					}
					hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
				}
				// if we are here it means all of the children are now the same
				// so set the parent to be the same

				pTreeItem->setIcon( hIcon );
			}
		}
		else
		{
			// if its in intermedate state, uncheck it

			HICON hIcon=NULL;

			if( pTreeItem->getItemImage(false) == m_hIndeterminantIcon )
				hIcon = m_hUnCheckedIcon;

			else // otherwise toggle the state
				hIcon = pTreeItem->getItemImage(false) == m_hUnCheckedIcon? m_hCheckedIcon:m_hUnCheckedIcon;

			pTreeItem->setIcon( hIcon );

			// set all children to be the same

			while( hChildItem != NULL )
			{
				pChildTreeItem = reinterpret_cast<CTriStateEntityTreeItem*>(GetItemData(hChildItem));
				pChildTreeItem->setIcon( hIcon );
				hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
			}
		}
REDRAW:
		RedrawWindow();
		return;
	}
	CObjectTreeCtrl::OnLButtonDown(nFlags, point);
}

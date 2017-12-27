/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickFindComboButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// QuickFindComboButton.cpp : implementation file
//

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/ControlStation.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "app/system_control/control_station/src/QuickFindComboButton.h"
#include "app/system_control/control_station/src/QuickFindTreeObject.h"
#include "core/data_access_interface/entity_access/src/QuickFindEntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IQuickFindEntity.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IItem.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QuickFindComboButton

const int QuickFindComboButton::DROP_BUTTON_WIDTH       = 16;
const int QuickFindComboButton::DROPDOWN_HEIGHT         = 220;
const int QuickFindComboButton::DROPPED_WIDTH_NOT_SET   = -1;

QuickFindComboButton::QuickFindComboButton() : m_currentlySelectedItem(NULL)
{
}

QuickFindComboButton::~QuickFindComboButton()
{
}


BEGIN_MESSAGE_MAP(QuickFindComboButton, CButton)
	//{{AFX_MSG_MAP(QuickFindComboButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
    ON_NOTIFY_EX(TVN_SELCHANGED,IDC_QUICK_FIND_TREE,onSelChanged)
    ON_NOTIFY_EX(TVN_ITEMEXPANDED,IDC_QUICK_FIND_TREE,onItemExpanded)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QuickFindComboButton message handlers

void QuickFindComboButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    FUNCTION_ENTRY("DrawItem");
    //Get CDC
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Draw the frame for the "Text box" portion of the button.
	CRect rc = lpDrawItemStruct->rcItem;
	pDC->DrawEdge( rc, EDGE_SUNKEN, BF_TOP | BF_LEFT | BF_BOTTOM | BF_RIGHT );

	// Draw a white background (leaving a 2 pixel border) for an enabled button, or a grey background for a disabled button
	rc.DeflateRect(2, 2);
	static CBrush brWindow(GetSysColor(COLOR_WINDOW));
	static CBrush brBtnFace(GetSysColor(COLOR_BTNFACE));
	static CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
	if (IsWindowEnabled())
    {
        pDC->FillRect(&rc, &brWindow);
    }
	else	
    {
        pDC->FillRect(&rc, &brBtnFace);
    }

	// Determine the rectangle for the drop-button (the one with the little carret).
	CRect rcButton = rc;
	rcButton.left = rcButton.right - DROP_BUTTON_WIDTH;
	if (rcButton.left < rc.left)
    {
        rcButton.left = rc.left;
    }

	// And from the drop-button rect, can determine the size of the caption rect (i.e. everwhere that isn't in the drop-button)
	CRect rcCaption = rc;
	rcCaption.right = rcButton.left - 1;
	if (rcCaption.right < rcCaption.left) 
    {
        rcCaption.right = rcCaption.left;
    }

	// Draw the focus rectangle (the dotted rectangle around the item that currently has keyboard focus)
	rcCaption.DeflateRect(1, 1);
	if (lpDrawItemStruct->itemState & ODS_FOCUS) 
    {
        pDC->DrawFocusRect(rcCaption);
    }
	rcCaption.DeflateRect(2, 1);

	// Retreive the caption text
	CString strText;
	GetWindowText(strText);

	// Draw the caption
	pDC->SetBkColor( (IsWindowEnabled()) ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_BTNFACE) );
	COLORREF crOldColor = SetTextColor(lpDrawItemStruct->hDC, RGB(0, 0, 0));
	DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), &rcCaption, DT_SINGLELINE | DT_VCENTER );
	SetTextColor(lpDrawItemStruct->hDC, crOldColor);

	// Retrieve Drop-Button Style
	UINT uStyle = DFCS_BUTTONPUSH;
	if (lpDrawItemStruct->itemState & ODS_SELECTED) 
    {
        uStyle |= DFCS_PUSHED;
    }

	// Then draw the drop-utton
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
    {
		pDC->FrameRect(rcButton, &brBtnShadow);
		rcButton.DeflateRect(1, 1);
		pDC->FillRect(&rcButton, &brBtnFace);
		rcButton.DeflateRect(1, 3, 0, 0);
	}
    else
    {
		pDC->FrameRect(rcButton, &brBtnFace);
		rcButton.DeflateRect(1, 1, 0, 0);
		DrawFrameControl(lpDrawItemStruct->hDC, &rcButton, DFC_BUTTON, uStyle);
	}

	// Now draw the drop-button arrow
	static CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	static CPen penBtnShadow(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	CPen* ppenOld;
	if (IsWindowEnabled())
    {
			ppenOld = pDC->SelectObject(&penBlack);
    }
	else
    {
        ppenOld = pDC->SelectObject(&penBtnShadow);
    }
	for(long i=0; i<4; i++)
    {
		pDC->MoveTo( rcButton.left + 3 + i, rcButton.top + rcButton.Height()/2 - 2 + i);
		pDC->LineTo( rcButton.left + 3 + 7 - i, rcButton.top + rcButton.Height()/2 - 2 + i);
		if (!IsWindowEnabled())
        {
            pDC->SetPixel(rcButton.left + 3 + 7 - i, rcButton.top + rcButton.Height()/2 - 2 + i + 1, 0xFFFFFF);
        }
	}

    // Reset the pen before ending the method.
	pDC->SelectObject(ppenOld);	
    FUNCTION_EXIT;
}

void QuickFindComboButton::OnLButtonDown(UINT nFlags, CPoint point) 
{	
    FUNCTION_ENTRY("OnLButtonDown");
	CButton::OnLButtonDown(nFlags, point);

    showDropWindow();
    FUNCTION_EXIT;
}

void QuickFindComboButton::showDropWindow()
{
    FUNCTION_ENTRY("showDropWindow");
    // Create the tree if it doesn't exist yet.
	if (NULL == m_dropTree.GetSafeHwnd()) 
    {
        createDropWindow();
    }

	if (NULL == m_dropTree.GetSafeHwnd())
    {
        FUNCTION_EXIT;
        return;
    }

	// Get the como box's bounding rect
	CRect rc;
	GetWindowRect(&rc);

	// And position the drop tree.
	m_dropTree.MoveWindow(rc.left, rc.bottom, rc.Width(), DROPDOWN_HEIGHT);

    // Then toggle the window visibility
    if (m_dropTree.IsWindowVisible() )
    {
        hideDropWindow();
    }
    else
    {
        // Set the currently selected item (the tree is not allowed to close with an inavlid item selected)
        m_currentlySelectedItem = m_dropTree.GetSelectedItem();

        // Display the winow in "drop-down" style
        ::AnimateWindow(m_dropTree.GetSafeHwnd(),200,AW_SLIDE | AW_VER_POSITIVE);
    }
    FUNCTION_EXIT;
}

void QuickFindComboButton::createDropWindow()
{
    FUNCTION_ENTRY("createDropWindow");
	// Ensure the tree control has NOT already been created
	if (NULL != m_dropTree.GetSafeHwnd())
    {
        return;
    }

	// Create the tree control
	m_dropTree.CreateEx(	WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,
							WC_TREEVIEW,
							NULL,
							WS_CHILD | WS_BORDER |
							WS_CLIPSIBLINGS | WS_OVERLAPPED |
							TVS_HASBUTTONS | TVS_HASLINES | TVS_DISABLEDRAGDROP |
							TVS_SHOWSELALWAYS | TVS_FULLROWSELECT | TVS_LINESATROOT,
							CRect(0, 0, 0, 0),
							this,
							IDC_QUICK_FIND_TREE,
							NULL  );

    // Set the desktop window as the parent (this may seem strange, but it is the way the list-box within a combo-box
    // actually works.
    // Also, note that the parent window was set to "this" during creation of the control, and is changed now.
    // This is due to a quirk in MFC that allows us to capture the tree control messages (WM_NOTIFY etc) as long
    // as we are the original parent of the control. If the control was *created* with the desktop window as parent
    // we would not receive those messages.
    m_dropTree.SetParent(GetDesktopWindow());

	// Move the drop window to be topmost (over all other windows)
	m_dropTree.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    // Add all the root items
    std::vector<TA_Base_Core::ILocation*> treeItems = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
    for (unsigned int i = 0;i < treeItems.size();i++)
    {        
        std::string name = treeItems[i]->getName();
        QuickFindTreeObject::ObjectTypeInfo typeInfo;
        typeInfo.type = QuickFindTreeObject::REGION;
        QuickFindTreeObject* treeItem = new QuickFindTreeObject(*treeItems[i],name,NULL,typeInfo);
        m_dropTree.AddRootItem(*treeItem,true);
    }

    FUNCTION_EXIT;
}


void QuickFindComboButton::loadEntities()
{
    FUNCTION_ENTRY("loadEntities");
    try
    {
        //m_rootEntities = TA_Base_Core::QuickFindEntityAccessFactory::getInstance().getAllQuickFindEntities();

    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "General Exception", 
            "Exception caught while attempting to load Quick Find root entites. There will be no entities.");
    }
    FUNCTION_EXIT;
}

void QuickFindComboButton::hideDropWindow()
{
    FUNCTION_ENTRY("hideDropWindow");

    if (m_dropTree.GetSafeHwnd() != NULL && m_dropTree.IsWindowVisible())
    {
        // Only need to do the following if something is actually selected
        QuickFindTreeObject* selectedItem = dynamic_cast<QuickFindTreeObject*>(m_dropTree.getSelectedItem());
        if (NULL != selectedItem && selectedItem->getObjectType() == QuickFindTreeObject::ENTITY)
        {
            SetWindowText(selectedItem->getItemLabelText());
        }
        else
        {
            // If not an entity, then reselect the last valid selected entity
            if (NULL != m_currentlySelectedItem)
            {
                m_dropTree.SelectItem(m_currentlySelectedItem);
            }
            else
            {
                // Finally, if there is NO currently, valid selected entity, then clear the selection
                m_dropTree.SelectItem(NULL);
                SetWindowText("");
            }
        }

        // Message the control station bar so it can adjust the status of the various tool-buttons
        // associated with this combo box. To do that it will need to know WHAT was selected, so extract the
        // entity data
        TA_Base_Core::IEntityData* entityData = NULL;
        if (selectedItem != NULL)
        {
            entityData = dynamic_cast<TA_Base_Core::IEntityData*>(selectedItem->getItemData());
        }

        GetParent()->PostMessage(WM_QUICKFIND_ITEM_SELECTED,reinterpret_cast<WPARAM>(entityData));

        m_dropTree.ShowWindow(SW_HIDE);
    }

    FUNCTION_EXIT;
}

LRESULT QuickFindComboButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    // This method can be used to catch key-presses while the drop-down window has focus
    // Any keypress that returns NULL is intercepted, and no further processing is carried out.
	switch (message)
    {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch ((int) wParam)
        {
			case VK_F4:
                {
				    showDropWindow();
				    return NULL;	// Toggle drop-down visibility on F4
                }
			case VK_DOWN:
			case VK_UP:
                {
				    if (message == WM_SYSKEYDOWN) // If Alt was also pressed toggle drop-down visibility
                    {
                        showDropWindow();	
                    }
                    return NULL;                  // Even if Alt wasn't pressed, don't want to pass on up/down
                }
			case VK_SPACE:
				return NULL;	// Disable space completely
            case VK_ESCAPE:
                {
                    m_dropTree.ShowWindow(SW_HIDE); // Explicitly call ShowWindow here as don't want to actually
                                                    // change selection
                    m_dropTree.Select(m_currentlySelectedItem,TVGN_CARET | TVGN_FIRSTVISIBLE);
                    return NULL;      // On escape, hide the window
                }
            case VK_RETURN:
                {
                    hideDropWindow();
                    return NULL; // On enter, select the currently selected item, hide the drop window, and update the
                                 // display text
                }
            default:
                {/*Do nothing...*/};
		}
    default:
        {/*Do nothing...*/};
	}		

	return CButton::WindowProc(message, wParam, lParam);
}

void QuickFindComboButton::OnKillFocus(CWnd* pNewWnd) 
{
    FUNCTION_ENTRY("OnKillFocus");
	CButton::OnKillFocus(pNewWnd);
	
    hideDropWindow();

    FUNCTION_EXIT;
}

BOOL QuickFindComboButton::PreTranslateMessage(MSG* pMsg) 
{
    FUNCTION_ENTRY("PreTranslateMessage");
	// Intercept Keys => send them to tree
	if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
    {
        if(NULL != m_dropTree.GetSafeHwnd() && m_dropTree.IsWindowVisible())
        {
			m_dropTree.SendMessage( pMsg->message, pMsg->wParam, pMsg->lParam);
            FUNCTION_EXIT;
			return FALSE;
		}
    }
	
    FUNCTION_EXIT;
	return CButton::PreTranslateMessage(pMsg);
}


BOOL QuickFindComboButton::onSelChanged( UINT id, NMHDR * pNotifyStruct, LRESULT * result )
{
    LPNMTREEVIEW tree = (LPNMTREEVIEW)pNotifyStruct;
    
    // Get the tree object of the newly selected item
    QuickFindTreeObject* item = dynamic_cast<QuickFindTreeObject*>(m_dropTree.getSelectedItem());

    // This first test determines if the selection change was a result of user action (and UNKNOWN result means
    // that it was a code-driven selection change)
    if (tree->action != TVC_UNKNOWN)
    {
        // Test if the newly selected object is or is NOT an entity. If it is, then just select it as normal 
        if (item->getObjectType() == QuickFindTreeObject::ENTITY)
        {
            // This execution block will only be reached by selecting an entity with with mouse of keyboard, 
            // so progress as normal, but hide the drop window.
            hideDropWindow();
        }
    }

    return FALSE;
}

BOOL QuickFindComboButton::onItemExpanded( UINT id, NMHDR * pNotifyStruct, LRESULT * result )
{
    LPNMTREEVIEW tree = (LPNMTREEVIEW)pNotifyStruct;

    // Sort the children (NOTE: The TVIS_EXPANDEDONCE notification does not get set correctly for TVN_ITEMEXPANDED
    // messages, so cannot rely on it to only apply the sort the first time the tree is expanded. It should not be
    // an issue however, as CTreeCtrl can sort 15000 items in a split second)
    m_dropTree.SortChildren(tree->itemNew.hItem);

    return FALSE;
}

TA_Base_Core::IEntityData* QuickFindComboButton::getCurrentlySelectedItem()
{
    QuickFindTreeObject* selectedItem = dynamic_cast<QuickFindTreeObject*>(m_dropTree.getSelectedItem());

    if (NULL != selectedItem)
    {
        // If can cast to IEntityData because other methods make sure that only entities can be selected.
        return dynamic_cast<TA_Base_Core::IEntityData*>(selectedItem->getItemData());
    }

    return NULL;
}

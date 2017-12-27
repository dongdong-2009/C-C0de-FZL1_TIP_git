/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ButtonListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends a list control. It is a list control where each of the icons
  * is a button. Each of those buttons is linked to an application that can be
  * launched by the user.
  * 
  * This list control determines which applications should be showing and adds them
  * to the list. It also performs the launching of the application when clicked. It
  * contains a lot of the drawing code for ensuring the buttons look like real buttons.
  */


#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/ButtonListCtrl.h"
#include "app/system_control/control_station/src/Application.h"
#include "app/system_control/control_station/src/resource.h"
#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"

#include <vector>

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IGui.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_MinMax.h"

using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is the maximum number of items that can fit in the list control before it
// needs to be expanded,
const int CButtonListCtrl::MAXIMUM_APPLICATIONS = 8;
const int CButtonListCtrl::ICON_SPACING = 70;
const int CButtonListCtrl::ICON_SIZE = 40;
const int CButtonListCtrl::NUM_COLUMN = 2;
const int CButtonListCtrl::NUM_ROW = 4;


/////////////////////////////////////////////////////////////////////////////
// CButtonListCtrl

CButtonListCtrl::CButtonListCtrl()
{
}


CButtonListCtrl::~CButtonListCtrl()
{
}


BEGIN_MESSAGE_MAP(CButtonListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CButtonListCtrl)
	ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetInfoTipLaunchList)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonListCtrl message handlers

void CButtonListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Set the item to be flat!
	
    // Find out what item is selected 
	int index(HitTest(point));

	// There was no item selected - leave
	if (index < 0)
    {
		return;
    }

	// Check to make sure the security levels are ok
	if (! getApplication(index)->isAccessible())
    {
		return;
    }

	// Find the area of the rect to be changed
	CRect boundingRect;	
	GetItemRect(index, boundingRect, LVIR_BOUNDS);

	// get the device context.
	// Make the button look like it has been pressed
    GetDC()->Draw3dRect(boundingRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
}


void CButtonListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnLButtonUp");

	// Set the item to be flat!
	// Find out what item is selected 
	int index(HitTest(point));

	// There was no item selected - leave
	if (index < 0)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "No application was selected for launch");
        LOG( SourceInfo, DebugUtil::FunctionExit, "OnLButtonUp");
		return;
    }

	// Check to make sure the security levels are ok
	if (! getApplication(index)->isAccessible())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Application selected for launch should not be accessible");
        LOG( SourceInfo, DebugUtil::FunctionExit, "OnLButtonUp");
		return;
    }

	// Find the bounding rectangle
	CRect boundingRect;
	GetItemRect(index, boundingRect, LVIR_BOUNDS);

    // Force a redraw so the button is displayed correctly (ie no longer pressed)
    RedrawWindow(boundingRect);

    // This may take a while so enable the wait cursor
	BeginWaitCursor();

    // Now launch the actual application
    unsigned long appType = getApplication(index)->getGuiDatabaseEntry().getKey();
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launching application with key %lu",appType);

    IProcessManager* pm = SessionManager::getInstance().getProcessManager();
    if (pm != NULL)
    {
        try
        {
            RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
                TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
                TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, 
                point.x);
            pm->launchApplication(
                appType, 
                "", 
                TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
                TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
                TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, 
                boundaryRect);

            // Done - disable the wait cursor
            EndWaitCursor();
            LOG( SourceInfo, DebugUtil::FunctionExit, "OnLButtonUp");
            return;
        }
        catch ( const TA_Base_Core::ApplicationException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Could not launch the GUI application with key %lu", appType );
        }

    }

    // If we get to here an error has occurred
	EndWaitCursor();

    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << getApplication(index)->getGuiDatabaseEntry().getFullName();
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
    LOG( SourceInfo, DebugUtil::FunctionExit, "OnLButtonUp");
}


void CButtonListCtrl::PreSubclassWindow() 
{
	// Setup the launch App list
	// ******************************************
	
	// Set a default icon for the spacing
	CImageList* imageList = new CImageList();
	imageList->Create(ICON_SIZE,ICON_SIZE, TRUE, NUM_COLUMN, NUM_ROW);
	imageList->Add(AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	// Make the image list look nice
	SetImageList(imageList,LVSIL_NORMAL);
	SetBkColor(::GetSysColor(COLOR_3DFACE));
	SetExtendedStyle(LVS_EX_FLATSB|0x00008000|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP);
	SetHoverTime(1);
	SetTextBkColor(::GetSysColor(COLOR_3DFACE));
	SetIconSpacing(ICON_SPACING,ICON_SPACING);
	
	CListCtrl::PreSubclassWindow();

	// Load the apps
	loadApps();
}


Application* CButtonListCtrl::getApplication(int index)
{
	return reinterpret_cast<Application*>(GetItemData(index));
}

void CButtonListCtrl::OnGetInfoTipLaunchList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVGETINFOTIP * pNMLVGETINFOTIP = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	// Display the info tip
    Application* app = getApplication(pNMLVGETINFOTIP->iItem);
	CString tip( app->getToolTip().c_str() );
	char buf[1024];
	strcpy(buf, tip.GetBuffer(0));
	pNMLVGETINFOTIP->pszText = buf;
	pNMLVGETINFOTIP->cchTextMax = tip.GetLength();
}


void CButtonListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

	switch(pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;          // ask for subitem notifications.
		break;

		case CDDS_ITEMPREPAINT:
		{
			// Find out what item is selected 
			int index = pLVCD->nmcd.dwItemSpec;

			// Do all the drawing ourselves!
			// Find the bounding rectangles
			CRect boundingRect;
			GetItemRect(index, boundingRect, LVIR_BOUNDS);
            
            CRect iconRect;
			GetItemRect(index, iconRect, LVIR_ICON);
            
            CRect textRect;
			GetItemRect(index, textRect, LVIR_LABEL);

			// Get the app object so we can use the icons and such
			Application* pApp = getApplication(index);

			// get the device context.
			CDC *pDC= CDC::FromHandle(pLVCD->nmcd.hdc);

			if ((pLVCD->nmcd.uItemState & CDIS_HOT) == CDIS_HOT)
			{
				// If the app isn't accessible just ignore the hot
				if (pApp->isAccessible())
				{
					// Clean the slate
                    pDC->FillSolidRect(&boundingRect, offsetColor(::GetSysColor(COLOR_BTNFACE),30));
    
					// Make the button a bit more impressive!
					pDC->DrawEdge(&boundingRect, BDR_RAISEDINNER, BF_RECT);

					// Center the textRect for drawing of the text
					pDC->DrawText(pApp->getDisplayName().c_str(), textRect, DT_CENTER|DT_BOTTOM);

					// Fiddle with the iconRect so it draws ok
					iconRect.DeflateRect(8,2);
					pDC->DrawIcon(iconRect.TopLeft(), pApp->getHotIcon());
				}
				else
				{
					HCURSOR hCur = LoadCursor(NULL, IDC_ARROW);
					SetCursor(hCur);
					COLORREF crOldColor = ::SetTextColor(pDC->m_hDC, RGB(255,0,0));

					pDC->DrawText("X", &boundingRect, DT_LEFT|DT_TOP);
					
					::SetTextColor(pDC->m_hDC, crOldColor);
				}

				*pResult = CDRF_SKIPDEFAULT;
			}
			else // Drawing for cold icon
			{
				// Clean the slate
				pDC->FillSolidRect(&boundingRect, ::GetSysColor(COLOR_3DFACE));

				// Make the button a bit more impressive!	
				COLORREF crOldColor = ::SetTextColor(pDC->m_hDC, RGB(45,45,45));

				// Center the textRect for drawing of the text
				pDC->DrawText(pApp->getDisplayName().c_str(), textRect, DT_CENTER|DT_BOTTOM);

				::SetTextColor(pDC->m_hDC, crOldColor);

				// Fiddle with the iconRect so it draws ok
				iconRect.DeflateRect(8,2);
				pDC->DrawIcon(iconRect.TopLeft(), pApp->getColdIcon());

				*pResult = CDRF_SKIPDEFAULT;
			}	
		}
		break;

		default:// it wasn't a notification that was interesting to us.
			*pResult = CDRF_DODEFAULT;
			break;
	}
}


void CButtonListCtrl::insertAppItem(Application *pItem)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "insertAppItem");

    // IF we have reached the maximum number of items that can fit in the
    // list control then a scroll bar will need to be added.
	if (GetItemCount() == MAXIMUM_APPLICATIONS)
	{
		// Set the spacing
		SetIconSpacing(ICON_SPACING,ICON_SPACING);
		std::vector<Application*> apps;
		while (GetItemCount() != 0)
		{
			apps.push_back(getApplication(0));
			DeleteItem(0);
		}
		for(unsigned int i = 0; i < apps.size(); i++)
		{
			SetItemData(InsertItem(0,"", 0), (DWORD) apps[i]);
		}
	}

	SetItemData(InsertItem(0, "" ,0), (DWORD)pItem);

    LOG( SourceInfo, DebugUtil::FunctionExit, "insertAppItem");
}


void CButtonListCtrl::loadApps()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadApps");
    DeleteAllItems();

    IProcessManager* pm = SessionManager::getInstance().getProcessManager();
    
    if (pm == NULL)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "ProcessManager was NULL so we could not retrieve the list of applications");
        LOG( SourceInfo, DebugUtil::FunctionExit, "loadApps");
        return;
    }
    
    IProcessManager::ApplicationMap& apps = pm->getApplicationObjects();
    
	// Insert the apps into the list ctrl
	for (IProcessManager::ApplicationMap::iterator appIter = apps.begin();
         appIter != apps.end();
         ++appIter)
	{
        IF_LOCAL_TEST
        {
            insertAppItem(appIter->second);
        }

		if ( (appIter->second->isIconVisible()) && (appIter->second->getHotIcon() != NULL) && (appIter->second->isAccessible()))
        {
            insertAppItem(appIter->second);
        }
	}

    SortItems(compareApp, (LPARAM) this); // TD 513 - do this so that the icons won't reshuffle

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadApps");
}


void CButtonListCtrl::reset()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "reset");
    loadApps();
    LOG( SourceInfo, DebugUtil::FunctionExit, "reset");
}


BOOL CButtonListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
}


DWORD CButtonListCtrl::offsetColor(DWORD color, short shOffset)
{
	if (shOffset < -255 || shOffset > 255)
    {
        return color;
    }

	// Get RGB components of specified color
	BYTE byRed = GetRValue(color);
	BYTE byGreen = GetGValue(color);
	BYTE byBlue = GetBValue(color);

	short	shOffsetR = shOffset;
	short	shOffsetG = shOffset;
	short	shOffsetB = shOffset;

    // Calculate max. allowed real offset
	if (shOffset > 0)
	{
		if (byRed + shOffset > 255)
        {
            shOffsetR = 255 - byRed;
        }
		if (byGreen + shOffset > 255)
        {
            shOffsetG = 255 - byGreen;
        }
		if (byBlue + shOffset > 255)
        {
            shOffsetB = 255 - byBlue;
        }

		shOffset = ta_min(ta_min(shOffsetR, shOffsetG), shOffsetB);
	} // if
	else
	{
		if (byRed + shOffset < 0)
        {
            shOffsetR = -byRed;
        }
		if (byGreen + shOffset < 0)
        {
            shOffsetG = -byGreen;
        }
		if (byBlue + shOffset < 0)
        {
            shOffsetB = -byBlue;
        }

		shOffset = ta_max(ta_max(shOffsetR, shOffsetG), shOffsetB);
	} // else

	// Set new color
	return RGB(byRed + shOffset, byGreen + shOffset, byBlue + shOffset);

} // End of OffsetColor


int CALLBACK CButtonListCtrl::compareApp(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    // lParamSort contains a pointer to the list view control.
    // The lParam of an item is just its index.
    CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
    CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
    CString    strItem2 = pListCtrl->GetItemText(lParam2, 0);

    return strcmp(strItem2, strItem1);
}


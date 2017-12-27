/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/EditableListCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// EditableListCtrl.cpp : implementation file
//

#include "..\stdafx.h"
#include "EditableListCtrl.h"
#include "ListCellEdit.h"
#include "ListCellMaskedEdit.h"
#include "ListCellComboBox.h"
#include "ListCellCheckBox.h"
#include "ListCellDateTime.h"
#include "ListCellButton.h"
#include "bus\mfc_extensions\src\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl

CEditableListCtrl::CEditableListCtrl() 
	: m_cellManager(NULL),m_cellEditCtrl(NULL),m_singleClick(false), m_noCheckIconIndex(-1), m_checkIconIndex(-1), m_numImages(9), m_imageList(NULL), m_buttonProcessor(NULL), m_comboxCellDataSortable(TRUE)
{
}

CEditableListCtrl::CEditableListCtrl(bool singleClick, int numImages) 
	: m_cellManager(NULL),m_cellEditCtrl(NULL),m_singleClick(singleClick),m_noCheckIconIndex(-1), m_checkIconIndex(-1),m_numImages(numImages), m_imageList(NULL), m_buttonProcessor(NULL), m_comboxCellDataSortable(TRUE)
{
}

CEditableListCtrl::CEditableListCtrl(ICellManager *cellManager, bool singleClick, int numImages) 
    : m_cellManager(cellManager), m_cellEditCtrl(NULL),m_singleClick(singleClick),m_noCheckIconIndex(-1), m_checkIconIndex(-1),m_numImages(numImages), m_imageList(NULL), m_buttonProcessor(NULL), m_comboxCellDataSortable(TRUE)
{
    ASSERT(m_cellManager != NULL);	
}

CEditableListCtrl::~CEditableListCtrl()
{
    if (m_cellEditCtrl != NULL)
    {
        delete m_cellEditCtrl;
        m_cellEditCtrl = NULL;
    }

    if (m_imageList != NULL)
    {
        delete m_imageList;
        m_imageList = NULL;
    }
}

void CEditableListCtrl::SetManager(ICellManager *cellManager)
{
    ASSERT(cellManager != NULL);
	m_cellManager = cellManager;
}

BEGIN_MESSAGE_MAP(CEditableListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditableListCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl message handlers

void CEditableListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	if (!m_singleClick)
	{
		startEdit(); 
	}

	*pResult = 1; // suppress the default item edit box  
}

void CEditableListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CEditableListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	if (!m_singleClick)
	{
		*pResult = 0;
		return;
	}

	*pResult = 1; // suppress the default click action

	startEdit();    
}

void CEditableListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if (GetFocus() != this)
        SetFocus();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditableListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if (GetFocus() != this)
        SetFocus();
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CEditableListCtrl::loadCheckBoxIcons()
{
#ifdef _DEBUG
    HMODULE module = GetModuleHandle("TA_MFCExtensions_d.dll");
#else
    HMODULE module = GetModuleHandle("TA_MFCExtensions.dll");
#endif
  
	// Load status icons
	m_noCheckIcon			= LoadIcon(module, MAKEINTRESOURCE(IDI_NOCHECK));
	m_checkIcon				= LoadIcon(module, MAKEINTRESOURCE(IDI_CHECK));
	m_disabledNoCheckIcon	= LoadIcon(module, MAKEINTRESOURCE(IDI_DISABLEDNOCHECK));
	m_disabledCheckIcon		= LoadIcon(module, MAKEINTRESOURCE(IDI_DISABLEDCHECK));

	m_noCheckIconIndex			= addIcon(m_noCheckIcon);
	m_checkIconIndex			= addIcon(m_checkIcon);
	m_disabledNoCheckIconIndex	= addIcon(m_disabledNoCheckIcon);
	m_disabledCheckIconIndex	= addIcon(m_disabledCheckIcon);

}

void CEditableListCtrl::showCheckIcon(int nItem, int nSubItem, bool checked, bool enabled)
{
	LVITEM   item;

	if (m_noCheckIconIndex == -1 || m_checkIconIndex == -1)
	{
		loadCheckBoxIcons();
	}

	ASSERT (m_noCheckIconIndex != -1 && m_checkIconIndex != -1);

	

	item.mask		= LVIF_TEXT | LVIF_IMAGE;
	item.iItem		= nItem;
	item.iSubItem	= nSubItem;
	item.pszText	= "";

	if (enabled)
	{
		item.iImage		= checked ? m_checkIconIndex : m_noCheckIconIndex;
	}
	else
	{
		item.iImage		= checked ? m_disabledCheckIconIndex : m_disabledNoCheckIconIndex;
	}

	SetItem(&item);
}

void CEditableListCtrl::destroyCheckIcon(int nItem, int nSubItem)
{
	LVITEM   item;

	item.mask		= LVIF_TEXT | LVIF_IMAGE;
	item.iItem		= nItem;
	item.iSubItem	= nSubItem;
	item.pszText	= "";
	item.iImage		= -1;

	SetItem(&item);
}

bool CEditableListCtrl::isItemChecked(int nItem, int nSubItem)
{
	LVITEM   item;

	item.mask		= LVIF_IMAGE;
	item.iItem		= nItem;
	item.iSubItem	= nSubItem;
	
	GetItem(&item);
	
	return (item.iImage == m_checkIconIndex || item.iImage == m_disabledCheckIconIndex);
}

void CEditableListCtrl::startEdit()
{
	if (m_cellManager == NULL)  // No cell manager so therefore don't allow editing
		return;

	// Get cell coordinates
    LVHITTESTINFO hitTestInfo;
	::GetCursorPos(&hitTestInfo.pt);
	ScreenToClient(&hitTestInfo.pt);
	SubItemHitTest(&hitTestInfo);

    int item = hitTestInfo.iItem;
    int subItem = hitTestInfo.iSubItem;

	if (!(hitTestInfo.flags & LVHT_ONITEM))
        return;

	if (EnsureVisible(item, FALSE) == FALSE)
		return;

    ICellManager::CellType cellType = m_cellManager->GetCellType(this, item, subItem);	
    if (cellType == ICellManager::CellType::CT_NOTEDITABLE)
        return;

    if (m_cellEditCtrl != NULL)
    {
        delete m_cellEditCtrl;
        m_cellEditCtrl = NULL;
    }

	CRect cellRect;
	GetSubItemRect(item, subItem, LVIR_LABEL, cellRect);

    switch (cellType)
    {
        case ICellManager::CellType::CT_TEXT:
        case ICellManager::CellType::CT_NUMBER:
        {
            CListCellEdit *edit = new CListCellEdit(this, m_cellManager, cellRect, item, subItem, GetItemText(item, subItem));
            m_cellEditCtrl = edit;

	        edit->SetNumeric(cellType == ICellManager::CellType::CT_NUMBER);

            unsigned long characterLimit = m_cellManager->GetCellMaximumCharacters(this, item, subItem);
            if (characterLimit != 0)
            {
                edit->SetLimitText(characterLimit);
            }

            break;
        }

        case ICellManager::CellType::CT_MASK:
        {
            CString mask = m_cellManager->GetCellMask(this, item, subItem);
            char promptChar = m_cellManager->GetCellMaskPromptChar(this, item, subItem);

            CListCellMaskedEdit *maskedEdit = new CListCellMaskedEdit(this, m_cellManager, cellRect, item, subItem);
            m_cellEditCtrl = maskedEdit;

            maskedEdit->Initialise(promptChar, mask, GetItemText(item, subItem));

            break;
        }

        case ICellManager::CellType::CT_SELECTION:
        {
            ICellManager::CellData cellData = m_cellManager->GetCellData(this, item, subItem);
            CListCellComboBox *combo = new CListCellComboBox(this, m_cellManager, cellRect, item, subItem, GetItemText(item, subItem), cellData, GetComboxCellDataSortable());
            m_cellEditCtrl = combo;

            break;
        }

        case ICellManager::CellType::CT_DECISION:
        {
            CListCellCheckBox *check = new CListCellCheckBox(this, m_cellManager, cellRect, item, subItem, GetItemText(item, subItem));
            m_cellEditCtrl = check;

            break;
        }

		case ICellManager::CellType::CT_CHECKBOX:
		{		
            bool checkOn = !isItemChecked(item, subItem);
			CString str = checkOn? "Yes":"No";

			if (m_cellManager->UpdateCellValue(this, item, subItem, str))
            {
                showCheckIcon(item, subItem, checkOn, true);
            }

			break;
        }

		case ICellManager::CellType::CT_CHECKBOX_DISABLED:
		{
            showCheckIcon(item, subItem, isItemChecked(item, subItem), false);
			break;
        }
        case ICellManager::CellType::CT_DATE:
        case ICellManager::CellType::CT_TIME:
        {
            ICellManager::CellData cellData = m_cellManager->GetCellData(this, item, subItem);
            CListCellDateTime *pCtrl = new CListCellDateTime(this, m_cellManager, cellRect, item,
				subItem, GetItemText(item, subItem), cellType);
            m_cellEditCtrl = pCtrl;
            break;
        }
        case ICellManager::CellType::CT_BUTTON:
		{
            ICellManager::CellData cellData = m_cellManager->GetCellData(this, item, subItem);
            CListCellButton *button = new CListCellButton(this, m_cellManager, cellRect, item, subItem, GetItemText(item, subItem), cellData, "...", 20, m_buttonProcessor);
            m_cellEditCtrl = button;
			break;
        }
        default:
            break;
    }
}

int CEditableListCtrl::addIcon(HICON icon)
{
    CImageList *imageList = GetImageList(LVSIL_SMALL);

	if (NULL == imageList)
	{
        m_imageList = new CImageList;
		m_imageList->Create(16,16,ILC_COLOR8,0,m_numImages);
		SetImageList(m_imageList,LVSIL_SMALL);
        imageList = m_imageList;
	}

	int iconIndex = imageList->GetImageCount();

	if (iconIndex < m_numImages)
	{
		imageList->Add(icon);
	}
	else
	{
		iconIndex = -1;
	}

	return iconIndex;
}


void CEditableListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (this->m_cellEditCtrl != NULL)
	{
		//TD15814, delete here will cause a runtime error on limit/meter tab of IP
		//delete m_cellEditCtrl;
		m_cellEditCtrl->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}

void CEditableListCtrl::SetComboxCellDataSortable(BOOL sortable)
{
	m_comboxCellDataSortable = sortable;
}

BOOL CEditableListCtrl::GetComboxCellDataSortable()
{
	return m_comboxCellDataSortable;
}

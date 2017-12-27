/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/multi_dialog/MultiDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// MultiDialog.cpp: implementation of the CMultiDialog class.
//
// Version 1.0
//
// Written by Martin Randall [18/6/2001]
//////////////////////////////////////////////////////////////////////

#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/multi_dialog/MultiDialog.h"
#include "bus\mfc_extensions\src\bitmap_and_text_push_button\FooButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*============================================================*\
	CMultiDialog::AddDialog
  ============================================================
	Author : Martin Randall (18/06/2001)

	Description :	Add a dialog to this dialog

	Parameters :
			UINT id			-	ID of the child dialog to add
			UINT idMarker	-	ID of the marker control to
								use as a position marker
	Note :	The marker control is hidden
\*============================================================*/
void CMultiDialog::AddDialog(UINT id, UINT idMarker )
{
	// Check if the dialog is already showing
	if( currentDlgId == id )
	{
		return;
	}
	
	// Locate the marker control
	CWnd *pMarker = GetDlgItem( idMarker );
	if ( !pMarker ) return;

	CRect rcMarker;
	pMarker->GetWindowRect( rcMarker );
	ScreenToClient( rcMarker );
	pMarker->ShowWindow( SW_HIDE );

	
	// Remove the previous child dialog
	if( childExists )
	{
		WndListIter scan = m_ChildList.begin();
		WndListIter tmpIter;
		while( scan != m_ChildList.end() )
		{
			tmpIter = scan;
			++scan;
		
			delete *tmpIter;
			m_ChildList.erase( tmpIter );
		}
	}

	// Locate the resource 
	HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE( id ), RT_DIALOG );
	ASSERT( hInst );
	HRSRC hRes = ::FindResource( hInst, MAKEINTRESOURCE( id ), RT_DIALOG );
	ASSERT( hRes );
	
	// Load it into memory
	HGLOBAL hTemplate = ::LoadResource( hInst, hRes );
	ASSERT( hTemplate);
	
	// Get a pointer to it
	DLGTEMPLATE *pTemplate = (DLGTEMPLATE*)::LockResource( hTemplate );
	ASSERT( pTemplate );
	
	int iItems = pTemplate->cdit;
	WORD nID;
	CString strName;
	
	// Point to menu array
	WORD *pArrayScan = (WORD *)(pTemplate+1);

	// All these should be blank for a child dialog
	pArrayScan = ReadTemplateArray( pArrayScan, nID, strName );		// Read menu ID
	pArrayScan = ReadTemplateArray( pArrayScan, nID, strName );		// Read window class
	pArrayScan = ReadTemplateArray( pArrayScan, nID, strName );		// Read title array

	if ( pTemplate->style & DS_SETFONT ) {
		int iFontSize;
		CString strFontName("");
		iFontSize = (int) *pArrayScan++;
		//localization. cuilike++
		//while ( *pArrayScan )
		//{
		//	strFontName += (char)*pArrayScan++;
		//}

		while ( *pArrayScan )
		{
			wchar_t wch = *pArrayScan++;
			char ch[3] = {0};
			WideCharToMultiByte(CP_ACP, 0, &wch, 1, ch, 3, NULL ,NULL);
			strFontName += ch;
		}
		//localization. ++cuilike
		pArrayScan++;

		// Create the font
		m_pCurrentFont = new CFont;
		m_pCurrentFont->CreatePointFont( iFontSize * 10, strFontName );
		m_FontList.push_back( m_pCurrentFont );
	} else
		m_pCurrentFont = 0;

	// Make sure we are on an even word boundary
	int iOffset = (pArrayScan - ((WORD*)pTemplate));
	if ( iOffset % 2 )
		pArrayScan++;
	
	DLGITEMTEMPLATE *pItemTemplate = (DLGITEMTEMPLATE*)(pArrayScan), *pNextTemplate;
	while ( iItems-- ) {
		pNextTemplate = AddDialogItem( pItemTemplate, rcMarker );

		// Make sure that we are on a DWORD boundary
		iOffset = (int)(((WORD*)pNextTemplate) - ((WORD*) pItemTemplate)) % 2;
		if ( iOffset) {
			int iPad = 2 - iOffset;
			pNextTemplate = (DLGITEMTEMPLATE*)(((WORD*)pNextTemplate) + iPad);
		}

		pItemTemplate = pNextTemplate;
	}

	// Store the current child dialog
	currentDlgId = id;
	childExists = true;


}

/*============================================================*\
	CMultiDialog::AddDialogItem
  ============================================================
	Author : Martin Randall (18/06/2001)

	Description :	Add a dialog item to the current dialog

	Return :			DLGITEMTEMPLATE *	-	
	Parameters :
			DLGITEMTEMPLATE *pItem	-	
			LPRECT pRect	-	
	Note :
\*============================================================*/
DLGITEMTEMPLATE *CMultiDialog::AddDialogItem(DLGITEMTEMPLATE *pItem, LPRECT pRect )
{
	// Point to the class array
	WORD *pArrayScan = (WORD*)(pItem + 1);

	// Get Class information
	WORD nClassID;
	CString strClassName;
	pArrayScan = ReadTemplateArray( pArrayScan, nClassID, strClassName );

	// Get control text information
	WORD nTitleID;
	CString strTitleName;
	pArrayScan = ReadTemplateArray( pArrayScan, nTitleID, strTitleName );

	// Get intialisation data
	BYTE *pData = 0;
	int iDataSize = *pArrayScan++;
	if ( iDataSize ) {
		pData = (BYTE*) *pArrayScan;
		pArrayScan += iDataSize/sizeof(WORD);
	}

	// Create the window
	CWnd *pNewWnd = 0;

	// Get position
	CRect pos( CPoint(pItem->x, pItem->y), CSize( pItem->cx, pItem->cy ) );
	MapDialogRect( pos );			// Convert from dialog coords to screen coords
	pos.OffsetRect( pRect->left, pRect->top );

	switch ( nClassID ) {	// What kind of control?

	case 0x0080:	// Button control
        if ((pItem->style & BS_OWNERDRAW) != 0)
        {
            pNewWnd = new FooButton;
    		((FooButton*)pNewWnd)->Create( strTitleName, pItem->style, pos, this, pItem->id );
        }
        else
        {
		    pNewWnd = new CButton;
		    ((CButton*)pNewWnd)->Create( strTitleName, pItem->style, pos, this, pItem->id );
        }
		break;
		
	case 0x0081:	// Edit control
		pNewWnd = new CEdit;
		((CEdit*)pNewWnd)->Create( pItem->style, pos, this, pItem->id );
		break;
		
	case 0x0082:	// Static control
		pNewWnd = new CStatic;
		((CStatic*)pNewWnd)->Create( strTitleName, pItem->style, pos, this, pItem->id );
		break;
		
	case 0x0083:	// List control
		pNewWnd = new CListBox;
		((CListBox*)pNewWnd)->Create( pItem->style, pos, this, pItem->id );
		break;
		
	case 0x0084:	// Scrollbar control
		pNewWnd = new CScrollBar;
		((CScrollBar*)pNewWnd)->Create( pItem->style, pos, this, pItem->id );
		break;
		
	case 0x0085:	// Combo box control
		pNewWnd = new CComboBox;
		((CComboBox*)pNewWnd)->Create( pItem->style, pos, this, pItem->id );
		break;
		
	default:
		if ( strClassName.GetLength() ) {
			pNewWnd = new CWnd;
			pNewWnd->Create( strClassName, strTitleName, pItem->style, pos, this, pItem->id );
		}
		break;
	}
	
	if ( pNewWnd ) {
		// Add child window to our list
		m_ChildList.push_back( pNewWnd );
		pNewWnd->SetFont( m_pCurrentFont ? m_pCurrentFont : GetFont() );		// Give them the same font as us!
	}
	

	return (DLGITEMTEMPLATE*)pArrayScan;
}


/*============================================================*\
	CMultiDialog::ReadTemplateArray
  ============================================================
	Author : Martin Randall (18/06/2001)

	Description :	Read a template array

	Return :			WORD *	-	
	Parameters :
			WORD *pArray	-	
			WORD &ID	-	
			CString &name	-	
	Note :
\*============================================================*/
WORD *CMultiDialog::ReadTemplateArray(WORD *pArray, WORD &ID, CString &name)
{
	ID = 0xffff;
	name = "";
	
	if ( *pArray == 0 ) {		// Nothing there!
		pArray++;
	} else if ( *pArray == 0xffff )	{	// This array has a resource ID associated with it?
		ID = pArray[1];
		pArray += 2;
	} else {							// This array has a uni-code string associated with it
		//localization. cuilike++
		//while(*pArray)
		//{
		//	name += (char)*pArray++;
		//}
		while ( *pArray )
		{
			wchar_t wch = *pArray++;
			char ch[3] = {0};
			WideCharToMultiByte(CP_ACP, 0, &wch, 1, ch, 3, NULL ,NULL);
			name += ch;
		}
		//localization. ++cuilike
		pArray++;
	}
	
	return pArray;
}

/*============================================================*\
	CMultiDialog::PostNcDestroy
  ============================================================
	Author : Martin Randall (18/06/2001)

	Description :	Destroy all the child windows and fonts 
					when the dialog is destroyed
\*============================================================*/
void CMultiDialog::PostNcDestroy() 
{
	CDialog::PostNcDestroy();

	// Delete child windows
	for ( WndListIter scan = m_ChildList.begin(); scan != m_ChildList.end(); scan++ )
		delete *scan;

	// Delete fonts
	for ( FontListIter fontscan = m_FontList.begin(); fontscan != m_FontList.end(); fontscan++ )
		delete *fontscan;
}


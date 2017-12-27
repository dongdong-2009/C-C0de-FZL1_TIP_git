/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/Animation.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// Animation.cpp: implementation of the CAnimation class.
//
// Smoov Code Project Screen Saver Release 1.1 by Ben Bryant
// Copyright (C) 2002 First Objective Software, Inc. All rights reserved
// Retain this notice and disclaimer
// This software class is provided as is with no warranty
// You may redistribute and modify this software, but the
// uncompiled source code cannot be sold without the authors written consent!
// You may use this class freely in compiled commercial applications, enjoy!
// Go to www.firstobject.com for the latest version and information
//
// The Code Project name, web service and some graphics are the
// property of The Code Project www.codeproject.com

#include "stdafx.h"

#include "app/system_control/transact_screensaver/src/Animation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    //////////////////////////////////////////////////////////////////////
    // CAnimationItem

    CAnimationItem::CAnimationItem()
    {
	    SetDefaults();
	    m_pAE = NULL;
    }


    CAnimationItem::CAnimationItem( CAnimationEngine* pAE )
    {
	    SetDefaults();
	    m_pAE = pAE;
    }


    CAnimationItem::~CAnimationItem()
    {
    }


    CAnimationItem& CAnimationItem::SetDefaults()
    {
	    m_nFrames = 20;
	    m_nEffects = 0;
	    m_pFont = NULL;
	    m_pBitmap = NULL;
	    int nFirstPic = -1;
	    int nLastPic = -1;
	    SetTextColor( RGB(191,191,191) );
	    SetPoint( CPoint(0,0) );
	    m_bDrawnYet = FALSE;
	    m_bWait = FALSE;
	    m_posKickOff = NULL;
	    m_posNext = NULL;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetFont( CString csFont )
    {
	    m_pFont = m_pAE->GetFont( csFont );
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetBitmap( UINT nBitmapID, int nFirst, int nLast )
    {
	    m_pBitmap = m_pAE->GetBmp( nBitmapID );
	    m_nFirstPic = nFirst;
	    m_nLastPic = nLast;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetTextColor( COLORREF rgb )
    {
	    m_rgbTextColor = rgb;
	    m_rgbTextColorCurrent = rgb;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetText( CString csText )
    {
	    m_csText = csText;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetStart( CPoint pt )
    {
	    m_ptStart = pt;
	    m_pt = m_ptStart;
	    m_dX = pt.x;
	    m_dY = pt.y;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetFinish( CPoint pt )
    {
	    m_ptFinish = pt;
	    return *this;
    }


    CAnimationItem& CAnimationItem::SetPoint( CPoint pt )
    {
	    SetStart( pt );
	    return SetFinish( pt );
    }


    CAnimationItem& CAnimationItem::SetFrames( int nFrames )
    {
	    m_nFrames = nFrames;
	    return *this;
    }


    CAnimationItem& CAnimationItem::GoAfter( POSITION pos )
    {
	    if ( ! pos )
		    return *this;
	    CAnimationItem* pItem = &m_pAE->m_listItems.GetAt( pos );
	    if ( pItem->m_posKickOff )
		    m_posNext = pItem->m_posKickOff;
	    pItem->m_posKickOff = m_pos;
	    m_bWait = TRUE;
	    return *this;
    }


    CAnimationItem& CAnimationItem::GoWith( POSITION pos )
    {
	    if ( ! pos )
		    return *this;
	    CAnimationItem* pItem = &m_pAE->m_listItems.GetAt( pos );
	    m_posNext = pItem->m_posNext;
	    m_bWait = pItem->m_bWait;
	    pItem->m_posNext = m_pos;
	    return *this;
    }

    //////////////////////////////////////////////////////////////////////
    // CAnimationEngine

    CAnimationEngine::CAnimationEngine()
    {
	    m_rect.SetRectEmpty();
	    m_rgbBackground = RGB(0,0,0);
	    m_brushBackground.CreateStockObject(BLACK_BRUSH);
	    m_pDefaultFont = GetFont( "15,MS Sans Serif" );
	    srand( (unsigned)time( NULL ) );
    }


    CAnimationEngine::~CAnimationEngine()
    {
	    DestroyGDIs();
    }


    CAnimationItem& CAnimationEngine::CreateItem( CString csText )
    {
	    CAnimationItem item( this );
	    item.m_csText = csText;
	    POSITION pos = m_listItems.AddTail( item );
	    CAnimationItem& itemRef = m_listItems.GetAt( pos );
	    itemRef.m_pos = pos;
	    return itemRef;
    }


    CAnimationItem& CAnimationEngine::CreateItemAfter( POSITION pos )
    {
	    CAnimationItem& itemFrom = m_listItems.GetAt( pos );
	    CAnimationItem& itemRef = CreateItem( itemFrom.m_csText );
	    itemRef.m_csText = itemFrom.m_csText;
	    itemRef.SetPoint( itemFrom.m_ptFinish );
	    itemRef.SetTextColor( itemFrom.m_rgbTextColor );
	    itemRef.m_pFont = itemFrom.m_pFont;
	    itemRef.m_nEffects |= (itemFrom.m_nEffects &
		    (CAnimationItem::AF_AlignCenter|CAnimationItem::AF_AlignRight));
	    itemRef.GoAfter( pos );
	    itemRef.m_pBitmap = itemFrom.m_pBitmap;
	    itemRef.m_nFirstPic = itemFrom.m_nFirstPic;
	    itemRef.m_nLastPic = itemFrom.m_nLastPic;
	    return itemRef;
    }


    int CAnimationEngine::RunFrame( CDC& dc )
    {
	    // Setup DC
	    CFont* pOldFont = dc.SelectObject( m_pDefaultFont );
	    dc.SetBkColor( RGB(0,0,0) );
	    dc.SetBkMode( TRANSPARENT );
	    CRect rect;

	    // Loop through items
	    int nItems = 0, nItemsAlive = 0;
	    POSITION pos = m_listItems.GetHeadPosition();
	    while ( pos )
	    {
		    POSITION posCurrent = pos;
		    CAnimationItem& item = m_listItems.GetNext( pos );

		    ++nItems;
		    if ( item.m_bWait )
			    continue;
		    ++nItemsAlive;

		    // Font
		    if ( item.m_pFont )
			    dc.SelectObject( item.m_pFont );
		    else
			    dc.SelectObject( m_pDefaultFont );

		    // Alignment
		    DWORD nDrawFlags = DT_WORDBREAK;
		    if ( item.m_nEffects & item.AF_AlignRight )
			    nDrawFlags |= DT_RIGHT;
		    else if ( item.m_nEffects & item.AF_AlignCenter )
			    nDrawFlags |= DT_CENTER;

		    // Set flag if erase is necessary (always erase on last step)
		    BOOL bErase = ( ! item.m_nFrames );

		    // Calculate new position
		    CPoint ptNew = item.m_pt;
		    if ( item.m_bDrawnYet && item.m_ptStart != item.m_ptFinish )
		    {
			    if ( ! item.m_nFrames )
				    ptNew = item.m_ptFinish;
			    else
			    {
				    item.m_dX += ( ((double)item.m_ptFinish.x) - item.m_dX ) / ((double)item.m_nFrames);
				    item.m_dY += ( ((double)item.m_ptFinish.y) - item.m_dY ) / ((double)item.m_nFrames);
				    ptNew = CPoint( (int)item.m_dX, (int)item.m_dY );
			    }
			    if ( item.m_pt != ptNew )
				    bErase = TRUE;
		    }

		    // Calculate fade
		    BOOL bFade = item.m_nEffects & (item.AF_FadeIn|item.AF_FadeOut);
		    if ( bFade )
		    {
			    BOOL bIn = item.m_nEffects & item.AF_FadeIn;
			    if ( ! item.m_nFrames )
				    item.m_rgbTextColorCurrent = bIn?item.m_rgbTextColor:RGB(0,0,0);
			    else
			    {
				    int nMask = 0xff;
				    for ( int nC = 0; nC < 3; ++nC )
				    {
					    int nColor = (item.m_rgbTextColor & nMask) >> (8*nC);
					    int nCurColor = (item.m_rgbTextColorCurrent & nMask) >> (8*nC);
					    if ( bIn )
						    nCurColor += ( (nColor-nCurColor) / item.m_nFrames );
					    else
						    nCurColor -= ( nCurColor / item.m_nFrames );
					    item.m_rgbTextColorCurrent = (item.m_rgbTextColorCurrent&(~nMask)) | (nCurColor << (8*nC));
					    nMask = nMask << 8;
				    }
			    }
		    }

		    // Remove if drawn previously
		    if ( item.m_pBitmap )
			    DrawBitmapItem( dc, item, ptNew, item.m_nFrames != 0 );
		    else if ( item.m_bDrawnYet && bErase )
			    DrawTextItem( dc, item, nDrawFlags, FALSE );
		    item.m_bDrawnYet = FALSE;

		    // See if any more frames remain for this item
		    if ( ! item.m_nFrames )
		    {
			    POSITION posKickOff = item.m_posKickOff;
			    while ( posKickOff )
			    {
				    CAnimationItem* pNext = &m_listItems.GetAt( posKickOff );
				    pNext->m_bWait = FALSE;
				    posKickOff = pNext->m_posNext;
				    pNext->m_posNext = NULL;
			    }
			    m_listItems.RemoveAt( posCurrent );
			    continue;
		    }
		    --item.m_nFrames;

		    // Draw at new position
		    item.m_pt = ptNew;
		    if ( ! item.m_pBitmap )
			    DrawTextItem( dc, item, nDrawFlags, TRUE );
		    item.m_bDrawnYet = TRUE;
	    }

	    if ( nItems && ! nItemsAlive )
		    ASSERT( 0 );

	    dc.SelectObject( pOldFont );
	    return nItems;
    }


    void CAnimationEngine::DrawBitmapItem( CDC& dc, CAnimationItem& item, CPoint ptNew, BOOL bShow )
    {
	    CPoint pt = item.m_pt;
	    CRect rectBmp( 0, 0, item.m_pBitmap->GetWidth(), item.m_pBitmap->GetHeight() );
	    if ( item.m_nFirstPic != -1 )
	    {
		    // Bitmap can be divided into square pictures
		    int nWidth = rectBmp.Height();
		    rectBmp.right = nWidth;
		    rectBmp.OffsetRect( item.m_nFirstPic*nWidth, 0 );
	    }
	    CRect rect( 0,0,rectBmp.Width(),rectBmp.Height() );
	    rect.OffsetRect( pt );
	    if ( item.m_nEffects & item.AF_AlignCenter )
		    rect.OffsetRect( - rectBmp.Width()/2, 0 );
	    if ( ! bShow )
	    {
		    dc.FillRect( &rect, &m_brushBackground );
		    return;
	    }

	    if ( pt != ptNew )
	    {
		    // Determine rects that need to be cleared
		    CRect rectLeftOrRight( rect ), rectTopOrBottom( rect );
		    int nXDiff = ptNew.x - pt.x;
		    int nYDiff = ptNew.y - pt.y;
		    if ( nXDiff >= 0 ) // moving right
			    rectLeftOrRight.right = rectTopOrBottom.left = rectLeftOrRight.left + nXDiff;
		    else // moving left
			    rectLeftOrRight.left = rectTopOrBottom.right = rectLeftOrRight.left + nXDiff;
		    if ( nYDiff >= 0 ) // moving down
			    rectTopOrBottom.bottom = rectTopOrBottom.top + nYDiff;
		    else // moving up
			    rectTopOrBottom.top = rectTopOrBottom.top + nYDiff;
		    dc.FillRect( &rectLeftOrRight, &m_brushBackground );
		    dc.FillRect( &rectTopOrBottom, &m_brushBackground );
	    }

	    rect.SetRect( 0,0,rectBmp.Width(),rectBmp.Height() );
	    rect.OffsetRect( ptNew );
	    if ( item.m_nEffects & item.AF_AlignCenter )
		    rect.OffsetRect( - rectBmp.Width()/2, 0 );
	    item.m_pBitmap->DrawDIB( &dc, rect, rectBmp );
    }


    void CAnimationEngine::DrawTextItem( CDC& dc, CAnimationItem& item, int nDrawFlags, BOOL bShow )
    {
	    CRect rect;
	    CPoint pt = item.m_pt;

	    if ( bShow )
		    dc.SetTextColor( item.m_rgbTextColorCurrent );
	    else
		    dc.SetTextColor( m_rgbBackground );

	    CSize size = dc.GetTextExtent( item.m_csText );
	    int nBottom = pt.y + size.cy * 2 + 1;
	    if ( item.m_nEffects & item.AF_AlignCenter )
		    rect.SetRect( pt.x-size.cx/2, pt.y, pt.x+size.cx/2+2, nBottom );
	    else if ( item.m_nEffects & item.AF_AlignRight )
		    rect.SetRect( pt.x-size.cx-1, pt.y, pt.x, nBottom );
	    else
		    rect.SetRect( pt.x, pt.y, pt.x+size.cx+1, nBottom );
	    dc.DrawText( item.m_csText, &rect, nDrawFlags );
    }


    CDIBitmap* CAnimationEngine::GetBmp( UINT nBitmapID )
    {
	    CDIBitmap* pBmp = NULL;
	    if ( ! m_mapBmps.Lookup( nBitmapID, pBmp ) )
	    {
		    pBmp = new CDIBitmap;
		    if ( ! pBmp->LoadResource(nBitmapID) )
		    {
			    delete pBmp;
			    return NULL;
		    }

		    // Create bitmap and add it to the pool
		    m_mapBmps.SetAt( nBitmapID, pBmp );
	    }
	    return pBmp;
    }


    CFont* CAnimationEngine::GetFont( CString csFont )
    {
	    CFont* pFont;
	    if ( ! m_mapFonts.Lookup( csFont, pFont ) )
	    {
		    // Create font and add it to the pool
		    LOGFONT logfont;
		    logfont.lfHeight = -12;
		    logfont.lfWidth = 0;
		    logfont.lfEscapement = 0;
		    logfont.lfOrientation = 0;
		    logfont.lfWeight = FW_NORMAL;
		    logfont.lfItalic = 0;
		    logfont.lfUnderline = 0;
		    logfont.lfStrikeOut = 0;
		    logfont.lfCharSet = ANSI_CHARSET;
		    logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		    logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		    logfont.lfQuality = NONANTIALIASED_QUALITY;
		    logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		    strcpy(logfont.lfFaceName, "Arial");

		    // Break out font string into comma-delimited font attributes
		    CString csAttrib;
		    int nComma = 0, nStart = 0;
		    while ( (nComma = csFont.Find(",",nStart)) != -1 )
		    {
			    csAttrib = csFont.Mid( nStart, nComma - nStart );
			    if ( csAttrib.GetLength() )
			    {
				    switch ( csAttrib[0] )
				    {
				    case 'B': // Bold
					    if ( csAttrib.GetLength() > 1 )
						    logfont.lfWeight = atoi(csAttrib.Mid(1));
					    else
						    logfont.lfWeight = FW_BOLD;
					    break;
				    case 'I': // Italic
					    logfont.lfItalic = TRUE;
					    break;
				    case 'U': // Underline
					    logfont.lfUnderline = TRUE;
					    break;
				    case 'S': // StrikeOut
					    logfont.lfStrikeOut = TRUE;
					    break;
				    case 'C': // CS is CharSet
					    if ( csAttrib.GetLength() > 2 )
						    logfont.lfCharSet = atoi(csAttrib.Mid(2));
					    break;
				    default:
					    if ( isdigit(csAttrib[0]) || csAttrib[0] == '-' )
						    logfont.lfHeight = - atoi( csAttrib );
					    break;
				    }
			    }
			    nStart = nComma + 1;
		    }
		    if ( csFont.GetLength() > nStart )
		    {
			    strncpy(logfont.lfFaceName, csFont.Mid(nStart), LF_FACESIZE);
			    logfont.lfFaceName[LF_FACESIZE-1] = '\0';
		    }
		    pFont = new CFont;
		    pFont->CreateFontIndirect( &logfont );
		    m_mapFonts.SetAt( csFont, pFont );
	    }
	    return pFont;
    }


    int CAnimationEngine::Rand( int n )
    {
	    int nRand = rand() * n / RAND_MAX;
	    return max(0,min(nRand,n-1)); // enforce bounds
    }


    CPoint CAnimationEngine::RandPt( int nMargin, int nMin )
    {
	    int nHorizMargin = max( 0, min(m_rect.Width()/2-nMin,nMargin) );
	    int nX = Rand(m_rect.Width()-nHorizMargin*2) + m_rect.left + nHorizMargin;
	    int nVertMargin = max( 0, min(m_rect.Height()/2-nMin,nMargin) );
	    int nY = Rand(m_rect.Height()-nVertMargin*2) + m_rect.left + nVertMargin;
	    return CPoint( nX, nY );
    }


    void CAnimationEngine::DestroyGDIs()
    {
	    POSITION pos = m_mapFonts.GetStartPosition();
	    while ( pos )
	    {
		    CString csKey;
		    CFont* pFont;
		    m_mapFonts.GetNextAssoc( pos, csKey, pFont );
		    pFont->DeleteObject();
		    delete pFont;
	    }
	    m_mapFonts.RemoveAll();

	    pos = m_mapBmps.GetStartPosition();
	    while ( pos )
	    {
		    UINT nID;
		    CDIBitmap* pBmp;
		    m_mapBmps.GetNextAssoc( pos, nID, pBmp );
		    delete pBmp;
	    }
	    m_mapBmps.RemoveAll();

	    m_brushBackground.DeleteObject();
    }


    void CAnimationEngine::SetScreen( CRect rect )
    {
	    m_rect = rect;
    }
}

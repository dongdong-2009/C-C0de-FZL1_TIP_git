/**
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Set the font of all windows created by the current thread, message boxes excepted.
  * Instantiate with the name of the font face, point size and italic/bold properties.
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "ace/Guard_T.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// ApplicationFont

ApplicationFont* ApplicationFont::s_pInstance = NULL;
ACE_Mutex ApplicationFont::s_mtxInstanceLock;

ApplicationFont::ApplicationFont()
{
    FUNCTION_ENTRY( "ApplicationFont" );

    FUNCTION_EXIT;
}

ApplicationFont::~ApplicationFont()
{
    FUNCTION_ENTRY( "~ApplicationFont" );

    FUNCTION_EXIT;
}

BOOL ApplicationFont::setChildFont(HWND hWnd, LPARAM lparam)
{
    if ( ::IsWindow( hWnd ) && ( NULL != lparam ))
    {
        ::SendMessage( hWnd, WM_SETFONT, static_cast<WPARAM>(lparam), TRUE );
    }

    return TRUE;
}

HGDIOBJ ApplicationFont::getFont( const std::string& strFontName, const int nPointSize, const bool bItalic, const bool bBold )
{
    ACE_Guard<ACE_Mutex> gudPoolLock( m_mtxFontsPoolLock );

    HGDIOBJ hFont = NULL;

    const unsigned long ulFontSize = (static_cast<unsigned long>(nPointSize) << (sizeof(unsigned long) * 4u)) | (bItalic ? 1u : 0u) | ((bBold ? 1u : 0u) << 1);

    // Find the font name
    FontNameToFontMapIt itFontMapFound = m_mapFontsPool.find( strFontName );
    if ( itFontMapFound != m_mapFontsPool.end() )
    {
        // Find the point size
        PointSizeToFontMapIt itPointMapFound = itFontMapFound->second.find( ulFontSize );
        if ( itFontMapFound->second.end() != itPointMapFound )
        {
            hFont = itPointMapFound->second;
        }
    }
    else
    {
        PointSizeToFontMap mapNewMap;
        std::pair<FontNameToFontMapIt, bool> pirRes = m_mapFontsPool.insert( FontNameToFontMap::value_type( strFontName, mapNewMap ));
        if ( pirRes.second )
        {
            itFontMapFound = pirRes.first;
        }
    }

    if ( NULL == hFont )
    {
        // No font name match, need to create a new font
        CWindowDC dcWin(NULL);
        int nHeight = -MulDiv(nPointSize, ::GetDeviceCaps(dcWin.GetSafeHdc(), LOGPIXELSY), 72);
        hFont = ::CreateFont( nHeight, 0, 0, 0, (bBold ? FW_BOLD : FW_NORMAL), bItalic, 0, 0, GB2312_CHARSET, 
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE | FF_DONTCARE, strFontName.c_str() );

        if (( NULL != hFont ) && ( m_mapFontsPool.end() != itFontMapFound ))
        {
            itFontMapFound->second.insert( PointSizeToFontMap::value_type( ulFontSize, hFont ));
        }
    }

    // Can't create font, throw exception
    if ( NULL == hFont )
    {
        TA_THROW( TA_Base_Core::TransactiveException( "Failed to create a font" ));
    }

    return hFont;
}

ApplicationFont& ApplicationFont::getInstance()
{
    if ( NULL == s_pInstance )
    {
        ACE_Guard<ACE_Mutex> gudInstanceLock( s_mtxInstanceLock );
        s_pInstance = new ApplicationFont();
    }

    if ( NULL == s_pInstance )
    {
        TA_THROW( TA_Base_Core::TransactiveException( "Failed to create instance of ApplicationFont" ));
    }

    return *s_pInstance;
}

void ApplicationFont::adjustChildrenFont( HWND hWnd, const HGDIOBJ hFont )
{
    if ( NULL == hWnd || !::IsWindow( hWnd ))
    {
        return;
    }

    const HGDIOBJ hActualFont = ( NULL == hFont ) ? getFont() : hFont;
    ::SendMessage( hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hActualFont), TRUE );
    ::EnumChildWindows( hWnd, setChildFont, reinterpret_cast<LPARAM>(hActualFont) );
}

}

//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_APPLICATIONFONT_H__04B9F2D3_2804_4B3F_8EAF_FDF66CFCADDA__INCLUDED_)
#define AFX_APPLICATIONFONT_H__04B9F2D3_2804_4B3F_8EAF_FDF66CFCADDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "ace/Mutex.h"


namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// ApplicationFont
const std::string DEFAULT_FONT_NAME = "Arial";
const int DefaultSize = 11;
const bool DefaultBold = true;
const bool DefaultItalic = false;

typedef std::map<unsigned long, HGDIOBJ> PointSizeToFontMap;
typedef PointSizeToFontMap::iterator PointSizeToFontMapIt;
typedef std::map<std::string, PointSizeToFontMap> FontNameToFontMap;
typedef FontNameToFontMap::iterator FontNameToFontMapIt;

class ApplicationFont
{
// Operations
public:
    static ApplicationFont& getInstance();
    static BOOL __stdcall setChildFont( HWND hWnd, LPARAM lparam );
    void adjustChildrenFont( HWND hWnd, const HGDIOBJ hFont =  NULL );
    HGDIOBJ getFont( const std::string& strFontName = DEFAULT_FONT_NAME, const int nPointSize = DefaultSize, const bool bItalic = DefaultItalic, const bool bBold = DefaultBold );
    ~ApplicationFont();

private:
    ApplicationFont();
    ApplicationFont(const ApplicationFont& applicationFont);
    ApplicationFont& operator=(const ApplicationFont& applicationFont);

// Attributes
private:
    static ApplicationFont* s_pInstance;
    static ACE_Mutex s_mtxInstanceLock;
    FontNameToFontMap m_mapFontsPool;
    ACE_Mutex m_mtxFontsPoolLock;
};

}

#endif // !defined(AFX_APPLICATIONFONT_H__04B9F2D3_2804_4B3F_8EAF_FDF66CFCADDA__INCLUDED_)

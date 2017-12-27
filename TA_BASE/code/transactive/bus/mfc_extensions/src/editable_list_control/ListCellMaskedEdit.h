//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellMaskedEdit.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/19 17:43:23 $
// Last modified by:  $Author: CM $
//
// Adapted from class CmEdit posted on CodeGuru by Dan Hintz.
//
// This is an in-table edit control that allows a mask to be applied to the text input by the user
// a character by character basis. The control ensures that the user enters only those values that 
// match the mask.
//
// The mask is formatted so that each changeable character is inside the brackets, and all static 
// characters (characters not entered by the user, such as a dash or a slash) are outside the brackets. 
// Dashes can be used inside the brackets to indicate a range of characters (based on the ASCII value 
// of the start and end character). For example, the mask used on a cell storing a 7-digit telephone 
// number that must begin with a 6 or a 9 may look something like this: "[69][0-9][0-9]-[0-9][0-9][0-9][0-9]".

#if !defined(AFX_LISTCELLMASKEDEDIT_H__69497F94_A7C0_4027_A59A_76F4765B6D98__INCLUDED_)
#define AFX_LISTCELLMASKEDEDIT_H__69497F94_A7C0_4027_A59A_76F4765B6D98__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "bus\mfc_extensions\src\editable_list_control\ListCellEdit.h"


class CListCellMaskedEditChar
{
public:    
    CListCellMaskedEditChar() :
        m_bStaticChar(false)
    {

    }

    CListCellMaskedEditChar(CString strChars) :
        m_strValids(strChars), m_bStaticChar(false)
    {
    }

    CListCellMaskedEditChar(CString strChars, bool bStaticChar) :
        m_strValids(strChars), m_bStaticChar(bStaticChar)
    {
    }
    
    CListCellMaskedEditChar(const CListCellMaskedEditChar& other) :
        m_strValids(other.m_strValids), m_strData(other.m_strData), m_bStaticChar(other.m_bStaticChar)
    {
    }

    CListCellMaskedEditChar& operator=(const CListCellMaskedEditChar& other)
    {
        if (this != &other)
        {
            m_strValids = other.m_strValids;
            m_strData = other.m_strData;
            m_bStaticChar = other.m_bStaticChar;
        }

        return *this;
    }

    CString m_strValids;
    CString m_strData;
    bool m_bStaticChar;
};



class CListCellMaskedEdit : public CListCellEdit
{
public:
	CListCellMaskedEdit(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem);
	virtual ~CListCellMaskedEdit();

    bool    Initialise(char promptChar, CString mask, CString text = "");

    // Hide CListCellEdit::SetNumeric()
	void SetNumeric(bool numeric = true) {};

protected:
    bool    Parse(const CString& strMask);
    bool    SetValidChars(const CString& strMask);
    void    SetData(const CString& data);
    CString GetRange(CString szLow, CString szHigh);
    int     FindNextChar(int iStartPos, bool bStatic, bool bForward);

private:
    typedef CArray<CListCellMaskedEditChar, CListCellMaskedEditChar&> MaskChars;

    MaskChars m_maskChars;
    CString   m_strPrompt;
    int       m_iCurrentChar;
    bool      m_bShiftDown;


protected:
	//{{AFX_VIRTUAL(CListCellMaskedEdit)
	//}}AFX_VIRTUAL

    //{{AFX_MSG(CListCellMaskedEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCELLMASKEDEDIT_H__69497F94_A7C0_4027_A59A_76F4765B6D98__INCLUDED_)


//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellMaskedEdit.cpp $
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

#include "bus\mfc_extensions\src\stdafx.h"
#include "bus\mfc_extensions\src\editable_list_control\ListCellMaskedEdit.h"
#include "bus\mfc_extensions\src\editable_list_control\EditableListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListCellMaskedEdit

CListCellMaskedEdit::CListCellMaskedEdit(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem) :
    CListCellEdit(parent, cellManager, cellRect, item, subItem, ""),
    m_strPrompt("_"),
    m_iCurrentChar(0),
    m_bShiftDown(false)
{
}


CListCellMaskedEdit::~CListCellMaskedEdit()
{
    m_maskChars.RemoveAll();
}


bool CListCellMaskedEdit::Initialise(char promptChar, CString mask, CString text)
{
    m_strPrompt = promptChar;

    if (!Parse(mask))
    {
        return false;
    }

    if (!SetValidChars(mask))
    {
        return false;                          
    }
    
    SetData(text);
    SetSel(0, -1);

    return true;
}


void CListCellMaskedEdit::SetData(const CString& data)
{
    CString strWindowText;
    CString strData;
    int     iNextDataChar = 0;

    // Initialise to blank form
    for (int i = 0; i < m_maskChars.GetSize(); i++)
    {
        if (m_maskChars[i].m_bStaticChar)
        {
            strWindowText += m_maskChars[i].m_strValids;
        }
        else
        {
            strWindowText += m_strPrompt;
        }
    }
    
    if (data.GetLength() == m_maskChars.GetSize())
    {
		int i = 0;
        for (i = 0; i < m_maskChars.GetSize(); i++)
        {
            // Ensure each character is valid
            if (m_maskChars[i].m_strValids.Find(data[i]) < 0)
            {
                break;
            }
        }

        // If the entire contents of the data string checked out OK
        if (i == m_maskChars.GetSize())
        {
            strWindowText = data;
        }
    }

    SetWindowText(strWindowText);
}


bool CListCellMaskedEdit::SetValidChars(const CString& strMask)
{
    CString strChars;
    CString strRangeLow; 
    bool    bInside       = false;
    bool    bInsideEscape = false;
    bool    bInsideRange  = false;
    int     iNextChar     = 0;
    int     i;
    
    // Clear valid chars
    m_maskChars.RemoveAll();


    // Walk through mask string
    for (i = 0; i < strMask.GetLength(); i++)
    {
        switch (strMask[i])
        {
        case '[':   bInside = true;
                    break;            //done

        case ']':   bInside = false;
                    m_maskChars.Add(CListCellMaskedEditChar(strChars, false));
                    strChars = "";
                    break;

        case '-':   if (bInside)
                    {
                        if (bInsideEscape)
                        {
                            strChars += "-";
                            bInsideEscape = false;  //done
                        }
                        else
                        {
                            if (strChars.GetLength() == 0)
                                return false;
                            bInsideRange = true;
                            strRangeLow = strChars.Right(1);
                            strChars = strChars.Left(strChars.GetLength() - 1); //done
                        }
                    }
                    else
                    {
                        m_maskChars.Add(CListCellMaskedEditChar("-", true));
                    }
                    break;

        case '\\':  if (bInside)
                    {
                        if (!bInsideEscape)
                        {
                            bInsideEscape = true;  //done
                        }
                        else
                        {
                            if (bInsideRange)
                            {
                                strChars += GetRange(strRangeLow, "\\");
                                strRangeLow = "";
                                bInsideRange = false;  //done
                            }
                            else
                            {
                                strChars += "\\";
                                bInsideEscape = false;  //done
                            }
                        }
                    }
                    else
                    {
                        m_maskChars.Add(CListCellMaskedEditChar("\\", true));
                    }
                    break;

        default:    if (bInside)
                    {
                        if (bInsideEscape)
                        {
                            bInsideEscape = false;        //done
                        }

                        if (bInsideRange)
                        {
                            strChars += GetRange(strRangeLow, strMask[i]);
                            strRangeLow = "";
                            bInsideRange = false;   //done
                        }
                        else
                        {
                            strChars += strMask[i]; //done
                        }
                    }
                    else
                    {
                        m_maskChars.Add(CListCellMaskedEditChar(strMask[i], true));
                    }
                    break;
        }
    }

    SetLimitText(m_maskChars.GetSize());
    
    return true;
}


bool CListCellMaskedEdit::Parse(const CString& strMask)
{
    bool bValid = true;
    int iBracketState = 0;
    //int iDashState = 0;
    
    for (int i = 1; i <= strMask.GetLength(); i++)
    {
        switch (strMask[i - 1])
        {
        case '[':   iBracketState++; break;

        case ']':   iBracketState--; break;

        //case '-':   if (iBracketState > 0)
        //                iDashState = 1;
        //            break;

        default:    //if (iBracketState > 0) && iDashState == 1)
                    //    iDashState = 0;
                    break;
        }

        // Make sure the [ ] are in sync
        if (iBracketState < 0 || iBracketState > 1)
            bValid = false;

        // Make sure there is a character after a '-' inside the [ ]
        //if (iBracketState == 0 && iDashState == 1)
        //    bValid = false;
    }

    return (iBracketState == 0 /*&& iDashState == 0*/ && bValid) ? true : false;
}


int CListCellMaskedEdit::FindNextChar(int iStartPos, bool bStatic, bool bForward)
{
    int iIndex = 0;
    int iReturnVal = -1;
    
    if (bForward)
    {
        for (int iIndex = iStartPos; iIndex < m_maskChars.GetSize(); iIndex++)
        {
            if (m_maskChars[iIndex].m_bStaticChar == bStatic)
            {
                iReturnVal = iIndex;
                break;
            }
        }
    }
    else
    {
        for (int iIndex = iStartPos; iIndex >= 0; iIndex--)
        {
            if (m_maskChars[iIndex].m_bStaticChar == bStatic)
            {
                iReturnVal = iIndex;
                break;
            }
        }
    }

    return iReturnVal;
}


CString CListCellMaskedEdit::GetRange(CString szLow, CString szHigh)
{
    CString szReturn = "";

    if (szLow.GetLength() == 1 && szHigh.GetLength() == 1)
    {
        char first = szLow[0];
        char last  = szHigh[0];

        for (int i = first; i <= last; i++)
            szReturn += (CString)(char)i;
    }

    return szReturn;
}


BEGIN_MESSAGE_MAP(CListCellMaskedEdit, CListCellEdit)
	//{{AFX_MSG_MAP(CListCellMaskedEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellMaskedEdit message handlers

void CListCellMaskedEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int iNext = 0;
    int iStartSel = 0, iEndSel = 0;

    switch (nChar)
    {
    // Escape and Return keys should be handled by CListCellEdit
    case VK_ESCAPE:
    case VK_RETURN:
                CListCellEdit::OnChar(nChar, nRepCnt, nFlags);
                break;

    default:           
                if (m_iCurrentChar < m_maskChars.GetSize())
                {
                    if ( m_maskChars[m_iCurrentChar].m_strValids.Find((TCHAR)nChar) >= 0 )
                    {
                        GetSel(iStartSel, iEndSel);
                
                        if (iStartSel != iEndSel)
                        {
                            CString szReplaceText = "";
                            for (int i = iStartSel; i < iEndSel; i++)
                            {
                                if (m_maskChars[i].m_bStaticChar)
                                {
                                    szReplaceText += m_maskChars[i].m_strValids;
                                }
                                else
                                {
                                    szReplaceText += m_strPrompt;
                                }
                            }
                            ReplaceSel(szReplaceText);
                            m_iCurrentChar = FindNextChar(iStartSel, false, true);
                            SetSel(m_iCurrentChar, m_iCurrentChar);
                        }

                        int iNewNext = FindNextChar(m_iCurrentChar + 1, false, true);
                        SetSel(m_iCurrentChar, m_iCurrentChar + 1);
                        ReplaceSel((CString)(char)nChar);
                        if (iNewNext >= 0)
                        {
                            m_iCurrentChar = iNewNext;
                        }
                        else
                        {
                            m_iCurrentChar++;
                        }
                        SetSel(m_iCurrentChar, m_iCurrentChar);
                    }
                }
                break;
    }
}


void CListCellMaskedEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{  
    int iNext = 0;
    int iStartSel = 0, iEndSel = 0;

    switch (nChar)
    {                       
    // Shift key
    case VK_SHIFT:
                m_bShiftDown = true;
                break;

    // Left/Up arrow key
    case VK_LEFT:
    case VK_UP:
                if (m_bShiftDown)
                {
                    GetSel(iStartSel, iEndSel);

                    iNext = FindNextChar((iEndSel > m_iCurrentChar? iEndSel : iStartSel) - 1, false, false);

                    if (iNext >= 0)
                    {
                        SetSel(iNext, m_iCurrentChar); 
                    }
                }
                else
                {
                    iNext = FindNextChar(m_iCurrentChar - 1, false, false);
                    if (iNext >= 0)
                    {
                        m_iCurrentChar = iNext;
                        SetSel(m_iCurrentChar, m_iCurrentChar); 
                    }
                }
                break;
    
    // Right/down arrow key
    case VK_RIGHT:
    case VK_DOWN:
                if (m_bShiftDown)
                {
                    GetSel(iStartSel, iEndSel);
 
                    iNext = FindNextChar((iStartSel < m_iCurrentChar? iStartSel : iEndSel) + 1, false, true);

                    SetSel(m_iCurrentChar, iNext); 
                }
                else
                { 
                    iNext = FindNextChar(m_iCurrentChar + 1, false, true);
                    if (iNext >= 0)
                    {
                        m_iCurrentChar = iNext;
                    }
                    else
                    {
                        m_iCurrentChar = FindNextChar(m_maskChars.GetSize() - 1, false, false) + 1;
                    }
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }
                break;

    // Home key
    case VK_HOME:
                if (m_bShiftDown)
                {
                    SetSel(0, m_iCurrentChar);
                }
                else
                {
                    m_iCurrentChar = FindNextChar(0, false, true);
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }
                break;

    // End key
    case VK_END:
                if (m_bShiftDown)
                {
                    int iLast = FindNextChar(m_maskChars.GetSize() - 1, false, false) + 1;
                    SetSel(m_iCurrentChar, iLast);
                }
                else
                {
                    m_iCurrentChar = FindNextChar(m_maskChars.GetSize() - 1, false, false) + 1;
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }
                break;

    // Backspace key
    case VK_BACK:
                GetSel(iStartSel, iEndSel);
                
                if (iStartSel != iEndSel)
                {
                    CString szReplaceText = "";
                    for (int i = iStartSel; i < iEndSel; i++)
                    {
                        if (m_maskChars[i].m_bStaticChar)
                        {
                            szReplaceText += m_maskChars[i].m_strValids;
                        }
                        else
                        {
                            szReplaceText += m_strPrompt;
                        }
                    }
                    ReplaceSel(szReplaceText);
                    m_iCurrentChar = FindNextChar(iStartSel, false, true);
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }                
                else if (m_iCurrentChar > 0)
                {
                    int iNewNext = FindNextChar(m_iCurrentChar - 1, false, false);
                    if (iNewNext >= 0)
                    {
                        SetSel(iNewNext, iNewNext + 1);
                        ReplaceSel(m_strPrompt);
                        m_iCurrentChar = iNewNext;
                        SetSel(m_iCurrentChar, m_iCurrentChar);
                    }
                }
                break;

    // Delete key
    case VK_DELETE:
                GetSel(iStartSel, iEndSel);
                
                if (iStartSel != iEndSel)
                {
                    CString szReplaceText = "";
                    for (int i = iStartSel; i < iEndSel; i++)
                    {
                        if (m_maskChars[i].m_bStaticChar)
                        {
                            szReplaceText += m_maskChars[i].m_strValids;
                        }
                        else
                        {
                            szReplaceText += m_strPrompt;
                        }
                    }
                    ReplaceSel(szReplaceText);
                    m_iCurrentChar = FindNextChar(iStartSel, false, true);
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }
                else if (m_iCurrentChar <= FindNextChar(m_maskChars.GetSize() - 1, false, false))
                {
                    SetSel(m_iCurrentChar, m_iCurrentChar + 1);
                    ReplaceSel(m_strPrompt);
                    iNext = FindNextChar(m_iCurrentChar + 1, false, true);
                    if (iNext >= 0)
                        m_iCurrentChar = iNext;
                    else
                        m_iCurrentChar++;
                    SetSel(m_iCurrentChar, m_iCurrentChar);
                }
                break;

    default:
                break;

    }
}


void CListCellMaskedEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    switch (nChar)
    {
    // Shift key
    case VK_SHIFT:
                m_bShiftDown = false;
                m_iCurrentChar = LOWORD(GetSel());
                break;
    
    default:
                break;
    }
    
	CListCellEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CListCellMaskedEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int iStartSel, iEndSel;
    GetSel(iStartSel, iEndSel);
    
    if (iStartSel == iEndSel)
    {
        m_iCurrentChar = iStartSel;
        SetSel(m_iCurrentChar, m_iCurrentChar);
    }

	CListCellEdit::OnLButtonUp(nFlags, point);
}


void CListCellMaskedEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    SetSel(0, -1);
}

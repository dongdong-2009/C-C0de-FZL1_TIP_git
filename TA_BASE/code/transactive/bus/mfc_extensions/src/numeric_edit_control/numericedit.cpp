/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/numeric_edit_control/numericedit.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// numericedit.cpp : implementation file
//

#pragma warning (disable : 4284)

#include <math.h>
#include "..\stdafx.h"
#include "..\mfc_extensions.h"
#include "numericedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit

CNumericEdit::CNumericEdit(bool integer, bool sign) :
    m_acceptableNumChars("\b-.0123456789"),
    m_integer(integer),
    m_signed(sign)
{
    m_minimum.isSet = m_maximum.isSet = false;
}


CNumericEdit::CNumericEdit(double minLimit, double maxLimit, bool integer, bool sign) :
    m_acceptableNumChars("\b-.0123456789"),
    m_integer(integer),
    m_signed(sign)
{
    SetRange(minLimit, maxLimit);
}


CNumericEdit::~CNumericEdit()
{
}


void CNumericEdit::Reset(bool integer, bool sign)
{
    if (m_integer != integer || m_signed != sign)
    {
        m_integer = integer;
        m_signed = sign;

        m_minimum.isSet = m_maximum.isSet = false;

        SetWindowText(_T(""));
    }
}


void CNumericEdit::SetMinimum(double minLimit)
{
    if (m_integer)
    {
        ASSERT(minLimit == ::floor(minLimit));  // no fractions allowed for integers
    }

    if (m_signed)
    {
        ASSERT(minLimit <= 0.0);
    }
    else
    {
        ASSERT(minLimit == 0.0);
    }

    if (m_maximum.isSet)
    {
        ASSERT(m_maximum.limit >= minLimit);
    }

    m_minimum.limit = minLimit;
    m_minimum.isSet = true;
}


void CNumericEdit::SetMaximum(double maxLimit)
{
    if (m_integer)
    {
        ASSERT(maxLimit == ::floor(maxLimit));  // no fractions allowed for integers
    }

    ASSERT(maxLimit >= 0.0);

    if (m_minimum.isSet)
    {
        ASSERT(m_minimum.limit <= maxLimit);
    }

    m_maximum.limit = maxLimit;
    m_maximum.isSet = true;
}


void CNumericEdit::SetRange(double minLimit, double maxLimit)
{
    ASSERT(minLimit <= maxLimit);

    m_minimum.isSet = m_maximum.isSet = false;

    SetMinimum(minLimit);
    SetMaximum(maxLimit);
}


BEGIN_MESSAGE_MAP(CNumericEdit, CEdit)
	//{{AFX_MSG_MAP(CNumericEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit message handlers

void CNumericEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   	if (m_acceptableNumChars.Find(nChar) == -1)
	{
        return;
	}

	CString editText;
	GetWindowText(editText);

	int start, end;
	GetSel(start, end);

	if (nChar == '.')
	{
        if (nRepCnt > 1 || m_integer || editText.Find('.') != -1)
        {
		    return;
        }
	}

	if (nChar == '-')
	{
        if (nRepCnt > 1 || !m_signed || start > 0)
        {
            return;
        }

        if (m_minimum.isSet && m_minimum.limit >= 0)
        {
           return;
        }
	}

    if (nChar != '\b' && (m_minimum.isSet || m_maximum.isSet))
    {
        CString newText(editText);
        newText.Delete(start, end - start); // in case the new char is replacing a selection
        newText.Insert(start, nChar);

        if (newText != '.' && newText != '-')
        {
            double value = atof(newText);

            if ((m_minimum.isSet && value < m_minimum.limit) || (m_maximum.isSet && value > m_maximum.limit))
            {
                return;
            }
        }
    }

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

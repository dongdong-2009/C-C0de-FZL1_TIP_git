/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/numeric_edit_control/numericedit.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_NUMERICEDIT_H__296D5648_4FAC_47F3_AEB5_F8B73D7C0870__INCLUDED_)
#define AFX_NUMERICEDIT_H__296D5648_4FAC_47F3_AEB5_F8B73D7C0870__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// numericedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit window
//
// This numeric edit control has range checking capabilities that can be used
// to ensure that the user enters only valid values.
//
// Minimum and/or maximum limits can be specified.
//
// The following restrictions apply:
//
// numeric type     | minimum limit | maximum limit
// -----------------+---------------+--------------
// signed integer   | <= 0          | >= 0
// unsigned integer | == 0          | >= 0
// signed decimal   | <= 0.0        | >= 0.0
// unsigned decimal | == 0.0        | >= 0.0

class AFX_EXT_CLASS CNumericEdit : public CEdit
{
// Construction/destruction
public:
	CNumericEdit(bool integer = false, bool sign = true);
    CNumericEdit(double minLimit, double maxLimit, bool integer = false, bool sign = true);
	virtual ~CNumericEdit();

// Operations
public:
    // Modifies the numeric type and/or sign type.
    // Any of these changes unsets the limits and clears the edit.
    void Reset(bool integer, bool sign);

    void SetMinimum(double minLimit);
    void SetMaximum(double maxLimit);
    void SetRange(double minLimit, double maxLimit);


// Attributes
protected:
	CString		m_acceptableNumChars;
	bool		m_integer;
	bool		m_signed;

    typedef struct
            {
                bool isSet;
                double limit;
            }
            ValueLimit;

    ValueLimit  m_minimum;
    ValueLimit  m_maximum;


// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumericEdit)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumericEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMERICEDIT_H__296D5648_4FAC_47F3_AEB5_F8B73D7C0870__INCLUDED_)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/Notes.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_NOTES_H__AE0282F5_A823_4FD7_91A7_6C8C55B934E1__INCLUDED_)
#define AFX_NOTES_H__AE0282F5_A823_4FD7_91A7_6C8C55B934E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Notes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotes window

class CNotes : public CStatic
{
// Construction
public:
    CNotes();
    CNotes(CString text);

// Attributes
private:
    static bool s_isVisible;
    int m_itemCount;
    CString m_text;
    CToolTipCtrl *m_toolTip;

// Operations
public:
    static void MakeVisible(bool value);
    void SetText(CString text);
    void AddItem(CString item);

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CNotes)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CNotes();

    // Generated message map functions
protected:
    // {{AFX_MSG(CNotes)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG


    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTES_H__AE0282F5_A823_4FD7_91A7_6C8C55B934E1__INCLUDED_)

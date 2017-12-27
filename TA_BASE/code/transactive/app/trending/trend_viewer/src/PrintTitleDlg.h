/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/PrintTitleDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PRINTTITLEDLG_H__CA929EBC_EE7C_40C4_B391_97C0908CF506__INCLUDED_)
#define AFX_PRINTTITLEDLG_H__CA929EBC_EE7C_40C4_B391_97C0908CF506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintTitleDlg.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CPrintTitleDlg dialog

class CPrintTitleDlg : public CDialog
{
// Construction
public:
    CPrintTitleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CPrintTitleDlg)
    enum { IDD = IDD_PRINT_TITLE };
    CEdit    m_printTitleCtrl;
    CString    m_printTitle;
    BOOL    m_changePen;
    BOOL    m_printDateTime;
    //}}AFX_DATA

    CString getPrintTitle()
    {
        return m_printTitle;
    }

    bool doChangePen()
    {
        return m_changePen != 0;
    }

    bool doPrintDateTime()
    {
        return m_printDateTime != 0;
    }

    std::vector<CString> getTitles()
    {
        return m_titles;
    }

   
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPrintTitleDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CPrintTitleDlg)
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnChangeTitle();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:

    std::vector<CString> m_titles;      // Each line of the print title will be stored as one element
    const int m_maxCharsPrintTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTTITLEDLG_H__CA929EBC_EE7C_40C4_B391_97C0908CF506__INCLUDED_)

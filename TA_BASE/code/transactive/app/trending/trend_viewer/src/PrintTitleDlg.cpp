/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/PrintTitleDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PrintTitleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "historyviewer.h"
#include "PrintTitleDlg.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintTitleDlg dialog


CPrintTitleDlg::CPrintTitleDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPrintTitleDlg::IDD, pParent),
      m_maxCharsPrintTitle(100)
{
    //{{AFX_DATA_INIT(CPrintTitleDlg)
    m_printTitle = _T("<No Title>");
    m_changePen = FALSE;
    m_printDateTime = TRUE;
    //}}AFX_DATA_INIT
}


void CPrintTitleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPrintTitleDlg)
    DDX_Control(pDX, IDC_TITLE, m_printTitleCtrl);
    DDX_Text(pDX, IDC_TITLE, m_printTitle);
    //DDV_MaxChars(pDX, m_printTitle, MAX_CHARS);
    DDX_Check(pDX, IDC_CHANGE_PEN, m_changePen);
    DDX_Check(pDX, IDC_DATE_TIME_CHECKBOX, m_printDateTime);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintTitleDlg, CDialog)
    //{{AFX_MSG_MAP(CPrintTitleDlg)
    ON_EN_CHANGE(IDC_TITLE, OnChangeTitle)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintTitleDlg message handlers

void CPrintTitleDlg::OnOK() 
{
    UpdateData(TRUE);

    m_titles.clear();

    int totalLineLength = 0;	

	 for(int i = 0; i < m_printTitleCtrl.GetLineCount(); ++i)
    {
		//TD17749++ clear buffer correctly
		CString buffer;
        int lineLength = m_printTitleCtrl.LineLength(totalLineLength + 2); // Add one for return, add one for index of first char in next line
        totalLineLength += lineLength;

        m_printTitleCtrl.GetLine(i, buffer.GetBuffer(lineLength));
        buffer.ReleaseBuffer();

		//++TD17749
        CString str(buffer.GetBuffer(0));
        if(str == "<No Title>")
        {
            m_titles.push_back("");
            CDialog::OnOK();
            continue;
        }

        m_titles.push_back(str);
		//buffer.ReleaseBuffer(); //TD17749
    }

    CDialog::OnOK();
}

void CPrintTitleDlg::OnCancel() 
{
    m_printTitle = "";
    CDialog::OnCancel();
}

void CPrintTitleDlg::OnChangeTitle() 
{
    FUNCTION_ENTRY("OnChangeTitle");

    // Only allow max of 2 lines
    if(m_printTitleCtrl.GetLineCount() >= 3)
    {
        m_printTitleCtrl.Undo();
    }

    // When two lines are used, the edit control only seems to let you type 98
    // characters. This is probably because it also includes the 2 return characters.
    // However, when you only have one line, it lets you type 100 characters. I'm
    // not really sure why it does this, so this is added here so that 100 characters
    // can be typed regardless of how many lines there are.
    if(m_printTitleCtrl.GetLineCount() == 2)
    {
        m_printTitleCtrl.SetLimitText(m_maxCharsPrintTitle+2);
    }
    else
    {
        m_printTitleCtrl.SetLimitText(m_maxCharsPrintTitle);
    }
    
    FUNCTION_EXIT;
}

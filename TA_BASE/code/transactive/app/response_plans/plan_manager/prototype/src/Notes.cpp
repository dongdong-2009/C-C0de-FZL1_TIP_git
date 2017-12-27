/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/Notes.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// Notes.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "Notes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool CNotes::s_isVisible = true;


/////////////////////////////////////////////////////////////////////////////
// CNotes

CNotes::CNotes() : m_itemCount(0), m_text(""), m_toolTip(NULL)
{
    FUNCTION_ENTRY( "CNotes" );
    FUNCTION_EXIT;
}


CNotes::CNotes(CString text) : m_itemCount(0), m_text(text), m_toolTip(NULL)
{
    FUNCTION_ENTRY( "CNotes" );
    FUNCTION_EXIT;
}


CNotes::~CNotes()
{
    FUNCTION_ENTRY( "~CNotes" );

    if (m_toolTip)
    {
        delete m_toolTip;
    }

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CNotes, CStatic)
    // {{AFX_MSG_MAP(CNotes)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotes message handlers

void CNotes::PreSubclassWindow()
{
    FUNCTION_ENTRY( "PreSubclassWindow" );

    // TODO: Add your specialized code here and/or call the base class
    if (!m_toolTip)
    {
        m_toolTip = new CToolTipCtrl;
        m_toolTip->Create(this);
        m_toolTip->AddTool(this, m_text);
        m_toolTip->SetDelayTime(TTDT_INITIAL, 0);
        m_toolTip->SetDelayTime(TTDT_AUTOPOP, SHRT_MAX);
        m_toolTip->SetMaxTipWidth(400);
    }

    ShowWindow(s_isVisible? SW_SHOW : SW_HIDE);

    CStatic::PreSubclassWindow();

    FUNCTION_EXIT;
}


BOOL CNotes::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_MOUSEMOVE && m_toolTip)
    {
        m_toolTip->RelayEvent(pMsg);
    }

    FUNCTION_EXIT;
    return CStatic::PreTranslateMessage(pMsg);
}


void CNotes::MakeVisible(bool value)
{
    FUNCTION_ENTRY( "MakeVisible" );

    s_isVisible = value;

    FUNCTION_EXIT;
}


void CNotes::SetText(CString text)
{
    FUNCTION_ENTRY( "SetText" );

    m_text = text;

    if (m_toolTip)
    {
        m_toolTip->UpdateTipText(m_text, this);
    }

    FUNCTION_EXIT;
}


void CNotes::AddItem(CString item)
{
    FUNCTION_ENTRY( "AddItem" );

    if (m_text.GetLength() > 0)
    {
        m_text += "\n--------------\n";
    }

    CString numberedItem;
    numberedItem.Format("%d. %s", ++m_itemCount, (LPCTSTR) item);

    m_text += numberedItem;

    if (m_toolTip)
    {
        m_toolTip->UpdateTipText(m_text, this);
    }

    FUNCTION_EXIT;
}



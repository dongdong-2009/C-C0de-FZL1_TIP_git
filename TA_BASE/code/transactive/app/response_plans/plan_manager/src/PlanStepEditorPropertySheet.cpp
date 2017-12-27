/**
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
  * Manages the General and Parameters tabs in the Step Editor. Allows individual tabs to be flagged
  * as disabled when adding the corresponding property page.
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanStepEditorPropertySheet.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorPropertySheet

IMPLEMENT_DYNAMIC(CPlanStepEditorPropertySheet, CPropertySheet)

CPlanStepEditorPropertySheet::CPlanStepEditorPropertySheet()
{
    FUNCTION_ENTRY( "CPlanStepEditorPropertySheet" );
    FUNCTION_EXIT;
}


CPlanStepEditorPropertySheet::CPlanStepEditorPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) :
    CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    FUNCTION_ENTRY( "CPlanStepEditorPropertySheet" );
    FUNCTION_EXIT;
}


CPlanStepEditorPropertySheet::CPlanStepEditorPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) :
    CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    FUNCTION_ENTRY( "CPlanStepEditorPropertySheet" );
    FUNCTION_EXIT;
}


CPlanStepEditorPropertySheet::~CPlanStepEditorPropertySheet()
{
    FUNCTION_ENTRY( "~CPlanStepEditorPropertySheet" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditorPropertySheet, CPropertySheet)
    // {{AFX_MSG_MAP(CPlanStepEditorPropertySheet)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorPropertySheet message handlers

BOOL CPlanStepEditorPropertySheet::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    BOOL bRet = CPropertySheet::OnInitDialog();

    m_tabCtrl.SubclassDlgItem(GetTabControl()->GetDlgCtrlID(), this);

    ApplicationFont::getInstance().adjustChildrenFont( m_tabCtrl.GetSafeHwnd() );

    FUNCTION_EXIT;
    return bRet;
}


BOOL CPlanStepEditorPropertySheet::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    if (m_tabCtrl.TranslatePropSheetMsg(pMsg))
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return CPropertySheet::PreTranslateMessage(pMsg);
}


void CPlanStepEditorPropertySheet::Add(CPropertyPage* propertyPage, bool tabEnabled)
{
    FUNCTION_ENTRY( "Add" );

    if (propertyPage)
    {
        AddPage(propertyPage);

        if (!tabEnabled)
        {
            m_tabCtrl.DisableTab(GetPageIndex(propertyPage));
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditorPropertySheet::Activate(CWnd* pParentWnd)
{
    FUNCTION_ENTRY( "Activate" );

    TA_ASSERT(pParentWnd, "Parent window is NULL");
    TA_ASSERT(GetPageCount() > 0, "No pages have been added");

    Create(pParentWnd, WS_CHILD | WS_VISIBLE, WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT);
    SetWindowPos(NULL,2,2,0,0,SWP_NOZORDER|SWP_NOSIZE);

    CPropertyPage* activePage = GetActivePage();
    TA_ASSERT(activePage, "Active page is NULL");

    CRect activePageRect;
    activePage->GetWindowRect(activePageRect);
    pParentWnd->ScreenToClient(activePageRect);
    activePage->MoveWindow(activePageRect);

    FUNCTION_EXIT;
}



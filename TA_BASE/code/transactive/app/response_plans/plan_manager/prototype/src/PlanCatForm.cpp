/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanCatForm.cpp : implementation file
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanCatForm.h"
#include "PlanCatEditMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanCatForm

IMPLEMENT_DYNCREATE(CPlanCatForm, CFormView)

CPlanCatForm::CPlanCatForm()
    : CFormView(CPlanCatForm::IDD), m_readOnly(true)
{
    FUNCTION_ENTRY( "CPlanCatForm" );

    // {{AFX_DATA_INIT(CPlanCatForm)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    m_notes.AddItem("Double-clicking on the category in the tree brings up the category edit window.");
    m_notes.AddItem("Edit functionality will be available only to users with plan editing rights.");

    FUNCTION_EXIT;
}


CPlanCatForm::~CPlanCatForm()
{
    FUNCTION_ENTRY( "~CPlanCatForm" );
    FUNCTION_EXIT;
}


void CPlanCatForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanCatForm)
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    DDX_Control(pDX, IDC_CATEGORY_HEADER, m_categoryHeader);
    DDX_Control(pDX, IDC_CATEGORY_NAME_EDIT, m_categoryNameEdit);
    DDX_Control(pDX, IDC_CATEGORY_DESCRIPTION_EDIT, m_categoryDescEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanCatForm, CFormView)
    // {{AFX_MSG_MAP(CPlanCatForm)
    ON_EN_CHANGE(IDC_CATEGORY_DESCRIPTION_EDIT, OnChangeCategoryDescriptionEdit)
    ON_EN_CHANGE(IDC_CATEGORY_NAME_EDIT, OnChangeCategoryNameEdit)
    ON_WM_SIZE()
    ON_COMMAND(ID_CAT_EXIT, OnCatExit)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanCatForm diagnostics

#ifdef _DEBUG
void CPlanCatForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CPlanCatForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanCatForm message handlers

void CPlanCatForm::OnChangeCategoryDescriptionEdit()
{
    FUNCTION_ENTRY( "OnChangeCategoryDescriptionEdit" );
    FUNCTION_EXIT;
}


void CPlanCatForm::OnChangeCategoryNameEdit()
{
    FUNCTION_ENTRY( "OnChangeCategoryNameEdit" );
    FUNCTION_EXIT;
}


void CPlanCatForm::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFormView::OnSize(nType, cx, cy);

    if (::IsWindow(m_categoryHeader.m_hWnd))
    {
        CRect tmpRect;

        CWnd* pHeaderWnd = GetDlgItem(IDC_CATEGORY_HEADER);
        m_categoryHeader.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx;
        m_categoryHeader.MoveWindow(tmpRect);

        m_categoryNameEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        m_categoryNameEdit.MoveWindow(tmpRect);

        m_categoryDescEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        tmpRect.bottom = cy - 10;
        m_categoryDescEdit.MoveWindow(tmpRect);
    }

    FUNCTION_EXIT;
}


void CPlanCatForm::PopulateData(const PV::Category *category)
{
    FUNCTION_ENTRY( "PopulateData" );

    CString catHeaderText;
    catHeaderText.Format("Plan Category - %s", category->m_name.c_str());
    m_categoryHeader.SetWindowText(catHeaderText);
    m_categoryNameEdit.SetWindowText(category->m_name.c_str());
    m_categoryDescEdit.SetWindowText(category->m_description.c_str());

    FUNCTION_EXIT;
}


void CPlanCatForm::EnableEditing()
{
    FUNCTION_ENTRY( "EnableEditing" );

    m_readOnly = false;

    m_categoryNameEdit.SetReadOnly(FALSE);
    m_categoryDescEdit.SetReadOnly(FALSE);

    ResetMenusAndToolbars();

    FUNCTION_EXIT;
}


void CPlanCatForm::ResetMenusAndToolbars()
{
    FUNCTION_ENTRY( "ResetMenusAndToolbars" );

    CPlanCatEditMainFrame *ceFrame = dynamic_cast<CPlanCatEditMainFrame *>(GetParentFrame());
    if (ceFrame && !m_readOnly)
    {
        ceFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_CAT_SAVE, TRUE);
        ceFrame->GetMenu()->EnableMenuItem(ID_CAT_SAVE, MF_BYCOMMAND | MF_ENABLED);
    }

    FUNCTION_EXIT;
}


void CPlanCatForm::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CFormView::OnInitialUpdate();

    // TODO: Add your specialized code here and/or call the base class
    CFont *font = m_categoryHeader.GetFont();
    if (font != NULL)
    {
        LOGFONT fontInfo;
        font->GetLogFont(&fontInfo);

        if (fontInfo.lfWeight != FW_BOLD)
        {
            fontInfo.lfWeight = FW_BOLD;

            if (m_boldFont.CreateFontIndirect(&fontInfo) == TRUE)
            {
                m_categoryHeader.SetFont(&m_boldFont);
            }
        }
    }

    FUNCTION_EXIT;
}


void CPlanCatForm::OnCatExit()
{
    FUNCTION_ENTRY( "OnCatExit" );

    // TODO: Add your command handler code here
    GetParentFrame()->DestroyWindow();

    FUNCTION_EXIT;
}



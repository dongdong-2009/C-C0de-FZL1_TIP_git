/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlansTreeView.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlansTreeView.cpp : implementation of the CPlansTreeView class
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanViewerDoc.h"
#include "PlansTreeView.h"
#include "BlankForm.h"
#include "PlanCatForm.h"
#include "PlanStepForm.h"
#include "PlanForm.h"
#include "AllPlansForm.h"
#include "ActivePlansForm.h"
#include "ApprovedPlansForm.h"
#include "UnapprovedPlansForm.h"
#include "MainFrm.h"
#include "PlanCatEditDoc.h"
#include "PlanCatEditMainFrm.h"
#include "PlanEditDoc.h"
#include "PlanEditMainFrm.h"
#include "PlanControlDoc.h"
#include "PlanControlMainFrm.h"
#include "globals.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
    typedef struct
    {
        UINT m_normal;
        UINT m_selected;
        UINT m_collapsed;
        UINT m_expanded;
    } NodeImg;
}

static enum
{
    VIEWER_BLANK = 0,
    VIEWER_PLANS_ALL,
    VIEWER_PLANS_ACTIVE,
    VIEWER_PLANS_APPROVED,
    VIEWER_PLANS_UNAPPROVED,
    VIEWER_CATEGORY,
    VIEWER_PLAN_APPROVED,
    VIEWER_PLAN_UNAPPROVED,
};

static CRuntimeClass* NODE_VIEWER_FORMS[] =
{
    RUNTIME_CLASS(CBlankForm),
    RUNTIME_CLASS(CAllPlansForm),
    RUNTIME_CLASS(CActivePlansForm),
    RUNTIME_CLASS(CApprovedPlansForm),
    RUNTIME_CLASS(CUnapprovedPlansForm),
    RUNTIME_CLASS(CBlankForm),
    RUNTIME_CLASS(CPlanForm),
    RUNTIME_CLASS(CPlanForm),
};

static NodeImg NODE_IMAGES[] =
{
    { IDI_BLANK_ICON, IDI_BLANK_ICON, IDI_BLANK_ICON, IDI_BLANK_ICON },
    { IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON }, // All Plans category
    { IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON }, // Active Plans category
    { IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON }, // Approved Plans category
    { IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON, IDI_ROOT_ICON }, // Unapproved Plans category
    { IDI_FOLDERCLOSED_ICON, IDI_FOLDERCLOSED_ICON, IDI_FOLDERCLOSED_ICON, IDI_FOLDEROPEN_ICON }, // Generic category
#if 0
    { IDI_TICK_ICON, IDI_TICK_ICON, IDI_TICK_ICON, IDI_TICK_ICON },     // Approved plan
    { IDI_BLANK_ICON, IDI_BLANK_ICON, IDI_BLANK_ICON, IDI_BLANK_ICON }, // Unapproved plan
#else
    { IDI_TICK_ICON, IDI_TICK_ICON, IDI_TICK_ICON, IDI_TICK_ICON },     // Approved plan
    { IDI_CROSS_ICON, IDI_CROSS_ICON, IDI_CROSS_ICON, IDI_CROSS_ICON }, // Unapproved plan
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CPlansTreeView


IMPLEMENT_DYNCREATE(CPlansTreeView, CTreeView)

BEGIN_MESSAGE_MAP(CPlansTreeView, CTreeView)
    // {{AFX_MSG_MAP(CPlansTreeView)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
    ON_COMMAND(ID_CAT_EDIT, OnEditCommand)
    ON_COMMAND(ID_PLAN_EDIT, OnEditCommand)
    ON_COMMAND(ID_PLAN_CONTROL, OnPlanControl)
    // }}AFX_MSG_MAP
    // ON_MESSAGE(TIMER_START_BUSINESS, OnStartBusiness)
      ON_COMMAND_RANGE(ID_VIEW_ALLPLANS, ID_VIEW_UNAPPROVEDPLANS, ShowView)
    ON_MESSAGE(WM_RESET_MENUS_AND_TBARS, OnResetMenusAndTbars)
    ON_MESSAGE(WM_EDIT_CATEGORY, OnEditCategory)
    ON_MESSAGE(WM_EDIT_PLAN, OnEditPlan)
    ON_MESSAGE(WM_CONTROL_PLAN, OnControlPlan)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlansTreeView construction/destruction

CPlansTreeView::CPlansTreeView() : m_currentView(0), m_catMenu(NULL), m_planMenu(NULL)
{
    FUNCTION_ENTRY( "CPlansTreeView" );
    FUNCTION_EXIT;
}


CPlansTreeView::~CPlansTreeView()
{
    FUNCTION_ENTRY( "~CPlansTreeView" );
    FUNCTION_EXIT;
}


BOOL CPlansTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Modify the Window class or styles here by modifying
    // the CREATESTRUCT cs
    FUNCTION_EXIT;
    return CTreeView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CPlansTreeView drawing

void CPlansTreeView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );

    CPlanViewerDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // TODO: add draw code for native data here

    FUNCTION_EXIT;
}


void CPlansTreeView::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CTreeView::OnInitialUpdate();

    m_startBusinessTimer = SetTimer(TIMER_START_BUSINESS, 300, 0);

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlansTreeView diagnostics

#ifdef _DEBUG
void CPlansTreeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CTreeView::AssertValid();

    FUNCTION_EXIT;
}


void CPlansTreeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CTreeView::Dump(dc);

    FUNCTION_EXIT;
}


CPlanViewerDoc* CPlansTreeView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlanViewerDoc)));

    FUNCTION_EXIT;
    return (CPlanViewerDoc*)m_pDocument;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPlansTreeView message handlers

int CPlansTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CTreeView::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    m_treeImageList.Create(20, 20, ILC_MASK, 5, 5);
    for (int i = 0; i < sizeof(NODE_IMAGES) / sizeof(NodeImg); i++)
    {
        // Icon resource ID is replaced with the corresponding image list index
        NODE_IMAGES[i].m_normal    = m_treeImageList.Add(AfxGetApp()->LoadIcon(NODE_IMAGES[i].m_normal));
        NODE_IMAGES[i].m_selected  = m_treeImageList.Add(AfxGetApp()->LoadIcon(NODE_IMAGES[i].m_selected));
        NODE_IMAGES[i].m_collapsed = m_treeImageList.Add(AfxGetApp()->LoadIcon(NODE_IMAGES[i].m_collapsed));
        NODE_IMAGES[i].m_expanded  = m_treeImageList.Add(AfxGetApp()->LoadIcon(NODE_IMAGES[i].m_expanded));
    }
    GetTreeCtrl().SetImageList(&m_treeImageList, TVSIL_NORMAL);

    ModifyStyle(0, WS_TABSTOP | TVS_INFOTIP | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS);

    FUNCTION_EXIT;
    return 0;
}


void CPlansTreeView::OnTimer(UINT nIDEvent)
{
    FUNCTION_ENTRY( "OnTimer" );

    switch(nIDEvent)
    {
        case TIMER_START_BUSINESS:
            KillTimer(m_startBusinessTimer);
            ShowView(ID_VIEW_ALLPLANS);
            break;

        case TIMER_SELECTION_CHANGED:
            KillTimer(m_selectionTimer);
            UpdateView();
            break;

        default:
            break;
    }

    CTreeView::OnTimer(nIDEvent);

    FUNCTION_EXIT;
}


void CPlansTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnSelchanged" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    m_selectionTimer = SetTimer(TIMER_SELECTION_CHANGED, pNMTreeView->action == TVC_BYKEYBOARD? 300 : 0, 0);

    *pResult = 0;

    FUNCTION_EXIT;
}


// Change item image when its expansion state changes.
void CPlansTreeView::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemexpanded" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    UINT action = pNMTreeView->action;
    TVITEM item = pNMTreeView->itemNew;
    item.mask |= TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    GetTreeCtrl().GetItem(&item);

    int viewerType;
    unsigned long pkey;

    SplitItemData((DWORD) item.lParam, viewerType, pkey);

    if (action == TVE_EXPAND && item.iImage != NODE_IMAGES[viewerType].m_expanded)
    {
        item.iImage = item.iSelectedImage = NODE_IMAGES[viewerType].m_expanded;
        GetTreeCtrl().SetItem(&item);
    }
    else if (action == TVE_COLLAPSE && item.iImage != NODE_IMAGES[viewerType].m_collapsed)
    {
        item.iImage = item.iSelectedImage = NODE_IMAGES[viewerType].m_collapsed;
        GetTreeCtrl().SetItem(&item);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CPlansTreeView::OnResetMenusAndTbars(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnResetMenusAndTbars" );

    LRESULT result = 0;
    CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    VERIFY(pMainFrame);
    CToolBarCtrl *pBar = &pMainFrame->m_wndToolBar.GetToolBarCtrl();
    VERIFY(pBar);
    CMenu *menu = pMainFrame->GetMenu();
    VERIFY(menu);

    HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();
    DWORD itemData = GetTreeCtrl().GetItemData(selItem);
    int viewerType;
    unsigned long pkey;
    SplitItemData(itemData, viewerType, pkey);


    BOOL approvedPlanNode = viewerType == VIEWER_PLAN_APPROVED;

    BOOL allPlansView = m_currentView == ID_VIEW_ALLPLANS;
    BOOL activePlansView = m_currentView == ID_VIEW_ACTIVEPLANS;
    BOOL approvedPlansView = m_currentView == ID_VIEW_APPROVEDPLANS;
    BOOL unapprovedPlansView = m_currentView == ID_VIEW_UNAPPROVEDPLANS;

    BOOL blankNode = wParam == VIEWER_BLANK;
    BOOL rootNode  = wParam >= VIEWER_PLANS_ALL && wParam <= VIEWER_PLANS_UNAPPROVED;
    BOOL catNode   = wParam == VIEWER_CATEGORY;
    BOOL planNode  = wParam == VIEWER_PLAN_APPROVED || wParam == VIEWER_PLAN_UNAPPROVED;

    // When viewing active plans, none of the buttons on the toolbar are available.
    pBar->ShowWindow(!activePlansView);
    pMainFrame->RecalcLayout(FALSE);

    pBar->HideButton(ID_CAT_NEW, !rootNode && !catNode);       pBar->EnableButton(ID_CAT_NEW, rootNode || catNode);
    pBar->HideButton(ID_CAT_DELETE, !rootNode && !catNode);    pBar->EnableButton(ID_CAT_DELETE, catNode);
    pBar->HideButton(ID_CAT_EDIT, !rootNode && !catNode);      pBar->EnableButton(ID_CAT_EDIT, catNode);
    pBar->HideButton(ID_SEP_CAT, !rootNode && !catNode);       pBar->EnableButton(ID_SEP_CAT, TRUE);

    pBar->HideButton(ID_PLAN_NEW, !rootNode && !catNode);       pBar->EnableButton(ID_PLAN_NEW, TRUE);
    pBar->HideButton(ID_PLAN_DELETE, !planNode);                pBar->EnableButton(ID_PLAN_DELETE, !approvedPlanNode);
    pBar->HideButton(ID_PLAN_CUT, !planNode);                   pBar->EnableButton(ID_PLAN_CUT, !approvedPlanNode);
    pBar->HideButton(ID_PLAN_COPY, !planNode);                  pBar->EnableButton(ID_PLAN_COPY, TRUE);
    pBar->HideButton(ID_PLAN_PASTE, !rootNode && !catNode);     pBar->EnableButton(ID_PLAN_PASTE, rootNode || catNode);
    pBar->HideButton(ID_PLAN_EDIT, !planNode);                  pBar->EnableButton(ID_PLAN_EDIT, !approvedPlanNode);
    pBar->HideButton(ID_SEP_PLAN1, !planNode);                  pBar->EnableButton(ID_SEP_PLAN1, TRUE);

    pBar->HideButton(ID_PLAN_APPROVE, !planNode);               pBar->EnableButton(ID_PLAN_APPROVE, !approvedPlanNode);
    pBar->HideButton(ID_PLAN_UNAPPROVE, !planNode);             pBar->EnableButton(ID_PLAN_UNAPPROVE, approvedPlanNode);
    pBar->HideButton(ID_SEP_PLAN2, !planNode);                  pBar->EnableButton(ID_SEP_PLAN2, TRUE);

    pBar->HideButton(ID_PLAN_SCHEDULE, !planNode);              pBar->EnableButton(ID_PLAN_SCHEDULE, approvedPlanNode);
    pBar->HideButton(ID_PLAN_CONTROL, !planNode);               pBar->EnableButton(ID_PLAN_CONTROL, approvedPlanNode);

    // When viewing active plans, the Category and Plan menus are not available.
    if (activePlansView)
    {
        if (m_catMenu == NULL && m_planMenu == NULL)
        {
            m_catMenu = menu->GetSubMenu(2);
            m_planMenu = menu->GetSubMenu(3);
            menu->RemoveMenu(2, MF_BYPOSITION);
            menu->RemoveMenu(2, MF_BYPOSITION);
            pMainFrame->DrawMenuBar();
        }
    }
    else
    {
        if (m_catMenu != NULL && m_planMenu != NULL)
        {
            menu->AppendMenu(MF_POPUP, (UINT) m_catMenu->GetSafeHmenu(), "&Category");
            menu->AppendMenu(MF_POPUP, (UINT) m_planMenu->GetSafeHmenu(), "&Plan");
            m_catMenu = m_planMenu = NULL;
            pMainFrame->DrawMenuBar();
        }
    }

    menu->EnableMenuItem(ID_CAT_NEW, (rootNode || catNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_CAT_DELETE, (catNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_CAT_EDIT, (catNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);

    menu->EnableMenuItem(ID_PLAN_NEW, (rootNode || catNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_DELETE, (approvedPlanNode? MF_GRAYED : MF_ENABLED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_CUT, (approvedPlanNode? MF_GRAYED : MF_ENABLED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_COPY, (planNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_PASTE, (rootNode || catNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_EDIT, (approvedPlanNode? MF_GRAYED : MF_ENABLED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_SCHEDULE, (approvedPlanNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    menu->EnableMenuItem(ID_PLAN_CONTROL, (approvedPlanNode? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);

    FUNCTION_EXIT;
    return result;

}


void CPlansTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclk" );

    // TODO: Add your control notification handler code here
    OnEditCommand();

    *pResult = 1; // prevent the node expanding/collapsing

    FUNCTION_EXIT;
}


void CPlansTreeView::OnEditCommand()
{
    FUNCTION_ENTRY( "OnEditCommand" );

    // TODO: Add your command handler code here
    HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();
    DWORD itemData = GetTreeCtrl().GetItemData(selItem);

    int viewerType;
    unsigned long pkey;

    SplitItemData(itemData, viewerType, pkey);

    if (viewerType == VIEWER_CATEGORY)
    {
        PostMessage(WM_EDIT_CATEGORY, pkey);
    }
    else if (viewerType == VIEWER_PLAN_APPROVED || viewerType == VIEWER_PLAN_UNAPPROVED)
    {
        PostMessage(WM_EDIT_PLAN, pkey);
    }

    FUNCTION_EXIT;
}


void CPlansTreeView::OnPlanControl()
{
    FUNCTION_ENTRY( "OnPlanControl" );

    // TODO: Add your command handler code here
     HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();
    DWORD itemData = GetTreeCtrl().GetItemData(selItem);

    int viewerType;
    unsigned long pkey;

    SplitItemData(itemData, viewerType, pkey);

    ASSERT(viewerType == VIEWER_PLAN_APPROVED);
    PostMessage(WM_CONTROL_PLAN, pkey);

    FUNCTION_EXIT;
}


LRESULT CPlansTreeView::OnEditCategory(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnEditCategory" );

    unsigned long pkey = wParam;
    PV::Category *category = CPvData::instance().getCategory(pkey);
    ASSERT(category != NULL);

    CString planCatFileName;
    planCatFileName.Format("%u.cat", category->m_pkey);

    CPlanCatEditDoc *planCatEditDoc = NULL;

    // Q: Is there a simpler way to create/open a document based on document type???
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    while (pos && !planCatEditDoc)
    {
        CDocTemplate* docTemplate = AfxGetApp()->GetNextDocTemplate(pos);

        CDocTemplate::Confidence conf = docTemplate->MatchDocType(planCatFileName, (CDocument *&) planCatEditDoc);
        if (conf == CDocTemplate::Confidence::yesAttemptNative)
        {
            POSITION docPos = docTemplate->GetFirstDocPosition();
            while (docPos && !planCatEditDoc)
            {
                CPlanCatEditDoc *openDoc = (CPlanCatEditDoc *) docTemplate->GetNextDoc(docPos);
                if (planCatFileName == openDoc->GetPathName().Right(planCatFileName.GetLength()))
                {
                    planCatEditDoc = openDoc;
                }
            }

            if (!planCatEditDoc)
            {
                planCatEditDoc = (CPlanCatEditDoc *) docTemplate->OpenDocumentFile(NULL);
                planCatEditDoc->SetPathName(planCatFileName, FALSE);
                planCatEditDoc->SetTitle((LPCTSTR) category->m_name.c_str());
            }
        }
    }
    ASSERT(planCatEditDoc != NULL);

    pos = planCatEditDoc->GetFirstViewPosition();
    if (pos != NULL)
    {
        CPlanCatForm* planCatEditForm = (CPlanCatForm *) planCatEditDoc->GetNextView(pos);
        if (planCatEditForm != NULL)
        {
            planCatEditForm->PopulateData(category);
            planCatEditForm->SetFocus();
            planCatEditForm->EnableEditing();
        }
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlansTreeView::OnEditPlan(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnEditPlan" );

    unsigned long pkey = wParam;
    PV::Plan *plan = CPvData::instance().getPlan(pkey);
    ASSERT(plan != NULL);

    CString planFileName;
    planFileName.Format("%u.eplan", plan->m_pkey);

    CPlanEditDoc *planEditDoc = NULL;

    // Q: Is there a simpler way to create/open a document based on document type???
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    while (pos && !planEditDoc)
    {
        CDocTemplate* docTemplate = AfxGetApp()->GetNextDocTemplate(pos);

        CDocTemplate::Confidence conf = docTemplate->MatchDocType(planFileName, (CDocument *&) planEditDoc);
        if (conf == CDocTemplate::Confidence::yesAttemptNative)
        {
            POSITION docPos = docTemplate->GetFirstDocPosition();
            while (docPos && !planEditDoc)
            {
                CPlanEditDoc *openDoc = (CPlanEditDoc *) docTemplate->GetNextDoc(docPos);
                if (planFileName == openDoc->GetPathName().Right(planFileName.GetLength()))
                {
                    planEditDoc = openDoc;
                }
            }

            if (!planEditDoc)
            {
                planEditDoc = (CPlanEditDoc *) docTemplate->OpenDocumentFile(NULL);
                planEditDoc->SetPathName(planFileName, FALSE);
                planEditDoc->SetTitle((LPCTSTR) plan->m_name.c_str());
            }
        }
    }
    ASSERT(planEditDoc != NULL);

    pos = planEditDoc->GetFirstViewPosition();
    if (pos != NULL)
    {
        CPlanForm* planForm = (CPlanForm *) planEditDoc->GetNextView(pos);
        if (planForm != NULL)
        {
            planForm->PopulateData(plan);
            planForm->EnableEditing();
            planForm->Resize();
            planForm->SetFocus();
        }
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlansTreeView::OnControlPlan(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnControlPlan" );

    unsigned long pkey = wParam;
    PV::Plan *plan = CPvData::instance().getPlan(pkey);
    ASSERT(plan != NULL);

    CString planFileName;
    planFileName.Format("%u.cplan", plan->m_pkey);

    CPlanControlDoc *planCtrlDoc = NULL;

    // Q: Is there a simpler way to create/open a document based on document type???
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    while (pos && !planCtrlDoc)
    {
        CDocTemplate* docTemplate = AfxGetApp()->GetNextDocTemplate(pos);

        CDocTemplate::Confidence conf = docTemplate->MatchDocType(planFileName, (CDocument *&) planCtrlDoc);
        if (conf == CDocTemplate::Confidence::yesAttemptNative)
        {
            POSITION docPos = docTemplate->GetFirstDocPosition();
            while (docPos && !planCtrlDoc)
            {
                CPlanControlDoc *openDoc = (CPlanControlDoc *) docTemplate->GetNextDoc(docPos);
                if (planFileName == openDoc->GetPathName().Right(planFileName.GetLength()))
                {
                    planCtrlDoc = openDoc;
                }
            }

            if (!planCtrlDoc)
            {
                planCtrlDoc = (CPlanControlDoc *) docTemplate->OpenDocumentFile(NULL);
                planCtrlDoc->SetPathName(planFileName, FALSE);
                planCtrlDoc->SetTitle((LPCTSTR) plan->m_name.c_str());
            }
        }
    }
    ASSERT(planCtrlDoc != NULL);

    pos = planCtrlDoc->GetFirstViewPosition();
    if (pos != NULL)
    {
        CPlanForm* planForm = (CPlanForm *) planCtrlDoc->GetNextView(pos);
        if (planForm != NULL)
        {
            planForm->PopulateData(plan);
            planForm->EnableControl();
            planForm->Resize();
            planForm->SetFocus();
        }
    }

    FUNCTION_EXIT;
    return 0;
}


// The 32-bit data associated with each tree item serves a dual purpose:
// 1. store the type of the node (category, plan, step etc).
// 2. store the pkey of the associated plan component
DWORD CPlansTreeView::CreateItemData(int viewerType, unsigned long pkey)
{
    FUNCTION_ENTRY( "CreateItemData" );

    DWORD itemData = (viewerType & 0xF) << 28;
    itemData |= (pkey & 0x0FFFFFFF);

    FUNCTION_EXIT;
    return itemData;
}


void CPlansTreeView::SplitItemData(DWORD itemData, int &viewerType, unsigned long &pkey)
{
    FUNCTION_ENTRY( "SplitItemData" );

    viewerType = (itemData >> 28) & 0xF;
    pkey = itemData & 0x0FFFFFFF;

    FUNCTION_EXIT;
}


void CPlansTreeView::PopulateTreeWithAllPlans()
{
    FUNCTION_ENTRY( "PopulateTreeWithAllPlans" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    CPvData &pvData = CPvData::instance();

    // Purge the existing contents of the tree.
    treeCtrl.DeleteAllItems();

    // Create a root node. Leave the right hand pane blank for the time being.
    HTREEITEM rootItem = treeCtrl.InsertItem("All Plans", NODE_IMAGES[VIEWER_PLANS_ALL].m_normal, NODE_IMAGES[VIEWER_PLANS_ALL].m_selected);
    treeCtrl.SetItemData(rootItem, CreateItemData(VIEWER_PLANS_ALL, 0));
    PostMessage(WM_RESET_MENUS_AND_TBARS, VIEWER_PLANS_ALL);

    PV::CategoryList categories;
    pvData.getAllCategories(categories);

    // Insert all categories
    for (PV::CategoryList::iterator citer = categories.begin(); citer != categories.end(); citer++)
    {
        PV::Category *category = *citer;

        HTREEITEM cItem = treeCtrl.InsertItem(category->m_name.c_str(), NODE_IMAGES[VIEWER_CATEGORY].m_normal, NODE_IMAGES[VIEWER_CATEGORY].m_selected, rootItem, TVI_LAST);
        treeCtrl.SetItemData(cItem, CreateItemData(VIEWER_CATEGORY, category->m_pkey));

        PV::PlanList plans;
        pvData.getPlansForCategory(plans, category->m_pkey);

        // Insert all plans under the given category
        for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
        {
            PV::Plan *plan = *piter;

            int planViewer = plan->m_isApproved? VIEWER_PLAN_APPROVED : VIEWER_PLAN_UNAPPROVED;
            HTREEITEM pItem = treeCtrl.InsertItem(plan->m_name.c_str(), NODE_IMAGES[planViewer].m_normal, NODE_IMAGES[planViewer].m_selected, cItem, TVI_LAST);
            treeCtrl.SetItemData(pItem, CreateItemData(planViewer, plan->m_pkey));
        }
    }

    // Expand and select the top node.
    treeCtrl.Expand(rootItem, TVE_EXPAND);
    treeCtrl.SelectItem(rootItem);

    FUNCTION_EXIT;
}


void CPlansTreeView::PopulateTreeWithActivePlans()
{
    FUNCTION_ENTRY( "PopulateTreeWithActivePlans" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    CPvData &pvData = CPvData::instance();

    // Purge the existing contents of the tree.
    treeCtrl.DeleteAllItems();

    // Create a root node. Leave the right hand pane blank for the time being.
    HTREEITEM rootItem = treeCtrl.InsertItem("Active Plans", NODE_IMAGES[VIEWER_PLANS_ACTIVE].m_normal, NODE_IMAGES[VIEWER_PLANS_ACTIVE].m_selected);
    treeCtrl.SetItemData(rootItem, CreateItemData(VIEWER_PLANS_ACTIVE, 0));
    PostMessage(WM_RESET_MENUS_AND_TBARS, VIEWER_PLANS_ACTIVE);

    PV::PlanList plans;
    pvData.getActivePlans(plans);

    if (plans.size() == 0)
    {
        HTREEITEM pItem = treeCtrl.InsertItem("There are no active plans", NODE_IMAGES[VIEWER_BLANK].m_normal, NODE_IMAGES[VIEWER_BLANK].m_selected, rootItem, TVI_FIRST);
        treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLANS_ACTIVE, 0));
    }
    else
    {
        // Insert all active plans under the root
        for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
        {
            PV::Plan *plan = *piter;

            CString label;
            label.Format("%s [%d]", plan->m_name.c_str(), plan->m_instances.size());

            HTREEITEM pItem = treeCtrl.InsertItem(label, NODE_IMAGES[VIEWER_PLAN_APPROVED].m_normal, NODE_IMAGES[VIEWER_PLAN_APPROVED].m_selected, rootItem, TVI_SORT);
            treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLAN_APPROVED, plan->m_pkey));
        }
    }

    // Expand and select the top node.
    treeCtrl.Expand(rootItem, TVE_EXPAND);
    treeCtrl.SelectItem(rootItem);

    FUNCTION_EXIT;
}


void CPlansTreeView::PopulateTreeWithApprovedPlans()
{
    FUNCTION_ENTRY( "PopulateTreeWithApprovedPlans" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    CPvData &pvData = CPvData::instance();

    // Purge the existing contents of the tree.
    treeCtrl.DeleteAllItems();

    // Create a root node. Leave the right hand pane blank for the time being.
    HTREEITEM rootItem = treeCtrl.InsertItem("Approved Plans", NODE_IMAGES[VIEWER_PLANS_APPROVED].m_normal, NODE_IMAGES[VIEWER_PLANS_APPROVED].m_selected);
    treeCtrl.SetItemData(rootItem, CreateItemData(VIEWER_PLANS_APPROVED, 0));
    PostMessage(WM_RESET_MENUS_AND_TBARS, VIEWER_PLANS_APPROVED);

    PV::PlanList plans;
    pvData.getApprovedPlans(plans);

    if (plans.size() == 0)
    {
        HTREEITEM pItem = treeCtrl.InsertItem("There are no approved plans", NODE_IMAGES[VIEWER_BLANK].m_normal, NODE_IMAGES[VIEWER_BLANK].m_selected, rootItem, TVI_FIRST);
        treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLANS_APPROVED, 0));
    }
    else
    {
        PV::CategoryList categories;
        pvData.getAllCategories(categories);

        // Insert all categories containing approved plans
        for (PV::CategoryList::iterator citer = categories.begin(); citer != categories.end(); citer++)
        {
            PV::Category *category = *citer;
            HTREEITEM cItem = NULL;

            plans.clear();
            pvData.getPlansForCategory(plans, category->m_pkey);

            // Insert all approved plans under the given category
            for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
            {
                PV::Plan *plan = *piter;

                if (plan->m_isApproved == true)
                {
                    // Insert the category if necessary
                    if (cItem == NULL)
                    {
                        cItem = treeCtrl.InsertItem(category->m_name.c_str(), NODE_IMAGES[VIEWER_CATEGORY].m_normal, NODE_IMAGES[VIEWER_CATEGORY].m_selected, rootItem, TVI_LAST);
                        treeCtrl.SetItemData(cItem, CreateItemData(VIEWER_CATEGORY, category->m_pkey));
                    }
                    HTREEITEM pItem = treeCtrl.InsertItem(plan->m_name.c_str(), NODE_IMAGES[VIEWER_PLAN_APPROVED].m_normal, NODE_IMAGES[VIEWER_PLAN_APPROVED].m_selected, cItem, TVI_SORT);
                    treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLAN_APPROVED, plan->m_pkey));
                }
            }
        }
    }

    // Expand and select the top node.
    treeCtrl.Expand(rootItem, TVE_EXPAND);
    treeCtrl.SelectItem(rootItem);

    FUNCTION_EXIT;
}


void CPlansTreeView::PopulateTreeWithUnapprovedPlans()
{
    FUNCTION_ENTRY( "PopulateTreeWithUnapprovedPlans" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    CPvData &pvData = CPvData::instance();

    // Purge the existing contents of the tree.
    treeCtrl.DeleteAllItems();

    // Create a root node. Leave the right hand pane blank for the time being.
    HTREEITEM rootItem = treeCtrl.InsertItem("Unapproved Plans", NODE_IMAGES[VIEWER_PLANS_UNAPPROVED].m_normal, NODE_IMAGES[VIEWER_PLANS_UNAPPROVED].m_selected);
    treeCtrl.SetItemData(rootItem, CreateItemData(VIEWER_PLANS_UNAPPROVED, 0));
    PostMessage(WM_RESET_MENUS_AND_TBARS, VIEWER_PLANS_UNAPPROVED);

    PV::PlanList plans;
    pvData.getUnapprovedPlans(plans);

    if (plans.size() == 0)
    {
        HTREEITEM pItem = treeCtrl.InsertItem("There are no unapproved plans", NODE_IMAGES[VIEWER_BLANK].m_normal, NODE_IMAGES[VIEWER_BLANK].m_selected, rootItem, TVI_FIRST);
        treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLANS_UNAPPROVED, 0));
    }
    else
    {
        PV::CategoryList categories;
        pvData.getAllCategories(categories);

        // Insert all categories containing unapproved plans
        for (PV::CategoryList::iterator citer = categories.begin(); citer != categories.end(); citer++)
        {
            PV::Category *category = *citer;
            HTREEITEM cItem = NULL;

            plans.clear();
            pvData.getPlansForCategory(plans, category->m_pkey);

            // Insert all unapproved plans under the given category
            for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
            {
                PV::Plan *plan = *piter;

                if (plan->m_isApproved == false)
                {
                    // Insert the category if necessary
                    if (cItem == NULL)
                    {
                        cItem = treeCtrl.InsertItem(category->m_name.c_str(), NODE_IMAGES[VIEWER_CATEGORY].m_normal, NODE_IMAGES[VIEWER_CATEGORY].m_selected, rootItem, TVI_LAST);
                        treeCtrl.SetItemData(cItem, CreateItemData(VIEWER_CATEGORY, category->m_pkey));
                    }
                    HTREEITEM pItem = treeCtrl.InsertItem(plan->m_name.c_str(), NODE_IMAGES[VIEWER_PLAN_UNAPPROVED].m_normal, NODE_IMAGES[VIEWER_PLAN_UNAPPROVED].m_selected, cItem, TVI_SORT);
                    treeCtrl.SetItemData(pItem, CreateItemData(VIEWER_PLAN_UNAPPROVED, plan->m_pkey));
                }
            }
        }
    }

    // Expand and select the top node.
    treeCtrl.Expand(rootItem, TVE_EXPAND);
    treeCtrl.SelectItem(rootItem);

    FUNCTION_EXIT;
}


bool CPlansTreeView::UpdateView()
{
    FUNCTION_ENTRY( "UpdateView" );

    HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();
    DWORD itemData = GetTreeCtrl().GetItemData(selItem);
    int viewerType;
    unsigned long pkey;

    SplitItemData(itemData, viewerType, pkey);

    // Set up the correct form view in the right hand pane.
    CSplitterWnd* wndSplitter = (CSplitterWnd*) GetParent();
    if (!wndSplitter->GetPane(0,1)->IsKindOf(NODE_VIEWER_FORMS[viewerType]))
    {
        wndSplitter->GetPane(0,1)->DestroyWindow();
        wndSplitter->CreateView(0, 1, NODE_VIEWER_FORMS[viewerType], CSize(0, 0), NULL);
        wndSplitter->RecalcLayout();
        wndSplitter->GetPane(0,1)->ShowWindow(SW_SHOW);
    }

    // Populate the right hand view with data...
    if (viewerType == VIEWER_PLANS_ACTIVE)
    {
        ((CActivePlansForm *) wndSplitter->GetPane(0,1))->PopulateData();
    }
/*
    else if (viewerType == VIEWER_CATEGORY)
    {
        PV::Category *category = CPvData::instance().getCategory(pkey);
        ASSERT(category != NULL);

        ((CPlanCatForm *) wndSplitter->GetPane(0,1))->PopulateData(category);
    }
*/
    else if (viewerType == VIEWER_PLAN_APPROVED || viewerType == VIEWER_PLAN_UNAPPROVED)
    {
        PV::Plan *plan = CPvData::instance().getPlan(pkey);
        ASSERT(plan != NULL);

        ((CPlanForm *) wndSplitter->GetPane(0,1))->PopulateData(plan);
        ((CPlanForm *) wndSplitter->GetPane(0,1))->Resize();
    }

    PostMessage(WM_RESET_MENUS_AND_TBARS, viewerType);
    SetFocus();

    FUNCTION_EXIT;
    return true;
}


// Sets the position of the window splitter by adjusting the width of the first column.
// The position is adjusted only if the current position is zero and the new position is
// non zero, or vice versa.
void CPlansTreeView::SetSplitterPos(int pos)
{
    FUNCTION_ENTRY( "SetSplitterPos" );

    CSplitterWnd* wndSplitter = (CSplitterWnd*) GetParent();
    int firstColWidth, firstColMinWidth;

    wndSplitter->GetColumnInfo(0, firstColWidth, firstColMinWidth);

    if ((firstColWidth == 0 && pos > 0) || (firstColWidth > 0 && pos == 0))
    {
        wndSplitter->SetColumnInfo(0, pos, 0);
        wndSplitter->RecalcLayout();
    }

    FUNCTION_EXIT;
}


void CPlansTreeView::ShowView(UINT viewTypeID)
{
    FUNCTION_ENTRY( "ShowView" );

    if (m_currentView == viewTypeID)
    {
        FUNCTION_EXIT;
        return;
    }


    switch (viewTypeID)
    {
        case ID_VIEW_ALLPLANS:
            // Restore left hand pane.
            SetSplitterPos(250);
            PopulateTreeWithAllPlans();
            break;

        case ID_VIEW_ACTIVEPLANS:
            // Minimise the left hand pane containing a tree of all the active plans.
            // The user can resize the splitter if they wish to see the left pane.
            SetSplitterPos(0);
            PopulateTreeWithActivePlans();
            break;

        case ID_VIEW_APPROVEDPLANS:
            // Restore left hand pane.
            SetSplitterPos(250);
            PopulateTreeWithApprovedPlans();
            break;

        case ID_VIEW_UNAPPROVEDPLANS:
            // Restore left hand pane.
            SetSplitterPos(250);
            PopulateTreeWithUnapprovedPlans();
            break;

        default:
            CString msg;
            msg.Format("Unsupported view type ID %u", viewTypeID);
            MessageBox(msg, "Error");
    }

    CMainFrame* mainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    VERIFY(mainFrame);
    CMenu *menu = mainFrame->GetMenu();
    VERIFY(menu);

    menu->CheckMenuItem(ID_VIEW_ALLPLANS, viewTypeID == ID_VIEW_ALLPLANS? MF_CHECKED : MF_UNCHECKED);
    menu->CheckMenuItem(ID_VIEW_ACTIVEPLANS, viewTypeID == ID_VIEW_ACTIVEPLANS? MF_CHECKED : MF_UNCHECKED);
    menu->CheckMenuItem(ID_VIEW_APPROVEDPLANS, viewTypeID == ID_VIEW_APPROVEDPLANS? MF_CHECKED : MF_UNCHECKED);
    menu->CheckMenuItem(ID_VIEW_UNAPPROVEDPLANS, viewTypeID == ID_VIEW_UNAPPROVEDPLANS? MF_CHECKED : MF_UNCHECKED);

    menu->CheckMenuItem(ID_VIEW_FLATMODE, viewTypeID == ID_VIEW_ACTIVEPLANS? MF_CHECKED : MF_UNCHECKED);

    m_currentView = viewTypeID;

    FUNCTION_EXIT;
}



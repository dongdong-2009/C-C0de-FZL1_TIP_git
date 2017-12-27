//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/CategoryListCtrl.h"
#include "app/response_plans/plan_manager/src/TreeNode.h"
#include "app/response_plans/plan_manager/src/CategoryNode.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    typedef enum
            {
                NLC_NODE_NAME = 0,
                NLC_NODE_TYPE,
                NLC_NODE_STATUS
            } 
            ENodeListColumn;
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CCategoryListCtrl

CCategoryListCtrl::CCategoryListCtrl()
{
}


CCategoryListCtrl::~CCategoryListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCategoryListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCategoryListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCategoryListCtrl message handlers

BOOL CCategoryListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS;

	return CListCtrl::PreCreateWindow(cs);
}


int CCategoryListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    InsertColumn(NLC_NODE_NAME, _T(LabelMaker::getNodeListNameColumnLabel()), LVCFMT_LEFT, /*300*/450);//TD14921 liqiang
    InsertColumn(NLC_NODE_TYPE, _T(LabelMaker::getNodeListTypeColumnLabel()), LVCFMT_LEFT, /*100*/250);//TD14921 liqiang
    InsertColumn(NLC_NODE_STATUS, _T(LabelMaker::getNodeListStatusColumnLabel()), LVCFMT_LEFT, /*100*/260);//TD14921 liqiang

    // Initialise the images used for tree items.
    CImageList *nodeImages = new CImageList();
	nodeImages->Create(IDB_TREE_NODE_IMAGE_LIST_BMP, 16, 1, RGB(0, 0, 0xFF));

    SetImageList(nodeImages, LVSIL_SMALL);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

	return 0;
}


void CCategoryListCtrl::OnDestroy() 
{
    // TD#2479: free the memory allocated for the image list
    CImageList *nodeImages = GetImageList(LVSIL_SMALL);
    SetImageList(NULL, LVSIL_SMALL);

    delete nodeImages;

	CListCtrl::OnDestroy();	
}


void CCategoryListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	// TODO: Add your control notification handler code here
    if ((pDispInfo->item.mask & LVIF_TEXT) == LVIF_TEXT)
    { 
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        switch (pDispInfo->item.iSubItem)
        {
            case NLC_NODE_NAME:
            {
                pDispInfo->item.pszText = const_cast<char *>(treeNode->getNodeName().c_str());
                break;
            }
 
            case NLC_NODE_TYPE:
            { 
                pDispInfo->item.pszText = const_cast<char *>(treeNode->getNodeType().c_str());
                break;
            }

            case NLC_NODE_STATUS:
            {
                pDispInfo->item.pszText = const_cast<char *>(treeNode->getNodeStatus().c_str());
                break;
            }
        }
    }

	if ((pDispInfo->item.mask & LVIF_IMAGE) == LVIF_IMAGE)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        pDispInfo->item.iImage = treeNode->getNodeImageType(false);
    }
	
	*pResult = 0;
}


void CCategoryListCtrl::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMListView->lParam);
        CategoryNode *categoryNode = dynamic_cast<CategoryNode *>(treeNode);

        if (categoryNode)
        {
            *pResult = 1;  // Don't allow category items to be selected
            return;
        }
    }

	*pResult = 0;
}


void CCategoryListCtrl::RemoveNode(TreeNode *treeNode)
{
    int item = FindNode(treeNode);
    if (item != -1)
    {
        DeleteItem(item);
    }
}


int CCategoryListCtrl::FindNode(TreeNode *treeNode)
{
    LVFINDINFO lvfi;

    lvfi.flags = LVFI_PARAM;
    lvfi.lParam = (LPARAM) treeNode;

    return FindItem(&lvfi);
}


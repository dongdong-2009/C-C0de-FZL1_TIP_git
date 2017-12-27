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

#include "stdafx.h"

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepParameterListCtrl.h"
#include "LabelMaker.h"
#include "PlanStepParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterListCtrl

CPlanStepParameterListCtrl::CPlanStepParameterListCtrl()
{
    FUNCTION_ENTRY( "CPlanStepParameterListCtrl" );
    FUNCTION_EXIT;
}


CPlanStepParameterListCtrl::~CPlanStepParameterListCtrl()
{
    FUNCTION_ENTRY( "~CPlanStepParameterListCtrl" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterListCtrl, CListCtrl)
    // {{AFX_MSG_MAP(CPlanStepParameterListCtrl)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterListCtrl message handlers

void CPlanStepParameterListCtrl::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemChanging" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here

    // The parameter list is used for display purposes only, entry selection is redundant.
    *pResult = (pNMListView->uNewState & LVIS_SELECTED);

    FUNCTION_EXIT;
}


void CPlanStepParameterListCtrl::PreSubclassWindow()
{
    FUNCTION_ENTRY( "PreSubclassWindow" );

    // TODO: Add your specialized code here and/or call the base class
    SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

    int subItem = 0;

    InsertColumn(subItem++, _T(LabelMaker::getStepParameterListNameColumnLabel()), LVCFMT_LEFT, 200);
    InsertColumn(subItem++, _T(LabelMaker::getStepParameterListValueColumnLabel()), LVCFMT_LEFT, 400);

    CListCtrl::PreSubclassWindow();

    FUNCTION_EXIT;
}


BOOL CPlanStepParameterListCtrl::SetReadOnly(BOOL readOnly)
{
    FUNCTION_ENTRY( "SetReadOnly" );

    DWORD colorRef = ::GetSysColor(readOnly? COLOR_3DFACE : COLOR_WINDOW);

    if (SetBkColor(colorRef) && SetTextBkColor(colorRef))
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


void CPlanStepParameterListCtrl::AddParameter(CString parameterName, CString parameterValue,PlanStepParameter* planStepParameter)
{
    FUNCTION_ENTRY( "AddParameter" );

    int item = GetItemCount();

    InsertItem(item, _T(parameterName));
    SetItemText(item, 1, _T(parameterValue));

    SetItemData(item, (DWORD) planStepParameter);

    FUNCTION_EXIT;
}



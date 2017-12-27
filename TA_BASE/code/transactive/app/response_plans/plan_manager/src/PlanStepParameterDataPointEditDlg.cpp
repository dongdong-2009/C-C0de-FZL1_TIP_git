//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterDataPointEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates selecting a single data point from all configured data points.

#include "stdafx.h"

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "planmanager.h"
#include "PlanStepParameterDataPointEditDlg.h"
#include "DataPointStepParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterDataPointEditDlg dialog

CPlanStepParameterDataPointEditDlg::CPlanStepParameterDataPointEditDlg(DataPointStepParameter& parameter,bool canEdit)
    : CPlanStepParameterEditDlg(parameter,CPlanStepParameterDataPointEditDlg::IDD, canEdit)
{
    FUNCTION_ENTRY( "CPlanStepParameterDataPointEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterDataPointEditDlg)
    m_dataPointName = parameter.getDataPoint().c_str();
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterDataPointEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterDataPointEditDlg)
    DDX_Control(pDX, IDC_EQUIPMENT_TREE, m_ScadaTree);
    DDX_Text(pDX, IDC_DATA_POINT_NAME, m_dataPointName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterDataPointEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterDataPointEditDlg)
    ON_BN_CLICKED(IDOK, OnOK)
    ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIPMENT_TREE, OnScadaTreeItemSelectionChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterDataPointEditDlg message handlers

void CPlanStepParameterDataPointEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    DataPointStepParameter* parameter = dynamic_cast<DataPointStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    UpdateData();
    parameter->setDataPoint((LPCSTR)m_dataPointName);

    EndDialog(IDOK);

    FUNCTION_EXIT;
}


void CPlanStepParameterDataPointEditDlg::OnScadaTreeItemSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnScadaTreeItemSelectionChanged" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get currently selected item
    bool isDataPoint = false;
    TA_Base_Bus::CEntityTreeItem* selectedItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(m_ScadaTree.getSelectedItem());
    if (selectedItem)
    {
        isDataPoint = (selectedItem->getEntityData()->getType()==TA_Base_Core::DataPointEntityData::getStaticType());

        m_dataPointName = (isDataPoint? _T(selectedItem->getEntityData()->getName().c_str()) : _T(""));

        UpdateData(FALSE);
    }

    CWnd* okButton = GetDlgItem(IDOK);
    if (okButton)
    {
        okButton->EnableWindow(isDataPoint);
    }

    // TODO: Add your control notification handler code here

    *pResult = 0;

    FUNCTION_EXIT;
}


BOOL CPlanStepParameterDataPointEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    // TODO: Add extra initialization here
    CWaitCursor waitCursor;

    m_ScadaTree.ModifyStyle(TVS_LINESATROOT, 0);
    m_ScadaTree.Expand(m_ScadaTree.GetRootItem(), TVE_EXPAND);

    DataPointStepParameter* parameter = dynamic_cast<DataPointStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");
    m_ScadaTree.setInitialDataPoint(parameter->getDataPoint());

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

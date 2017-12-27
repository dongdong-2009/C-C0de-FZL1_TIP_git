//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterDataPointEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates selecting a single data point from all configured data points.

#if !defined(AFX_PLANSTEPPARAMETERDATAPOINTEDITDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_)
#define AFX_PLANSTEPPARAMETERDATAPOINTEDITDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"
#include "resource.h"
#include "PlanStepParameterEditDlg.h"


namespace TA_Base_App
{
    class DataPointStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterDataPointEditDlg dialog

    class CPlanStepParameterDataPointEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterDataPointEditDlg(DataPointStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterDataPointEditDlg)
    enum { IDD = IDD_PLAN_STEP_PARAMETER_DATA_POINT_EDIT_DLG };
    TA_Base_Bus::CScadaTreeCtrl    m_ScadaTree;
    CString    m_dataPointName;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterDataPointEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterDataPointEditDlg)
        afx_msg void OnOK();
        afx_msg void OnScadaTreeItemSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
        virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // namespace

#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#endif // !defined(AFX_PLANSTEPPARAMETERDATAPOINTEDITDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_)

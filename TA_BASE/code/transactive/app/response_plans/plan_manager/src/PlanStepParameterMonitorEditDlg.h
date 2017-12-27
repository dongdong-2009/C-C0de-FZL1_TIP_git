//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterMonitorEditDlg.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Dialog for selecting a monitor name for the Assign Camera To Monitor step.

#if !defined(AFX_PLANSTEPPARAMETERMONITOREDITDLG_H__CAB6BDC9_BBA9_40A2_8FD2_01477378B548__INCLUDED_)
#define AFX_PLANSTEPPARAMETERMONITOREDITDLG_H__CAB6BDC9_BBA9_40A2_8FD2_01477378B548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "resource.h"
#include "PlanStepParameterEditDlg.h"

namespace TA_Base_App
{
    class MonitorStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterMonitorEditDlg dialog

    class CPlanStepParameterMonitorEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterMonitorEditDlg(MonitorStepParameter& parameter, bool canEdit);

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterMonitorEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_MONITOR_EDIT_DLG };
        CButton    m_okButton;
        CTreeCtrl    m_monitorTree;
        CString    m_monitorName;
        CString    m_parameterName;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterMonitorEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterMonitorEditDlg)
        afx_msg void onSelectionChangedMonitorTree(NMHDR* pNMHDR, LRESULT* pResult);
        virtual BOOL OnInitDialog();
        afx_msg void onOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        /**
          * loadMonitorTree
          *
          * Loads the tree containing the monitors and their groups.  Then
          * selects the monitor identified by m_monitorParameter.getMonitorKey().
          */

        void loadMonitorTree();

        // The parameter that is updated using this dialog.
        // This is stored in the CPlanStepParameterEditDlg class, but we need it
        // as a MonitorStepParameter not just a PlanStepParameter.

        MonitorStepParameter& m_monitorParameter;

        // Keeps track of the currently selected monitor's key.

        unsigned long m_monitorKey;
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#endif // !defined(AFX_PLANSTEPPARAMETERMONITOREDITDLG_H__CAB6BDC9_BBA9_40A2_8FD2_01477378B548__INCLUDED_)

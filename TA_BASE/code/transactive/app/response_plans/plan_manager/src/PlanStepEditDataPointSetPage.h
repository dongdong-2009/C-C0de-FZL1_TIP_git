//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointSetPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Data Point Set step.

#if !defined(AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_SET)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "DataPointStepParameter.h"
#include "DataPointTestTypeStepParameter.h"
#include "DataPointValueStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "DataPointConfigAccess.h"


namespace TA_Base_App
{
    class DataPointSetStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDataPointSetPage dialog
    class CPlanStepEditDataPointSetPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDataPointSetPage)

    // Construction
    public:
        CPlanStepEditDataPointSetPage(DataPointSetStep* step);
        ~CPlanStepEditDataPointSetPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

        void UpdateDataPointType();
        void UpdateDataPointValue();

        void ResetDataPointValueDisplay();


    // Attributes
    private:
        DataPointStepParameter m_dataPointParameter;
        DataPointConfigAccess::EDataPointType m_dataPointType;
        TA_Base_Core::DataPointValue m_dataPointValue;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointSetPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_DATA_POINT_SET_DLG };
        CEdit    m_dataPointValueTextEdit;
        CNumericEdit    m_dataPointValueNumberEdit;
        CComboBox    m_dataPointValueSetCombo;
        CString    m_dataPointLabel;
        CString    m_dataPointTypeLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDataPointSetPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDataPointSetPage)
        afx_msg void OnPickDataPointButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // defined (STEPTYPE_DATA_POINT_SET)

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_)

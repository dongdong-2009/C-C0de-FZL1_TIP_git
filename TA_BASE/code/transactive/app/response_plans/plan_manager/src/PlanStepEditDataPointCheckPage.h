//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#if !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "DataPointStepParameter.h"
#include "DataPointTestTypeStepParameter.h"
#include "DataPointValueStepParameter.h"
#include "StepNumberStepParameter.h"
#include "DataPointConfigAccess.h"


namespace TA_Base_App
{
    class DataPointCheckStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDataPointCheckPage dialog
    class CPlanStepEditDataPointCheckPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDataPointCheckPage)

    // Construction
    public:
        CPlanStepEditDataPointCheckPage(DataPointCheckStep* step);
        ~CPlanStepEditDataPointCheckPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

        void UpdateDataPointType();
        void UpdateDataPointTestType();
        void UpdateDataPointValue();

        void ResetDataPointTestTypeDisplay();
        void ResetDataPointValueDisplay();
        void ResetSuccessAndFailureStepDisplay();


    // Attributes
    private:
        DataPointStepParameter m_dataPointParameter;
        StepNumberStepParameter m_successParameter;
        StepNumberStepParameter m_failureParameter;
        DataPointConfigAccess::EDataPointType m_dataPointType;
        TA_Base_Core::EDataPointTest m_dataPointTestType;
        TA_Base_Core::DataPointValue m_dataPointValue;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointCheckPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_DATA_POINT_CHECK_DLG };
        CEdit    m_dataPointValueTextEdit;
        CNumericEdit    m_dataPointValueNumberEdit;
        CComboBox    m_dataPointValueSetCombo;
        CComboBox    m_testTypeCombo;
        CString    m_failStepLabel;
        CString    m_successStepLabel;
        CString    m_dataPointLabel;
        CString    m_dataPointTypeLabel;
    // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDataPointCheckPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDataPointCheckPage)
        afx_msg void OnPickDataPointButton();
        afx_msg void OnPickFailStepButton();
        afx_msg void OnPickSuccessStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // defined (STEPTYPE_DATA_POINT_CHECK)

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)

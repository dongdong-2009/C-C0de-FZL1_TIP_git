//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanManager.h"
#include "PlanStepEditDataPointCheckPage.h"
#include "DataPointCheckStep.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const TA_Base_Core::EDataPointTest NUMERIC_DP_TESTS[] =
{
    TA_Base_Core::EQUAL_TO_TEST,
    TA_Base_Core::NOT_EQUAL_TO_TEST,
    TA_Base_Core::LESS_THAN_TEST,
    TA_Base_Core::GREATER_THAN_TEST,
    TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST,
    TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST
};

static const TA_Base_Core::EDataPointTest TEXT_DP_TESTS[] =
{
    TA_Base_Core::EQUAL_TO_TEST,
    TA_Base_Core::NOT_EQUAL_TO_TEST,
    TA_Base_Core::LESS_THAN_TEST,
    TA_Base_Core::GREATER_THAN_TEST,
    TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST,
    TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST
};

static const TA_Base_Core::EDataPointTest BOOLEAN_DP_TESTS[] =
{
    TA_Base_Core::EQUAL_TO_TEST,
    TA_Base_Core::NOT_EQUAL_TO_TEST
};

static const TA_Base_Core::EDataPointTest UNDEFINED_DP_TESTS[] =
{
    TA_Base_Core::UNDEFINED_TEST
};

static const char *const WRITEONLY_DATA_POINT = "输出数据点不能检查";
static const char *const UNDEFINED_DATA_POINT_VALUE = "未定义的数据点值";
static const char *const UNDEFINED_DATA_POINT_JUMP_STEP = "未定义的步骤";


using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDataPointCheckPage, CPlanStepEditParameterPage)

CPlanStepEditDataPointCheckPage::CPlanStepEditDataPointCheckPage(DataPointCheckStep* step) :
   CPlanStepEditParameterPage(CPlanStepEditDataPointCheckPage::IDD, step),
   m_dataPointParameter(*step->getDataPointParameter()),
   m_successParameter(*step->getSuccessParameter()),
   m_failureParameter(*step->getFailParameter()),
   m_dataPointType(DataPointConfigAccess::DPT_UNDEFINED),
   m_dataPointTestType(step->getTestTypeParameter()->getDataPointTestType()),
   m_dataPointValue(step->getValueParameter()->getDataPointValue())
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointCheckPage" );

    // {{AFX_DATA_INIT(CPlanStepEditDataPointCheckPage)
    m_failStepLabel = _T("");
    m_successStepLabel = _T("");
    m_dataPointLabel = _T("");
    m_dataPointTypeLabel = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditDataPointCheckPage::~CPlanStepEditDataPointCheckPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDataPointCheckPage)
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_TEXT, m_dataPointValueTextEdit);
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_NUMBER, m_dataPointValueNumberEdit);
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_BOOL, m_dataPointValueSetCombo);
    DDX_Control(pDX, IDC_TEST_TYPE, m_testTypeCombo);
    DDX_Text(pDX, IDC_FAIL_STEP, m_failStepLabel);
    DDX_Text(pDX, IDC_SUCCESS_STEP, m_successStepLabel);
    DDX_Text(pDX, IDC_DATA_POINT, m_dataPointLabel);
    DDX_Text(pDX, IDC_DATA_POINT_TYPE, m_dataPointTypeLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDataPointCheckPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDataPointCheckPage)
    ON_BN_CLICKED(IDC_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    ON_BN_CLICKED(IDC_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage message handlers

void CPlanStepEditDataPointCheckPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

    UpdateData(); // TD#1537

    std::string prevDataPoint = m_dataPointParameter.getDataPoint();

    UpdateDataPointTestType();
    UpdateDataPointValue();

    m_dataPointParameter.editParameter();

    if (prevDataPoint != m_dataPointParameter.getDataPoint())
    {
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

    UpdateData(); // TD#1537

    m_failureParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    UpdateData(); // TD#1537

    m_successParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* dataPointButton = GetDlgItem(IDC_PICK_DATA_POINT_BUTTON);
    if (dataPointButton)
    {
        dataPointButton->EnableWindow(enable);
    }

    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(enable);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(enable);
    }

    m_testTypeCombo.EnableWindow(enable);
    m_dataPointValueSetCombo.EnableWindow(enable);
    m_dataPointValueTextEdit.SetReadOnly(!enable);
    m_dataPointValueNumberEdit.SetReadOnly(!enable);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    CWaitCursor waitCursor;
    UpdateData();

    // Set data point name display
    m_dataPointLabel= m_dataPointParameter.getDataPoint().c_str();

    // Determine data point type and set the display accordingly
    UpdateDataPointType();
    m_dataPointTypeLabel = Utilities::convertToString(m_dataPointType).c_str();

    // Set the contents of the test type combo. Ensure that the currently selected type is applicable to
    // the data point.
    ResetDataPointTestTypeDisplay();

    // Display the appropriate control for the type of data point. If the current value does not match the
    // type of data point, the content of the control is cleared.
    ResetDataPointValueDisplay();

    // Set the success and failure step display to show the step number and description. If the data point
    // type is undefined disable the step picker buttons.
    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    DataPointCheckStep* step = dynamic_cast<DataPointCheckStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the data point check step page");

    UpdateDataPointTestType();
    UpdateDataPointValue();

    step->getDataPointParameter()->setDataPoint(m_dataPointParameter.getDataPoint());
    step->getTestTypeParameter()->setDataPointTestType(m_dataPointTestType);
    step->getValueParameter()->setDataPointValue(m_dataPointValue);
    step->getSuccessParameter()->setStepId(m_successParameter.getStepId());
    step->getFailParameter()->setStepId(m_failureParameter.getStepId());

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::UpdateDataPointTestType()
{
    FUNCTION_ENTRY( "UpdateDataPointTestType" );

    // TODO: Add your control notification handler code here
    int selectedTestTypeIdx = m_testTypeCombo.GetCurSel();

    if (selectedTestTypeIdx != CB_ERR)
    {
        m_dataPointTestType = (TA_Base_Core::EDataPointTest) m_testTypeCombo.GetItemData(selectedTestTypeIdx);
    }
    else
    {
        m_dataPointTestType = TA_Base_Core::UNDEFINED_TEST;
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::UpdateDataPointValue()
{
    FUNCTION_ENTRY( "UpdateDataPointValue" );

    CString dataPointValueStr;

    switch (m_dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
        case DataPointConfigAccess::DPT_SIGNED_INT16_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT16_SM:
        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
        case DataPointConfigAccess::DPT_SIGNED_INT32_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT32_SM:
        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
        {
            m_dataPointValueNumberEdit.GetWindowText(dataPointValueStr);

            if (dataPointValueStr.IsEmpty())
            {
                m_dataPointValue = TA_Base_Core::DataPointValue();
            }
            else
            {
                TA_Base_Core::NumericDataPoint value;
                Utilities::convertFromString((LPCTSTR) dataPointValueStr, value.value, value.precision);
                m_dataPointValue.numericValue(value);
            }
            break;
        }

        case DataPointConfigAccess::DPT_BOOLEAN:
        {
            if (m_dataPointValueSetCombo.GetCurSel() == CB_ERR)
            {
                m_dataPointValue = TA_Base_Core::DataPointValue();
            }
            else
            {
                bool value;
                m_dataPointValueSetCombo.GetWindowText(dataPointValueStr);
                Utilities::convertFromString((LPCTSTR) dataPointValueStr, value);
                m_dataPointValue.booleanValue(value);
            }
            break;
        }

        case DataPointConfigAccess::DPT_TEXT:
        {
            m_dataPointValueTextEdit.GetWindowText(dataPointValueStr);
            m_dataPointValue.textValue(CORBA::string_dup((LPCTSTR) dataPointValueStr));
            break;
        }

        case DataPointConfigAccess::DPT_DERIVED_IN:
        {
            if (m_dataPointValueSetCombo.GetCurSel() == CB_ERR)
            {
                m_dataPointValue = TA_Base_Core::DataPointValue();
            }
            else
            {
                TA_Base_Core::NumericDataPoint value;
                value.value = m_dataPointValueSetCombo.GetItemData(m_dataPointValueSetCombo.GetCurSel());
                value.precision = 0;
                m_dataPointValue.numericValue(value);
            }
            break;
        }

        default:
            m_dataPointValue.textValue(CORBA::string_dup(UNDEFINED_DATA_POINT_VALUE));
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::UpdateDataPointType()
{
    FUNCTION_ENTRY( "UpdateDataPointType" );

    m_dataPointType = DataPointConfigAccess::DPT_UNDEFINED;
    std::string dataPointName = m_dataPointParameter.getDataPoint();

    if (!dataPointName.empty())
    {
        m_dataPointType = DataPointConfigAccess::getDataPointType(dataPointName);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::ResetDataPointTestTypeDisplay()
{
    FUNCTION_ENTRY( "ResetDataPointTestTypeDisplay" );

    const TA_Base_Core::EDataPointTest *dpTestTypeList = NULL;
    int dpTestTypeCount = 0;

    // Determine the set of data point value tests that are applicable to the type of data point
    switch (m_dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
        case DataPointConfigAccess::DPT_SIGNED_INT16_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT16_SM:
        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
        case DataPointConfigAccess::DPT_SIGNED_INT32_2C:
        case DataPointConfigAccess::DPT_SIGNED_INT32_SM:
        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
        case DataPointConfigAccess::DPT_DERIVED_IN:
            dpTestTypeList = NUMERIC_DP_TESTS;
            dpTestTypeCount = sizeof(NUMERIC_DP_TESTS) / sizeof(TA_Base_Core::EDataPointTest);
            break;

        case DataPointConfigAccess::DPT_BOOLEAN:
            dpTestTypeList = BOOLEAN_DP_TESTS;
            dpTestTypeCount = sizeof(BOOLEAN_DP_TESTS) / sizeof(TA_Base_Core::EDataPointTest);
            break;

        case  DataPointConfigAccess::DPT_TEXT:
            dpTestTypeList = TEXT_DP_TESTS;
            dpTestTypeCount = sizeof(TEXT_DP_TESTS) / sizeof(TA_Base_Core::EDataPointTest);
            break;

        default:
            dpTestTypeList = UNDEFINED_DP_TESTS;
            dpTestTypeCount = sizeof(UNDEFINED_DP_TESTS) / sizeof(TA_Base_Core::EDataPointTest);
            break;
    }

    // Populate the test type combo box with the relevant tests
    m_testTypeCombo.ResetContent();
    for (int i = 0; i < dpTestTypeCount; i++)
    {
        int testTypeIdx = m_testTypeCombo.AddString(Utilities::convertToString(dpTestTypeList[i]).c_str());
        m_testTypeCombo.SetItemData(testTypeIdx, dpTestTypeList[i]);
    }

    // See if the new set of test types supports the currently configured test type. If it does then select it, otherwise
    // if current data point type is invalid then set the combo to "undefined test".
    int requiredTextTypeIdx = m_testTypeCombo.FindStringExact(-1, Utilities::convertToString(m_dataPointTestType).c_str());
    if (requiredTextTypeIdx != CB_ERR)
    {
        m_testTypeCombo.SetCurSel(requiredTextTypeIdx);
    }
    else if (m_dataPointType == DataPointConfigAccess::DPT_UNDEFINED)
    {
        m_testTypeCombo.SetCurSel(0);
    }

    // Enable the combo box if editing is allowed and data point type is valid
    m_testTypeCombo.EnableWindow(IsEditingEnabled() && m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED);

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::ResetDataPointValueDisplay()
{
    FUNCTION_ENTRY( "ResetDataPointValueDisplay" );

    m_dataPointValueSetCombo.ShowWindow(SW_HIDE);    m_dataPointValueSetCombo.SetWindowText("");
    m_dataPointValueNumberEdit.ShowWindow(SW_HIDE);  m_dataPointValueNumberEdit.SetWindowText("");
    m_dataPointValueTextEdit.ShowWindow(SW_HIDE);    m_dataPointValueTextEdit.SetWindowText("");

    switch (m_dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
            m_dataPointValueNumberEdit.Reset(true, false);
            m_dataPointValueNumberEdit.SetRange(0.0, 65535.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 65535.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-32768.0, 32767.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32768.0 &&
                m_dataPointValue.numericValue().value <= 32767.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT16_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-32767.0, 32767.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -32767.0 &&
                m_dataPointValue.numericValue().value <= 32767.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
            m_dataPointValueNumberEdit.Reset(true, false);
            m_dataPointValueNumberEdit.SetRange(0.0, 4294967295.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= 0.0 &&
                m_dataPointValue.numericValue().value <= 4294967295.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;


        case DataPointConfigAccess::DPT_SIGNED_INT32_2C: // 2's complement
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-2147483648.0, 2147483647.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483648.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_SIGNED_INT32_SM: // sign-magnitude
            m_dataPointValueNumberEdit.Reset(true, true);
            m_dataPointValueNumberEdit.SetRange(-2147483647.0, 2147483647.0);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().precision == 0 &&
                m_dataPointValue.numericValue().value >= -2147483647.0 &&
                m_dataPointValue.numericValue().value <= 2147483647.0)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
            m_dataPointValueNumberEdit.Reset(false, true);
            m_dataPointValueNumberEdit.SetRange(-3.4E+38, 3.4E+38);
            if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                m_dataPointValue.numericValue().value >= -3.4E+38 &&
                m_dataPointValue.numericValue().value <= 3.4E+38)
            {
                m_dataPointValueNumberEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(m_dataPointValue.numericValue().value,
                                                                                                  m_dataPointValue.numericValue().precision));
            }
            m_dataPointValueNumberEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_BOOLEAN:
            m_dataPointValueSetCombo.ResetContent();
            m_dataPointValueSetCombo.AddString(LabelMaker::getTrueFalseLabel(true));
            m_dataPointValueSetCombo.AddString(LabelMaker::getTrueFalseLabel(false));
            if (m_dataPointValue._d() == TA_Base_Core::BOOLEAN_DATA_POINT)
            {
                m_dataPointValueSetCombo.SelectString(-1, LabelMaker::getTrueFalseLabel(m_dataPointValue.booleanValue()));
            }
            m_dataPointValueSetCombo.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_TEXT:
            m_dataPointValueTextEdit.LimitText(TA_Base_Bus::DPT_MAX_TEXT_LENGTH);
            m_dataPointValueTextEdit.SetReadOnly(FALSE);
            if (m_dataPointValue._d() == TA_Base_Core::TEXT_DATA_POINT &&
                strcmp(m_dataPointValue.textValue(), UNDEFINED_DATA_POINT_VALUE))
            {
                m_dataPointValueTextEdit.SetWindowText(m_dataPointValue.textValue());
            }
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
            break;

        case DataPointConfigAccess::DPT_DERIVED_IN:
        {
            std::string dataPointName = m_dataPointParameter.getDataPoint();
            DataPointConfigAccess::StateValueList derivedStates = DataPointConfigAccess::getDataPointStates(dataPointName);

            m_dataPointValueSetCombo.ResetContent();
            for (DataPointConfigAccess::StateValueList::iterator iter = derivedStates.begin(); iter != derivedStates.end(); iter++)
            {
                DataPointConfigAccess::StateValue derivedState(*iter);
                std::ostringstream stateLabel;
                stateLabel << derivedState.state << ": " << derivedState.description;

                int stateIdx = m_dataPointValueSetCombo.AddString(stateLabel.str().c_str());
                m_dataPointValueSetCombo.SetItemData(stateIdx, derivedState.state);

                if (m_dataPointValue._d() == TA_Base_Core::NUMERIC_DATA_POINT &&
                    m_dataPointValue.numericValue().value == derivedState.state)
                {
                    m_dataPointValueSetCombo.SetCurSel(stateIdx);
                }
            }
            m_dataPointValueSetCombo.ShowWindow(SW_SHOW);
            break;
        }

        case DataPointConfigAccess::DPT_COMBINED_OUT:
            m_dataPointValueTextEdit.SetWindowText(WRITEONLY_DATA_POINT);
            m_dataPointValueTextEdit.SetReadOnly(TRUE);
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
            break;

        default:
            m_dataPointValueTextEdit.SetWindowText(UNDEFINED_DATA_POINT_VALUE);
            m_dataPointValueTextEdit.SetReadOnly(TRUE);
            m_dataPointValueTextEdit.ShowWindow(SW_SHOW);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointCheckPage::ResetSuccessAndFailureStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    bool stepPickerEnabled;

    if (m_dataPointType != DataPointConfigAccess::DPT_UNDEFINED)
    {
        // Set the success and failure step display with step number and description
        m_successStepLabel = m_successParameter.getDestinationStepDescription().c_str();
        m_failStepLabel = m_failureParameter.getDestinationStepDescription().c_str();

        // Enable step picker button
        stepPickerEnabled = IsEditingEnabled();
    }
    else
    {
        if (IsEditingEnabled())
        {
            // Reset success and failure step parameters.
            m_successParameter.setStepId(0);
            m_failureParameter.setStepId(0);
        }

        // Set display to "undefined step"
        m_successStepLabel = UNDEFINED_DATA_POINT_JUMP_STEP;
        m_failStepLabel = UNDEFINED_DATA_POINT_JUMP_STEP;

        // Disable step picker button
        stepPickerEnabled = false;
    }

    // Enable step picker button as appropriate
    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(stepPickerEnabled);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(stepPickerEnabled);
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK)

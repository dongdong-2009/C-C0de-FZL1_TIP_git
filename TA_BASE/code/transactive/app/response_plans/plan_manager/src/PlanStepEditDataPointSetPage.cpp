//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointSetPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Data Point Set step.

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_SET)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanManager.h"
#include "PlanStepEditDataPointSetPage.h"
#include "DataPointSetStep.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *const READONLY_DATA_POINT = "输入数据点不能设置";
static const char *const UNDEFINED_DATA_POINT_VALUE = "未定义的数据点值";


using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointSetPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDataPointSetPage, CPlanStepEditParameterPage)

CPlanStepEditDataPointSetPage::CPlanStepEditDataPointSetPage(DataPointSetStep* step) :
   CPlanStepEditParameterPage(CPlanStepEditDataPointSetPage::IDD, step),
   m_dataPointParameter(*step->getDataPointParameter()),
   m_dataPointType(DataPointConfigAccess::DPT_UNDEFINED),
   m_dataPointValue(step->getValueParameter()->getDataPointValue())
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointSetPage" );

    // {{AFX_DATA_INIT(CPlanStepEditDataPointCheckPage)
    m_dataPointLabel = _T("");
    m_dataPointTypeLabel = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditDataPointSetPage::~CPlanStepEditDataPointSetPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDataPointSetPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDataPointSetPage)
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_TEXT, m_dataPointValueTextEdit);
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_NUMBER, m_dataPointValueNumberEdit);
    DDX_Control(pDX, IDC_DATA_POINT_VALUE_BOOL, m_dataPointValueSetCombo);
    DDX_Text(pDX, IDC_DATA_POINT, m_dataPointLabel);
    DDX_Text(pDX, IDC_DATA_POINT_TYPE, m_dataPointTypeLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDataPointSetPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDataPointSetPage)
    ON_BN_CLICKED(IDC_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDataPointCheckPage message handlers

void CPlanStepEditDataPointSetPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

    UpdateData(); // TD#1537

    std::string prevDataPoint = m_dataPointParameter.getDataPoint();

    UpdateDataPointValue();

    m_dataPointParameter.editParameter();

    if (prevDataPoint != m_dataPointParameter.getDataPoint())
    {
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* dataPointButton = GetDlgItem(IDC_PICK_DATA_POINT_BUTTON);
    if (dataPointButton)
    {
        dataPointButton->EnableWindow(enable);
    }

    m_dataPointValueSetCombo.EnableWindow(enable);
    m_dataPointValueTextEdit.SetReadOnly(!enable);
    m_dataPointValueNumberEdit.SetReadOnly(!enable);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    CWaitCursor waitCursor;
    UpdateData();

    // Set data point name display
    m_dataPointLabel= m_dataPointParameter.getDataPoint().c_str();

    // Determine data point type and set the display accordingly
    UpdateDataPointType();
    m_dataPointTypeLabel = Utilities::convertToString(m_dataPointType).c_str();

    // Display the appropriate control for the type of data point. If the current value does not match the
    // type of data point, the content of the control is cleared.
    ResetDataPointValueDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    DataPointSetStep* step = dynamic_cast<DataPointSetStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the data point set step page");

    UpdateDataPointValue();

    step->getDataPointParameter()->setDataPoint(m_dataPointParameter.getDataPoint());
    step->getValueParameter()->setDataPointValue(m_dataPointValue);

    FUNCTION_EXIT;
}


void CPlanStepEditDataPointSetPage::UpdateDataPointValue()
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

        case DataPointConfigAccess::DPT_COMBINED_OUT:
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


void CPlanStepEditDataPointSetPage::UpdateDataPointType()
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


void CPlanStepEditDataPointSetPage::ResetDataPointValueDisplay()
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
            m_dataPointValueNumberEdit.SetRange(-32767.0, 32767.0L);
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

        case DataPointConfigAccess::DPT_COMBINED_OUT:
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

        case DataPointConfigAccess::DPT_DERIVED_IN:
            m_dataPointValueTextEdit.SetWindowText(READONLY_DATA_POINT);
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


#endif // defined (STEPTYPE_DATA_POINT_SET)

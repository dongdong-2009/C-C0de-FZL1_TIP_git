//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchGuiAppParameterPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_LAUNCH_GUI_APP)

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditLaunchGuiAppParameterPage.h"
#include "LaunchGuiAppStep.h"
#include "GuiAppTypeStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned long COMMAND_LINE_TXT_LIMIT = 250;//DB limit is 1000 but this should be more then enough

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchGuiAppParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLaunchGuiAppParameterPage, CPlanStepEditParameterPage)

CPlanStepEditLaunchGuiAppParameterPage::CPlanStepEditLaunchGuiAppParameterPage(LaunchGuiAppStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditLaunchGuiAppParameterPage::IDD, step),
    m_xPositionEdit(true, false),
    m_yPositionEdit(true, false),
    m_widthEdit(true, false),
    m_heightEdit(true, false)
{
    FUNCTION_ENTRY( "CPlanStepEditLaunchGuiAppParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditLaunchGuiAppParameterPage)
    // }}AFX_DATA_INIT

    long screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    long screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    m_xPositionEdit.SetRange(0, screenWidth);
    m_yPositionEdit.SetRange(0, screenHeight);
    m_widthEdit.SetRange(0, screenWidth);
    m_heightEdit.SetRange(0, screenHeight);

    FUNCTION_EXIT;
}


CPlanStepEditLaunchGuiAppParameterPage::~CPlanStepEditLaunchGuiAppParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditLaunchGuiAppParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_appTypeCombo.EnableWindow(enable);
    m_commandLineEdit.SetReadOnly(!enable);
    m_xPositionEdit.SetReadOnly(!enable);
    m_yPositionEdit.SetReadOnly(!enable);
    m_widthEdit.SetReadOnly(!enable);
    m_heightEdit.SetReadOnly(!enable);

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    LaunchGuiAppStep *step = dynamic_cast<LaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();

    m_commandLineEdit.SetWindowText(commandLineParameter->getMessageText().c_str());

    if (!appTypeParameter->isHiddenValue())
    {
        int appTypeIdx = m_appTypeCombo.FindStringExact(-1, appTypeParameter->getApplicationName().c_str());
        if (appTypeIdx != CB_ERR)
        {
            m_appTypeCombo.SetCurSel(appTypeIdx);
        }
    }

    if (!xPositionParameter->isHiddenValue())
    {
        m_xPositionEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(xPositionParameter->getNumericValue()));
    }

    if (!yPositionParameter->isHiddenValue())
    {
        m_yPositionEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(yPositionParameter->getNumericValue()));
    }

    if (!widthParameter->isHiddenValue())
    {
        m_widthEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(widthParameter->getNumericValue()));
    }

    if (!heightParameter->isHiddenValue())
    {
        m_heightEdit.SetWindowText(TA_Base_Bus::StringUtilities::convertToString(heightParameter->getNumericValue()));
    }

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    LaunchGuiAppStep* step = dynamic_cast<LaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();

    int appTypeIdx = m_appTypeCombo.GetCurSel();
    if (appTypeIdx != CB_ERR)
    {
        appTypeParameter->setValue(m_appTypeCombo.GetItemData(appTypeIdx));
        appTypeParameter->hideValue(false);
    }

    CString editText;
    m_commandLineEdit.GetWindowText(editText);
    commandLineParameter->setText((LPCTSTR) editText);

    m_xPositionEdit.GetWindowText(editText);
    unsigned long xPosition = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_X_POSITION : atol(editText));
    xPositionParameter->setValue(xPosition);
//    xPositionParameter->hideValue(editText.IsEmpty());

    m_yPositionEdit.GetWindowText(editText);
    unsigned long yPosition = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_Y_POSITION : atol(editText));
    yPositionParameter->setValue(yPosition);
//    yPositionParameter->hideValue(editText.IsEmpty());

    m_widthEdit.GetWindowText(editText);
    unsigned long width = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_WIDTH : atol(editText));
    widthParameter->setValue(width);
    widthParameter->hideValue(editText.IsEmpty() != 0);

    m_heightEdit.GetWindowText(editText);
    unsigned long height = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_HEIGHT : atol(editText));
    heightParameter->setValue(height);
    heightParameter->hideValue(editText.IsEmpty() != 0);

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLaunchGuiAppParameterPage)
    DDX_Control(pDX, IDC_Y_POS_EDIT, m_yPositionEdit);
    DDX_Control(pDX, IDC_X_POS_EDIT, m_xPositionEdit);
    DDX_Control(pDX, IDC_WIDTH_EDIT, m_widthEdit);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineEdit);
    DDX_Control(pDX, IDC_HEIGHT_EDIT, m_heightEdit);
    DDX_Control(pDX, IDC_APP_TYPE_COMBO, m_appTypeCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditLaunchGuiAppParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditLaunchGuiAppParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchGuiAppParameterPage message handlers

BOOL CPlanStepEditLaunchGuiAppParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

     // as per TD 12384
     m_commandLineEdit.SetLimitText (COMMAND_LINE_TXT_LIMIT);

    try
    {
        typedef  std::vector<TA_Base_Core::IGui *> GuiList;
        GuiList guiList = TA_Base_Core::GuiAccessFactory::getInstance().getAllGuis();

        for (GuiList::const_iterator iter = guiList.begin(); iter != guiList.end(); iter++)
        {
            std::auto_ptr<TA_Base_Core::IGui> gui(*iter);

            if (gui.get() != NULL && gui->getKey() > 0)
            {
                int appTypeIdx = m_appTypeCombo.AddString(gui->getFullName().c_str());
                m_appTypeCombo.SetItemData(appTypeIdx, gui->getKey());
            }
        }

        guiList.clear();
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_LOAD_APPLICATIONS, ex);

        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


#endif // defined (STEPTYPE_LAUNCH_GUI_APP)

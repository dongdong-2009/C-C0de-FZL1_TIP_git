/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchSchematicParameterPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "PlanManager.h"
#include "PlanStepEditLaunchSchematicParameterPage.h"
#include "LaunchSchematicStep.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT MIN_SCREEN_NUMBER = 1;
static const UINT MAX_SCREEN_NUMBER = 10;
static const unsigned long COMMAND_LINE_TXT_LIMIT = 250;//DB limit is 1000 but this should be more then enough

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchSchematicParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLaunchSchematicParameterPage, CPlanStepEditParameterPage)

CPlanStepEditLaunchSchematicParameterPage::CPlanStepEditLaunchSchematicParameterPage(LaunchSchematicStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditLaunchSchematicParameterPage::IDD, step),
    m_screenNumber(MIN_SCREEN_NUMBER),
    m_commandLine(_T(""))
{
    FUNCTION_ENTRY( "CPlanStepEditLaunchSchematicParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditLaunchSchematicParameterPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditLaunchSchematicParameterPage::~CPlanStepEditLaunchSchematicParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditLaunchSchematicParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_schematicNameCombo.EnableWindow(enable);

    CEdit* screenNumberEdit = static_cast<CEdit*>(GetDlgItem(IDC_SCREEN_NUMBER_EDIT));
    if (screenNumberEdit)
    {
        screenNumberEdit->SetReadOnly(!enable);
    }

    CEdit* commandLineEdit = static_cast<CEdit*>(GetDlgItem(IDC_COMMAND_LINE_EDIT));
    if (commandLineEdit)
    {
        commandLineEdit->SetReadOnly(!enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    LaunchSchematicStep *step = dynamic_cast<LaunchSchematicStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch schematic step page");

    TextualStepParameter *schematicNameParameter = step->getSchematicNameParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *screenNumberParameter = step->getScreenNumberParameter();

    int schematicIdx = m_schematicNameCombo.FindStringExact(-1, schematicNameParameter->getMessageText().c_str());
    if (schematicIdx != CB_ERR)
    {
        m_schematicNameCombo.SetCurSel(schematicIdx);
    }

    m_commandLine = commandLineParameter->getMessageText().c_str();
    m_screenNumber = screenNumberParameter->getNumericValue();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    UpdateData(TRUE);

    LaunchSchematicStep* step = dynamic_cast<LaunchSchematicStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch schematic step page");

    TextualStepParameter *schematicNameParameter = step->getSchematicNameParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *screenNumberParameter = step->getScreenNumberParameter();

    CString editText;
    m_schematicNameCombo.GetWindowText(editText);
    schematicNameParameter->setText(static_cast<LPCTSTR>(editText));
    commandLineParameter->setText(static_cast<LPCTSTR>(m_commandLine));
    screenNumberParameter->setValue(m_screenNumber);

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLaunchSchematicParameterPage)
    DDX_Text(pDX, IDC_COMMAND_LINE_EDIT, m_commandLine);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineTxt);
    DDX_Text(pDX, IDC_SCREEN_NUMBER_EDIT, m_screenNumber);
    DDX_Control(pDX, IDC_SCREEN_NUMBER_SPIN, m_screenNumberSpin);
    DDX_Control(pDX, IDC_SCHEMATIC_NAME_COMBO, m_schematicNameCombo);
    DDV_MinMaxUInt(pDX, m_screenNumber, MIN_SCREEN_NUMBER, MAX_SCREEN_NUMBER);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditLaunchSchematicParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditLaunchSchematicParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchSchematicParameterPage message handlers

BOOL CPlanStepEditLaunchSchematicParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    m_screenNumberSpin.SetBuddy(GetDlgItem(IDC_SCREEN_NUMBER_EDIT));
    m_screenNumberSpin.SetRange(MIN_SCREEN_NUMBER, MAX_SCREEN_NUMBER);

    m_commandLineTxt.SetLimitText (COMMAND_LINE_TXT_LIMIT);

    try
    {
        CWaitCursor waitCursor;

        typedef  std::vector<TA_Base_Core::IScadaDisplay*> SchematicList;
        SchematicList schematicList = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getAllScadaDisplays();

        for (SchematicList::const_iterator iter = schematicList.begin(); iter != schematicList.end(); iter++)
        {
            std::auto_ptr<TA_Base_Core::IScadaDisplay> schematic(*iter);

            if (schematic.get() != NULL)
            {
                m_schematicNameCombo.AddString(schematic->getPath().c_str());
            }
        }

        schematicList.clear();
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_LOAD_SCHEMATICS, ex);
    }

    FUNCTION_EXIT;
    return TRUE;
}


#endif // defined (STEPTYPE_LAUNCH_SCHEMATIC)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "PlanManager.h"
#include "PlanStepEditBackgroundLaunchSchematicParameterPage.h"
#include "BackgroundLaunchSchematicStep.h"
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
// CPlanStepEditBackgroundLaunchSchematicParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditBackgroundLaunchSchematicParameterPage, CPlanStepEditParameterPage)

CPlanStepEditBackgroundLaunchSchematicParameterPage::CPlanStepEditBackgroundLaunchSchematicParameterPage(BackgroundLaunchSchematicStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditBackgroundLaunchSchematicParameterPage::IDD, step),
    m_screenNumber(MIN_SCREEN_NUMBER),
    m_commandLine(_T("")),
    m_dataPointParameter(*step, step->getDataPointDisplayParameter()->getName(), ""),
    m_actionTypeKey(*step->getActionType()),
    m_operatorProfileParameter(*step->getOperatorProfile())
{
    FUNCTION_ENTRY( "CPlanStepEditBackgroundLaunchSchematicParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditBackgroundLaunchSchematicParameterPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditBackgroundLaunchSchematicParameterPage::~CPlanStepEditBackgroundLaunchSchematicParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditBackgroundLaunchSchematicParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchSchematicParameterPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_schematicNameCombo.EnableWindow(enable);

    CEdit* screenNumberEdit = static_cast<CEdit*>(GetDlgItem(IDC_BG_SCREEN_NUMBER_EDIT));
    if (screenNumberEdit)
    {
        screenNumberEdit->SetReadOnly(!enable);
    }

    CEdit* commandLineEdit = static_cast<CEdit*>(GetDlgItem(IDC_COMMAND_LINE_EDIT));
    if (commandLineEdit)
    {
        commandLineEdit->SetReadOnly(!enable);
    }

    CWnd* operatorProfileCombo = GetDlgItem(IDC_OPERATOR_PROFILE_COMBO);
    if (operatorProfileCombo)
    {
        operatorProfileCombo->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchSchematicParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    BackgroundLaunchSchematicStep *step = dynamic_cast<BackgroundLaunchSchematicStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch schematic step page");

    TextualStepParameter *schematicNameParameter = step->getSchematicNameParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *screenNumberParameter = step->getScreenNumberParameter();
    NumericStepParameter *actionTypeKey = step->getActionType();

    int schematicIdx = m_schematicNameCombo.FindStringExact(-1, schematicNameParameter->getMessageText().c_str());
    if (schematicIdx != CB_ERR)
    {
        m_schematicNameCombo.SetCurSel(schematicIdx);
    }

    m_commandLine = commandLineParameter->getMessageText().c_str();
    m_screenNumber = screenNumberParameter->getNumericValue();

    int numberOfActionType = m_actionTypeCombo.GetCount();
    m_actionTypeCombo.SetCurSel(0);
    for (int actionListIndex = 0; actionListIndex < numberOfActionType; ++actionListIndex)
    {
        if(actionTypeKey->getNumericValue() == m_actionTypeCombo.GetItemData(actionListIndex))
        {
            m_actionTypeCombo.SetCurSel(actionListIndex);
            break;
        }
    }
    
    std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
    try
    {
        dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(step->getDataPointParameter()->getNumericValue()));
    }
    catch (...) 
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get entity data, entity key = %d", step->getDataPointParameter()->getNumericValue());
        return;
    }

    // Set data point name display
    m_dataPointLabel= dpEntity->getName().c_str();

    try
    {
        std::auto_ptr<TA_Base_Core::IProfile> iProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(m_operatorProfileParameter.getNumericValue(), false));
        int appTypeIdx = m_operatorProfileCombo.FindStringExact(-1, iProfile->getName().c_str());
        if (appTypeIdx != CB_ERR)
        {
            m_operatorProfileCombo.SetCurSel(appTypeIdx);
        }
    }
    catch(...)
    {

    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchSchematicParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    UpdateData(TRUE);

    BackgroundLaunchSchematicStep* step = dynamic_cast<BackgroundLaunchSchematicStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch schematic step page");

    TextualStepParameter *schematicNameParameter = step->getSchematicNameParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *screenNumberParameter = step->getScreenNumberParameter();

    CString editText;
    m_schematicNameCombo.GetWindowText(editText);
    schematicNameParameter->setText(static_cast<LPCTSTR>(editText));
    commandLineParameter->setText(static_cast<LPCTSTR>(m_commandLine));
    screenNumberParameter->setValue(m_screenNumber);

    int profileIdx = m_operatorProfileCombo.GetCurSel();
    if (profileIdx != CB_ERR)
    {
        CString currentProfileName;
        m_operatorProfileCombo.GetLBText(profileIdx, currentProfileName);
        step->getOperatorProfile()->setValue(m_operatorProfileCombo.GetItemData(profileIdx));
        step->getOperatorProfileName()->setText((LPCTSTR)currentProfileName);
    }

    int actionTypeIdx = m_actionTypeCombo.GetCurSel();
    if (actionTypeIdx != CB_ERR)
    {
        step->getActionType()->setValue(m_actionTypeCombo.GetItemData(actionTypeIdx));

        CString strActionType;
        m_actionTypeCombo.GetLBText(actionTypeIdx, strActionType);
        step->getActionTypeDisplayParameter()->setText(strActionType.GetBuffer(0));
    }
    

    std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
    try
    {
        dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_dataPointParameter.getDataPoint()));
    }
    catch (...) 
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get entity data, entity name = %s", m_dataPointParameter.getDataPoint());
        return;
    }

    step->getDataPointParameter()->setValue(dpEntity->getKey());
    step->getDataPointDisplayParameter()->setText(m_dataPointParameter.getDataPoint());

    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchSchematicParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditBackgroundLaunchSchematicParameterPage)
    DDX_Text(pDX, IDC_COMMAND_LINE_EDIT, m_commandLine);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineTxt);
    DDX_Text(pDX, IDC_BG_SCREEN_NUMBER_EDIT, m_screenNumber);
    DDX_Control(pDX, IDC_BG_SCREEN_NUMBER_SPIN, m_screenNumberSpin);
    DDX_Control(pDX, IDC_SCHEMATIC_NAME_COMBO, m_schematicNameCombo);
    DDV_MinMaxUInt(pDX, m_screenNumber, MIN_SCREEN_NUMBER, MAX_SCREEN_NUMBER);
    DDX_Control(pDX, IDC_ACTION_COMBO, m_actionTypeCombo);
    DDX_Text(pDX, IDC_DATA_POINT, m_dataPointLabel);
    DDX_Control(pDX, IDC_OPERATOR_PROFILE, m_operatorProfileCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditBackgroundLaunchSchematicParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditBackgroundLaunchSchematicParameterPage)
    ON_BN_CLICKED(IDC_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditBackgroundLaunchSchematicParameterPage message handlers

BOOL CPlanStepEditBackgroundLaunchSchematicParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    CWnd *pBgScreenNumWnd = GetDlgItem(IDC_BG_SCREEN_NUMBER_EDIT);
    m_screenNumberSpin.SetBuddy(pBgScreenNumWnd);
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

    try
    {
        typedef std::vector<TA_Base_Core::IAction *> ActionList;
        ActionList allAction = TA_Base_Core::ActionAccessFactory::getInstance().getAllActions();
        for (ActionList::const_iterator iter = allAction.begin(); iter != allAction.end(); iter++)
        {
            int appTypeIdx = m_actionTypeCombo.AddString((*iter)->getName().c_str());
            m_actionTypeCombo.SetItemData(appTypeIdx, (*iter)->getKey());
            delete *iter;
        }
        allAction.clear();
    }
    catch (...)
    {
        return FALSE;
    }

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            int profileIdx = m_operatorProfileCombo.AddString((*iter)->getName().c_str());
            m_operatorProfileCombo.SetItemData(profileIdx, (*iter)->getKey());
            delete *iter;
        }
        allProfile.clear();
    }
    catch (...)
    {
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}

void TA_Base_App::CPlanStepEditBackgroundLaunchSchematicParameterPage::OnPickDataPointButton()
{
    FUNCTION_ENTRY( "OnPickDataPointButton" );

    UpdateData(); // TD#1537

    UpdateParameters();

    std::string prevDataPoint = m_dataPointParameter.getDataPoint();

    m_dataPointParameter.editParameter();

    if (prevDataPoint != m_dataPointParameter.getDataPoint())
    {
        UpdateParameters();
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)


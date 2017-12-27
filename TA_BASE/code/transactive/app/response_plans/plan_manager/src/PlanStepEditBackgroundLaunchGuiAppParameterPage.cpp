//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundLaunchGuiAppParameterPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditBackgroundLaunchGuiAppParameterPage.h"
#include "BackgroundLaunchGuiAppStep.h"
#include "GuiAppTypeStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
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
// CPlanStepEditBackgroundLaunchGuiAppParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditBackgroundLaunchGuiAppParameterPage, CPlanStepEditParameterPage)

CPlanStepEditBackgroundLaunchGuiAppParameterPage::CPlanStepEditBackgroundLaunchGuiAppParameterPage(BackgroundLaunchGuiAppStep* step) :
CPlanStepEditParameterPage(CPlanStepEditBackgroundLaunchGuiAppParameterPage::IDD, step),
m_xPositionEdit(true, false),
m_yPositionEdit(true, false),
m_widthEdit(true, false),
m_heightEdit(true, false),
m_actionTypeKey(*step->getActionType()),
m_dataPointParameter(*step, step->getDataPointDisplayParameter()->getName(), ""),
m_operatorProfileParameter(*step->getOperatorProfile())
{
    FUNCTION_ENTRY( "CPlanStepEditBackgroundLaunchGuiAppParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
    // }}AFX_DATA_INIT

    long screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    long screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    m_xPositionEdit.SetRange(0, screenWidth);
    m_yPositionEdit.SetRange(0, screenHeight);
    m_widthEdit.SetRange(0, screenWidth);
    m_heightEdit.SetRange(0, screenHeight);
    m_triggerEntity = FALSE;
    m_triggerEntityCheckAction = TRUE;

    FUNCTION_EXIT;
}


CPlanStepEditBackgroundLaunchGuiAppParameterPage::~CPlanStepEditBackgroundLaunchGuiAppParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditBackgroundLaunchGuiAppParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchGuiAppParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_appTypeCombo.EnableWindow(enable);
    m_commandLineEdit.SetReadOnly(!enable);
    m_xPositionEdit.SetReadOnly(!enable);
    m_yPositionEdit.SetReadOnly(!enable);
    m_widthEdit.SetReadOnly(!enable);
    m_heightEdit.SetReadOnly(!enable);
    CWnd* addTriggerEntity = GetDlgItem(IDC_TRIGGER_ENTITY_CHECK);
    if (addTriggerEntity)
    {
        addTriggerEntity->EnableWindow(enable);
    }

    CWnd* addTriggerEntityCheck = GetDlgItem(IDC_TRIGGER_ENTITY_ACTION_CHECK);
    if (addTriggerEntityCheck)
    {
        addTriggerEntityCheck->EnableWindow(enable);
    }

    
    CWnd* datapointBtn = static_cast<CWnd*>(GetDlgItem(IDC_PICK_DATA_POINT_BUTTON));
    if (datapointBtn)
    {
        datapointBtn->EnableWindow(enable);
    }

    CWnd* datapoint = static_cast<CWnd*>(GetDlgItem(IDC_DATA_POINT));
    if (datapoint)
    {
        datapoint->EnableWindow(enable);
    }

    CWnd* operatorProfileCombo = GetDlgItem(IDC_OPERATOR_PROFILE_COMBO);
    if (operatorProfileCombo)
    {
        operatorProfileCombo->EnableWindow(enable);
    }
    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundLaunchGuiAppParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    BackgroundLaunchGuiAppStep *step = dynamic_cast<BackgroundLaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();
    YesNoStepParameter   *yesnoParameter = step->getAddTriggerEntityParameter();
    NumericStepParameter *actionTypeKey = step->getActionType();


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

    m_triggerEntity = yesnoParameter->getAffirmative ();
    m_triggerEntityCheckAction = step->getTriggerEntityCheckActionParameter()->getAffirmative();

    CWnd* datapointBtn = static_cast<CWnd*>(GetDlgItem(IDC_PICK_DATA_POINT_BUTTON));
    if (datapointBtn)
    {
        datapointBtn->EnableWindow(!m_triggerEntityCheckAction);
    }

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


void CPlanStepEditBackgroundLaunchGuiAppParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    BackgroundLaunchGuiAppStep* step = dynamic_cast<BackgroundLaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();
    YesNoStepParameter *triggerEntityParameter = step->getAddTriggerEntityParameter();
    YesNoStepParameter *triggerEntityCheckParameter = step->getTriggerEntityCheckActionParameter();

    int profileIdx = m_operatorProfileCombo.GetCurSel();
    if (profileIdx != CB_ERR)
    {
        CString currentProfileName;
        m_operatorProfileCombo.GetLBText(profileIdx, currentProfileName);
        step->getOperatorProfile()->setValue(m_operatorProfileCombo.GetItemData(profileIdx));
        step->getOperatorProfileName()->setText((LPCTSTR)currentProfileName);
    }

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
    unsigned long xPosition = (editText.IsEmpty()? BackgroundLaunchGuiAppStep::DEFAULT_X_POSITION : atol(editText));
    xPositionParameter->setValue(xPosition);
    //    xPositionParameter->hideValue(editText.IsEmpty());

    m_yPositionEdit.GetWindowText(editText);
    unsigned long yPosition = (editText.IsEmpty()? BackgroundLaunchGuiAppStep::DEFAULT_Y_POSITION : atol(editText));
    yPositionParameter->setValue(yPosition);
    //    yPositionParameter->hideValue(editText.IsEmpty());

    m_widthEdit.GetWindowText(editText);
    unsigned long width = (editText.IsEmpty()? BackgroundLaunchGuiAppStep::DEFAULT_WIDTH : atol(editText));
    widthParameter->setValue(width);
    widthParameter->hideValue(editText.IsEmpty() != 0);

    m_heightEdit.GetWindowText(editText);
    unsigned long height = (editText.IsEmpty()? BackgroundLaunchGuiAppStep::DEFAULT_HEIGHT : atol(editText));
    heightParameter->setValue(height);
    heightParameter->hideValue(editText.IsEmpty() != 0);

    triggerEntityParameter->setAffirmative(m_triggerEntity != 0);
    triggerEntityCheckParameter->setAffirmative(m_triggerEntityCheckAction != 0);

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


void CPlanStepEditBackgroundLaunchGuiAppParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
    DDX_Control(pDX, IDC_Y_POS_EDIT, m_yPositionEdit);
    DDX_Control(pDX, IDC_X_POS_EDIT, m_xPositionEdit);
    DDX_Control(pDX, IDC_WIDTH_EDIT, m_widthEdit);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineEdit);
    DDX_Control(pDX, IDC_HEIGHT_EDIT, m_heightEdit);
    DDX_Control(pDX, IDC_APP_TYPE_COMBO, m_appTypeCombo);
    DDX_Check(pDX, IDC_TRIGGER_ENTITY_CHECK, m_triggerEntity);
    DDX_Control(pDX, IDC_ACTION_COMBO, m_actionTypeCombo);
    DDX_Text(pDX, IDC_DATA_POINT, m_dataPointLabel);
    DDX_Check(pDX, IDC_TRIGGER_ENTITY_ACTION_CHECK, m_triggerEntityCheckAction);
    DDX_Control(pDX, IDC_OPERATOR_PROFILE_COMBO, m_operatorProfileCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditBackgroundLaunchGuiAppParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
    ON_BN_CLICKED(IDC_PICK_DATA_POINT_BUTTON, OnPickDataPointButton)
    ON_BN_CLICKED(IDC_TRIGGER_ENTITY_ACTION_CHECK, OnTriggerEntityActionCheckClick)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditBackgroundLaunchGuiAppParameterPage message handlers

BOOL CPlanStepEditBackgroundLaunchGuiAppParameterPage::OnInitDialog()
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
            int appTypeIdx = m_operatorProfileCombo.AddString((*iter)->getName().c_str());
            m_operatorProfileCombo.SetItemData(appTypeIdx, (*iter)->getKey());
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

void TA_Base_App::CPlanStepEditBackgroundLaunchGuiAppParameterPage::OnPickDataPointButton()
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

    FUNCTION_EXIT("OnPickDataPointButton");
}

void TA_Base_App::CPlanStepEditBackgroundLaunchGuiAppParameterPage::OnTriggerEntityActionCheckClick()
{

    FUNCTION_ENTRY("OnTriggerEntityActionCheckClick");
    UpdateData();

    CWnd* datapointBtn = static_cast<CWnd*>(GetDlgItem(IDC_PICK_DATA_POINT_BUTTON));
    if (datapointBtn)
    {
        datapointBtn->EnableWindow(!m_triggerEntityCheckAction);
    }
    FUNCTION_EXIT("OnTriggerEntityActionCheckClick");
}

#endif // defined (STEPTYPE_LAUNCH_GUI_APP)

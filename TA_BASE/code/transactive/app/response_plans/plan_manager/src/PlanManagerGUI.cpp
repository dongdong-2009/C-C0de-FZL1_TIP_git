//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerGUI.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2012/08/03 17:02:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "PlanManager.h"
#include "PlanManagerGUI.h"
#include "MainFrame.h"
#include "AllPlanDoc.h"
#include "ApprovedPlanDoc.h"
#include "UnapprovedPlanDoc.h"
#include "ActivePlanDoc.h"
#include "AllPlanTreeFrame.h"
#include "ApprovedPlanTreeFrame.h"
#include "UnapprovedPlanTreeFrame.h"
#include "ActivePlanFrame.h"
#include "AllPlanTreeView.h"
#include "ApprovedPlanTreeView.h"
#include "UnapprovedPlanTreeView.h"
#include "ActivePlanListView.h"
#include "PlanManagerCommonDefs.h"
#include "MessageBox.h"
#include "OperatorRights.h"
#include "FloatingWindowThreadRegister.h"


#include "TreeNodeFactory.h"
#include "OperatorRights.h"
#include "PlanAgentAccess.h"
#include "PlanAgentUpdateProcessor.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;


/////////////////////////////////////////////////////////////////////////////
// PlanManagerGUI

PlanManagerGUI::PlanManagerGUI() : m_execPlanIdChanged(false),
m_viewPlanIdChanged(false),
m_assocAlarmIdChanged (false)
{
    FUNCTION_ENTRY( "PlanManagerGUI" );
    FUNCTION_EXIT;
}


PlanManagerGUI::~PlanManagerGUI()
{
    FUNCTION_ENTRY( "~PlanManagerGUI" );

	//prepareForClose();

    FUNCTION_EXIT;
}


void PlanManagerGUI::selectStartupView()
{
    FUNCTION_ENTRY( "selectStartupView" );

    TA_ASSERT(getApplicationWnd(), "Main frame not initialised");

    // The presence of the --exec-plan-id parameter overrides the access rights based rules
    // used to determine the initial startup screen. In this case the Approved Plans view
    // is always activated.
    RunParams &runParams = RunParams::getInstance();
    if (!runParams.get(RPARAM_EXECPLANID).empty())
    {
        getApplicationWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

        // Set RPARAM_FWDEXECPLANID to the value of RPARAM_EXECPLANID.
        // This will trigger a notification which is handled by the Approved Plans view.
        runParams.set(RPARAM_FWDEXECPLANID, runParams.get(RPARAM_EXECPLANID).c_str());
    }
    if (!runParams.get(RPARAM_VIEWPLANID).empty())
    {
        getApplicationWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

        // Set RPARAM_FWDVIEWPLANID to the value of RPARAM_VIEWPLANID.
        // This will trigger a notification which is handled by the Approved Plans view.
        runParams.set(RPARAM_FWDVIEWPLANID, runParams.get(RPARAM_VIEWPLANID).c_str());
    }
    if (!runParams.get(RPARAM_ASSOC_ALARMID).empty())
    {
        getApplicationWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

        // Set RPARAM_FWDEXECPLANID to the value of RPARAM_EXECPLANID.
        // This will trigger a notification which is handled by the Approved Plans view.
        runParams.set(RPARAM_FWDASSOCALARMID, runParams.get(RPARAM_ASSOC_ALARMID).c_str());
    }
    else
    {
        // The rules used to determine the initial startup screen are based
        // on operator access rights and are as follows:
        // 
        // Execution | Editing | View
        // ----------+---------+-----------------
        // No    |   No    | All Plans
        // Yes   |   No    | Approved Plans
        // No    |   Yes   | Unapproved Plans
        // Yes   |   Yes   | All Plans

        bool haveExecutionRights = OperatorRights::getInstance().canControlPlans();
        bool haveEditingRights = OperatorRights::getInstance().canEditCategories() || OperatorRights::getInstance().canEditPlans();

        int startupView = ID_VIEW_ALLPLANS; // default startup view

        if (haveExecutionRights && !haveEditingRights)
        {
            startupView = ID_VIEW_APPROVEDPLANS;
        }
        else if (!haveExecutionRights && haveEditingRights)
        {
            startupView = ID_VIEW_UNAPPROVEDPLANS;
        }

        // Activate the appropriate child frame window
        getApplicationWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(startupView, 0), 0);
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// AbstractGUIApplication operations

void PlanManagerGUI::createApplicationObject()
{
    FUNCTION_ENTRY( "createApplicationObject" );

    ::SetThreadLocale( MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED) );

    // Register the application's document templates.  Document templates
    // serve as the connection between documents, frame windows and views.
    CMultiDocTemplate* docTemplate;
    docTemplate = new CMultiDocTemplate(
        IDR_PLAN_TREE_VIEW,
        RUNTIME_CLASS(CAllPlanDoc),
        RUNTIME_CLASS(CAllPlanTreeFrame),
        RUNTIME_CLASS(CAllPlanTreeView));
    AfxGetApp()->AddDocTemplate(docTemplate);

    docTemplate = new CMultiDocTemplate(
        IDR_PLAN_TREE_VIEW,
        RUNTIME_CLASS(CApprovedPlanDoc),
        RUNTIME_CLASS(CApprovedPlanTreeFrame),
        RUNTIME_CLASS(CApprovedPlanTreeView));
    AfxGetApp()->AddDocTemplate(docTemplate);

    docTemplate = new CMultiDocTemplate(
        IDR_PLAN_TREE_VIEW,
        RUNTIME_CLASS(CUnapprovedPlanDoc),
        RUNTIME_CLASS(CUnapprovedPlanTreeFrame),
        RUNTIME_CLASS(CUnapprovedPlanTreeView));
    AfxGetApp()->AddDocTemplate(docTemplate);

    docTemplate = new CMultiDocTemplate(
        IDR_ACTIVE_PLAN_VIEW,
        RUNTIME_CLASS(CActivePlanDoc),
        RUNTIME_CLASS(CActivePlanFrame),
        RUNTIME_CLASS(CActivePlanListView));
    AfxGetApp()->AddDocTemplate(docTemplate);

    // Create main MDI frame window
    CMainFrame* mainFrame = new CMainFrame(this);
    if (!mainFrame->LoadFrame(IDR_MAINFRAME))
    {
        TA_THROW(GenericGUIException("Unable to load main Plan Manager frame",
            GenericGUIException::INITIALISATION_ERRORS));
    }
    AfxGetApp()->m_pMainWnd = mainFrame;

    // Store a pointer for AbstractGUIApplication to use
    setApplicationWnd(*mainFrame);

    // Create child MDI frame windows and their respective views
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    TA_ASSERT(pos, "First document template position is null");
    while (pos)
    {
        CDocTemplate *planDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
        TA_ASSERT(planDocTemplate, "Plan document template pointer is null");

        // Create a single document from the given template. This will
        // instantiate the corresponding child frame and associated view.
        // Do not make the frame visible yet.
        CDocument *planDoc = planDocTemplate->OpenDocumentFile(NULL, FALSE);
        TA_ASSERT(planDoc, "Plan document pointer is null");
        CPlanBrowserDoc * pBaseDoc = dynamic_cast<CPlanBrowserDoc *>(planDoc);
        TA_ASSERT(pBaseDoc, "Plan document pointer is null");
        m_vecDocList.push_back(pBaseDoc);
    }

    TA_Base_Bus::CPlanActiveInstanceListCtrl::PLAN_COLUMN_LABEL = "预案";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::CATEGORY_COLUMN_LABEL = "类型";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::INSTANCE_COLUMN_LABEL = "实例";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::LOCATION_COLUMN_LABEL = "位置";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::OWNER_COLUMN_LABEL = "用户";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::ACTIVATION_COLUMN_LABEL = "激活";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::STATE_COLUMN_LABEL = "情形";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::STEP_COLUMN_LABEL = "步骤";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::REMARKS_COLUMN_LABEL = "备注";
    TA_Base_Bus::CPlanTreeCtrl::RootItemName = "所有预案";
    TA_Base_Bus::StringUtilities::RUNNING_PLAN_EXEC_STATE = "运行";
    TA_Base_Bus::StringUtilities::OPERATOR_PAUSED_EXEC_STATE = "运行";
    TA_Base_Bus::StringUtilities::STOPPED_PLAN_EXEC_STATE = "停止";
    TA_Base_Bus::StringUtilities::PAUSED_PLAN_EXEC_STATE = "暂停";
    TA_Base_Bus::StringUtilities::DELAYED_PLAN_EXEC_STATE = "运行";
    TA_Base_Bus::StringUtilities::COMPLETED_PLAN_EXEC_STATE = "完成";
    TA_Base_Bus::StringUtilities::ABORTED_PLAN_EXEC_STATE = "中止";
    TA_Base_Bus::StringUtilities::FAILED_PLAN_EXEC_STATE = "失败";
    TA_Base_Bus::StringUtilities::PENDING_PLAN_EXEC_STATE = "待定";
    TA_Base_Bus::StringUtilities::UNDEFINED_PLAN_EXEC_STATE = "未定义";

    FUNCTION_EXIT;
}


void PlanManagerGUI::prepareForClose()
{
	//cl-20371
	HWND hWnd = NULL;
	DWORD dwProcessId = 0;
	DWORD dwExitCode = 0;
    try
    {
        saveAllUserSettings();
    }
    catch (UserSettingsException&)
    {
        MessageBox::error(IDS_SAVE_USER_SETTINGS);
    }

	CWnd* pWnd = getApplicationWnd();
    if ( NULL != pWnd )
    {
        hWnd = pWnd->GetSafeHwnd();
    }

    if( hWnd == NULL || ( !::IsWindow( hWnd ))) 
	{
			TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
			FloatingWindowThreadRegister::getInstance().terminateAllThreads();
	}
	else
	{
	    FloatingWindowThreadRegister::destroyInstance();
        TreeNodeFactory::destroyInstance();
        PlanAgentAccess::destroyInstance();
        PlanAgentUpdateProcessor::destroyInstance();
        OperatorRights::destroyInstance();
	    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
	    FloatingWindowThreadRegister::getInstance().terminateAllThreads();
	    GetWindowThreadProcessId( hWnd, &dwProcessId );
	    HANDLE hHandle = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
	    GetExitCodeProcess(hHandle, &dwExitCode);
        TerminateProcess(hHandle, dwExitCode);
	}
}

// TD 11846 Fix: Changed setPosition's signature to follow AbstractGUIApplication's setPosition's signature.
void PlanManagerGUI::setPosition(const unsigned long posFlag, const unsigned long alignFlag, const RECT& objectDim, const RECT& boundaryDim )
{
    FUNCTION_ENTRY( "setPosition" );

    AbstractDocumentGUI::setPosition(posFlag, alignFlag, objectDim, boundaryDim);

    // The Plan Manager window has been moved. A preceeding ExecPlanId parameter update can
    // now be forwarded for processing. The resulting Plan Controller should display on top
    // of Plan Manager.
    if (m_execPlanIdChanged)
    {
        RunParams::getInstance().set(RPARAM_FWDEXECPLANID, RunParams::getInstance().get(RPARAM_EXECPLANID).c_str());

        m_execPlanIdChanged = false;
    }

    if (m_viewPlanIdChanged)
    {
        RunParams::getInstance().set(RPARAM_FWDVIEWPLANID, RunParams::getInstance().get(RPARAM_VIEWPLANID).c_str());

        m_viewPlanIdChanged = false;
    }

    // similar for Assoc Alarm
    if (m_assocAlarmIdChanged)
    {
        RunParams::getInstance().set(RPARAM_FWDASSOCALARMID, RunParams::getInstance().get(RPARAM_ASSOC_ALARMID).c_str());
        m_assocAlarmIdChanged = false;
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIApplication operations

unsigned long PlanManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );
    FUNCTION_EXIT;
    return PLAN_MANAGER_GUI_APPTYPE;
}


void PlanManagerGUI::checkEntity(IEntityData* guiEntity)
{
    FUNCTION_ENTRY( "checkEntity" );

    PlanManagerEntityData *pmEntityData = dynamic_cast<PlanManagerEntityData *> (guiEntity);
    if (!pmEntityData)
    {
        TA_THROW(GenericGUIException("Entity configuration is not of type Plan Manager", GenericGUIException::ENTITY_CONFIGURATION_INVALID));
    }

    // Entity config OK
    setGuiEntity(*guiEntity);

    // Set the root directory of the flowchart as a run param so that we could access it
    // in other parts of the app
    RunParams::getInstance().set(RPARAM_FLOWCHARTDIR, pmEntityData->getFlowchartDirectory().c_str());
    RunParams::getInstance().set(RPARAM_REPORTTEMPLATE, pmEntityData->getReportTemplate().c_str());
    RunParams::getInstance().set(RPARAM_MAXCATEGORY, pmEntityData->getMaxCategoryDepth().c_str() );

    TA_Base_Core::RunParams::getInstance().set(RPARAM_APPNAME, "Plan Manager");

    FUNCTION_EXIT;
}


void PlanManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );
    FUNCTION_EXIT;
}


void PlanManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );

    PlanManagerEntityData *pmEntityData = dynamic_cast<PlanManagerEntityData *> (getGUIEntity());
    TA_ASSERT(pmEntityData != NULL, "The GUI Entity cannot be converted into Plan Manager entity");
    RunParams::getInstance().set(RPARAM_FLOWCHARTDIR, pmEntityData->getFlowchartDirectory().c_str());

    // TD 13265
    RunParams::getInstance().set(RPARAM_REPORTTEMPLATE, pmEntityData->getReportTemplate().c_str());

    FUNCTION_EXIT;
}


void PlanManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );
    FUNCTION_EXIT;
}


void PlanManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIAccess operations

void PlanManagerGUI::loadAllUserSettings()
{
    FUNCTION_ENTRY( "loadAllUserSettings" );

    // First check if we should be loading the user settings.
    if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
        setWindowPosition();

        FUNCTION_EXIT;
        return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new SettingsMgr(getSessionId(), getApplicationType())); // AbstractGUIApplication cleans this up
    }

    loadPositionSettings();

    // TODO: Restore any other settings here

    FUNCTION_EXIT;
}


void PlanManagerGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY( "saveAllUserSettings" );

    // First check if we should be saving the user settings.
    if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
        FUNCTION_EXIT;
        return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new SettingsMgr(getSessionId(), getApplicationType())); // AbstractGUIApplication cleans this up
    }

    savePositionSettings();

    // TODO: Save any other settings here

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// RunParamUser operations

void PlanManagerGUI::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY( "onRunParamChange" );

    if (name == RPARAM_EXECPLANID)
    {
        // ExecPlanId update ultimately results in a Plan Controller being opened. However,
        // this needs to be postponed until the Plan Manager window has been positioned
        // by the Control Station so that Plan Controller displays on top of Plan Manager.
        // The ExecPlanId change is processed in setPosition().
        m_execPlanIdChanged = true;
    }
    else if (name == RPARAM_VIEWPLANID)
    {
        m_viewPlanIdChanged = true;
    }
    else if (name == RPARAM_SESSIONID)
    {
        // SessionId update will cause the active plans to fail if they require interactive
        // steps or if a plan is paused (TD 10849).
        ::PostMessage(getApplicationWnd()->GetSafeHwnd(), WM_SESSION_ID_CHANGED, 0, 0);
    }
    else if (name == RPARAM_ASSOC_ALARMID)
    {
        // as per ExecPlanId , the AssocAlarmId will result in Plan Controller being opened
        // which needs to be postponed
        // AssocAlarmId is processed in setPosition
        m_assocAlarmIdChanged = true;
    }

    FUNCTION_EXIT;
}


//TD18095, jianghp
void PlanManagerGUI::onInitGenericGUICompleted()
{
    FUNCTION_ENTRY( "onInitGenericGUICompleted" );

    try
    {
     //   getApplicationWnd()->LockWindowUpdate();
        DocumentList::iterator it = m_vecDocList.begin();
        for (;it != m_vecDocList.end(); ++it)
        {
            TA_ASSERT((*it) != NULL, "the document pointer is NULL");
            (*it)->OnGenericGuiInitCompleted();
        }
        // Include in the title the location of the Plan Agent used for accessing plan data,
        // as well as editing and running plans.
        CString title;
        if (!title.LoadString(IDR_MAINFRAME))
        {
            title = "Plan Manager";
        }

        title += " - ";
        title += TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocationName().c_str();

        CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(getApplicationWnd());
        TA_ASSERT(pMainFrame != NULL, "the main frame windows is NULL!");
        // Update the child frames and their views.
        pMainFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

		pMainFrame->SetBaseTitle(title.GetBuffer());
        title.ReleaseBuffer();

        // Show the approriate plan tree, depending on operator privileges.
        selectStartupView();

		// Register for --view-plan-id runtime parameter updates."ViewPlanId"
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_VIEWPLANID);
        // Register for --exec-plan-id runtime parameter updates.
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_EXECPLANID);
        // Register for --session-id runtime parameter updates (TD 10849).
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
        // Register for --assoc-alarm-id runtime parameter updates.
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_ASSOC_ALARMID);

       pMainFrame->initCorbaObj();
      // getApplicationWnd()->UnlockWindowUpdate();
      //  getApplicationWnd()->UpdateWindow();
    }
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Exception thrown by PlanManagerGUI", "something wrong in initializing the corba objects" );
    }

    FUNCTION_EXIT;
}



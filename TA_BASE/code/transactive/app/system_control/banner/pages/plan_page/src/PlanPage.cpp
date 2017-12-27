/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/pages/plan_page/src/PlanPage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanPage.cpp : implementation file
//

#include <sstream>

#include "app/system_control/banner/src/stdafx.h"
#include "app/system_control/banner/src/banner.h"
#include "app/system_control/banner/pages/plan_page/src/PlanPage.h"
#include "app/system_control/banner/pages/plan_page/src/PlanBannerPage_rc.h"
#include "app/system_control/banner/src/BannerDlg.h"

#include "bus/banner_framework/src/constants.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/applauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/resources/resource.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "windows.h"

//Td_jinhua++
const int FLASHTIMERID = 1;
const int FLASHELAPSE = 3000;
/////////////////////////////////////////////////////////////////////////////
// PlanPage dialog


PlanPage::PlanPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
	: BannerPage(buttonName, PlanPage::IDD, xPos, yPos, width, height, parentDialog)
{
	//{{AFX_DATA_INIT(PlanPage)
    TA_Base_Bus::CPlanActiveInstanceListCtrl::PLAN_COLUMN_LABEL = "预案";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::CATEGORY_COLUMN_LABEL = "类型";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::INSTANCE_COLUMN_LABEL = "实例";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::LOCATION_COLUMN_LABEL = "位置";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::OWNER_COLUMN_LABEL = "用户";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::ACTIVATION_COLUMN_LABEL = "激活";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::STATE_COLUMN_LABEL = "情形";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::STEP_COLUMN_LABEL = "步骤";
    TA_Base_Bus::CPlanActiveInstanceListCtrl::REMARKS_COLUMN_LABEL = "备注";
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
	//}}AFX_DATA_INIT
}


PlanPage::~PlanPage()
{
	try
    {
        if (m_rightsLibrary != NULL)
        {
            delete m_rightsLibrary;
            m_rightsLibrary = NULL;
        }
    }
    catch ( ... )
    {
    }

}


void PlanPage::DoDataExchange(CDataExchange* pDX)
{
	BannerPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlanPage)
	DDX_Control(pDX, IDC_ALL_OWNERS_BUTTON, m_allOwnersBtn);
	DDX_Control(pDX, IDC_ALL_LOCATIONS_BUTTON, m_allLocationsBtn);
	DDX_Control(pDX, IDC_PLANLIST, m_planListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PlanPage, BannerPage)
	//{{AFX_MSG_MAP(PlanPage)
	ON_BN_CLICKED(IDC_ALL_OWNERS_BUTTON, OnAllOwnersCheck)
	ON_BN_CLICKED(IDC_ALL_LOCATIONS_BUTTON, OnAllLocationsCheck)
	ON_NOTIFY(NM_DBLCLK, IDC_PLANLIST, OnDblclkPlanlist)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlanPage message handlers

BOOL PlanPage::OnInitDialog() 
{
	BannerPage::OnInitDialog();

    TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;
    m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

    // Set All Owners and All Locations button colour
    m_allOwnersBtn.SetCheckedColour(BANNER_CONST::CCL_GOLD);
    m_allLocationsBtn.SetCheckedColour(BANNER_CONST::CCL_GOLD);

    // Set All Owners and All Locations button initial state
    m_allOwnersBtn.setCheck(m_planListCtrl.allOwnersNeeded());
    m_allLocationsBtn.setCheck(m_planListCtrl.allLocationsNeeded());

	// Adjust size of list control and scale column widths accordingly
    int left = 106;
    int top = 0;
	int width = getWidth() - 2 - left;
    int height = getHeight() - 2;

	m_planListCtrl.MoveWindow(left, top, width, height);
	m_planListCtrl.AutoAdjustColumns();
	m_headerCtrl.subclassHeader(m_planListCtrl.GetHeaderCtrl());//TD16118
    // Enable the All Locations button in accordance with operator rights 
    SetAllLocationsBtnEnableState();

    // Register for Session ID runtime parameter updates.
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

	//TD_JINHUA++
	SetTimer(FLASHTIMERID, FLASHELAPSE, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void PlanPage::onButtonDoubleClick()
{
    // Launch Plan Manager with no extra command line arguments
    LaunchPlanManager("");
}


void PlanPage::addBannerPage()
{
    TA_Base_Bus::BannerIcon bannerIcon;
    bannerIcon.onIcon = IDB_PLAN;
    bannerIcon.offIcon = IDB_PLANG;
	bannerIcon.onDownIcon = IDB_PLAND;
	bannerIcon.offDownIcon = IDB_PLANDG;

    getParentDialog()->AddBannerPage(IDD_PLANS, getButtonName(), bannerIcon, this);
}


void PlanPage::OnAllOwnersCheck()
{
    // Toggle the button state. Force an immediate repaint of button background.
    m_allOwnersBtn.setCheck(!m_allOwnersBtn.isChecked());
    m_allOwnersBtn.UpdateWindow();

    // Adjust plan list contents accordingly.
    m_planListCtrl.showAllOwners(m_allOwnersBtn.isChecked());
}


void PlanPage::OnAllLocationsCheck()
{
    // Toggle the button state. Force an immediate repaint of button background.
    m_allLocationsBtn.setCheck(!m_allLocationsBtn.isChecked());
    m_allLocationsBtn.UpdateWindow();

    try
    {
        // Adjust plan list contents accordingly.
        m_planListCtrl.showAllLocations(m_allLocationsBtn.isChecked());
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());

        // Revert button state.
        m_allLocationsBtn.setCheck(!m_allLocationsBtn.isChecked());
        m_allLocationsBtn.UpdateWindow();

        std::ostringstream errorMessage;
        errorMessage << "Unable to apply location filter to active plan display." << std::endl
                     << ex.what();

		// TD14164 azenitha++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString reason = (errorMessage.str()).c_str();
		userMsg << reason;
		CString errMsg = userMsg.constructMessage(IDS_UE_020071);
		/*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
		AfxMessageBox(errMsg, MB_ICONERROR | MB_OK);
		// TD14164 ++azenitha
    }
}


void PlanPage::OnDblclkPlanlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMITEMACTIVATE* pNMITEMACTIVATE = (NMITEMACTIVATE*)pNMHDR;

	//
	// If double clicked on Plan Instance, launch the PlanManager with the PlanController 
	// loaded with the related Plan
	//
	int clickedOnIndex = m_planListCtrl.HitTest(pNMITEMACTIVATE->ptAction);
	if (clickedOnIndex >= 0)
	{
		// Double clicked on instance so determine its attributes
        TA_Base_Core::ActivePlanId planInstanceId = m_planListCtrl.GetPlanInstanceId(clickedOnIndex);

		// Plan Manager takes the plan ID, instance number and location key in the following format:
        // --exec-plan-id=<plan ID>:<instance #>:<location key>
		std::ostringstream commandLine;
        commandLine << "--view-plan-id=" << planInstanceId.plan 
                    << ":" << planInstanceId.instance
                    << ":" << planInstanceId.location;

		LaunchPlanManager(commandLine.str());		
	}

	*pResult = 0;
}


void PlanPage::onRunParamChange(const std::string& name, const std::string& value)
{
    if (name == RPARAM_SESSIONID)
    {
        // Session ID can change on operator override. When this happens operator rights 
        // have to be re-evaluated.
        SetAllLocationsBtnEnableState();
    }
}


void PlanPage::LaunchPlanManager(const std::string &extraCommandLine)
{
	try
	{
        // Launch the Plan Manager on the same screen as the banner.
        CRect bounds = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
                                                                  TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, 
                                                                  getXPosition());

		TA_Base_Bus::AppLauncher::getInstance().launchApplication(PLAN_MANAGER_GUI_APPTYPE, 
                                                             extraCommandLine,
                                                             TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
                                                             TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
                                                             NULL, 
                                                             bounds);
	}
	catch (const TA_Base_Core::ApplicationException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", ex.what());

        CString applicationName;
        applicationName.LoadString(IDS_PLAN_MANAGER);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << applicationName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
    }	
}


void PlanPage::SetAllLocationsBtnEnableState()
{
    bool canViewAllLocations = false;
	
	
	for(int retry=0; retry <2; retry ++) //CL-20663++: Plan Banner will try one more time if the rights of "ALL LOCATION" cannot be determined.
	{
		try
		{
			// Determine the operator's rights to view active plans at all locations.
			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_ASSERT(!entityName.empty(), "Entity name not set. This should have been picked up on startup!!!");

			std::auto_ptr<TA_Base_Core::IEntityData> bannerEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
			TA_ASSERT(bannerEntityData.operator->(), "Banner entity data is null");

			std::auto_ptr<TA_Base_Core::IResource> bannerResource(
				TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(bannerEntityData->getKey()));

			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_ASSERT(!sessionId.empty(), "Session ID not set");

	//        TA_Base_Bus::RightsLibraryFactory rightsFactory;
	//        std::auto_ptr<TA_Base_Bus::RightsLibrary> rightsLib(rightsFactory.buildRightsLibrary());

			TA_Base_Bus::EDecisionModule decisionModule;
			std::string reasonForDenial;

			canViewAllLocations = m_rightsLibrary->isActionPermittedOnResource(sessionId, bannerResource->getKey(), 
																		 TA_Base_Bus::aca_PLAN_VIEW_ACTIVE_ALL_LOCATIONS, 
																		 reasonForDenial, decisionModule);
			retry = 2; //CL-20663++: Plan Banner got the rights of "ALL LOCATION", need not retry one more time.
		}
		catch (const TA_Base_Core::TransactiveException &ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());

			//CL-20663++: Remove the pop up error msg.
			/*
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_020069);
			*/
			//++CL-20663
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");

			//CL-20663++: Remove the pop up error msg.
			/*
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_020069);
			*/
			//++CL-20663
		}
	}
    // Enable the All Locations button according to user rights.
    m_allLocationsBtn.EnableWindow(canViewAllLocations);

    // If operator is not permitted to access active plans at all locations, depress the All Locations
    // button and make sure that only local plan instances are shown in the list.
    if (!canViewAllLocations)
    {
        m_allLocationsBtn.setCheck(false);
        m_planListCtrl.showAllLocations(false);
    }
}


//TD_jinhua++
void PlanPage::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_planListCtrl.GetItemCount() > 0 )
	{
		enableButtonFlash();	
	}
	else
	{
		disableButtonFlash();
	}
	//TStackedPage::OnTimer(nIDEvent);
	CDialog::OnTimer(nIDEvent);
}

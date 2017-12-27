// SubsystemDutyDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"
#include "bus/generic_gui/src/TransactiveMessage.h" 

using namespace TA_Base_Bus;
namespace
{
	const TA_Base_Bus::LocationKey ALL_LOCATION(0xFFFFFFFE); //0 is all location if central profile //TD15533
}
namespace TA_Base_App
{
	// SubsystemDutyDlg dialog

	IMPLEMENT_DYNAMIC(SubsystemDutyDlg, CDialog)

		SubsystemDutyDlg::SubsystemDutyDlg(CWnd* pParent /*=NULL*/, bool bRevoke)
		: CDialog(SubsystemDutyDlg::IDD, pParent)
		, bRevoke_(bRevoke),				
		sessionCache_(DutyDataStore::getSessionCache()),
		dataCache_(DutyDataStore::getDataCache()),
		subsystemDutyMatrix_(DutyDataStore::getSubsystemDutyMatrix())	
	{
		currentSessionInfo_ = sessionCache_.getSessionInfo(sessionCache_.getCurrentSessionId());
	}

	SubsystemDutyDlg::~SubsystemDutyDlg()
	{
	}

	void SubsystemDutyDlg::InitControlTexts()
	{
		FUNCTION_ENTRY("InitControlTexts");

		CString title,action, all;
		if(bRevoke_)
		{
			title       = _T("收回子系统权限");
			action  = _T("收回");
			all        = _T("全线收回");

			if (sessionCache_.isContainCentralProfile(sessionCache_.getCurrentSessionId()))
			{
				showAllSubsystemsChkBox_.EnableWindow(TRUE);
			}
		}
		else
		{
			title       = _T("移交子系统权限");
			action  = _T("移交");
			all        = _T("全线移交");
		}

		this->SetWindowText(title);
		allBtn_.SetWindowText(all);
		actionBtn_.SetWindowText(action);

		FUNCTION_EXIT;
	}

	void SubsystemDutyDlg::ShowAllSubsystemsList(bool _isShowAllSubsystems)
	{
		// Clear all content first
		clbSubystem_.ResetContent();

		SubsystemKeyList subsystemKeys = subsystemDutyMatrix_.getRowIdsByRights(currentSessionInfo_.ProfileId);

		std::sort(subsystemKeys.begin(), subsystemKeys.end());

		int nIdx = -0;
		for(SubsystemKeyList::const_iterator itr = subsystemKeys.begin(); itr != subsystemKeys.end(); itr ++)
		{
			SubsystemKey  key = * itr;

			std::string name;
			std::string displayName;

			if((_isShowAllSubsystems == false
				&& dataCache_.isExclusiveSubSystem(key))
				|| (_isShowAllSubsystems == true))
			{
				name = dataCache_.getSubsystemName(key);
				displayName = dataCache_.getSubsystemDisplayName(key);
				if(displayName.empty() || displayName.length() == 0)
				{
					nIdx  = clbSubystem_.AddString(name.c_str());
					clbSubystem_.SetItemData(nIdx,key);
				}
				else
				{
					nIdx = clbSubystem_.AddString(displayName.c_str());
					clbSubystem_.SetItemData(nIdx,key);
				}
			}
		}
	}

	void SubsystemDutyDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType,cx,cy);
	
	}

	SubsystemKeyList 		SubsystemDutyDlg::GetTransferableSubsystems()
	{

		FUNCTION_ENTRY("GetTransferableSubsystems");

		TransferableSubsystemList subsystems; 

		RegionKeyList regions;

		// HACK current implementation the location id is identical to region id
		regions.push_back(currentSessionInfo_.LocationId);		
		// TODO get grantable duties
		//subsystems = dutyAgent_.getTransferableSubsystems(currentSessionInfo_.SessionId ,regions);
		subsystems = dutyAgent_.getTransferableSubsystems(currentSessionInfo_ ,regions);

		SubsystemKeyList result;
		
		for(TransferableSubsystemList::const_iterator itr = subsystems.begin(); itr != subsystems.end(); itr++)
		{
			result.push_back(itr->subsystemKey);
		}

		FUNCTION_EXIT;

		return result;
	}
	BOOL SubsystemDutyDlg::OnInitDialog()
	{
			FUNCTION_ENTRY("OnInitDialog");

			CDialog::OnInitDialog();		

			//currentSessionInfo_ = sessionCache_.getSessionInfo(sessionCache_.getCurrentSessionId());
			//set title and caption of buttons according to internal state
			this->InitControlTexts();

			/*
			//subsystems associated with the profiles
		    SubsystemKeyList subsystemKeys = subsystemDutyMatrix_.getRowIdsByRights(currentSessionInfo_.ProfileId);

			//TODO according to the flag, get the subsystems current session has duty or has no duty
			//get subsystems the current session has duty
			//SubsystemKeyList transferableSubsystems  =  GetTransferableSubsystems();

			std::sort(subsystemKeys.begin(), subsystemKeys.end());
			//std::sort(transferableSubsystems.begin(), transferableSubsystems.end());

			//assumed that  subsystemKeys.size() > transferableSubsystems.size() 
			//	subsystemKeys is a superset of transferableSubsystems

			int nIdx = -0;
			for(SubsystemKeyList::const_iterator itr = subsystemKeys.begin(); itr != subsystemKeys.end(); itr ++)
			{
				SubsystemKey  key = * itr;

				if(dataCache_.isExclusiveSubSystem(key))
				{
					std::string name = dataCache_.getSubsystemName(key);
					std::string displayName = dataCache_.getSubsystemDisplayName(key);
					if(displayName.empty() || displayName.length() == 0)
					{
						nIdx  = clbSubystem_.AddString(name.c_str());
						clbSubystem_.SetItemData(nIdx,key);
					}
					else
					{
						nIdx = clbSubystem_.AddString(displayName.c_str());
						clbSubystem_.SetItemData(nIdx,key);
					}

					// TODO: if the current user doesn't have the duty, the check box should be disabled. Check the duty
					//SubsystemKeyList::const_iterator itrFound =std::find(transferableSubsystems.begin(),transferableSubsystems.end(), key);
					//if(itrFound == transferableSubsystems.end())
					//{
					//	clbSubystem_.Enable(nIdx,FALSE);
					//}
					//
					
				}
			}
			*/
			this->ShowAllSubsystemsList(false);

			RegionKeyList regionKeys =subsystemDutyMatrix_.getColumnIds();
			
			regionsLv_.SetExtendedStyle(regionsLv_.GetExtendedStyle() |  LVS_EX_INFOTIP| LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES );
			regionsLv_.InsertColumn(0, _T("Dummy"), LVCFMT_LEFT, 20);
			regionsLv_.InsertColumn(1,_T("Location"), LVCFMT_LEFT, 320);

			 for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
			 {
					std::string name = dataCache_.getRegionName(*regionIt);

					if(name.compare("All Locations")  == 0 ) continue;

					int nItem = regionsLv_.InsertItem(regionsLv_.GetItemCount(), _T("")); //for checkbox

					std::string displayName = dataCache_.getRegionDisplayName(*regionIt);
					if(displayName.empty() || displayName.length() ==0 )
					{
						regionsLv_.SetItemText(nItem,1, name.c_str());
					}
					else
					{
						regionsLv_.SetItemText(nItem,1, displayName.c_str());
					}
					regionsLv_.SetItemData(nItem,(*regionIt));
			 }
			
			FUNCTION_EXIT;
			return TRUE;
	}
	void SubsystemDutyDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_ACTION_BTN, actionBtn_);
		DDX_Control(pDX, IDC_ALL_BTN, allBtn_);
		DDX_Control(pDX, IDC_LIST_REGIONS, regionsLv_);
		DDX_Control(pDX,IDC_LIST_SUBSYSTEMS,clbSubystem_);
		DDX_Control(pDX,IDC_CHECKBOX_SHOW_ALL_SUBSYSTEMS,showAllSubsystemsChkBox_);
	}

	/**
	*  Get Seleted Regions
	*/
	RegionKeyList SubsystemDutyDlg::GetSelectedRegions()
	{
			FUNCTION_ENTRY("GetSelectedRegions");

			RegionKeyList regions;

			int nCount = regionsLv_.GetItemCount();

			for(int i=0; i< nCount; i++)
			{
				if(regionsLv_.GetCheck(i) != 0 )
				{
					DWORD_PTR data = regionsLv_.GetItemData(i);
					RegionKey key = (RegionKey) data;
					regions.push_back(key);
				}
			}

			FUNCTION_EXIT;
			return regions;
	}
	RegionKeyList SubsystemDutyDlg::GetRegions()
	{
		FUNCTION_ENTRY("GetRegions");

		RegionKeyList regionKeys =subsystemDutyMatrix_.getColumnIds();

		RegionKeyList regions; 

		for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
		{
			std::string name = dataCache_.getRegionName(*regionIt);

			if(name.compare("All Locations")  == 0)  continue;
				
			regions.push_back(*regionIt);
		}

		FUNCTION_EXIT;
		return regions;
	}

	SubsystemKeyList SubsystemDutyDlg::GetSelectedSubsystems()
	{
			FUNCTION_ENTRY("GetSelectedSubsystems");

			SubsystemKeyList subsystems;
			int nCount = clbSubystem_.GetCount();
			for(int i = 0; i < nCount ; i ++)
			{
				if(1== clbSubystem_.GetCheck(i) )
				{
					subsystems.push_back(clbSubystem_.GetItemData(i));
				}
			}

			FUNCTION_EXIT;
			return subsystems;
	}

	void SubsystemDutyDlg::OnActionBtn()
	{
		FUNCTION_ENTRY("OnActionBtn");

		//get selected regions
		RegionKeyList regions = GetSelectedRegions();

		//get selected subsystem
		SubsystemKeyList subsystems = GetSelectedSubsystems();

		TA_Base_Bus::TransActiveMessage userMsg;
		const std::string errorMsgSub = "请选择一个子系统！";
		const std::string errorMsgReg = "请选择一个车站！";
		//validate the inputs  at least one region and one subsystem are selected
		if( 0 == subsystems.size())
		{
			userMsg<<errorMsgSub;
			userMsg.showMsgBox(IDS_UW_050018, "权限管理器");

			FUNCTION_EXIT;
			return;
		}
		if(0 == regions.size())
		{
			userMsg<<errorMsgReg;
			userMsg.showMsgBox(IDS_UW_050018, "权限管理器");

			FUNCTION_EXIT;
			return;
		}
		//do the delegation or revoke action
		try
		{
			CWaitCursor waitCursor;

			// Request subsystem duty transfer. If this succeeds close the dialog and return 
			// to the main Duty Manager window.
			//m_dutyAgent.transferSubsystemDuty(m_sessionCache.getCurrentSessionId(), selectedSubsystems);
			if(bRevoke_)
			{
				//revoke
				SubsystemTreeMap selectedSubsystems;

				for(RegionKeyList::const_iterator regionItr= regions.begin(); regionItr< regions.end(); regionItr++)
				{
						selectedSubsystems.insert(	std::make_pair( (*regionItr), subsystems));
				}

				//dutyAgent_.transferSubsystemDuty(sessionCache_.getCurrentSessionId(),selectedSubsystems );
				dutyAgent_.transferSubsystemDuty(sessionCache_.getSessionInfo(sessionCache_.getCurrentSessionId()),selectedSubsystems );
			}
			else
			{
				//delegation, similar with degrade
				dutyAgent_.degradeSubsystemsAtRegions(sessionCache_.getCurrentSessionId(),subsystems, regions);	
			}
			EndDialog(IDOK);
		}
		catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
		{
			//MessageBox::error("Error while delegating subsystem duty.", ex);

			//TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg<<ex.what();
			//userMsg<<ex.getDetails();
			//userMsg.showMsgBox(IDS_UW_670005, "权限管理器");

			MessageBox::error("",ex, IDS_UW_670005);
		}

		FUNCTION_EXIT;
	}

	void SubsystemDutyDlg::OnAllBtn()
	{
		FUNCTION_ENTRY("OnAllBtn");

		//get selected subsystem
		SubsystemKeyList subsystems = GetSelectedSubsystems();

		//validate input, at least on subsystem should be selected
		if(0 == subsystems.size())
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			const std::string errorMsg = "请选择一个子系统！";
			userMsg<<errorMsg;
			userMsg.showMsgBox(IDS_UW_050018, "权限管理器");

			FUNCTION_EXIT;
			return;
		}
		//do whole-line delegation or revoking action
		RegionKeyList regions ;
		regions.push_back(ALL_LOCATION);
		try
		{
			CWaitCursor waitCursor;

			// Request subsystem duty transfer. If this succeeds close the dialog and return 
			// to the main Duty Manager window.
			//m_dutyAgent.transferSubsystemDuty(m_sessionCache.getCurrentSessionId(), selectedSubsystems);
			if(this->bRevoke_)
			{
			   //revoke all
				SubsystemTreeMap selectedSubsystems;
				
				RegionKeyList regionKeys = GetRegions();

				for(RegionKeyList::const_iterator itr =regionKeys.begin(), end = regionKeys.end(); itr != end; ++ itr )
				{
					selectedSubsystems.insert(
						std::make_pair( *itr, subsystems));
				}

				//selectedSubsystems.insert(
				//		std::make_pair( ALL_LOCATION, subsystems));

				//dutyAgent_.transferSubsystemDuty(sessionCache_.getCurrentSessionId(),selectedSubsystems);
				dutyAgent_.transferSubsystemDuty(sessionCache_.getSessionInfo(sessionCache_.getCurrentSessionId()),selectedSubsystems);
			}
			else
			{
				//delegation all
				dutyAgent_.degradeSubsystemsAtRegions(sessionCache_.getCurrentSessionId(),subsystems, regions);
			}
			EndDialog(IDOK);
		}
		catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
		{
			//MessageBox::error("Error while delegating all subsystem duty.", ex);

			//TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg<<ex.what();
			//userMsg<<ex.getDetails();
			//userMsg.showMsgBox(IDS_UW_670005, "权限管理器");
			
			MessageBox::error("",ex, IDS_UW_670005);
		}
		FUNCTION_EXIT;
	}

	void SubsystemDutyDlg::OnShowAllBtn()
	{
		FUNCTION_ENTRY("OnShowAllBtn");
		if (showAllSubsystemsChkBox_.GetCheck() == BST_CHECKED)
		{
			this->ShowAllSubsystemsList(true);
		}
		else
		{
			this->ShowAllSubsystemsList(false);
		}
		FUNCTION_EXIT;
	}

	BEGIN_MESSAGE_MAP(SubsystemDutyDlg, CDialog)
		ON_BN_CLICKED(IDC_ACTION_BTN, OnActionBtn)
		ON_BN_CLICKED(IDC_ALL_BTN,OnAllBtn)
		ON_BN_CLICKED(IDC_CHECKBOX_SHOW_ALL_SUBSYSTEMS,OnShowAllBtn)
		ON_WM_SIZE()
	END_MESSAGE_MAP()


	// SubsystemDutyDlg message handlers

} //end of namespace TA_Base_App

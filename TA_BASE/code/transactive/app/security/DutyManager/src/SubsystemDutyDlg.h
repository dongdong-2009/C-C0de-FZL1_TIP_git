
#if _MSC_VER > 1000
#pragma once
#endif
using namespace TA_Base_Bus;

namespace TA_Base_App
{

	// SubsystemDutyDlg dialog

	class SubsystemDutyDlg : public CDialog
	{
		DECLARE_DYNAMIC(SubsystemDutyDlg)

	public:
		SubsystemDutyDlg(CWnd* pParent = NULL, bool bRevoke = false);   // standard constructor
		virtual ~SubsystemDutyDlg();

		// Dialog Data
		enum { IDD = IDD_SUBSYSTEM_DUTY };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    virtual BOOL OnInitDialog();
		afx_msg void OnActionBtn();
		afx_msg void OnAllBtn();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnShowAllBtn();
		DECLARE_MESSAGE_MAP()

	private:
		void InitControlTexts();
		void ShowAllSubsystemsList(bool _isShowAllSubsystems);

		RegionKeyList										GetSelectedRegions();
		SubsystemKeyList								GetSelectedSubsystems();		
		SubsystemKeyList								GetTransferableSubsystems();
		RegionKeyList										GetRegions();
	private:
		// //if revoke the rights
		bool						bRevoke_;
		CButton					allBtn_;
		CButton					actionBtn_;		
		CListCtrl				regionsLv_;
		CCheckListBox	clbSubystem_;
		CButton					showAllSubsystemsChkBox_;

		TA_Base_Bus::DutyAgentAccess				dutyAgent_;
		TA_Base_Bus::SessionInfoEx						currentSessionInfo_;
		TA_Base_Bus::SubsystemDutyMatrix&		subsystemDutyMatrix_;
		TA_Base_Bus::DataCache&							dataCache_;
		 TA_Base_Bus::SessionCache&					sessionCache_;
	};

} //end of namespace TA_Base_App

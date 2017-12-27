#if !defined(AFX_SUBSYSTEMDUTYPROPERTYPAGE_H__2DD28A53_0AA4_4153_9C2F_8004F3884A5B__INCLUDED_)
#define AFX_SUBSYSTEMDUTYPROPERTYPAGE_H__2DD28A53_0AA4_4153_9C2F_8004F3884A5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/SubsystemDutyPropertyPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Subsystems" tab. It displays the active 
  * subsystem duty state for every physical subsystem in every region.
  *
  */

//TD15533++
namespace TA_Base_App
{
	typedef struct 
	{
		int baseImage;
		std::string baseText;
		int overrideImage;
		std::string overrideText;
		bool isOverrided;
	} CellData;

	typedef struct 
	{
		TA_Base_Bus::ProfileKey profileKey;
		TA_Base_Bus::SubsystemKey subsystemKey;
		TA_Base_Bus::ESubsystemState subsystemState;
		TA_Base_Bus::ActionGroupKey actionGroupKey;
	} PSSA;

	typedef std::vector<PSSA> ProfilesWithDuties;
}

#include "bus/mfc_extensions/src/grid_control/GridCtrlS.h"
//++TD15533

namespace TA_Base_App
{
    class SubsystemDutyPropertyPage : public CPropertyPage,
                                      public TA_Base_Bus::ISessionObserver,
                                      public TA_Base_Bus::ISubsystemDutyObserver
    {
	    DECLARE_DYNCREATE(SubsystemDutyPropertyPage)

    // Operations
    public:
	    SubsystemDutyPropertyPage();
	    virtual ~SubsystemDutyPropertyPage();

        // ISessionObserver interface
        virtual void sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate);

        // ISubsystemDutyObserver interface
        virtual void dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate);

        //TD17082++ made public to allow the client handle the initialization call
		void initialiseSubsystemDutyGrid();
    protected:
        
        void enableDutyControlButtons();
        void addSubsystem(
			TA_Base_Bus::SubsystemKey subsystemKey, 
			TA_Base_Bus::ProfileKeyList profileKeys,
			TA_Base_Bus::RegionKeyList regionKeys); //TD15533
        void removeSubsystem(TA_Base_Bus::SubsystemKey subsystemKey);
        void updateSubsystems(TA_Base_Bus::SubsystemKeyList oldSubsystemKeys, TA_Base_Bus::SubsystemKeyList newSubsystemKeys, TA_Base_Bus::ProfileKeyList profileKeys); //TD15533

    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(SubsystemDutyPropertyPage)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(SubsystemDutyPropertyPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT OnReceiveSessionUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnReceiveSubsystemDutyUpdate(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnShowAllSubsystems();
	    afx_msg void OnTransferSubsystemDutyBtn2();
	    afx_msg void OnTransferAllSubsystemDutyBtn();
		afx_msg void OnRevokeSubsystemDutyBtn();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

	protected:
		//place holder for the old implementation, never called by other place
		 void OnTransferSubsystemDutyBtn();	     
    // Dialog Data
    private:
	    //{{AFX_DATA(SubsystemDutyPropertyPage)
	    enum { IDD = IDD_SUBSYSTEMS_DLG };
	    CButton	m_showAllSubsystemsBtn;
	    CButton	m_transferSubsystemDutyBtn;
	    CButton	m_transferAllSubsystemDutyBtn;
		CButton m_revokeSubsystemDutyBtn;
	    //}}AFX_DATA

        /**
          * convertColourStrToColorref
          *
          * Converts a comma delimited set of RGB values into a COLORREF.
          *
          * @param colourStr The colour string.
          * @param defaultColour The colour to use if a problem was found during the conversion.
          *
          * @return COLORREF to the converted colour.
          */
        COLORREF convertColourStrToColorref( const std::string& colourStr, const COLORREF& defaultColour ) const;

        /**
          * determineCellBkColour
          *
          * Determines the colour to shade the cell represented by the matrix element.
          *
          * @param inRegion Specifies whether the subsystem is in the region.
          * @param state Specifies the current state of the subsystem.
          *
          * @return COLORREF The colour to shade the cell.
          */
        COLORREF determineCellBkColour( const bool inRegion, const TA_Base_Bus::ESubsystemState state ) const;

        //CGridCtrl m_subsystemDutyGrid; //TD15533
		CGridCtrlS m_subsystemDutyGrid; //TD15533

        TA_Base_Bus::DutyAgentAccess m_dutyAgent;
        TA_Base_Bus::DutyRights m_dutyRights;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
        TA_Base_Bus::SubsystemDutyMatrix& m_subsystemDutyMatrix;
        TA_Base_Bus::SessionInfoEx m_currentSessionInfo;

        COLORREF m_normalDutyColour;
        COLORREF m_delegatedDutyColour;
        COLORREF m_degradedDutyColour;
        COLORREF m_unallocatedSubsystemColour;

		//TD15533++
		std::vector<TA_Base_App::PSSA> m_actionGroupsDB;

		TA_Base_Bus::ActionGroupKey getActionGroupDB(
			const TA_Base_Bus::ProfileKey profileKey,
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::ESubsystemState subsystemState);

		TA_Base_App::CellData generateCellData(
			const TA_Base_Bus::SubsystemKey subsystemKey, 
			const TA_Base_Bus::RegionKey regionKey,
			const TA_Base_Bus::ESubsystemState subsystemState);

		TA_Base_App::ProfilesWithDuties getProfilesWithDuties(
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::RegionKey regionKey);

		std::string getActionGroupNameWithErrorChecking(
			const TA_Base_Bus::ActionGroupKey actionGroupKey);

		bool isSubsystemAccessible(
			const TA_Base_Bus::ProfileKeyList profileKeys,
			const TA_Base_Bus::SubsystemKey subsystemKey);

		TA_Base_Bus::ProfileKeyList getProfileKeyList(
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::RegionKey regionKey);

		TA_Base_Bus::RegionKeyList getLocations();

		bool isRegionAccessible(
			const TA_Base_Bus::RegionKey regionKey,
			const TA_Base_Bus::RegionKeyList regionKeys);
		//++TD15533

		// Yanrong++ Improve on launching speed.
		std::string getActionGroupNameWithAGK(unsigned long actionGroupKey);

		std::vector<TA_Base_Core::IActionGroup*> m_actionGroups;
		// ++ Yanrong
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SUBSYSTEMDUTYPROPERTYPAGE_H__2DD28A53_0AA4_4153_9C2F_8004F3884A5B__INCLUDED_)

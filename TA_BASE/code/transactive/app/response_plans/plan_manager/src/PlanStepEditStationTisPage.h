/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditStationTisPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of Station TIS step parameters.
  *
  */

#if !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)
#define AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include "PlanStepEditParameterPage.h"
#include "TisPidListStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "PidsStepParameter.h"

namespace TA_Base_App
{
    class StationTisStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditStationTisPage dialog

    class CPlanStepEditStationTisPage : public CPlanStepEditParameterPage
    {
    // Construction/destruction
    public:
        CPlanStepEditStationTisPage(StationTisStep* step);
        virtual ~CPlanStepEditStationTisPage();

    // Operations
    public:
        //virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        CPlanStepEditStationTisPage(const CPlanStepEditStationTisPage&);
        CPlanStepEditStationTisPage& operator=(const CPlanStepEditStationTisPage&);

        void UpdatePidList();
        void UpdatePriorityCombo();
        void SetListEnableState(CListCtrl& list, BOOL enabled);
        void SetButtonState(CButton& button, int checkState);
        void SetComboSelection(CComboBox& combo, int item);
        void SetStationLibraryVersion();


    // Attributes
    private:
        unsigned short m_stationLibraryVersion;
		PidsStepParameter		m_pidsStepParameter;
		TisPidListStepParameter m_pidListParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditStationTisPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_STATION_TIS_DLG };
        CListCtrl    m_pidList;        
        CEdit    m_messageEdit;       
        CButton    m_pickPidBtn;   
		//select msg fullscreen
        CButton m_msgFullscreenBtn;
		//select msg roll
		CButton m_msgRollBtn;
		//msg is emergency msg
		CButton m_msgEmergencyBtn;
		
        // }}AFX_DATA

		bool m_msgType;
		bool m_showType;

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditStationTisPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditStationTisPage)
        virtual BOOL OnInitDialog();      
        afx_msg void OnPickPidButton();
		afx_msg void OnFullscreenBtn();
		afx_msg void OnRollBtn();
		afx_msg void OnPickEmergencyBtn();
       
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		void SetShowType(bool showType);
		void SetMsgType(bool msgType);
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)

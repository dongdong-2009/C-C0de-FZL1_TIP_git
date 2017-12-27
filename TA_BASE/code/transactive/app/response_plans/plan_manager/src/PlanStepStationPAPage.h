//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepStationPAPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_)
#define AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <map>

#include "PlanStepEditParameterPage.h"
#include "StationPAStep.h"

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStationPALogPage dialog

    class CPlanStepStationPAPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepStationPAPage)

    // Construction
    public:
        CPlanStepStationPAPage(StationPAStep* step);
        ~CPlanStepStationPAPage();


    // Operations
    protected:
       virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


     private:
         void PopulateStationsCombo ();
    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStationPALogPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStationPALogPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnStationBtn();
        afx_msg void OnStationChange();
        afx_msg void OnZonesBtn();
        afx_msg void OnMessagesBtn();
        afx_msg void OnCyclicBtn();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    private:
        CUIntArray m_zoneIds;
        TA_Base_Core::PAMessage m_paMessage;
        // {{AFX_DATA(CPlanStationPALogPage)
        enum { IDD = IDD_PLAN_STEP_PA_STATION_DLG };
        CDateTimeCtrl m_durationTime;
        CSpinButtonCtrl m_intervalSpin;
        CSpinButtonCtrl m_cylicTimesSpin;
        CComboBox m_stationCombo;
        CString m_stationName;
        CButton m_stationBtn;
        CButton m_cyclicBtn;
        CButton m_TISSynchBtn;
        CButton m_chimeBtn;
        CListBox m_stationZonesLst;
        CEdit m_messageNameTxt;
        CEdit m_intervalTxt;
        CEdit m_cylicTimesTxt;
        UINT m_duration;
        UINT m_interval;
        BOOL m_chime;
        BOOL m_cyclic;
        BOOL m_TISSynch;
        int m_station;
        std::map<unsigned long, std::string> m_mapLocationKeyToName;
        // }}AFX_DATA
    };

} // end namespace

#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PAStationZones.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
   PAStationZones.h : header file
   to select zones for PA Station plan step
*/

#if !defined(AFX_PASTATIONZONES_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_)
#define AFX_PASTATIONZONES_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <map>
#include <string>

#include "StationStepParameter.h"
#include "PaZonesStepParameter.h"
#include "PlanStepParameterEditDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPAStationZones dialog

namespace TA_Base_App
{
    class CPAStationZones : public CPlanStepParameterEditDlg
    {
        // Construction
    public:
        CPAStationZones(const std::string& stationName, unsigned long stationLocationKey, bool bAllLocation, bool canEdit, PaZonesStepParameter& parameter, const std::map<unsigned long, std::string>& mapLocations);
        virtual ~CPAStationZones ();
        CStringArray*    GetSelectedZones () { return m_selectedZones; }
        CUIntArray* CPAStationZones::GetZoneIDs ();

        // Dialog Data
        // {{AFX_DATA(CPAStationZones)
        enum { IDD = IDD_PA_STATION_ZONES_DLG };
        CEdit    m_stationNameTxt;
        CButton    m_unselectBtn;
        CButton    m_unselectAllBtn;
        CButton    m_selectBtn;
        CButton    m_selAllBtn;
        CListBox    m_selectedZonesLst;
        CListBox    m_availableZonesLst;
        CComboBox m_zoneGroupsCombo;
        CString    m_stationNameStr;
    // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPAStationZones)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPAStationZones)
        virtual BOOL OnInitDialog();
        virtual void OnCancel();
        virtual void OnOK();
        afx_msg void OnSelectAllBtn();
        afx_msg void OnSelectBtn();
        afx_msg void OnUnselectAllBtn();
        afx_msg void OnUnselectBtn();
        afx_msg void OnZoneGroupSelChange ();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateZonesList ();
        VOID PopulateZoneGroups ();
        CStringArray* m_selectedZones;
        CUIntArray* m_zoneIDs;
        const std::map<unsigned long, std::string>& m_mapLocationKeyToName;
        unsigned long m_locationKey;
        std::string m_stationName;
        bool m_bAllLocation;
    };
}//namespace

#endif //defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PASTATIONZONES_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_)

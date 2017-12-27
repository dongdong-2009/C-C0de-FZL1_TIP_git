/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTisPidListEditDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog for selecting a set of PIDs for the Station TIS step.
  *
  **/

#if !defined(AFX_PLANSTEPPARAMETERTISPIDLISTEDITDLG_H__526C1EAE_73E9_432B_8515_B9B6AF8FFD4E__INCLUDED_)
#define AFX_PLANSTEPPARAMETERTISPIDLISTEDITDLG_H__526C1EAE_73E9_432B_8515_B9B6AF8FFD4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include <vector>
#include <string>
#include "PlanStepParameterEditDlg.h"


namespace TA_Base_App
{
    class TisPidListStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterTisPidListEditDlg dialog

    class CPlanStepParameterTisPidListEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction/Destruction
    public:
        CPlanStepParameterTisPidListEditDlg(const std::string& stationName, TisPidListStepParameter& parameter, bool canEdit);
        virtual ~CPlanStepParameterTisPidListEditDlg();

    public:
        std::vector<std::string> GetSelectedPids() const  { return m_selectedPids; }

    private:
        CPlanStepParameterTisPidListEditDlg(const CPlanStepParameterTisPidListEditDlg&);
        CPlanStepParameterTisPidListEditDlg& operator=(const CPlanStepParameterTisPidListEditDlg&);

        std::vector<int> GetSelectedListItems(const CListBox& list) const;
        void TransferSelectedListItems(CListBox& sourceList, CListBox& destinationList);
        void TransferAllListItems(CListBox& sourceList, CListBox& destinationList);
        void DeselectAllListItems(CListBox& list);
        void SetPidAddRemoveButtonEnableState();

    // Attributes
    private:
        std::string m_stationName;
        TisPidListStepParameter& m_pidListParameter;
        std::vector<std::string> m_selectedPids;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepParameterTisPidListEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_TIS_PIDLIST_EDIT_DLG };
        CListBox    m_selectedPidList;
        CButton    m_removeSelectionBtn;
        CButton    m_removeAllBtn;
        CListBox    m_availablePidList;
        CButton    m_addSelectionBtn;
        CButton    m_addAllBtn;
        CString    m_parameterName;
        // }}AFX_DATA

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterTisPidListEditDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterTisPidListEditDlg)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        afx_msg void OnRemoveAllButton();
        afx_msg void OnRemoveSelectionButton();
        afx_msg void OnAddAllButton();
        afx_msg void OnAddSelectionButton();
        afx_msg void OnAvailablePidListSetFocus();
        afx_msg void OnSelectedPidListSetfocus();
        afx_msg void OnPidListSelectionChanged();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_PLANSTEPPARAMETERTISPIDLISTEDITDLG_H__526C1EAE_73E9_432B_8515_B9B6AF8FFD4E__INCLUDED_)

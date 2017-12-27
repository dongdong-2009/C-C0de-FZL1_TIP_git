/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Represents the plan step list shown on the Steps tab of plan node view.
  *
  **/

#if !defined(AFX_PLANSTEPLISTCTRL_H__6EBF2E0A_E467_4086_98F8_D1FEDFFE763F__INCLUDED_)
#define AFX_PLANSTEPLISTCTRL_H__6EBF2E0A_E467_4086_98F8_D1FEDFFE763F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "PlanStepListHeaderCtrl.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class PlanStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepListCtrl window

    class CPlanStepListCtrl : public CEditableListCtrl
    {
    // Construction
    public:
        CPlanStepListCtrl(bool m_execStatusVisible = true);
        virtual ~CPlanStepListCtrl();

    // Operations
    public:
		BOOL EnableTips();//TD14921 liqiang++
        BOOL SetReadOnly(BOOL readOnly = TRUE);
        void AddStep(const TA_Base_Core::StepDetail &stepDetail, PlanStep *planStep);
        PlanStep *GetStep(int item);
        PlanStep *GetSelectedStep();
        void SetSelectedStep(PlanStep *planStep, bool scrollIntoView = false);
        void SetSelectedStep(PlanStepNumber stepNumber, bool scrollIntoView = false);
        void SetSelectedSteps(PlanStepNumber stepNumber, unsigned long stepCount, bool scrollIntoView = false);
        void SetPlanExecutionUpdateId(TA_Base_Core::UpdateId updateId);
        void SetStepCustomisationUpdateId(TA_Base_Core::UpdateId updateId);
        void ResetPlanExecutionProgress();
        void ResetCustomStepSettings();
        void UpdatePlanExecutionProgress(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        void UpdatePlanExecutionProgress(const TA_Base_Core::ActiveStepDetail &activeStepDetail, bool scrollIntoView = false);
        void UpdateCustomStepSettings(const TA_Base_Core::PlanCustomisationUpdate &planCustomUpdate);
        void UpdateCustomStepSettings(const TA_Base_Core::StepCustomisableDetail &stepCustomDetail);

    protected:
        int FindStep(PlanStepNumber stepNumber); // Returns -1 if step is not found
        EPlanStepImageType GetStepImage(int item);
        void SetStepImage(int item, EPlanStepImageType stepImage);
        void SetStepImageAndSelection(int item, EPlanStepImageType stepImage, bool selected);
		bool enableNoWaitCheckButton(PlanStep *planStep, bool fIsSkipped);

    // Attributes
    public:
        typedef enum
                {
                    SLC_STEP_EXEC_STATE = 0,
                    SLC_STEP_POSITION,
                    SLC_STEP_NAME,
                    SLC_STEP_TYPE,
                    SLC_STEP_DESCRIPTION,
                    SLC_STEP_SKIP,
                    SLC_STEP_IGNORE_FAILURE,
                    SLC_STEP_DELAY,
					SLC_STEP_NOWAIT
                }
                EStepListColumn;

    private:
        CPlanStepListHeaderCtrl m_headerCtrl;
        bool m_execStatusVisible;
        TA_Base_Core::UpdateId m_planExecutionUpdateId;
        TA_Base_Core::UpdateId m_stepCustomisationUpdateId;

		//TD14921 liqiang++
		CToolTipCtrl	m_toolTip;
		int				m_nItem;		
		int				m_nSubItem;
		BOOL			m_bEnableTips;
		//TD14921 ++liqiang

    // Overrides
	protected:
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CPlanStepListCtrl)
        virtual void PreSubclassWindow();
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
		virtual BOOL PreTranslateMessage(MSG* pMsg);//TD14921 liqiang++
    	//}}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        //{{AFX_MSG(CPlanStepListCtrl)
	    afx_msg void OnItemCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg BOOL OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    	afx_msg BOOL OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg BOOL OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnTimer(UINT nIDEvent);
	    afx_msg void OnDestroy();
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);//TD14921 liqiang++
    	//}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPLISTCTRL_H__6EBF2E0A_E467_4086_98F8_D1FEDFFE763F__INCLUDED_)

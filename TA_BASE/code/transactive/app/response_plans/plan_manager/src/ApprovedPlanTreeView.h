//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_)
#define AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeView.h"

#include "core/threads/src/Thread.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"

namespace TA_Base_App
{
    class CApprovedPlanDoc;


	// CL16116++
    /////////////////////////////////////////////////////////////////////////////
    // ControlPlanThread, create thread to handle control Plan dialog
	class ControlPlanThread : public TA_Base_Core::Thread
	{
	public:
		ControlPlanThread(PlanNode* pNode, LocationKey location, PlanInstanceNumber instanceId = -1, 
			const std::string& planName = "", bool exec=false)
			:m_pPlanNode(pNode),
			 m_Location(location),
			 m_InstanceId(instanceId),
			 m_bStop(true),
			 m_planName(planName),
             m_execPlan(exec)
		{
		}
		void run();
		void terminate()
		{
			m_bStop = true;
		}
		virtual ~ControlPlanThread()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[+++ly+++] ControlPlanThread terminate");
		}
	private:
		ControlPlanThread(){}

		PlanNode*			m_pPlanNode;	// PlanNode pointer
		LocationKey			m_Location;		// Plan Manager Location
		PlanInstanceNumber	m_InstanceId;	// Plan Instance Id
		bool				m_bStop;
		std::string			m_planName;	// for audit message use
        bool                m_execPlan;
	};
	// ++CL16116

    /////////////////////////////////////////////////////////////////////////////
    // CApprovedPlanTreeView view
    class CApprovedPlanTreeView : public CPlanTreeView
    {
    protected:
        CApprovedPlanTreeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CApprovedPlanTreeView)

    // Operations
    public:
        CApprovedPlanDoc* GetDocument();

    protected:
        virtual void ProcessRparamExecPlanIdUpdate();
        virtual void ProcessRparamViewPlanIdUpdate();
        virtual void ProcessRparamAssocAlarmIdUpdate();
        virtual void ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate);
        virtual void ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CApprovedPlanTreeView)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CApprovedPlanTreeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CApprovedPlanTreeView)
        afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

	private:
		std::vector<ControlPlanThread*>		m_vecControlPlan;
    };

    #ifndef _DEBUG  // debug version in ApprovedPlanTreeView.cpp
    inline CApprovedPlanDoc* CApprovedPlanTreeView::GetDocument()
        {
        FUNCTION_ENTRY( "GetDocument" );

        return (CApprovedPlanDoc*)CPlanTreeView::GetDocument(); }


    #endif

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_APPROVEDPLANTREEVIEW_H__B1CC033E_3FD2_4BDD_A60C_98705FA5B9DE__INCLUDED_)

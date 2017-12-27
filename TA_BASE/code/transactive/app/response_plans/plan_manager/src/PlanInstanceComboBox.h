//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanInstanceComboBox.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANINSTANCECOMBOBOX_H__0CFA9854_1D87_4531_862F_AF94DB27FDB9__INCLUDED_)
#define AFX_PLANINSTANCECOMBOBOX_H__0CFA9854_1D87_4531_862F_AF94DB27FDB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class PlanInstance;
    class PlanInstanceFilter;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanInstanceComboBox window

    class CPlanInstanceComboBox : public CComboBox
    {
    // Construction
    public:
        CPlanInstanceComboBox();
        virtual ~CPlanInstanceComboBox();

    // Operations
    public:
        virtual void SetPlanInstanceFilter(PlanInstanceFilter *planInstanceFilter);
        virtual bool IsNewInstance(const PlanInstanceId& planInstance);
        virtual void AddInstance(const int& nActivePlanState, const TA_Base_Core::NodeId& planId, const PlanInstanceId& instanceId);
        virtual void RemoveInstance(const PlanInstanceId &planInstanceId);
        virtual void RepositionInstance(const PlanInstanceSharedPtr& planInstance);
        virtual void SetSelectedInstance(PlanInstanceSharedPtr planInstance);
        virtual PlanInstanceId& GetSelectedInstance();
        virtual void Purge();

    protected:
        virtual CString GetPlanInstanceLabel(const PlanInstanceId &planInstanceId);
        virtual void SelectItem(int item);


    // Attributes
    private:
        TA_Base_Core::NodeId m_currentPlanNodeId;
        PlanInstanceId m_idCurSelection;
        PlanInstanceFilter *m_planInstanceFilter;

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanInstanceComboBox)
        // }}AFX_VIRTUAL

    // Implementation
        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanInstanceComboBox)
        afx_msg BOOL OnItemSelected();
        afx_msg void OnDestroy( );
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANINSTANCECOMBOBOX_H__0CFA9854_1D87_4531_862F_AF94DB27FDB9__INCLUDED_)

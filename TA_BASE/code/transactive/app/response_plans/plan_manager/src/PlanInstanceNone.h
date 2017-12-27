//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanInstanceNone.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANINSTANCENONE_H__AFAF17B8_7A59_43D5_8C03_777CB94DA94E__INCLUDED_)
#define AFX_PLANINSTANCENONE_H__AFAF17B8_7A59_43D5_8C03_777CB94DA94E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanInstance.h"


namespace TA_Base_App
{
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // PlanInstanceNone

    class PlanInstanceNone : public PlanInstance
    {
    // Operations
    public:
        PlanInstanceNone();
        virtual ~PlanInstanceNone();

        virtual bool operator<(const PlanInstance &other) const; // used for instance display sort order
        virtual PlanInstanceId getInstanceId() const;
        virtual bool customisationEnabled() const;
        virtual bool isLocalInstance() const;
        virtual bool ownedByCurrentSession() const;

        // Enable actions that can be performed on this instance
        virtual void updateCmdUI(CCmdUI &cmdUI, PlanNode &planNode);

        // Window controls modifiers
        virtual void addToPlanInstanceList(CPlanInstanceListCtrl &listCtrl);
        virtual void setSinglePlanInstanceList(CSinglePlanInstanceListCtrl &instanceListCtrl);

    protected:
        virtual TA_Base_Core::ExecutionHistory *getExecutionHistory();
        virtual TA_Base_Core::CustomisationHistory *getCustomisationHistory();

    private:
        PlanInstanceNone(const PlanInstanceNone &other);
        PlanInstanceNone &operator=(const PlanInstanceNone &other);
    };
}

#endif // !defined(AFX_PLANINSTANCENONE_H__AFAF17B8_7A59_43D5_8C03_777CB94DA94E__INCLUDED_)

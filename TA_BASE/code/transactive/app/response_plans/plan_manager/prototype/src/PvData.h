/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PvData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PvData.h : header file
//

#if !defined(AFX_PVDATA_H__E39DC7E4_A802_11D5_B63F_8394FA78D92B__INCLUDED_)
#define AFX_PVDATA_H__E39DC7E4_A802_11D5_B63F_8394FA78D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PvCommonDefs.h"


class CPvData
{
public:
    static CPvData &instance();
    ~CPvData();

    void getAllCategories(PV::CategoryList &categories);
    void getAllPlans(PV::PlanList &plans);
    void getApprovedPlans(PV::PlanList &plans);
    void getUnapprovedPlans(PV::PlanList &plans);
    void getActivePlans(PV::PlanList &plans);
    void getPlansForCategory(PV::PlanList &plans, unsigned long categoryPKey);
    void getStepsForPlan(PV::StepList &steps, unsigned long planPKey);

    PV::Category *getCategory(unsigned long categoryPKey);
    PV::Plan *getPlan(unsigned long planPKey);
    PV::Step *getStep(unsigned long stepPKey);

private:
    CPvData();
    CPvData(const CPvData &pvData);
    void createDummySteps();
    void createDummyActiveInstances();

private:
    PV::CategoryList m_categories;
    PV::PlanList m_plans;
    PV::StepList m_steps;
};

#endif // AFX_PVDATA_H__E39DC7E4_A802_11D5_B63F_8394FA78D92B__INCLUDED_
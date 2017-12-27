/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/DatabasePlanAccess.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// DatabasePlanAccess.h : header file
//

#if !defined(AFX_DATABASEPLANACCESS_H__CFB7B020_5E81_4D7A_8EFA_3D7A7CCFEDB3__INCLUDED_)
#define AFX_DATABASEPLANACCESS_H__CFB7B020_5E81_4D7A_8EFA_3D7A7CCFEDB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "bus\response_plans\plan_tree\src\IPlanAccess.h"
#include "bus\response_plans\plan_tree\src\PlanFilter.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // DatabasePlanAccess

    class DatabasePlanAccess : public IPlanAccess
    {
    // Operations
    public:
	    DatabasePlanAccess(PlanFilter::PlanFilterType planFilterType);
	    DatabasePlanAccess(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans);
	    virtual ~DatabasePlanAccess();

        // IPlanAccess interface
        virtual IPlanAccess::PlanNodeList getRootChildren();
        virtual IPlanAccess::PlanNodeList getChildren(TA_Base_Core::NodeId categoryId);
        virtual IPlanAccess::PlanIdList getNodeIdsInPlanPath(TA_Base_Core::NodeId planId);
        virtual std::string getPlanPath(TA_Base_Core::NodeId planId);
        virtual TA_Base_Core::NodeId getPlanId(const std::string& planPath);

    private:
        DatabasePlanAccess(const DatabasePlanAccess& planAccess);
        DatabasePlanAccess& operator=(const DatabasePlanAccess& planAccess);


    // Attributes
    private:
        PlanFilter m_planFilter;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_DATABASEPLANACCESS_H__CFB7B020_5E81_4D7A_8EFA_3D7A7CCFEDB3__INCLUDED_)

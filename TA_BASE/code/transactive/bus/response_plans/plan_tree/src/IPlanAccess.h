/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/IPlanAccess.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// IPlanAccess.h : header file
//

#if !defined(AFX_IPLANACCESS_H__060D6044_CEE0_447C_A32D_19986AF01C18__INCLUDED_)
#define AFX_IPLANACCESS_H__060D6044_CEE0_447C_A32D_19986AF01C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // IPlanAccess Interface

    class IPlanAccess
    {
    public:
        typedef std::vector<TA_Base_Core::NodeSummary> PlanNodeList;
        typedef std::vector<TA_Base_Core::NodeId> PlanIdList;
        
        virtual IPlanAccess::PlanNodeList getRootChildren() = 0;
        virtual IPlanAccess::PlanNodeList getChildren(TA_Base_Core::NodeId categoryId) = 0;
        virtual IPlanAccess::PlanIdList getNodeIdsInPlanPath(TA_Base_Core::NodeId planId) = 0;
        virtual std::string getPlanPath(TA_Base_Core::NodeId planId) = 0;
        virtual TA_Base_Core::NodeId getPlanId(const std::string& planPath) = 0;

        virtual ~IPlanAccess() {}
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_IPLANACCESS_H__060D6044_CEE0_447C_A32D_19986AF01C18__INCLUDED_)

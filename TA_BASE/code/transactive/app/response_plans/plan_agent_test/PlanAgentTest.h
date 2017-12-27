/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent_test/PlanAgentTest.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#pragma warning(disable: 4018 4146 4786)

#include <time.h>
#include <iostream>
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"

namespace PaIf = TA_Base_Bus::IPlanAgentCorbaDef;
namespace PaData = TA_Base_Core;
namespace ENode = PaData::ENode;
namespace EApprovalState = PaData::EApprovalState;
namespace EExecutionMode = PaData::EExecutionMode;


class PlanAgentTest
{
public:
	PlanAgentTest();
	~PlanAgentTest();

	

	void getRootNodeChildren( void );
	void getNodeDetails( PaData::NodeId node );
	void getStepNames( void );
	void getParameterNames( PaData::EStep stepType );
	void getAllParameterNames( void );
	void getAllStepDetails( PaData::NodeId plan );
	void getSpecifiedStepDetails( PaData::NodeId plan, const PaData::StepIds& steps );
	void getStepParameters( PaData::NodeId plan );
	void getNodePath( PaData::NodeId plan );
	TA_Base_Core::NodeIds getNodeIdsInNodePath ( TA_Base_Core::NodeId category );
	void getActivePlanDetails(PaData::NodeId plan);
	void getAllActivePlanDetails();

	void runPlan( const  char * sessionId,PaData::ActivePlanId& plan );
	void customisePlan( const  char * sessionId,TA_Base_Core::ActivePlanId& plan );
	void stopPlan( const  char * sessionId,const PaData::ActivePlanId& plan );
	void pausePlan( const  char * sessionId,const PaData::ActivePlanId& plan );
	void resumePlan( const  char * sessionId,const PaData::ActivePlanId& plan );

	void insertPlan(const  char * sessionId,const TA_Base_Core::NodeId parent,EExecutionMode execMode);
	void movePlan(const TA_Base_Core::NodeId fromPlan, const TA_Base_Core::NodeId toParent);
	void copyPlan(const  char * sessionId,const TA_Base_Core::NodeId plan, const TA_Base_Core::NodeId parent,EExecutionMode execMode);
	void deletePlans(void);
	void deleteCategory(const TA_Base_Core::NodeId category);
	void updateCategory(const TA_Base_Core::NodeId parent);
	void insertCategory(const TA_Base_Core::NodeId parent);
	TA_Base_Core::StepId getNextStepId(void);
	void updatePlan(const TA_Base_Core::NodeId node);
	void getEditPlanLock( const std::string& session,TA_Base_Core::NodeId plan );
	void releaseEditPlanLock( const std::string& session,TA_Base_Core::NodeId plan );

	void customiseStep(const  char * sessionId, const TA_Base_Core::ActivePlanId& plan);

	void getPlanCustomHistory(const TA_Base_Core::ActivePlanId& plan);
	void getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan);
	void takeControl(const  char * sessionId, TA_Base_Core::ActivePlanId& plan);

private:
	TA_Base_Bus::PlanAgentLibrary m_planAgent;
};

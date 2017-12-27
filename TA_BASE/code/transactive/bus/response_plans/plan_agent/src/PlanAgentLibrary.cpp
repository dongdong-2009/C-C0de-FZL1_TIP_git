/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_agent/src/PlanAgentLibrary.cpp $
* @author:  Ripple
* @version: $Revision: #4 $
*
* Last modification: $DateTime: 2015/04/15 19:15:46 $
* Last modified by:  $Author: limin.zhu $
*
*/
#ifdef __WIN32__
#pragma warning(disable:4250)
#endif

#include <sstream>
#include "core/data_access_interface/entity_access/src/PlanAgentData.h"
#include "core/utilities/src/RunParams.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h"


namespace TA_Base_Bus
{
    PlanAgentLibrary::PlanAgentLibrary() :
m_planAgentLocationKey(0)
{
	TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "Location Key not set" );
	
	// Get the entity name based on the locationKey
	m_planAgentLocationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );
	
	// this will only return a single entry
	TA_Base_Core::CorbaNameList planAgentName = 
		TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
		TA_Base_Core::PlanAgentData::getStaticType(), m_planAgentLocationKey,true );
	
	// make sure we've got something
	if (planAgentName.size() == 0)
	{
		std::ostringstream msg;
		msg << "There are no entities of type PlanAgent with location key set to " << m_planAgentLocationKey << ".";
		
		TA_THROW(PlanAgentEntityNotFoundException(msg.str()));
	}
	
	// Set the agent name - this is the name of the agent to contact. 
	m_planAgent.setCorbaName( planAgentName[0] );
	
}

PlanAgentLibrary::PlanAgentLibrary(unsigned long locationKey) :
m_planAgentLocationKey(locationKey)
{
	
	// this will only return a single entry
	TA_Base_Core::CorbaNameList planAgentName = 
		TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
		TA_Base_Core::PlanAgentData::getStaticType(), m_planAgentLocationKey,true);
	
	// make sure we've got something
	if (planAgentName.size() == 0)
	{
		std::ostringstream msg;
		msg << "There are no entities of type PlanAgent with location key set to " << m_planAgentLocationKey << ".";
		
		TA_THROW(PlanAgentEntityNotFoundException(msg.str()));
	}
	
	// Set the agent name - this is the name of the agent to contact. 
	m_planAgent.setCorbaName( planAgentName[0] );
	
}

PlanAgentLibrary::~PlanAgentLibrary()
{
	
}

void PlanAgentLibrary::setPlanAgentTimeout(unsigned short timeout)
{
	m_planAgent.setObjectTimeout(timeout);
}

unsigned long PlanAgentLibrary::getPlanAgentLocationKey() const
{
	return m_planAgentLocationKey;
}

CORBA::Boolean PlanAgentLibrary::isRegisteredManager(const char* session, const char* pmId)
{
	   CORBA::Boolean bRet = false;
	   try{
		   CORBA_CALL_RETURN( bRet,
		                      m_planAgent,
		                      isRegisteredManager,
		                      ( session, pmId ) );
	   }
	   catch(...)
	   {
			LOG_EXCEPTION_CATCH(SourceInfo, " PlanAgentLibrary::isRegisteredManager","Unknown Exception caught during m_planAgent->isRegisteredManager()");
	   }
	   return bRet;
}

void PlanAgentLibrary::registerPlanManager(const char* session, const char* pmId, TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerRef)
{
	CORBA_CALL( m_planAgent,
	            registerPlanManager,
	            ( session, pmId, planManagerRef ) );
}


void PlanAgentLibrary::deregisterPlanManager(const char* session, const char* pmId)
{
	CORBA_CALL( m_planAgent,
	            deregisterPlanManager,
	            ( session, pmId ) );
}


CORBA::Boolean PlanAgentLibrary::isOwner(const char* session, const TA_Base_Core::ActivePlanId& plan)
{
	CORBA::Boolean bRet = false;
	CORBA_CALL_RETURN( bRet,
	                   m_planAgent,
	                   isOwner,
	                   ( session, plan ) );
	return bRet;
}


void PlanAgentLibrary::processDecisionStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status)
{
	CORBA_CALL( m_planAgent,
	            processDecisionStepResponse,
	            ( session, plan, status ) );
}


void PlanAgentLibrary::processPromptStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status)
{
	CORBA_CALL( m_planAgent,
	            processPromptStepResponse,
	            ( session, plan, status ) );
}


void PlanAgentLibrary::processLogStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* logMessage)
{
	CORBA_CALL( m_planAgent,
	            processLogStepResponse,
	            ( session, plan, status, logMessage ) );
}


void PlanAgentLibrary::processRunPlanStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error)
{
	CORBA_CALL( m_planAgent,
	            processRunPlanStepResult,
	            ( session, plan, status, error ) );
}


void PlanAgentLibrary::processLaunchGuiAppStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error)
{
	CORBA_CALL( m_planAgent,
	            processLaunchGuiAppStepResult,
	            ( session, plan, status, error ) );
}


void PlanAgentLibrary::processLaunchSchematicStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error)
{
	CORBA_CALL( m_planAgent,
	            processLaunchSchematicStepResult,
	            ( session, plan, status, error ) );
}



//lizettejl++ (DP-Changes)
void PlanAgentLibrary::processFailureSummaryStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error)
{
	CORBA_CALL( m_planAgent,
	            processFailureSummaryStepResult,
	            ( session, plan, status, error ) );
}



//++lizettejl

void PlanAgentLibrary::runPlan(const char* session, TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            runPlan,
	            ( session, plan ) );
}


// TD20368 Yanrong++
unsigned long PlanAgentLibrary::runPlanByNameWithoutActionPermittedCheck(
	const char* session, 
	const char* path, 
	const std::string param /* = "" */)
{
	unsigned long ret =0;
	CORBA_CALL_RETURN( ret,
	                   m_planAgent,
	                   runPlanByNameWithoutActionPermittedCheck,
	                   ( session, path, param.c_str() ) );
	return ret;
}
// ++Yanrong


	unsigned long PlanAgentLibrary::runPlanByName(const char* session, const char* path, const std::string param /* = "" */)
{
	unsigned long ret =0;
	CORBA_CALL_RETURN( ret,
	                   m_planAgent,
	                   runPlanByName,
	                   ( session, path, param.c_str() ) );
	return ret;
}



//marvin++
//TD13527
unsigned long PlanAgentLibrary::runPlanByNameWithEntityRestriction( const char* session, const char* path, unsigned long entitykey)
{
	unsigned long ret =0;
	CORBA_CALL_RETURN( ret,
	                   m_planAgent,
	                   runPlanByNameWithEntityRestriction,
	                   ( session, path, entitykey ) );
	return ret;
}
//marvin++
//TD13527

void PlanAgentLibrary::runOperatorTriggeredPlan(const char* session, const char* pmId, TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            runOperatorTriggeredPlan,
	            ( session, pmId, plan ) );
}


void PlanAgentLibrary::customisePlan(const char* session, const char* pmId, TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            customisePlan,
	            ( session, pmId, plan ) );
}


void PlanAgentLibrary::pausePlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            pausePlan,
	            ( session, plan ) );
}


void PlanAgentLibrary::resumePlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            resumePlan,
	            ( session, plan ) );
}


void PlanAgentLibrary::stopPlan(const char* session, const TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            stopPlan,
	            ( session, plan ) );
}


void PlanAgentLibrary::takeControl(const char* session, const char* pmId, const TA_Base_Core::ActivePlanId& plan)
{
	CORBA_CALL( m_planAgent,
	            takeControl,
	            ( session, pmId, plan ) );
}


void PlanAgentLibrary::customiseStep(const char* session, const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail)
{
	CORBA_CALL( m_planAgent,
	            customiseStep,
	            ( session, plan, detail ) );
}


void PlanAgentLibrary::getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::ExecutionHistory_out history)
{
	CORBA_CALL( m_planAgent,
	            getPlanExecutionHistory,
	            ( plan, history ) );
}


void PlanAgentLibrary::getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::CustomisationHistory_out history)
{
	CORBA_CALL( m_planAgent,
	            getPlanCustomisationHistory,
	            ( plan, history ) );
}


void PlanAgentLibrary::setPlanApprovalState(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState state)
{
	CORBA_CALL( m_planAgent,
	            setPlanApprovalState,
	            ( session, plan, state ) );
}

void PlanAgentLibrary::setPlanType(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType eType)
{
    CORBA_CALL( m_planAgent,
        setPlanType,
        ( session, plan, eType ) );
}

void PlanAgentLibrary::getRootNodeChildren(TA_Base_Core::SiblingNodes_out children)
{
	CORBA_CALL( m_planAgent,
	            getRootNodeChildren,
	            ( children ) );
}


void PlanAgentLibrary::getChildren(TA_Base_Core::NodeId parent, TA_Base_Core::SiblingNodes_out children)
{
	CORBA_CALL( m_planAgent,
	            getChildren,
	            ( parent, children ) );
}


void PlanAgentLibrary::getNodeDetail(TA_Base_Core::NodeId node, TA_Base_Core::NodeDetail_out detail)
{
	CORBA_CALL( m_planAgent,
	            getNodeDetail,
	            ( node, detail ) );
}


void PlanAgentLibrary::getNodeSummary(TA_Base_Core::NodeId node, TA_Base_Core::NodeSummary_out summary)
{
	CORBA_CALL( m_planAgent,
	            getNodeSummary,
	            ( node, summary ) );
}


void PlanAgentLibrary::getNodePath(TA_Base_Core::NodeId node, TA_Base_Core::NodePath_out path)
{
	CORBA_CALL( m_planAgent,
	            getNodePath,
	            ( node, path ) );
}


void PlanAgentLibrary::getNodeIdsInNodePath(TA_Base_Core::NodeId plan, TA_Base_Core::NodeIds_out plans)
{
	CORBA_CALL( m_planAgent,
	            getNodeIdsInNodePath,
	            ( plan, plans ) );
}


TA_Base_Core::NodeId PlanAgentLibrary::getNodeId(const char* path)
{
	TA_Base_Core::NodeId ret =0;
	CORBA_CALL_RETURN( ret,
	                   m_planAgent,
	                   getNodeId,
	                   ( path ) );
	return ret;
}

void PlanAgentLibrary::getActivePlanDetail(TA_Base_Core::NodeId plan, TA_Base_Core::ActivePlanDetails_out details)
{
	CORBA_CALL( m_planAgent,
	            getActivePlanDetail,
	            ( plan, details ) );
}


void PlanAgentLibrary::getAssocAlarmPlanDetail(const char* alarmId, TA_Base_Core::ActivePlanDetail_out detail)
{
	CORBA_CALL( m_planAgent,
	            getAssocAlarmPlanDetail,
	            ( alarmId, detail ) );
}


void PlanAgentLibrary::getActivePlanDetailByName(const char* path, TA_Base_Core::ActivePlanDetails_out details)
{
	CORBA_CALL( m_planAgent,
	            getActivePlanDetailByName,
	            ( path, details ) );
}


void PlanAgentLibrary::getAllActivePlanDetails(TA_Base_Core::ActivePlanDetails_out details)
{
	CORBA_CALL( m_planAgent,
	            getAllActivePlanDetails,
	            ( details ) );
}


void PlanAgentLibrary::getEditPlanLock(const char* session, const char* pmId, TA_Base_Core::NodeId plan)
{
	CORBA_CALL( m_planAgent,
	            getEditPlanLock,
	            ( session, pmId, plan ) );
}


void PlanAgentLibrary::releaseEditPlanLock(const char* session, TA_Base_Core::NodeId plan)
{
	CORBA_CALL( m_planAgent,
	            releaseEditPlanLock,
	            ( session, plan ) );
}


void PlanAgentLibrary::getStepNames(TA_Base_Core::StepNames_out names)
{
	CORBA_CALL( m_planAgent,
	            getStepNames,
	            ( names ) );
}


void PlanAgentLibrary::getAllStepDetails(TA_Base_Core::NodeId plan, TA_Base_Core::StepDetails_out details)
{
	CORBA_CALL( m_planAgent,
	            getAllStepDetails,
	            ( plan, details ) );
}


void PlanAgentLibrary::getSpecifiedStepDetails(TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps, TA_Base_Core::StepDetails_out details)
{
	CORBA_CALL( m_planAgent,
	            getSpecifiedStepDetails,
	            ( plan, steps, details ) );
}


void PlanAgentLibrary::getStepNumber(TA_Base_Core::StepId step, TA_Base_Core::StepNumber& position)
{
	CORBA_CALL( m_planAgent,
	            getStepNumber,
	            ( step, position ) );
}


void PlanAgentLibrary::getParameterNames(TA_Base_Core::EStep stepType, TA_Base_Core::ParameterNames_out names)
{
	CORBA_CALL( m_planAgent,
	            getParameterNames,
	            ( stepType, names ) );
}


void PlanAgentLibrary::getStepNumberParameters(TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber position, TA_Base_Core::StepParameters_out parameters)
{
	CORBA_CALL( m_planAgent,
	            getStepNumberParameters,
	            ( plan, position, parameters ) );
}


void PlanAgentLibrary::getStepIdParameters(TA_Base_Core::StepId step, TA_Base_Core::StepParameters_out parameters)
{
	CORBA_CALL( m_planAgent,
	            getStepIdParameters,
	            ( step, parameters ) );
}


void PlanAgentLibrary::updateCategoryDetail(const char* session, const TA_Base_Core::CategoryDetail& detail)
{
	CORBA_CALL( m_planAgent,
	            updateCategoryDetail,
	            ( session, detail ) );
}


void PlanAgentLibrary::insertCategory(const char* session, TA_Base_Core::NodeId parent)
{
	CORBA_CALL( m_planAgent,
	            insertCategory,
	            ( session, parent ) );
}


void PlanAgentLibrary::insertCategoryIntoRoot(const char* session)
{
	CORBA_CALL( m_planAgent,
	            insertCategoryIntoRoot,
	            ( session ) );
}


void PlanAgentLibrary::deleteCategory(const char* session, TA_Base_Core::NodeId category)
{
	CORBA_CALL( m_planAgent,
	            deleteCategory,
	            ( session, category ) );
}


void PlanAgentLibrary::updatePlanDetail(const char* session, const TA_Base_Core::PlanDetail& detail)
{
	CORBA_CALL( m_planAgent,
	            updatePlanDetail,
	            ( session, detail ) );
}


void PlanAgentLibrary::updatePlan(const char* session, const TA_Base_Core::Plan& plan)
{
	CORBA_CALL( m_planAgent,
	            updatePlan,
	            ( session, plan ) );
}


void PlanAgentLibrary::insertPlan(const char* session, TA_Base_Core::NodeId parent)
{
	CORBA_CALL( m_planAgent,
	            insertPlan,
	            ( session, parent ) );
}


void PlanAgentLibrary::insertPlanIntoRoot(const char* session)
{
	CORBA_CALL( m_planAgent,
	            insertPlanIntoRoot,
	            ( session ) );
}


void PlanAgentLibrary::deletePlans(const char* session, const TA_Base_Core::NodeIds& plans)
{
	CORBA_CALL( m_planAgent,
	            deletePlans,
	            ( session, plans ) );
}


void PlanAgentLibrary::movePlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce)
{
	CORBA_CALL( m_planAgent,
	            movePlan,
	            ( session, fromPlan, toParent, useForce ) );
}


void PlanAgentLibrary::movePlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce)
{
	CORBA_CALL( m_planAgent,
	            movePlanToRoot,
	            ( session, plan, useForce ) );
}


void PlanAgentLibrary::copyPlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce)
{
	CORBA_CALL( m_planAgent,
	            copyPlan,
	            ( session, fromPlan, toParent, useForce ) );
}


void PlanAgentLibrary::copyPlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce)
{
	CORBA_CALL( m_planAgent,
	            copyPlanToRoot,
	            ( session, plan, useForce ) );
}


void PlanAgentLibrary::getNextStepId(const char* session, TA_Base_Core::StepId& step)
{
	CORBA_CALL( m_planAgent,
	            getNextStepId,
	            ( session, step ) );
}


void PlanAgentLibrary::exportPlan (const char* session, TA_Base_Core::AuxPlanDetail_out plan, TA_Base_Core::NodeId planId)
{
	CORBA_CALL( m_planAgent,
	            exportPlan,
	            ( session, plan, planId ) );
	
}


void PlanAgentLibrary::importPlan (const char* session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
{
	CORBA_CALL( m_planAgent,
	            importPlan,
	            ( session, plan, parentId ) );
}

void PlanAgentLibrary::processBackgroundDecisionStepResponse( const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status )
{
    CORBA_CALL( m_planAgent,
        processBackgroundDecisionStepResponse,
        ( currentStepDetail, status) );
}

void PlanAgentLibrary::processBackgroundLogStepResponse( const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status, const char* logMessage )
{
    CORBA_CALL( m_planAgent,
        processBackgroundLogStepResponse,
        ( currentStepDetail, status, logMessage) );
}

void PlanAgentLibrary::processBackgroundPromptStepResponse( const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status )
{
    CORBA_CALL( m_planAgent,
        processBackgroundPromptStepResponse,
        ( currentStepDetail, status) );
}

void PlanAgentLibrary::processBackgroundLaunchGuiAppStepResponse( const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status )
{
    CORBA_CALL( m_planAgent,
        processBackgroundLaunchGuiAppStepResponse,
        ( currentStepDetail, status) );
}

void PlanAgentLibrary::processBackgroundLaunchSchematicStepResponse( const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status )
{
    CORBA_CALL( m_planAgent,
        processBackgroundLaunchSchematicStepResponse,
        ( currentStepDetail, status) );
}

}

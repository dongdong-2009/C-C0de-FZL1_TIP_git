/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent_test/PlanAgentTest.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "core/corba/src/CorbaUtil.h"
#include "PlanAgentTest.h"

PlanAgentTest::PlanAgentTest()
{
    TA_Base_Core::CorbaUtil::getInstance().initialise();
    TA_Base_Core::CorbaUtil::getInstance().activate();
}

PlanAgentTest::~PlanAgentTest()
{
    TA_Base_Core::CorbaUtil::getInstance().shutdown();
    TA_Base_Core::CorbaUtil::getInstance().cleanup();
}

void PlanAgentTest::getRootNodeChildren( void )
{
	std::cout << "=> Root Node Children " << std::endl;

	PaData::SiblingNodes_var childNodeSeq;

	m_planAgent.getRootNodeChildren(childNodeSeq.out());

	for (CORBA::ULong i = 0; i < childNodeSeq->length(); i++)
	{
		std::cout << "\tnode id: " << childNodeSeq[i].node << std::endl;
		std::cout << "\tname : " << childNodeSeq[i].name.in() << std::endl;
		std::cout << "\ttype : " << (childNodeSeq[i].type == PaData::PLAN_NODE? "Plan" : "Category") << std::endl;                    
		std::cout << "\tis approved : " << (childNodeSeq[i].approvalState == PaData::APPROVED_APPROVAL_STATE? "Yes" : "No") << std::endl;
		std::cout << "\thas children : " << (childNodeSeq[i].hasChildren? "Yes" : "No") << std::endl;
		std::cout << "\thas approved descendant : " << (childNodeSeq[i].hasApprovedDescendant? "Yes" : "No") << std::endl;
		std::cout << "\thas unapproved descendant : " << (childNodeSeq[i].hasUnapprovedDescendant? "Yes" : "No") << std::endl;

		getNodeDetails( childNodeSeq[i].node );
	}
}

void PlanAgentTest::getNodeDetails( PaData::NodeId node )
{
	std::cout << "=> Detail for " << node << std::endl;

	PaData::NodeDetail_var nodeDetail;

	m_planAgent.getNodeDetail( node, nodeDetail.out() );	

	switch( nodeDetail->_d() )
	{
		case PaData::PLAN_NODE:
		{
			std::cout << "\tname : " << nodeDetail->plan().name.in() << std::endl;
			std::cout << "\tdescript ion : " << nodeDetail->plan().description.in() << std::endl;
			std::cout << "\tapproval state : " << (nodeDetail->plan().approvalState? "Unapproved" : "Approved") << std::endl;
			std::cout << "\texecution mode : " << (nodeDetail->plan().executionMode? "Batch" : "Interactive") << std::endl;
		}
		break;

		case PaData::CATEGORY_NODE:
		{
			std::cout << "\tname : " << nodeDetail->category().name.in() << std::endl;
			std::cout << "\tdescription : " << nodeDetail->category().description.in() << std::endl;
		}
		break;

	}
}

void PlanAgentTest::getStepNames( void )
{
	std::cout << "=> Step names " << std::endl;

	PaData::StepNames_var stepNames;

	m_planAgent.getStepNames( stepNames.out() );	

	for (CORBA::ULong i = 0; i < stepNames->length(); i++)
	{
        std::cout << "\t" << stepNames[i].in() << std::endl;
	}

}

void PlanAgentTest::getParameterNames( PaData::EStep stepType )
{
	std::cout << "=> Parameter names for step type " << stepType << std::endl;

	PaData::ParameterNames_var parameterNames;

	m_planAgent.getParameterNames( stepType, parameterNames.out() );

	for (CORBA::ULong i = 0; i < parameterNames->length(); i++)
	{
        std::cout << "\t" << parameterNames[i].in() << std::endl;
	}
}

void PlanAgentTest::getAllParameterNames( void )
{
	getParameterNames( PaData::ABORT_STEP               );
	getParameterNames( PaData::ACTIVE_PLAN_CHECK_STEP   );
	getParameterNames( PaData::END_STEP					);
	getParameterNames( PaData::DATA_POINT_CHECK_STEP    );
	getParameterNames( PaData::DATA_POINT_SET_STEP      );
	getParameterNames( PaData::DECISION_STEP            );
	getParameterNames( PaData::JUMP_STEP                );
	getParameterNames( PaData::LOG_STEP                 );
	getParameterNames( PaData::PROMPT_STEP              );
	getParameterNames( PaData::RUN_PLAN_STEP            );
	getParameterNames( PaData::TERMINATE_PLAN_STEP      );
}

void PlanAgentTest::getAllStepDetails( PaData::NodeId plan )
{
	std::cout << "=> Step Details for node " << plan << std::endl;

	PaData::StepDetails_var stepDetails;

	m_planAgent.getAllStepDetails( plan, stepDetails.out() );

	for (CORBA::ULong i = 0; i < stepDetails->length(); i++)
	{
		std::cout << "\tposition : " << stepDetails[i].position << std::endl;
		std::cout << "\tdescription : " << stepDetails[i].description.in() << std::endl;
		std::cout << "\ttype : " << stepDetails[i].type << std::endl;
		std::cout << "\tskip : " << stepDetails[i].skip << std::endl;
		std::cout << "\tskippable : " << stepDetails[i].skippable << std::endl;
		std::cout << "\tdelay : " << stepDetails[i].delay << std::endl;
	}
}

void PlanAgentTest::getSpecifiedStepDetails( PaData::NodeId plan, const PaData::StepIds& steps )
{
	std::cout << "=> Step Details for node " << plan << std::endl;
	std::cout << "=> and steps ";

	PaData::StepDetails_var stepDetails;

	CORBA::ULong i = 0;

	for( i = 0; i < steps.length(); i++ )
	{
		std::cout << steps[i];
		if( steps.length()-1 != i )
		{
			std::cout << ", ";
		}
	}
	std::cout << std::endl;

	m_planAgent.getSpecifiedStepDetails( plan, steps, stepDetails.out() );

	for ( i = 0; i < stepDetails->length(); i++ )
	{
        std::cout << "\tposition : " << stepDetails[i].position << std::endl;
        std::cout << "\tdescription : " << stepDetails[i].description.in() << std::endl;
        std::cout << "\ttype : " << stepDetails[i].type << std::endl;
        std::cout << "\tskip : " << stepDetails[i].skip << std::endl;
        std::cout << "\tskippable : " << stepDetails[i].skippable << std::endl;
        std::cout << "\tdelay : " << stepDetails[i].delay << std::endl;
	}
}

void PlanAgentTest::getStepParameters( PaData::NodeId plan )
{
	for( PaData::StepNumber step = 1; step <= 14; step++ )
	{
		PaData::StepParameters_var stepParams;
		
		std::cout << "=> Step " << step;

		m_planAgent.getStepIdParameters( step, stepParams );

		switch( stepParams->_d() )
		{
			case PaData::ACTIVE_PLAN_CHECK_STEP:
			{
				std::cout << " (Active Plan Check)" << std::endl;
				// Nous Commented 
				// This has been commented because The planPath is not a member of 'ActivePlanCheckParameters' 
				// std::cout << "\tPlan path : " << stepParams->activePlanCheck().planPath.in() << std::endl;
				// Nous scommented ends


				std::cout << "\tYes step : " << stepParams->activePlanCheck().yesStep << std::endl;
				std::cout << "\tNo step : " << stepParams->activePlanCheck().noStep << std::endl;
			}
			break;

			case PaData::DATA_POINT_CHECK_STEP:
			{
				std::cout << " (Data Point Check)" << std::endl;
				std::cout << "\tData point : " << stepParams->dataPointCheck().dataPoint.in() << std::endl;
				
				switch( stepParams->dataPointCheck().value._d() )
				{
					case PaData::BOOLEAN_DATA_POINT:
						std::cout << "\tBoolean value : " << stepParams->dataPointCheck().value.booleanValue() << std::endl;
						break;

					case PaData::NUMERIC_DATA_POINT:
						std::cout << "\tNumerical value (value) : " << stepParams->dataPointCheck().value.numericValue().value << std::endl;
						std::cout << "\tNumerical value (precision) : " << stepParams->dataPointCheck().value.numericValue().precision << std::endl;
						break;

					case PaData::TEXT_DATA_POINT:
						std::cout << "\tText value : " << stepParams->dataPointCheck().value.textValue() << std::endl;
						break;
				}

				std::cout << "\tTest type : " << stepParams->dataPointCheck().testType << std::endl;
				std::cout << "\tSuccess step : " << stepParams->dataPointCheck().successStep << std::endl;
				std::cout << "\tFail step : " << stepParams->dataPointCheck().failureStep << std::endl;
			}
			break;

			case PaData::DATA_POINT_SET_STEP:
			{
				std::cout << " (Data Point Set)" << std::endl;
				std::cout << "\tData point : " << stepParams->dataPointSet().dataPoint.in() << std::endl;

				switch( stepParams->dataPointSet().newValue._d() )
				{
					case PaData::BOOLEAN_DATA_POINT:
						std::cout << "\tBoolean value : " << stepParams->dataPointSet().newValue.booleanValue() << std::endl;
						break;

					case PaData::NUMERIC_DATA_POINT:
						std::cout << "\tNumerical value (value) : " << stepParams->dataPointSet().newValue.numericValue().value << std::endl;
						std::cout << "\tNumerical value (precision) : " << stepParams->dataPointSet().newValue.numericValue().precision << std::endl;
						break;

					case PaData::TEXT_DATA_POINT:
						std::cout << "\tText value : " << stepParams->dataPointSet().newValue.textValue() << std::endl;
						break;
				}
			}
			break;

			case PaData::DECISION_STEP:
			{
				std::cout << " (Decision)" << std::endl;
				std::cout << "\tMessage : " << stepParams->decision().decisionMessage.in() << std::endl;
				std::cout << "\tTimeout : " << stepParams->decision().responseTimeout << std::endl;
				std::cout << "\tYes step : " << stepParams->decision().yesStep << std::endl;
				std::cout << "\tNo step : " << stepParams->decision().noStep << std::endl;
			}
			break;

			case PaData::JUMP_STEP:
			{
				std::cout << " (Jump)" << std::endl;
				std::cout << "\tStep : " << stepParams->jump().step << std::endl;
			}
			break;

			case PaData::LOG_STEP:
			{
				std::cout << " (Log)" << std::endl;
				std::cout << "\tForce : " << stepParams->log().message << std::endl;
			}
			break;

			case PaData::PROMPT_STEP:
			{
				std::cout << " (Prompt)" << std::endl;
				std::cout << "\tMessage : " << stepParams->prompt().message.in() << std::endl;
				std::cout << "\tTimeout : " << stepParams->prompt().dismissTimeout << std::endl;
			}
			break;

			case PaData::RUN_PLAN_STEP:
			{
				std::cout << " (Prompt)" << std::endl;
				std::cout << "\tPlan : " << stepParams->runPlan().planPath.in() << std::endl;
				std::cout << "\tManual launch : " << stepParams->runPlan().manualLaunch << std::endl;
				std::cout << "\tWait for completion : " << stepParams->runPlan().waitForCompletion << std::endl;
			}
			break;

			case PaData::TERMINATE_PLAN_STEP:
			{
				std::cout << " (Prompt)" << std::endl;
				std::cout << "\tPlan : " << stepParams->terminatePlan().planPath.in() << std::endl;
			}
			break;

			case PaData::UNDEFINED_STEP:
				break;
		}
	}
}


void PlanAgentTest::getNodePath( PaData::NodeId plan )
{
	std::cout << "=> Node path for plan " << plan << std::endl << std::endl;

	PaData::NodePath_var path;

	m_planAgent.getNodePath( plan, path.out()  );
	std::cout << path.in() << std::endl;
}


void PlanAgentTest::getActivePlanDetails(PaData::NodeId plan)
{
	std::cout << "=> Active instance details for plan " << plan << std::endl;

    PaData::ActivePlanDetails_var activePlanDetails;

    m_planAgent.getActivePlanDetail(plan, activePlanDetails.out());

    for (int i = 0; i < activePlanDetails->length(); i++)
    {       
        std::string planState;

        switch (activePlanDetails[i].activePlanState)
        {
             case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
                 planState = "executing";
                 break;

             case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
                 planState = "paused";
                 break;

             case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
                 planState = "stopped";
                 break;

             case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                 planState = "completed";
                 break;

             case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                 planState = "aborted";
                 break;

             case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
                 planState = "failed";
                 break;

             case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
                 planState = "pending";
                 break;

             case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
                 planState = "pausedforOperator";
                 break;

             case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
                 planState = "expired";
                 break;

              case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
                 planState = "undefined";
                 break;
        }

        std::cout << "\tpath : " << activePlanDetails[i].path << std::endl;
        std::cout << "\tinstance : " << activePlanDetails[i].plan.instance << std::endl;
        std::cout << "\tlocation : " << activePlanDetails[i].plan.location << std::endl;
        std::cout << "\towner : " << activePlanDetails[i].owner << std::endl;
        std::cout << "\tactivation : " << activePlanDetails[i].timeOfInstantiation << std::endl;
        std::cout << "\tstate : " << planState << std::endl;
        std::cout << "\tstep : " << activePlanDetails[i].currentStepDetail.position << " - " << activePlanDetails[i].currentStepDetail.name << std::endl;
        std::cout << "\tupdate ID : " << activePlanDetails[i].activeUpdateId << std::endl << std::endl;
    }
}


void PlanAgentTest::getAllActivePlanDetails()
{
	std::cout << "=> Active instance details for all plans " << std::endl << std::endl;

    PaData::ActivePlanDetails_var activePlanDetails;

    m_planAgent.getAllActivePlanDetails(activePlanDetails.out());

    for (int i = 0; i < activePlanDetails->length(); i++)
    {       
        std::string planState;

        switch (activePlanDetails[i].activePlanState)
        {
             case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
                 planState = "executing";
                 break;

             case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
                 planState = "paused";
                 break;

             case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
                 planState = "stopped";
                 break;

             case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                 planState = "completed";
                 break;

             case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                 planState = "aborted";
                 break;

             case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
                 planState = "failed";
                 break;

             case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
                 planState = "pending";
                 break;

             case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
                 planState = "pausedforOperator";
                 break;

             case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
                 planState = "expired";
                 break;

              case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
                 planState = "undefined";
                 break;
        }

        std::cout << "\tpath : " << activePlanDetails[i].path << std::endl;
        std::cout << "\tplan ID : " << activePlanDetails[i].plan.plan << std::endl;
        std::cout << "\tinstance : " << activePlanDetails[i].plan.instance << std::endl;
        std::cout << "\tlocation : " << activePlanDetails[i].plan.location << std::endl;
        std::cout << "\towner : " << activePlanDetails[i].owner << std::endl;
        std::cout << "\tactivation : " << activePlanDetails[i].timeOfInstantiation << std::endl;
        std::cout << "\tstate : " << planState << std::endl;
        std::cout << "\tstep : " << activePlanDetails[i].currentStepDetail.position << " - " << activePlanDetails[i].currentStepDetail.name << std::endl;
        std::cout << "\tupdate ID : " << activePlanDetails[i].activeUpdateId << std::endl << std::endl;
    }
}


void PlanAgentTest::runPlan( const  char * sessionId,PaData::ActivePlanId& plan )
{
	std::cout << "=> Run plan " << plan.plan << std::endl; 

	m_planAgent.runPlan( sessionId, plan );

	
	std::cout << "plan instance = " << plan.instance << std::endl;
}

void PlanAgentTest::stopPlan( const  char * sessionId,const PaData::ActivePlanId& plan )
{
	std::cout << "=> Stop plan( " << plan.plan << "," << plan.instance << ")" << std::endl; 

	m_planAgent.stopPlan( sessionId, plan );
}

void PlanAgentTest::pausePlan( const  char * sessionId,const PaData::ActivePlanId& plan )
{
	std::cout << "=> Pause plan( " << plan.plan << "," << plan.instance << ")" << std::endl; 

	m_planAgent.pausePlan( sessionId, plan );
}

void PlanAgentTest::resumePlan( const  char * sessionId,const PaData::ActivePlanId& plan )
{
	std::cout << "=> Resume plan( " << plan.plan << "," << plan.instance << ")" << std::endl; 

	try
	{
		m_planAgent.resumePlan( sessionId, plan );
	}
	catch ( const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae )
	{
		std::cout << "Exception: " << pae.what << std::endl;
	}
	catch ( ... )
	{
		std::cout << "Exception: Unknown"  << std::endl;
	}

}



void PlanAgentTest::deleteCategory(const TA_Base_Core::NodeId category)
{
	try
	{
		m_planAgent.deleteCategory("10", category);
	}
	catch ( const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae )
	{
		std::cout << "Exception: " << pae.what << std::endl;
	}
}


void PlanAgentTest::deletePlans()
{
	TA_Base_Core::NodeIds_var plans = new TA_Base_Core::NodeIds;
	plans->length(1);

	plans[0] = 2583;
/*	plans[1] = 2394;
	plans[2] = 2416;
	plans[3] = 2418; */
	try
	{
		m_planAgent.deletePlans("",plans);
	}
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae)
	{
		std::cout << "Caught PlanAgentError exception: " << pae.what << std::endl;
	}
}


void PlanAgentTest::updateCategory(const TA_Base_Core::NodeId parent)
{
	TA_Base_Core::CategoryDetail_var detail = new TA_Base_Core::CategoryDetail;


	detail->parent = 5;
	detail->category =  1241;
	detail->name = CORBA::string_dup("updateCatMahaveerpSchema");
	detail->description= CORBA::string_dup("updateCatMahaveerpSchemaDescription");
	m_planAgent.updateCategoryDetail("", detail);

}


void PlanAgentTest::insertCategory(const TA_Base_Core::NodeId parent)
{


	m_planAgent.insertCategory("", parent);

}



TA_Base_Core::NodeIds PlanAgentTest::getNodeIdsInNodePath ( TA_Base_Core::NodeId category )
{
	TA_Base_Core::NodeIds_var ni;
	try
	{
		m_planAgent.getNodeIdsInNodePath( category, ni );
	}
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae)
	{
		std::cout << "Caught PlanAgentError exception: " << pae.what << std::endl;
	}

	int n = ni->length();

	TA_Base_Core::NodeId nodeId = 0;

	for (int i=0; i<n; i++)
	{

		nodeId = ni.operator[](i);
	}

	return ni;
}


void PlanAgentTest::movePlan(const TA_Base_Core::NodeId fromPlan, const TA_Base_Core::NodeId toParent)
{
	try
	{
		
		//Nous Commented
		// Here since the method requires four parameters a boolean value of true is added
		//m_planAgent.movePlan("", fromPlan, toParent);
		// Nous Commented ends
		// Nous changes
		m_planAgent.movePlan("", fromPlan, toParent, true);
		// Nous changes ends

	}
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae)
	{
		std::cout << "Caught PlanAgentError exception: " << pae.what << std::endl;
	}
}

void PlanAgentTest::copyPlan(const  char * sessionId,const TA_Base_Core::NodeId plan, const TA_Base_Core::NodeId parent,EExecutionMode execMode)
{
	try
	{
		// Nous commented
		// Here since the method requires four parameters a boolean value of true is added
		//m_planAgent.copyPlan(sessionId, plan, parent, execMode);
		// Nous commented ends

		// Nous changes
		m_planAgent.copyPlan(sessionId, plan, parent, execMode, true);
		// NOus changes

	}
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae)
	{
		std::cout << "Caught PlanAgentError exception: " << pae.what << std::endl;
	}

}

void PlanAgentTest::insertPlan(const  char * sessionId,const TA_Base_Core::NodeId parent,EExecutionMode execMode)
{
	TA_Base_Core::PlanDetail_var planDetail = new TA_Base_Core::PlanDetail;


		try
		{
			m_planAgent.insertPlan(sessionId, parent, execMode);
		}

		catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& pae)
		{
			std::cout << "Caught PlanAgentError exception: " << pae.what << std::endl;
		}


}


TA_Base_Core::StepId PlanAgentTest::getNextStepId()
{

	TA_Base_Core::StepId nextStepId;

	m_planAgent.getNextStepId("", nextStepId);
	return nextStepId;
}

void PlanAgentTest::updatePlan(const TA_Base_Core::NodeId node)
{

	TA_Base_Core::Plan_var plan = new TA_Base_Core::Plan;

	//Initialise the plan detail
	plan->detail.parent = 1; //does not matter
	plan->detail.plan = node;
	plan->detail.name = CORBA::string_dup("First Updated Plan");
	plan->detail.description = CORBA::string_dup("Description of First Updated Plan");
	plan->detail.approvalState = TA_Base_Core::UNAPPROVED_APPROVAL_STATE;
	plan->detail.executionMode = TA_Base_Core::BATCH_EXECUTION_MODE;

	//Initialise stepseq

	plan->stepseq.length(3); //Set the number of steps

	//Inittialise step detail
	TA_Base_Core::StepId jump1StepId = getNextStepId();
	plan->stepseq[0].detail.position =1;
	plan->stepseq[0].detail.step = jump1StepId;
    plan->stepseq[0].detail.name = CORBA::string_dup("First Jump");
	plan->stepseq[0].detail.description = CORBA::string_dup("Description of First Jump");
	plan->stepseq[0].detail.type = TA_Base_Core::JUMP_STEP;
	plan->stepseq[0].detail.skip = false;
	plan->stepseq[0].detail.skippable = false;
	plan->stepseq[0].detail.delay = 2;

	TA_Base_Core::StepId jump2StepId = getNextStepId();
	plan->stepseq[1].detail.position =2;
	plan->stepseq[1].detail.step = jump2StepId;
    plan->stepseq[1].detail.name = CORBA::string_dup("Second Jump");
	plan->stepseq[1].detail.description = CORBA::string_dup("Description of Second Jump");
	plan->stepseq[1].detail.type = TA_Base_Core::JUMP_STEP;
	plan->stepseq[1].detail.skip = false;
	plan->stepseq[1].detail.skippable = false;
	plan->stepseq[1].detail.delay = 2;

	plan->stepseq[2].detail.position =3;
	plan->stepseq[2].detail.step = getNextStepId();
    plan->stepseq[2].detail.name = CORBA::string_dup("End Step");
	plan->stepseq[2].detail.description = CORBA::string_dup("Description of End Step");
	plan->stepseq[2].detail.type = TA_Base_Core::END_STEP;
	plan->stepseq[2].detail.skip = false;
	plan->stepseq[2].detail.skippable = false;
	plan->stepseq[2].detail.delay = 0;


	TA_Base_Core::JumpParameters_var jump1Params = new TA_Base_Core::JumpParameters;
	jump1Params->step = jump2StepId;

	plan->stepseq[0].parameters.jump(jump1Params);

	TA_Base_Core::JumpParameters_var jump2Params = new TA_Base_Core::JumpParameters;
	jump2Params->step = jump1StepId;

	plan->stepseq[1].parameters.jump(jump2Params);

	TA_Base_Core::EndParameters endParams = 0;
	plan->stepseq[2].parameters.end(endParams);

	m_planAgent.updatePlan("", plan);

}


void PlanAgentTest::getEditPlanLock( const std::string& session,TA_Base_Core::NodeId plan )
{
	// Nous Commented
	// Here the method takes three parameters and the  second one is string. hence a dummy string is added 
	//m_planAgent.getEditPlanLock(session.c_str(), plan );
	// Nous commented ends

	// Nous changes
	m_planAgent.getEditPlanLock(session.c_str(), "" ,  plan );   	
	// Nous changes


}

void PlanAgentTest::releaseEditPlanLock( const std::string& session,TA_Base_Core::NodeId plan )
{
	try
	{
		m_planAgent.releaseEditPlanLock(session.c_str(), plan);
	}
	catch(...)
	{
		std::cout << "Only owner can release the lock." << std::endl;
	}
}


void PlanAgentTest::customisePlan(const  char * sessionId,TA_Base_Core::ActivePlanId& plan)
{
	try
	{
		// Nous Commented
		// Here the method takes three parameters and the  second one is string. hence a dummy string is added 
		//m_planAgent.customisePlan(sessionId, plan);
		// Nous commented ends
		
		// Nous Changes
		m_planAgent.customisePlan(sessionId, "" , plan);
		// Nous changes ends

	}
	catch ( ... )
	{
		std::cout << "Exception thrown while customising a Plan." << std::endl;
	}


}

void PlanAgentTest::customiseStep(const  char * sessionId,const TA_Base_Core::ActivePlanId& plan )
{
	try
	{
		TA_Base_Core::StepCustomisableDetail_var detail1 = new TA_Base_Core::StepCustomisableDetail;
		detail1->position = 1;
		detail1->skip = 0;
		detail1->delay = 10;
		m_planAgent.customiseStep( sessionId, plan, detail1);

		TA_Base_Core::StepCustomisableDetail_var detail2 = new TA_Base_Core::StepCustomisableDetail;
		detail2->position = 2;
		detail2->skip = 0;
		detail2->delay = 20;
		m_planAgent.customiseStep( sessionId, plan, detail2);

		detail1->position = 1;
		detail1->skip = 0;
		detail1->delay = 5;
		m_planAgent.customiseStep( sessionId, plan, detail1);


	}
	catch ( ... )
	{


	}


}

void PlanAgentTest::getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan)
{	
	TA_Base_Core::ExecutionHistory_var history = new TA_Base_Core::ExecutionHistory;
	try
	{
		
		m_planAgent.getPlanExecutionHistory(plan,history);
	}
	catch ( ... )
	{
		std::cout << "Exception in getPlanExecutionHistory." << std::endl;
	}

	TA_Base_Core::UpdateId upId = history->activeUpdateId;
	int nlen = history->details.length();
	for ( int i = 0; i < nlen; i++ )
	{
		TA_Base_Core::StepNumber position = history->details[i].position;
		std::string name = history->details[i].name.in();
		TA_Base_Core::EActiveStepState state = history->details[i].state;
	}

}


void PlanAgentTest::getPlanCustomHistory(const TA_Base_Core::ActivePlanId& plan)
{	
	TA_Base_Core::CustomisationHistory_var history = new TA_Base_Core::CustomisationHistory;
	try
	{
		
		m_planAgent.getPlanCustomisationHistory(plan,history);
	}
	catch ( ... )
	{
		std::cout << "Exception in getPlanCustomHistory." << std::endl;
	}

	TA_Base_Core::UpdateId upId = history->customUpdateId;
	int nlen = history->details.length();
	for ( int i = 0; i < nlen; i++ )
	{
		TA_Base_Core::StepNumber position = history->details[i].position;
		unsigned long skip = history->details[i].skip;
		unsigned long delay = history->details[i].delay;
	}

}

void PlanAgentTest::takeControl(const  char * sessionId, TA_Base_Core::ActivePlanId& plan)
{
	try
	{
		// Nous commented
		// Here the method takes three parameters and the  second one is string. hence a dummy string is added 
		//m_planAgent.takeControl(sessionId, plan);
		// Nous commented ends

		// Nous changes
		m_planAgent.takeControl(sessionId, "" ,  plan);
		// Nous changes ends

	}
	catch ( ... )
	{
		std::cout << "Exception in takeControl." << std::endl;
	}

}


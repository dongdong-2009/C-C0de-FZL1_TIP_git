/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_EVALUATE_EXPRESSION )

#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/BooleanContextImpl.h"
#include "app/response_plans/plan_agent/src/ActiveEvaluateExpressionStep.h"
#include "app/response_plans/common/src/StringConstants.h"


// namespace alias for Corba structures/types

namespace TA_Base_App
{
	ActiveEvaluateExpressionStep::ActiveEvaluateExpressionStep( const TA_Base_Core::StepDetail& stepDetail,
        const TA_Base_Core::EvaluateExpressionParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveEvaluateExpressionStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Evaluate Expression step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		bool success = false;
		std::string expression = m_params.expression.in();
		std::auto_ptr<TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator());
		BooleanContextImpl* context = new BooleanContextImpl ();

		try
		{
			//as we are only interested in boolean expressions 
			//we use boolean context only 
			expEval->setNewExpression(expression);
			expEval->addBooleanContext (context);
			success = expEval->evaluateBooleanExpression ();

            if ( !m_flowControl.setNextStep( success ? m_params.successStep : m_params.failureStep ))
            {
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
            }
		}
		catch(TA_Base_Core::MathematicalEvaluationException& mee )
		{	//should not get here as we do check for valid expression before
			//runing this step (plan)
			std::ostringstream msg;
			msg << dumpContext() << ": " << mee.what();
			
			m_remark = RemarkInvalidExpress;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::MathematicalEvaluationException", msg.str().c_str() );
			//delete context;
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			m_remark = RemarkInvalidStepParameter;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::ActiveStepParamException", dumpContext().c_str() );
			//delete context;
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
	
		delete context;
		return stepState;
	}

	
} // TA_Base_App


#endif //  STEPTYPE_EVALUATE_EXPRESSION


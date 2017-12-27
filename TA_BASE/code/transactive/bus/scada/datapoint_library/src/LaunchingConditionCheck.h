/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/LaunchingConditionCheck.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/21 14:56:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  */

#ifndef LAUNCHING_CONDITION_CHECK_H
#define LAUNCHING_CONDITION_CHECK_H

#include <string>

namespace TA_Base_Bus
{
    // forward definition
	class ExpressionEvaluator;
	class IAnswerBackErrorNotification;
	class BooleanContext;
	class RealNumberContext;

	class LaunchingConditionCheck
	{
	public:

		/**
		*
		* Contructor
		*
		* Parameterised contructor
		*
		* @param    launchingExpression			The expression to be evaluated before command is executed
		*
		*/
		LaunchingConditionCheck ( const std::string & launchingExpression );


		/**
		*
		* Destructor
		*
		*/
		virtual ~LaunchingConditionCheck();


		/**
		*
		* updateLaunchingExpression
		*
		* update the current launching (interlocking) expression with specified expression
		*
		* @param    newExpression	The new launching expression to be evaluated
		*
		*/
		void updateLaunchingExpression ( const std::string & newExpression );


		/**
		*
		* evaluateLaunchingExpression
		*
		* Start evaluating the launching condition.
		*
		*/
		bool evaluateLaunchingExpression();


		/**
		*
		* getLaunchingConditionResultString
		*
		* Returns a string containing the result of the expression evaluation in
		* the following format:
		*
		* [PointDescription1] is [Value1], [PointDescription2] is [Value2], ...
		*
		*/
		std::string getLaunchingConditionResultString();


	private:


		std::string m_launchingExpression;
		TA_Base_Bus::ExpressionEvaluator * m_BusExpressionEvaluator;
		CommonContext* m_context;

		LaunchingConditionCheck(){};
		LaunchingConditionCheck ( const LaunchingConditionCheck & obj );
		LaunchingConditionCheck & operator= ( const LaunchingConditionCheck & rhs );
	};

}

#endif

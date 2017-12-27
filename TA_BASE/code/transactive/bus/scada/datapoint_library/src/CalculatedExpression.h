/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/CalculatedExpression.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/21 14:56:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  */

#ifndef CALCULATED_EXPRESSION_H
#define CALCULATED_EXPRESSION_H

#include <string>


namespace TA_Base_Bus
{
	class IExpressionNodeNotification;
};


namespace TA_Base_Bus
{
	class ExpressionEvaluator;
};


namespace TA_Base_Bus
{
	class CommonContext;
};


namespace TA_Base_Bus
{
	class CalculatedExpression
	{
	public:

		/**
		*
		* CalculatedExpression
		*
		* Parameterised contructor.  Note that Calculated expression uses the existing interface
		* IExpressionNodeNotification as a means to inform the user of changes from the underlying
		* dependencies.
		*
		* @param    user						The registered user for notification of changes
		* @param    calculatedExpression		The calculated expression to be evaluated
		*/
		CalculatedExpression ( IExpressionNodeNotification * user,
							  const std::string & calculatedExpression );
		virtual ~CalculatedExpression();


		// 
		// operations specific to CalculatedExpression
		//


		/**
		*
		* evaluateBooleanExpression
		*
		* evaluates the current mathematical expression as a boolean expression.
		*
		* @param    returnedValue		The result of the expression after evaluation
		* @param    returnedStatus		The status of the returned result
		*
		* @exception MathematicalEvaluationException thrown if the expression
		*            could not be evaluated.
		*
		*/
        void evaluateBooleanExpression ( bool & returnedValue, TA_Base_Bus::EQualityStatus & returnedStatus );

		
		/**
		*
		* evaluateRealNumberExpression
		*
		* evaluates the current mathematical expression as a real number expression.
		*
		* @param    returnedValue		The result of the expression after evaluation
		* @param    returnedStatus		The status of the returned result
		*
		* @exception MathematicalEvaluationException thrown if the expression
		*            could not be evaluated.
		*
		*/
		void evaluateRealNumberExpression ( double & returnedValue, TA_Base_Bus::EQualityStatus & returnedStatus );


		/**
		*
		* updateCalculatedExpression
		*
		* update the current mathematical expression with specified expression
		*
		* @param    newExpression	The new mathematical expression to be evaluated
		*
		*/
		void updateCalculatedExpression ( const std::string & newExpression );


		/**
		*
		* enableCallback
		*
		* This enables the callback mechanism of the calculated expression such that
		* any online update of the underlying datapoints will be relayed to the user
		* via IExpressionNodeNotification interface.
		*
		* @param    enabled		Indication if callback is enabled.
		*
		*/
		void enableCallback ( bool enabled );


	private:

		std::string m_calculatedExpression;

		TA_Base_Bus::ExpressionEvaluator * m_BusExpressionEvaluator;
		CommonContext* m_context;

		CalculatedExpression(){};
		CalculatedExpression ( const CalculatedExpression & obj );
		CalculatedExpression & operator= ( const CalculatedExpression & rhs );
	};

} // close namespace TA_Base_Bus


#endif

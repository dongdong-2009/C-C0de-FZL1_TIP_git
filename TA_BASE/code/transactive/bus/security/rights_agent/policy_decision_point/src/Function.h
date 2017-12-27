#ifndef FUNCTION_H_INCLUDED
#define FUNCTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Function.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  *
  *  The root of a GoF's Interpreter pattern hierarchy for expression evaluation.
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Function
	{
	public:

		Function(){}
		virtual ~Function(){}

		virtual PdpConst::EConditionResult evaluate( const Context& context ) const;

		virtual ValueSet getValues( const Context& context ) const;

	private:

		Function( const Function &x );
		Function& operator=( const Function &x );

	}; // Function
}; // TA_Base_Bus

#endif // FUNCTION_H_INCLUDED

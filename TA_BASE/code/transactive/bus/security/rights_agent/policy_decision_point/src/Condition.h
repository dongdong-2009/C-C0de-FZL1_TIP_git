#ifndef TA_APP_CONDITION_H_INCLUDED
#define TA_APP_CONDITION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Condition.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  *
  *  Currently this is just a wrapper for Function. However in a more complete 
  *  implementation, Condition would contain additional functionality. It is left
  *  here as a placeholder for that functionality.
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Condition
	{
	public:

		Condition(){}
		~Condition(){}

		Condition( const Function* function );

		PdpConst::EConditionResult evaluate( const Context& context ) const;

	private:

		Condition( const Condition &x );
		Condition& operator=( const Condition &x );

		std::auto_ptr<const Function> m_function;

	}; // Condition
}; // TA_Base_Bus

#endif // TA_APP_CONDITION_H_INCLUDED

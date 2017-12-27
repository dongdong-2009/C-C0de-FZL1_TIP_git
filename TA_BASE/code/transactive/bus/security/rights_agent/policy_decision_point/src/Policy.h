#ifndef POLICY_H_INCLUDED
#define POLICY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Policy.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Policy
	{
	public:
		
		Policy( const Target* target, const std::list<const Rule*>& ruleList );		
		Policy( const Target* target, const std::list<const Rule*>& ruleList, bool isDuty );
		
		~Policy();

		void evaluate( const Context& context, PolicyCombiningAlg& result ) const;
		bool isDutyPolicy() const;
	private:

		Policy( const Policy &x );
		Policy& operator=( const Policy &x );

		typedef std::list<const Rule* > RuleList;
		typedef RuleList::iterator RuleListIt;
		typedef RuleList::const_iterator RuleListConstIt;

		std::auto_ptr<const Target> m_target;
		RuleList m_ruleList;
		bool isDuty_;

	}; // Policy
}; // TA_Base_Bus

#endif // POLICY_H_INCLUDED

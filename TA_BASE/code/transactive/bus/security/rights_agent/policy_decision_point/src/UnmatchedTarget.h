#ifndef UNMATCHEDTARGETALG_H_INCLUDED
#define UNMATCHEDTARGETALG_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/UnmatchedTarget.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  When a target for a rule or policy is unmatched by the request context
  *  this class will determine the approptiate policy result.
  * 
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class UnmatchedTarget
	{
	public:

		static PdpConst::EPolicyResult evaluate( PdpConst::EMatchResult targetResult );

	private:

		UnmatchedTarget( const UnmatchedTarget &x );
		UnmatchedTarget& operator=( const UnmatchedTarget &x );

	}; // UnmatchedTarget
}; // TA_Base_Bus

#endif // UNMATCHEDTARGETALG_H_INCLUDED

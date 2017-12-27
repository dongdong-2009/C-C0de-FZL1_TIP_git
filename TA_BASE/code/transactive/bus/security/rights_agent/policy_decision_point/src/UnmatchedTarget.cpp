/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/UnmatchedTarget.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

namespace TA_Base_Bus
{

	PdpConst::EPolicyResult UnmatchedTarget::evaluate( PdpConst::EMatchResult targetResult )
	{
		switch( targetResult )
		{
			case PdpConst::NO_MATCH_MR:
				return( PdpConst::NOT_APPLICABLE_PR );

			case PdpConst::INDETERMINATE_MR:
				return( PdpConst::INDETERMINATE_PR );

			default:
				std::ostringstream msg;
				msg << "targetResult == " << targetResult;
				TA_ASSERT( false, msg.str().c_str() );
				return( PdpConst::UNDEFINED_PR );
				break;
		}
	}
	
} // TA_Base_Bus

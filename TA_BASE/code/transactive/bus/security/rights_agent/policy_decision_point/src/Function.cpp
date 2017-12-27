/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Function.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

// namespace alias for Corba structures/types

namespace TA_Base_Bus
{

	PdpConst::EConditionResult Function::evaluate( const Context& context ) const
	{
		TA_ASSERT( false, "Default behaviour for this method is to assert" );
		return( PdpConst::UNDEFINED_CR );
	}

	ValueSet  Function::getValues( const Context& context ) const
	{
		TA_ASSERT( false, "Default behaviour for this method is to assert" );
		ValueSet tmpVs;
		return( tmpVs );
	}

} // TA_Base_Bus

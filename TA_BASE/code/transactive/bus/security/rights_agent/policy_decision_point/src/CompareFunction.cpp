/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/CompareFunction.cpp $
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
	CompareFunction::CompareFunction( const Function* lhsOp, const Function* rhsOp,
		PdpConst::EComparisonType compareType ) :
		m_lhsOp(lhsOp),
		m_rhsOp(rhsOp),
		m_compareType(compareType)
	{
		TA_ASSERT( NULL != m_lhsOp.get(), "m_lhsOp == NULL" );
		TA_ASSERT( NULL != m_rhsOp.get(), "m_rhsOp == NULL" );
	}

	PdpConst::EConditionResult CompareFunction::evaluate( const Context& context ) const
	{
		ValueSet lhs = m_lhsOp->getValues( context );
		ValueSet rhs = m_rhsOp->getValues( context );

		if( (0 == lhs.size()) || (0 == rhs.size()) )
		{
			return( PdpConst::INDETERMINATE_CR );
		}

		switch( m_compareType )
		{
			case PdpConst::IS_EQUAL_CT: 
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s == %s", lhs.dump().c_str(), rhs.dump().c_str() );
				if( lhs == rhs )
				{
					return( PdpConst::TRUE_CR );
				}
				break;

			case PdpConst::IS_LESSTHAN_OR_EQUAL_CT:
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s <= %s", lhs.dump().c_str(), rhs.dump().c_str() );
				if( lhs <= rhs )
				{
					return( PdpConst::TRUE_CR );
				}
				break;

			case PdpConst::IS_NOT_EQUAL_CT:
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s != %s", lhs.dump().c_str(), rhs.dump().c_str() );
				if( lhs != rhs )
				{
					return( PdpConst::TRUE_CR );
				}
				break;

			default:
				std::ostringstream msg;
				msg << "m_compareType == " << m_compareType;
				TA_ASSERT( false, msg.str().c_str() );
				break;
		}

		return( PdpConst::FALSE_CR );
	}

} // TA_Base_Bus

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Rule.cpp $
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

	Rule::Rule( const std::string& id, const Target* target, PdpConst::EPolicyResult effect ) :
		m_id(id),
		m_target(target),
		m_effect(effect)
	{
	}

	Rule::Rule( const std::string& id, const Target* target, const Condition* condition, PdpConst::EPolicyResult effect ) :
		m_id(id),
		m_target(target),
		m_condition(condition),
		m_effect(effect)
	{
	}

	void Rule::evaluate( const Context& context, RuleCombiningAlg& result ) const
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Rule: %s", m_id.c_str() );

		if( NULL != m_target.get() )
		{
			if( 0 < m_target->size() )
			{
				PdpConst::EMatchResult targetResult = m_target->evaluate( context );

				if( PdpConst::MATCH_MR != targetResult )
				{
					result.setResult( m_effect, UnmatchedTarget::evaluate(targetResult) );
					return;
				}
			}
		}

		if( NULL == m_condition.get() )
		{
			result.setResult( m_effect, m_effect );
			return;
		}

		switch( m_condition->evaluate( context ) )
		{
			case PdpConst::TRUE_CR:
				result.setResult( m_effect, m_effect );
				break;

			case PdpConst::FALSE_CR:
				result.setResult( m_effect, PdpConst::NOT_APPLICABLE_PR );
				break;

			case PdpConst::INDETERMINATE_CR:
				result.setResult( m_effect, PdpConst::INDETERMINATE_PR );
				break;

			default:
				std::ostringstream msg;
				msg << "m_condition->evaluate( context ) == " << m_condition->evaluate( context );
				TA_ASSERT( false, msg.str().c_str() );
				result.setResult( m_effect, PdpConst::UNDEFINED_PR );
				break;
		}
	}

} // TA_Base_Bus

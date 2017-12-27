/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Target.cpp $
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
	void Target::addElement( const SubjectAttributeSelector* as1,
			const ConstAttributeSelector* as2,
			PdpConst::EComparisonType cmpType )
	{
		std::auto_ptr<const Function> fn1( as1 );
		std::auto_ptr<const Function> fn2( as2 );

		std::auto_ptr<const CompareFunction> cmp( new CompareFunction( fn1.release(), fn2.release(), cmpType) );

		m_elementList.push_back( cmp.release() );
	}

	void Target::addElement( const ActionAttributeSelector* as1,
			const ConstAttributeSelector* as2,
			PdpConst::EComparisonType cmpType )
	{
		std::auto_ptr<const Function> fn1( as1 );
		std::auto_ptr<const Function> fn2( as2 );

		std::auto_ptr<const CompareFunction> cmp( new CompareFunction( fn1.release(), fn2.release(), cmpType) );

		m_elementList.push_back( cmp.release() );
	}

	void Target::addElement( const ResourceAttributeSelector* as1,
			const ConstAttributeSelector* as2,
			PdpConst::EComparisonType cmpType )
	{
		std::auto_ptr<const Function> fn1( as1 );
		std::auto_ptr<const Function> fn2( as2 );

		std::auto_ptr<const CompareFunction> cmp( new CompareFunction( fn1.release(), fn2.release(), cmpType) );

		m_elementList.push_back( cmp.release() );
	}

	PdpConst::EMatchResult Target::evaluate( const Context& context ) const
	{
		ElementList::const_iterator it;

		PdpConst::EMatchResult mr( PdpConst::MATCH_MR );

		it = m_elementList.begin();

		while( (PdpConst::MATCH_MR == mr) && (m_elementList.end() != it) )
		{
			switch( (*it)->evaluate( context ) )
			{
			case PdpConst::TRUE_CR:
				break;

			case PdpConst::FALSE_CR:
				mr = PdpConst::NO_MATCH_MR;
				break;

			case PdpConst::INDETERMINATE_CR:
				mr = PdpConst::INDETERMINATE_MR;
				break;

			default:
				std::ostringstream msg;
				msg << "(*it)->evaluate( context ) == " << (*it)->evaluate( context );
				TA_ASSERT( false, msg.str().c_str()  );
				break;
			}
			it++;
		}

		return( mr );
	}

	std::list<const CompareFunction*>::size_type Target::size() const
	{
		return( m_elementList.size() );
	}

} // TA_Base_Bus

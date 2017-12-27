/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/ValueSet.cpp $
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
	ValueSet::ValueSet()
	{
	}

	ValueSet::ValueSet( const ValueSet& x ) :
		m_valueSet(x.m_valueSet)
	{
	}

	ValueSet::ValueSet( const std::set<std::string>& values ) :
		m_valueSet(values)
	{
	}

	ValueSet::ValueSet( const std::vector<std::string>& values )
	{
		std::vector<std::string>::const_iterator it;

		for( it = values.begin(); it != values.end(); it++ )
		{
			m_valueSet.insert( *it );
		}
	}

	bool ValueSet::insert( const std::string& newValue )
	{
		return( m_valueSet.insert( newValue ).second );
	}

	std::set<std::string>::size_type ValueSet::size() const
	{
		return( m_valueSet.size() );
	}

	bool ValueSet::operator==( const ValueSet& rhs ) const
	{
		TA_ASSERT( m_valueSet.size() > 0, "m_valueSet.size() <= 0" );
		TA_ASSERT( rhs.size() > 0 , "rhs.size() <= 0" );

		if( m_valueSet.size() == rhs.size() )
		{
			return( containedBy( rhs ) );
		}

		return( false );	
	}

	bool ValueSet::operator<=( const ValueSet& rhs ) const
	{
		TA_ASSERT( m_valueSet.size() > 0, "m_valueSet.size() <= 0" );
		TA_ASSERT( rhs.size() > 0, "rhs.size() <= 0" );

		if( m_valueSet.size() <= rhs.size() )
		{
			return( containedBy( rhs ) );
		}
		return( false );
	}
	
	bool ValueSet::operator!=( const ValueSet& rhs ) const
	{
		TA_ASSERT( m_valueSet.size() > 0, "m_valueSet.size() <= 0" );
		TA_ASSERT( rhs.size() > 0, "rhs.size() <= 0"  );

		if( *this == rhs )
		{
			return( false );
		}
		return( true );
	}

	bool ValueSet::containedBy( const ValueSet& rhs  ) const
	{
		TA_ASSERT( m_valueSet.size() > 0, "m_valueSet.size() <= 0" );
		TA_ASSERT( rhs.size() > 0, "rhs.size() <= 0" );

		ValueSetType::const_iterator lhsIt, rhsIt;

		lhsIt = m_valueSet.begin();

		bool match(true);

		while( match && m_valueSet.end() != lhsIt )
		{
			rhsIt = rhs.m_valueSet.find( *lhsIt );

			if( rhs.m_valueSet.end() == rhsIt )
			{
				match = false;
				continue;
			}

			lhsIt++;
		}

		return( match );
	}

	std::string ValueSet::dump() const
	{
		std::ostringstream rval;

		ValueSetType::const_iterator it;

		it = m_valueSet.begin();

		while( m_valueSet.end() != it )
		{
			rval << *it;
			it++;

			if( m_valueSet.end() != it )
			{
				rval << ", ";
			}
		}

		return( rval.str() );
	}

} // TA_Base_Bus

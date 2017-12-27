/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/AttributeSet.cpp $
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
	AttributeSet::AttributeSet( const std::string& id /*= ""*/, const std::string& category /*= ""*/ )
	{
		setId( id );
		setCategory( category );
	}
	
	AttributeSet::AttributeSet( const AttributeSet& x ) :
		m_attrib(x.m_attrib)
	{
	}

	bool AttributeSet::setId( const std::string& id )
	{
		if( id.size() > 0 )
		{
			return( addAttribute( PdpConst::ATTRIBUTE_ID, id ) );
		}

		return( false );
	}

	bool AttributeSet::setCategory( const std::string& category )
	{
		if( category.size() > 0 )
		{
			return( addAttribute( PdpConst::ATTRIBUTE_CATEGORY, category ) );
		}
		return( false );
	}

	bool AttributeSet::addAttribute( const std::string& name, const ValueSet& value )
	{
		TA_ASSERT( name.size()  > 0, "AttributeSet::addAttribute: name.size() <= 0" );

		if( value.size() <= 0 )
		{
			std::ostringstream msg;
	
			msg << name << " has no associated values";
			TA_THROW( TA_Base_Core::PolicyDecisionPointException(msg.str()) );
		}

		std::pair<AsMapIt,bool> result;

		result = m_attrib.insert( AsMapValueType( name, value ) );

		return( result.second );
	}

	bool AttributeSet::addAttribute( const std::string& name, const std::string& value )
	{
		TA_ASSERT( name.size()  > 0, "AttributeSet::addAttribute: name.size() <= 0" );

		if( value.size() <= 0 )
		{
			std::ostringstream msg;
	
			msg << name << " has no associated value";
			TA_THROW( TA_Base_Core::PolicyDecisionPointException(msg.str()) );
		}


		ValueSet tmpSet;
		
		if( tmpSet.insert( value ) )
		{
			return( m_attrib.insert( AsMapValueType( name, tmpSet ) ).second );
		}
		return( false );
	}

	PdpConst::EMatchResult AttributeSet::contains( const AttributeSet& rhs ) const
	{
		PdpConst::EMatchResult cr = PdpConst::MATCH_MR;

		if( rhs.m_attrib.size() == 0 )
		{
			return( PdpConst::INDETERMINATE_MR );
		}

		AsMapConstIt rhsIt;

		rhsIt = rhs.m_attrib.begin();

		while( (PdpConst::MATCH_MR == cr) && (rhs.m_attrib.end() != rhsIt) )
		{
			cr = privateContains( (*rhsIt).first, (*rhsIt).second );

			rhsIt++;
		}
		
		return( cr );
	}

	PdpConst::EMatchResult AttributeSet::contains( const std::string& name, const ValueSet& value ) const
	{
		TA_ASSERT( name.size()  > 0, "AttributeSet::contains: name.size() <= 0" );
		TA_ASSERT( value.size() > 0, "AttributeSet::contains: value.size() <= 0" );

		return( privateContains( name, value ) );
	}

	PdpConst::EMatchResult AttributeSet::contains( const std::string& name, const std::string& value ) const
	{
		TA_ASSERT( name.size()  > 0, "AttributeSet::contains: name.size() <= 0" );
		TA_ASSERT( value.size() > 0, "AttributeSet::contains: name.size() <= 0" );

		ValueSet tmpSet;
		
		if( tmpSet.insert( value ) )
		{
			return( privateContains( name, tmpSet ) );
		}

		return( PdpConst::INDETERMINATE_MR );
	}

	std::map<std::string,ValueSet>::size_type AttributeSet::size() const
	{
		return( m_attrib.size() );
	}

	PdpConst::EMatchResult AttributeSet::privateContains( const std::string& name, const ValueSet& value ) const
	{
		TA_ASSERT( name.size()  > 0, "AttributeSet::privateContains: name.size() <= 0" );
		TA_ASSERT( value.size() > 0, "AttributeSet::privateContains: value.size() <= 0" );

		AsMapConstIt lhsIt;

		//std::set<std::string>::const_iterator rhsSetIt;
		//std::set<std::string>::const_iterator lhsSetIt;

		PdpConst::EMatchResult cr = PdpConst::UNDEFINED_MR;

		lhsIt = m_attrib.find( name );

		if( m_attrib.end() == lhsIt  )
		{
			cr = PdpConst::INDETERMINATE_MR;
		}
		else
		{
			// from this point we can only return match or no match
			if( value <= lhsIt->second  )
			{
				cr = PdpConst::MATCH_MR;
			}
			else
			{
				cr = PdpConst::NO_MATCH_MR;
			}
		}

		TA_ASSERT( PdpConst::UNDEFINED_MR != cr, "cr == UNDEFINED_MR" );

		return( cr );
	}

	ValueSet AttributeSet::select( const std::string& name ) const
	{
		AsMapConstIt it;

		it = m_attrib.find( name );

		if( m_attrib.end() != it )
		{
			return( it->second );
		}

		// gcc 3.2.1 doesn't like in-place construction!
		ValueSet tmpVs;

		return( tmpVs );
	}

} // TA_Base_Bus

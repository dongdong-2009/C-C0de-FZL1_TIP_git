/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Context.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/02/02 19:02:10 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

// namespace alias for Corba structures/types

namespace TA_Base_Bus
{
	const std::string Context::SUBJECT_CATEGORY ("Context::access-subject");
	const std::string Context::ACTION_CATEGORY  ("Context::action");
	const std::string Context::RESOURCE_CATEGORY("Context::resource");

	Context::Context()
	{
		m_setMap.insert( SetMapValueType(SUBJECT_CATEGORY , AttributeSet()) );
		m_setMap.insert( SetMapValueType(ACTION_CATEGORY  , AttributeSet()) );
		m_setMap.insert( SetMapValueType(RESOURCE_CATEGORY, AttributeSet()) );
	}

	Context::Context( const Context &x ) :
		m_setMap(x.m_setMap)
	{
	}

	Context::Context( const std::string& subject, const std::string& resource,
		const std::string& action )
	{
		m_setMap.insert( SetMapValueType( SUBJECT_CATEGORY , AttributeSet(subject , SUBJECT_CATEGORY) ) );
		m_setMap.insert( SetMapValueType( ACTION_CATEGORY  , AttributeSet(action  ) ) );
		m_setMap.insert( SetMapValueType( RESOURCE_CATEGORY, AttributeSet(resource) ) );
	}

	Context::Context( const std::map<std::string,std::string>& subjects,
			const std::string& resource, const std::string& action )
	{
		std::map<std::string,std::string>::const_iterator it;

		for( it = subjects.begin(); it != subjects.end(); it++ )
		{
			m_setMap.insert( SetMapValueType(it->first, AttributeSet(it->first, it->second)) );
		}
		m_setMap.insert( SetMapValueType( ACTION_CATEGORY  , AttributeSet(action  ) ) );
		m_setMap.insert( SetMapValueType( RESOURCE_CATEGORY, AttributeSet(resource) ) );
	}

	void Context::setSubjectId ( const std::string& id )
	{
		setId( SUBJECT_CATEGORY, id );
	}

	void Context::setActionId  ( const std::string& id )
	{
		setId( ACTION_CATEGORY, id );
	}

	void Context::setResourceId( const std::string& id )
	{
		setId( RESOURCE_CATEGORY, id );
	}
	
	void Context::addSubjectAttribute( const std::string& attribName, const std::string& attribValue,
		const std::string& category /*= Context::DEFAULT_CATEGORY*/ )
	{
		ValueSet valueSet;
		valueSet.insert( attribValue );

		addAttribute( category, attribName, valueSet );
	}

	void Context::addActionAttribute  ( const std::string& attribName, const std::string& attribValue )
	{
		ValueSet valueSet;
		valueSet.insert( attribValue );

		addAttribute( ACTION_CATEGORY, attribName, valueSet );
	}

	void Context::addResourceAttribute( const std::string& attribName, const std::string& attribValue )
	{
		ValueSet valueSet;
		valueSet.insert( attribValue );

		addAttribute( RESOURCE_CATEGORY, attribName, valueSet );
	}
	
	void Context::addSubjectAttribute( const std::string& attribName, const ValueSet& attribValue,
			const std::string& category /*= Context::SUBJECT_CATEGORY*/ )
	{
		addAttribute( category, attribName, attribValue );
	}

	void Context::addActionAttribute( const std::string& attribName, const ValueSet& attribValue )
	{
		addAttribute( ACTION_CATEGORY, attribName, attribValue );
	}

	void Context::addResourceAttribute( const std::string& attribName, const ValueSet& attribValue )
	{
		addAttribute( RESOURCE_CATEGORY, attribName, attribValue );
	}

	PdpConst::EMatchResult Context::contains( const Context& rhs ) const
	{
		SetMapConstIt lhsIt;
		SetMapConstIt rhsIt;

		PdpConst::EMatchResult mr = PdpConst::MATCH_MR;

		rhsIt = rhs.m_setMap.begin();

		while( (PdpConst::MATCH_MR == mr) && (rhs.m_setMap.end() != rhsIt)  )
		{
			// don't match empty attributes
			if( rhsIt->second.size() > 0 )
			{
				lhsIt = m_setMap.find( rhsIt->first );
				
				if( lhsIt == m_setMap.end() )
				{
					mr = PdpConst::INDETERMINATE_MR;
					continue;
				}

				mr = lhsIt->second.contains( rhsIt->second );
			}
			rhsIt++;
		}

		return( mr );
	}

	void Context::addAttribute( const std::string& category, const std::string& attribName,
			const ValueSet& attribValue )
	{
		SetMapIt it;

		it = m_setMap.find( category );

		if( it != m_setMap.end() )
		{
			it->second.addAttribute( attribName, attribValue );
		}
	}

	void Context::setId( const std::string& category, const std::string& id )
	{
		SetMapIt it;

		it = m_setMap.find( category );

		if( m_setMap.end() != it )
		{
			it->second.setId( id );
		}
	}

	bool Context::isEmpty() const
	{
		SetMapConstIt it;
		bool empty = true;

		it = m_setMap.begin();

		while( empty && it != m_setMap.end() )
		{
			if( 0 != it->second.size() )
			{
				empty = false;
				break;
			}
			it++;
		}

		return( empty );
	}

	ValueSet Context::select( const SubjectAttributeSelector& as ) const
	{
		if( 0 == as.m_category.size() )
		{
			return( select( SUBJECT_CATEGORY, as.m_attribName ) );
		}

		return( select( as.m_category, as.m_attribName ) );
	}

	ValueSet Context::select( const ActionAttributeSelector& as ) const
	{
		return( select( ACTION_CATEGORY, as.m_attribName ) );
	}

	ValueSet Context::select( const ResourceAttributeSelector& as ) const
	{
		return( select( RESOURCE_CATEGORY, as.m_attribName ) );
	}

	ValueSet Context::select( const std::string& category, const std::string& attribName ) const
	{
		SetMapConstIt it;

		it = m_setMap.find( category );

		if( m_setMap.end() != it )
		{
			return( it->second.select(attribName) );
		}

		ValueSet tmpVs;

		return( tmpVs );
	}


} // TA_Base_Bus

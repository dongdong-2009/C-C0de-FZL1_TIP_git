#ifndef ATTRIBUTESET_H_INCLUDED
#define ATTRIBUTESET_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/AttributeSet.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Supporting class for Context. This implements all the stl::map logic to make
  *  context attributes behave as defined in:
  *
  *  http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class AttributeSet
	{
	public:

		AttributeSet( const std::string& id = "", const std::string& category = "" );
		AttributeSet( const AttributeSet &x );

		bool setId      ( const std::string& id       );
		bool setCategory( const std::string& category );

		bool addAttribute( const std::string& name, const ValueSet& value );
		bool addAttribute( const std::string& name, const std::string& value );

		// comparisons
		PdpConst::EMatchResult contains( const AttributeSet& rhs ) const;
		PdpConst::EMatchResult contains( const std::string& name, const std::string& value ) const;
		PdpConst::EMatchResult contains( const std::string& name, const ValueSet& value ) const;
		
		std::map<std::string,ValueSet>::size_type size() const;

		ValueSet select( const std::string& name ) const;

	private:
		
		
		AttributeSet& operator=( const AttributeSet &x );

		typedef std::map<std::string,ValueSet > AsMap;
		typedef AsMap::iterator AsMapIt;
		typedef AsMap::const_iterator AsMapConstIt;
		typedef AsMap::value_type AsMapValueType;

		PdpConst::EMatchResult privateContains( const std::string& name, const ValueSet& value ) const;

		AsMap m_attrib;

	}; // AttributeSet
}; // TA_Base_Bus

#endif // ATTRIBUTESET_H_INCLUDED

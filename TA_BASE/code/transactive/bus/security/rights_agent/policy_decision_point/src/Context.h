#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Context.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Context
	{
	public:

		Context();
		Context( const Context &x );

		Context( const std::string& subject, const std::string& resource,
			const std::string& action );

		Context( const std::map<std::string,std::string>& subjects,
			const std::string& resource, const std::string& action );

		// can only set the default subjects id
		void setSubjectId ( const std::string& id );
		void setActionId  ( const std::string& id );
		void setResourceId( const std::string& id );

		void addSubjectAttribute( const std::string& attribName, const std::string& attribValue,
			const std::string& category = Context::SUBJECT_CATEGORY );

		void addActionAttribute  ( const std::string& attribName, const std::string& attribValue );
		void addResourceAttribute( const std::string& attribName, const std::string& attribValue );

		void addSubjectAttribute( const std::string& attribName, const ValueSet& attribValue,
			const std::string& category = Context::SUBJECT_CATEGORY );

		void addActionAttribute  ( const std::string& attribName, const ValueSet& attribValue );
		void addResourceAttribute( const std::string& attribName, const ValueSet& attribValue );

		PdpConst::EMatchResult contains( const Context& target ) const;

		ValueSet select( const SubjectAttributeSelector& as ) const;
		ValueSet select( const ActionAttributeSelector& as ) const;
		ValueSet select( const ResourceAttributeSelector& as ) const;

		bool isEmpty() const;

	private:

		
		Context& operator=( const Context &x );

		static const std::string SUBJECT_CATEGORY ;
		static const std::string ACTION_CATEGORY  ;
		static const std::string RESOURCE_CATEGORY;

		typedef std::map<std::string,AttributeSet> SetMap;
		typedef SetMap::iterator SetMapIt;
		typedef SetMap::const_iterator SetMapConstIt;
		typedef SetMap::value_type SetMapValueType;

		void addAttribute( const std::string& category, const std::string& attribName,
			const ValueSet& attribValue );

		void setId( const std::string& category, const std::string& id );

		ValueSet select( const std::string& category, const std::string& attribName ) const;

		SetMap m_setMap;

	}; // Context
}; // TA_Base_Bus

#endif // CONTEXT_H_INCLUDED

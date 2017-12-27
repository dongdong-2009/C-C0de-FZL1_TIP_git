#ifndef TARGET_H_INCLUDED
#define TARGET_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/Target.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  *
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Target
	{
	public:

		Target(){}
		~Target(){}

		void addElement( const SubjectAttributeSelector* as1,
				const ConstAttributeSelector* as2,
				PdpConst::EComparisonType cmpType );
		void addElement( const ActionAttributeSelector* as1,
				const ConstAttributeSelector* as2,
				PdpConst::EComparisonType cmpType );
		void addElement( const ResourceAttributeSelector* as1,
				const ConstAttributeSelector* as2,
				PdpConst::EComparisonType cmpType );

		PdpConst::EMatchResult evaluate( const Context& context ) const;
	
		std::list<const CompareFunction*>::size_type size() const;

	private:

		Target( const Target &x );
		Target& operator=( const Target &x );

		typedef std::list<const CompareFunction*> ElementList;

		ElementList m_elementList;

	}; // Target
}; // TA_Base_Bus

#endif // TARGET_H_INCLUDED

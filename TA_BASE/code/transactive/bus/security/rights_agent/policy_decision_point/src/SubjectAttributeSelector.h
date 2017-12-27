#ifndef SUBJECTATTRIBUTESELECTOR_H_INCLUDED
#define SUBJECTATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/SubjectAttributeSelector.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  */

namespace TA_Base_Bus
{
	//Class declaration
	class SubjectAttributeSelector : public Function
	{
	public:

		friend class Context;

		SubjectAttributeSelector( const std::string& attribName, const std::string& category = "" );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		SubjectAttributeSelector( const SubjectAttributeSelector &x );
		SubjectAttributeSelector& operator=( const SubjectAttributeSelector &x );

		std::string m_attribName;
		std::string m_category;
		
	}; // SubjectAttributeSelector
}; // TA_Base_Bus

#endif // SUBJECTATTRIBUTESELECTOR_H_INCLUDED

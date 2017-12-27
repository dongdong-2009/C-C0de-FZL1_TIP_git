#ifndef RESOURCEATTRIBUTESELECTOR_H_INCLUDED
#define RESOURCEATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/ResourceAttributeSelector.h $
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
	class ResourceAttributeSelector : public Function
	{
	public:

		friend class Context;

		ResourceAttributeSelector( const std::string& attribName );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		ResourceAttributeSelector( const ResourceAttributeSelector &x );
		ResourceAttributeSelector& operator=( const ResourceAttributeSelector &x );

		std::string m_attribName;

	}; // ResourceAttributeSelector
}; // TA_Base_Bus

#endif // RESOURCEATTRIBUTESELECTOR_H_INCLUDED

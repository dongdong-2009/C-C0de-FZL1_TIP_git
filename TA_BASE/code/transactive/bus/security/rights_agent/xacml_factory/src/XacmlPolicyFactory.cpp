/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/xacml_factory/src/XacmlPolicyFactory.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * This class implements the IRightsAgentCorbaDef interface.
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{
	std::auto_ptr<PolicySet> XacmlPolicyFactory::build() const
	{
		std::list<const Policy*> policyList;

		// For C830 we have a rights, duty and default policy.
		// The rights policy is used for logical resources, the
		// duty policy is used for physical resources and the
		// default policy denies every request. The default
		// policy is only consulted if the first two are found
		// to be not applicable or indeterminate.
		policyList.push_back( buildPolicy( DISPLAY_ONLY_RIGHTS_POLICY      ) );
		policyList.push_back( buildPolicy( DISPLAY_ONLY_DUTY_POLICY        ) );
		policyList.push_back( buildPolicy( RIGHTS_POLICY      ) );
		policyList.push_back( buildPolicy( DUTY_POLICY        ) );
		policyList.push_back( buildPolicy( DEFAULT_POLICY     ) );

		std::auto_ptr<PolicySet> policySet( new PolicySet( policyList ) );

		return( policySet );
	}

	const Rule* XacmlPolicyFactory::buildRule( XacmlPolicyFactory::ERule rule ) const
	{

		switch( rule )
		{
			case PERMIT_VALID_RIGHTS_RULE:
			{
				std::auto_ptr<const Function> as1(new ActionAttributeSelector (RightsConst::RIGHTS));
				std::auto_ptr<const Function> as2(new SubjectAttributeSelector(RightsConst::RIGHTS));
				
				std::auto_ptr<const Function> cmp(new CompareFunction(as2.release(),
					as1.release(), PdpConst::IS_LESSTHAN_OR_EQUAL_CT));
				
				std::auto_ptr<const Condition> cnd(new Condition(cmp.release()));


				return( new Rule("PERMIT_VALID_RIGHTS_RULE",
								 NULL,
								 cnd.release(),
								 PdpConst::PERMIT_PR) );
			}
			break;
			
			case PERMIT_VALID_DUTY_RULE:
			{
				std::auto_ptr<const Function> as1(new ActionAttributeSelector (RightsConst::RIGHTS));
				std::auto_ptr<const Function> as2(new SubjectAttributeSelector(RightsConst::DUTY));
				std::auto_ptr<const Function> cmp(new CompareFunction(as2.release(),
					as1.release(), PdpConst::IS_LESSTHAN_OR_EQUAL_CT));
				
				std::auto_ptr<const Condition> cnd(new Condition(cmp.release()));


				return( new Rule("PERMIT_VALID_DUTY_RULE",
								 NULL,
								 cnd.release(),
								 PdpConst::PERMIT_PR) );
			}
			break;

			case PERMIT_VALID_DISPLAY_ONLY_RULE:
			{
				std::auto_ptr<const Function> as1(new ActionAttributeSelector (RightsConst::DISPLAY_ONLY));
				std::auto_ptr<const Function> as2(new ConstAttributeSelector(RightsConst::FALSE_S));
				std::auto_ptr<const Function> cmp(new CompareFunction(as2.release(),
					as1.release(), PdpConst::IS_EQUAL_CT));
				
				std::auto_ptr<const Condition> cnd(new Condition(cmp.release()));


				return( new Rule("PERMIT_VALID_DISPLAY_ONLY_RULE",
								 NULL,
								 cnd.release(),
								 PdpConst::DENY_PR) );
			}
			break;
			case DENY_RULE:
			{
				
				return( new Rule("DENY_RULE",
								 NULL,
								 NULL,
								 PdpConst::DENY_PR) );
			}
			break;

			default:
				std::ostringstream msg;
				msg << "rule == " << rule;
				TA_ASSERT( false, msg.str().c_str() );
				return NULL;
				break;
		}
	}

	const Policy* XacmlPolicyFactory::buildPolicy( XacmlPolicyFactory::EPolicy policy ) const
	{
		switch( policy )
		{
			// for logical resources
			case RIGHTS_POLICY:
			{
				Target* newTarget = new Target;

				std::auto_ptr<const ResourceAttributeSelector>
					resourceType(new ResourceAttributeSelector(RightsConst::RESOURCE_TYPE));
				
				std::auto_ptr<const ConstAttributeSelector>
					 logical(new ConstAttributeSelector(RightsConst::LOGICAL));

				newTarget->addElement( resourceType.release(), logical.release(), PdpConst::IS_EQUAL_CT );

				std::list<const Rule*> ruleList;

				ruleList.push_back( buildRule( PERMIT_VALID_RIGHTS_RULE ) );

				return( new Policy(newTarget, ruleList) );
			}
			break;

			// for display only logical resources
			case DISPLAY_ONLY_RIGHTS_POLICY:
			{
				Target* newTarget = new Target;

				std::auto_ptr<const ResourceAttributeSelector>
					resourceType(new ResourceAttributeSelector(RightsConst::RESOURCE_TYPE));
				
				std::auto_ptr<const ConstAttributeSelector>
					logical(new ConstAttributeSelector(RightsConst::DISPLAY_ONLY_LOGICAL));

				newTarget->addElement( resourceType.release(), logical.release(), PdpConst::IS_EQUAL_CT );

				std::list<const Rule*> ruleList;
				ruleList.push_back( buildRule( PERMIT_VALID_DISPLAY_ONLY_RULE ) );
				ruleList.push_back( buildRule( PERMIT_VALID_RIGHTS_RULE ) );

				return( new Policy(newTarget, ruleList) );
			}
			break;
			// for physical resources
			case DUTY_POLICY:
			{
				Target* newTarget = new Target;

				std::auto_ptr<const ResourceAttributeSelector>
					resourceType(new ResourceAttributeSelector(RightsConst::RESOURCE_TYPE));
				
				std::auto_ptr<const ConstAttributeSelector>
					 physical(new ConstAttributeSelector(RightsConst::PHYSICAL));

				newTarget->addElement( resourceType.release(), physical.release(), PdpConst::IS_EQUAL_CT );

				std::list<const Rule*> ruleList;

				ruleList.push_back( buildRule( PERMIT_VALID_DUTY_RULE   ) );

				return( new Policy(newTarget, ruleList,true) );
			}
			break;
			// for display only physical resources
			case DISPLAY_ONLY_DUTY_POLICY:
			{
				Target* newTarget = new Target;

				std::auto_ptr<const ResourceAttributeSelector>
					resourceType(new ResourceAttributeSelector(RightsConst::RESOURCE_TYPE));
				
				std::auto_ptr<const ConstAttributeSelector>
					physical(new ConstAttributeSelector(RightsConst::DISPLAY_ONLY_PHYSICAL));

				newTarget->addElement( resourceType.release(), physical.release(), PdpConst::IS_EQUAL_CT );

				std::list<const Rule*> ruleList;
				ruleList.push_back( buildRule( PERMIT_VALID_DISPLAY_ONLY_RULE   ) );
				ruleList.push_back( buildRule( PERMIT_VALID_DUTY_RULE   ) );

				return( new Policy(newTarget, ruleList,true) );
			}
			break;
			case DEFAULT_POLICY:
			{
				std::list<const Rule*> ruleList;

				ruleList.push_back( buildRule( DENY_RULE ) );

				return( new Policy(NULL, ruleList) );
			}
			break;

			default:
				std::ostringstream msg;
				msg << "policy == " << policy;
				TA_ASSERT( false, msg.str().c_str() );
				return NULL;
				break;
		}
	}
			
}

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyPolicy.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Applies the Duty Policy to the context provided.
 * 
 */

#if !defined(IDUTYPOLICY_H_INCLUDED)
#define IDUTYPOLICY_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyContext;

	class IDutyPolicy
	{
	public:

		virtual ~IDutyPolicy(){}
			/*
		 * All possible results from evaluating the policy
		 */
		enum POLICY_RESULT
		{
			PR_ADD    ,
			PR_REPLACE,
			PR_DENY   ,
			PR_ADDONLY,
			PR_REPONLY,
			PR_REMOVE,
		};

		/*
		 * Types of profiles
		 */
		enum PROFILE_TYPE
		{
			PT_CENTRAL,
			PT_LOCAL,
			PT_UNDEFINED
		};

		/*
		 * For attributes that are compared (e.g. profile groups)
		 */
		enum EQUALITY
		{
			EQ_EQ ,
			EQ_NEQ,
			EQ_NA,
			EQ_UNDEFINED
		};

		/*
		 * Used for rights AND duty
		 */
		enum RIGHTS_TYPE
		{
			RT_EXCLUSIVE,
			RT_ADMISSIVE,
			RT_NONE,
			RT_UNDEFINED
		};

		/*
		 * Types of subsystem (admissive is the opposite of exclusive)
		 */
		enum SUBSYSTEM_TYPE
		{
			ST_EXCLUSIVE,
			ST_ADMISSIVE,
			ST_UNDEFINED
		};

		virtual POLICY_RESULT evaluateAllocRegionPolicy     ( const DutyContext& context ) const = 0;
		virtual POLICY_RESULT evaluateAllocSubPolicy        ( const DutyContext& context ) const = 0;
		virtual POLICY_RESULT evaluateXferRegionPolicy      ( const DutyContext& context ) const = 0;
		virtual POLICY_RESULT evaluateXferSubViaRegionPolicy( const DutyContext& context ) const = 0;
		virtual POLICY_RESULT evaluateXferSubPolicy         ( const DutyContext& context ) const = 0;
	};
}







#endif // !defined(IDUTYPOLICY_H_INCLUDED)

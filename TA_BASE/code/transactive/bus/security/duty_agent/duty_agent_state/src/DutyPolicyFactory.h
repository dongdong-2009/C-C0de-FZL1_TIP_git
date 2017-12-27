/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPolicyFactory.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Applies the Duty Policy to the context provided.
 * This is static for efficiency reasons but does not need to be thread-safe
 * as all varables are local.
 */

#if !defined(DUTYPOLICYFACTORY_H_INCLUDED)
#define DUTYPOLICYFACTORY_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyContext;

	class DutyPolicyFactoryImpl
	{

	public:

		friend class ACE_Singleton<DutyPolicyFactoryImpl, ACE_Recursive_Thread_Mutex>;
 
		const IDutyPolicy& getDutyPolicy() const;

	private:

		class RailDutyPolicy : public IDutyPolicy
		{
		public:

			virtual IDutyPolicy::POLICY_RESULT evaluateAllocRegionPolicy     ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateAllocSubPolicy        ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferRegionPolicy      ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferSubViaRegionPolicy( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferSubPolicy         ( const DutyContext& context ) const;

		private:
			// Degrees of Freedom:
			// Dst Profile Type, Regions, Profile Groups
			const static IDutyPolicy::POLICY_RESULT m_allocRegionPolicy[2][3][3];

			// Degrees of Freedom:
			// Dst Rights, Subsystem Type, Src Duty
			const static IDutyPolicy::POLICY_RESULT m_allocSubPolicy[3][2][3];

			// Degrees of Freedom:
			// Profile Groups
			const static IDutyPolicy::POLICY_RESULT m_xferRegionPolicy[3];

			// Degrees of Freedom:
			// Dst Rights, Subsystem Type, Src Duty, Profile Groups
			const static IDutyPolicy::POLICY_RESULT m_xferSubViaRegionPolicy[3][2][3][3];

			// Degrees of Freedom:
			// Dst Rights (after), Subsystem Type, Src Duty, Src Rights (after)
			const static IDutyPolicy::POLICY_RESULT m_xferSubPolicy[3][2][3][3];
		};

		class RoadDutyPolicy : public IDutyPolicy
		{
		public:

			virtual IDutyPolicy::POLICY_RESULT evaluateAllocRegionPolicy     ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateAllocSubPolicy        ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferRegionPolicy      ( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferSubViaRegionPolicy( const DutyContext& context ) const;
			virtual IDutyPolicy::POLICY_RESULT evaluateXferSubPolicy         ( const DutyContext& context ) const;

		private:
			// Degrees of Freedom:
			// Dst Profile Type, Regions
			const static IDutyPolicy::POLICY_RESULT m_allocRegionPolicy[2][3];

			// Degrees of Freedom:
			// Dst Rights, Subsystem Type, Src Duty
			const static IDutyPolicy::POLICY_RESULT m_allocSubPolicy[3][2][3];

			// Degrees of Freedom:
			// None
			const static IDutyPolicy::POLICY_RESULT m_xferRegionPolicy[1];

			// Degrees of Freedom:
			// Dst Rights, Subsystem Type, Src Duty
			const static IDutyPolicy::POLICY_RESULT m_xferSubViaRegionPolicy[3][2][3];

			// Degrees of Freedom:
			// Dst Rights (after), Subsystem Type, Src Duty, Src Rights (after)
			const static IDutyPolicy::POLICY_RESULT m_xferSubPolicy[3][2][3][3];
		};

		// Only we can construct and destruct ourselves
		DutyPolicyFactoryImpl();
		~DutyPolicyFactoryImpl();

		// Don't want to copy a Singleton
		DutyPolicyFactoryImpl( const DutyPolicyFactoryImpl& );
		DutyPolicyFactoryImpl& operator=( const DutyPolicyFactoryImpl& );

		std::vector<IDutyPolicy*> m_dutyPolicy;
	};

	typedef ACE_Singleton<DutyPolicyFactoryImpl, ACE_Recursive_Thread_Mutex> DutyPolicyFactory;
}







#endif // !defined(DUTYPOLICYFACTORY_H_INCLUDED)

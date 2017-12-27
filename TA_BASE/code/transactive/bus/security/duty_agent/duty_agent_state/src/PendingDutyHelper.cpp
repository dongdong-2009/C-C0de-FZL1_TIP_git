/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/PendingDutyHelper.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  * Regions and Subsystems both support the concept of two-stage duty change where a duty change is
  * validated by the Duty Agent's policies, verified externally then committed. This class encapsulates
  * the logic required.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	PendingDutyHelper::PendingDutyHelper()
	{
	}
	
	PendingDutyHelper::PendingDutyHelper( const DASType::DutySet& duty ) :
		m_duty(duty)
	{
	}

	void PendingDutyHelper::processPolicyResult( const DutyPrimitive& dstSession, const DutyPrimitive& srcSession,
			const IDutyPolicy::POLICY_RESULT& policyResult )
	{
		switch(policyResult)
		{
			case IDutyPolicy::PR_ADD: // no break
			case IDutyPolicy::PR_ADDONLY:
			{
				m_pendingAdd.insert(dstSession);
			}
			break;

			case IDutyPolicy::PR_REPLACE: // no break
			case IDutyPolicy::PR_REPONLY:
			{
				m_pendingAdd.insert(dstSession);
				m_pendingRem.insert(srcSession);
			}
			break;
			
			case IDutyPolicy::PR_REMOVE:
			{
				m_pendingRem.insert(srcSession);
			}
			break;

			case IDutyPolicy::PR_DENY:
			{
				return;
			}
			break;

			default:
			{
				std::ostringstream msg;

				msg << "Invalid policy result: " << policyResult;
				TA_ASSERT(false, msg.str().c_str() );
			}
			break;
		}
	}

	void PendingDutyHelper::processPolicyResult( const DutyPrimitive& dstSession,
		const IDutyPolicy::POLICY_RESULT& policyResult )
	{
		switch(policyResult)
		{
			case IDutyPolicy::PR_ADD: // no break
			case IDutyPolicy::PR_ADDONLY:
			{
				m_pendingAdd.insert(dstSession);
			}
			break;

			case IDutyPolicy::PR_DENY:
			{
				return;
			}
			break;

			case IDutyPolicy::PR_REPLACE: // no break
			case IDutyPolicy::PR_REPONLY: // no break
			case IDutyPolicy::PR_REMOVE:  // no break
			default:
			{
				std::ostringstream msg;

				msg << "Invalid policy result: " << policyResult;
				TA_ASSERT(false, msg.str().c_str() );
			}
			break;
		}
	}

	const DASType::DutySet& PendingDutyHelper::getDuty() const
	{
		return( m_duty );
	}

	bool PendingDutyHelper::hasDuty(const DutyPrimitive& session) const
	{
		bool result; 
		result = (m_duty.find(session) != m_duty.end());
		return result;
	}
	void PendingDutyHelper::add( const DutyPrimitive& duty )
	{
		m_pendingAdd.insert(duty);
	}
	
	void PendingDutyHelper::rem( const DutyPrimitive& duty )
	{
		m_pendingRem.insert(duty);
	}

	bool PendingDutyHelper::apply()
	{
		bool didSomething = false;

		// erase must be first in case there is a duplicate between m_pendingAdd and m_duty
		DASType::DutySet::const_iterator it;

		for( it = m_pendingRem.begin(); it != m_pendingRem.end(); it++ )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Removing %s(%lu)", it->m_session.c_str(), it->m_profile );
			m_duty.erase(*it);
			didSomething = true;
		}

		for( it = m_pendingAdd.begin(); it != m_pendingAdd.end(); it++ )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Adding %s(%lu)", it->m_session.c_str(), it->m_profile );
			m_duty.insert(*it);
			didSomething = true;
		}

		cancel();

		return( didSomething );
	}

	void PendingDutyHelper::cancel()
	{
		m_pendingAdd.clear();
		m_pendingRem.clear();
	}
}

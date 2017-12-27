/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PercentageNetworkFailureAlarmPolicy.cpp $
  * @author:   Teik Oh
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * PercentageNetworkFailureAlarmPolicy class implimentation. See header file for class descrption
  */

#include "PercentageNetworkFailureAlarmPolicy.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{

	PercentageNetworkFailureAlarmPolicy::PercentageNetworkFailureAlarmPolicy(unsigned int numFails, unsigned int setSize) :
			m_numFails(numFails),
			m_setSize(setSize),
			m_cursor(0)
	{
		
		FUNCTION_ENTRY("PercentageNetworkFailureAlarmPolicy");

		if (m_setSize < 1)
		{
			m_setSize = 1;
		}
		else if (m_setSize > MAX_WORKING_SET)
		{
			m_setSize = MAX_WORKING_SET;
		}

		if (m_numFails < 1)
		{
			m_numFails = 1;
		}
		else if (m_numFails > m_setSize)
		{
			m_numFails = m_setSize;
		}

		for (unsigned int i = 0; i < m_setSize; i++)
		{
			m_workingSet[i] = true;
		}

		FUNCTION_EXIT;
	}

	 
	void PercentageNetworkFailureAlarmPolicy::initPolicyState(bool state)
	{
		FUNCTION_ENTRY("initPolicyState");

		for (unsigned int i = 0; i < m_setSize; i++)
		{
			m_workingSet[i] = state;
		}

		m_cursor = 0;

		FUNCTION_EXIT;
	}

	 
	void PercentageNetworkFailureAlarmPolicy::addResult(bool pingSuccess)
	{
		FUNCTION_ENTRY("addResult");

		m_workingSet[m_cursor] = pingSuccess;

		m_cursor++;
		if (m_cursor >= m_setSize)
		{
			m_cursor = 0;
		}

		FUNCTION_EXIT;
	}

	
	bool PercentageNetworkFailureAlarmPolicy::pointIsPingable()
	{
		FUNCTION_ENTRY("pointIsPingable");

		unsigned int failCount = 0;

		for (unsigned int i = 0; i < m_setSize; i++)
		{
			if (!m_workingSet)
				failCount++;
		}

		FUNCTION_EXIT;

		return failCount < m_numFails;	
	}

} //end namespace TA_Base_App

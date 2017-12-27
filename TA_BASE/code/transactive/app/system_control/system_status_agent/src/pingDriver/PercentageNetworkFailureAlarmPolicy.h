/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PercentageNetworkFailureAlarmPolicy.h $
  * @author:  Teik Oh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This policy allows for a number of failures (out of a specified working set)
  * before indicating "down".  The working set is a sliding window.  Thus, if
  * there are a lot of failures it may take a good run of successes before
  * pointIsPingable() returns true.
  *
  * The setSize should be a reasonably large number to make any statistical
  * sense.
  *
  * WARNING: THIS CODE HAS NOT BEEN MAINTAINED IN THE SYSTEM STATUS AGENT PROJECT
  * IF YOU PLAN TO USE IT YOU ARE GOING TO HAVE TO FIX IT 
  *
  */


#ifndef PERCENTAGE_NETWORK_FAILURE_ALARM_POLICY_H
#define PERCENTAGE_NETWORK_FAILURE_ALARM_POLICY_H

#include "INetworkFailureAlarmPolicy.h"

namespace TA_Base_App
{
    class PercentageNetworkFailureAlarmPolicy : public INetworkFailureAlarmPolicy
    {
    public:
		PercentageNetworkFailureAlarmPolicy(unsigned int numFails, unsigned int setSize);
		
		void initPolicyState(bool state);
		void addResult(bool pingSuccess);
		bool pointIsPingable();

	private:
		enum Limits
		{
			MAX_WORKING_SET = 100
		};

		unsigned int m_numFails;
		unsigned int m_setSize;
		unsigned int m_workingSet[MAX_WORKING_SET];
		unsigned int m_cursor;
	};
}

#endif

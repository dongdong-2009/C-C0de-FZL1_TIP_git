/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/INetworkFailureAlarmPolicy.h $
  * @author:  Teik Oh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  * The policy decides how and when alarms are raised when there is a network
  * failure.  The policy is settable via RunParams command line parameters.
  *
  * For convenience, if not good design (yes, I have heard of abstract
  * factories - you wanna go write one for me?), there is a ConstructFromRunParams
  * static method.
  *
  * There are 2 descended policy classes:
  *   ConsecutiveNetworkFailureAlarmPolicy and
  *   PercentageNetworkFailureAlarmPolicy.
  *
  * To use a policy, start by init'ing it to either up or down using
  * InitPolicyState(bool).  Next, feed it with addResult() and poll it
  * with pointIsPingable() to determine what state the policy considers the network
  * to be in.
  */


#ifndef INETWORK_FAILURE_ALARM_POLICY_H
#define INETWORK_FAILURE_ALARM_POLICY_H

class INetworkFailureAlarmPolicy
{
public:
	virtual ~INetworkFailureAlarmPolicy() {};


    /** 
      * initPolicyState
      *
      * set state: true =point Is Pingable, false = point Is NOT Pingable
      *
      * @param bool state
      */		
	virtual void initPolicyState(bool state) = 0;

    /** 
      * addResult
      *
      * add the next ping result
      *
      * @param bool pingSuccess
      */
	virtual void addResult(bool pingSuccess) = 0;

    /** 
      * pointIsPingable
      *
      * based on the sequences of inputs applied via addResult(), are
	  * we pingable or not?
      *
	  *
	  * 
      * @return bool 
      *
      */
	virtual bool pointIsPingable() = 0;
};


#endif //#ifndef INETWORK_FAILURE_ALARM_POLICY_H

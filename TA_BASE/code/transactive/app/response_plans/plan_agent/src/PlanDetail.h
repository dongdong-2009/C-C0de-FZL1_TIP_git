#ifndef PLANDETAIL_H_INCLUDED
#define PLANDETAIL_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDetail.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/23 16:58:54 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <string>

#include "ace/Time_Value.h"
#include "ace/Future.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{

typedef std::map<unsigned long, TA_Base_Core::ActiveStepDetail> ExeHistoryMap;
typedef std::map<unsigned long, TA_Base_Core::ActiveStepDetail>::iterator ExeHistoryMapIterator;
typedef std::map<unsigned long, TA_Base_Core::ActiveStepDetail>::const_iterator ConstExeHistoryMapIterator;

typedef std::map<unsigned long, TA_Base_Core::StepCustomisableDetail> CustHistoryMap;
typedef std::map<unsigned long, TA_Base_Core::StepCustomisableDetail>::iterator CustHistoryMapIterator;
typedef std::map<unsigned long, TA_Base_Core::StepCustomisableDetail>::const_iterator ConstCustHistoryMapIterator;

class PlanDetail
{
public:
    PlanDetail( const PlanDetail& );
	PlanDetail( const std::string& session, const std::string& pmId, const unsigned long userKey, const unsigned long updateId,
        const std::string& owner, const TA_Base_Core::ActivePlanId& refActivePlan, const int initialState, 
        const std::string& remark, const ACE_Future<int>& result , const bool manExecuted, const std::string& assocAlarmId );
	PlanDetail( const TA_Base_Core::ActivePlanDetail& detail );

	bool set( const int state );

	TA_Base_Core::ActivePlanDetail m_detail;
	//note m_detail.timeout is reset for each step in a plan

	std::string m_session;
    std::string m_pmId;
    //alarm ID of the alarm raised by this Plan
    std::string m_alarmID;

	unsigned long m_userKey;

	ExeHistoryMap m_exeHistoryMap;
	CustHistoryMap m_custHistoryMap;

	time_t  m_lastModified;
	ACE_Future<int> m_result;

	bool isNonActive() const;
    bool isActive () const;

private:
    PlanDetail();
	PlanDetail& operator=( const PlanDetail& );
};

} // TA_Base_App

#endif //PLANDETAIL_H_INCLUDED

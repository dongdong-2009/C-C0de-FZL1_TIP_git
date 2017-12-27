/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDetail.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/23 16:58:54 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "ace/OS.h"

#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"


namespace TA_Base_App
{

PlanDetail::PlanDetail( const PlanDetail& rhs ) :
m_detail( rhs.m_detail ),
m_session( rhs.m_session.c_str() ),
m_pmId( rhs.m_pmId.c_str() ),
m_alarmID( rhs.m_alarmID.c_str() ),
m_userKey( rhs.m_userKey ),
m_exeHistoryMap( rhs.m_exeHistoryMap ),
m_custHistoryMap( rhs.m_custHistoryMap ),
m_lastModified( rhs.m_lastModified ),
m_result( rhs.m_result )
{
}


PlanDetail::PlanDetail( const std::string& session, const std::string& pmId, const unsigned long userKey, const unsigned long updateId,
	const std::string& owner, const TA_Base_Core::ActivePlanId& refActivePlan, const int initialState, const std::string& remark, 
    const ACE_Future<int>& result , const bool manExec, const std::string& assocAlarmId ) :
m_detail(),
m_session( session.c_str() ),
m_pmId( pmId.c_str() ),
m_alarmID( "" ),
m_userKey( userKey ),
m_exeHistoryMap(),
m_custHistoryMap(),
m_lastModified( ACE_OS::time() ),
m_result( result )
{
    m_detail.plan.plan = refActivePlan.plan;
    m_detail.plan.instance = refActivePlan.instance;
    m_detail.plan.location = refActivePlan.location;
    m_detail.plan.triggerEntity = refActivePlan.triggerEntity;
    m_detail.plan.entitykey = refActivePlan.entitykey;
    m_detail.path = TA_Base_Bus::PlanDataReader::instance()->getNodePath(refActivePlan.plan).c_str();
    m_detail.owner = owner.c_str();
    m_detail.activeUpdateId = updateId;
    m_detail.activePlanState = static_cast<TA_Base_Core::EActivePlanState>(initialState);
    m_detail.currentStepDetail.name = CORBA::string_dup( "undefined" );
    m_detail.currentStepDetail.remark = CORBA::string_dup( "" );
    m_detail.currentStepDetail.position = 0u;
    m_detail.currentStepDetail.state = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;
    m_detail.remark = remark.c_str();
    m_detail.manuallyExecuted = manExec;
    m_detail.assocAlarmId = assocAlarmId.c_str();
    m_detail.timer = 0u;
    m_detail.lastStateTransitionTime = m_lastModified;
    m_detail.instantiationTime = m_lastModified;
}


PlanDetail::PlanDetail( const TA_Base_Core::ActivePlanDetail& detail ) :
m_detail(),
m_session( "" ),
m_pmId( "" ),
m_alarmID( "" ),
m_userKey( 0 ),
m_exeHistoryMap(),
m_custHistoryMap(),
m_lastModified( ACE_OS::time() ),
m_result( 0 )
{
    m_detail.plan = detail.plan;
    m_detail.path = detail.path;
    m_detail.owner = detail.owner;
    m_detail.activeUpdateId = detail.activeUpdateId;
    m_detail.activePlanState = detail.activePlanState;
    m_detail.currentStepDetail = detail.currentStepDetail;
    m_detail.manuallyExecuted = detail.manuallyExecuted;
    m_detail.assocAlarmId = detail.assocAlarmId;
    m_detail.timer = detail.timer;
	m_detail.remark = detail.remark;
	m_detail.lastStateTransitionTime = detail.lastStateTransitionTime;
	m_detail.instantiationTime = detail.instantiationTime;

	if ( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE == detail.activePlanState )
	{
		m_detail.currentStepDetail.state = TA_Base_Core::PAUSED_ACTIVE_STEP_STATE;
	}
	else
	{
		m_detail.currentStepDetail.state = detail.currentStepDetail.state;
	}
}

bool PlanDetail::set( const int state )
{
	if( state != m_detail.activePlanState )
	{
		m_detail.activePlanState = static_cast<TA_Base_Core::EActivePlanState>(state);
        m_detail.lastStateTransitionTime = ACE_OS::time();
		m_lastModified = ACE_OS::time();
		if( isNonActive() )
		{
			m_result.set(state);
		}

		return true;
	}

	return false;
}

bool PlanDetail::isNonActive() const
{
    switch( m_detail.activePlanState )
    {
    case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
        return true;
        //break;

    default:
        break;
    }

    return false;
}

bool PlanDetail::isActive() const
{
    switch( m_detail.activePlanState )
    {
    case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
    case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
    case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
        return true;
        //break;

    default:
        break;
    }

    return false;
}

} // TA_Base_App

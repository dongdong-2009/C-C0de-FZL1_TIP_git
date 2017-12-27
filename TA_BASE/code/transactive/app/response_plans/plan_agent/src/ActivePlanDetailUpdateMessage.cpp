/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h"

namespace TA_Base_App
{
    ActivePlanDetailUpdateMessage::ActivePlanDetailUpdateMessage(const TA_Base_Core::ActivePlanDetail& planDetail) : 
    m_detail ( planDetail )
    {
    }

    void ActivePlanDetailUpdateMessage::execute( ActivePlan& activePlan ) const
    {
        //Update the step number
        //Send appropriate updates so that ActivePlnaBulletinBoard can be updated.

        TA_Base_Core::StepNumber position = m_detail.currentStepDetail.position;
        if ( position != activePlan.getStepNumber() )
        {
            // Update only when current step position is changed
            activePlan.setCurrentStepPosition( position );
        }

        ActivePlanBulletinBoard::instance()->setUpdateId(m_detail.activeUpdateId);

        if ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == m_detail.activePlanState )
        {
            ActivePlanBulletinBoard::instance()->cullStoppedPlan( m_detail.plan );
            return;
        }

        // Ensure plan is not active when its state is non-active
        if (( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == m_detail.activePlanState ) || 
            ( TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE == m_detail.activePlanState ) || 
            ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == m_detail.activePlanState ))
        {
            activePlan.setIsActive(false);
        }

        if ( TA_Base_Core::LOADED_ACTIVE_STEP_STATE == m_detail.currentStepDetail.state )
        {
            activePlan.getCurrentStep()->refreshStartTime();
        }

        // Update step dynamic remark
        activePlan.getCurrentStep()->remark( m_detail.currentStepDetail.remark.in() );

        // Update all fields of ActivePlanDetail since we don't know which update should be called
        ActivePlanBulletinBoard::instance()->updateActivePlanDetail( m_detail );

//         if ( TA_Base_Core::LOADED_ACTIVE_STEP_STATE == m_detail.currentStepDetail.state )
//         {
//             ActivePlanBulletinBoard::instance()->update( m_detail.plan, 
//                                                          m_detail.currentStepDetail.position,
//                                                          m_detail.currentStepDetail.name.in(),
//                                                          m_detail.remark.in() );
//         }
//         else 
//         {
//             switch (m_detail.activePlanState)
//             {
//                 case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
//                 case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
//                 case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
//                     activePlan.setIsActive(false);
//                     ActivePlanBulletinBoard::instance()->update( m_detail.plan, 
//                                                                  m_detail.activePlanState,
//                                                                  m_detail.remark.in() );
//                     break;
// 
//                 case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
//                     ActivePlanBulletinBoard::instance()->cullStoppedPlan( m_detail.plan );
//                     break;
// 
//                 default:
//                     ActivePlanBulletinBoard::instance()->update( m_detail.plan, 
//                                                                  m_detail.currentStepDetail.position,
//                                                                  m_detail.currentStepDetail.state,
//                                                                  m_detail.remark.in() );
//             }
//         }
    }
}

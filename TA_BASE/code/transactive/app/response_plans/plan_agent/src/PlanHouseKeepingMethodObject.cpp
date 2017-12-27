/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.cpp $
  *  @author:  jiang.huang
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/23 16:58:54 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanHouseKeepingMethodObject.
  */

#include "ace/Time_Value.h"
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.h"

namespace TA_Base_App
{

    const int WORKING_INTERVAL = 100;

    PlanHouseKeepingMethodObject::PlanHouseKeepingMethodObject( PlanExecutionEngine& task ) : 
    m_engine( task )
    {
    }

    PlanHouseKeepingMethodObject::~PlanHouseKeepingMethodObject()
    {
    }

    int PlanHouseKeepingMethodObject::call()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Run plan house keeping method object");

        ACE_Time_Value tmWorkingInterval = ACE_Time_Value::zero;
        tmWorkingInterval.msec( WORKING_INTERVAL );

        while( !ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
        {
            try
            {
                ACE_OS::sleep( tmWorkingInterval );
                if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
                {
                    break;
                }

                ActivePlanBulletinBoard::instance()->cullStoppedPlans();

                ACE_OS::sleep( tmWorkingInterval );
                if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
                {
                    break;
                }

                m_engine.consolidateMq();

                ACE_OS::sleep( tmWorkingInterval );
            }
            catch( const std::exception& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Exit plan house keeping method object");
	    
        return 0;
    }
}
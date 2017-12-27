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
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/CMMessageSender.h"

namespace TA_Base_App
{
    CMMessageSenderImpl::CMMessageSenderImpl()
    {
		m_commsSender.reset(TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::PlanAgentComms::Context ));
    }

	CMMessageSenderImpl::~CMMessageSenderImpl()
	{
	}

    bool CMMessageSenderImpl::sendActiveStepUpdateMessage( const TA_Base_Core::CurrentPlanStepDetail& stepDetail )
    {
        try
        {
            if (ActivePlanBulletinBoard::instance()->isControlMode())
            {
                if (NULL != m_commsSender.get())
                {
                    CORBA::Any commsData;
                    commsData <<= stepDetail;

                    m_commsSender->sendCommsMessage(TA_Base_Core::PlanAgentComms::ActiveStepUpdate, 
                        PlanAgentUser::getInstance()->getAgentEntityKey(),
                        commsData,
                        PlanAgentUser::getInstance()->getAgentSubsystemKey(),
                        PlanAgentUser::getInstance()->getAgentLocationKey()
                        );
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Comms Sender is NULL, fail to send active step update message");
                    return false;
                }
            }
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to send active step update message");
            return false;
        }
        return true;
    }

    
}



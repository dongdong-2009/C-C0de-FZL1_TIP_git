/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"

namespace TA_Base_App
{

TA_Base_App::PlanAgentUpdateProcessor *TA_Base_App::PlanAgentUpdateProcessor::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::PlanAgentUpdateProcessor::s_singletonLock;

/////////////////////////////////////////////////////////////////////////////
// PlanAgentUpdateProcessor

PlanAgentUpdateProcessor::PlanAgentUpdateProcessor()
{
    FUNCTION_ENTRY( "PlanAgentUpdateProcessor" );

    std::string strLocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
    TA_Base_Core::FilterData filterData;
    TA_Base_Core::NameValuePair filter("LocationKey", strLocation);
    filterData.push_back(&filter);
    // Subscribe for active plan updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, this, &filterData );

    const unsigned long ulCurLocation = TA_Base_Core::DataConversion::toUnsignedInteger( strLocation );
    // Subscribe for active step updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
        TA_Base_Core::PlanAgentComms::ActiveStepUpdate, this, TA_Base_Core::ANY_ENTITY, 0u, ulCurLocation );

    FUNCTION_EXIT;
}


PlanAgentUpdateProcessor::~PlanAgentUpdateProcessor()
{
    FUNCTION_ENTRY( "~PlanAgentUpdateProcessor" );

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

    FUNCTION_EXIT;
}


PlanAgentUpdateProcessor &PlanAgentUpdateProcessor::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if ( NULL == s_instance )
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if ( NULL == s_instance )
        {
            s_instance = new PlanAgentUpdateProcessor();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void PlanAgentUpdateProcessor::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    if ( NULL != s_instance )
    {
        delete s_instance;

        s_instance = NULL;
    }

    FUNCTION_EXIT;
}

void PlanAgentUpdateProcessor::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "receiveSpecialisedMessage" );

    // Try extracting to ActivePlanDetail.
    TA_Base_Core::ActivePlanDetail *activePlanDetail = NULL;
    if ((message.messageState >>= activePlanDetail))
    {
        TA_Base_Core::ActivePlanDetail activePlanDetailtmp = *activePlanDetail;
        ActivePlanAgency::instance()->processActivePlanUpdate( activePlanDetailtmp );

        FUNCTION_EXIT;
        return;
    }

    // Try extracting to ActivePlanDetail.
    TA_Base_Core::CurrentPlanStepDetail *currentPlanStepDetail = NULL;
    if ((message.messageState >>= currentPlanStepDetail))
    {
        TA_Base_Core::CurrentPlanStepDetail currentPlanStepDetailtmp = *currentPlanStepDetail;
        ActivePlanAgency::instance()->processActiveStepDetailUpdate(currentPlanStepDetailtmp);

        FUNCTION_EXIT;
        return;
    }

    FUNCTION_EXIT;
}

}

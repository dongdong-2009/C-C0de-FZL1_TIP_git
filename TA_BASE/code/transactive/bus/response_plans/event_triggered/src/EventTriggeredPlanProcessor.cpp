/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/event_triggered/src/EventTriggeredPlanProcessor.cpp $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the processor that is responsible for triggering plans based off an event.
 */

#include "bus/response_plans/event_triggered/src/EventTriggeredPlanProcessor.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    void EventTriggeredPlanProcessor::registerWithProcessingLibrary()
    {
        static bool isRegistered = false;

        // Only create a new processor and register it if it has not been done before.
        if ( !isRegistered )
        {
            TA_Base_Core::IEventProcessor* planProcessor = new EventTriggeredPlanProcessor();
            TA_Base_Core::EventProcessingLibrary::instance()->addProcessor( planProcessor );
            isRegistered = true;
        }
    }


    EventTriggeredPlanProcessor::EventTriggeredPlanProcessor()
    {
    }


    EventTriggeredPlanProcessor::~EventTriggeredPlanProcessor()
    {
    }


    unsigned char EventTriggeredPlanProcessor::getNumRetries()
    {
        // We should not retry plan processing as a plan is only relevant at the instance the event is
        // raised. Running a plan at an unspecified time in the future may perform operations that are
        // no longer required.
        return 0;
    }


    unsigned long EventTriggeredPlanProcessor::getRetryDelay()
    {
        // Plan execution is never retried...
        return 0;
    }


    bool EventTriggeredPlanProcessor::processEvent( TA_Base_Core::Event& event )
    {
        // See if the event matches any associations in the cache.
        EventPlanAssocCache::PlanNames planNames = m_assocCache.findPlansForEvent( event );

        if ( !planNames.empty() )
        {
            // Make sure we have a valid interface to the plan agent.
            if ( NULL == m_planAgent.get() )
            {
                // Get the location key as a string and convert it to an unsigned long.
                std::stringstream locationStr;
                locationStr << TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY );
                unsigned long locationKey;
                locationStr >> locationKey;

                // Construct the plan agent object.
                m_planAgent = std::auto_ptr<PlanAgentLibrary>( new PlanAgentLibrary( locationKey ) );
            }

            // For each match found call on the plan agent.
            EventPlanAssocCache::PlanNames::iterator it;
            for ( it=planNames.begin(); it!=planNames.end(); it++ )
            {
                try
                {
				    // TD14811 to make sure only 1 plan execute each time
                    // Execute the plan using the super session.
                    //m_planAgent->runPlanByName( TA_Base_Core::SessionAccessFactory::getSuperSessionId().c_str(),
                    //                            (*it).c_str() );
				    m_planAgent->runPlanByNameWithEntityRestriction( TA_Base_Core::SessionAccessFactory::getSuperSessionId().c_str(),
                                                (*it).c_str(), 0);
                }
                catch ( const IPlanAgentCorbaDef::PlanAgentError& ex )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "PlanAgentError exception (%s) while executing plan %s.",
                        ex.what.in(), (*it).c_str() );
                }
                catch ( const IPlanAgentCorbaDef::PlanNotFoundError& ex )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "PlanNotFoundError exception (%s) while executing plan %s.",
                        ex.what.in(), (*it).c_str() );
                }
                catch ( const IPlanAgentCorbaDef::NoPermission& ex )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "NoPermission exception (%s) while executing plan %s.",
                        ex.what.in(), (*it).c_str() );
                }
                catch ( const IPlanAgentCorbaDef::PlanStepDetailError& ex )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "PlanStepDetailError exception (%s) while executing plan %s.",
                        ex.what.in(), (*it).c_str() );
                }
                catch ( const TA_Base_Core::TransactiveException& ex )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "TransActive exception (%s) while executing plan %s.",
                        ex.what(), (*it).c_str() );
                }
                catch ( ... )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown exception caught while executing plan %s.",
                        (*it).c_str() );
                }
            }
        }

        // Never retry.
        return true;
    }
};

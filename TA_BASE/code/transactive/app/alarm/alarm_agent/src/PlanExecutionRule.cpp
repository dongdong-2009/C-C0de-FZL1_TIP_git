/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/PlanExecutionRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
	#pragma warning(disable : 4786)
	#pragma warning(disable : 4250)
#endif

#include "app/alarm/alarm_agent/src/PlanExecutionRule.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/AlarmAgentAlarm_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PlanAgentData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"



using TA_Base_Core::RunParams;

namespace TA_Base_App
{

    //
    // Constructor
    //
    PlanExecutionRule::PlanExecutionRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                                         TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key, 
                                         const std::string& planToRun )
    : AbstractRule(entityKey, entityType, alarmTypeKey, trigger, key), m_planToRun(planToRun), m_planAgent(NULL)
    {}


    //
    // Destructor
    //
    PlanExecutionRule::~PlanExecutionRule()
    {
        if( m_planAgent != NULL )
        {
            delete m_planAgent;
            m_planAgent = 0;
        }
		//chenlei--
		//may hang up while destroying AlarmHelperBase
		//thus cause AlarmAgentServant cannot release lock which leads to dead lock
		//TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    }


    //
    // process
    //
    void PlanExecutionRule::process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                     TA_Base_App::AbstractRule::ETriggerType trigger)
    {
        try
        {
            if( doesRuleApply(message, trigger) )
            {
                
				if( m_planAgent == NULL )
                {
					unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());

                    m_planAgent = new TA_Base_Bus::PlanAgentLibrary(locationKey);
                }

                //m_planAgent->runPlanByName(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()).c_str(), m_planToRun.c_str());
				//marvin++
				//TD13527
				m_planAgent->runPlanByNameWithEntityRestriction(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()).c_str(), m_planToRun.c_str(), message.assocEntityKey);
				//++marvin
				//TD13527
            }
        }
        catch( ... )
        {
            unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());

			TA_Base_Core::DescriptionParameters dp;
	        dp.push_back(new TA_Base_Core::NameValuePair("PlanId", m_planToRun));

            TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::AlarmAgentAlarm::FailedToExecutePlan,
                            atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENTITY_KEY.c_str()).c_str()),
                            atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENTITY_TYPE_KEY.c_str()).c_str()),
                            dp, RunParams::getInstance().get(RPARAM_ENTITYNAME),locationKey,0,
							RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ASSET_NAME.c_str()),
							TA_Base_Core::AlarmConstants::defaultTime );
        }
    }
};

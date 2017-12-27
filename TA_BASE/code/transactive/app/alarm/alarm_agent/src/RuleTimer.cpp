/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/RuleTimer.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <time.h>

#include "app/alarm/alarm_agent/src/RuleTimer.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"

namespace TA_Base_App
{
    unsigned int RuleTimer::SUPPRESSION_RULE_LIFETIME_DEFAULT = 600;

    TA_Base_Core::ReEntrantThreadLockable RuleTimer::m_lock;
    
    RuleTimer::RuleTimer(RuleRepository& ruleRepository)
        : m_ruleRepository(ruleRepository),
          m_ruleLifetime(0),
          m_terminated(false)
    {
        FUNCTION_ENTRY("RuleTimer");

        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

        try
        {
            TA_Base_Core::IEntityData * pTmpEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName.c_str());
			std::auto_ptr<TA_Base_Core::IEntityData> entityData(pTmpEntityData);

			TA_Base_Core::AlarmAgentEntityData* agentData = dynamic_cast <TA_Base_Core::AlarmAgentEntityData*> (entityData.get());
			m_ruleLifetime = agentData->getSuppressionRuleLifetime();
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An error occurred while retrieving suppression rule lifetime parameter. Setting to default.");
            m_ruleLifetime = SUPPRESSION_RULE_LIFETIME_DEFAULT;
            return;
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An error occurred while retrieving suppression rule lifetime parameter. Setting to default.");
            m_ruleLifetime = SUPPRESSION_RULE_LIFETIME_DEFAULT;
            return;
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An unknown error occurred while retrieving suppression rule lifetime parameter. Setting to default.");
            m_ruleLifetime = SUPPRESSION_RULE_LIFETIME_DEFAULT;
            return;
        }

        FUNCTION_EXIT;
    }


    RuleTimer::~RuleTimer()
    {
    }


    void RuleTimer::run()
    {
        FUNCTION_ENTRY("run");

        while(!m_terminated)
        {
			std::vector<unsigned long> vtRemovedRuleId;
			{
				//xinsong ++, need to guard
				//note: if another thread and this thread also use m_ruleRepository to remove Rule, it will cause deadlock for waiting another locks
				//if no gurad, the iterator maybe corrupted, for anthother thread will delete the next iter.
				TA_Base_Core::ThreadGuard guard(m_lock);

				std::map<unsigned long, time_t>::iterator iter = m_timedRules.begin();
				while(iter != m_timedRules.end())
				{
					time_t timeNow;
					time(&timeNow);

					if(iter->second < timeNow)
					{
						// Rule has expired
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timer expired for suppression rule %lu.", iter->first);
                
						unsigned long ruleId = iter->first;
						++iter;
						
						//xinsong ++
						//m_ruleRepository.removeRule(ruleId);
						vtRemovedRuleId.push_back(ruleId);
					}    
					else
					{
						++iter;
					}
				}
			}//guard end

			for( size_t i=0; i<vtRemovedRuleId.size(); i++)
			{
				unsigned long ruleId = vtRemovedRuleId[i];
				m_ruleRepository.removeRule(ruleId);
            }
            
            int sleepInMillisecs = 1000; // wait one second before checking again.
            TA_Base_Core::Thread::sleep(sleepInMillisecs);
        }

        FUNCTION_EXIT;
    }
    

    void RuleTimer::terminate()
    {
        FUNCTION_ENTRY("terminate");

        m_terminated = true;

        FUNCTION_EXIT;
    }


    void RuleTimer::addRule(unsigned long ruleId)
    {
        FUNCTION_ENTRY("addRule");

		// If the SuppressionRuleLifeTime for this agent is set to zero, there is no expiry
		// ie do not add the rule for timing
		if (m_ruleLifetime==0) 
			return;

        TA_Base_Core::ThreadGuard guard(m_lock);
        std::map<unsigned long, time_t>::iterator iter = m_timedRules.find(ruleId);
        
        if(iter != m_timedRules.end())
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Rule already exists, ruleId=%d", ruleId);
        }
        else
        {
            // Determine the expiry time
            time_t timeNow;
            time(&timeNow);
            time_t expiryTime = timeNow + m_ruleLifetime;

            m_timedRules.insert(std::map<unsigned long, time_t>::value_type(ruleId, expiryTime));
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Rule add into timeRules map, ruleId=%d", ruleId);
        }
        
        FUNCTION_EXIT;
    }


    void RuleTimer::removeRule(unsigned long ruleId)
    {
        FUNCTION_ENTRY("removeRule");

        std::map<unsigned long, time_t>::iterator iter = m_timedRules.find(ruleId);
        
        if(iter == m_timedRules.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Rule is not timed.");
        }
        else
        {
            TA_Base_Core::ThreadGuard guard(m_lock);

            // Remove the suppression rule from the alarm agent
            m_timedRules.erase(iter);

            // Remove the suppression rule from the database
            try
            {
                TA_Base_Core::IAlarmRuleData* pTmpRuleData = 
					TA_Base_Core::AlarmRuleAccessFactory::getInstance().getAlarmRule(ruleId);
				std::auto_ptr<TA_Base_Core::IAlarmRuleData> iRuleData( pTmpRuleData);
				TA_Base_Core::SuppressionRuleData* ruleDetails = dynamic_cast <TA_Base_Core::SuppressionRuleData*> (iRuleData.get());
				ruleDetails->deleteThisObject();
				// No need to delete ruleDetails pointer because its the same pointer stored in the iRuleData auto_ptr //
				// delete ruleDetails; //TODO: Check whether need to delete it
            }
            catch(const TA_Base_Core::DatabaseException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not delete alarm rule: %lu from database", ruleId);
                return;
            }
            catch(const TA_Base_Core::DataException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not delete alarm rule: %lu from database", ruleId);
                return;
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not delete alarm rule: %lu from database", ruleId);
                return;
            }
		}

        FUNCTION_EXIT;
    }
    
};

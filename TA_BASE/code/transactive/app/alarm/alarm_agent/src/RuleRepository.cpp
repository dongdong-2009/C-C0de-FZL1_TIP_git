/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "app/alarm/alarm_agent/src/RuleRepository.h"
#include "app/alarm/alarm_agent/src/RuleTimer.h"
#include "app/alarm/alarm_agent/src/SuppressionRule.h"
#include "app/alarm/alarm_agent/src/PlanExecutionRule.h"
#include "app/alarm/alarm_agent/src/AutoAcknowledgeRule.h"
#include "app/alarm/alarm_agent/src/MMSRule.h"
#include "app/alarm/alarm_agent/src/CCTVRule.h"
#include "app/alarm/alarm_agent/src/DatabaseLoader.h"
#include "app/alarm/alarm_agent/src/AvalancheSuppressionRule.h"
#include "app/alarm/alarm_agent/src/AvalancheSuppressionChild.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"

#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmMessageCorbaDef;
using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::IAlarmRuleData;
using TA_Base_Core::AlarmRuleAccessFactory;


namespace TA_Base_App
{
    const std::string ADD_RULE = "add";
    const std::string REMOVE_RULE = "remove";
    const std::string SUBMIT = "submit";
    const std::string ACK = "acknowledge";
    const std::string CLOSE = "close";

    const unsigned int RuleRepository::MAX_AVALANCHE_RULES = 100;
    const unsigned int RuleRepository::MAX_AVALANCHE_CHILDREN = 600;
    
    RuleRepository* RuleRepository::m_instance = 0;
    TA_Base_Core::NonReEntrantThreadLockable RuleRepository::m_singletonLock;


    //
    // getInstance
    //
    RuleRepository* RuleRepository::getInstance()
    {
        if( m_instance == 0 )
        {
            TA_Base_Core::ThreadGuard guard(m_singletonLock);

            if( m_instance == 0 )
            {
                m_instance = new RuleRepository();
            }
        }
        return m_instance;
    }

	void RuleRepository::releaseInstance()
    {
        
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if( m_instance != 0 )
		{
			delete m_instance;
		}
		m_instance=NULL;
    }

    //
    // Private Constructor
    //
    RuleRepository::RuleRepository()
    : m_entityKey(0), m_entityName(TA_Base_Core::ALARM_AGENT_NAME), m_dbLoader(0), m_dataLoaded(false),
	  m_ruleTimer(NULL), m_genericAgent(NULL), m_lastAvalancheSuppressionRule(NULL)
    {
		LOG0(SourceInfo,DebugUtil::DebugDebug,"constructor in")

        m_ruleTimer = new RuleTimer(*this);
        m_ruleTimer->start();

        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ALARM_RULE ,*this );

        LOG0(SourceInfo,DebugUtil::DebugDebug,"constructor out")
    }

	
    //
    // Destructor
    //
    RuleRepository::~RuleRepository()
    {
        LOG0(SourceInfo,DebugUtil::DebugDebug,"destructor in")

        TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ALARM_RULE, *this );

        for( unsigned int i = 0; i < m_repository.size(); i++ )
        {
            if( m_repository[i] != NULL )
            {
                delete m_repository[i];
                m_repository[i] = 0;
            }
        }
        m_repository.clear();

        if( m_dbLoader != NULL )
        {
            m_dbLoader->terminateAndWait();
            delete m_dbLoader;
            m_dbLoader = 0;
        }

        if(m_ruleTimer != NULL)
        {
            m_ruleTimer->terminateAndWait();
            delete m_ruleTimer;
            m_ruleTimer = 0;
        }

        LOG0(SourceInfo,DebugUtil::DebugDebug,"destructor out")
    }


    //
    // hasDataLoaded
    //
    bool RuleRepository::hasDataLoaded()
    {
        return m_dataLoaded;
    }


    //
    // loadRules
    //
    void RuleRepository::loadRules()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "loadRules" );

        if( m_dbLoader == 0 )
        {
            m_dbLoader = new DatabaseLoader(this);
            m_dbLoader->start();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "loadRules" );
    }

	void RuleRepository::unloadRules()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "unloadRules" );

		// delete the database loader
		if( m_dbLoader != 0 )
		{
			m_dbLoader->terminateAndWait();
			delete m_dbLoader;
			m_dbLoader = 0;
		}

		// clear all existing rules
		for( unsigned int i = 0; i < m_repository.size(); i++ )
		{
			if( m_repository[i] != NULL )
			{
				delete m_repository[i];
				m_repository[i] = 0;
			}
		}
		m_repository.clear();

		LOG( SourceInfo, DebugUtil::FunctionExit, "unloadRules" );
	}

    //
    // loadFromDatabase
    //
    void RuleRepository::loadFromDatabase()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "loadFromDatabase" );

        try
        {
            ThreadGuard guard(m_writeLock);

            createSuppressionRules();
            createAutoAcknowledgeRules();
            createAvalancheSuppressionRules();
            createPlanExecutionRules();
            createMMSRules();
            createCCTVRules();

            m_dataLoaded = true;

            LOG( SourceInfo, DebugUtil::FunctionExit, "loadFromDatabase" );

        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            // Failed to load our data, make sure that we try again
            m_dataLoaded = false;
        }
    }


	//
	// terminateDataLoading
	//
	void RuleRepository::terminateDataLoading()
	{
		FUNCTION_ENTRY("terminateDataLoading");
		
		m_dbLoader->terminate();

		FUNCTION_EXIT;
	}

    //
    // requestFromControllingAgent
    //
    void RuleRepository::requestFromControllingAgent()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "requestFromControllingAgent" );

        // We now no longer synchronise Alarm Rules .. this is no longer needed.
        // It is now fine to just read it from the database when in monitor mode.

        loadFromDatabase();

        LOG( SourceInfo, DebugUtil::FunctionExit, "requestFromControllingAgent" );
    }


    //
    // processUpdate
    //
    void RuleRepository::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

        switch(updateEvent.getModifications())
        {
            case TA_Base_Core::Create:
            {
                addRule(updateEvent.getKey());
                break;
            }
            case TA_Base_Core::Delete:
            {
                removeRule(updateEvent.getKey());
                break;
            }
            case TA_Base_Core::Update:
            {
                updateRule(updateEvent.getKey());
                break;
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );
    }

    
    //
    // setGenericGuiObject
    //
    void RuleRepository::setGenericAgentObject(TA_Base_Bus::GenericAgent& genericAgent)
    {
        m_genericAgent = &genericAgent;
    }


    //
    // setGenericGuiObject
    //
    bool RuleRepository::doAnyRulesOfTypeApplyToAlarm(const std::type_info& ruleType, unsigned long entityTypeKey, 
        unsigned long entityKey, unsigned long alarmTypeKey)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "doAnyRulesOfTypeApplyToAlarm" );

        //aquireReadLock();
		ThreadGuard guard(m_writeLock);

        std::vector<AbstractRule*>::iterator start = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();

        while( start != end )
        {
            AbstractRule* rule = *start;
            
            // Only check if it matches if it is of the same type
            if(ruleType != typeid(*rule))
            {
                start++;
                continue;
            }

            if((rule->getEntityType() == entityTypeKey && rule->getAlarmTypeKey() == alarmTypeKey) ||
                (rule->getEntityKey() == entityKey && rule->getAlarmTypeKey() == alarmTypeKey))
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "doAnyRulesOfTypeApplyToAlarm - true" );

                //releaseReadLock();
                return true;
            }
            start++;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "doAnyRulesOfTypeApplyToAlarm - false" );

		LOG( SourceInfo, DebugUtil::FunctionEntry, "doAnyRulesOfTypeApplyToAlarm" );
        //releaseReadLock();
        return false;
    }


    //
    // addRule
    //
    void RuleRepository::addRule( unsigned long ruleId )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "addRule" );

        try
        {
            ThreadGuard guard(m_writeLock);

            std::vector<AbstractRule*>::iterator p = m_repository.begin();

			TA_Base_Core::IAlarmRuleData * pTmpRule = AlarmRuleAccessFactory::getInstance().getAlarmRule(ruleId);
			std::auto_ptr<TA_Base_Core::IAlarmRuleData> rule(pTmpRule);

            AbstractRule* abstractRule = convertIAlarmRuleDataToAbstractRule(rule.get());

            if( abstractRule == NULL )
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "addRule rule is null" );
                return;
            }

            if( findRule(abstractRule, p) )
            {
                delete abstractRule;
				abstractRule = NULL;
                LOG( SourceInfo, DebugUtil::FunctionExit, "addRule rule already exists" );
                return;
            }
			if (AVALANCHE == abstractRule->getRuleType())
			{
				//get all avalanche rule.
				std::vector<AvalancheSuppressionRule*> AvalancheRules;
				for (std::vector<AbstractRule*>::iterator p = m_repository.begin(); p != m_repository.end(); p++)
				{
					if (AVALANCHE == (*p)->getRuleType())
					{
						AvalancheRules.push_back(dynamic_cast<AvalancheSuppressionRule*>(*p));
					}
				}
				AvalancheRules.push_back(dynamic_cast<AvalancheSuppressionRule*>(abstractRule));
				//initialize the children
				for(std::vector<AvalancheSuppressionRule*>::iterator AvalancheRulesItr=AvalancheRules.begin();AvalancheRulesItr!=AvalancheRules.end();AvalancheRulesItr++)
				{
					for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
					{
						(*itr)->initChild(*AvalancheRulesItr);
					}
				}
				//check if there is a circle in the tree
				for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
				{
					if((*itr)->existCircle())
					{
						AvalancheRules.clear();
						LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "There are circle of alarm rule, please check the configuration of database" );
						// Need to terminate with "don't restart me" message.            
						m_genericAgent->terminateWithoutRestart();

						// Finally throw an exception, so the data loader will stop trying to load any more data
						TA_THROW(TA_Base_Core::TransactiveException("There are circle of alarm rule"));
					}
				}
			}
 
            m_repository.push_back(abstractRule);

            AlarmCache::getInstance()->processAlarmRuleUpdate(*abstractRule, TA_Base_App::AbstractRule::RULE_CREATED);
			LOG( SourceInfo, DebugUtil::FunctionExit, "addRule rule added" );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException",
                 "Unable to add rule");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                 "Unable to add rule");
        }
        catch( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Unable to add rule");
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "addRule rule added" );
    }


    //
    // updateRule
    //
    void RuleRepository::updateRule( unsigned long ruleId )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "updateRule" );

        removeRule(ruleId);
        addRule(ruleId);

        std::vector<AbstractRule*>::iterator p = m_repository.begin();

        if( false == findRule(ruleId, p) )
        {
            LOG( SourceInfo, DebugUtil::FunctionExit, "removeRule rule not found" );
            return;
        }

        AlarmCache::getInstance()->processAlarmRuleUpdate(**p,  TA_Base_App::AbstractRule::RULE_UPDATED);

        LOG( SourceInfo, DebugUtil::FunctionExit, "updateRule" );
    }


    //
    // removeRule
    //
    void RuleRepository::removeRule( unsigned long ruleId )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "removeRule" );

        try
        {
            ThreadGuard guard(m_writeLock);

            std::vector<AbstractRule*>::iterator p = m_repository.begin();

            if( false == findRule(ruleId, p) )
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "removeRule rule not found" );
                return;
            }

			std::auto_ptr<AbstractRule> rp(*p);
			if (AVALANCHE == rp->getRuleType())
			{
				//get all avalanche rule.
				std::vector<AvalancheSuppressionRule*> AvalancheRules;
				for (std::vector<AbstractRule*>::iterator repositoryItr = m_repository.begin(); repositoryItr != m_repository.end(); repositoryItr++)
				{
					if (AVALANCHE == (*repositoryItr)->getRuleType())
					{
						AvalancheRules.push_back(dynamic_cast<AvalancheSuppressionRule*>(*repositoryItr));
					}
				}
				//remove from the children
				for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
				{
					(*itr)->removeChild(dynamic_cast<AvalancheSuppressionRule*>(rp.get()));
				}
			}
            m_repository.erase(p);
            AlarmCache::getInstance()->processAlarmRuleUpdate(*rp,  TA_Base_App::AbstractRule::RULE_DELETED);
            
            m_ruleTimer->removeRule(ruleId);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException",
                 "Unable to remove rule");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                 "Unable to remove rule");
        }
        catch( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Unable to remove rule");
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "removeRule rule deleted" );
    }


    //
    // isAlarmSuppressed
    //
    bool RuleRepository::isAlarmSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "isAlarmSuppressed" );

        TA_Base_Core::ThreadGuard guard(m_writeLock);
		//aquireReadLock();

        std::vector<AbstractRule*>::iterator start = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();

        while( start != end )
        {
            if( (*start)->alarmShouldBeSuppressed(alarm) )
            {
				m_lastAvalancheSuppressionRule = dynamic_cast<AvalancheSuppressionRule*>(*start);
                LOG( SourceInfo, DebugUtil::FunctionExit, "isAlarmSuppressed - true" );
                return true;
            }
            start++;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "isAlarmSuppressed - false" );

        //releaseReadLock();

        return false;
    }

	//
	// getLastAvalancheHead
	//
	std::string RuleRepository::getLastAvalancheHead()
	{
		if(m_lastAvalancheSuppressionRule != NULL)
		{
			return m_lastAvalancheSuppressionRule->getHeadAlarmId();
		}
		else
		{
			return "";
		}
	}


    //
    // isAlarmAnAvalancheHead
    //
    bool RuleRepository::isAlarmAnAvalancheHead( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "isAlarmAnAvalancheHead" );

        TA_Base_Core::ThreadGuard guard(m_writeLock);
		//aquireReadLock();

        std::vector<AbstractRule*>::iterator start = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();

        while( start != end )
        {
            if( (*start)->alarmIsAnAvalancheHead(alarm) )
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "isAlarmAnAvalancheHead - true" );
                return true;
            }
            start++;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "isAlarmAnAvalancheHead - false" );

        //releaseReadLock();

        return false;
    }


    //
    // processAlarm
    //
    void RuleRepository::processAlarm( const TA_Base_Core::AlarmDetailCorbaDef& alarm,
                                       TA_Base_App::AbstractRule::ETriggerType trigger )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "processAlarm" );

        //aquireReadLock();
        TA_Base_Core::ThreadGuard guard(m_writeLock);
        std::vector<AbstractRule*>::iterator start = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "processAlarm Rule" );
        while( start != end )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, "Processing Alarm Rule" );
            (*start)->process(alarm, trigger);
            start++;
        }

        //releaseReadLock();

        LOG( SourceInfo, DebugUtil::FunctionExit, "processAlarm" );
    }


    //
    // findRule
    //
    bool RuleRepository::findRule( AbstractRule* rule, std::vector<AbstractRule*>::iterator& itr )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "findRule" );

        itr = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();
        while( itr != end )
        {
            if( **itr == *rule )
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "findRule - found" );
                return true;
            }
            itr++;
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "findRule - not found" );

        return false;
    }


    //
    // findRule
    //
    bool RuleRepository::findRule( unsigned long key, std::vector<AbstractRule*>::iterator& itr )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "findRule by key" );

        itr = m_repository.begin();
        std::vector<AbstractRule*>::iterator end = m_repository.end();
        while( itr != end )
        {
            if( (*itr)->getKey() == key )
            {
                LOG( SourceInfo, DebugUtil::FunctionExit, "findRule by key - found" );
                return true;
            }
            itr++;
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "findRule by key - not found" );

        return false;
    }


    //
    // createSuppressionRules
    //
    void RuleRepository::createSuppressionRules()
    {
        std::vector<TA_Base_Core::SuppressionRuleData*> rules = 
                AlarmRuleAccessFactory::getInstance().getAllSuppressionRules();
        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
				std::string suppressType = rules[i]->getRuleType();
				SuppressionRule* suppressionRule;
				if (suppressType.compare(TA_Base_Core::SuppressionByDescriptionRuleData::RULE_TYPE) == 0)
				{
					suppressionRule = new SuppressionRule((rules[i])->getEntityKey(),
																		   (rules[i])->getEntityTypeKey(),
																		   (rules[i])->getAlarmType(),
																		   stringToTrigger((rules[i])->getRuleTrigger()),
																		   (rules[i])->getKey(),
																		   (rules[i])->getAlarmDescription() );
				}
				else 
				{
					suppressionRule = new SuppressionRule((rules[i])->getEntityKey(),
																		   (rules[i])->getEntityTypeKey(),
																		   (rules[i])->getAlarmType(),
																		   stringToTrigger((rules[i])->getRuleTrigger()),
																		   (rules[i])->getKey() );
				}

                if( suppressionRule != NULL )
                {
                    m_repository.push_back(suppressionRule);
                }

            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu suppression rules", rules.size() );

    }


    //
    // createMMSRules
    //
    void RuleRepository::createMMSRules()
    {
        std::vector<TA_Base_Core::MMSRuleData*> rules = 
                AlarmRuleAccessFactory::getInstance().getAllMMSRules();
        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
                MMSRule* mmsRule = new MMSRule((rules[i])->getEntityKey(),
                                               (rules[i])->getEntityTypeKey(),
                                               (rules[i])->getAlarmType(),
                                               stringToTrigger((rules[i])->getRuleTrigger()),
                                               (rules[i])->getKey() );

                if( mmsRule != NULL )
                {
                    m_repository.push_back(mmsRule);
                }

            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu MMS rules", rules.size() );

    }


    //
    // createCCTVRules
    //
    void RuleRepository::createCCTVRules()
    {
        std::vector<TA_Base_Core::CCTVRuleData*> rules = 
                AlarmRuleAccessFactory::getInstance().getAllCCTVRules();
        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
                CCTVRule* cctvRule = new CCTVRule((rules[i])->getEntityKey(),
                                               (rules[i])->getEntityTypeKey(),
                                               (rules[i])->getAlarmType(),
                                               stringToTrigger((rules[i])->getRuleTrigger()),
                                               (rules[i])->getKey() );

                if( cctvRule != NULL )
                {
                    m_repository.push_back(cctvRule);
                }

            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu CCTV rules", rules.size() );

    }


    //
    // createAutoAcknowledgeRules
    //
    void RuleRepository::createAutoAcknowledgeRules()
    {
        std::vector<TA_Base_Core::AutomaticAcknowledgementRuleData*> rules = 
                AlarmRuleAccessFactory::getInstance().getAllAutomaticAcknowledgementRules();
        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
                AutoAcknowledgeRule* autoAcknowledgeRule = new AutoAcknowledgeRule((rules[i])->getEntityKey(),
                                                                                   (rules[i])->getEntityTypeKey(),
                                                                                   (rules[i])->getAlarmType(),
                                                                                   stringToTrigger((rules[i])->getRuleTrigger()),
                                                                                   (rules[i])->getKey() );

                if( autoAcknowledgeRule != NULL )
                {
                    m_repository.push_back(autoAcknowledgeRule);
                }
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu auto acknowledge rules", rules.size() );

    }


    //
    // createAvalancheSuppressionRules
    //
    void RuleRepository::createAvalancheSuppressionRules()
    {
        std::vector<TA_Base_Core::AvalancheSuppressionRuleData*> rules;
		unsigned long LocationKey=0;
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY))
		{            
			std::string localstr=TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			if (localstr!="")
			{
				LocationKey=atol(localstr.c_str());
			}
		}
		if (LocationKey==0)
		{		 
           rules=AlarmRuleAccessFactory::getInstance().getAllAvalancheSuppressionRules();
		}
		else
		{
		   rules=AlarmRuleAccessFactory::getInstance().getAllAvalancheSuppressionRules(LocationKey);
		}
		std::vector<AvalancheSuppressionRule*> AvalancheRules;
        if (rules.size() > MAX_AVALANCHE_RULES)
        {
            // Oops - in this situation we close down with a "config error" alarm (safer than just loading 
            // a random set of 100 rules).
            // First log the problem
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Loaded more than %d avalanche alarm rules which exceeds capacity. Will terminate until configuration is fixed.",
                MAX_AVALANCHE_RULES);

            // Need to terminate with "don't restart me" message.            
            m_genericAgent->terminateWithoutRestart();

            // Finally throw an exception, so the data loader will stop trying to load any more data
            TA_THROW(TA_Base_Core::TransactiveException("Capacity of Alarm Avalanche Rules hit"));
        }

        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
                std::vector<TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair> children = (rules[i])->getChildren();

                std::vector<AvalancheSuppressionChild*> ruleChildren;

                for( unsigned int j = 0; j < children.size(); j++ )
                {
                    AvalancheSuppressionChild* child = new AvalancheSuppressionChild(0, (children[j]).first, (children[j]).second);
                    if( child != NULL )
                    {
                        ruleChildren.push_back(child);
                    }
                }

                AvalancheSuppressionRule* avalancheSuppressionRule = new AvalancheSuppressionRule((rules[i])->getEntityKey(),
                                                                                                  (rules[i])->getEntityTypeKey(),
                                                                                                  (rules[i])->getAlarmType(),
                                                                                                  stringToTrigger((rules[i])->getRuleTrigger()),
                                                                                                  (rules[i])->getKey(),
                                                                                                  (rules[i])->getTimeout(),
                                                                                                  (rules[i])->getTimein(),
                                                                                                  (rules[i])->getMinAlarms(),
                                                                                                  ruleChildren );
                
                if( avalancheSuppressionRule != NULL )
                {

					AvalancheRules.push_back(avalancheSuppressionRule);
                    m_repository.push_back(avalancheSuppressionRule);
                }

            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }           
		
		//initialize the child
		for(std::vector<AvalancheSuppressionRule*>::iterator AvalancheRulesItr=AvalancheRules.begin();AvalancheRulesItr!=AvalancheRules.end();AvalancheRulesItr++)
		{
	        for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
	    	{
				(*itr)->initChild(*AvalancheRulesItr);
	    	}
		}
		
		//check if there is a circle in the tree
		for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
    	{
			if((*itr)->existCircle())
			{
				AvalancheRules.clear();
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "There are circle of alarm rule, please check the configuration of database" );
				// Need to terminate with "don't restart me" message.            
				m_genericAgent->terminateWithoutRestart();

				// Finally throw an exception, so the data loader will stop trying to load any more data
				TA_THROW(TA_Base_Core::TransactiveException("There are circle of alarm rule"));
			}
    	}
	 
		AvalancheRules.clear();
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu avalanche suppression rules", rules.size() );

    }


    //
    // createPlanExecutionRules
    //
    void RuleRepository::createPlanExecutionRules()
    {
        std::vector<TA_Base_Core::PlanExecutionRuleData*> rules = 
                AlarmRuleAccessFactory::getInstance().getAllPlanExecutionRules();
        for( unsigned int i = 0; i < rules.size(); i++ )
        {
            try
            {
                PlanExecutionRule* planExecutionRule = new PlanExecutionRule((rules[i])->getEntityKey(),
                                                                             (rules[i])->getEntityTypeKey(),
                                                                             (rules[i])->getAlarmType(),
                                                                             stringToTrigger((rules[i])->getRuleTrigger()),
                                                                             (rules[i])->getKey(),
                                                                             (rules[i])->getPlanPath() );

                if( planExecutionRule != NULL )
                {
                    m_repository.push_back(planExecutionRule);
                }
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                     "Error encountered when loading rule. Proceeding to next rule.");
            }
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded %lu plan execution rules", rules.size() );

    }


    //
    // convertIAlarmRuleDataToAbstractRule
    //
    AbstractRule* RuleRepository::convertIAlarmRuleDataToAbstractRule( TA_Base_Core::IAlarmRuleData* rule )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "convertIAlarmRuleDataToAbstractRule" );

        try
        {

            std::string ruleType = rule->getRuleType();

            if( 0 == ruleType.compare(SUPPRESS_BY_TYPE) )
            {
                TA_Base_Core::SuppressionRuleData* s = dynamic_cast<TA_Base_Core::SuppressionRuleData*>(rule);
				
				SuppressionRule* retRule;
	            
				retRule = new SuppressionRule(s->getEntityKey(), s->getEntityTypeKey(), s->getAlarmType(),
		                                                           stringToTrigger(s->getRuleTrigger()), s->getKey()); 
                setSuppressionRuleTimer(s);

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning suppression_by_type rule" );

                return retRule;
			}
			else if (  0 == ruleType.compare(SUPPRESS_BY_DESCRIPTION) )
			{
                TA_Base_Core::SuppressionByDescriptionRuleData* s = dynamic_cast<TA_Base_Core::SuppressionByDescriptionRuleData*>(rule);
				
				SuppressionRule* retRule;
				
				retRule = new SuppressionRule(s->getEntityKey(), s->getEntityTypeKey(), s->getAlarmType(),
		                                                           stringToTrigger(s->getRuleTrigger()), s->getKey(), s->getAlarmDescription() ); 
                
                setSuppressionRuleTimer(s);

				LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning suppression_by_description rule" );

                return retRule;
            }
            else if( 0 == ruleType.compare(MMS) )
            {
                TA_Base_Core::MMSRuleData* m = dynamic_cast<TA_Base_Core::MMSRuleData*>(rule);

                MMSRule* retRule = new MMSRule(m->getEntityKey(), m->getEntityTypeKey(), m->getAlarmType(),
                                               stringToTrigger(m->getRuleTrigger()), m->getKey()); 

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning MMS rule" );

                return retRule;
            }
            else if( 0 == ruleType.compare(CCTV) )
            {
                TA_Base_Core::CCTVRuleData* m = dynamic_cast<TA_Base_Core::CCTVRuleData*>(rule);

                CCTVRule* retRule = new CCTVRule(m->getEntityKey(), m->getEntityTypeKey(), m->getAlarmType(),
                                               stringToTrigger(m->getRuleTrigger()), m->getKey()); 

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning CCTV rule" );

                return retRule;
            }
            else if( 0 == ruleType.compare(EXECUTE_PLAN) )
            {
                TA_Base_Core::PlanExecutionRuleData* p = dynamic_cast<TA_Base_Core::PlanExecutionRuleData*>(rule);

                PlanExecutionRule* retRule = new PlanExecutionRule(p->getEntityKey(), p->getEntityTypeKey(), p->getAlarmType(),
                                                                   stringToTrigger(p->getRuleTrigger()), p->getKey(),p->getPlanPath()); 

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning suppression rule" );

                return retRule;
            }
            else if( 0 == ruleType.compare(AUTO_ACK) )
            {
                TA_Base_Core::AutomaticAcknowledgementRuleData* a = dynamic_cast<TA_Base_Core::AutomaticAcknowledgementRuleData*>(rule);

                AutoAcknowledgeRule* retRule = new AutoAcknowledgeRule(a->getEntityKey(), a->getEntityTypeKey(), a->getAlarmType(),
                                                                       stringToTrigger(a->getRuleTrigger()), a->getKey()); 

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning suppression rule" );

                return retRule;
            }
            else if( 0 == ruleType.compare(AVALANCHE) )
            {
                TA_Base_Core::AvalancheSuppressionRuleData* a = dynamic_cast<TA_Base_Core::AvalancheSuppressionRuleData*>(rule);

                std::vector<TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair> children = a->getChildren();

                std::vector<AvalancheSuppressionChild*> ruleChildren;

                for( unsigned int j = 0; j < children.size(); j++ )
                {
                    AvalancheSuppressionChild* child = new AvalancheSuppressionChild(0, (children[j]).first, (children[j]).second);
                    if( child != NULL )
                    {
                        ruleChildren.push_back(child);
                    }
                }

                AvalancheSuppressionRule* retRule = new AvalancheSuppressionRule(a->getEntityKey(), a->getEntityTypeKey(), a->getAlarmType(),
                                                                               stringToTrigger(a->getRuleTrigger()), a->getKey(),
                                                                               a->getTimeout(), a->getTimein(), a->getMinAlarms(), ruleChildren); 

                LOG( SourceInfo, DebugUtil::FunctionExit, "convertIAlarmRuleDataToAbstractRule - returning suppression rule" );

                return retRule;
            }

            return NULL;
        }
        catch( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Unknown exception caught, re-throwing to calling method");
            throw;
        }
    }


    //
    // stringToTrigger
    //
    TA_Base_App::AbstractRule::ETriggerType RuleRepository::stringToTrigger( const std::string& trigger )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "stringToTrigger" );

        TA_Base_App::AbstractRule::ETriggerType triggerRet;
        
        if( 0 == trigger.compare(SUBMIT) )
        {
            triggerRet = TA_Base_App::AbstractRule::SUBMIT;
        }

        if( 0 == trigger.compare(ACK) )
        {
            triggerRet = TA_Base_App::AbstractRule::ACK;
        }

        if( 0 == trigger.compare(CLOSE) )
        {
            triggerRet = TA_Base_App::AbstractRule::CLOSE;
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "stringToTrigger" );

        return triggerRet;
    }


    //
    // aquireReadLock
    //
//     void RuleRepository::aquireReadLock()
//     {
//         LOG( SourceInfo, DebugUtil::FunctionEntry, "aquireReadLock" );
// 
//         m_readLock.acquire();
//             m_readCount++;
//             if( m_readCount == 1 )
//             {
//                 m_writeLock.acquire();
//             }
//         m_readLock.release();
// 
//         LOG( SourceInfo, DebugUtil::FunctionExit, "aquireReadLock" );
//     }


    //
    // releaseReadLock
    //
//     void RuleRepository::releaseReadLock()
//     {
//         LOG( SourceInfo, DebugUtil::FunctionEntry, "releaseReadLock" );
// 
//         m_readLock.acquire();
// 
//             m_readCount--;
//             if( m_readCount == 0 )
//             {
//                 m_writeLock.release();
//             }
// 
//         m_readLock.release();
// 
//         LOG( SourceInfo, DebugUtil::FunctionExit, "releaseReadLock" );
//     }


    void RuleRepository::setSuppressionRuleTimer(TA_Base_Core::SuppressionRuleData* suppressionRule)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setSuppressionRuleTimer" );

        // If the rule was not created by the configuration editor, set a timer for it
        if(suppressionRule->getCreatedByProfileKey() != ULONG_MAX)
        {
            m_ruleTimer->addRule(suppressionRule->getKey());
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Suppression rule created via CE. No need for timer.");
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "setSuppressionRuleTimer" );
    }
// 
// 	void RuleRepository::createAvalancheSuppressionRulesTest()
// 	{
// 		std::vector<AvalancheSuppressionRule*> AvalancheRules;
// 		//constuct first alarm suppression rule
// 		std::vector<AvalancheSuppressionChild*> ruleChildren1;
// 		AvalancheSuppressionChild* child2 = new AvalancheSuppressionChild(0, 1002, 30022);
// 		AvalancheSuppressionChild* child3 = new AvalancheSuppressionChild(0, 1003, 30022);
// 		AvalancheSuppressionChild* child4 = new AvalancheSuppressionChild(0, 1004, 30022);
// 		ruleChildren1.push_back(child2);
// 		ruleChildren1.push_back(child3);
// 		ruleChildren1.push_back(child4);
// 
// 		AvalancheSuppressionRule* avalancheSuppressionRule1 = 
// 			new AvalancheSuppressionRule(1001,0,30022,TA_Base_App::AbstractRule::SUBMIT,1,40000,0,0,ruleChildren1 );
// 
// 		if( avalancheSuppressionRule1 != NULL )
// 		{
// 			AvalancheRules.push_back(avalancheSuppressionRule1);
// 			m_repository.push_back(avalancheSuppressionRule1);
// 		}
// 
// 		//constuct second alarm suppression rule
// 		std::vector<AvalancheSuppressionChild*> ruleChildren2;
// 		AvalancheSuppressionChild* child5 = new AvalancheSuppressionChild(0, 1005, 30022);
// 		AvalancheSuppressionChild* child6 = new AvalancheSuppressionChild(0, 1006, 30022);
// 		AvalancheSuppressionChild* child7 = new AvalancheSuppressionChild(0, 1007, 30022);
// 		ruleChildren2.push_back(child5);
// 		ruleChildren2.push_back(child6);
// 		ruleChildren2.push_back(child7);
// 
// 		AvalancheSuppressionRule* avalancheSuppressionRule2 = 
// 			new AvalancheSuppressionRule(1003,0,30022,TA_Base_App::AbstractRule::SUBMIT,1,40000,0,0,ruleChildren2 );
// 
// 		if( avalancheSuppressionRule2 != NULL )
// 		{
// 			AvalancheRules.push_back(avalancheSuppressionRule2);
// 			m_repository.push_back(avalancheSuppressionRule2);
// 		}	
// 
// 		//constuct third alarm suppression rule
// 		std::vector<AvalancheSuppressionChild*> ruleChildren3;
// 		AvalancheSuppressionChild* child8 = new AvalancheSuppressionChild(0, 1008, 30022);
// 		AvalancheSuppressionChild* child9 = new AvalancheSuppressionChild(0, 1009, 30022);
// 		//test case1: rule will generate a circle.
// 		//AvalancheSuppressionChild* child10 = new AvalancheSuppressionChild(0, 1001, 30022);
// 		//test case2: rule will not generate a circle.
// 		AvalancheSuppressionChild* child10 = new AvalancheSuppressionChild(0, 1010, 30022);
// 		ruleChildren3.push_back(child8);
// 		ruleChildren3.push_back(child9);
// 		ruleChildren3.push_back(child10);
// 
// 		AvalancheSuppressionRule* avalancheSuppressionRule3 = 
// 			new AvalancheSuppressionRule(1006,0,30022,TA_Base_App::AbstractRule::SUBMIT,1,40000,0,0,ruleChildren3 );
// 
// 		if( avalancheSuppressionRule3 != NULL )
// 		{
// 			AvalancheRules.push_back(avalancheSuppressionRule3);
// 			m_repository.push_back(avalancheSuppressionRule3);
// 		}	
// 
// 		//initialize the child
// 		for(std::vector<AvalancheSuppressionRule*>::iterator AvalancheRulesItr=AvalancheRules.begin();AvalancheRulesItr!=AvalancheRules.end();AvalancheRulesItr++)
// 		{
// 			for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
// 			{
// 				(*itr)->initChild(*AvalancheRulesItr);
// 			}
// 		}
// 
// 		//check if there is a circle in the tree
// 		for(std::vector<AvalancheSuppressionRule*>::iterator itr=AvalancheRules.begin();itr!=AvalancheRules.end();itr++)
// 		{
// 			if((*itr)->existCircle())
// 			{
// 				AvalancheRules.clear();
// 				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "There are circle of alarm rule, please check the configuration of database" );
// 				// Need to terminate with "don't restart me" message.            
// 				m_genericAgent->terminateWithoutRestart();
// 
// 
// 				// Finally throw an exception, so the data loader will stop trying to load any more data
// 				TA_THROW(TA_Base_Core::TransactiveException("There are circle of alarm rule"));
// 			}
// 		}
// 		AvalancheRules.clear();
// 	}

	void RuleRepository::processMMSAlarm( const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "processMMSAlarm" );

		TA_Base_Core::ThreadGuard guard(m_writeLock);

		std::vector<AbstractRule*>::iterator start = m_repository.begin();
		std::vector<AbstractRule*>::iterator end = m_repository.end();
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "processAlarm MMS Rule" );
		while( start != end)
		{
			std::string ruleType = (*start)->getRuleType();
			if( 0 == ruleType.compare(MMS) )
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, "Processing MMS Alarm Rule" );
				(*start)->process(alarm, AbstractRule::SUBMIT);
			}
			start++;
		} 
		LOG( SourceInfo, DebugUtil::FunctionExit, "processMMSAlarm" );
	}

};

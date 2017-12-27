/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/RuleRepository.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class stores all configured alarm rules
  */
#if !defined(RULEREPOSITORY)
#define RULEREPOSITORY

#ifndef WIN32
#include <typeinfo>
#endif

#include <vector>
#include <string>

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "app/alarm/alarm_agent/src/IDataLoader.h"
#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "app/alarm/alarm_agent/src/AvalancheSuppressionRule.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"

namespace TA_Base_Core
{
    class StateUpdateMessageSender; // Forward declaration
    class SuppressionRuleData;
}

namespace TA_Base_App
{
    class DatabaseLoader;   // Forward declaration
    class RuleTimer;

    class RuleRepository : public virtual IDataLoader,
                           public virtual TA_Base_Core::IOnlineUpdatable
    {

    public:

        /**
          * getInstance
          *
          * Returns the single instance of the RuleRepository
          */
        static RuleRepository* getInstance();

        /**
          * ReleaseInstance
          *
          * Release the single instance of the RuleRepository
          */
		static void releaseInstance();

        /**
          * Destructor
          */
        ~RuleRepository();

        /** 
          * addRule
          *
          * This method adds a new rule to the repository
          *
          * @param The pkey of the new rule
          */
        void addRule( unsigned long ruleId );

        /** 
          * removeRule
          *
          * This method removes an existing rule from the repository
          *
          * @param The pkey of the new rule
          */
        void removeRule( unsigned long ruleId );

        /** 
          * updateRule
          *
          * This method updates the details of an existing rule in the repository
          *
          * @param The pkey of the rule
          */
        void updateRule( unsigned long ruleId );

        /** 
          * isAlarmSuppressed
          *
          * @return Return TRUE if the alarm should be suppressed 
          *
          * @param The alarm
          */
        bool isAlarmSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& alarm );

        /**
          * isAlarmAnAvalancheHead
          *
          * @param The alarm
          *
          * @return Return TRUE if the alarm is the head of an avalanche, FALSE otherwise
          */
        bool isAlarmAnAvalancheHead( const TA_Base_Core::AlarmDetailCorbaDef& alarm );

        /** 
          * processAlarm
          *
          * This method takes an alarm and calls process on any rules within
          * the repository that pertain to it.
          *
          * @param The alarm
          */
        void processAlarm( const TA_Base_Core::AlarmDetailCorbaDef& alarm,
                           TA_Base_App::AbstractRule::ETriggerType trigger );

        /** 
          * loadRules
          *
          * This method populates the repository with rules stored in the 
          * database or requests rules from the controlling agent depending on
          * the current operation mode.
          */
        void loadRules();

		void unloadRules();

        /**
          * loadFromDatabase
          *
          * This method overloads the pure virtual loadFromDatabase method in 
          * IDataLoader
          */
        void loadFromDatabase();

        /**
          * requestFromControllingAgent
          *
          * This method overloads the pure virtual requestFromControllingAgent method in 
          * IDataLoader
          */
        void requestFromControllingAgent(); 

        /**
          * hasDataLoaded
          *
          * This method overloads the pure virtual hasDataLoaded method in IDataLoader
          *
          * @return TRUE if data has been successfully loaded, FALSE otherwise
          */
        bool hasDataLoaded();

        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * setGenericAgentObject
         *
         * If a database capacity is hit while loading the rules for the Alarm Agent,
         * the agent needs to notify the system controller of the config error.
         */
        void setGenericAgentObject(TA_Base_Bus::GenericAgent& genericAgent);

        bool doAnyRulesOfTypeApplyToAlarm(const std::type_info& ruleType, unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey);

		std::string getLastAvalancheHead();

		/**
		 * terminateDataLoading()
		 *
		 * This is to terminate any data loading if it is still processing the loading
		 *
		 */
		void terminateDataLoading();


		 /** 
          * processMMSAlarm
          *
          * This method takes an alarm and calls process on MMS rules within
          * the repository that pertain to it.
          *
          * @param The alarm
          */
        void processMMSAlarm( const TA_Base_Core::AlarmDetailCorbaDef& alarm);             


    private:

        static RuleRepository* m_instance;

        // Disable constructors and assignment operator for Singleton
        RuleRepository();
        RuleRepository( const RuleRepository& theRuleRepository);
        RuleRepository& operator=(const RuleRepository&);

        /** 
          * findRule
          *
          * This method takes an alarm rule and an iterator. If the alarm rule
          * exists within the repository, the method returns TRUE and sets the 
          * iterator to point to the rule.
          *
          * @return TRUE if the alarm exists in the repository
          *
          * @param The rule
          * @param This iterator will point to the alarm within the repository
          *        if it is found.
          */
        bool findRule( AbstractRule* rule, std::vector<AbstractRule*>::iterator& itr );

        /** 
          * findRule
          *
          * This method takes an alarm rule *key* and an iterator. If the alarm rule
          * exists within the repository, the method returns TRUE and sets the 
          * iterator to point to the rule.
          *
          * @return TRUE if the alarm exists in the repository
          *
          * @param The rule
          * @param This iterator will point to the alarm within the repository
          *        if it is found.
          */
        bool findRule( unsigned long key, std::vector<AbstractRule*>::iterator& itr );

        /**
          * createSuppressionRules
          *
          * This method reads the list of configured suppression rules from the database
          * and generates SuppressionRule objects.
          */
        void createSuppressionRules();

        /**
          * createMMSRules
          *
          * This method reads the list of configured MMS rules from the database
          * and generates MMSRule objects.
          */
        void createMMSRules();

        /**
          * createCCTVRules
          *
          * This method reads the list of configured CCTV rules from the database
          * and generates CCTVRule objects.
          */
        void createCCTVRules();

        /**
          * createAutoAcknowledgeRules
          *
          * This method reads the list of configured automatic acknowledgement rules from the database
          * and generates AutoAcknowledgementRule objects.
          */
        void createAutoAcknowledgeRules();

        /**
          * createAvalancheSuppressionRules
          *
          * This method reads the list of configured avalanche suppression rules from the database
          * and generates AvalancheSuppressionRule objects.
          */
        void createAvalancheSuppressionRules();

        /**
          * createPlanExecutionRules
          *
          * This method reads the list of configured plan execution rules from the database
          * and generates PlanExecutionRule objects.
          */
        void createPlanExecutionRules();

        /**
          * convertIAlarmRuleDataToAbstractRule
          *
          * This method converts an IAlarmRuleData object to an AbstractRuleObject
          *
          * @param Pointer to an IAlarmRuleData object
          *
          * @return Pointer to an AbstractRule object
          */
        AbstractRule* convertIAlarmRuleDataToAbstractRule( TA_Base_Core::IAlarmRuleData* rule );

        /**
          * stringToTrigger
          *
          * This method takes a string representation of a Trigger type and returns
          * a TA_Base_App::AbstractRule::ETriggerType
          *
          * @param The string representation of the Trigger
          *
          * @return The TA_Base_App::AbstractRule::ETriggerType
          */
        TA_Base_App::AbstractRule::ETriggerType stringToTrigger( const std::string& trigger );

        /**
          * aquireReadLock
          *
          * This method performs the necessary operations when aquiring the read lock
          */
       // void aquireReadLock(); //remove it, the "m_readCount" is not initilized

        /**
          * releaseReadLock
          *
          * This method performs the necessary operations when release the read lock
          */
        //void releaseReadLock(); //remove it, the "m_readCount" is not initilized

        /**
          * setSuppressionRuleTimer
          *
          * This method sets a timer if the suppression rule was a rule created outside the
          * Configuration Editor.
          */
        void setSuppressionRuleTimer(TA_Base_Core::SuppressionRuleData* suppressionRule);

//		void createAvalancheSuppressionRulesTest();

        // Vector containing all configured rules
        std::vector<AbstractRule*> m_repository;

        // Agent entity key
        unsigned long m_entityKey;

        // Agent entity name
        std::string m_entityName;

        // DatabaseLoader thread (used to periodically attempt to load our rules
        // if the first attempt fails)
        TA_Base_App::DatabaseLoader* m_dbLoader;

        TA_Base_Core::ReEntrantThreadLockable m_writeLock;
        //TA_Base_Core::NonReEntrantThreadLockable m_readLock;  //remove it, the "m_readCount" is not initilized
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
        //int m_readCount;

        bool m_dataLoaded;

        RuleTimer* m_ruleTimer;
        TA_Base_Bus::GenericAgent* m_genericAgent;

        static const unsigned int MAX_AVALANCHE_RULES;
        static const unsigned int MAX_AVALANCHE_CHILDREN;

		AvalancheSuppressionRule* m_lastAvalancheSuppressionRule;
    };
}

#endif // !defined(RULEREPOSITORY)

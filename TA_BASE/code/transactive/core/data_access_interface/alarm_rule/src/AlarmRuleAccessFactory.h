/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AlarmRuleAccessFactory is a singleton that is used to retrieve AlarmRuleData objects 
  * from the database. It is able to create new objects, and provide access to the
  * Alarm Rule database tables.
  *
  */

#if !defined(ALARMRULEACCESSFACTORY_H)
#define ALARMRULEACCESSFACTORY_H

#include "core/data_access_interface/src/IDatabase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"

#include <string>
#include <vector>


namespace TA_Base_Core
{

    // Forward declaration
    class IAlarmRuleData;
	class IDatabase;

    class AlarmRuleAccessFactory
    {

    public:
        virtual ~AlarmRuleAccessFactory() {}

        /**
          * getInstance
          *
          * Creates and returns an instance of this object.
          *
          * @return A reference to an instance of an AlarmRuleAccessFactory object.
          */
        static AlarmRuleAccessFactory& getInstance();


        /** 
          * getAlarmRule
          *
          * Returns the AlarmRule associated with the primary key provided,
          * as a pointer to an object conforming to the IAlarmRuleData
          * interface.
          *
          * @return IAlarmRuleData*   A pointer to an object containing data
          *                              about the alarm rule.
          *
          * @param pkey  The primary key of the Alarm Rule in the database.
          *
          * @exception DatabaseException Thrown if there is a problem
          *                              communicating with the database.
          * @exception DataException Thrown if no alarm rule matching the 
          *               pkey is found (NO_VALUE), or if more than
          *               one alarm rule is found (NOT_UNIQUE).
          */
        IAlarmRuleData* getAlarmRule(unsigned long pkey);


        /** 
          * getAllAlarmRules
          *
          * Returns all alarm rules available.
          *
          * @param 
          *
          * @return A vector of pointers to objects conforming to the
          *         IAlarmRuleData interface.
          *
          */
        std::vector<IAlarmRuleData*> getAllAlarmRules();

        


        /** 
          * getAllSuppressionRules
          *
          * Gets all alarm rules from the database that are of the suppression
          * type.
          *
          * @return a vector of SuppressionRuleData pointers.
          *
          */
        std::vector<SuppressionRuleData*> getAllSuppressionRules();


        /** 
          * getSuppressionRulesByLocation
          *
          * Gets the alarm rules from the database that are of the suppression
          * type and were created under the location specified.
          *
          * @return a vector of SuppressionRuleData pointers.
          *
          */
        std::vector<SuppressionRuleData*> getSuppressionRulesByLocation(unsigned long locationKey);

        /** 
          * getAllPlanExecutionRules
          *
          * Gets all alarm rules from the database that are of the plan
          * execution type.
          *
          * @return a vector of PlanExecutionRuleData pointers.
          *
          */
        std::vector<PlanExecutionRuleData*> getAllPlanExecutionRules();

        /** 
          * getAllAutomaticAcknowledgementRules
          *
          * Gets all alarm rules from the database that are of the automatic
          * acknowledgement type.
          *
          * @return a vector of AutomaticAcknowledgementData pointers.
          *
          */
        std::vector<AutomaticAcknowledgementRuleData*>
            getAllAutomaticAcknowledgementRules();

        /** 
          * getAllAvalancheSuppressionRules
          *
          * Gets all alarm rules from the database that are of the avalanche
          * suppression type.
          *
          * @return a vector of AvalancheSuppressionRuleData pointers.
          *
          */
        std::vector<AvalancheSuppressionRuleData*>
            getAllAvalancheSuppressionRules();


        /** 
          * getAllAvalancheSuppressionRules
          *
          * Gets all alarm rules from the database that are of the avalanche
          * suppression type, and that are for entities at the given location.
          *
          * @param locationKey  the pkey in the location table of the location
          *                     on which the results are filtered.
          *
          * @return a vector of AvalancheSuppressionRuleData pointers.
          *
          */
        std::vector<AvalancheSuppressionRuleData*>
            getAllAvalancheSuppressionRules(unsigned long locationKey);


        /** 
          * getAllAvalancheSuppressionRules
          *
          * Gets all alarm rules from the database that are of the avalanche
          * suppression type, and that are for entities at the given location.
          *
          * @param locationName  the name in the location table of the location
          *                      on which the results are filtered.
          *
          * @return a vector of AvalancheSuppressionRuleData pointers.
          *
          */
        std::vector<AvalancheSuppressionRuleData*>
            getAllAvalancheSuppressionRules(const std::string& locationName);

        /** 
          * getAllMMSRules
          *
          * Gets all alarm rules from the database that are of the MMS
          * type.
          *
          * @return a vector of MMSRuleData pointers.
          *
          */
        std::vector<MMSRuleData*> getAllMMSRules();

        /** 
          * getAllCCTVRules
          *
          * Gets all alarm rules from the database that are of the CCTV
          * type.
          *
          * @return a vector of CCTVRuleData pointers.
          *
          */
        std::vector<CCTVRuleData*> getAllCCTVRules();

        /** 
          * createAlarmRule
          *
          * Creates a new Alarm Rule with the given properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleType       The rule type associated with the alarm rule.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          * @return a pointer to an object containing data about the alarm
          *         rule.
          */
        IAlarmRuleData* createAlarmRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypekey,
            const std::string& ruleType,
            const std::string& ruleTrigger);


        /** 
          * createSuppressionRule
          *
          * Creates a new Suppression Alarm Rule with the given properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          * @param createdBySessionKey  The key of the session under which the alarm rule was created. If the
          *                             rule is created by the Configuration Editor, this parameter should not
          *                             be specified.
          *
          * @return a pointer to an object containing data about the
          *         suppression rule.
          *
          */
        SuppressionRuleData* createSuppressionRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            const std::string& createdBySessionKey = "Unknown",
			const std::string& assetName = "",
			const std::string& description = "",
			const std::string& value = "");

        SuppressionByDescriptionRuleData* createSuppressionByDescriptionRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
			const std::string& description, // description must be provided for this suppression rule
            const std::string& createdBySessionKey = "Unknown",
			const std::string& assetName = "",
			const std::string& value = "");        

        /** 
          * createSuppressionRule
          *
          * Creates a new Suppression Alarm Rule with blank fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        SuppressionRuleData* createSuppressionRule();
		SuppressionByDescriptionRuleData* createSuppressionByDescriptionRule();


        /** 
          * createPlanExecutionRule
          *
          * Creates a new Plan Execution Alarm Rule with the given properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          * @param planPath       The full path of the plan.
          *
          * @return a pointer to an object containing data about the plan
          *         execution rule.
          *
          */
        PlanExecutionRuleData* createPlanExecutionRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            const std::string& planPath);


        /** 
          * createPlanExecutionRule
          *
          * Creates a new Plan Execution Alarm Rule with blank fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        PlanExecutionRuleData* createPlanExecutionRule();


        /** 
          * createAutomaticAcknowledgementRule
          *
          * Creates a new AutomaticAcknowledgement Alarm Rule with the given
          * properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          * @return a pointer to an object containing data about the
          *         Automatic Acknowledgement rule.
          *
          */
        AutomaticAcknowledgementRuleData* createAutomaticAcknowledgementRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);


        /** 
          * createAutomaticAcknowledgementRule
          *
          * Creates a new Automatic Acknowledgement Alarm Rule with blank
          * fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        AutomaticAcknowledgementRuleData* createAutomaticAcknowledgementRule();


        /** 
          * createAvalancheSuppressionRule
          *
          * Creates a new AvalancheSuppression Alarm Rule with the given
          * properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          * @param timeout                  The timeout in milli-seconds.
          * @param entityKeyAlarmTypePairs  The entity key/alarm type pairs
          *                                 that define the suppression rules.
          *
          * @return a pointer to an object containing data about the
          *         Avalanche Suppression rule.
          *
          */
        AvalancheSuppressionRuleData* createAvalancheSuppressionRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            unsigned long timeout,
            unsigned long timein,
            unsigned long minAlarms,
            const std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>&
                entityKeyAlarmTypePairs);


        /** 
          * createAvalancheSuppressionRule
          *
          * Creates a new Avalanche Suppression Alarm Rule with blank fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        AvalancheSuppressionRuleData* createAvalancheSuppressionRule();


        /** 
          * createMMSRule
          *
          * Creates a new MMS Alarm Rule with the given properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          * @return a pointer to an object containing data about the
          *         mms rule.
          *
          */
        MMSRuleData* createMMSRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);

        

        /** 
          * createMMSRule
          *
          * Creates a new MMS Alarm Rule with blank fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        MMSRuleData* createMMSRule();

        /** 
          * createCCTVRule
          *
          * Creates a new CCTV Alarm Rule with the given properties.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          * @return a pointer to an object containing data about the
          *         mms rule.
          *
          */
        CCTVRuleData* createCCTVRule(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);

        

        /** 
          * createCCTVRule
          *
          * Creates a new CCTV Alarm Rule with blank fields.
          *
          * @return a pointer to an object containing data about the
          *         new rule.
          *
          */
        CCTVRuleData* createCCTVRule();

        /** 
          * copySuppressionRule
          *
          * Creates a copy of the given SuppressionRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        SuppressionRuleData* copySuppressionRule(
			const SuppressionRuleData& original);

        /** 
          * copySuppressionByDescriptionRule
          *
          * Creates a copy of the given copySuppressionByDescriptionRule
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        SuppressionByDescriptionRuleData* copySuppressionByDescriptionRule(
			const SuppressionByDescriptionRuleData& original);


        /** 
          * copyPlanExecutionRule
          *
          * Creates a copy of the given PlanExecutionRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        PlanExecutionRuleData* copyPlanExecutionRule(
            const PlanExecutionRuleData& original);


        /** 
          * copyAutomaticAcknowledgementRule
          *
          * Creates a copy of the given AutomaticAcknowledgementRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        AutomaticAcknowledgementRuleData* copyAutomaticAcknowledgementRule(
            const AutomaticAcknowledgementRuleData& original);


        /** 
          * copyAvalancheSuppressionRule
          *
          * Creates a copy of the given AvalancheSuppressionRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        AvalancheSuppressionRuleData* copyAvalancheSuppressionRule(
            const AvalancheSuppressionRuleData& original);

        /** 
          * copyMMSRule
          *
          * Creates a copy of the given MMSRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        MMSRuleData* copyMMSRule(
            const MMSRuleData& original);

        /** 
          * copyCCTVRule
          *
          * Creates a copy of the given CCTVRuleData
          * object.
          *
          * @return a copy of the object that has not yet been written to the
          *         database.
          *
          * @param original  the alarm rule to copy.
          */
        CCTVRuleData* copyCCTVRule(
            const CCTVRuleData& original);

        /** 
          * getAllRuleTriggers
          *
          * Gets all rule triggers that can be set in an alarm rule and
          * returns them as a vector of strings.
          *
          * @return  the allowable rule triggers.
          *
          */
        std::vector<std::string> getAllRuleTriggers();

        //TD15302 Mintao++
        /** 
          * getMMSRuleForAlarmTypeKey
          *
          * Gets the MMS rule from the database that are of the MMS
          * type.
          *
          * @return a vector of MMSRuleData pointers.
          *
          */
        MMSRuleData* getMMSRuleForAlarmTypeKey(unsigned long alarmtypekey);
        //TD15302 Mintao++

	static const std::string RPARAM_MMS_SUBMITTER_TYPE; //TD16491++

    protected:


        /** 
          * getKeys
          *
          * Given an sql statement that returns a single column of unsigned
          * long data, returns the column as a vector.
          * This is used to retrieve the pkeys of Alarm Rules.
          *
          * @return the column of data as a vector of unsigned longs.
          *
          * @param sql  the statement to execute.
          */
        std::vector<unsigned long> getKeys(IDatabase* pDatabase, const SQLStatement& sql);



        /** 
          * getAlarmRuleFromKeyAndType
          *
          * Given an Alarm Rule's key and type, returns an IAlarmRuleData
          * pointer that may be down-cast to the appropriate type.
          * (eg SuppressionRuleData*)
          *
          * @return an IAlarmRuleData pointer of the appropriate type.
          *
          * @param pkey      the pkey of the alarm rule
          * @param ruleType  the type of the rule
          *                  eg SuppressionRuleData::RULE_TYPE
          *
          */
        IAlarmRuleData* getAlarmRuleFromKeyAndType(
            unsigned long pkey, const std::string& ruleType);


    private:
        AlarmRuleAccessFactory() {};
        AlarmRuleAccessFactory( const AlarmRuleAccessFactory& theAlarmRuleAccessFactory);
        AlarmRuleAccessFactory& operator=(const AlarmRuleAccessFactory &);
    
        static AlarmRuleAccessFactory* m_instance;

        static NonReEntrantThreadLockable m_singletonLock;
    };

} // closes TA_Base_Core

   
#endif // ALARMRULEACCESSFACTORY_H

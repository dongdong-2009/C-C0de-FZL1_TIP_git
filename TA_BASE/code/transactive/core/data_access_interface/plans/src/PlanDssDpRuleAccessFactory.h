/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PlanDssDpRuleAccessFactory is a singleton that is used to retrieve PlanDssDpRule objects either from the
  * database or newly created. All PlanDssDpRule objects returned will adhere to the IPlanDssDpRule interface.
  */



#if !defined(PlanDssDpRuleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PlanDssDpRuleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>


namespace TA_Base_Core
{

    class IPlanDssDpRule;
    class IConfigPlanDssDpRule;

    enum DenyMultipleStrategy
    {
        StrategyNoDeny = 0,
        StrategyDeactivateExists,
        StrategyRejectNew,
        StrategyCount
    };

    const char* const DenyMultipleStrategyString[StrategyCount] = 
    {
        "No Strategy",
        "Deactivate exist active plan",
        "Reject new active plan"
    };

	class PlanDssDpRuleAccessFactory
	{

	private:
        // Disable the following methods
		PlanDssDpRuleAccessFactory();
		PlanDssDpRuleAccessFactory( const PlanDssDpRuleAccessFactory& thePlanDssDpRuleAccessFactory);
		PlanDssDpRuleAccessFactory& operator=(const PlanDssDpRuleAccessFactory &);

	public:
        
        /** 
         * ~PlanDssDpRuleAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PlanDssDpRuleAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PlanDssDpRuleAccessFactory object.
         */
		static PlanDssDpRuleAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();
        
        /**
         * getPlanDssDpRule
         *
         * Retrieves the specified PlanDssDpRule and returns the associated data as an 
         * object conforming to the IPlanDssDpRule interface.
         *
         * @param   key        the primary key/id of the PlanDssDpRule to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPlanDssDpRule objects or set to false to return 
         *                     readonly IPlanDssDpRule objects.
         *
         * @return  A pointer to an object conforming to the IPlanDssDpRule interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPlanDssDpRule object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PlanDssDpRule matching the supplied key
         *          - if there is more than one PlanDssDpRule matching the supplied key. 
         *
         */
		IPlanDssDpRule* getPlanDssDpRule(const unsigned long key, const bool readWrite);

        /**
         * getPlanDssDpRuleByLocationKey
         *
         * Retrieves the specified PlanDssDpRule and returns the associated data as an 
         * object conforming to the IPlanDssDpRule interface.
         *
         * @param   key        the locationKey of the PlanDssDpRule to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPlanDssDpRule objects or set to false to return 
         *                     readonly IPlanDssDpRule objects.
         *
         * @return  A pointer to an object conforming to the IPlanDssDpRule interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPlanDssDpRule object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PlanDssDpRule matching the supplied key
         *          - if there is more than one PlanDssDpRule matching the supplied key. 
         *
         */
		std::vector<IPlanDssDpRule*> getPlanDssDpRuleByLocationKey(const unsigned long locationKey, const bool readWrite);

        /**
         * getAllPlanDssDpRules
         *
         * Retrieves all PlanDssDpRules and returns the associated data as a vector of  
         * objects conforming to the IPlanDssDpRule interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPlanDssDpRule objects or set to false to return 
         *                     readonly IPlanDssDpRule objects.
         *
         * @return  a vector of pointers to objects conforming to the IPlanDssDpRule interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPlanDssDpRule object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::vector<IPlanDssDpRule*> getAllPlanDssDpRules( const bool readWrite = false );
        

        /**
         * createPlanDssDpRule
         * 
         * Creates an empty IConfigPlanDssDpRule object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPlanDssDpRule object.
         *         
         * @return A pointer to an IConfigPlanDssDpRule object
         *
         */
		IConfigPlanDssDpRule* createPlanDssDpRule();


        /**
         * copyPlanDssDpRule
         * 
         * Creates a new configuration PlanDssDpRule object using the PlanDssDpRule passed in
         *
         * @param  PlanDssDpRuleToCopy the ConfigPlanDssDpRule to copy
         *
         * @return the new object as a pointer to an IConfigPlanDssDpRule object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPlanDssDpRule* copyPlanDssDpRule(const IConfigPlanDssDpRule* PlanDssDpRuleGroupToCopy);


    private:

        static PlanDssDpRuleAccessFactory* m_instance;

        /**
         * getPlanDssDpRulesReturnedFromStatement
         *
         * Retrieves all PlanDssDpRules that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPlanDssDpRule objects or set to false to return 
         *                     readonly IPlanDssDpRule objects.
         *
         * @return  a vector of pointers to objects conforming to the IPlanDssDpRule interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPlanDssDpRule object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::vector<IPlanDssDpRule*> getPlanDssDpRulesReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PlanDssDpRuleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

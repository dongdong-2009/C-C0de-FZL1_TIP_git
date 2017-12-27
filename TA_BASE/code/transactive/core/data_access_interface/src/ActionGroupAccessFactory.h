/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionGroupAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ActionGroupAccessFactory is a singleton that is used to retrieve ActionGroup objects either from the
  * database or newly created. All ActionGroup objects returned will adhear to the IActionGroup interface.
  * Data is primarily retrieved from the SE_ACTION_GROUP table
  *
  */



#if !defined(ACTIONGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ACTIONGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IActionGroup;
    class IConfigActionGroup;

	class ActionGroupAccessFactory
	{

	public:
        virtual ~ActionGroupAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ActionGroupAccessFactory object.
         */
		static ActionGroupAccessFactory& getInstance();


        /**
         * getActionGroup
         *
         * Retrieves the ActionGroup of the key provided.
         *
         * @return A pointer to an object conforming to the IActionGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IActionGroup 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        IActionGroup* getActionGroup( const unsigned long actionGroupKey, const bool readWrite = false );

        /**
         * getAllActionGroups
         *
         * Retrieves all specified ActionGroups and returns the associated data as a vector of  
         * objects conforming to the IActionGroup interface.
         *
         * @return A vector of pointers to objects conforming to the IActionGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IActionGroup 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IActionGroup*> getAllActionGroups( const bool readWrite = false );


        /**
         * createActionGroup
         * 
         * Creates a new configuration ActionGroup object.
         *
         * @return The configuration ActionGroup object as a pointer to an IConfigActionGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigActionGroup* createActionGroup();

        void getAllActionGroupKeys(std::vector<unsigned long>& groupKeys);
	std::string getActionGroupName(unsigned long key);
        void getAllActionGroupNames(std::map<unsigned long, std::string>& groupNames);


        /**
         * copyActionGroup
         * 
         * Creates a new configuration ActionGroup object using the ActionGroup passed in
         *
         * @param IConfigActionGroup* The ActionGroup to copy
         *
         * @return The configuration ActionGroup object as a pointer to an IConfigActionGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigActionGroup* copyActionGroup(const IConfigActionGroup* actionGroupToCopy);


    private:
        //
        // These are private as this method is a singleton
        //
		ActionGroupAccessFactory();
		//avoid copy constructor
		ActionGroupAccessFactory( const ActionGroupAccessFactory& theActionGroupAccessFactory);
		//avoid copy constructor
		ActionGroupAccessFactory& operator=(const ActionGroupAccessFactory &);
		//initialize action group
		void initializeActionGroup();
		//get action by action group
		void getActionByActionGroup(unsigned long actionGroupId, std::vector<unsigned long>& actions);
		
		/**
         * getActionGroup
         *
         * Retrieves the ActionGroup of the sql statement provided.
         *
         */
		void getActionGroupBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IActionGroup*>& actionGroups, const bool readWrite = false);

		
		typedef std::map< unsigned long, std::vector<unsigned long> > ActionGroupToActionMap;
		ActionGroupToActionMap m_actionGroupToActionMap;
		
		static ActionGroupAccessFactory* m_instance;
		bool m_initialized;
    };

} // closes TA_Base_Core

#endif // !defined(ActionGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

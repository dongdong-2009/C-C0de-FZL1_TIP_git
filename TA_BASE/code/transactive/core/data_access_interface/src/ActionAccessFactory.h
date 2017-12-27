/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
  * database or newly created. All action objects returned will adhear to the IAction interface.
  * Data is primarily retrieved from the SE_ACTION table
  *
  */


#if !defined(ACTIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ACTIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>
#include <set>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IAction;

	class ActionAccessFactory
	{

	public:
		virtual ~ActionAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ActionAccessFactory object.
         */
		static ActionAccessFactory& getInstance();

        /**
         * getAction
         *
         * Retrieves the action of the key provided.
         *
         * @return A pointer to an object conforming to the IAction interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAction
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
        IAction* getAction( const unsigned long actionKey );


        /**
         * getAllActions
         *
         * Retrieves all specified actions and returns the associated data as a vector of  
         * objects conforming to the IAction interface.
         *
         * @return A vector of pointers to objects conforming to the IAction interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAction
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
        std::vector<IAction*> getAllActions();

        //limin++ for centralize duty/rights                action_group   action_keys
        typedef std::map< unsigned long, std::vector< unsigned long > > ActionGroupMap;
        void getActionGroupMap( const std::set< unsigned long >& groupIdList, ActionGroupMap& actionGroupMap );
        void getNonPhysicalSubsystemActionGroup(const std::vector<unsigned long>& profileList, std::multimap<unsigned long, unsigned long>& groupIdList);

    private:
        //
        // These are private as this method is a singleton
        //
		ActionAccessFactory();

		void initializeAction();
		void getActionGroupByAction(unsigned long actionId, std::vector<std::string>& actionGroups);
		void getActionBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAction*>& actions);
		
		ActionAccessFactory( const ActionAccessFactory& theActionAccessFactory);
		ActionAccessFactory& operator=(const ActionAccessFactory &);

		typedef std::vector < std::string > ActionGroupVector;
		
		typedef std::map< unsigned long, std::vector< std::string > > ActionToActionGroupMap;
		ActionToActionGroupMap m_actionToActionGroupMap;

		static ActionAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(ActionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

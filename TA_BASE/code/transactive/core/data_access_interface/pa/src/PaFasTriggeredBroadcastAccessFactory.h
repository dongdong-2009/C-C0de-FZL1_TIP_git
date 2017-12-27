/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  * 
  * PaFasTriggeredBroadcastAccessFactory is a singleton that is used to retrieve PaFasTriggeredBroadcast objects either from the
  * database or newly created. All PaFasTriggeredBroadcast objects returned will adhere to the IPaFasTriggeredBroadcast interface.
  */



#if !defined(PaFasTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaFasTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaFasTriggeredBroadcast.h"
#include "core/data_access_interface/pa/src/IConfigPaFasTriggeredBroadcast.h"



namespace TA_Base_Core
{
	class PaFasTriggeredBroadcastAccessFactory
	{
    public:

        struct FasAlarmEntity  
        {
            unsigned long pKey;
            unsigned long locaionKey;
            std::string entityName;
            std::string entityDescription;
        };   
        typedef std::vector<FasAlarmEntity> FasAlarmEntities;

	private:
        
        // Disable the following methods
		PaFasTriggeredBroadcastAccessFactory() { };
		PaFasTriggeredBroadcastAccessFactory( const PaFasTriggeredBroadcastAccessFactory& thePaFasTriggeredBroadcastAccessFactory);
		PaFasTriggeredBroadcastAccessFactory& operator=(const PaFasTriggeredBroadcastAccessFactory &);


	public:
        
        
        /** 
         * ~PaFasTriggeredBroadcastAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaFasTriggeredBroadcastAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaFasTriggeredBroadcastAccessFactory object.
         */
		static PaFasTriggeredBroadcastAccessFactory& getInstance();


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
         * getPaFasTriggeredBroadcast
         *
         * Retrieves the specified PaFasTriggeredBroadcast and returns the associated data as an 
         * object conforming to the IPaFasTriggeredBroadcast interface.
         *
         * @param   key        the primary key/id of the PaFasTriggeredBroadcast to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaFasTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaFasTriggeredBroadcast objects.
         *
         * @return  A pointer to an object conforming to the IPaFasTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaFasTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaFasTriggeredBroadcast matching the supplied key
         *          - if there is more than one PaFasTriggeredBroadcast matching the supplied key. 
         *
         */
		IPaFasTriggeredBroadcast* getPaFasTriggeredBroadcast(const unsigned long key, const bool readWrite);


        /**
         * getAllPaFasTriggeredBroadcasts
         *
         * Retrieves all PaFasTriggeredBroadcasts and returns the associated data as a vector of  
         * objects conforming to the IPaFasTriggeredBroadcast interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaFasTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaFasTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaFasTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaFasTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaFasTriggeredBroadcasts getAllPaFasTriggeredBroadcasts( const bool readWrite = false );

        
        /**
         * getPaFasTriggeredBroadcastsByLocationKey
         *
         * Retrieves all PaFasTriggeredBroadcastss associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA ATS Triggered Broadcasts from
         *                     each PA ATS Triggered Broadcast with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaFasTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaFasTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaFasTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaFasTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaFasTriggeredBroadcasts getPaFasTriggeredBroadcastsByLocationKey(unsigned long locationKey, const bool readWrite = false );


        /**
         * createPaFasTriggeredBroadcast
         * 
         * Creates an empty IConfigPaFasTriggeredBroadcast object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaFasTriggeredBroadcast object.
         *
         * @return A pointer to an IConfigPaFasTriggeredBroadcast object
         *
         */
		IConfigPaFasTriggeredBroadcast* createPaFasTriggeredBroadcast();


        /**
         * copyPaFasTriggeredBroadcast
         * 
         * Creates a new configuration PaFasTriggeredBroadcast object using the PaFasTriggeredBroadcast passed in
         *
         * @param  PaFasTriggeredBroadcastToCopy the ConfigPaFasTriggeredBroadcast to copy
         *
         * @return the new object as a pointer to an IConfigPaFasTriggeredBroadcast object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaFasTriggeredBroadcast* copyPaFasTriggeredBroadcast(const IConfigPaFasTriggeredBroadcast* PaFasTriggeredBroadcastGroupToCopy);

    private:

        static PaFasTriggeredBroadcastAccessFactory* m_instance;

        /**
         * getPaFasTriggeredBroadcastsReturnedFromStatement
         *
         * Retrieves all PaFasTriggeredBroadcasts that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaFasTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaFasTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaFasTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaFasTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaFasTriggeredBroadcasts getPaFasTriggeredBroadcastsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PaFasTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

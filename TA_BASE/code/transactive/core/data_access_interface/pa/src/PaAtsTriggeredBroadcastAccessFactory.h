/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaAtsTriggeredBroadcastAccessFactory is a singleton that is used to retrieve PaAtsTriggeredBroadcast objects either from the
  * database or newly created. All PaAtsTriggeredBroadcast objects returned will adhere to the IPaAtsTriggeredBroadcast interface.
  */



#if !defined(PaAtsTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaAtsTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/pa/src/IConfigPaAtsTriggeredBroadcast.h"



namespace TA_Base_Core
{

    

	class PaAtsTriggeredBroadcastAccessFactory
	{
    public:

        struct PlatformId  
        {
            unsigned long pKey;
            unsigned long platformId;
            std::string   platformName;
            unsigned long stationId;
        };   
        typedef std::vector<PlatformId> PlatformIds;

	private:
        
        // Disable the following methods
		PaAtsTriggeredBroadcastAccessFactory() { };
		PaAtsTriggeredBroadcastAccessFactory( const PaAtsTriggeredBroadcastAccessFactory& thePaAtsTriggeredBroadcastAccessFactory);
		PaAtsTriggeredBroadcastAccessFactory& operator=(const PaAtsTriggeredBroadcastAccessFactory &);


	public:
        
        
        /** 
         * ~PaAtsTriggeredBroadcastAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaAtsTriggeredBroadcastAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaAtsTriggeredBroadcastAccessFactory object.
         */
		static PaAtsTriggeredBroadcastAccessFactory& getInstance();


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
         * getPaAtsTriggeredBroadcast
         *
         * Retrieves the specified PaAtsTriggeredBroadcast and returns the associated data as an 
         * object conforming to the IPaAtsTriggeredBroadcast interface.
         *
         * @param   key        the primary key/id of the PaAtsTriggeredBroadcast to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaAtsTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaAtsTriggeredBroadcast objects.
         *
         * @return  A pointer to an object conforming to the IPaAtsTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaAtsTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaAtsTriggeredBroadcast matching the supplied key
         *          - if there is more than one PaAtsTriggeredBroadcast matching the supplied key. 
         *
         */
		IPaAtsTriggeredBroadcast* getPaAtsTriggeredBroadcast(const unsigned long key, const bool readWrite);


        /**
         * getAllPaAtsTriggeredBroadcasts
         *
         * Retrieves all PaAtsTriggeredBroadcasts and returns the associated data as a vector of  
         * objects conforming to the IPaAtsTriggeredBroadcast interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaAtsTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaAtsTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaAtsTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaAtsTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaAtsTriggeredBroadcasts getAllPaAtsTriggeredBroadcasts( const bool readWrite = false );

        
        /**
         * getPaAtsTriggeredBroadcastsByLocationKey
         *
         * Retrieves all PaAtsTriggeredBroadcastss associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA ATS Triggered Broadcasts from
         *                     each PA ATS Triggered Broadcast with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaAtsTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaAtsTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaAtsTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaAtsTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaAtsTriggeredBroadcasts getPaAtsTriggeredBroadcastsByLocationKey(unsigned long locationKey, const bool readWrite = false );


        /**
         * createPaAtsTriggeredBroadcast
         * 
         * Creates an empty IConfigPaAtsTriggeredBroadcast object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaAtsTriggeredBroadcast object.
         *
         * @return A pointer to an IConfigPaAtsTriggeredBroadcast object
         *
         */
		IConfigPaAtsTriggeredBroadcast* createPaAtsTriggeredBroadcast();


        /**
         * copyPaAtsTriggeredBroadcast
         * 
         * Creates a new configuration PaAtsTriggeredBroadcast object using the PaAtsTriggeredBroadcast passed in
         *
         * @param  PaAtsTriggeredBroadcastToCopy the ConfigPaAtsTriggeredBroadcast to copy
         *
         * @return the new object as a pointer to an IConfigPaAtsTriggeredBroadcast object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaAtsTriggeredBroadcast* copyPaAtsTriggeredBroadcast(const IConfigPaAtsTriggeredBroadcast* paAtsTriggeredBroadcastGroupToCopy);

        
        /**
         * enumeratePlatformIds
         *
         * Queries the database and returns the set of data present in the
         *  table containing platform ID type information
         *
         * @return an array of PlatformId objects defining available platform IDs
         */
        PlatformIds enumeratePlatformIds();

    private:

        static PaAtsTriggeredBroadcastAccessFactory* m_instance;

        /**
         * getPaAtsTriggeredBroadcastsReturnedFromStatement
         *
         * Retrieves all PaAtsTriggeredBroadcasts that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaAtsTriggeredBroadcast objects or set to false to return 
         *                     readonly IPaAtsTriggeredBroadcast objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaAtsTriggeredBroadcast interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaAtsTriggeredBroadcast object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaAtsTriggeredBroadcasts getPaAtsTriggeredBroadcastsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PaAtsTriggeredBroadcastAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

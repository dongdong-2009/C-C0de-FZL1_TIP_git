/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaStationAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaStationAccessFactory is a singleton that is used to retrieve PaStation objects either from the
  * database or newly created. All PaStation objects returned will adhere to the IPaStation interface.
  */



#if !defined(PaStationAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaStationAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaStation.h"
#include "core/data_access_interface/pa/src/IConfigPaStation.h"



namespace TA_Base_Core
{

    

	class PaStationAccessFactory
	{
    public:
    

	private:
        
        // Disable the following methods
		PaStationAccessFactory() { };
		PaStationAccessFactory( const PaStationAccessFactory& thePaStationAccessFactory);
		PaStationAccessFactory& operator=(const PaStationAccessFactory &);


	public:
        
        
        /** 
         * ~PaStationAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaStationAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaStationAccessFactory object.
         */
		static PaStationAccessFactory& getInstance();


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
         * getPaStation
         *
         * Retrieves the specified PaStation and returns the associated data as an 
         * object conforming to the IPaStation interface.
         *
         * @param   key        the primary key/id of the PaStation to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaStation objects or set to false to return 
         *                     readonly IPaStation objects.
         *
         * @return  A pointer to an object conforming to the IPaStation interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaStation object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaStation matching the supplied key
         *          - if there is more than one PaStation matching the supplied key. 
         *
         */
		IPaStation* getPaStation(const unsigned long key, const bool readWrite);


        /**
         * getAllPaStations
         *
         * Retrieves all PaStations and returns the associated data as a vector of  
         * objects conforming to the IPaStation interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaStation objects or set to false to return 
         *                     readonly IPaStation objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaStation interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaStation object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaStation matching the supplied key
         *          - if there is more than one PaStation matching the supplied key. 
         *
         */
        IPaStations getAllPaStations( const bool readWrite = false );

        /**
         * getPaStationsByLocationKey
         *
         * Retrieves all PaStations associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA DVA Messages from
         *                     each PA Dva Message with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaStation objects or set to false to return 
         *                     readonly IPaStation objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaStation interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaStation object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaStations getPaStationsByLocationKey(unsigned long locationKey, const bool readWrite = false );

        /**
         * createPaStation
         * 
         * Creates an empty IConfigPaStation object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaStation object.
         *
         * @return A pointer to an IConfigPaStation object
         *
         */
		IConfigPaStation* createPaStation();


        /**
         * copyPaStation
         * 
         * Creates a new configuration PaStation object using the PaStation passed in
         *
         * @param  PaStationToCopy the ConfigPaStation to copy
         *
         * @return the new object as a pointer to an IConfigPaStation object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaStation* copyPaStation(const IConfigPaStation* paStationToCopy);
        

    private:

        static PaStationAccessFactory* m_instance;

        /**
         * getPaStationsReturnedFromStatement
         *
         * Retrieves all PaStations that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the PaStations to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaStation objects or set to false to return 
         *                     readonly IPaStation objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaStation interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaStation object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaStations getPaStationsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);

	};

} // closes TA_Base_Core

#endif // !defined(PaStationAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

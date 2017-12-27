/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaZoneAccessFactory is a singleton that is used to retrieve PaZone objects either from the
  * database or newly created. All PaZone objects returned will adhere to the IPaZone interface.
  */



#if !defined(PaZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <list>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaZone.h"
#include "core/data_access_interface/pa/src/IConfigPaZone.h"



namespace TA_Base_Core
{

    

	class PaZoneAccessFactory
	{


	private:
        
        // Disable the following methods
		PaZoneAccessFactory() { };
		PaZoneAccessFactory( const PaZoneAccessFactory& thePaZoneAccessFactory);
		PaZoneAccessFactory& operator=(const PaZoneAccessFactory &);


	public:
        
        
        /** 
         * ~PaZoneAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaZoneAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaZoneAccessFactory object.
         */
		static PaZoneAccessFactory& getInstance();


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
         * getPaZone
         *
         * Retrieves the specified PaZone and returns the associated data as an 
         * object conforming to the IPaZone interface.
         *
         * @param   key        the primary key/id of the PaZone to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZone objects or set to false to return 
         *                     readonly IPaZone objects.
         *
         * @return  A pointer to an object conforming to the IPaZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaZone matching the supplied key
         *          - if there is more than one PaZone matching the supplied key. 
         *
         */
		IPaZone* getPaZone(const unsigned long key, 
                            const bool readWrite);


        /**
         * getAllPaZones
         *
         * Retrieves all PaZones and returns the associated data as a vector of  
         * objects conforming to the IPaZone interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZone objects or set to false to return 
         *                     readonly IPaZone objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZones getAllPaZones( const bool readWrite = false );

        /**
         * getPaZonesByLocationKey
         *
         * Retrieves all PAZones associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA Zones from
         *                     each PA Zone with a locationKey matching this will be returned
         * @param   readWrite  set to true to return writeable
         *                     IConfigPaZone objects or set to false to return 
         *                     readonly IPaZone objects.
         *
         * @return  a pointer to an object conforming to the IPaZone or IConfigPaZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZones getPaZonesByLocationKey(unsigned long locationKey, 
                                         const bool readWrite);

        /**
         * getPaZonesByGroupId
         *
         * Retrieves all PAZones associated with a specific PaZoneGroup, the input parameter
         * to provide the key of the Pa Zone Group we want to return zones from
         *
         * @param   paZoneGroupKey the key defining which Pa Zone Group we want to fetch the
         *                          associated Pa Zones from
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZone objects or set to false to return 
         *                     readonly IPaZone objects.
         *
         * @return  a pointer to an object conforming to the IPaZone or IConfigPaZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZones getPaZonesByGroupId(unsigned long paZoneGroupKey, 
                                     const bool readWrite);
        
        /**
         * getPaZoneKeysByGroupId
         *
         * Retrieves all PA Zone primary keys associated with a specific group, the input 
         * parameter to provide the key of the Pa Zone Group we want to return zones from
         *
         * @param   paZoneGroupKey the key defining which Pa Zone Group we want to fetch the
         *                          associated Pa Zones from
         *
         * @return  an array of all zone id keys corresponding to this group
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::list<unsigned long> getPaZoneKeysByGroupId(unsigned long paZoneGroupKey);


        /**
         * getPagingConsolePaZoneKeys
         *
         * Retrieves all PA Zone primary keys associated with paging consoles. 
         *
         * @return  an array of all zone id keys
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::list<unsigned long> getPagingConsolePaZoneKeys();
        
        /**
         * createPaZone
         * 
         * Creates an empty IConfigPaZone object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaZone object.
         *
         * @return A pointer to an IConfigPaZone object
         *
         */
		IConfigPaZone* createPaZone();


        /**
         * copyPaZone
         * 
         * Creates a new configuration PaZone object using the PaZone passed in
         *
         * @param  PaZoneToCopy the ConfigPaZone to copy
         *
         * @return the new object as a pointer to an IConfigPaZone object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaZone* copyPaZone(const IConfigPaZone* paZoneToCopy);


    private:

        static PaZoneAccessFactory* m_instance;


        /**
         * getPaZonesReturnedFromStatement
         *
         * Retrieves all PaZones that are filtered by where input clause, should be
         * of the form 'LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param tableToJoin (optional) can specify a table we want to perform a join
         *          operation on, that may be used in the where clause
         *
         * @param whereClause the sql statements used to define the PaZones to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZone objects or set to false to return 
         *                     readonly IPaZone objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZones    getPaZonesReturnedFromStatement(const std::string& tableToJoin,
                                                    const std::string& whereClause = "", 
                                                    const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PaZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

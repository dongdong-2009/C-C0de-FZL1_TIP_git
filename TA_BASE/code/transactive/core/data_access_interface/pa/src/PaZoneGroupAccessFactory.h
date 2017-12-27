/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneGroupAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaZoneGroupAccessFactory is a singleton that is used to retrieve PaZoneGroup objects either from the
  * database or newly created. All PaZoneGroup objects returned will adhere to the IPaZoneGroup interface.
  */



#if !defined(PaZoneGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaZoneGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaZoneGroup.h"
#include "core/data_access_interface/pa/src/IConfigPaZoneGroup.h"



namespace TA_Base_Core
{

    

	class PaZoneGroupAccessFactory
	{


	private:
        
        // Disable the following methods
		PaZoneGroupAccessFactory() { };
		PaZoneGroupAccessFactory( const PaZoneGroupAccessFactory& thePaZoneGroupAccessFactory);
		PaZoneGroupAccessFactory& operator=(const PaZoneGroupAccessFactory &);


	public:
        
        
        /** 
         * ~PaZoneGroupAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaZoneGroupAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaZoneGroupAccessFactory object.
         */
		static PaZoneGroupAccessFactory& getInstance();


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
         * getPaZoneGroup
         *
         * Retrieves the specified PaZoneGroup and returns the associated data as an 
         * object conforming to the IPaZoneGroup interface.
         *
         * @param   key        the primary key/id of the PaZoneGroup to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZoneGroup objects or set to false to return 
         *                     readonly IPaZoneGroup objects.
         *
         * @return  A pointer to an object conforming to the IPaZoneGroup interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZoneGroup object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaZoneGroup matching the supplied key
         *          - if there is more than one PaZoneGroup matching the supplied key. 
         *
         */
		IPaZoneGroup* getPaZoneGroup(const unsigned long key, const bool readWrite);


        /**
         * getAllPaZoneGroups
         *
         * Retrieves all PaZoneGroups and returns the associated data as a vector of  
         * objects conforming to the IPaZoneGroup interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZoneGroup objects or set to false to return 
         *                     readonly IPaZoneGroup objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaZoneGroup interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZoneGroup object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZoneGroups getAllPaZoneGroups( const bool readWrite = false );

        
        /**
         * getPaZoneGroupsByLocationKey
         *
         * Retrieves all PaZoneGroupss associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA Zone Groups from
         *                     each PA Zone Group with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZoneGroup objects or set to false to return 
         *                     readonly IPaZoneGroup objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaZoneGroup interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZoneGroup object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZoneGroups getPaZoneGroupsByLocationKey(unsigned long locationKey, const bool readWrite = false );


        /**
         * createPaZoneGroup
         * 
         * Creates an empty IConfigPaZoneGroup object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaZoneGroup object.
         *
         * @return A pointer to an IConfigPaZoneGroup object
         *
         */
		IConfigPaZoneGroup* createPaZoneGroup();


        /**
         * copyPaZoneGroup
         * 
         * Creates a new configuration PaZoneGroup object using the PaZoneGroup passed in
         *
         * @param  PaZoneGroupToCopy the ConfigPaZoneGroup to copy
         *
         * @return the new object as a pointer to an IConfigPaZoneGroup object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaZoneGroup* copyPaZoneGroup(const IConfigPaZoneGroup* paZoneGroupToCopy);


    private:

        static PaZoneGroupAccessFactory* m_instance;

        /**
         * getPaZoneGroupsReturnedFromStatement
         *
         * Retrieves all PaZoneGroups that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the PaZones to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaZoneGroup objects or set to false to return 
         *                     readonly IPaZoneGroup objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaZoneGroup interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaZoneGroup object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaZoneGroups getPaZoneGroupsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PaZoneGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisZoneMappingAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaTisZoneMappingAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */



#ifndef PATISZONEMAPPINGACCESSFACTORY__INCLUDED_
#define PATISZONEMAPPINGACCESSFACTORY__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaTisZoneMap.h"
#include "core/data_access_interface/pa/src/IConfigPaTisZoneMap.h"



namespace TA_Base_Core
{

	class PaTisZoneMappingAccessFactory
	{
    public:
    
	private:
        // Disable the following methods
		PaTisZoneMappingAccessFactory() { };
		PaTisZoneMappingAccessFactory( const PaTisZoneMappingAccessFactory& thePaTisZoneMappingAccessFactory);
		PaTisZoneMappingAccessFactory& operator=(const PaTisZoneMappingAccessFactory &);


	public:
        /** 
         * ~PaTisZoneMappingAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaTisZoneMappingAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaTisZoneMappingAccessFactory object.
         */
		static PaTisZoneMappingAccessFactory& getInstance();


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
         * getAllPaDvaMessages
         *
         * Retrieves all PaDvaMessages and returns the associated data as a vector of  
         * objects conforming to the IPaDvaMessage interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessage objects or set to false to return 
         *                     readonly IPaDvaMessage objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessage matching the supplied key
         *          - if there is more than one PaDvaMessage matching the supplied key. 
         *
         */
        IPaTisZoneMaps getAllPaTisZoneMappings( const bool readWrite = false );


        /**
         * getPaDvaMessagesByLocationKey
         *
         * Retrieves all PaDvaMessages associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA DVA Messages from
         *                     each PA Dva Message with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessage objects or set to false to return 
         *                     readonly IPaDvaMessage objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaTisZoneMaps getPaTisZoneMappingByLocationKey(unsigned long locationKey, const bool readWrite = false );

        IPaTisZoneMaps getPaTisZoneMappingByPaZoneKey(unsigned long zoneKey, const bool readWrite = false);


        /**
         * createPaDvaMessage
         * 
         * Creates an empty IConfigPaDvaMessage object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaDvaMessage object.
         *
         * @return A pointer to an IConfigPaDvaMessage object
         *
         */
		IConfigPaTisZoneMap* createPaTisZoneMapping();


        /**
         * copyPaDvaMessage
         * 
         * Creates a new configuration PaDvaMessage object using the PaDvaMessage passed in
         *
         * @param  PaDvaMessageToCopy the ConfigPaDvaMessage to copy
         *
         * @return the new object as a pointer to an IConfigPaDvaMessage object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaTisZoneMap* copyPaTisZoneMapping(const IConfigPaTisZoneMap* paDvaMessageToCopy);


        /**
         * copyPaDvaMessage
         * 
         * Creates a new configuration PaDvaMessage object using the PaDvaMessage passed in
         *
         * @param  PaDvaMessageToCopy the ConfigPaDvaMessage to copy
         *
         * @return the new object as a pointer to an IConfigPaDvaMessage object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaTisZoneMap* copyPaDvaMessage(const IConfigPaTisZoneMap* paDvaMessageToCopy);


    private:

        static PaTisZoneMappingAccessFactory* m_instance;

        /**
         * getPaDvaMessagesReturnedFromStatement
         *
         * Retrieves all PaDvaMessages that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the PaDvaMessages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessage objects or set to false to return 
         *                     readonly IPaDvaMessage objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaTisZoneMaps getPaTisZoneMappingsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);

	};

} // closes TA_Base_Core

#endif // !defined(PaTisZoneMappingAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

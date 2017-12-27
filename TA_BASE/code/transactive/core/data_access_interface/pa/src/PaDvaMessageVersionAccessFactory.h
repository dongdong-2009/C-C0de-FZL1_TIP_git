/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageVersionAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaDvaMessageVersionAccessFactory is a singleton that is used to retrieve PaDvaMessageVersion objects either from the
  * database or newly created. All PaDvaMessageVersion objects returned will adhere to the IPaDvaMessageVersion interface.
  */



#if !defined(PaDvaMessageVersionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaDvaMessageVersionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaDvaMessageVersion.h"
#include "core/data_access_interface/pa/src/IConfigPaDvaMessageVersion.h"



namespace TA_Base_Core
{

    

	class PaDvaMessageVersionAccessFactory
	{


	private:
        
        // Disable the following methods
		PaDvaMessageVersionAccessFactory() { };
		PaDvaMessageVersionAccessFactory( const PaDvaMessageVersionAccessFactory& thePaDvaMessageVersionAccessFactory);
		PaDvaMessageVersionAccessFactory& operator=(const PaDvaMessageVersionAccessFactory &);


	public:
        
        
        /** 
         * ~PaDvaMessageVersionAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessageVersionAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaDvaMessageVersionAccessFactory object.
         */
		static PaDvaMessageVersionAccessFactory& getInstance();


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
         * getPaDvaMessageVersion
         *
         * Retrieves the specified PaDvaMessageVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessageVersion interface.
         *
         * @param   key        the primary key/id of the PaDvaMessageVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessageVersion objects or set to false to return 
         *                     readonly IPaDvaMessageVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessageVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessageVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessageVersion matching the supplied key
         *          - if there is more than one PaDvaMessageVersion matching the supplied key. 
         *
         */
		IPaDvaMessageVersion* getPaDvaMessageVersion(const unsigned long key, const bool readWrite);

        /**
         * getPaDvaMessageVersionByLocationKey
         *
         * Retrieves the specified PaDvaMessageVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessageVersion interface.
         *
         * @param   key        the locationKey of the PaDvaMessageVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessageVersion objects or set to false to return 
         *                     readonly IPaDvaMessageVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessageVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessageVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessageVersion matching the supplied key
         *          - if there is more than one PaDvaMessageVersion matching the supplied key. 
         *
         */
		IPaDvaMessageVersion* getPaDvaMessageVersionByLocationKey(const unsigned long locationKey, const bool readWrite);

        /**
         * getAllPaDvaMessageVersions
         *
         * Retrieves all PaDvaMessageVersions and returns the associated data as a vector of  
         * objects conforming to the IPaDvaMessageVersion interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessageVersion objects or set to false to return 
         *                     readonly IPaDvaMessageVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessageVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessageVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessageVersions getAllPaDvaMessageVersions( const bool readWrite = false );
        

        /**
         * createPaDvaMessageVersion
         * 
         * Creates an empty IConfigPaDvaMessageVersion object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaDvaMessageVersion object.
         *         
         * @return A pointer to an IConfigPaDvaMessageVersion object
         *
         */
		IConfigPaDvaMessageVersion* createPaDvaMessageVersion();


        /**
         * copyPaDvaMessageVersion
         * 
         * Creates a new configuration PaDvaMessageVersion object using the PaDvaMessageVersion passed in
         *
         * @param  PaDvaMessageVersionToCopy the ConfigPaDvaMessageVersion to copy
         *
         * @return the new object as a pointer to an IConfigPaDvaMessageVersion object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaDvaMessageVersion* copyPaDvaMessageVersion(const IConfigPaDvaMessageVersion* paDvaMessageVersionGroupToCopy);


    private:

        static PaDvaMessageVersionAccessFactory* m_instance;

        /**
         * getPaDvaMessageVersionUsing
         *
         * Retrieves the specified PaDvaMessageVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessageVersion interface.
         *
         * @param   colName    the name of the column to test for equality
         * @param   colValue   the value to test against the column
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessageVersion objects or set to false to return 
         *                     readonly IPaDvaMessageVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessageVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessageVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessageVersion matching the supplied key
         *          - if there is more than one PaDvaMessageVersion matching the supplied key. 
         *
         */
        IPaDvaMessageVersion* getPaDvaMessageVersionUsing(const std::string& colName,
                                                            unsigned long colValue,
                                                            bool readWrite);


        /**
         * getPaDvaMessageVersionsReturnedFromStatement
         *
         * Retrieves all PaDvaMessageVersions that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessageVersion objects or set to false to return 
         *                     readonly IPaDvaMessageVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessageVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessageVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessageVersions getPaDvaMessageVersionsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined(PaDvaMessageVersionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

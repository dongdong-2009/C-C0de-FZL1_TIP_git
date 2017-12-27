/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConsoleAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConsoleAccessFactory is a singleton that is used to retrieve Console objects either from the
  * database or newly created. All Console objects returned will adhear to the IConsole interface.
  */


#if !defined(ConsoleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ConsoleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
//TODO: Uncomment when write access is required
//#include "IConfigConsole.h"

namespace TA_Base_Core
{

	class ConsoleAccessFactory
	{

	public:
        virtual ~ConsoleAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ConsoleAccessFactory object.
         */
		static ConsoleAccessFactory& getInstance();


        /**
         * getConsole
         *
         * Retrieves the specified Console and returns the associated data as an 
         * object conforming to the IConsole interface.
         *
         * @param key The database key to the Console to retrieve
         * @param readwrite This indicates whether the console is read-only or read-write.
         *                  The default is false indicating a read-only console
         *
         * @return A pointer to an object conforming to the IConsole interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IConsole 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Console
         *            matching the supplied PKEY, or if there is more than one Console
         *            matching the supplied PKEY. 
         * @exception EntityTypeException This will be thrown if the pkey specified does not
         *            retrieve an entity of type 'Console'.
         */
		IConsole* getConsole(const unsigned long key, const bool readWrite = false);


        /**
         * getConsole
         *
         * Retrieves the specified Console and returns the associated data as an 
         * object conforming to the IConsole interface.
         *
         * @param name The name of the Console in the database
         * @param readwrite This indicates whether the console is read-only or read-write.
         *                  The default is false indicating a read-only console
         *
         * @return A pointer to an object conforming to the IConsole interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IConsole 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Console
         *            matching the supplied PKEY, or if there is more than one Console
         *            matching the supplied PKEY. Addtionally, a DataException will be thrown
         *            if the entry in the PKEY column matching this Console NAME cannot be
         *            converted to an unsigned long
         * @exception EntityTypeException This will be thrown if the name specified does not
         *            retrieve an entity of type 'Console'.
         */
		IConsole* getConsole(const std::string& name, const bool readWrite = false);

        /**
         * getConsoleFromSession
         *
         * Retrieves the Console associated with the specified session id, and returns
         * the associated data as an ojbect conforming to the IConsole interface.
         *
         * @param unsigned long A session id.
         * @param readwrite This indicates whether the console is read-only or read-write.
         *                  The default is false indicating a read-only console
         *
         * @return A pointer to an object conforming to the IConsole interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IConsole 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no session
         *            matching the supplied key, or if there is more than one session
         *            matching the supplied key, and in the same situations for the Console
         *            key retrieved form the session table.
         * @exception EntityTypeException This will be thrown if the sessionid specified does not
         *            retrieve an entity of type 'Console'.
         */
		IConsole* getConsoleFromSession(const std::string& sessionId, const bool readWrite = false);

        /**
         * getConsoleFromAddress
         *
         * Retrieves the Console with the specified address. This returns
         * the associated data as an ojbect conforming to the IConsole interface.
         *
         * @param name The address of the console. ie the hostname of the workstation
         * @param readwrite This indicates whether the console is read-only or read-write.
         *                  The default is false indicating a read-only console
         *
         * @return A pointer to an object conforming to the IConsole interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IConsole 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no session
         *            matching the supplied key, or if there is more than one session
         *            matching the supplied key, and in the same situations for the Console
         *            key retrieved form the session table.
         * @exception EntityTypeException This will be thrown if the sessionid specified does not
         *            retrieve an entity of type 'Console'.
         */
         IConsole* getConsoleFromAddress(const std::string& address, const bool readWrite = false);

         /**
         * getAllConsoles
         *
         * Retrieves all Consoles and returns the associated data as
         * objects conforming to the IConsole interface.
         *
         * @param readwrite This indicates whether the console is read-only or read-write.
         *                  The default is false indicating a read-only console
         *
         * @return A vector of pointers to objects conforming to the IConsole interface.
         *         If there are no consoles, an empty vector is returned.
         * N.B.: It is the responsibility of the *client* to delete the returned IConsole 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
         std::vector<IConsole*> getAllConsoles(const bool readWrite = false);

		 /** 
		 * getLocationByKey
		 *
		 * Get a location based on the locationKey
		 *
		 * @return ILocation* the
		 *
		 * @param unsigned long locationKey
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there are
		 *            any problems in communicating with the database. The possilble 
		 *            reasons are:
		 *            1) Invalid connection string (as stored in RunParams)
		 *            2) Database is not/cannot be opened
		 *            3) Error while attempting to execute a database query
		 * @exception DataException 
		 */
		IConsole* getConsoleByKey( const unsigned long consoleKey, const bool readWrite = false );

        /**
         * createConsole
         * 
         * Creates an empty IConfigConsole object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigConsole object
         *
         * @return A pointer to an IConfigConsole object
         *
         */
//TODO: Uncomment when write access is required
//			IConfigConsole* createConsole();

	private:
        //
        // Constructors and assigment operator made private as this is a singleton.
        //
		ConsoleAccessFactory();
		ConsoleAccessFactory( const ConsoleAccessFactory& theConsoleAccessFactory);
		ConsoleAccessFactory& operator=(const ConsoleAccessFactory &);
    
        //IConsole* m_console;    // This is a default console that will be used to check
                                // whether the key passed in is a real console. It will then
                                // clone a copy of itself.

        static const std::string KEY;
        static const std::string TYPENAME;
        static const std::string CONSOLEKEY;

        static ConsoleAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(ConsoleAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)


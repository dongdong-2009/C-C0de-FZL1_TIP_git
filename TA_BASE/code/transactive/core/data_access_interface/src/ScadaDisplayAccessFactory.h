/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ScadaDisplayAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ScadaDisplayAccessFactory is a singleton that is used to retrieve display objects either from the
 * database or newly created. All display objects returned will adhear to the IScadaDisplay interface.
 */


#if !defined(ScadaDisplayAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ScadaDisplayAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IScadaDisplay;
    class IConfigScadaDisplay;

	class ScadaDisplayAccessFactory
	{

	private:
        // These are hidden as this class is a singleton.
		ScadaDisplayAccessFactory() 
		{ 
		
		};

		ScadaDisplayAccessFactory( const ScadaDisplayAccessFactory& theScadaDisplayAccessFactory);

		ScadaDisplayAccessFactory& operator=(const ScadaDisplayAccessFactory &);

	public:
        virtual ~ScadaDisplayAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ScadaDisplayAccessFactory object.
         */
		static ScadaDisplayAccessFactory& getInstance();

		
        /**
         * getScadaDisplay
         *
         * Retrieves the specified display and returns the associated data as an 
         * object conforming to the IScadaDisplay interface.
         *
         * @param key The database key to the display to retrieve
         *
         * @return A pointer to an object conforming to the IScadaDisplay interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IScadaDisplay 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no display
         *            matching the supplied PKEY, or if there is more than one display
         *            matching the supplied PKEY. 
         */
		IScadaDisplay* getScadaDisplay(const unsigned long key);


        /**
         * getScadaDisplay
         *
         * Retrieves the specified display and returns the associated data as an 
         * object conforming to the IScadaDisplay interface.
         *
         * @param name The name of the display in the database
         *
         * @return A pointer to an object conforming to the IScadaDisplay interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IScadaDisplay 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no display
         *            matching the supplied PKEY, or if there is more than one display
         *            matching the supplied PKEY. Addtionally, a DataException will be thrown
         *            if the entry in the PKEY column matching this display NAME cannot be
         *            converted to an unsigned long
         */
		IScadaDisplay* getScadaDisplay(const std::string& name);


        /**
         * getScadaDisplaysForLocation
         *
         * Retrieves all displays for a specified location and returns the associated data as a vector of  
         * objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IScadaDisplay interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IScadaDisplay
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IScadaDisplay*> getScadaDisplaysForLocation( const unsigned long locationKey );

        
        /**
         * getAllScadaDisplays
         *
         * Retrieves all displays and returns the associated data as a vector of  
         * objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IScadaDisplay interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IScadaDisplay
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IScadaDisplay*> getAllScadaDisplays( const bool readWrite = false );


        /**
         * createScadaDisplay
         * 
         * Creates an empty IConfigScadaDisplay object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigScadaDisplay object
         *
         * @return A pointer to an IConfigScadaDisplay object
         *
         */
		IConfigScadaDisplay* createScadaDisplay();


        /**
         * copyScadaDisplay
         * 
         * Creates a new configuration display object using the display passed in
         *
         * @param IConfigScadaDisplay* The display to copy
         *
         * @return The configuration display object as a pointer to an IConfigScadaDisplay object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigScadaDisplay* copyScadaDisplay(const IConfigScadaDisplay* displayToCopy);


    private:
        static ScadaDisplayAccessFactory* s_instance;
		
		void getScadaDisplayBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IScadaDisplay*>& displayPointers, const bool readWrite);

	};

} // closes TA_Base_Core

#endif // !defined(ScadaDisplayAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GlobalParameterAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GlobalParameterAccessFactory is a singleton that is used to retrieve GlobalParameter objects either from the
  * database or newly created. All GlobalParameter objects returned will adhear to the IGlobalParameter interface.
  *
  */



#if !defined(GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IGlobalParameter.h"

namespace TA_Base_Core
{
    class IConfigGlobalParameter;

	class GlobalParameterAccessFactory
	{
	public:
        virtual ~GlobalParameterAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an GlobalParameterAccessFactory object.
         */
		static GlobalParameterAccessFactory& getInstance();

		
        /**
         * getAllGlobalParameters
         *
         * Retrieves all specified GlobalParameters and returns the associated data as a vector of  
         * objects conforming to the IGlobalParameter interface.
         *
         * @return A vector of pointers to objects conforming to the IGlobalParameter interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGlobalParameter 
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
        std::vector<IGlobalParameter*> getAllGlobalParameters( const bool readWrite = false );


        /**
         * getAllGlobalParametersForSubsystem
         *
         * Retrieves all specified GlobalParameters and returns the associated data as a vector of  
         * objects conforming to the IGlobalParameter interface.
         *
         * @return A vector of pointers to objects conforming to the IGlobalParameter interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGlobalParameter 
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
        std::vector<IGlobalParameter*> getAllGlobalParametersForSubsystem( unsigned long subsystemKey, 
                                                                           const bool readWrite = false );

        /** 
          * getGlobalParameterByKey
          *
          * Get a GlobalParameter based on the GlobalParameterKey
          *
          * @return IGlobalParameter* the
          *
          * @param unsigned long GlobalParameterKey
		  *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          */
		IGlobalParameter* getGlobalParameterByKey( const unsigned long GlobalParameterKey, const bool readWrite = false );


        /** 
          * getGlobalParameterByName
          *
          * Get a GlobalParameter based on the GlobalParameter name
          *
          * @return IGlobalParameter* the object representing the GlobalParameter with the specified name
          *
          * @param std::string GlobalParameterName The name of the GlobalParameter to retrieve
		  *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          */
        IGlobalParameter* getGlobalParameterByNameAndSubsystem( const std::string& name, 
                                                                unsigned long subsystemKey, 
                                                                const bool readWrite = false );


        /** 
         * getGlobalParameterKeyFromName
         *
         * Gets the GlobalParameter key of a station, given its name.
         *
         * @param GlobalParameterName  the name of the GlobalParameter whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        unsigned long getGlobalParameterKeyFromNameAndSubsystem( const std::string& name, 
                                                                 unsigned long subsystemKey);


        /** 
         * getGlobalParameterValueFromNameAndSubsystem
         *
         * Gets the GlobalParameter value of a subsystem, given its name.
         *
         * @param GlobalParameterName  the name of the GlobalParameter whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        std::string getGlobalParameterValueFromNameAndSubsystem( const std::string& name, 
                                                                 unsigned long subsystemKey);

        /**
         * createGlobalParameter
         * 
         * Creates a new configuration GlobalParameter object.
         *
         * @return The configuration GlobalParameter object as a pointer to an IConfigGlobalParameter object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigGlobalParameter* createGlobalParameter();


        /**
         * copyGlobalParameter
         * 
         * Creates a new configuration GlobalParameter object using the GlobalParameter passed in
         *
         * @param IConfigGlobalParameter* The GlobalParameter to copy
         *
         * @return The configuration GlobalParameter object as a pointer to an IConfigGlobalParameter object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigGlobalParameter* copyGlobalParameter(const IConfigGlobalParameter* GlobalParameterToCopy);


    private:
        //
        // These are private as this method is a singleton
        //
		GlobalParameterAccessFactory() {};
		GlobalParameterAccessFactory( const GlobalParameterAccessFactory& theGlobalParameterAccessFactory);
		GlobalParameterAccessFactory& operator=(const GlobalParameterAccessFactory &);
		
		void getGlobalParameterBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< IGlobalParameter* >& globalParameters, const bool readWrite);


		static GlobalParameterAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

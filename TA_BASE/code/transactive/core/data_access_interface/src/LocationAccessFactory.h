/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/LocationAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * LocationAccessFactory is a singleton that is used to retrieve Location objects either from the
  * database or newly created. All Location objects returned will adhear to the ILocation interface.
  *
  */



#if !defined(LOCATIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define LOCATIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ILocation.h"

namespace TA_Base_Core
{
    class IConfigLocation;

	class LocationAccessFactory
	{
	public:
        virtual ~LocationAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an LocationAccessFactory object.
         */
		static LocationAccessFactory& getInstance();

		
        /**
         * getAllLocations
         *
         * Retrieves all specified Locations and returns the associated data as a vector of  
         * objects conforming to the ILocation interface.
         *
         * @return A vector of pointers to objects conforming to the ILocation interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ILocation 
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
        std::vector<ILocation*> getAllLocations( const bool readWrite = false );

		//xinsong++, exclude location=0
        std::vector<ILocation*> getAllAgentLocations( const bool readWrite = false );

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
		ILocation* getLocationByKey( const unsigned long locationKey, const bool readWrite = false );


        /** 
          * getLocationByName
          *
          * Get a location based on the location name
          *
          * @return ILocation* the object representing the location with the specified name
          *
          * @param std::string locationName The name of the location to retrieve
		  *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          */
        ILocation* getLocationByName( const std::string& locationKey, const bool readWrite = false );

        /**
         * getLocationsByProfile
         *
         * Get all locations associated with the specified profile
         *
         * @return A vector of pointers to ILocation objects that are associated with the specified profile
         *
         * @param profileKey The key to the profile for which to retreive associated locations
         *
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        std::vector<ILocation*> getLocationsByProfile( unsigned long profileKey, const bool readWrite = false );


        /** 
         * getLocationType
         *
         * Get the type of a location given its name.
         *
         * @param locationName  the name of the location whose type is returned.
         *
         * Returns the location type for this location. This is used in applications that differentiate between
         * a central location (or locations), and general locations. For C830 this will determine the OCC, Depot,
         * and station locaitons.
         *
         * @return The location type for this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        ILocation::ELocationType getLocationType(const std::string& locationName) const;
        ILocation::ELocationType getLocationType(const unsigned long locationKey) const;
        

        /** 
         * getOccLocationKey
         *
         * Get the location key of the OCC.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        unsigned long getOccLocationKey();
        unsigned long getOccComLocationKey();
        unsigned long getOccIntLocationKey();
        unsigned long getOccScadaLocationKey();

		std::string getOccComLocationName();


        /** 
         * getDepotLocationKey
         *
         * Get the location key of the Depot.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        unsigned long getDepotLocationKey();

		//hongran++ TD 15473

        /** 
         * getDepotLocationKey
         *
         * Get the location key of the Depot of ND.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		unsigned long getDepotNDLocationKey();
        /** 
         * getDepotLocationKey
         *
         * Get the location key of the Depot of SD.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		unsigned long getDepotSDLocationKey();
        /** 
         * getDepotLocationKey
         *
         * Get the location key of the Depot of MD.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		unsigned long getDepotMDLocationKey();
		//++hongran TD 15473


        /** 
         * getLocationKeyFromName
         *
         * Gets the location key of a station, given its name.
         *
         * @param locationName  the name of the location whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        unsigned long LocationAccessFactory::getLocationKeyFromName(
            const std::string& name);


		/**
		 * Get collection of TIMS origin values.
		 */
		std::map<std::string, unsigned char> getTIMSOriginValues();

        /**
         * createLocation
         * 
         * Creates a new configuration location object.
         *
         * @return The configuration location object as a pointer to an IConfigLocation object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigLocation* createLocation();


        /**
         * copyLocation
         * 
         * Creates a new configuration location object using the location passed in
         *
         * @param IConfigLocation* The location to copy
         *
         * @return The configuration location object as a pointer to an IConfigLocation object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigLocation* copyLocation(const IConfigLocation* locationToCopy);

		unsigned long getDepotGBDLocationKey();

		unsigned long getDepotGDCCLocationKey();

		std::vector<unsigned long> getOrderIdsFromRegionKeys(std::vector<unsigned long> theRegionKeys,
			std::map<unsigned long, std::string>& theOrderIdDisplayNameMap);

        std::vector<ILocation*> getLocationsByType( ILocation::ELocationType locationType, const bool readWrite = false );

	protected:
		LocationAccessFactory() 
		{

		};

    private:
        //
        // These are private as this method is a singleton
        //
		LocationAccessFactory( const LocationAccessFactory& theLocationAccessFactory);
		LocationAccessFactory& operator=(const LocationAccessFactory &);
		
		void getLocationBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< ILocation* >& locations, const bool readWrite);

        ILocation::ELocationType convertLocationName2Type(const std::string& locationName) const; // old logic

		static LocationAccessFactory* m_instance;

        // The names of the OCC and Depot in the database.
        static const std::string OCC_LOCATION_NAME;
        static const std::string OCC_COM_LOCATION_NAME;
        static const std::string OCC_INT_LOCATION_NAME;
        static const std::string OCC_SCADA_LOCATION_NAME;

        static const std::string DEPOT_LOCATION_NAME;
		//hongran++ TD 15473
		static const std::string DEPOT_ND_LOCATION_NAME;
		static const std::string DEPOT_SD_LOCATION_NAME;
		static const std::string DEPOT_MD_LOCATION_NAME;
		//++hongran TD 15473
		
		//955 depots
		static const std::string DEPOT_GBD_LOCATION_NAME;
		static const std::string DEPOT_GDCC_LOCATION_NAME;
		
    };

} // closes TA_Base_Core

#endif // !defined(LocationAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

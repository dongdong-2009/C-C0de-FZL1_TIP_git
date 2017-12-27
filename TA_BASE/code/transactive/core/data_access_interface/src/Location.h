/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Location.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Location is an implementation of ILocation. It holds the data specific to an Location entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(Location_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Location_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/ILocation.h"

namespace TA_Base_Core
{
	class IData;
    class LocationHelper;


    class Location : public ILocation
    {

    public:

        /**
         * Constructor
         *
         * Construct a Location class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Location in the database
         */
        Location( const unsigned long key );

        /**
         * Constructor
         *
         * Construct a complete location, this will set isValidData to true
         *
         * @param key The key of this Location in the database
         * @param name The name of this Location in the database
         * @param description The description of this Location in the database
         * @param dateCreated The date and time the location was created
         * @param dateModified The date and time the location was last modified
         */
        Location( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,const std::string & displayname,
                  const time_t dateCreated, const time_t dateModified,bool displayOnly);


		/**
		 * Constructor
		 *
		 * Construct a Location class based around dataset
		 */
		Location(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Destructor
         */
        virtual ~Location();

        /**
         * getKey
         *
         * Returns the key for this Location.
         *
         * @return The key for this Location as an unsigned long.
         */
        virtual unsigned long getKey();

		/** 
		 * getDisplayName
		 *
		 * Returns the display name for this Location.
		 */
		virtual std::string getDisplayName();

        /**
         * getName
         *
         * Returns the name of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this Location as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();		

        /**
         * getDescription
         *
         * Returns the description of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this location as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();

        /**
         * getOrderId
         *
         * Returns the oderId Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The physical order of this location
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getOrderId();
		

		/**
         * getDefaultDisplay
         *
         * Returns the path for the preferred display for this location. This is so GraphWorx
         * will load the correct display for the current location and profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long profileKey - the profileKey for the display
         *
         * @return The display for this profile as a std::string. This string should be a path.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDefaultDisplay( unsigned long profileKey, int displayNumber );


        /**
         * getLocationType
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
        virtual ILocation::ELocationType getLocationType();        

        /**
         * isProfileAssociated
         *
         * Determines if the specified profile is associated with this location. This can be used to determine if a 
         * profile is valid at the given location.
         *
         * @return true if the profile is associated with the location, otherwise false.
         *
         * @param profileId The key to the profile for which to check the association.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        bool isProfileAssociated(unsigned long profileId);


        /**
         * invalidate
         *
         * Make the data contained by this Location as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

		virtual bool isDisplayOnly();

        virtual std::string getTypeName();
        static ILocation::ELocationType convertType( const std::string& typeName );
        static const std::string& convertType( const ILocation::ELocationType );

    private:
		
        // Assignment operator not used so it is made private
		Location& operator=(const Location&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the LocationHelper
        // has only been written for ConfigLocation objects and will not copy the location helper
        // for a read-only object (ie this one).
        Location( const Location& theLocation);            	
		

        LocationHelper* m_locationHelper;
    };
} // closes TA_Base_Core

#endif // !defined(Location_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

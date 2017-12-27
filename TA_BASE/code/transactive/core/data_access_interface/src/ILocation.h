/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ILocation.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ILocation is an interface to a Location object. It allows the Location object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  * NOTE: This is temporary and must be changed later.
  */

#if !defined(ILocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ILocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class ILocation : public IItem
    {

    public:
        virtual ~ILocation() { };


        /**
         * LocationType enum.
         */
        enum ELocationType
        {
            OCC,
            DPT,
            STATION
        };

		
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
        virtual std::string getDescription() = 0;

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
        virtual unsigned long getOrderId() = 0;

		virtual std::string getDisplayName() = 0;
		
        /**
         * getDefaultDisplay
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile.
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
        virtual std::string getDefaultDisplay(unsigned long profileKey, int displayNumber) = 0;


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
        virtual ELocationType getLocationType() = 0;

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
        virtual bool isProfileAssociated(unsigned long profileId) = 0;

		virtual bool isDisplayOnly() = 0;

        virtual std::string getTypeName() { return ""; }
    };

    typedef boost::shared_ptr<ILocation> ILocationPtr;
} //close namespace TA_Base_Core

#endif // !defined(ILocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

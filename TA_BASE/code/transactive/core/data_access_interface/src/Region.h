/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Region.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Region is an implementation of IRegion. It holds the data specific to an Region entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(REGION_H_INCLUDED)
#define REGION_H_INCLUDED

#include <string>
#include <vector>
#include <ctime>

#include "core/data_access_interface/src/IRegion.h"

namespace TA_Base_Core
{
	class IData;
    class RegionHelper;

    class Region : public IRegion
    {

    public:

        /**
         * Constructor
         *
         * Construct a Region class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Region in the database
         */
        Region( const unsigned long key );

        /**
         * Constructor
         *
         * Construct a complete region, this will set isValidData to true
         *
         * @param key The key of this Region in the database
         * @param name The name of this Region in the database
         * @param description The description of this Region in the database
         * @param dateCreated The date and time the region was created
         * @param dateModified The date and time the region was last modified
         */
        Region( const unsigned long key, const std::string& name, const std::string& description,
                  const time_t dateCreated, const time_t dateModified);

		/**
		 * Constructor
		 *
		 * Construct a Region class based around dataset
		 */
		Region(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Destructor
         */
        virtual ~Region();

        /**
         * getKey
         *
         * Returns the key for this Region.
         *
         * @return The key for this Region as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this Region as a std::string
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
         * Returns the description of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this region as a std::string
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
         * getDisplayName
         *
         * Returns the display name of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The display name for this Region as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDisplayName();		

        /**
         * invalidate
         *
         * Make the data contained by this Region as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		Region& operator=(const Region&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RegionHelper
        // has only been written for ConfigRegion objects and will not copy the region helper
        // for a read-only object (ie this one).
        Region( const Region& theRegion);            	
		

        RegionHelper* m_regionHelper;
    };
} // closes TA_Base_Core

#endif // !defined(REGION_H_INCLUDED)

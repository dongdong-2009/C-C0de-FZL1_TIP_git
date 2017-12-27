/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ECSStationColourData.h $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ECSStationColourData is an implementation of IAlarmStationData. It holds the data specific to a Alarm
 * Station entry in the database.
 */

#ifndef ECS_STATION_COLOUR_DATA_H
#define ECS_STATION_COLOUR_DATA_H

#include <string>

#include "core/data_access_interface/src/IECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourHelper.h"

namespace TA_Base_Core
{

	class ECSStationColourData : public IECSStationColourData
	{

	public:
        /**
         * Constructor
         *
         * Construct a Station class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Station in the database
         */
        ECSStationColourData( const unsigned long key );

		/**
         * Constructor
         *
         * Construct a ECSStationColourData class based around dataset
         */
		ECSStationColourData(const unsigned long row, TA_Base_Core::IData& data);


		virtual ~ECSStationColourData();
	
	
		/**
         * getKey
         *
         * Returns the key for this Alarm Station.
         *
         * @return The key for this Alarm Station as an unsigned long.
         */
		unsigned long getKey();


		/**
         * getName
         *
         * Returns the name of this Alarm Station. If this is the first time data for this AlarmType
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this Alarm Station as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
		std::string getName();


		/**
		 * getStationColourKey
		 *
		 * Returns the Station RGB level of this Alarm type. 
         *
         * @param EColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one alarm colour was available.
		 *
		 * @return The Station RGB level of the Alarm type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		virtual unsigned long getStationColourKey(EColourType type);


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm Station.
         *
         * @return The date created for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm Station.
         *
         * @return The date modified for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateModified();


		/**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

	private:

		void reload();

        // Assignment operator and copy constructor not used so
        // they are made private
		ECSStationColourData& operator=(const ECSStationColourData&);
		ECSStationColourData( const ECSStationColourData& theECSStationColourData);   

        ECSStationColourHelper* m_helper;

	};
} // closes TA_Base_Core

#endif // ECS_STATION_COLOUR_DATA_H

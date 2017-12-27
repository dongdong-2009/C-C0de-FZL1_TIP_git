/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmSeverityData.h $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmSeverityData is an implementation of IAlarmSeverityData. It holds the data specific to a Alarm
 * severity entry in the database.
 */

#ifndef ALARM_SEVERITY_DATA_H
#define ALARM_SEVERITY_DATA_H

#include <string>

#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityHelper.h"

namespace TA_Base_Core
{

	class AlarmSeverityData : public IAlarmSeverityData
	{

	public:
        /**
         * Constructor
         *
         * Construct a Severity class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Severity in the database
         */
        AlarmSeverityData( const unsigned long key );
		
		AlarmSeverityData(const unsigned long row, TA_Base_Core::IData& data);

		virtual ~AlarmSeverityData();
	
	
		/**
         * getKey
         *
         * Returns the key for this Alarm Severity.
         *
         * @return The key for this Alarm Severity as an unsigned long.
         */
		unsigned long getKey();


		/**
         * getName
         *
         * Returns the name of this Alarm Severity. If this is the first time data for this AlarmType
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this Alarm Severity as a std::string
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
		 * getSeverityColourKey
		 *
		 * Returns the severity RGB level of this Alarm type. 
         *
         * @param EColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one alarm colour was available.
		 *
		 * @return The severity RGB level of the Alarm type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		virtual unsigned long getSeverityColourKey(EColourType type = OPEN_ACKED_FG1);


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm severity.
         *
         * @return The date created for this alarm severity as a time_t.
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
         * Returns the date modified for this alarm severity.
         *
         * @return The date modified for this alarm severity as a time_t.
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
		AlarmSeverityData& operator=(const AlarmSeverityData&);
		
		AlarmSeverityData( const AlarmSeverityData& theAlarmSeverityData);   

        AlarmSeverityHelper* m_helper;

	};
} // closes TA_Base_Core

#endif // ALARM_SEVERITY_DATA_H

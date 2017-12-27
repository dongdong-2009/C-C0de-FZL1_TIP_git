/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IDataPointStateChangeData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * IDataPointStateChangeData is an interface to the lo_datapt_state_change table. It provides both read and write access
 * for agents and user applicaitons.
 */


#if !defined(IDataPointStateChangeData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)
#define IDataPointStateChangeData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_

#include <string>

namespace TA_Base_Core
{

   
	class IDataPointStateChangeData
	{

	public:

		/**
         * getKey
         *
         * Returns the key for this state change log.
         *
         * @return The key for this state change log as a string.
         */
        virtual const std::string& getKey() const =0;

        /**
         * getTimestamp
         *
         * Returns the timestamp on this data point change. This is the time according to the PLC.
         *
         * @return The create time for this data point change as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
		virtual const time_t getTimestamp() =0;

        /**
         * setTimestamp
         *
         * Sets the timestamp on this data point change. This time should be taken from the PLC.
         *
         * @param timestamp The time the data point change was raised by the PLC.
         */
        virtual void setTimestamp(const time_t timestamp) =0;

        /**
         * getLogtime
         *
         * Returns the log time for this data point change.
         *
         * @return The log time for this data point change as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
		virtual const time_t getLogtime() =0;


        /**
         * setLogtime
         *
         * Sets the log time for this data point change. This is the time the data point change was queued for
         * logging.
         *
         * @param logTime The time the data point change was queued for logging.
         */
		virtual void setLogtime(const time_t logTime) =0;

        /**
         * getEntityKey
         *
         * Returns the entity key associated with this data point change.
         *
         * @return The entity key associated with the data point change as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
		virtual const unsigned long getEntityKey() =0;


        /**
         * setEntityKey
         *
         * Sets the entity key for this data point change. This is the key to the data point entity.
         *
         * @param key The key to the data point entity that has registered a change.
         */
        virtual void setEntityKey(const unsigned long key) =0;

        /**
         * getValue
         * 
         * Returns the value of the data point at the time specified by the timestamp. This is the "new" value
         * at this time (i.e. a data point change is a record of the data point value changing, and the value
         * is what it is changing TO, as opposed to what it is changing FROM).
         *
         * Note that this value has no "context". Data point values can be one of boolean, float, or string - 
         * this value will contain all types. The data is intended for human consumption only and therefore does
         * not need to be "interpreted" as a type (as strings, floats, and booleans are visually quite different).
         *
         * @return The value of this data point change as a string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
        virtual const std::string& getValue() =0;

        /**
         * setValue
         *
         * Sets the value associated with the data point state change. This is the value the data point is changing
         * TO - the value of the datapoint at the timestamp.
         *
         * @param value The value of this datapoint at the timestamp.
         */
        virtual void setValue(const std::string& value) =0;

        /**
         * getStatus
         *
         * Returns the status of the value of the data point at the timestamp (i.e. at the change). The status reflects
         * the quality of the data - e.g. bad (comms failure), unknown (sensor accuracy), etc.
         *
         * @return The status of the data point at the timestamp as a string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
        virtual const std::string& getStatus() =0;

        /**
         * setStatus
         *
         * Sets the status of the value of the data point at the timestamp (i.e. at the change). The status reflects
         * the quality of the data - e.g. bad (config error), unknown (unit bounds exceeded). This should be a human-readable
         * description of the status.
         * 
         * @param status The status of the data point at the timestamp as a string. This should be a human-readable
         * textual description.
         */
        virtual void setStatus(const std::string& status) =0;
        

        /**
         * getSubsystemName
         *
         * Returns the name of the Subsystem associated with this state change
         *
         * @return The name of the Subsystem associated with this state change
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong ammount of data is retrieved.
         */
        virtual std::string& getSubsystemName() =0;

        virtual void setSubsystemName(const std::string& subsystemName) =0;
        

        /**
         * getLocationName
         *
         * Returns the name of the Location associated with this state change
         *
         * @return The name of the Location associated with this state change
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong ammount of data is retrieved.
         */
        virtual std::string& getLocationName() =0;

        virtual void setLocationName(const std::string& locationName) =0;
        

        /**
         * getEntityName
         *
         * Returns the name of the Entity associated with this state change
         *
         * @return The name of the Entity associated with this state change
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong ammount of data is retrieved.
         */
        virtual std::string& getEntityName() =0;

        virtual void setEntityName(const std::string& entityName) =0;

        // Raymond Pau++
        // Bug#802 TD7194
        virtual void setMessage(long generalMessage) = 0;
        virtual void setMessage(const std::string& messageType) = 0;

        /**
         * getEventTypeKey
         *
         * Returns the event type key associated with this data point change.
         *
         * @return The event type key associated with the data point change as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TIMESTAMP should be a 
         *            date), or if the wrong ammount of data is retrieved.
         */
		virtual const unsigned long getEventTypeKey() =0;

        /**
         * setEventTypeKey
         *
         * Sets the event type key for this data point change.
         *
         * @param key The event type key for the change.
         */
        virtual void setEventTypeKey(const unsigned long eventTypeKey) =0;
        // Bug#802 TD7194
        // ++Raymond Pau

        virtual void invalidate() =0;

		virtual void applyChanges() =0;

		virtual ~IDataPointStateChangeData() {};

	};

} //close namespace TA_Base_Core

#endif // !defined(IDataPointStateChangeData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)

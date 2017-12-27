/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DataPointStateChangeData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the IDataPointStateChangeData interface. It provides access to the lo_datapt_state_change table
 */

#if !defined(AFX_DATAPOINTSTATECHANGEDATA_H__253C5C02_9ECB_4002_B56B_714067701F05__INCLUDED_)
#define AFX_DATAPOINTSTATECHANGEDATA_H__253C5C02_9ECB_4002_B56B_714067701F05__INCLUDED_

#include <map>

#include "IDataPointStateChangeData.h"


namespace TA_Base_Core
{

    class DataPointStateChangeData : public TA_Base_Core::IDataPointStateChangeData
    {
    public:

        /**
         * Constructor (key)
         *
         * Constructs a new instance of the data point change for the database entry with the specified key.
         *
         * @param key The key to this data point change in the database.
         */
        DataPointStateChangeData(const std::string& key, const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap);


        /**
         * Constructor ()
         *
         * Constructs a new instance of the data point change for a change that has not yet been added to the
         * database. This version of the constructor will automatically create a key.
         */
        DataPointStateChangeData(const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap);

        /**
         * Consturctor (all data)
         *
         * Constructs a new instance of the data point change with all data supplied at creation (i.e. no loading necessary)
         *
         * @param key The key to this data point change in the database
         * @param timeStamp The timestamp of this data point state change
         * @param logTime The log time of this data poitn state change
         * @param entityKey The entity key associated with this data point state change
         * @param value The value of this data point state change
         * @param status The status of this data point state change
         * @param entityName The name of the entity associated with this data point state change
         * @param subsystem The subsystem of the entity associated with this data point state change
         * @param location The location of the entity associated with this data point state change
         */
        DataPointStateChangeData(const std::string& key, const time_t timeStamp, const time_t logTime, const unsigned long entityKey,
            const std::string& value, const std::string& status, const std::string& entityName, 
            const std::string& subsystemName, const std::string& locationName,
            const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap);


        /**
         * Destructor
         *
         * Destructs this object.
         */
	    virtual ~DataPointStateChangeData();

		/**
         * getKey
         *
         * Returns the key for this state change log.
         *
         * @return The key for this state change log as a string.
         */
        const std::string& getKey() const;

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
		const time_t getTimestamp();

        /**
         * setTimestamp
         *
         * Sets the timestamp on this data point change. This time should be taken from the PLC.
         *
         * @param timestamp The time the data point change was raised by the PLC.
         */
        void setTimestamp(const time_t timestamp);

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
		const time_t getLogtime();


        /**
         * setLogtime
         *
         * Sets the log time for this data point change. This is the time the data point change was queued for
         * logging.
         *
         * @param logTime The time the data point change was queued for logging.
         */
		void setLogtime(const time_t logTime);

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
		const unsigned long getEntityKey();


        /**
         * setEntityKey
         *
         * Sets the entity key for this data point change. This is the key to the data point entity.
         *
         * @param key The key to the data point entity that has registered a change.
         */
        void setEntityKey(const unsigned long key);
        

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
        std::string& getSubsystemName();

        void setSubsystemName(const std::string& subsystemName);
        

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
        std::string& getLocationName();

        void setLocationName(const std::string& locationName);
        

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
        std::string& getEntityName();

        void setEntityName(const std::string& entityName);

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
        const std::string& getValue();

        /**
         * setValue
         *
         * Sets the value associated with the data point state change. This is the value the data point is changing
         * TO - the value of the datapoint at the timestamp.
         *
         * @param value The value of this datapoint at the timestamp.
         */
        void setValue(const std::string& value);

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
        const std::string& getStatus();

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
        void setStatus(const std::string& status);

        // Raymond Pau++
        // Bug#802 TD7194
        virtual void setMessage(long generalMessage);
        virtual void setMessage(const std::string& messageType);

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
        virtual const unsigned long getEventTypeKey();

        /**
         * setEventTypeKey
         *
         * Sets the event type key for this data point change.
         *
         * @param key The event type key for the change.
         */
        virtual void setEventTypeKey(const unsigned long eventTypeKey);
        // Bug#802 TD7194
        // ++Raymond Pau

        /**
         * invalidate
         *
         * Flags the contents of the object as "old". They will be reloaded from the database at the next call to a "get"
         * method.
         */
		void invalidate();

        /**
         * applyChanges
         *
         * Writes the contents of the object to the database.
         */
		void applyChanges();

    private:

        /**
         * reload
         *
         * (Re)loads all the data from the database entry secified by the key.
         */
        void reload();

        /**
         * addNewStateChange
         *
         * Creates a new entry in the database for this state change.
         */
        void addNewStateChange();

        /**
         * modifyExistingStateChange
         *
         * Updates the data for the database entry represented by this state change object.
         */
        void modifyExistingStateChange();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        // The key to this entry in the database
        std::string m_key;

        // The time the PLC raised the state change
        time_t m_timestamp;

        // The time the state change event was added to the logging queue
        time_t m_logtime;

        // The entity key of the data point raising the state change
        unsigned long m_entityKey;

        // The value the data point has changed TO
        std::string m_value;

        // The quality status of the value
        std::string m_status;

        // Flag to specify if the data needs to be (re)read from the database
        bool m_isValidData;

        // Flag to specify if this object represents a new data object (i.e. one that was not
        // loaded from the database).
        bool m_isNewEntry;

        // The name of the entity associated with this state change
        std::string m_entityName;

        // The name of the subsystem in which the entity associated with this change exists.
        std::string m_subsystemName;

        // The name of the location in which the entity associated with this change exists.
        std::string m_locationName;

        // The name of the local database to send updates to
        std::string m_localDatabase;
        // Raymond Pau++
        // Bug #802 (TD7194)
        unsigned long m_alarmMessageId;
        long m_generalMessageId;
        
        // The eventType key of this state change
        unsigned long m_eventTypeKey;

        const std::map< std::string, unsigned long > & m_alarmMessageDataPointNameToPKeyMap;
        // Bug #802 (TD7194)
        // ++Raymond Pau
    };  
}

#endif // !defined(AFX_DATAPOINTSTATECHANGEDATA_H__253C5C02_9ECB_4002_B56B_714067701F05__INCLUDED_)

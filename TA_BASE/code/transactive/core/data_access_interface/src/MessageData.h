/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This is an implementation of the IMessageData interface. It provides access to the data
 * stored in a single entry in the Event table.
 */


#if !defined(MessageData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_)
#define MessageData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_

#include "core/data_access_interface/src/IMessageData.h"

#include <string>

namespace TA_Base_Core
{
    class MessageData : public IMessageData
    {
        public:
	        
		MessageData(const std::string& key)
			: m_isValidData(false), m_key(key), m_isNew(false) {};

        MessageData()
			: m_isValidData(false),m_key(""),m_description(""),
            m_paramString(""),m_details(" "),m_type(""),
            m_entityKey(ULONG_MAX),m_sessionID(" "),m_isNew(true),
            // TD13673
            m_locationKey(ULONG_MAX),
            m_alarmKey(" "), m_incidentID(" ") 
        {
            m_createTime.time = 0;
            m_createTime.millitm = 0;
            m_logTime.time = 0;
            m_logTime.millitm = 0;
        };

        MessageData(const std::string& key,
                    const timeb createTime,
                    const timeb logTime,
                    const std::string& description,
                    const std::string& details,
                    const std::string& paramList,
                    const std::string& eventTypeId,
                    const std::string& eventTypeName,
                    const unsigned long entityKey,
                    const std::string& entityName,
                    const std::string& subsystemName,
                    const std::string& locationName,
                    const std::string& assetName,
                    const std::string& sessionId,
                    const std::string& operatorName,
                    const std::string& alarmId,
                    const std::string& severityName,
                    // TD13673
                    const unsigned long locationKey,
                    const std::string& incidentId):
        m_isValidData(true),m_key(key),m_description(description),m_paramString(paramList),m_createTime(createTime),
            m_logTime(logTime),m_details(details),m_type(eventTypeId),m_typeName(eventTypeName),m_entityKey(entityKey),
            m_entityName(entityName),m_locationName(locationName),m_assetName(assetName),m_subsystemName(subsystemName),
            m_sessionID(sessionId),m_operatorName(operatorName),m_alarmKey(alarmId),m_severityName(severityName),
            // TD13673
            m_locationKey(locationKey),
            m_incidentID(incidentId),m_isNew(false){};

	    /**
         * getKey
         *
         * Returns the key for this Message.
         *
         * @return The key for this message as an unsigned long.
         */
        virtual std::string getKey();

		/**
         * getDescription
         *
         * Returns the description for this Message.
         *
         * @return The description for this message as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();

		virtual void setDescription(const std::string& description);

        std::string getParamList();

        void setParamList(const std::string& paramString);

        /**
         * getCreatetime
         *
         * Returns the creat time for this Message.
         *
         * @return The create time for this message as a timeb.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
		virtual timeb getCreatetime();

        virtual void setCreatetime(const timeb createTime);

        /**
         * getLogtime
         *
         * Returns the log time for this Message.
         *
         * @return The log time for this message as a timeb.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
		virtual timeb getLogtime();

		virtual void setLogtime(const timeb logTime);

        /**
         * getDetails
         *
         * Returns the details of this Message.
         *
         * @return The details of this message as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDetails();

        virtual void setDetails(const std::string& details);

        /**
         * getType
         *
         * Returns the type for this Message.
         *
         * @return The type for this message as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getType();

		virtual void setType (const unsigned long typeKey);

		virtual void setType (const std::string& typeName);

        /**
         * getTypeName
         *
         * Returns the name of the message type associated with this message
         *
         * @return The message type name corresponding to the type of this message
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
        std::string& getTypeName();

        void setTypeName (const std::string& typeName);

        /**
         * getEntityName
         *
         * Returns the name of the entity associated with this message
         *
         * @return The name of the entity associated with this message
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
        std::string& getEntityName();

        void setEntityName(const std::string& entityName);
        

        /**
         * getOperatorName
         *
         * Returns the name of the Operator associated with this message
         *
         * @return The name of the Operator associated with this message
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong ammount of data is retrieved.
         */
        std::string& getOperatorName();

        void setOperatorName(const std::string& operatorName);
        

        /**
         * getSeverityName
         *
         * Returns the name of the Severity associated with this message
         *
         * @return The name of the Severity associated with this message
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong ammount of data is retrieved.
         */
        std::string& getSeverityName();

        void setSeverityName(const std::string& severityName);
        

        /**
         * getSubsystemName
         *
         * Returns the name of the Subsystem associated with this message
         *
         * @return The name of the Subsystem associated with this message
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
         * Returns the name of the Location associated with this message
         *
         * @return The name of the Location associated with this message
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
         * getAssetName
         *
         * Returns the name of the Asset associated with this message (for example, the 
		 * "AssetName" value of a DataPoint's parent DataNode, otherwise the name of the entity).
         *
         * @return The name of the Asset associated with this message
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getAssetName();

        virtual void setAssetName(const std::string& assetName);

		/**
         * getEntityKey
         *
         * Returns the entity key associated with this Message.
         *
         * @return The entity key associated with the message as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
		virtual unsigned long getEntityKey();

        virtual void setEntityKey(const unsigned long key);

        /**
         * getSessionID
         *
         * Returns the operator key associated with this Message.
         *
         * @return The session associated with the message as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
		virtual std::string getSessionID();

		virtual void setSessionID( const std::string& sessionID );

        std::string getAlarmKey();

        void setAlarmKey(const std::string& key);

        // TD13673
        /**
         * getLocationKey
         *
         * Returns the locationKey associated with this Message.
         *
         * @return The locationKey associated with the message as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual unsigned long getLocationKey();

        virtual void setLocationKey(unsigned long locationKey);

        // CL-21508 begin
        virtual void setAlarmDescription( const std::string& alarmDescription );
        virtual void setAlarmValue( const std::string& alarmValue );
        virtual void setAlarmComments( const std::string& alarmComments );
        virtual void setAlarmMMSState( const std::string& alarmMMSState );
        virtual void setAlarmDSSState( const std::string& alarmDSSState );
        virtual void setAlarmAVLstate( const std::string& alarmAVLstate );
        virtual void setAlarmACKedBy( const std::string& alarmACKedBy );
        virtual void setAlarmStatus( const std::string& alarmStatus );
        virtual void setEventLelOrSeverityKey( const unsigned long eventLelOrSeverityKey );
        virtual void setAlarmTypeKey( const unsigned long alarmTypeKey );
		virtual void setOMFlag( const std::string& OMFlag );
        // CL-21058 end

        /**
         * getIncidentID
         *
         * Returns the incidentID associated with this Message.
         *
         * @return The incidentID associated with the message as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            b), or if the wrong ammount of data is retrieved.
         */
		virtual std::string getIncidentID();

		virtual void setIncidentID( const std::string& incidentID );

		virtual void invalidate();			
        
        virtual void applyChanges(const std::string& key);

		virtual void applyChanges();

		
            virtual ~MessageData() {};

        private:
            /**
             * getLocalDatabaseName
             *
             * returns the name of the local database
             */
            std::string getLocalDatabaseName();

	        MessageData( const MessageData& theMessageData);
	        MessageData& operator=(const MessageData &);

            void reload();
            void createNewMessage();
            void updateExistingMessage();
			void updateOthersExistingMessage();
			void updateAlarmExistingMessage();

            // CL-21508 begin
            void createOthersMessage();
            void createAlarmNewMessage();
            // CL-21508 end

            bool m_isValidData;
            std::string m_key;
            std::string m_description;
            std::string m_paramString;
            timeb m_createTime;
            timeb m_logTime;
            std::string m_details;
            std::string m_type;
            std::string m_typeName;
            unsigned long m_entityKey;
            std::string m_entityName;
            std::string m_locationName;
			std::string m_assetName;
            std::string m_subsystemName;
			std::string m_sessionID;
            std::string m_operatorName;
            std::string m_alarmKey;
            std::string m_severityName;
			std::string m_incidentID;
            // TD13673
            unsigned long m_locationKey;
            // The name of the local database to send updates to
            std::string m_localDatabase;

            bool m_isNew;

            // CL-21508 begin, these data not exist in EVENT table
            std::string m_alarmDescription;
            std::string m_alarmValue;
            std::string m_alarmComments;
            std::string m_alarmMMSState;
            std::string m_alarmDSSState;
            std::string m_alarmAVLstate;
            std::string m_alarmACKedBy;
            std::string m_alarmStatus;
			std::string m_OMFlag;
            unsigned long m_eventLelOrSeverityKey;
            unsigned long m_alarmTypeKey;
            // CL-21058 end
    };
} // closes TA_Base_Core

#endif // !defined(MessageData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_)

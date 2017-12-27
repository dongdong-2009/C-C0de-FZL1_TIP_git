/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IMessageData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * IMessageData is an interface to the Event table. It provides both read and write access
 * for agents and user applicaitons.
 */


#if !defined(IMessageData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)
#define IMessageData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_

#include <string>

#include <sys/timeb.h>

namespace TA_Base_Core
{
   
	class IMessageData
	{

	public:

		/**
         * getKey
         *
         * Returns the key for this Message.
         *
         * @return The key for this message as an unsigned long.
         */
        virtual std::string getKey() =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string getDescription() =0;

		virtual void setDescription(const std::string& description) =0;

        virtual std::string getParamList() =0;

        virtual void setParamList(const std::string& paramString) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
		virtual timeb getCreatetime() =0;

        virtual void setCreatetime(const timeb createTime) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
		virtual timeb getLogtime() =0;

		virtual void setLogtime(const timeb logTime) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string getDetails() =0;

        virtual void setDetails(const std::string& details) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string getType() =0;

		virtual void setType (const unsigned long typeKey) =0;

		virtual void setType (const std::string& typeName) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getTypeName() =0;

        virtual void setTypeName (const std::string& typeName) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getEntityName() =0;

        virtual void setEntityName(const std::string& entityName) =0;
        

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getOperatorName() =0;

        virtual void setOperatorName(const std::string& operatorName) =0;
        

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getSeverityName() =0;

        virtual void setSeverityName(const std::string& severityName) =0;
        

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getSubsystemName() =0;

        virtual void setSubsystemName(const std::string& subsystemName) =0;
        

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
         *            bool), or if the wrong amount of data is retrieved.
         */
        virtual std::string& getLocationName() =0;

        virtual void setLocationName(const std::string& locationName) =0;

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
        virtual std::string& getAssetName() =0;

        virtual void setAssetName(const std::string& assetName) =0;

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
         *            bool), or if the wrong amount of data is retrieved.
         */
		virtual unsigned long getEntityKey() =0;

        virtual void setEntityKey(const unsigned long key) =0;

        /**
         * getSessionID
         *
         * Returns the sessionID associated with this Message.
         *
         * @return The session key associated with the message as an unsinged long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACKNOWLEDGED should be a 
         *            bool), or if the wrong amount of data is retrieved.
         */
		virtual std::string getSessionID() =0;

		virtual void setSessionID( const std::string& sessionID ) =0;

        virtual std::string getAlarmKey() =0;

        virtual void setAlarmKey(const std::string& key) =0;

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
        virtual unsigned long getLocationKey() =0;

        virtual void setLocationKey(unsigned long locationKey) =0;

        // CL-21508 begin
        virtual void setAlarmDescription( const std::string& alarmDescription ) = 0;
        virtual void setAlarmValue( const std::string& alarmValue ) = 0;
        virtual void setAlarmComments( const std::string& alarmComments ) = 0;
        virtual void setAlarmMMSState( const std::string& alarmMMSState ) = 0;
        virtual void setAlarmDSSState( const std::string& alarmDSSState ) = 0;
        virtual void setAlarmAVLstate( const std::string& alarmAVLstate ) = 0;
        virtual void setAlarmACKedBy( const std::string& alarmACKedBy ) = 0;
        virtual void setAlarmStatus( const std::string& alarmStatus ) = 0;
        virtual void setEventLelOrSeverityKey( const unsigned long eventLelOrSeverityKey ) = 0;
        virtual void setAlarmTypeKey( const unsigned long alarmTypeKey ) = 0;
		virtual void setOMFlag( const std::string& OMFlag ) = 0;
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
         *            bool), or if the wrong amount of data is retrieved.
         */
		virtual std::string getIncidentID() = 0;

		virtual void setIncidentID( const std::string& incidentID ) = 0;

		virtual void invalidate() =0;			
        
        virtual void applyChanges(const std::string& key) =0;

		virtual void applyChanges() =0;

		virtual ~IMessageData() {};

	};

} //close namespace TA_Base_Core

#endif // !defined(IMessageData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)

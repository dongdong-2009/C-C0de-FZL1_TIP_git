/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisConnectiontEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MIS_CONNECTION_DATA_H_INCLUDED
#define MIS_CONNECTION_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MisConnectiontEntityData : public EntityData
    {

    public:

        MisConnectiontEntityData() {}
        MisConnectiontEntityData(unsigned long key);

        virtual ~MisConnectiontEntityData();

    public:



        /**
         * getPingPeriodSeconds
         *
         * @return the value of the PING_PERIOD_SEC param. This parameter indicates
		 * how often the MMS Connection should be pinged.
         */
		unsigned int getPingPeriodSeconds();

        /**
         * getMessageCheckSeconds
         *
         * @return the value of the MSG_CHECK_PERIOD_SEC param. This parameter indicates
		 * how often we should check for MMS messages on our queue.
         */
		unsigned int getMessageCheckSeconds();

        /**
         * getMmsServerTimeoutSeconds
         *
         * @return the value of the TIMEOUT_TENTH_SEC param. This is
		 * the time we should wait for the MMS Connection to respond in tenths of seconds
         */
		unsigned int getTimeoutTenthSecs();

        /**
         * getGlobalMmsInhibit
         *
         * @return the state of global mms inhibit; applied or not applied
         */
		bool getGlobalMmsInhibit();

        /**
         * setGlobalMmsInhibit
         *
         * set the state of global mms inhibit; applied or not applied
		 *
         * @throws DatabaseException if there is a database error
         *
         */
		void setGlobalMmsInhibit(bool applied);

        /**
         * getAlarmQueueName
         *
         * @return the value of the MQ series object name. This is
		 * the name we will use to connect to the server
         */
        std::string getAlarmQueueName();

        /**
         * getCounterQueueName
         *
         * @return the value of the MQ series object name. This is
		 * the name we will use to connect to the server
         */
        std::string getCounterQueueName();

        /**
         * getReportQueueName
         *
         * @return the value of the MQ series object name. This is
		 * the name we will use to connect to the server
         */
        std::string getReportQueueName();

        /**
         * getQueueManagerName
         *
         * @return the value of the MQ series queue manager name. This is
		 * the name we will use to connect to the server
         */
        std::string getQueueManagerName();

        /**
         * getServerHostName
         *
         * @return the value of the MQ series queue host name. This is
		 * the name we will use to connect to the server
         */
        std::string getServerHostName();

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new MisConnectiontEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MisConnectiontEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getSCADAChannelName
         *
         * @return the value of the MQ series SCADA channel name. This is
		 * the name we will use to connect to the server
         */
        std::string getSCADAChannelName();

        /**
         * getMISChannelName
         *
         * @return the value of the MQ series SCADA channel name. This is
		 * the name we will use to connect to the server
         */
        std::string getMISChannelName();

        /**
         * getSCADAReportFilePath
         *
         * @return the report file path which used to send SCADA report to MQ Sever
         */
        std::string getSCADAReportFilePath();

        /**
         * getMisMessageStatusLogFileName
         *
         * @return the log file name which used to recode message sending status
         */
		std::string getMisMessageStatusLogFileName();

        /**
         * getLessFileDataQueueName
         *
         * @return the value of the MQ series object name. This is
		 * the name we will use to connect to the server
         */
        std::string getLessFileDataQueueName();

		std::string getScadaListenerPort();

		
	private:

        MisConnectiontEntityData( const MisConnectiontEntityData&);            
		MisConnectiontEntityData& operator=(const MisConnectiontEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string PING_PERIOD_SEC;
		static const std::string MSG_CHECK_PERIOD_SEC;
		static const std::string TIMEOUT_TENTH_SEC;
		static const std::string GLOBAL_MMS_INHIBIT;
		static const std::string QUEUE_MANAGER_NAME;
		static const std::string ALARM_QUEUE_NAME;
		static const std::string COUNTER_QUEUE_NAME;
        static const std::string REPORT_QUEUE_NAME;
        static const std::string SERVER_HOST_NAME;

		//xxs add
		static const std::string MIS_CHANNEL_NAME;
        static const std::string SCADA_CHANNEL_NAME;
        static const std::string SCADA_REPORT_FILE_PATH;
		static const std::string MIS_MESSAGE_STATUS_LOG_FILE;
		static const std::string LESS_FILEDATA_QUEUE_NAME;
		static const std::string MQ_SCADA_LISTEN_PORT;
		//end xxs add
    };
    
    typedef boost::shared_ptr<MisConnectiontEntityData> MisConnectiontEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MIS_CONNECTION_DATA_H_INCLUDED

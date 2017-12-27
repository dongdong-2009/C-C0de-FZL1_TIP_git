/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RTUEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/22 10:48:00 $
  * Last modified by: $Author: haijun.li $
  * 
  * RTUEntityData implements the interface IRTUEntityData to provide read-only access
  * to RTU entity data.
  */

#ifndef RTU_ENTITY_DATA_H
#define RTU_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class RTUEntityData : public EntityData
    {

	public:

		RTUEntityData();
        RTUEntityData ( unsigned long key );

        virtual ~RTUEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new RTUEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

		//
		// operations required by the RTUEntityData
		//

        /**
         * getSlaveID
         *
         * Returns the slave ID of the RTU entity
         *
         * @return The integer value presenting the slave ID 
         */
        virtual int getSlaveID();

        /**
         * getInServiceState
         *
         * Returns the InService state of RTU entity
         *
         * @return The boolean value to indicate if this entity is in service,
		 *		   TRUE if in service and FALSE otherwise
         */
        virtual bool getInServiceState();

        /**
         * getScanTimeMSecs
         *
         * Returns the scan time in msecs for RTU entity
         *
         * @return The integer value presenting the scan time in msecs
         */
        virtual int getScanTimeMSecs();

        /**
         * getRTUStatusTableStartAddress
         *
         * Returns the start address of the Status Table for RTU entity
         *
         * @return The starting address of the RTU Status table
         */
        virtual unsigned long getRTUStatusTableStartAddress();

        /**
         * getRTUStatusTableEndAddress
         *
         * Returns the end address of the Status Table for RTU entity
         *
         * @return The ending address of the RTU Status table
         */
        virtual unsigned long getRTUStatusTableEndAddress();

        /**
         * getRTUStatusTableEndAddress
         *
         * Returns the end address of the Status Table for RTU entity
         *
         * @return The ending address of the RTU Status table
         */
        virtual unsigned long getRTUStatusOffsetAddress();

        /**
         * getRTUStatusTableServiceNumber
         *
         * Returns the service number of the Status Table for RTU entity
         *
         * @return The service number of the RTU Status table
         */
        virtual const std::string getRTUStatusTableServiceNumber();

        /**
         * getRTUCommandTableStartAddress
         *
         * Returns the start address of the Command Table for RTU entity
         *
         * @return The starting address of the RTU Command table
         */
        virtual unsigned long getRTUCommandTableStartAddress();

        /**
         * getRTUCommandTableEndAddress
         *
         * Returns the end address of the Command Table for RTU entity
         *
         * @return The ending address of the RTU Command table
         */
        virtual unsigned long getRTUCommandTableEndAddress();

        /**
         * getRTUCommandTableServiceNumber
         *
         * Returns the service number of the Command Table for RTU entity
         *
         * @return The service number of the RTU Command table
         */
        virtual const std::string getRTUCommandTableServiceNumber();

        /**
         * getRTUPollingTableStartAddress
         *
         * Returns the start address of the Polling Table for RTU entity
         *
         * @return The starting address of the RTU Polling table
         */
        virtual unsigned long getRTUPollingTableStartAddress();

        /**
         * getRTUPollingTableEndAddress
         *
         * Returns the end address of the Polling Table for RTU entity
         *
         * @return The ending address of the RTU Polling table
         */
        virtual unsigned long getRTUPollingTableEndAddress();

        /**
         * getRTUPollingTableServiceNumber
         *
         * Returns the service number of the Polling Table for RTU entity
         *
         * @return The service number of the RTU Polling table
         */
        virtual const std::string getRTUPollingTableServiceNumber();

        /**
         * getTcpNoDelay
         *
         * Returns whether to set the TCPNODELAY socket option for the RTU entity
         *
         * @return whether to set the TCPNODELAY socket option
         */
        virtual bool getTcpNoDelay();

        /**
         * getCommsErrorRetries
         *
         * Returns the number of retries before raising a comms alarm
         *
         * @return The integer value presenting the comms error retries
         */
        virtual int getCommsErrorRetries();

        //wenching
        /**
         * getPollTimeout
         *
         * Returns the number of milli seconds for TCP socket timeout for polling
         *
         * @return The integer value presenting the timeout in msecs
         */
        virtual int getPollTimeout();

        //wenching
		/**
         * getCommandTimeout
         *
         * Returns the number of milli seconds for TCP socket timeout for command
         *
         * @return The integer value presenting the timeout in msecs
         */
        virtual int getCommandTimeout();


		/*
		*For PScadaRTU
		*Return the number of seconds for polling all pscada data periodly
		*/
		virtual int getPollAllDataTimeInSec();

		/*
		*For PScadaRTU
		*Return the number of seconds for polling pscada kwh data periodly
		*/
		virtual int getPollKwhDataTimeInSec();

		/*
		*For PScadaRTU
		*Return the number of data lost durable time
		*/
		virtual int getDataLostDurableTimeInSec();

		/*
		*For PScadaRTU
		*Return the number of update lost durable time
		*/
		virtual int getUpdateLostPeriodInSec();
		//TD10586
		virtual unsigned int getRTUFtpPort();
		virtual std::string getRTUFtpUsername();
		virtual std::string getRTUFtpPassword();
		virtual std::string getRTUConfigFilePath();
		virtual std::string getPMSConfigFilePath();

		//Using for IEC104 Protocol 
		virtual std::string getSecondAddress();

		//using for mutiple rtumanager
		virtual  int  getGroupID();

		
		/**
         * isRaiseAlarmWhenCommError
         *
         * Returns whether raise alarm when RTU communication errors happen.
         *
         */
		virtual bool isRaiseAlarmWhenCommError();
    private:

		std::string getStringData ( const std::string & parameterName );
		bool getBooleanData ( const std::string & parameterName );
		int getIntegerData ( const std::string & parameterName );
		unsigned long getUnsignedLongData ( const std::string & parameterName );
		double getRealData  ( const std::string & parameterName );

    private:
		          
        RTUEntityData ( const RTUEntityData & obj ){};            
		RTUEntityData & operator= ( const RTUEntityData &){};
        // 
        // The entity type is fixed for all instances of this class.
        // 

	public:

        static const std::string ENTITY_TYPE;
		static const std::string RTU_ENTITY_TYPE;
		static const std::string INSERVICE;
		static const std::string SCANTIME;
		static const std::string STATIONNAME;
		static const std::string RTU_SLAVE_ID;
		static const std::string RTU_STATUS_TABLE_START_ADDRESS;
		static const std::string RTU_STATUS_TABLE_END_ADDRESS;
		static const std::string RTU_STATUS_OFFSET_ADDRESS;
		static const std::string RTU_STATUS_TABLE_SERVICE_NUMBER;
		static const std::string RTU_COMMAND_TABLE_START_ADDRESS;
		static const std::string RTU_COMMAND_TABLE_END_ADDRESS;
		static const std::string RTU_COMMAND_TABLE_SERVICE_NUMBER;
		static const std::string RTU_POLLING_TABLE_START_ADDRESS;
		static const std::string RTU_POLLING_TABLE_END_ADDRESS;
		static const std::string RTU_POLLING_TABLE_SERVICE_NUMBER;
		static const std::string TCPNODELAY;
		static const std::string COMMS_ERROR_RETRIES;
		static const std::string RTU_FTP_PORT;
		static const std::string RTU_FTP_USERNAME;
		static const std::string RTU_FTP_PASSWORD;
		static const std::string RTU_CONFIGFILE_PATH;
		static const std::string PMS_CONFIGFILE_PATH;
		static const std::string POLL_TIMEOUT;   //wenching
		static const std::string COMMAND_TIMEOUT;   //wenching        
		static const std::string RAISE_ALARM_COMM_ERROR;
		
		static const std::string RTU_GROUP_ID;
		static const std::string POLL_ALL_DATA_TIME_IN_SEC;
		static const std::string POLL_KWH_DATA_TIME_IN_SEC;
		static const std::string DATA_LOST_DURABLE_TIME_IN_SEC;
		static const std::string UPDATE_LOST_PERIOD_IN_SEC;
		//Using for IEC 104 Protocol
		static const std::string RTUSECONDADDRESS;
    };
    
    typedef boost::shared_ptr<RTUEntityData> RTUEntityDataPtr;
} //close namespace TA_Base_Core

#endif

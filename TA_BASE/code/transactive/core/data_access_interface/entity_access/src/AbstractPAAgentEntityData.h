/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/24 13:41:41 $
  * Last modified by: $Author: qi.huang $
  * 
  * AbstractPAAgentEntityData implements the interface IAbstractPAAgentEntityData to provide read-only access
  * to StationPAAgent entity data.
  */

#ifndef AbstractPAAgent_ENTITY_DATA_H
#define AbstractPAAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class AbstractPAAgentEntityData :  public virtual IPAAgentEntityData, public virtual EntityData
    {
    private:
		          
        AbstractPAAgentEntityData ( const AbstractPAAgentEntityData & obj ){};
		AbstractPAAgentEntityData & operator= ( const AbstractPAAgentEntityData &){};

	public:

        static const std::string FAST_POLL_RATE;
        static const std::string WORKER_THREADS_SLEEP_TIME;
        static const std::string SOCKET_TIMEOUT;
        static const std::string TIS_DISPLAY_DURATION;
        static const std::string KEEP_TERMINATED_BROADCASTS;
        static const std::string RETRIES;
        static const std::string PRIMARY_FEPC_ADDRESS;
        static const std::string SECONDARY_FEPC_ADDRESS;
        static const std::string PRIMARY_FEPC_PORT;
        static const std::string SECONDARY_FEPC_PORT;
        static const std::string MAX_BROADCASTS;
        static const std::string MY_MAJOR_SUMMARY_ALARM_TOKEN;
        static const std::string MY_MINOR_SUMMARY_ALARM_TOKEN;
        static const std::string MY_FIRE_COUNTDOWN_ACTIVE_TOKEN;
        static const std::string MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN;
		static const std::string ASSET_NAME;

        // AZ++ hardware timeout
        static const std::string LIVE_BROADCAST_TIMEOUT_IN_MSECS;
        static const std::string DVA_BROADCAST_TIMEOUT_IN_MSECS;
        // ++AZ

        static const std::string FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND;
        static const std::string PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND;
        static const std::string SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND;
        static const std::string DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND;
        static const std::string PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND;
		static const std::string FAS_ENTITY_NAME;

		AbstractPAAgentEntityData();

        virtual ~AbstractPAAgentEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();


		//
		// operations required by the IPAAgentEntityData
		//



        /**
         * getFastPollRateInMSecs
         *
         * Returns the processing rate for the fast poll loop in msec.  This is the sleep time of the 
         * main PAS table reading loop + the actual time taken to read them.
         *
         * @return processing rate for the fast poll loop in msec.
         */
        virtual unsigned long getFastPollRateInMSecs();


        /**
         * getWorkerThreadsSleepTimeInMSecs
         *
         * Returns the sleep time for all PA Agent worker threads in msec.
         *
         * @return worker threads sleep time in msec.
         */
        virtual unsigned long getWorkerThreadsSleepTimeInMSecs();

        /**
         * getSocketTimeoutInMSecs
         *
         * Returns the socket timeout for all reads and connection requests.
         *
         * @return socket comms timeout in sec.
         */
        virtual unsigned long getSocketTimeoutInSecs();


        /**
         * getTisDisplayDurationInSecs
         *
         * @return the Tis duration for the display of STIS synchronised broadcasts in seconds.
         */
        virtual unsigned long getTisDisplayDurationInSecs();


        /**
         * getKeepTerminatedBroadcastsInSecs
         *
         * @return the time to keep terminated broadcasts in seconds.
         */
        virtual unsigned long getKeepTerminatedBroadcastsInSecs();
        
        /**
         * getRetries
         *
         * Returns the number of retries for FEPC communications failures before switching to the 
         * redundant FEPC.
         *
         * @return number of retries for FEPC comms failures.
         */
        virtual unsigned long getRetries();

        
        /**
         * getPrimaryFEPCAddress
         *
         * Returns the IP address of the primary FEPC.
         *
         * @return IP address of the primary FEPC
         */
        virtual std::string getPrimaryFEPCAddress();

        
        /**
         * getSecondaryFEPCAddress
         *
         * Returns the IP address of the secondary FEPC.
         *
         * @return IP address of the primary FEPC
         */
        virtual std::string getSecondaryFEPCAddress();

        
        /**
         * getPrimaryFEPCPort
         *
         * Returns the TCP/IP port of the primary FEPC.
         *
         * @return port of the primary FEPC
         */
        virtual unsigned long getPrimaryFEPCPort();

        
        /**
         * getSecondaryFEPCPort
         *
         * Returns the TCP/IP port of the secondary FEPC.
         *
         * @return port of the primary FEPC
         */
        virtual unsigned long getSecondaryFEPCPort();

        /**
         * getMaxBroadcasts
         *
         * @return MaxBroadcast
         */
        virtual unsigned long getMaxBroadcasts();
        
        /**
         * getMyMajorSummaryAlarmToken
         *
         * @return MyMajorSummaryAlarmToken
         */
        virtual std::string getMyMajorSummaryAlarmToken();

        /**
         * getMyMinorSummaryAlarmToken
         *
         * @return MyMinorSummaryAlarmToken
         */
        virtual std::string getMyMinorSummaryAlarmToken();

        /**
         * getMyFireCountdownActiveToken
         *
         * @return MyFireCountdownActiveToken
         */
        virtual std::string getMyFireCountdownActiveToken();

        /**
         * getMyFireCountdownActiveOutputToken
         *
         * @return MyFireCountdownActiveOutputToken
         */
        virtual std::string getMyFireCountdownActiveOutputToken();

        // AZ++ hardware timeout

        /**
         * getLiveBroadcastTimeoutInMsecs
         *
         * @return LiveBroadcastTimeoutInMsecs
         */
        virtual unsigned long getLiveBroadcastTimeoutInMsecs();

        /**
         * getDvaBroadcastTimeoutInMsecs
         *
         * @return DvaBroadcastTimeoutInMsecs
         */
        virtual unsigned long getDvaBroadcastTimeoutInMsecs();

        // ++AZ

		virtual std::string getAssetName();

		virtual std::string getFasEntityName();

        virtual unsigned long getFireAlarmPollingIntervalInMillisecond();

        virtual unsigned long getPasZonePollingIntervalInMillisecond();

        virtual unsigned long getSystemFaultPollingIntervalInMillisecond();

        virtual unsigned long getDvaLibPollingIntervalInMillisecond();

        virtual unsigned long getPasDvaRecordingMaxDurationInSecond();
    };

    typedef boost::shared_ptr<AbstractPAAgentEntityData> AbstractPAAgentEntityDataPtr;
    
} //close namespace TA_Base_Core

#endif

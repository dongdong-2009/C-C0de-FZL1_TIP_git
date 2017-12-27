/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IMasterPAAgentEntityData provides read-only access to MasterPAAgent entity data.
  */

#ifndef IPAAgent_ENTITY_DATA_H
#define IPAAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IPAAgentEntityData : public virtual IEntityData
    {
    public:

        

        /**
         * getFastPollRateInMSecs
         *
         * Returns the processing rate for the fast poll loop in msec.  This is the sleep time of the 
         * main PAS table reading loop + the actual time taken to read them.
         *
         * @return processing rate for the fast poll loop in msec.
         */
        virtual unsigned long getFastPollRateInMSecs() = 0;


        /**
         * getWorkerThreadsSleepTimeInMSecs
         *
         * Returns the sleep time for all PA Agent worker threads in msec.
         *
         * @return worker threads sleep time in msec.
         */
        virtual unsigned long getWorkerThreadsSleepTimeInMSecs() = 0;


        /**
         * getSocketTimeoutInMSecs
         *
         * Returns the socket timeout for all reads and connection requests.
         *
         * @return socket comms timeout in sec.
         */
        virtual unsigned long getSocketTimeoutInSecs() = 0;


        /**
         * getTisDisplayDurationInSecs
         *
         * @return the Tis duration for the display of STIS synchronised broadcasts in seconds.
         */
        virtual unsigned long getTisDisplayDurationInSecs() = 0;

        /**
         * getKeepTerminatedBroadcastsInSecs
         *
         * @return the time to keep terminated broadcasts in seconds.
         */
        virtual unsigned long getKeepTerminatedBroadcastsInSecs() = 0;

        /**
         * getRetries
         *
         * Returns the number of retries for FEPC communications failures before switching to the 
         * redundant FEPC.
         *
         * @return number of retries for FEPC comms failures.
         */
        virtual unsigned long getRetries() = 0;

        
        /**
         * getPrimaryFEPCAddress
         *
         * Returns the IP address of the primary FEPC.
         *
         * @return IP address of the primary FEPC
         */
        virtual std::string getPrimaryFEPCAddress() = 0;

        
        /**
         * getSecondaryFEPCAddress
         *
         * Returns the IP address of the secondary FEPC.
         *
         * @return IP address of the primary FEPC
         */
        virtual std::string getSecondaryFEPCAddress() = 0;

        
        /**
         * getPrimaryFEPCPort
         *
         * Returns the TCP/IP port of the primary FEPC.
         *
         * @return port of the primary FEPC
         */
        virtual unsigned long getPrimaryFEPCPort() = 0;

        
        /**
         * getSecondaryFEPCPort
         *
         * Returns the TCP/IP port of the secondary FEPC.
         *
         * @return port of the primary FEPC
         */
        virtual unsigned long getSecondaryFEPCPort() = 0;
    
    
        /**
         * getMaxBroadcasts
         *
         * @return MaxBroadcast
         */
        virtual unsigned long getMaxBroadcasts() = 0;
        
        /**
         * getMyMajorSummaryAlarmToken
         *
         * @return MyMajorSummaryAlarmToken
         */
        virtual std::string getMyMajorSummaryAlarmToken() = 0;

        /**
         * getMyMinorSummaryAlarmToken
         *
         * @return MyMinorSummaryAlarmToken
         */
        virtual std::string getMyMinorSummaryAlarmToken() = 0;

        /**
         * getMyFireCountdownActiveToken
         *
         * @return MyFireCountdownActiveToken
         */
        virtual std::string getMyFireCountdownActiveToken() = 0;

        /**
         * getMyFireCountdownActiveOutputToken
         *
         * @return MyFireCountdownActiveOutputToken
         */
        virtual std::string getMyFireCountdownActiveOutputToken() = 0;

        // AZ++ hardware timeout

        /**
         * getLiveBroadcastTimeoutInMsecs
         *
         * @return LiveBroadcastTimeoutInMsecs
         */
        virtual unsigned long getLiveBroadcastTimeoutInMsecs() = 0;

        /**
         * getDvaBroadcastTimeoutInMsecs
         *
         * @return DvaBroadcastTimeoutInMsecs
         */
        virtual unsigned long getDvaBroadcastTimeoutInMsecs() = 0;

        // ++AZ

	virtual std::string getAssetName() = 0;

    };

} //close namespace TA_Base_Core

#endif


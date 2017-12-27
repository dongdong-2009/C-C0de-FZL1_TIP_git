/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/24 13:41:41 $
  * Last modified by: $Author: qi.huang $
  * 
  * AbstractPAAgentEntityData implements the interface IAbstractPAAgentEntityData to provide read-only access
  * to AbstractPAAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"


static std::string toUpper(const std::string& in)
{
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(), toupper);
    return out;
}




namespace TA_Base_Core
{

    static const unsigned long DEFAULT_FAST_POLL_RATE                       
        = 500;
    static const unsigned long DEFAULT_WORKER_THREADS_SLEEP_TIME            
        = 100;
    static const unsigned long DEFAULT_SOCKET_TIMEOUT
        = 3;
    static const unsigned long DEFAULT_TIS_DISPLAY_DURATION                 
        = 30;
    static const unsigned long DEFAULT_KEEP_TERMINATED_BROADCASTS   
        = 60*60*24;  // 1 day
    static const unsigned long DEFAULT_RETRIES                              
        = 3;
    static const std::string   DEFAULT_PRIMARY_FEPC_ADDRESS             
        = "";
    static const std::string    DEFAULT_SECONDARY_FEPC_ADDRESS           
        = "";
    static const unsigned long  DEFAULT_PRIMARY_FEPC_PORT                
        = 27000;
    static const unsigned long  DEFAULT_SECONDARY_FEPC_PORT              
        = 27000;
    static const std::string    DEFAULT_MY_MAJOR_SUMMARY_ALARM_TOKEN          
        = "diiPAC-MajorAlarm";
    static const std::string    DEFAULT_MY_MINOR_SUMMARY_ALARM_TOKEN          
        = "diiPAC-MinorAlarm";
    static const std::string    DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_TOKEN        
        = "diiPAC-FireCountdown";
    static const std::string    DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN 
        = "dioPAC-FireCountdown";
    static const unsigned long  DEFAULT_MAX_BROADCASTS                   
        = 100;

    // AZ++ hardware timeout

    static const unsigned long  DEFAULT_LIVE_BROADCAST_TIMEOUT_IN_MSECS                   
        = 500;
    static const unsigned long  DEFAULT_DVA_BROADCAST_TIMEOUT_IN_MSECS                   
        = 3000;

    // ++AZ

    const std::string AbstractPAAgentEntityData::FAST_POLL_RATE
        = "FastPollRateInMSecs";
    const std::string AbstractPAAgentEntityData::WORKER_THREADS_SLEEP_TIME
        = "WorkerThreadsSleepTimeInMSecs";
    const std::string AbstractPAAgentEntityData::SOCKET_TIMEOUT
        = "SocketTimeoutInSecs";
    const std::string AbstractPAAgentEntityData::TIS_DISPLAY_DURATION
        = "TisDisplayDurationInSecs";
    const std::string AbstractPAAgentEntityData::KEEP_TERMINATED_BROADCASTS
        = "KeepTerminatedBroadcastsInSecs";
    const std::string AbstractPAAgentEntityData::RETRIES
        = "Retries";
    const std::string AbstractPAAgentEntityData::PRIMARY_FEPC_ADDRESS
        = "PrimaryFEPCAddress";
    const std::string AbstractPAAgentEntityData::SECONDARY_FEPC_ADDRESS
        = "SecondaryFEPCAddress";
    const std::string AbstractPAAgentEntityData::PRIMARY_FEPC_PORT
        = "PrimaryFEPCPort";
    const std::string AbstractPAAgentEntityData::SECONDARY_FEPC_PORT
        = "SecondaryFEPCPort";
    const std::string AbstractPAAgentEntityData::MY_MAJOR_SUMMARY_ALARM_TOKEN              
        = "MyMajorSummaryAlarmToken";
    const std::string AbstractPAAgentEntityData::MY_MINOR_SUMMARY_ALARM_TOKEN              
        = "MyMinorSummaryAlarmToken";
    const std::string AbstractPAAgentEntityData::MY_FIRE_COUNTDOWN_ACTIVE_TOKEN            
        = "MyFireCountdownActiveToken";
    const std::string AbstractPAAgentEntityData::MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN     
        = "MyFireCountdownActiveOutputToken";
    const std::string AbstractPAAgentEntityData::MAX_BROADCASTS                          
        = "MaxBroadcasts";

    // AZ++ hardware timeout

    const std::string AbstractPAAgentEntityData::LIVE_BROADCAST_TIMEOUT_IN_MSECS                          
        = "LiveBroadcastTimeoutInMsecs";
    const std::string AbstractPAAgentEntityData::DVA_BROADCAST_TIMEOUT_IN_MSECS                          
        = "DvaBroadcastTimeoutInMsecs";

    // ++AZ

    const std::string AbstractPAAgentEntityData::FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND = "FireAlarmPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND = "PasZonePollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND = "SystemFaultPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND = "DvaLibPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND = "PasDvaRecordingMaxDurationInSecond";

	const std::string AbstractPAAgentEntityData::ASSET_NAME = "AssetName";
	const std::string AbstractPAAgentEntityData::FAS_ENTITY_NAME = "FasEntityName";

    static const unsigned long DEFAULT_FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND = 500;
    static const unsigned long DEFAULT_PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND = 200;
    static const unsigned long DEFAULT_SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND = 500;
    static const unsigned long DEFAULT_DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND = 2000;
    static const unsigned long DEFAULT_PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND = 30;

	AbstractPAAgentEntityData::AbstractPAAgentEntityData()
    {
    }


    AbstractPAAgentEntityData::~AbstractPAAgentEntityData()
    {
    }


    /**
     * getTisDisplayDuration
     *
     * Returns the TIS display duration
     *
     * @return unsigned long display duration in secs
     */
    unsigned long AbstractPAAgentEntityData::getTisDisplayDurationInSecs()
    {
        FUNCTION_ENTRY("getTisDisplayDurationInSecs");

		std::string value = getHelper()->getParameter(TIS_DISPLAY_DURATION);

		unsigned long tisDisplayDuration = 0;

		if(value.empty())
		{
			tisDisplayDuration = DEFAULT_TIS_DISPLAY_DURATION;
		}
		else
		{
			tisDisplayDuration = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return tisDisplayDuration;
	}


    unsigned long AbstractPAAgentEntityData::getKeepTerminatedBroadcastsInSecs()
    {
        FUNCTION_ENTRY("getKeepTerminatedBroadcastsInSecs");

		std::string value = getHelper()->getParameter(KEEP_TERMINATED_BROADCASTS);

		unsigned long keepTerminatedBroadcasts = 0;

		if(value.empty())
		{
			keepTerminatedBroadcasts = DEFAULT_KEEP_TERMINATED_BROADCASTS;
		}
		else
		{
			keepTerminatedBroadcasts = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return keepTerminatedBroadcasts;
	}

    /**
     * getFastPollRate
     *
     * Returns the processing rate for the fast poll loop in msec.  This is the sleep time of the 
     * main PAS table reading loop + the actual time taken to read them.
     *
     * @return processing rate for the fast poll loop in msec.
     */
    unsigned long AbstractPAAgentEntityData::getFastPollRateInMSecs()
    {
        FUNCTION_ENTRY("getFastPollRateInMSecs");
		std::string value = getHelper()->getParameter(FAST_POLL_RATE);
		unsigned long fastPollRate = 0;

		if(value.empty())
		{
			fastPollRate = DEFAULT_FAST_POLL_RATE;
		}
		else
		{
			fastPollRate = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return fastPollRate;
	}


    unsigned long AbstractPAAgentEntityData::getWorkerThreadsSleepTimeInMSecs()
    {
        FUNCTION_ENTRY("getWorkerThreadsSleepTimeInMSecs");
		std::string value = getHelper()->getParameter(WORKER_THREADS_SLEEP_TIME);

		unsigned long workerThreadsSleepTime = 0;
		if(value.empty())
		{
			workerThreadsSleepTime = DEFAULT_WORKER_THREADS_SLEEP_TIME;
		}
		else
		{
			workerThreadsSleepTime = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return workerThreadsSleepTime;
	}


    unsigned long AbstractPAAgentEntityData::getSocketTimeoutInSecs()
    {
        FUNCTION_ENTRY("getSocketTimeoutInSecs");
		unsigned long socketTimeout = 0;

		std::string value = getHelper()->getParameter(SOCKET_TIMEOUT);
		if(value.empty())
		{
			socketTimeout = DEFAULT_SOCKET_TIMEOUT;
		}
		else
		{
			socketTimeout = EntityHelper::getUnsignedLongData(value);
		}
        FUNCTION_EXIT;
        return socketTimeout;
	}

    /**
     * getRetries
     *
     * Returns the number of retries for FEPC communications failures before switching to the 
     * redundant FEPC.
     *
     * @return number of retries for FEPC comms failures.
     */
    unsigned long AbstractPAAgentEntityData::getRetries()
    {
        FUNCTION_ENTRY("getRetries");
		unsigned long retries = 0;

		std::string value = getHelper()->getParameter(RETRIES);
		if(value.empty())
		{
			retries = DEFAULT_RETRIES;
		}
		else
		{
			retries = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return retries;
	}
    
    /**
     * getPrimaryFEPCAddress
     *
     * Returns the IP address of the primary FEPC.
     *
     * @return IP address of the primary FEPC
     */
    std::string AbstractPAAgentEntityData::getPrimaryFEPCAddress()
    {
        FUNCTION_ENTRY("getPrimaryFEPCAddress");
		std::string primaryFEPCAddress("");
		std::string value = getHelper()->getParameter(PRIMARY_FEPC_ADDRESS);
		if(value.empty())
		{
			primaryFEPCAddress = DEFAULT_PRIMARY_FEPC_ADDRESS;
		}
		else
		{
			primaryFEPCAddress = value;
		}

        FUNCTION_EXIT;
        return primaryFEPCAddress;
	}
    
    /**
     * getSecondaryFEPCAddress
     *
     * Returns the IP address of the secondary FEPC.
     *
     * @return IP address of the primary FEPC
     */
    std::string AbstractPAAgentEntityData::getSecondaryFEPCAddress()
    {
        FUNCTION_ENTRY("getSecondaryFEPCAddress");
		std::string secondaryFEPCAddress("");
		std::string value = getHelper()->getParameter(SECONDARY_FEPC_ADDRESS);
		if(value.empty())
		{
			// TD17559
			// secondaryFEPCAddress = DEFAULT_WORKER_THREADS_SLEEP_TIME;
			secondaryFEPCAddress = DEFAULT_SECONDARY_FEPC_ADDRESS;
			// TD17559
		}
		else
		{
			secondaryFEPCAddress = value;
		}

        FUNCTION_EXIT;
        return secondaryFEPCAddress;
	}
    
    /**
     * getPrimaryFEPCPort
     *
     * Returns the TCP/IP port of the primary FEPC.
     *
     * @return port of the primary FEPC
     */
    unsigned long AbstractPAAgentEntityData::getPrimaryFEPCPort()
    {
        FUNCTION_ENTRY("getPrimaryFEPCPort");
		unsigned long primaryFEPCPort = 0;
		std::string value = getHelper()->getParameter(PRIMARY_FEPC_PORT);
		if(value.empty())
		{
			primaryFEPCPort = DEFAULT_PRIMARY_FEPC_PORT;
		}
		else
		{
			primaryFEPCPort = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return primaryFEPCPort;
	}
    
    /**
     * getSecondaryFEPCPort
     *
     * Returns the TCP/IP port of the secondary FEPC.
     *
     * @return port of the primary FEPC
     */
    unsigned long AbstractPAAgentEntityData::getSecondaryFEPCPort()
    {
        FUNCTION_ENTRY("getSecondaryFEPCPort");
		unsigned long  secondaryFEPCPort = 0;
		std::string value = getHelper()->getParameter(SECONDARY_FEPC_PORT);
		if(value.empty())
		{
			secondaryFEPCPort = DEFAULT_SECONDARY_FEPC_PORT;
		}
		else
		{
			secondaryFEPCPort = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return secondaryFEPCPort;
	}
     
    unsigned long AbstractPAAgentEntityData::getMaxBroadcasts()
    {
        FUNCTION_ENTRY("getMaxBroadcasts");
		unsigned long maxBroadcasts = 0;
		std::string value = getHelper()->getParameter(MAX_BROADCASTS);
		if(value.empty())
		{
			maxBroadcasts = DEFAULT_MAX_BROADCASTS;
		}
		else
		{
			maxBroadcasts = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return maxBroadcasts;
	}

    std::string AbstractPAAgentEntityData::getMyMajorSummaryAlarmToken()
    {
        FUNCTION_ENTRY("getMyMajorSummaryAlarmToken");
		std::string myMajorSummaryAlarmToken("");
		std::string value = getHelper()->getParameter(MY_MAJOR_SUMMARY_ALARM_TOKEN);
		if(value.empty())
		{
			myMajorSummaryAlarmToken = DEFAULT_MY_MAJOR_SUMMARY_ALARM_TOKEN;
		}
		else
		{
			myMajorSummaryAlarmToken = value;
		}

        FUNCTION_EXIT;
        return myMajorSummaryAlarmToken;
	}

    std::string AbstractPAAgentEntityData::getMyMinorSummaryAlarmToken()
    {
        FUNCTION_ENTRY("getMyMinorSummaryAlarmToken");
		std::string myMinorSummaryAlarmToken("");
		std::string value = getHelper()->getParameter(MY_MINOR_SUMMARY_ALARM_TOKEN);
		if(value.empty())
		{
			myMinorSummaryAlarmToken = DEFAULT_MY_MINOR_SUMMARY_ALARM_TOKEN;
		}
		else
		{
			myMinorSummaryAlarmToken = value;
		}

        FUNCTION_EXIT;
        return myMinorSummaryAlarmToken;
	}

    std::string AbstractPAAgentEntityData::getMyFireCountdownActiveToken()
    {
        FUNCTION_ENTRY("getMyFireCountdownActiveToken");
		std::string myFireCountdownActiveToken("");
		std::string value = getHelper()->getParameter(MY_FIRE_COUNTDOWN_ACTIVE_TOKEN);
		if(value.empty())
		{
			myFireCountdownActiveToken = DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_TOKEN;
		}
		else
		{
			myFireCountdownActiveToken = value;
		}

        FUNCTION_EXIT;
        return myFireCountdownActiveToken;
	}

    std::string AbstractPAAgentEntityData::getMyFireCountdownActiveOutputToken()
    {
        FUNCTION_ENTRY("getMyFireCountdownActiveOutputToken");
		std::string myFireCountdownActiveOutputToken("");
		std::string value = getHelper()->getParameter(MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN);
		if(value.empty())
		{
			myFireCountdownActiveOutputToken = DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN;
		}
		else
		{
			myFireCountdownActiveOutputToken = value;
		}

        FUNCTION_EXIT;
        return myFireCountdownActiveOutputToken;
	}

    // AZ++ hardware timeout

    unsigned long AbstractPAAgentEntityData::getLiveBroadcastTimeoutInMsecs()
    {
        FUNCTION_ENTRY("getLiveBroadcastTimeoutInMsecs");
		unsigned long liveBroadcastTimeoutInMsecs = 0;
		std::string value = getHelper()->getParameter(LIVE_BROADCAST_TIMEOUT_IN_MSECS);
		if(value.empty())
		{
			liveBroadcastTimeoutInMsecs = DEFAULT_LIVE_BROADCAST_TIMEOUT_IN_MSECS;
		}
		else
		{
			liveBroadcastTimeoutInMsecs = EntityHelper::getUnsignedLongData(value);
		}
        
        FUNCTION_EXIT;
        return liveBroadcastTimeoutInMsecs;
    }

    unsigned long AbstractPAAgentEntityData::getDvaBroadcastTimeoutInMsecs()
    {
        FUNCTION_ENTRY("getDvaBroadcastTimeoutInMsecs");
		unsigned long dvaBroadcastTimeoutInMsecs = 0;
		std::string value = getHelper()->getParameter(DVA_BROADCAST_TIMEOUT_IN_MSECS);
		if(value.empty())
		{
			dvaBroadcastTimeoutInMsecs = DEFAULT_DVA_BROADCAST_TIMEOUT_IN_MSECS;
		}
		else
		{
			dvaBroadcastTimeoutInMsecs = EntityHelper::getUnsignedLongData(value);
		}
        
        FUNCTION_EXIT;
        return dvaBroadcastTimeoutInMsecs;
    }

    // ++AZ

	std::string AbstractPAAgentEntityData::getAssetName()
	{
        FUNCTION_ENTRY("getAssetName");
		std::string assetName("");
		std::string value = getHelper()->getParameter(ASSET_NAME);
		if(value.empty())
		{
			assetName = "Unknown";
		}
		else
		{
			assetName = value;
		}
        
        FUNCTION_EXIT;
        return assetName;
	}

	std::string AbstractPAAgentEntityData::getFasEntityName()
	{
		FUNCTION_ENTRY("getFasEntityName");

		std::string strFasEntityName("");
		std::string value = getHelper()->getParameter(FAS_ENTITY_NAME);
		if(value.empty())
		{
			strFasEntityName = "Unknown";
		}
		else
		{
			strFasEntityName = value;
		}

		FUNCTION_EXIT;
		return strFasEntityName;
	}

    void AbstractPAAgentEntityData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        getHelper()->invalidate();

        FUNCTION_EXIT;
    }

    unsigned long AbstractPAAgentEntityData::getFireAlarmPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("getFireAlarmPollingIntervalInMillisecond");
        unsigned long fireAlarmPollingIntervalInMillisecond = 0;

        std::string value = getHelper()->getParameter(FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND);
        if(value.empty())
        {
            fireAlarmPollingIntervalInMillisecond = DEFAULT_FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND;
        }
        else
        {
            fireAlarmPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return fireAlarmPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getPasZonePollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("getPasZonePollingIntervalInMillisecond");
        unsigned long pasZonePollingIntervalInMillisecond = 0;

        std::string value = getHelper()->getParameter(PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND);
        if(value.empty())
        {
            pasZonePollingIntervalInMillisecond = DEFAULT_PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND;
        }
        else
        {
            pasZonePollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return pasZonePollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getSystemFaultPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("SystemFaultPollingIntervalInMillisecond");
        unsigned long systemFaultPollingIntervalInMillisecond = 0;

        std::string value = getHelper()->getParameter(SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND);
        if(value.empty())
        {
            systemFaultPollingIntervalInMillisecond = DEFAULT_SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND;
        }
        else
        {
            systemFaultPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return systemFaultPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getDvaLibPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("DvaLibPollingIntervalInMillisecond");
        unsigned long dvaLibPollingIntervalInMillisecond = 0;

        std::string value = getHelper()->getParameter(DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND);
        if(value.empty())
        {
            dvaLibPollingIntervalInMillisecond = DEFAULT_DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND;
        }
        else
        {
            dvaLibPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return dvaLibPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getPasDvaRecordingMaxDurationInSecond()
    {
        FUNCTION_ENTRY("PasDvaRecordingMaxDurationInSecond");
        unsigned long pasDvaRecordingMaxDurationInSecond = 0;

        std::string value = getHelper()->getParameter(PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND);
        if(value.empty())
        {
            pasDvaRecordingMaxDurationInSecond = DEFAULT_PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND;
        }
        else
        {
            pasDvaRecordingMaxDurationInSecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return pasDvaRecordingMaxDurationInSecond;
    }

} //close namespace TA_Base_Core

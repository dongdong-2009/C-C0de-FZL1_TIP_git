/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisConnectionEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MisConnectiontEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MisConnectiontEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MisConnectiontEntityData::ENTITY_TYPE                 = "MisConnection";
	const std::string MisConnectiontEntityData::PING_PERIOD_SEC = "PingPeriodInSeconds";
	const std::string MisConnectiontEntityData::MSG_CHECK_PERIOD_SEC = "CheckForMessagePeriodInSeconds";
	const std::string MisConnectiontEntityData::TIMEOUT_TENTH_SEC = "TimeoutInTenthSeconds";
	const std::string MisConnectiontEntityData::GLOBAL_MMS_INHIBIT = "GlobalMisInhibit";
	const std::string MisConnectiontEntityData::QUEUE_MANAGER_NAME = "QueueManagerName";
	const std::string MisConnectiontEntityData::ALARM_QUEUE_NAME = "AlarmQueueName";
    const std::string MisConnectiontEntityData::COUNTER_QUEUE_NAME = "CounterQueueName";
    const std::string MisConnectiontEntityData::REPORT_QUEUE_NAME = "ReportQueueName";
    const std::string MisConnectiontEntityData::SERVER_HOST_NAME = "ServerHostName";

	//xxs add
    const std::string MisConnectiontEntityData::MIS_CHANNEL_NAME = "MISChannelName";
    const std::string MisConnectiontEntityData::SCADA_CHANNEL_NAME = "SCADAChannelName";
    const std::string MisConnectiontEntityData::SCADA_REPORT_FILE_PATH = "SCADAReportFilePath";
	const std::string MisConnectiontEntityData::MIS_MESSAGE_STATUS_LOG_FILE = "MisMessageStatusLogFileName";
	const std::string MisConnectiontEntityData::LESS_FILEDATA_QUEUE_NAME = "LESSFileDataQueueName";
	const std::string MisConnectiontEntityData::MQ_SCADA_LISTEN_PORT = "SCADAListenerPort";
	//end xxs add

    MisConnectiontEntityData::MisConnectiontEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }


    MisConnectiontEntityData::~MisConnectiontEntityData()
    { 
    }


    IEntityData* MisConnectiontEntityData::clone(unsigned long key)
    {
        return new MisConnectiontEntityData(key);        
    }


    void MisConnectiontEntityData::invalidate()
    {
		getHelper()->invalidate();
    }

    unsigned int MisConnectiontEntityData::getPingPeriodSeconds()
    {
		unsigned int pingPeriodSeconds = 0;
		std::string value = getHelper()->getParameter(PING_PERIOD_SEC);
		if (!value.empty())
		{
			pingPeriodSeconds = static_cast< unsigned int >(EntityHelper::getUnsignedLongData(value));
		}
		
        return pingPeriodSeconds;
    }


    unsigned int MisConnectiontEntityData::getMessageCheckSeconds()
    {
        unsigned int messageCheckPeriodSecs = 0;
		std::string value = getHelper()->getParameter(MSG_CHECK_PERIOD_SEC);
		if (!value.empty())
		{
			messageCheckPeriodSecs = static_cast< unsigned int >(EntityHelper::getUnsignedLongData(value));
		}

        return messageCheckPeriodSecs;
    }


    unsigned int MisConnectiontEntityData::getTimeoutTenthSecs()
    {
        unsigned int serverTimeoutTenthSecs = 10;
		std::string value = getHelper()->getParameter(TIMEOUT_TENTH_SEC);
		if (!value.empty())
		{
			serverTimeoutTenthSecs = static_cast< unsigned int >(EntityHelper::getUnsignedLongData(value));
		}

        return serverTimeoutTenthSecs;
    }


	bool MisConnectiontEntityData::getGlobalMmsInhibit()
    {
		bool globalMmsInhibit = false;
		std::string value = getHelper()->getParameter(GLOBAL_MMS_INHIBIT);
		if (!value.empty())
		{
			globalMmsInhibit = EntityHelper::getBooleanData(value);
		}

        return globalMmsInhibit;
    }


    void MisConnectiontEntityData::setGlobalMmsInhibit(bool applied)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;

		std::string value = "1";
		if(!applied) value = "0";
        paramMap[ GLOBAL_MMS_INHIBIT ] = value;
        getHelper()->writeParameters( paramMap );
    }

    std::string MisConnectiontEntityData::getServerHostName()
    {
        return getHelper()->getParameter(SERVER_HOST_NAME);
    }

    std::string MisConnectiontEntityData::getQueueManagerName()
    {
        return getHelper()->getParameter(QUEUE_MANAGER_NAME);
    }


    std::string MisConnectiontEntityData::getAlarmQueueName()
    {
        return getHelper()->getParameter(ALARM_QUEUE_NAME);
    }

    std::string MisConnectiontEntityData::getCounterQueueName()
    {
        return getHelper()->getParameter(COUNTER_QUEUE_NAME);
    }


    std::string MisConnectiontEntityData::getReportQueueName()
    {
        return getHelper()->getParameter(REPORT_QUEUE_NAME);
    }


    std::string MisConnectiontEntityData::getType()
    {
        return getStaticType();
    }


    std::string MisConnectiontEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    std::string MisConnectiontEntityData::getSCADAChannelName()
    {
        return getHelper()->getParameter(SCADA_CHANNEL_NAME);
    }

    std::string MisConnectiontEntityData::getMISChannelName()
    {
        return getHelper()->getParameter(MIS_CHANNEL_NAME);
    }

    std::string MisConnectiontEntityData::getSCADAReportFilePath()
    {
        return getHelper()->getParameter(SCADA_REPORT_FILE_PATH);
    }

    //return the log file name which used to recode message sending status
	std::string MisConnectiontEntityData::getMisMessageStatusLogFileName()
	{
        return getHelper()->getParameter(MIS_MESSAGE_STATUS_LOG_FILE);
	}


    //getLessFileDataQueueName
	std::string MisConnectiontEntityData::getLessFileDataQueueName()
	{
        return getHelper()->getParameter(LESS_FILEDATA_QUEUE_NAME);
	}

	//getScadaListenerPort
	std::string MisConnectiontEntityData::getScadaListenerPort()
	{
        return getHelper()->getParameter(LESS_FILEDATA_QUEUE_NAME);
	}
}

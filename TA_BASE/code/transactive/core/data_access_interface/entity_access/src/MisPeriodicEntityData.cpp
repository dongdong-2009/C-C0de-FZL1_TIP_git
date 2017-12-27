/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisPeriodicEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/05/27 10:17:39 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MisPeriodicEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MisPeriodicEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string MisPeriodicEntityData::ENTITY_TYPE = "MisPeriodicSubmitter";
	const std::string MisPeriodicEntityData::SCAN_PERIOD_MIN = "ScanPeriodInMinutes";
	const std::string MisPeriodicEntityData::BACKLOG_CHECK_PERIOD_HOURS = "BacklogCheckPeriodInHours";
    const std::string MisPeriodicEntityData::LESS_SERVER_FTP_ADDRESS = "LessServerFTPAddress";
    const std::string MisPeriodicEntityData::LESS_SERVER_FTP_USERNAME = "LessServerFTPUsername";
    const std::string MisPeriodicEntityData::LESS_SERVER_FTP_PASSSWORD = "LessServerFTPPassword";
    const std::string MisPeriodicEntityData::LESS_SERVER_FTP_REPORT_FILENAME = "LessServerFTPReportFilename";
    const std::string MisPeriodicEntityData::LESS_SERVER_FTP_SYNCTIME = "LessServerFTPSyncTime";
	const std::string MisPeriodicEntityData::SCADA_FILE_RPORT_SCHEDULE_TIME = "ScadaFileReportScheduleTime";

    MisPeriodicEntityData::MisPeriodicEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }


    MisPeriodicEntityData::~MisPeriodicEntityData()
    { // Nothing to clean up
    }


    IEntityData* MisPeriodicEntityData::clone(unsigned long key)
    {
        return new MisPeriodicEntityData(key);        
    }


    void MisPeriodicEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    unsigned int MisPeriodicEntityData::getScanPeriodMinutes()
    {
		unsigned int scanPeriodMinutes = 10;
		std::string value = getHelper()->getParameter(SCAN_PERIOD_MIN);
		if (!value.empty())
		{
			scanPeriodMinutes = EntityHelper::getIntegerData(value);
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", SCAN_PERIOD_MIN.c_str() );
		}

        return scanPeriodMinutes;
    }


    unsigned int MisPeriodicEntityData::getBackLogCheckPeriodHours()
    {
		unsigned int backlogPeriodHours = 1;
		std::string value = getHelper()->getParameter(BACKLOG_CHECK_PERIOD_HOURS);
		if (!value.empty())
		{
			backlogPeriodHours = EntityHelper::getIntegerData(value);
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", BACKLOG_CHECK_PERIOD_HOURS.c_str() );
		}

        return backlogPeriodHours;
    }

    std::string MisPeriodicEntityData::getLessServerFtpAddress()
    {
		std::string value = getHelper()->getParameter(LESS_SERVER_FTP_ADDRESS);
		if ( value.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", LESS_SERVER_FTP_ADDRESS.c_str() );
			return "";
		}

        return value;
    }

    
    std::string MisPeriodicEntityData::getLessServerFtpUsername()
    {
		std::string value = getHelper()->getParameter(LESS_SERVER_FTP_USERNAME);
		if ( value.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", LESS_SERVER_FTP_USERNAME.c_str() );
			return "";
		}
        return value;
    }

    
    std::string MisPeriodicEntityData::getLessServerFtpPassword()
    {
		std::string value = getHelper()->getParameter(LESS_SERVER_FTP_PASSSWORD);
		if ( value.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", LESS_SERVER_FTP_PASSSWORD.c_str() );
			return "";
		}
        return value;		
    }

    
    std::string MisPeriodicEntityData::getLessServerFtpReportFilename()
    {
		std::string value = getHelper()->getParameter(LESS_SERVER_FTP_REPORT_FILENAME);
		if ( value.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", LESS_SERVER_FTP_REPORT_FILENAME.c_str() );
			return "";
		}
        return value;		
	}

	std::string MisPeriodicEntityData::parseTimeString( std::string & timeString, int & hour, int & minute )
	{
		hour = 23; 
		minute = 59;
		
		// Verify that SyncTime is in the format HH:MM
       if ( timeString.length() == 5 )
        {
            if (timeString[2] == ':')
            {
                char chrColon = 0;
                std::stringstream ssSyncTime;
                ssSyncTime << timeString;
                ssSyncTime >> hour;
                ssSyncTime >> chrColon;
                ssSyncTime >> minute;

                if ( chrColon == ':' &&
                     hour >= 0 && hour <= 23 &&
                     minute >= 0 && minute <= 59 )
                {
                    return timeString;
                }
            }
        }
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "MisPeriodicEntityData (%s) contains invalid value (%s); Using default value of '23:59'",
            getName().c_str(), timeString.c_str() );
		return "23:59";
	}
    
    std::string MisPeriodicEntityData::getLessServerFtpSyncTime( int& hour, int& minute )
    {
		std::string timeString = getHelper()->getParameter(LESS_SERVER_FTP_SYNCTIME);
		if ( timeString.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", LESS_SERVER_FTP_SYNCTIME.c_str()  );
		}
		return parseTimeString( timeString, hour, minute );
    }

	std::string MisPeriodicEntityData::getScadaFileReportScheduleTime( int& hour, int& minute )
	{
		std::string timeString = getHelper()->getParameter(SCADA_FILE_RPORT_SCHEDULE_TIME);
		if ( timeString.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s wasn't setup", SCADA_FILE_RPORT_SCHEDULE_TIME.c_str() );
		}
		return parseTimeString( timeString, hour, minute );

	}

    std::string MisPeriodicEntityData::getType()
    {
        return getStaticType();
    }


    std::string MisPeriodicEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationStatisticsThread.cpp $
 * @author:  Andy Parker
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * Statistics generation thread for notification service.
 * This is used to record notification statistics in the database
 * for diagnostic purposes.
 */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "NotificationStatisticsThread.h"
#include "omniNotify.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"

#include <ace/OS.h>

#include <time.h>

#include <map>
#include <string>
#include <vector>


using namespace TA_Base_App;


const long STATISTIC_OMNINOTIFY_ANNOUNCEMENT_PERSEC = 1;
const long STATISTIC_OMNINOTIFY_ANNOUNCEMENT_TOTAL  = 2;
const long STATISTIC_OMNINOTIFY_NOTIFICATION_PERSEC = 3;
const long STATISTIC_OMNINOTIFY_NOTIFICATION_TOTAL  = 4;

/**
 * Constructor
 *
 * Creates the statistics generation thread
 *
 */
NotificationStatisticsThread::NotificationStatisticsThread(const NotificationAgent::ChannelNameToChannelDetailsMap& channels)
							 :m_channels(channels),
							  m_terminate(false),
 							  m_startTime(0),
							  m_endTime(0)
{
    FUNCTION_ENTRY( "NotificationStatisticsThread" );

    // Initialise the stats map
    ChannelStatistics  entry;
    entry.totalAnnouncements.value  = 0;
    entry.announcementsPerSec.value = 0;
    entry.totalNotifications.value  = 0;
    entry.notificationsPerSec.value = 0;
    entry.totalAnnouncements.timeSecs  = 0;
    entry.announcementsPerSec.timeSecs = 0;
    entry.totalNotifications.timeSecs  = 0;
    entry.notificationsPerSec.timeSecs = 0;

    NotificationAgent::ChannelNameToChannelDetailsMap::const_iterator chanIT;
    for ( chanIT=m_channels.begin(); chanIT!=m_channels.end(); chanIT++ )
    {
        entry.channelName = chanIT->first;
        m_stats[chanIT->second.entityKey] = entry;
    }


    // Load the statistic frequency
    loadStatisticFrequency();

    FUNCTION_EXIT;
}


/**
 * Destructor
 *
 * Terminates the statistics generation thread.
 *
 */
NotificationStatisticsThread::~NotificationStatisticsThread()
{
    FUNCTION_ENTRY( "~NotificationStatisticsThread" );
    FUNCTION_EXIT;
}


/**
 * run()
 *
 * Performs the statistics generation
 *
 */
void NotificationStatisticsThread::run()
{
    FUNCTION_ENTRY( "run" );

    int runCycle = 61;
    while ( !m_terminate )
    {
        // Load the frequencies every minute (and first time)
        // CL-17309 ++
        // agentTerminate can't return if isAnyStatisticsRequired() always false
        while ( !m_terminate && runCycle>60 )
        // ++ CL-17309
        //while ( runCycle>60 )
        {
            if ( isAnyStatisticsRequired() )
            {
                runCycle=0;
            }
            else
            {
                sleep(60*1000);  // Wait another minute and then try again
            }
        }
        runCycle++;

        // Wait for another second to pass
        while ( m_startTime==m_endTime )
        {
            sleep(100);  // Wait another 100 ms for the end of the second
            m_endTime=time(NULL);
        }

        try
        {
            // Get the stats for each event channel
            getStatsForChannels();

            // Write all the stats to the database
            writeStatsForChannels();
        }
        catch (...)
        {
            LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught while processing statistics.");
        }

        m_startTime = m_endTime;
    }

    // Reset termination flag
    m_terminate = false;

    FUNCTION_EXIT;
}


void NotificationStatisticsThread::getStatsForChannels()
{
    FUNCTION_ENTRY( "getStatsForChannels" );

    // Calculate the time normalisation factor (in case we took more than 1 second)
    unsigned int timeNormalisation = m_endTime - m_startTime;

	// Get all the new stats
	NotificationAgent::ChannelNameToChannelDetailsMap::const_iterator chanIT;
	for (chanIT=m_channels.begin(); chanIT!=m_channels.end(); chanIT++)
	{
		if (chanIT->second.eventChannel!=NULL)
		{
			//AttNotification::EventChannel_ptr eventChannel = //RenQiang--, memory leaks 
            AttNotification::EventChannel_var eventChannel = AttNotification::EventChannel::_narrow(chanIT->second.eventChannel);
			if (!CORBA::is_nil(eventChannel))
			{
				// Get the statistics entry
				std::map<unsigned long,ChannelStatistics>::iterator   statsIT; 
				statsIT = m_stats.find(chanIT->second.entityKey);

                // Get standard stats
                AttNotification::ChannelStats_var  stats;
                stats = eventChannel->obtain_stats();

                // Update map of stats
                if ( m_startTime!=0 )
                {
                    statsIT->second.announcementsPerSec.value += stats->num_announcements - statsIT->second.totalAnnouncements.value;
                    statsIT->second.notificationsPerSec.value += stats->num_notifications - statsIT->second.totalNotifications.value;

                    // Update all the time counts
                    statsIT->second.totalAnnouncements.timeSecs += timeNormalisation;
                    statsIT->second.totalNotifications.timeSecs += timeNormalisation;
                    statsIT->second.announcementsPerSec.timeSecs += timeNormalisation;
                    statsIT->second.notificationsPerSec.timeSecs += timeNormalisation;
                }

                statsIT->second.totalAnnouncements.value = stats->num_announcements;
                statsIT->second.totalNotifications.value = stats->num_notifications;

            }
        }
    }

    FUNCTION_EXIT;
}


void NotificationStatisticsThread::writeStatsForChannels()
{
    FUNCTION_ENTRY( "writeStatsForChannels" );

    // Get access to the database
    TA_Base_Core::IDatabase* databaseConnection =
        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Statistics_Ad, TA_Base_Core::Write);

    struct tm transactiveTimeStruct;

    char transactiveTimeString[22];
    if ( ACE_OS::localtime_r(&m_endTime, &transactiveTimeStruct) != NULL )
    {
        sprintf(transactiveTimeString, "%02d/%02d/%04d %02d:%02d:%02d", transactiveTimeStruct.tm_mday, transactiveTimeStruct.tm_mon + 1,
                transactiveTimeStruct.tm_year + 1900, transactiveTimeStruct.tm_hour, transactiveTimeStruct.tm_min, transactiveTimeStruct.tm_sec);
    }

    std::map<unsigned long,ChannelStatistics>::iterator   statsIT;
    for ( statsIT = m_stats.begin();statsIT != m_stats.end(); statsIT++ )
    {
        if ( isStatisticReady(statsIT->second.announcementsPerSec.timeSecs,STATISTIC_OMNINOTIFY_ANNOUNCEMENT_PERSEC) )
        {
            writeStatToDatabase(*databaseConnection,STATISTIC_OMNINOTIFY_ANNOUNCEMENT_PERSEC,
                                transactiveTimeString,
                                statsIT->second.announcementsPerSec.value/statsIT->second.announcementsPerSec.timeSecs,
                                statsIT->first);
            statsIT->second.announcementsPerSec.timeSecs = 0;
            statsIT->second.announcementsPerSec.value = 0;

            // Correct for time (normalise to actions per second)
            //        statsIT->second.announcementsPerSec.value = statsIT->second.announcementsPerSec.value/timeNormalisation;
            //        statsIT->second.notificationsPerSec.value = statsIT->second.notificationsPerSec.value/timeNormalisation;
        }

        if ( isStatisticReady(statsIT->second.totalAnnouncements.timeSecs,STATISTIC_OMNINOTIFY_ANNOUNCEMENT_TOTAL) )
        {
            writeStatToDatabase(*databaseConnection,STATISTIC_OMNINOTIFY_ANNOUNCEMENT_TOTAL,
                                transactiveTimeString,statsIT->second.totalAnnouncements.value,statsIT->first);
            statsIT->second.totalAnnouncements.timeSecs = 0;
        }

        if ( isStatisticReady(statsIT->second.notificationsPerSec.timeSecs,STATISTIC_OMNINOTIFY_NOTIFICATION_PERSEC) )
        {
            writeStatToDatabase(*databaseConnection,STATISTIC_OMNINOTIFY_NOTIFICATION_PERSEC,
                                transactiveTimeString,
                                statsIT->second.notificationsPerSec.value/statsIT->second.notificationsPerSec.timeSecs,
                                statsIT->first);
            statsIT->second.notificationsPerSec.timeSecs = 0;
            statsIT->second.notificationsPerSec.value = 0;
        }

        if ( isStatisticReady(statsIT->second.totalNotifications.timeSecs,STATISTIC_OMNINOTIFY_NOTIFICATION_TOTAL) )
        {
            writeStatToDatabase(*databaseConnection,STATISTIC_OMNINOTIFY_NOTIFICATION_TOTAL,
                                transactiveTimeString,statsIT->second.totalNotifications.value,statsIT->first);
            statsIT->second.totalNotifications.timeSecs = 0;
        }
    }

    FUNCTION_EXIT;
}


/**
 * terminate()
 *
 * Stops the statistics generation
 *
 */
void NotificationStatisticsThread::terminate()
{
    FUNCTION_ENTRY( "terminate" );

    m_terminate = true;

    FUNCTION_EXIT;
}


void NotificationStatisticsThread::writeStatToDatabase( TA_Base_Core::IDatabase& db,
                                                        long statType,
                                                        char* time,long value,
                                                        unsigned long entityKey )
{
    FUNCTION_ENTRY( "writeStatToDatabase" );

	//const char * insertBase = "INSERT INTO ST_VALUES (STVALU_ID,STTYPE_ID,CREATETIME,VALUELONG,ENTITYKEY) VALUES (";


    // Build the insert string
	//std::ostringstream sql;
    //sql.str("");
    //sql << insertBase << "STVALU_SEQ.nextval," 
	//				  << statType << ","
	//	              << "TO_DATE('" << time <<"','dd/mm/yyyy hh24:mi:ss'),"
	//				  << value << ","
	//				  << entityKey << ")";
    try
    {
		//db.executeModification(sql.str());

		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		db.prepareSQLStatement(strSql, ST_VALUES_INSERT_24551,
			statType, time, value, entityKey);
		db.executeModification(strSql);
    }
    catch (...)
    {
        // Did not make it - possibly statistic key not defined
        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             "Unable to save statistic on the database.");
    }

    FUNCTION_EXIT;
}


void NotificationStatisticsThread::loadStatisticFrequency()
{
    FUNCTION_ENTRY( "loadStatisticFrequency" );

	//const char* select = "SELECT STTYPE_ID,FREQUENCY FROM ST_TYPE";

    try
    {
        // Get access to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Statistics_Ad, TA_Base_Core::Read);

        std::vector<std::string> columns;
        columns.push_back("STTYPE_ID");
        columns.push_back("FREQUENCY");
		//TA_Base_Core::IData*  results = databaseConnection->executeQuery(select,columns);
		//TA_ASSERT(results!=NULL,"No result returned from query.");
		
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;		
		databaseConnection->prepareSQLStatement(strSql, ST_TYPE_SELECT_25001);
		TA_Base_Core::IData*  results = databaseConnection->executeQuery(strSql, columns);
        TA_ASSERT(results!=NULL,"No result returned from query.");

        // Clear the map
        m_statsFrequency.clear();

        // Loop for each row returned
        do
        {
            // Load all the selected records
            for ( unsigned long row = 0;row<results->getNumRows();row++ )
            {
                long statType = results->getUnsignedLongData(row,columns[0]);
                long statFreq = results->getUnsignedLongData(row,columns[1]);
                m_statsFrequency[statType] = statFreq;
            }

            delete results;
            results = NULL;
        }
        while ( databaseConnection->moreData(results) );
    }
    catch (...)
    {
        // Did not get them
        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to load statistic types from the database.");
    }

    FUNCTION_EXIT;
}


bool NotificationStatisticsThread::isAnyStatisticsRequired()
{
    FUNCTION_ENTRY( "isAnyStatisticsRequired" );

    if ( m_statsFrequency[STATISTIC_OMNINOTIFY_ANNOUNCEMENT_PERSEC]>0 )
    {
        FUNCTION_EXIT;
        return true;
    }

    if ( m_statsFrequency[STATISTIC_OMNINOTIFY_ANNOUNCEMENT_TOTAL]>0 )
    {
        FUNCTION_EXIT;
        return true;
    }

    if ( m_statsFrequency[STATISTIC_OMNINOTIFY_NOTIFICATION_PERSEC]>0 )
    {
        FUNCTION_EXIT;
        return true;
    }

    if ( m_statsFrequency[STATISTIC_OMNINOTIFY_NOTIFICATION_TOTAL]>0 )
    {
        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


bool NotificationStatisticsThread::isStatisticReady(long currentSecs,long statType)
{
    FUNCTION_ENTRY( "isStatisticReady" );

    if ( 0 == m_statsFrequency[statType] )
    {
        // Not required at all
        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return ( currentSecs >= m_statsFrequency[statType] );
}

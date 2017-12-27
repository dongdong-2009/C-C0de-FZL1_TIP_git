#ifndef NOTIFICATION_STATISTICS_THREAD
#define NOTIFICATION_STATISTICS_THREAD
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationStatisticsThread.h $
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

#include "NotificationAgent.h"

#include "core/threads/src/Thread.h"

#include <time.h>

#include <map>
#include <string>
#include <vector>


namespace TA_Base_Core
{
    class IDatabase;  // Forward declaration
}

namespace TA_Base_App
{

    class NotificationStatisticsThread : public TA_Base_Core::Thread
    {

        public:

			/**
			 * Constructor
			 *
			 * Creates the statistics generation thread
			 *
			 */
			NotificationStatisticsThread(const NotificationAgent::ChannelNameToChannelDetailsMap& channels);


            /**
             * Destructor
             *
             * Terminates the statistics generation thread.
             *
             */
            virtual ~NotificationStatisticsThread();


            /**
             * run()
             *
             * Performs the statistics generation
             *
             */
            void run();

            /**
             * terminate()
             *
             * Stops the statistics generation
             *
             */
            void terminate();

        private:

            /**
             * getStatsForChannels()
             *
             * Gets all the statistics
             *
             */
            void getStatsForChannels();


            /**
             * writeStatsForChannels()
             *
             * writes all the statistics to the database
             *
             */
            void writeStatsForChannels();

            /**
             * writeStatToDatabase()
             *
             * writes the statistic to the database
             *
             */
            void writeStatToDatabase(TA_Base_Core::IDatabase& db, long statType, char* time,
                                     long value,unsigned long entityKey);


            /**
             * loadStatisticFrequency()
             *
             * gets the current frequencies for each statistic to be monitored
             *
             */
            void loadStatisticFrequency();

            /**
             * isAnyStatisticsRequired()
             *
             * indicates if any statisical logging is required
             *
             */
            bool isAnyStatisticsRequired();

            /**
             * isStatisticReady()
             *
             * indicates if a specific statisic is ready for logging
             * as indicated by reporting frequency
             *
             */
            bool isStatisticReady(long currentSecs,long statType);

        private:

            typedef struct
            {
                long    value;
                long    timeSecs;
            } StatCounter;

            typedef struct
            {
                std::string  channelName;
                StatCounter  totalAnnouncements;
                StatCounter  totalNotifications;
                StatCounter  announcementsPerSec;
                StatCounter  notificationsPerSec;
            } ChannelStatistics;

            std::map<unsigned long,ChannelStatistics>   m_stats;
            time_t                                      m_startTime;
            time_t                                      m_endTime;

            // Map the frequencies for each stat type
            std::map<unsigned long, unsigned long>     m_statsFrequency;

            bool   m_terminate;

			const NotificationAgent::ChannelNameToChannelDetailsMap&        m_channels;

	};
}
#endif // NOTIFICATION_STATISTICS_THREAD

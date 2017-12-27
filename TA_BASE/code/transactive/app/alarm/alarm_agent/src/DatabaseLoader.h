/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/DatabaseLoader.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This thread periodically instructs the AlarmCache to reload its alarms.
  * It stops when terminated. See the AlarmCache loadAlarms method to see
  * when/how it is used.
  */
#if !defined(DATABASELOADER)
#define DATABASELOADER

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{

    class IDataLoader; // Forward Declaration

        class DatabaseLoader : public virtual TA_Base_Core::Thread
    {
    public:
        
        /**
          * Constructor
          *
          * @param The IDataLoader that we will call back on
          * @param The total time that the DatabaseLoader will wait 
          *        before attempting to reload data (milliseconds)
          * @param The period at which the DatabaseLoader will check 
          *        whether or not it has reached the pollingPeriod (milliseconds)
          */
		//TD14082
		//marvin++
        DatabaseLoader( IDataLoader* loader, unsigned long p_pollingPeriod = 60000, unsigned long p_sleepTime = 150 );
		//TD14082
		//++marvin

        /**
          * Destructor
          */
        virtual ~DatabaseLoader();

        /**
          * run
          *
          * Overides the pure virtual run method declared in Thread
          */
        void run();

        /**
          * terminate
          *
          * Overides the pure virtual terminate method declared in Thread
          */
        void terminate();

		/**
		 * acquire
		 *
		 * This is to inform this DB loader whether there is need to acquire or load again
		 * as the previous loading has succeeded or failed
		 *
		 * @param bool toAcquire This provides the instruction to acquire: True=to acquire and False=not to acquire
		 */
		void acquire(bool toAcquire);

    private:

        DatabaseLoader( const DatabaseLoader& theDatabaseLoader);
        DatabaseLoader& operator=(const DatabaseLoader&);

        unsigned long m_pollingPeriod;
        unsigned long m_sleepTime;
    
        bool m_terminated;
        IDataLoader* m_loader;

		// TD 14082 This is to indicate there is need to acquire again
		bool m_acquire;
    };
}

#endif // !defined(DATABASELOADER)

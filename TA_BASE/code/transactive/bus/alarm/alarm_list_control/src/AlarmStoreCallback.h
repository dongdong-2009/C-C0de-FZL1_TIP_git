/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmStoreCallback.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This object is called by the alarm store to alert the list of alarm updates and changes.
  */

#ifndef ALARM_STORE_CALLBACK_WED_04_02_2004_H
#define ALARM_STORE_CALLBACK_WED_04_02_2004_H

#include <queue>
#include <vector>
#include <string>

#include "bus/alarm/alarm_list_control/src/CommonStructures.h"

#include "core/alarm/IDL/src/AlarmStoreCallbackCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Bus
{

    class AlarmStoreCallback :  public virtual POA_TA_Base_Core::IAlarmStoreCallbackCorbaDef,
                                public TA_Base_Core::ServantBase
    {
    public:
       /**
        * Constructor
        */
        AlarmStoreCallback();


       /**
        * Destructor
        */
        virtual ~AlarmStoreCallback();


       /**
        * indexCleared
        *
        * This method will be called when the alarms in the index are cleared.  This
        * might occur if the connection to an agent is lost.
        */
        virtual void indexCleared();


       /**
        * itemInserted
        *
        * This method is called when a new alarm is added to the index. 
        *
        * @param - alarmId - Unique alarm id being added
        * @param - position - Position in index where alarm should be placed (zero based) 
        */
        virtual void itemInserted(const char* alarmID, long position);


       /**
        * itemMoved
        *
        * This method is called when an item in the index should be moved. 
        *
        * @param - alarmId - Unique alarm id being deleted
        * @param - oldPosition - Position in index where alarm was located (zero based) 
        * @param - newPosition - Position in index where alarm should now be located (after delete,zero based) 
        */
        virtual void itemMoved(const char* alarmID, long oldPosition, long newPosition);


       /**
        * itemDeleted
        *
        * This method is called when a new alarm is added to the index. 
        *
        * @param - alarmId - Unique alarm id being deleted
        * @param - position - Position in index where alarm was located (zero based) 
        */
        virtual void itemDeleted(const char* alarmID, long position);


        /**
         * callbackCheck
         *
         * This method is called when the store wishes to validate a callback to
         * see it is ok.  The method should return true to indicate that the
         * store should retain the index.  Returning false, or not responding
         * will result in the index being released. 
         */
        virtual bool callbackCheck();


        /**
         * updateWithQueuedChanges
         * 
         * This will update the alarm list with all the current alarm updates
         *
         * @param AlarmList& - This list will have all alarm updates made to it
         * @param bool& - Set to true if any new alarms were added.
         *
         * @return vector<string> - This holds all UUID's that have had data updated. They will need
         *                          to be reloaded if they are currently in the cache.
         */
        std::vector<std::string> updateWithQueuedChanges(AlarmList& alarmList, bool& wereAlarmsAdded);

		/** TD 12710
		 * These functions are to stop the updating of alarm list from the queue changes
		 * and to clear the queue of changes
		 *
		 */
		void clearQueueOfChanges();
		void terminateChanges(bool toTerminate);


    private:
        enum EChangeType
        {
            CLEARED,
            INSERTED,
            MOVED,
            DELETED
        };


        struct SItemChange
        {
            std::string alarmId;    // The Id of the alarm this change refers to
            EChangeType change;     // The type of change this is ie insert, move, delete etc
            long position;          // The position of this alarm in the list
            long oldPosition;       // The old position of this alarm in the list (only valid for moves).
        };

        // The queue of changes to be made to the alarm list. New changes are pushed on the end of
        // the list and updates should be popped off the front.
        std::queue<SItemChange> m_queueOfChanges;

		bool m_terminateUpdate; // TD 12710 this is to discontinue the updatewithqueuedchanges while loop

        // The thread guard for the queue of changes.
        TA_Base_Core::NonReEntrantThreadLockable m_queueLock;
    };
}

#endif // ALARM_STORE_CALLBACK_WED_04_02_2004_H

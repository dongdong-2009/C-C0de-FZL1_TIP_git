/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmStoreCallback.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This object is called by the alarm store to alert the list of alarm updates and changes.
  */

#include "bus/alarm/alarm_list_control/src/AlarmStoreCallback.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    AlarmStoreCallback::AlarmStoreCallback()
    {
        // servant is activated automatically when _this() is called in the AlarmStore.
		m_terminateUpdate = false;
    }


    AlarmStoreCallback::~AlarmStoreCallback()
    {
    }


    void AlarmStoreCallback::indexCleared()
    {
        SItemChange newChange;
        newChange.change = CLEARED;
        // All other fields will be unused so just set to any value
        newChange.alarmId = "";
        newChange.position = -1;
        newChange.oldPosition = -1;

        TA_Base_Core::ThreadGuard guard(m_queueLock);
        m_queueOfChanges.push(newChange);
        
    }


    void AlarmStoreCallback::itemInserted(const char* alarmID, long position)
    {
        SItemChange newChange;
        newChange.alarmId = alarmID;
        newChange.change = INSERTED;
        newChange.position = position;
        newChange.oldPosition = -1;

        TA_Base_Core::ThreadGuard guard(m_queueLock);
        m_queueOfChanges.push(newChange);
    }


    void AlarmStoreCallback::itemMoved(const char* alarmID, long oldPosition, long newPosition)
    {
        SItemChange newChange;
        newChange.alarmId = alarmID;
        newChange.change = MOVED;
        newChange.position = newPosition;
        newChange.oldPosition = oldPosition;

        TA_Base_Core::ThreadGuard guard(m_queueLock);
        m_queueOfChanges.push(newChange);
    }


    void AlarmStoreCallback::itemDeleted(const char* alarmID, long position)
    {
        SItemChange newChange;
        newChange.alarmId = alarmID;
        newChange.change = DELETED;
        newChange.position = position;
        newChange.oldPosition = -1;

        TA_Base_Core::ThreadGuard guard(m_queueLock);
        m_queueOfChanges.push(newChange);
    }


    bool AlarmStoreCallback::callbackCheck()
    {
        return true;
    }

	void AlarmStoreCallback::clearQueueOfChanges()
	{
		FUNCTION_ENTRY("clearQueueOfChanges");
		
		// Done with this item so pop it off the queue
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		while (!m_queueOfChanges.empty())
		{
			m_queueOfChanges.pop();
		}

		FUNCTION_EXIT;
	}

	void AlarmStoreCallback::terminateChanges(bool isTerminate)
	{
		m_terminateUpdate = isTerminate;
	}

    std::vector<std::string> AlarmStoreCallback::updateWithQueuedChanges(AlarmList& alarmList, bool& wereAlarmsAdded)
    {
        FUNCTION_ENTRY("updateWithQueuedChanges");

        wereAlarmsAdded = false;
        std::vector<std::string> alarmsToUpdateDataFor;

        int count = 0;
        while(!m_queueOfChanges.empty() && count < 500)
        {
			if (m_terminateUpdate) break;

            ++count;
            SItemChange nextChange = m_queueOfChanges.front();

            switch(nextChange.change)
            {
                case CLEARED:
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "List cleared");

                    // Clear the list
                    AlarmList().swap(alarmList);
                    break;
                }

                case INSERTED:
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Inserting %s in position %d", nextChange.alarmId.c_str(), nextChange.position);
                    alarmsToUpdateDataFor.push_back(nextChange.alarmId);
                    
                    if (nextChange.position > static_cast<long>(alarmList.size()))
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Asked to insert into a position greater than our alarm list");
                        
                        // Reset the position and insert the alarm at the top of the list
                        nextChange.position = 0;
                    }

                    // Find the iterator in the specified position so we know where to insert the new item
                    AlarmList::iterator position = alarmList.begin();
                    std::advance(position, nextChange.position);
                    
                    // Create a structure for the new alarm
                    SAlarmDetails details;
                    details.uuid = nextChange.alarmId;
                    details.isSelected = false;
                    
                    // Add it to the list
                    alarmList.insert(position, details);

                    wereAlarmsAdded = true;

                    break;
                }

                case MOVED:
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Moving %s from position %d to position %d", nextChange.alarmId.c_str(), nextChange.oldPosition, nextChange.position);
                    alarmsToUpdateDataFor.push_back(nextChange.alarmId);

                    if (nextChange.position > static_cast<long>(alarmList.size()))
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Asked to move into a position greater than our alarm list");
                        break;
                    }

                    if (nextChange.position == nextChange.oldPosition)
                    {
                        // This was just an update did not result in a move so we are done processing
                        break;
                    }

                    // Set up the item we are searching for
                    SAlarmDetails details;
                    details.uuid = nextChange.alarmId;
                    details.isSelected = false;     // This doesn't really matter but we'll give it a value anyway

                    // Find the item
                    AlarmList::iterator position = std::find(alarmList.begin(), alarmList.end(), details);
                    if (position != alarmList.end())
                    {
                        // Now save the details and remove it
                        details = *position;
                        alarmList.erase(std::remove(alarmList.begin(), alarmList.end(), details), alarmList.end());
                    }

                    // Find the iterator in the specified position so we know where to move the item to
                    position = alarmList.begin();
                    std::advance(position, nextChange.position);

                    // Add it to the list in its new position
                    alarmList.insert(position, details);

                    break;
                }

                case DELETED:
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Deleting %s from position %d", nextChange.alarmId.c_str(), nextChange.position);
                    alarmsToUpdateDataFor.push_back(nextChange.alarmId);

                    // Find the iterator in the specified position so we know where to delete the item from
                    SAlarmDetails itemToMatch;
                    itemToMatch.uuid = nextChange.alarmId;

                    // Erase all uuid's matching the one passed to us. Since the uuid's are unique then only one
                    // item should be erased.
                    alarmList.erase(std::remove(alarmList.begin(), alarmList.end(), itemToMatch), alarmList.end());

                    break;
                }
            }

            // Done with this item so pop it off the queue
           TA_Base_Core::ThreadGuard guard(m_queueLock);
           m_queueOfChanges.pop();
        }

        FUNCTION_EXIT;
        return alarmsToUpdateDataFor;
    }


}

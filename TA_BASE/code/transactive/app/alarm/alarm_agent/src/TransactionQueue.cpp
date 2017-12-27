/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

// CL-21508
#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"
#include "app/alarm/alarm_agent/src/TransactionQueue.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::Semaphore;

namespace TA_Base_App
{

    //
    // Constructor
    //
    TransactionQueue::TransactionQueue()
    : m_terminated(false), m_queueSemaphore(0)
    {}


    //
    // Destructor
    //
    TransactionQueue::~TransactionQueue()
    {
        clear();
    }


    //
    // push
    //
    // CL-21508 begin
//     void TransactionQueue::push( TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item )
    void TransactionQueue::push( AlarmQueueWrapper* pWrapper )
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "push");

        ThreadGuard guard(m_lock);

        TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item = pWrapper->getAlarmAgentQueueItem();

        if( !m_terminated )
        {            
			
			//fix: alarm agent cannot start to monitor mode when the control mode alarmagent has queue items.
			//     we need to give a default value to these enum types, otherwise the monitor agent raise "MARSHAL_InvalidEnumValue" exception.
			if( p_item->containsAlarmUpdateCorbaDef )
			{
				p_item->theMessage.state=TA_Base_Core::AlarmOpen;
				p_item->theMessage.mmsState=TA_Base_Core::MMS_NONE;
				p_item->theMessage.decisionSupportState=TA_Base_Core::NoRelatedPlan;				
			}
			else
			{
				if( p_item->containsAlarmDetailCorbaDef )
				{				 
					p_item->theUpdate.mmsState=TA_Base_Core::MMS_NONE;
				    p_item->theUpdate.decisionSupportState=TA_Base_Core::NoRelatedPlan;		 
				}
			}
// 			m_updateQueue.push_back( p_item );
			m_updateQueue.push_back( pWrapper );
            m_queueSemaphore.post();
        }
    }
    

    //
    // getCurrentQueue
    //
    TA_Base_Core::AlarmAgentQueueItemSequence* TransactionQueue::getCurrentQueue()
    {
        ThreadGuard guard(m_lock);
		ThreadGuard guardWrite(m_lockWriteQueue);

        TA_Base_Core::AlarmAgentQueueItemSequence* seq = new TA_Base_Core::AlarmAgentQueueItemSequence();

        unsigned int len = m_updateQueue.size()+ m_writeQueue.size() ;
        seq->length(len);

        UpdateQueue::const_iterator p = m_updateQueue.begin();

        unsigned int i = 0;

        while( (p != m_updateQueue.end()) && (i < len) )
        {
            // CL-21508 begin
//             (*seq)[i] = **p;
            (*seq)[i] = *((*p)->getAlarmAgentQueueItem());
            // CL-21508 end
            p++;
            i++;
        }

		UpdateQueue::const_iterator pWrite = m_writeQueue.begin();
		while( (pWrite != m_writeQueue.end()) && (i < len) )
		{
			// CL-21508 begin
			//             (*seq)[i] = **p;
			(*seq)[i] = *((*pWrite)->getAlarmAgentQueueItem());
			// CL-21508 end
			pWrite++;
			i++;
		}
        return seq;
    }

	void TransactionQueue::cleanUpdateQeueu()
	{
		ThreadGuard guard(m_lock);		
		UpdateQueue::iterator p;
		UpdateQueue::iterator begin = m_updateQueue.begin();
		UpdateQueue::iterator end = m_updateQueue.end();

		for( p = begin; p != end; p++ )
		{
			if( *p != NULL )
			{
				delete *p;
			}
		}
		m_updateQueue.clear();

	}
	void TransactionQueue::cleanWriteQueue()
	{
		ThreadGuard guard(m_lockWriteQueue);
		UpdateQueue::iterator p;
		UpdateQueue::iterator begin = m_writeQueue.begin();
		UpdateQueue::iterator end = m_writeQueue.end();
		for( p = begin; p != end; p++ )
		{
			if( *p != NULL )
			{
				delete *p;
			}
		}
		m_writeQueue.clear();
	}
    //
    // clear
    //
    void TransactionQueue::clear()
    {
		 cleanUpdateQeueu();
		 cleanWriteQueue();
    }


    //
    // getQueue
    //
    // CL-21508
    TransactionQueue::UpdateQueue& TransactionQueue::getWriteQueue()
    {
        return m_writeQueue;
    }

	//
	// popTop
	//
	AlarmQueueWrapper* TransactionQueue::popTopWriteQueue()
	{

		AlarmQueueWrapper *wrapper = NULL;
		ThreadGuard guard(m_lockWriteQueue);
		wrapper = m_writeQueue.front();
		m_writeQueue.pop_front();
		return wrapper;
	}

	void TransactionQueue::pushTopWriteQueue(AlarmQueueWrapper* pWrapper )
	{
		if (pWrapper==NULL) return;
		ThreadGuard guard(m_lockWriteQueue);		
		m_writeQueue.push_front(pWrapper);
	}

	//
	//
	// isEmptyWriteQueue
	//
	bool TransactionQueue::isEmptyWriteQueue()
	{
		ThreadGuard guard(m_lockWriteQueue);

		return m_writeQueue.empty();
	}

	//
	// size
	//
	size_t TransactionQueue::size()
	{
		ThreadGuard guard(m_lock);

		return m_updateQueue.size();
	}

	void TransactionQueue::MoveToWriteQueue()
	{
		ThreadGuard guard(m_lock);
		
		if (!m_updateQueue.empty())
		{
			ThreadGuard guardWrite(m_lockWriteQueue);
			m_writeQueue.insert(m_writeQueue.end(), m_updateQueue.begin(), m_updateQueue.end());			
			m_updateQueue.clear();
		}
	}

	bool TransactionQueue::removeUpdateQueue(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item)
	{
		//remove from writeQueue
		bool ret=false;
		ThreadGuard guard(m_lock);
		UpdateQueue& updateQueue = m_updateQueue;

		UpdateQueue::iterator p;
		UpdateQueue::iterator begin = updateQueue.begin();
		UpdateQueue::iterator end = updateQueue.end();

		for( p = begin; p != end; p++ )
		{
			if( (*p)->getAlarmAgentQueueItem()->containsAlarmDetailCorbaDef == p_item.containsAlarmDetailCorbaDef )
			{
				if( p_item.containsAlarmDetailCorbaDef )
				{
					if( 0 == std::string(p_item.theMessage.alarmID).compare((*p)->getAlarmAgentQueueItem()->theMessage.alarmID) )
					{
						// This is it
						ret=true;
						delete *p;
						updateQueue.erase(p);
						break;
					}
				}
				else
				{
					if( 0 == std::string(p_item.theUpdate.alarmID).compare((*p)->getAlarmAgentQueueItem()->theUpdate.alarmID) )
					{
						if( p_item.theUpdate.alarmHasBeenAcknowledged == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenAcknowledged )
						{
							if( p_item.theUpdate.alarmHasBeenClosed == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenClosed )
							{
								if( p_item.theUpdate.decisionSupportStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.decisionSupportStateHasChanged )
								{
									if (p_item.theUpdate.mmsStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.mmsStateHasChanged )
									{
										// This is it
										ret=true;
										delete *p;
										updateQueue.erase(p);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		if (ret)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"update Queue size is %d", updateQueue.size());
		}
		return ret;

	}
	bool TransactionQueue::removeWriteQueue(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item)
	{
		//remove from writeQueue
		bool ret=false;
		ThreadGuard guard(m_lockWriteQueue);
		UpdateQueue& updateQueue = m_writeQueue;

		UpdateQueue::iterator p;
		UpdateQueue::iterator begin = updateQueue.begin();
		UpdateQueue::iterator end = updateQueue.end();

		for( p = begin; p != end; p++ )
		{
			if( (*p)->getAlarmAgentQueueItem()->containsAlarmDetailCorbaDef == p_item.containsAlarmDetailCorbaDef )
			{
				if( p_item.containsAlarmDetailCorbaDef )
				{
					if( 0 == std::string(p_item.theMessage.alarmID).compare((*p)->getAlarmAgentQueueItem()->theMessage.alarmID) )
					{
						// This is it
						ret=true;
						delete *p;
						updateQueue.erase(p);
						break;
					}
				}
				else
				{
					if( 0 == std::string(p_item.theUpdate.alarmID).compare((*p)->getAlarmAgentQueueItem()->theUpdate.alarmID) )
					{
						if( p_item.theUpdate.alarmHasBeenAcknowledged == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenAcknowledged )
						{
							if( p_item.theUpdate.alarmHasBeenClosed == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenClosed )
							{
								if( p_item.theUpdate.decisionSupportStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.decisionSupportStateHasChanged )
								{
									if (p_item.theUpdate.mmsStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.mmsStateHasChanged )
									{
										// This is it
										ret=true;
										delete *p;
										updateQueue.erase(p);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		if (ret)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
			"write Queue size is %d", updateQueue.size());
		}
	    return ret;
	}

	void TransactionQueue::remove(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item)
	{
		 if (!removeUpdateQueue(p_item))
		 {
			 removeWriteQueue(p_item);
		 }
		

	}
 
};

#include "app/scada/OpcBridgeLib/src/TagThread.h"
#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_App
{	
	TagThread::TagThread() 
		: m_bTerminate(false), m_updateCnt(0), m_semaphore(0), m_head(NULL), m_tail(NULL)
	{
		start();
	}
	
	TagThread::~TagThread()
	{
		terminateAndWait();
	}

	void TagThread::terminate()
	{
		m_bTerminate = true;
		m_semaphore.post();
	}

	void TagThread::run()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--Tag thread start %u", getId());

		AbstractDataObjectAttributes * item = NULL;
		
		while(!m_bTerminate)
		{
			item = getItem();

			if (item != NULL)
			{
				try
				{
					item->processProxyUpdate();
					++m_updateCnt;
				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception happened when processProxyUpdate %u", getId());
				}
			}
			
			m_semaphore.wait();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--Tag thread stop %u", getId());
	}

	AbstractDataObjectAttributes * TagThread::getItem()
	{
		TA_Base_Core::ThreadGuard guard(m_queueLock);

		AbstractDataObjectAttributes * item = NULL;

		if (m_head != NULL)
		{
			item = m_head;
			m_head = item->m_next;
			if (m_head == NULL)
			{
				m_tail = NULL;
			}
			item->m_next = NULL;
		}

		return item;
	}

	void TagThread::insertUniqueItem(AbstractDataObjectAttributes * item)
	{
		TA_Base_Core::ThreadGuard guard(m_queueLock);

		if (item->m_next == NULL && item != m_tail)
		{
			if (m_head == NULL)
			{
				m_head = item;
			}
			else
			{
				m_tail->m_next = item;
			}

			m_tail = item;
			m_semaphore.post();
		}
	}

	unsigned int TagThread::getUpdateCnt()
	{
		return m_updateCnt;
	}
}


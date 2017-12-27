#ifndef TAG_THREAD_H
#define TAG_THREAD_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

namespace TA_Base_App
{
	class AbstractDataObjectAttributes;
	
	class TagThread : public TA_Base_Core::Thread
	{
	public:
		TagThread();
		~TagThread();

		void run();

		void terminate();

		void insertUniqueItem(AbstractDataObjectAttributes *);

		unsigned int getUpdateCnt();

	private:
		AbstractDataObjectAttributes *  getItem();

		bool										m_bTerminate;
		unsigned int								m_updateCnt;
		TA_Base_Core::NonReEntrantThreadLockable	m_queueLock;
		TA_Base_Core::Semaphore						m_semaphore;

		AbstractDataObjectAttributes * 				m_head;
		AbstractDataObjectAttributes * 				m_tail;
	};
}

#endif

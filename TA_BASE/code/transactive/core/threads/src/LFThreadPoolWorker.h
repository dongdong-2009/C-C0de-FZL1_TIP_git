/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/SCADA_development/TA_BASE/transactive/core/threads/src/LFThreadPoolWorker.h $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  *Leader / Follower thread pool' worker.
  *
  */

#ifndef LFThreadPoolWorker_H
#define LFThreadPoolWorker_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

#include "./IThreadPoolBase.h"

namespace TA_Base_Core
{
    class IThreadPoolWorkerCallback;
};

namespace TA_Base_Core
{
	class LFThreadPoolWorker : public Thread, public IListItem
	{
	public:

		LFThreadPoolWorker(ILFThreadPoolManager& callback);
		virtual ~LFThreadPoolWorker();

		virtual void run();
		virtual void terminate();


		void changeToLeader();

	private:

        LFThreadPoolWorker();

		void doLeaderWorker();

		volatile bool m_threadRunning;
		ReEntrantThreadLockable m_lock;
		Semaphore m_semaphore;
        ILFThreadPoolManager& m_callback;
	};
};

#endif

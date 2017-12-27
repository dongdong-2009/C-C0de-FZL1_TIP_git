/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$Source: /cvs/1TS-9999-T/code/transactive/core/threads/src/Attic/IThreadPoolWorkerCallback.h,v $
  * @author:	Adam Radics
  * @version:	$Revision: 1.1.2.1 $
  *
  * Last modification:	$Date: 2006/03/15 06:50:46 $
  * Last modified by:	$Author: darrens $
  *
  * Description:
  *
  * This is an interface implemented by the thread pool singleton manager
  * it is used by worker threads to notify the manager when they are free
  * in order for the manager to allocate a new task.
  *
  */

#ifndef THREADPOOLWORKER_CALLBACK_H
#define THREADPOOLWORKER_CALLBACK_H


namespace TA_Base_Core
{

	class IThreadPoolWorkerCallback
	{

	public:

		virtual ~IThreadPoolWorkerCallback(){};

		/**
		*
		* workerFree
		*
		* This is called by a worker when it becomes free
		*
		*/
		virtual void workerFree() = 0;
	};

}

#endif

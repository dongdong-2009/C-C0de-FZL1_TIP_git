/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/SCADA_development/TA_BASE/transactive/core/threads/src/IThreadPoolBase.h $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  *
  */

#ifndef ITHREAD_POOL_BASE_H
#define ITHREAD_POOL_BASE_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Core
{
	class DuLinkList;
	class SgLinkList;
	class IListItem
	{
		friend class DuLinkList;
		friend class SgLinkList;
	public:
		IListItem(): m_last(0), m_next(0) {};

	private:
		IListItem* m_last;
		IListItem* m_next;
	};

	//Singly-linked list
	class SgLinkList
	{
	public:
		SgLinkList();
		virtual ~SgLinkList();
		void cleanUp();
		void InsertItem(IListItem*);
		//void removeItem(IListItem*);
		IListItem* pop();
		unsigned long size() {return m_size;};


	private:
		IListItem* m_listHead;
		IListItem* m_listTail;
		unsigned long			m_size;
	};

	//doubly linked list
	class DuLinkList
	{
	public:
		DuLinkList();
		virtual ~DuLinkList();
		void cleanUp();
		void InsertItem(IListItem*);
		void removeItem(IListItem*);
		IListItem* pop();
		unsigned long size() {return m_size;};


	private:
		IListItem* m_listHead;
		IListItem* m_listTail;
		unsigned long			m_size;
	};

	class IThreadPoolExecuteItem : public IListItem
	{
	public:
		virtual ~IThreadPoolExecuteItem() {};
		/**
		 * executeWorkItem
		 *
		* This method is used as a callback mechnism by the Thread Pool to notify
		* the user that it is the time to execute its asynchronous operation.
		 *
		 */
		virtual void executeWorkItem() = 0;
	};

	class IThreadPoolManager
	{
	public:
		virtual ~IThreadPoolManager(){};

		/**
		*
		* workerFree
		*
		* This is called by a worker when it becomes free
		*
		*/
		virtual void workerFree(IListItem*) = 0;
	};

	class ILFThreadPoolManager : public IThreadPoolManager
	{
	public:
		virtual ~ILFThreadPoolManager(){};
		/**
		*
		* leaderExecuteItem
		*
		* leader do leader's job.
		*
		*/
		virtual IThreadPoolExecuteItem* leaderExecuteItem() = 0;

		virtual void start() = 0;
	};

	class WorkItemExeSync
	{
	public:
		WorkItemExeSync();
		virtual ~WorkItemExeSync();
		void itemBegin();
		void itemEnd();
		void wait();
		int timedWait(unsigned long milliSeconds);

	private:
		int							m_itemCount;
		ReEntrantThreadLockable		m_lock;
		Condition					m_condition;
	};
};

#endif

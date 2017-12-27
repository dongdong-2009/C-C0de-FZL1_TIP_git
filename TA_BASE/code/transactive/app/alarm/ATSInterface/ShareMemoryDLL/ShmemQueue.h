// Queue.h: interface for the ShmemQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__148F4F04_97FB_467A_978C_0EED67C89F09__INCLUDED_)
#define AFX_QUEUE_H__148F4F04_97FB_467A_978C_0EED67C89F09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
#include "Shmem.h"
#include "ace\Unbounded_Queue.h"

#define MAX_SIZE 100

template<typename ITEM>
class ShmemQueue : public ACE_Unbounded_Queue<ITEM>
{
public:
	ShmemQueue(TA_Base_Core::ALLOCATOR *alloc = 0);
	virtual ~ShmemQueue();

private:
	ShmemQueue(const ShmemQueue<ITEM> &queue);
	ShmemQueue &operator=(const ShmemQueue<ITEM> &queue);
};

template <typename ITEM>
ShmemQueue<ITEM>::ShmemQueue(TA_Base_Core::ALLOCATOR *alloc /*= 0*/) : ACE_Unbounded_Queue<ITEM>(alloc)
{
}

template <typename ITEM>
ShmemQueue<ITEM>::~ShmemQueue()
{
}

#endif // !defined(AFX_QUEUE_H__148F4F04_97FB_467A_978C_0EED67C89F09__INCLUDED_)

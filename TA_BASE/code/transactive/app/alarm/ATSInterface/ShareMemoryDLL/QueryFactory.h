// QueryFactory.h: interface for the QueryFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYFACTORY_H__80748F1B_A9F8_4AA6_8FA8_9057D0CDA399__INCLUDED_)
#define AFX_QUERYFACTORY_H__80748F1B_A9F8_4AA6_8FA8_9057D0CDA399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShmemQueue.h"
#include <map>

template<typename ITEM>
class QueryFactory  
{
public:
	QueryFactory();
	virtual ~QueryFactory();

	static ShmemQueue<ITEM>* CreateQueue(TA_Base_Core::ALLOCATOR *pAlloc, const char *name);
};

template <typename ITEM>
QueryFactory<ITEM>::QueryFactory()
{
}

template <typename ITEM>
QueryFactory<ITEM>::~QueryFactory()
{
}

template <typename ITEM>
ShmemQueue<ITEM>* QueryFactory<ITEM>::CreateQueue(TA_Base_Core::ALLOCATOR *pAlloc, const char *name)
{
    void *ptr = TA_Base_Core::Shmem::alloc( sizeof( ShmemQueue<ITEM> ) );
	if (0 == ptr)
		return NULL;

	new(ptr)ShmemQueue<ITEM>(pAlloc);
	if (-1 == pAlloc->bind(name, ptr))
	{
		TA_Base_Core::Shmem::dealloc(ptr);
		return NULL;
	}

 	return (ShmemQueue<ITEM> *) ptr;
}

#endif // !defined(AFX_QUERYFACTORY_H__80748F1B_A9F8_4AA6_8FA8_9057D0CDA399__INCLUDED_)

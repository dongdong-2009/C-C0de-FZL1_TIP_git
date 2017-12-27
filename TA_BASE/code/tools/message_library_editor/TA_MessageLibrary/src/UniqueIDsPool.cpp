// UniqueIDsBank.cpp: implementation of the UniqueIDsBank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ta_messagelibrary.h"
#include "UniqueIDsPool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UniqueIDsPool::UniqueIDsPool(unsigned long size) : m_size(size), m_nextAvailableID(1)
{
    m_ids = new bool[m_size];

    returnAllIDs();
}

UniqueIDsPool::~UniqueIDsPool()
{
    delete [] m_ids;
}

unsigned long UniqueIDsPool::borrowID(void) // borrow the next available ID
{
    return getNextAvailableID();
}

bool UniqueIDsPool::borrowID(unsigned long i) // borrow a specific id (returns false if unavailable)
{
    ASSERT(i >= 0 && i < m_size);
    if (! (i >= 0 && i < m_size))
    {
        return false;
    }

    if (true == m_ids[i])
    {
        m_ids[i] = false;
        return true;
    }
    else
    {
        return false;
    }
}

unsigned long UniqueIDsPool::getNextAvailableID()
{
    while (m_nextAvailableID < m_size
        && m_ids[m_nextAvailableID] != true)
    {
        m_nextAvailableID++;
    }

    if (m_nextAvailableID < m_size)
    {
        m_ids[m_nextAvailableID] = false;
        return m_nextAvailableID;
    }
    else
    {
        return -1; // no IDs left.
    }
}

void UniqueIDsPool::returnID(unsigned long id)
{
    ASSERT(id < m_size);
    ASSERT(id >= 0);
    ASSERT(false == m_ids[id]); // It is a programming error to return an id
                                // that wasn't used in the first place
    m_ids[id] = true;

    if (id < m_nextAvailableID)
    {
        m_nextAvailableID = id;
    }
}

void UniqueIDsPool::returnAllIDs(void)
{
    for (unsigned long i = 0; i < m_size; i++)
    {
        m_ids[i] = true;
    }

    m_nextAvailableID = 1;
}
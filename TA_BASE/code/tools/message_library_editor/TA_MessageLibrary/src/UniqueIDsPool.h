// UniqueIDsPool.h: interface for the UniqueIDsPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIQUEIDSPOOL_H__9A6598F4_91B9_4482_BAF5_5C45EC86052C__INCLUDED_)
#define AFX_UNIQUEIDSPOOL_H__9A6598F4_91B9_4482_BAF5_5C45EC86052C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UniqueIDsPool  
{
public:
	UniqueIDsPool(unsigned long size);
	virtual ~UniqueIDsPool();

    unsigned long borrowID(void); // borrow the next available ID from the pool
    bool borrowID(unsigned long id); // borrow a specific id (returns false if unavailable)
    void returnID(unsigned long id); // return an id to the pool
    void returnAllIDs(void);

private:
    bool *m_ids;
    unsigned long m_size;
    unsigned long m_nextAvailableID;

    unsigned long getNextAvailableID(void);
};

#endif // !defined(AFX_UNIQUEIDSPOOL_H__9A6598F4_91B9_4482_BAF5_5C45EC86052C__INCLUDED_)

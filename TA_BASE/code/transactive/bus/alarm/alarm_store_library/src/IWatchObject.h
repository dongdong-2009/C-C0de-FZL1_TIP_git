// IWatchObject.h: interface for the IWatchObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IWATCHOBJECT_H__1B5BB6BC_FC85_447C_A1FC_1D6FBB6214FA__INCLUDED_)
#define AFX_IWATCHOBJECT_H__1B5BB6BC_FC85_447C_A1FC_1D6FBB6214FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Bus
{


/**
 * IWatchedObject
 *
 * This is Interface for watching
 *
 * Class need implement check() method, this interface used by CheckWorker
 */
class IWatchedObject  
{
public:
	IWatchedObject();
	virtual ~IWatchedObject();

	virtual bool check() = 0;
};

class CheckWorker: public TA_Base_Core::Thread
{
public:
	CheckWorker(IWatchedObject* watchedObject);
	virtual ~CheckWorker();

	virtual void run();

	virtual void terminate();

	unsigned long setCheckGap(unsigned long);

private:
	IWatchedObject*		m_pWatchedObject;	
	unsigned long		m_ulCheckGap;		// check time gap, milliseconds
	bool				m_bTerminate;
	TA_Base_Core::Condition	m_CondTerminate;
};

}

#endif // !defined(AFX_IWATCHOBJECT_H__1B5BB6BC_FC85_447C_A1FC_1D6FBB6214FA__INCLUDED_)

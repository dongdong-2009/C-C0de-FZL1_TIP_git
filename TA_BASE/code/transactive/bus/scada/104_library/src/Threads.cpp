
#include <queue>
#include <list>
#include "string.h"
#include "Threads.h"
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
using namespace std;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

namespace esca
{

//////////////////////////////////////////////////////////////////////
// CMutex
//////////////////////////////////////////////////////////////////////

#ifndef WIN32
pthread_mutexattr_t CMutex::m_normal;
pthread_mutexattr_t CMutex::m_recursive;
#endif

CMutex *CMutex::c_interlockMutex = 0;

CMutex::CMutex(bool recursive)
{
#ifdef WIN32

#ifdef _DEBUG
	m_inUse = false;
#endif

	::InitializeCriticalSection(&m_mutex);
#else
	int status;

	status = pthread_mutex_init(&m_mutex,recursive ? &m_recursive : &m_normal);

	if (status != 0)
	{
		//std::cerr << "Unable to initialize mutex in CMutex::CMutex()" << endl;
		//printf("status= %d ebusy=%d,einval= %d,efault=%d",status,EBUSY,EINVAL,EFAULT);
		//OsalAbort(OSAL_F_SEESTDERR);
	}
#endif
}

void CMutex::Init()
{
#ifndef WIN32
	int status;

	status = pthread_mutexattr_init(&m_normal);
	assert(status == 0);
	status = pthread_mutexattr_init(&m_recursive);
	assert(status == 0);

	status = pthread_mutexattr_settype(&m_normal,PTHREAD_MUTEX_NORMAL);
	assert(status == 0);
	status = pthread_mutexattr_settype(&m_recursive,PTHREAD_MUTEX_RECURSIVE);
	assert(status == 0);
#endif

	CDualMsgQThread::Init();
	c_interlockMutex = new CMutex(false);
}

void CMutex::CleanUp()
{
	CDualMsgQThread::Cleanup();
	delete c_interlockMutex;

#ifndef WIN32
	pthread_mutexattr_destroy(&m_normal);
	pthread_mutexattr_destroy(&m_recursive);
#endif
}

//////////////////////////////////////////////////////////////////////
// CMutex2
//////////////////////////////////////////////////////////////////////

CMutex2::CMutex2()
{
#ifndef WIN32
	pthread_cond_init(&m_cond,0);
#endif
}

CMutex2::~CMutex2()
{
#ifndef WIN32
	pthread_cond_destroy(&m_cond);
#endif
}

#ifndef WIN32
void CMutex2::Lock(bool &val)
{
	m_mutex.Lock();

	while (true)
		if (!val)
		{
			val = true;
			m_mutex.Unlock();
			break;
		}
		else		
			pthread_cond_wait(&m_cond,&m_mutex.m_mutex);
}

void CMutex2::Unlock(bool &val)
{
	CLock lock(&m_mutex);

	val = false;
	pthread_cond_broadcast(&m_cond);
}
#endif

//////////////////////////////////////////////////////////////////////
// CEvent
//////////////////////////////////////////////////////////////////////

CEvent::CEvent(bool signaled,bool broadcast)
#ifndef WIN32
	: m_eventmutex(false)
#endif
{
#ifdef WIN32
	m_event = ::CreateEvent(0,broadcast,signaled,0);
#else
	m_isSignaled = false;
	m_broadcast = broadcast;
	pthread_cond_init(&m_event,0);

	if (signaled)
		Signal();
#endif
}

CEvent::~CEvent()
{
#ifdef WIN32
	::CloseHandle(m_event);
#else
	pthread_cond_destroy(&m_event);
#endif
}

void CEvent::Signal()
{
#ifdef WIN32
	::SetEvent(m_event);
#else
	CLock lock(&m_eventmutex);

	if (!m_isSignaled)
	{
		m_isSignaled = true;

		if (m_broadcast)
			pthread_cond_broadcast(&m_event);
		else
			pthread_cond_signal(&m_event);
	}
#endif
}

void CEvent::Unsignal()
{
#ifdef WIN32
	::ResetEvent(m_event);
#else
	CLock lock(&m_eventmutex);
	m_isSignaled = false;
#endif
}

void CEvent::Wait()
{
#ifdef WIN32
	::WaitForSingleObject(m_event,INFINITE);
#else
	m_eventmutex.Lock();

	while (true)
		if (m_isSignaled)
		{			
			// Auto reset events are un-signaled as soon as some thread gets through.
			if (!m_broadcast)
				m_isSignaled = false;
			
			m_eventmutex.Unlock();
			break;
		}
		else		
			pthread_cond_wait(&m_event,&m_eventmutex.m_mutex);
#endif
}

//////////////////////////////////////////////////////////////////////
// CSemaphore
//////////////////////////////////////////////////////////////////////

CSemaphore::CSemaphore()
#ifndef WIN32
	: m_semmutex(false)
#endif
{
#ifdef WIN32
	m_sem = ::CreateSemaphore(0,0,LONG_MAX,0);
#else
	m_cnt = 0;
	pthread_cond_init(&m_sem,0);
#endif
}

CSemaphore::~CSemaphore()
{
#ifdef WIN32
	::CloseHandle(m_sem);
#else
	pthread_cond_destroy(&m_sem);
#endif
}

void CSemaphore::Incr()
{
#ifdef WIN32
	::ReleaseSemaphore(m_sem,1,0);
#else
	CLock lock(&m_semmutex);
	m_cnt++;
	pthread_cond_signal(&m_sem);
#endif
}

void CSemaphore::Decr()
{
#ifdef WIN32
	::WaitForSingleObject(m_sem,INFINITE);
#else
	m_semmutex.Lock();

	while (true)
		if (m_cnt)
		{
			m_cnt--;
			m_semmutex.Unlock();
			break;
		}
		else		
			pthread_cond_wait(&m_sem,&m_semmutex.m_mutex);
#endif
}

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

CThread::CThread() : m_hThread(0)
{
}

CThread::~CThread()
{	
	if (m_hThread)
	{
	#ifdef WIN32
		::CloseHandle(m_hThread);
	#else
		pthread_detach(m_hThread);
	#endif
		m_hThread = 0;
	}
}

void CThread::Start(int stackSize)
{
#ifdef WIN32
	m_hThread = ::CreateThread(0,0,ThreadProc,this,0,&m_ThreadId);
	//printf("Query thread %d start",m_ThreadId);
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (stackSize)
		pthread_attr_setstacksize(&attr,max(PTHREAD_STACK_MIN,stackSize));

	pthread_create(&m_hThread,NULL,ThreadProc,this);
//	pthread_attr_destroy(&attr);
#endif
}

void CThread::Kill()
{
	if (m_hThread)
	{
	#ifdef WIN32
		::TerminateThread(m_hThread,0);
		::CloseHandle(m_hThread);
	#else
		pthread_cancel(m_hThread);
		pthread_detach(m_hThread);
	#endif
	
		m_hThread = 0;
	}
}

void CThread::Wait()
{
	if (m_hThread)
	{
	#ifdef WIN32
		::WaitForSingleObject(m_hThread,INFINITE);
	#else
		pthread_join(m_hThread,0);
	#endif
	}
}

#ifdef WIN32

DWORD WINAPI CThread::ThreadProc(LPVOID thread)
{
	return reinterpret_cast<CThread*>(thread)->Main();
}

#else

void *CThread::ThreadProc(void *p)
{
	CThread *threadp = (CThread*)p;
	return (void*)threadp->Main();
}

#endif


CLogMessage::CLogMessage(b_ui32 len,char *data)
:m_len(len),
m_data(NULL)
{
	if(m_len>0)
	{
		m_data = new char[m_len];
		memcpy(m_data,data,m_len);
	}
	setCurTime();
}
CLogMessage::CLogMessage(const CLogMessage &msg)
{
	m_len = msg.m_len;
	m_data = new char[m_len];
	memcpy(m_data,msg.m_data,m_len);
	this->m_year = msg.m_year;
	this->m_mon = msg.m_mon;
	this->m_day  = msg.m_day;
	this->m_hour = msg.m_hour;
	this->m_min = msg.m_min;
	this->m_sec = msg.m_sec;
	this->m_msec = msg.m_msec;
		
}
CLogMessage& CLogMessage::operator=(const CLogMessage &msg)
{
	m_len = msg.m_len;
	char *tmp = m_data;
	m_data = new char[m_len];
	memcpy(m_data,msg.m_data,m_len);
	if(tmp!=NULL)
		delete[] tmp;
	this->m_year = msg.m_year;
	this->m_mon = msg.m_mon;
	this->m_day  = msg.m_day;
	this->m_hour = msg.m_hour;
	this->m_min = msg.m_min;
	this->m_sec = msg.m_sec;
	this->m_msec = msg.m_msec;
	return *this;
}
void CLogMessage::setCurTime()
{
#ifdef WIN32
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	this->m_year = systime.wYear-208;
	this->m_mon = (b_ui8)systime.wMonth ;
	this->m_day  = (b_ui8)systime.wDay ;
	this->m_hour = (b_ui8)systime.wHour ;
	this->m_min = (b_ui8)systime.wMinute;
	this->m_sec = (b_ui8)systime.wSecond ;
	this->m_msec = systime.wMilliseconds;
#else
	time_t ltime;
	time( &ltime );
	struct tm *tcurrent;
	if(tcurrent = localtime(&ltime))
	{
		this->m_year = tcurrent->tm_year-100;
		this->m_mon = tcurrent->tm_mon+1;
		this->m_day  = tcurrent->tm_mday;
		this->m_hour = tcurrent->tm_hour;
		this->m_min = tcurrent->tm_min;
		this->m_sec = tcurrent->tm_sec;
		this->m_msec = 0;
	}
#endif
		
}
CLogMessage::~CLogMessage()
{
	if(m_data)
		delete[] m_data;
}

/*
*------------------------------------------------------------
*/
CLogQThread ::CLogQThread () : m_messageMutex(false)
{
}

CLogQThread ::~CLogQThread ()
{

}

void CLogQThread ::PostMessage(b_ui32 id,void *data)
{
	CLock lock(&m_messageMutex);
	m_messages.push(CLogMessage(id,(char*)data));
	m_messageSem.Incr();
}


void CLogQThread ::GetMessage(CLogMessage &msg)
{
	m_messageSem.Decr();
	CLock lock(&m_messageMutex);
	msg = m_messages.front();	
	m_messages.pop();
}
//////////////////////////////////////////////////////////////////////
// CMsgQThread
//////////////////////////////////////////////////////////////////////

CMsgQThread::CMsgQThread() : m_messageMutex(false)
{
}

CMsgQThread::~CMsgQThread()
{
	/**
	while (!m_messages.empty())
	{
		CMessage msg = m_messages.front();
		m_messages.pop();
		delete msg.m_data;
	}
	**/
}

void CMsgQThread::PostMessage(b_ui32 id,void *data)
{
	CLock lock(&m_messageMutex);
	m_messages.push(CMessage(id,data));
	m_messageSem.Incr();
}

void CMsgQThread::GetMessage(b_ui32 &id,void *&data)
{
	m_messageSem.Decr();

	CLock lock(&m_messageMutex);
	CMessage &msg = m_messages.front();
	id = msg.m_id;
	data = msg.m_data;
	m_messages.pop();
}


//////////////////////////////////////////////////////////////////////
// CDualMsgQThread
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
	CSemaphore CDualMsgQThread::m_globalMessageSem;
#else
	pthread_cond_t CDualMsgQThread::m_cond;
	CMutex *CDualMsgQThread::m_condMutex;
	int CDualMsgQThread::m_globalCnt = 0;
#endif

CMutex *CDualMsgQThread::m_globalMessageMutex;
std::queue<CMessage> CDualMsgQThread::m_globalMessages;

CDualMsgQThread::CDualMsgQThread() : m_localMessageMutex(false)
{
#ifdef WIN32
	m_semHandles[0] = m_localMessageSem;
	m_semHandles[1] = m_globalMessageSem;
#else
	m_localCnt = 0;
#endif
}

CDualMsgQThread::~CDualMsgQThread()
{
}

void CDualMsgQThread::Init()
{
	m_globalMessageMutex = new CMutex(false);

#ifndef WIN32
	m_condMutex = new CMutex(false);
	pthread_cond_init(&m_cond,0);
#endif
}

void CDualMsgQThread::Cleanup()
{
	delete m_globalMessageMutex;

#ifndef WIN32
	delete m_condMutex;
#endif
}

void CDualMsgQThread::PostLocalMessage(b_ui32 id,void *data)
{
	{
		// Added scope here to release the mutex immediately
		CLock lock(&m_localMessageMutex);
		m_localMessages.push(CMessage(id,data));
	}

#ifdef WIN32
	m_localMessageSem.Incr();
#else
	CLock lock(m_condMutex);
	m_localCnt++;
	// We have to broadcast to make sure that this thread is unblocked to read from its local
	// queue. If we merely signaled the condition variable, this may not be the single thread
	// that is unblocked.
	pthread_cond_broadcast(&m_cond);
#endif
}

void CDualMsgQThread::PostGlobalMessage(b_ui32 id,void *data)
{
	{
		// Added scope here to release the mutex immediately
		CLock lock(m_globalMessageMutex);
		m_globalMessages.push(CMessage(id,data));
	}

#ifdef WIN32
	m_globalMessageSem.Incr();
#else
	CLock lock(m_condMutex);
	m_globalCnt++;
	pthread_cond_signal(&m_cond);
#endif
}

void CDualMsgQThread::GetMessage(b_ui32 &id,void *&data)
{
#ifdef WIN32
	DWORD h = ::WaitForMultipleObjects(sizeof m_semHandles / sizeof m_semHandles[0],
		m_semHandles,FALSE,INFINITE);

	switch (h - WAIT_OBJECT_0)
	{
	case 0:
		GetLocalMessage(id,data);
		break;
	case 1:
		GetGlobalMessage(id,data);
		break;
	}
#else
	m_condMutex->Lock();

	while (true)
		if (m_localCnt)
		{
			m_localCnt--;
			m_condMutex->Unlock();
			GetLocalMessage(id,data);
			break;
		}
		else if (m_globalCnt)
		{
			m_globalCnt--;
			m_condMutex->Unlock();
			GetGlobalMessage(id,data);
			break;
		}
		else		
			pthread_cond_wait(&m_cond,&m_condMutex->m_mutex);
#endif
}

void CDualMsgQThread::GetLocalMessage(b_ui32 &id,void *&data)
{
	CLock lock(&m_localMessageMutex);
	CMessage msg = m_localMessages.front();
	id = msg.m_id;
	data = msg.m_data;
	m_localMessages.pop();
}

void CDualMsgQThread::GetGlobalMessage(b_ui32 &id,void *&data)
{
	CLock lock(m_globalMessageMutex);
	CMessage msg = m_globalMessages.front();
	id = msg.m_id;
	data = msg.m_data;
	m_globalMessages.pop();
}

}


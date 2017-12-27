
#if !defined(WEBFGTHREADS_H)
#define WEBFGTHREADS_H



#include <queue>
#include <string>
#include <time.h>
#ifdef WIN32
 #include <windows.h>
#endif

#include "Btype.h"
namespace esca
{

//////////////////////////////////////////////////////////////////////
// CMutex
//////////////////////////////////////////////////////////////////////

class CMutex
{
public:
	CMutex(bool = true);
	~CMutex();
	void Lock();
	void Unlock();
	static void Init();
	static void CleanUp();

#ifdef WIN32
	CRITICAL_SECTION m_mutex;

#ifdef _DEBUG
	bool m_inUse;
#endif

#else
	pthread_mutex_t m_mutex;
	static pthread_mutexattr_t m_normal;
	static pthread_mutexattr_t m_recursive;
#endif
	static CMutex *c_interlockMutex;
};

#if defined(WIN32) && defined(_DEBUG)
	#define ASSERT_MUTEX_LOCKED(m) \
		if (!(m)->m_inUse)	\
		{	\
			cerr << "Thread has not acquired this mutex!" << endl;	\
			OsalAbort(OSAL_F_SEESTDERR);	\
		}
#else
	#define ASSERT_MUTEX_LOCKED(m)
#endif

inline CMutex::~CMutex()
{
#ifdef WIN32
	::DeleteCriticalSection(&m_mutex);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}

inline void CMutex::Lock()
{
#ifdef WIN32
	::EnterCriticalSection(&m_mutex);

#ifdef _DEBUG
	if (m_inUse)
	{
	}

	m_inUse = true;
#endif

#else
	pthread_mutex_lock(&m_mutex);
#endif
}

inline void CMutex::Unlock()
{
#ifdef WIN32

#ifdef _DEBUG
	m_inUse = false;
#endif

	::LeaveCriticalSection(&m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}

//////////////////////////////////////////////////////////////////////
// CMutex2
//////////////////////////////////////////////////////////////////////

class CMutex2
{
public:
	CMutex2();
	~CMutex2();
	void Lock(bool&);
	void Unlock(bool&);

	CMutex m_mutex;
#ifndef WIN32
	pthread_cond_t m_cond;
#endif
};

#ifdef WIN32
inline void CMutex2::Lock(bool&)
{
	m_mutex.Lock();
}

inline void CMutex2::Unlock(bool&)
{
	m_mutex.Unlock();
}
#endif

//////////////////////////////////////////////////////////////////////
// CLock
//////////////////////////////////////////////////////////////////////

class CLock
{
public:

	CLock(CMutex *mutex)
	{
#if defined( VMS ) || defined ( __VMS )
		m_oldASTStatus = sys$setast( 0 );
#endif
		m_mutex = mutex;
		m_mutex->Lock();
	}

	~CLock()
	{
		if (m_mutex)
			m_mutex->Unlock();
#if defined( VMS ) || defined ( __VMS )
		if ( m_oldASTStatus == SS$_WASSET )
			sys$setast( 1 );
#endif
	}

	void Unlock()
	{
		m_mutex->Unlock();
		m_mutex = 0;
#if defined( VMS ) || defined ( __VMS )
		if ( m_oldASTStatus == SS$_WASSET )
			sys$setast( 1 );
#endif
	}

private:
	CMutex *m_mutex;
#if defined( VMS ) || defined ( __VMS )
    int m_oldASTStatus;
#endif
};

//////////////////////////////////////////////////////////////////////
// CLock2
//////////////////////////////////////////////////////////////////////

class CLock2
{
public:

	CLock2(CMutex2 *mutex,bool &val) : m_mutex(mutex),m_val(val)
	{
		m_mutex->Lock(m_val);
	}

	~CLock2()
	{
		m_mutex->Unlock(m_val);
	}

private:

	CMutex2 *m_mutex;
	bool &m_val;
};

//////////////////////////////////////////////////////////////////////
// Interlocked
//////////////////////////////////////////////////////////////////////

inline b_i64 InterlockedIncrement(b_i64* lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return ++(*lpAddend);
}

inline b_i64 InterlockedDecrement(b_i64* lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return --(*lpAddend);
}

inline void InterlockedSet(b_i64* lpVal,b_i64 newVal)
{
	CLock lock(CMutex::c_interlockMutex);
	*lpVal = newVal;
}

inline b_i64 InterlockedGet(b_i64* lpVal)
{
	CLock lock(CMutex::c_interlockMutex);
	return *lpVal;
}

inline b_i32 InterlockedIncrement(b_i32 * lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return ++(*lpAddend);
}

inline b_i32 InterlockedDecrement(b_i32 * lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return --(*lpAddend);
}

inline void InterlockedSet(b_i32 * lpVal,b_i32 newVal)
{
	CLock lock(CMutex::c_interlockMutex);
	*lpVal = newVal;
}

inline b_i32 InterlockedGet(b_i32 * lpVal)
{
	CLock lock(CMutex::c_interlockMutex);
	return *lpVal;
}

inline b_ui64 InterlockedIncrement(b_ui64 *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return ++(*lpAddend);
}

inline b_ui64 InterlockedDecrement(b_ui64 *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return --(*lpAddend);
}

inline void InterlockedSet(b_ui64 *lpVal,b_ui64 newVal)
{
	CLock lock(CMutex::c_interlockMutex);
	*lpVal = newVal;
}

inline b_ui64 InterlockedGet(b_ui64 *lpVal)
{
	CLock lock(CMutex::c_interlockMutex);
	return *lpVal;
}

inline b_ui16* InterlockedIncrement(b_ui16* *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return ++(*lpAddend);
}

inline b_ui16* InterlockedDecrement(b_ui16* *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return --(*lpAddend);
}

inline void InterlockedSet(b_ui16* *lpVal,b_ui16* newVal)
{
	CLock lock(CMutex::c_interlockMutex);
	*lpVal = newVal;
}

inline b_ui16* InterlockedGet(b_ui16* *lpVal)
{
	CLock lock(CMutex::c_interlockMutex);
	return *lpVal;
}
inline b_i16 InterlockedIncrement(b_i16 *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return ++(*lpAddend);
}

inline b_i16 InterlockedDecrement(b_i16 *lpAddend)
{
	CLock lock(CMutex::c_interlockMutex);
	return --(*lpAddend);
}

inline void InterlockedSet(b_i16 *lpVal,b_i16 newVal)
{
	CLock lock(CMutex::c_interlockMutex);
	*lpVal = newVal;
}

inline b_i16 InterlockedGet(b_i16 *lpVal)
{
	CLock lock(CMutex::c_interlockMutex);
	return *lpVal;
}


//////////////////////////////////////////////////////////////////////
// CEvent
//////////////////////////////////////////////////////////////////////

class CEvent
{
public:
	CEvent(bool,bool);
	~CEvent();
	void Signal();
	void Unsignal();
	void Wait();

#ifdef WIN32
	operator HANDLE() const { return m_event; }
#endif

private:
#ifdef WIN32
	HANDLE m_event;
#else
	pthread_cond_t m_event;
	CMutex m_eventmutex;
	bool m_isSignaled;
	bool m_broadcast;
#endif
};

//////////////////////////////////////////////////////////////////////
// CSemaphore
//////////////////////////////////////////////////////////////////////

class CSemaphore
{
public:
	CSemaphore();
	~CSemaphore();
	void Incr();
	void Decr();

#ifdef WIN32
	operator HANDLE() const { return m_sem; }
#endif

private:
#ifdef WIN32
	HANDLE m_sem;
#else
	pthread_cond_t m_sem;
	CMutex m_semmutex;
	int m_cnt;
#endif
};

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

class CThread
{
public:
	CThread();
	virtual ~CThread();
#ifdef WIN32
   virtual DWORD Main() = 0;
#else
   virtual b_ui64 Main() = 0;
#endif
	
	void Start(int = 0);
	void Kill();
	void Wait();

protected:

#ifdef WIN32
	static DWORD WINAPI ThreadProc(LPVOID);
	HANDLE m_hThread;
	DWORD m_ThreadId;
#else
	static void *ThreadProc(void*);
	pthread_t m_hThread;
#endif
};

//////////////////////////////////////////////////////////////////////
// CMsgQThread
//////////////////////////////////////////////////////////////////////

struct CMessage
{
	CMessage(b_ui32 id,void *data) : m_id(id),m_data(data) {}
	b_ui32 m_id;
	void *m_data;
};

class CMsgQThread : public CThread
{
public:
	CMsgQThread();
	virtual ~CMsgQThread();
	void PostMessage(b_ui32,void*);

protected:
	void GetMessage(b_ui32&,void*&);

	CSemaphore m_messageSem;
	CMutex m_messageMutex;
	std::queue<CMessage> m_messages;
};
////////////////////////////////////////////////////////////////////////
//CLogMessage
////////////////////////////////////
class CLogMessage
{
public:
	CLogMessage(b_ui32 len,char *data);
	CLogMessage(const CLogMessage &msg);
	CLogMessage& operator=(const CLogMessage &msg);
	void setCurTime();
	~CLogMessage();

	b_ui32 m_len;
	char *m_data;
	b_ui8 m_year;
	b_ui8 m_mon;
	b_ui8 m_day;
	b_ui8 m_hour;
	b_ui8 m_min;
	b_ui8 m_sec;
	b_ui16 m_msec;
};


//////////////////////////////////////////////////////////////////////
// CLogQThread
//////////////////////////////////////////////////////////////////////
class CLogQThread : public CThread
{
public:
	CLogQThread();
	virtual ~CLogQThread();
	void PostMessage(b_ui32,void*);

protected:
	void GetMessage(CLogMessage &msg);

	CSemaphore m_messageSem;
	CMutex m_messageMutex;
	std::queue<CLogMessage> m_messages;
};


//////////////////////////////////////////////////////////////////////
// CDualMsgQThread
//////////////////////////////////////////////////////////////////////

class CDualMsgQThread : public CThread
{
public:
	CDualMsgQThread();
	virtual ~CDualMsgQThread();
	void PostLocalMessage(b_ui32,void*);
	static void PostGlobalMessage(b_ui32,void*);
	static void Init();
	static void Cleanup();

protected:
	void GetMessage(b_ui32&,void*&);
	void GetLocalMessage(b_ui32&,void*&);
	void GetGlobalMessage(b_ui32&,void*&);
    
	CMutex m_localMessageMutex;
	std::queue<CMessage> m_localMessages;
	static CMutex *m_globalMessageMutex;
	static std::queue<CMessage> m_globalMessages;
	

#ifdef WIN32
	CSemaphore m_localMessageSem;
	static CSemaphore m_globalMessageSem;
	HANDLE m_semHandles[2];
#else
	static pthread_cond_t m_cond;
	static CMutex *m_condMutex;
	int m_localCnt;
	static int m_globalCnt;
#endif
};

//////////////////////////////////////////////////////////////////////
// Global Functions
//////////////////////////////////////////////////////////////////////

b_i32 InterlockedIncrement(b_i32 *);
b_i32 InterlockedDecrement(b_i32 *);
b_i64 InterlockedIncrement(b_i64*);
b_i64 InterlockedDecrement(b_i64*);
extern CMutex *c_interlockMutex;

}

#endif

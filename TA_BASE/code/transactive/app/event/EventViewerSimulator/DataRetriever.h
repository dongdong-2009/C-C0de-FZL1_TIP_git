#pragma once

#include "core/event/src/EventItem.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>
#include <string>

typedef std::vector< TA_Base_Core::EventItem* > T_VecEvents;

class CDataRetriever
	: public TA_Base_Core::Thread
{
public: // functions
	CDataRetriever(int nQueryLocation, unsigned long _ulQueryInterval, bool _bIsLogNeeded = false);
	~CDataRetriever(void);

	virtual void run();
	virtual void terminate();

	bool getDataForDisplay(T_VecEvents& vecDataList);

private: // functions
	void _insertToCache(TA_Base_Core::T_EventItemList& pEventSeq);
	void _releaseCacheData();
	//TA_Base_Core::EventItem* _eventDataToCorbaDef(TA_Base_Core::EventSerializeCorbaDef& evDetailCorba);
	std::string decompressData(const char* data);
	std::string _formatTime(long ulTime);
private: //Variables

	TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
	bool m_bTerminate;
	bool m_bNeedReload;
	std::string m_curEventID;
	T_VecEvents m_cacheEventList;
	unsigned long m_ulQueryInterval;
	bool m_bIsLogNeeded;
};

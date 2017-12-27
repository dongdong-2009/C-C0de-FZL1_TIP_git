#include "StdAfx.h"
#include "DataRetriever.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/event/src/EventHelper.h"
#include "core/event/src/EventHelperException.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include "boost/iostreams/filtering_stream.hpp"
#include <boost/iostreams/device/back_inserter.hpp>
//BOOST_CLASS_EXPORT(TA_Base_App::EventData);


using namespace TA_Base_Core;

CDataRetriever::CDataRetriever(int nQueryLocation, unsigned long _ulQueryInterval, bool _bIsLogNeeded/* = false*/)
		:m_ulQueryInterval(_ulQueryInterval) 
		,m_bTerminate(false)
		,m_bIsLogNeeded(_bIsLogNeeded)
{
	TA_Base_Core::EventHelper::getInstance().init(nQueryLocation);
}

CDataRetriever::~CDataRetriever(void)
{
	_releaseCacheData();
}

void CDataRetriever::run()
{
	FUNCTION_ENTRY("run");
	bool bNeedReload = true;

	while(!m_bTerminate)
	{
		bool bFailure = false;
		try
		{
			if (!bNeedReload)
			{
				std::string strEventID = m_curEventID;
				TA_Base_Core::T_EventItemList eventSeq;
				TA_Base_Core::EventHelper::getInstance().getLatestEvents(
					strEventID.c_str(),
					"Super Session ID",
					eventSeq
					);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve Events from agent size = %d", eventSeq.size());

				_insertToCache(eventSeq);					
			}
			else if (bNeedReload)
			{
				TA_Base_Core::T_EventItemList eventSeq;
				TA_Base_Core::EventHelper::getInstance().getAllEvents(
					"Super Session ID",
					eventSeq
					);
				
				// Delete all existing event data
				_releaseCacheData();

				m_bNeedReload = true;

				// Insert new cache events
				_insertToCache(eventSeq);		

				bNeedReload = false;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Can't locate or initialise Control Event Agent. Try again later.");
			}
		}
		catch(const TA_Base_Core::EventHelperException& ex )
		{
			TA_Base_Core::EventHelperException::ERROR_CODE errCode = ex.getErrorCode();
			if (errCode == TA_Base_Core::EventHelperException::ERR_ID_NOT_FOUND )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Agent out of sync!!! need to reload events from control agent.");
				bNeedReload = true;
			}
			continue;
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't refresh Event Cache or Queue from Control Event Agent. Caught General Exception" );
			bFailure = true;
		}

		if (bFailure)
			TA_Base_Core::Thread::sleep(5000); // Sleep additional 5 seconds before trying again

		TA_Base_Core::Thread::sleep(m_ulQueryInterval);
	}
	FUNCTION_EXIT;
}

void CDataRetriever::terminate()
{
	m_bTerminate = true;
}

bool CDataRetriever::getDataForDisplay(T_VecEvents& vecDataList)
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	bool bNeedReload = m_bNeedReload;

	
	T_VecEvents::iterator itr = m_cacheEventList.begin();
	vecDataList.insert(vecDataList.end(), itr, m_cacheEventList.end());

	m_cacheEventList.clear();

	m_bNeedReload = false;
	return bNeedReload;
}

void CDataRetriever::_insertToCache(TA_Base_Core::T_EventItemList& pEventSeq)
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
// 	SYSTEMTIME st, et;
// 	::GetLocalTime(&st); 

// 	m_cacheEventList.insert(m_cacheEventList.end(), pEventSeq.begin(), pEventSeq.end());
// 	TA_Base_Core::T_EventItemList::iterator itr = pEventSeq.end();
// 	itr--;
// 	m_curEventID = (*itr)->eventID;
	TA_Base_Core::T_EventItemList::iterator itr = pEventSeq.begin();
	for (itr; itr != pEventSeq.end(); itr++)
	{
		TA_Base_Core::EventItem* pDetail = *itr;
		m_curEventID = pDetail->eventID;
		m_cacheEventList.push_back(pDetail);

		if (m_bIsLogNeeded)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CreateTime: %s | MgsSeqNumber: %d-%d | EventID: %s | LocationKey: %d | Description: %s", _formatTime(pDetail->createTime.time).c_str(), pDetail->eventSequence.ulHighOrder, pDetail->eventSequence.ulLowOrder, pDetail->eventID, pDetail->locationKey, pDetail->description);
		}
	}

// 	::GetLocalTime(&et); 
// 	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Deserialize Time: ST: %d:%d:%d:%d || ET: %d:%d:%d:%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, et.wHour, et.wMinute, et.wSecond, et.wMilliseconds );

}

void CDataRetriever::_releaseCacheData()
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	T_VecEvents::iterator itr = m_cacheEventList.begin();

	for (itr; itr!=m_cacheEventList.end(); itr++)
	{
		delete (*itr);
	}

	m_cacheEventList.clear();
}

// TA_Base_Core::EventItem* CDataRetriever::_eventDataToCorbaDef(TA_Base_Core::EventSerializeCorbaDef& evDetailCorba)
// {
// 	FUNCTION_ENTRY("_eventDataToCorbaDef");
// 	TA_Base_Core::EventItem* refRetVal;
// 
// 	CORBA::Octet * datenLData = evDetailCorba.eventDetail.get_buffer();
// 	refRetVal = new TA_Base_Core::EventItem((char*)datenLData);
// 
// 	FUNCTION_EXIT;
// 	return refRetVal;
// }

std::string CDataRetriever::decompressData(const char* data)
{
	using namespace boost::iostreams;
	using namespace std;
	using namespace boost::archive;
	string output;
 	string inStream = data;
// 	//stringstream outStream;
// 	filtering_istream inFilterStream;
// 	boost::iostreams::stream<boost::iostreams::array_source> outSource;
// 
// 
 	//inStream << data;
//  	inFilterStream.push(zlib_decompressor());
//  	inFilterStream.push(inStream);
//  
// 	boost::iostreams::copy(inFilterStream, outSource);


	try
	{
		// Create decompressing stream writing to sink
		boost::iostreams::filtering_ostream decompressingStream;
		decompressingStream.push(zlib_decompressor());
		decompressingStream.push(boost::iostreams::back_inserter(output));

		decompressingStream << inStream;

		boost::iostreams::close(decompressingStream);
	}
	catch(boost::iostreams::zlib_error& error)
	{
 	   cerr << "Caught bzip2 error with code: " << error.error() << endl;
 	   cerr << "data_error: " << boost::iostreams::zlib::data_error << endl;
 	   
	   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Caught zlib error with code %d", error.error());
	   //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "data_error: ",  boost::iostreams::zlib::data_error);

	}

	return output.c_str();
}

std::string CDataRetriever::_formatTime(long ulTime)
{
	char szTime[256];
	CTime time(ulTime);
	CString str = time.Format("%d-%b-%y - %H:%M:%S");
	strncpy(szTime, str.GetBuffer(0), sizeof(szTime) - 1);
	return std::string(szTime);
}

// DataStoreReceiver.cpp: implementation of the DataStoreReceiver class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// namespace TA_IRS_Bus
// {
// 	DataStoreReceiver::DataStoreReceiver():m_isWorking(FALSE)
// 	{
// 		m_trainInfoManager=new TrainInfoManager(999, TrainInfoManager::TrainInformationTotalSize);
// 		m_lastScanTime.dwLowDateTime=0;
// 		m_lastScanTime.dwHighDateTime=0;
// 	}
// 	
// 	DataStoreReceiver::~DataStoreReceiver()
// 	{
// 		stopWork();
// 		if (m_trainInfoManager!=NULL)
// 		{
// 			delete m_trainInfoManager;
// 			m_trainInfoManager=NULL;
// 		}
// 	}
// 	void DataStoreReceiver::AddObserver(ICacheDataUpdateObserver* observer)
// 	{
// 		m_Observers.push_back(observer);
// 	}
// 	
// 	void DataStoreReceiver::RemoveObserver(ICacheDataUpdateObserver* observer)
// 	{
// 		std::vector<ICacheDataUpdateObserver*>::iterator pos = 
// 			std::find(m_Observers.begin(), m_Observers.end(), observer);
// 		if( pos!= m_Observers.end())
// 			m_Observers.erase(pos);
// 	}
// 	void DataStoreReceiver::run()
// 	{
// 		FUNCTION_ENTRY("DataStoreReceiver::run");
// 		const unsigned int SLEEP_TIME = 200;  
// 		while (m_isWorking)
// 		{
// 			try
// 			{
// 				PollData();
// 			}	 
// 			catch(...)
// 			{
// 				LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught an unknown exception");								
// 			}
// 			this->sleep(SLEEP_TIME);
// 		}
// 		FUNCTION_EXIT
// 	}
// 	
// 	void DataStoreReceiver::PollData()
// 	{		
//         SYSTEMTIME st;
// 		GetSystemTime(&st); 
// 		FILETIME   ft;
// 		SystemTimeToFileTime(&st, &ft); 
// 		ICacheDataList*  dlist=m_trainInfoManager->scanData(m_lastScanTime);
// 		m_lastScanTime=ft;	        
// 		
//         
// 		
// 		boost::shared_ptr<CacheDataWraper> datas (new CacheDataWraper);
//         datas->cacheDatas=dlist;
// 		std::vector<ICacheDataUpdateObserver*>::iterator iter;
// 		for (iter=m_Observers.begin();iter!=m_Observers.end();iter++)
// 		{
// 			(*iter)->processData(datas);
// 		}
// 	}
// 	
// 	void DataStoreReceiver::terminate()
// 	{
// 		FUNCTION_ENTRY("DataStoreReceiver::terminate");
// 		m_isWorking=false;		
// 		FUNCTION_EXIT;
// 	}
//     void DataStoreReceiver::startWork()
// 	{
// 		start();		
// 	}
// 	void DataStoreReceiver::stopWork()
// 	{
// 		std::vector<ICacheDataUpdateObserver*>::iterator iter;
// 		for (iter=m_Observers.begin();iter!=m_Observers.end();iter++)
// 		{
// 			if (*iter !=NULL)
// 			{
// 				delete *iter;
// 				*iter=NULL;
// 			}
// 		}
// 		m_Observers.clear();	
// 		terminateAndWait();
// 	}
// }

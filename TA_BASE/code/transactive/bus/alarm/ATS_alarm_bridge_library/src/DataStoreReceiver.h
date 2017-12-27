// DataStoreReceiver.h: interface for the DataStoreReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASTORERECEIVER_H__E391380D_2782_4123_8726_44EF4B18CADA__INCLUDED_)
#define AFX_DATASTORERECEIVER_H__E391380D_2782_4123_8726_44EF4B18CADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)


#include <string>
#include <vector>
#include <map>
#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

//#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "CacheDataManager.h"
#include "ATSAlarmInfoManager.h"

#include "ICacheDataUpdateObserver.h"
//#include "MemoryAllocReset.h"


 
using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	class DataStoreReceiver : public TA_Base_Core::Thread
	{
	protected:
		//virtual void updateMap(CacheDataT* cacheData)=0;	
	public:
			
		DataStoreReceiver();
		virtual ~DataStoreReceiver();	 
		//virtual  DataManager* getDataManager()=0;
		virtual void initialize();
	 
		virtual std::vector< boost::shared_ptr<CacheDataT> >* getData();
		virtual std::vector< boost::shared_ptr<CacheDataT> >* getDataWithoutLock();
	 
		virtual void RemoveObserver(ICacheDataUpdateObserver<CacheDataT>* observer);		 
		virtual void AddObserver(ICacheDataUpdateObserver<CacheDataT>* observer);		 
		virtual void run();	
		virtual TA_Base_Core::IProcessLockable* getlock()
		{
			if (m_cacheDataManager !=NULL )return m_cacheDataManager->getlock();
			return NULL;
		}
	 
		virtual void terminate()
		{
			FUNCTION_ENTRY("DataStoreReceiver::terminate");
			m_isWorking=false;		
			FUNCTION_EXIT;
		}

	   
	private:
         	
		DataManager* m_cacheDataManager;
	 
		bool m_isInitialized;
		 
		std::string m_receiverName;

		std::vector<ICacheDataUpdateObserver<CacheDataT>* > m_Observers;
		bool m_isWorking;				
	 
		TA_Base_Core::NonReEntrantThreadLockable m_ObserverLock;

		virtual void PullData();	

	};


	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::DataStoreReceiver()
	{
		m_isInitialized=FALSE;
		m_cacheDataManager=NULL;		
		m_receiverName=TA_Base_Core::gPtrToStr(this).c_str();
	}
	
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::~DataStoreReceiver()
	{
		 
	} 

	 template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 std::vector< boost::shared_ptr<CacheDataT> >*  DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::getData()
	 {
          std::vector< boost::shared_ptr<CacheDataT> >*  dlist;
		  dlist=NULL; 
		  dlist=getDataWithoutLock();	
		  return dlist;
	 } 

	 template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 std::vector< boost::shared_ptr<CacheDataT> >*  DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::getDataWithoutLock()
	 {
		 std::vector< boost::shared_ptr<CacheDataT> >*  dlist;
		 dlist=NULL; 
		 dlist=m_cacheDataManager->getDataWithoutLock();	
		 return dlist;
	 } 


	 template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		 void DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::run()
	 {
		 FUNCTION_ENTRY("DataStoreReceiver::run");
		 
		 LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"DataStoreReceiver::run(): %s begin to run", m_receiverName.c_str() );			
		 const unsigned int SLEEP_TIME = 200;  
		 while (m_isWorking)
		 {
			 DWORD beginclock;
			 beginclock=GetTickCount();
			 try
			 {
				 this->PullData();
			 }	 
			 catch (std::exception& e)
			 {
				 std::string msg( m_receiverName );				 
				 msg.append( " has std::exception:");
				 msg.append( e.what() );
				 LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
			 }    
			 catch(...)
			 {
				 LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "PullData Caught an unknown exception");								
			 }
			 DWORD endclock=GetTickCount();
			 unsigned int duration=SLEEP_TIME-(unsigned int)(endclock-beginclock);
			 if (duration>0 && duration<=SLEEP_TIME)
			 {
				 this->sleep(duration);
			 }
			 else
			 {
				 this->sleep(50);
			 }
			 
		 }
		 FUNCTION_EXIT
	 }
	 
	 template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 void DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::PullData()
	 { 		 
		 std::vector< boost::shared_ptr<CacheDataT> >*  dlist=NULL;
		 dlist=getData();		 			
		 if (dlist==NULL) 
		 {			 
			 return;
		 }
		 if (dlist->size()<=0)
		 {			 
			 delete dlist;
			 return;
		 }			 
		 boost::shared_ptr<CacheDataWraper<CacheDataT> > datas (new CacheDataWraper <CacheDataT>);		 
	 
		 datas->cacheDatas=dlist;
		 std::vector<ICacheDataUpdateObserver<CacheDataT> *>::iterator iter;
		 TA_Base_Core::ThreadGuard guard( m_ObserverLock);
		 for (iter=m_Observers.begin();iter!=m_Observers.end();iter++)
		 {
			 try
			 {
				 (*iter)->processData(datas);
			 }
			 catch(...)
			 {
				 LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "processData error");	
			 }
		 }
	 }

	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 void DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::initialize()
	{
		if(m_isInitialized)
		{
			return;
		}	
		//TA_Base_Core::ThreadGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}
		 
		 	
		if (m_cacheDataManager==NULL)
		{			 
			m_cacheDataManager= new DataManager(DataManager::MaxItemCount, DataManager::TotalSizeLength);
		}
		 
		m_isInitialized=TRUE;		 
		 
	}

	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		void DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::RemoveObserver(ICacheDataUpdateObserver<CacheDataT>* observer)
	{			
		TA_Base_Core::ThreadGuard guard( m_ObserverLock);
		std::vector<ICacheDataUpdateObserver<CacheDataT>* >::iterator pos = 
			std::find(m_Observers.begin(), m_Observers.end(), observer);
		if( pos!= m_Observers.end())
			m_Observers.erase(pos);			
	}
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		void DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::AddObserver(ICacheDataUpdateObserver<CacheDataT>* observer)
	{
		TA_Base_Core::ThreadGuard guard( m_ObserverLock);
		m_Observers.push_back(observer);		 
	}

	 
	 
}

#endif // !defined(AFX_DATASTORERECEIVER_H__E391380D_2782_4123_8726_44EF4B18CADA__INCLUDED_)

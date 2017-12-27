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
// #include "core/utilities/src/DebugUtil.h"
// #include "core/utilities/src/TA_String.h"
// #include "core/threads/src/Thread.h"
// #include "core/synchronisation/src/NonReEntrantThreadLockable.h"
// #include "core/synchronisation/src/ReadWriteThreadLockable.h"
// #include "core/synchronisation/src/ThreadGuard.h"
// #include "core/synchronisation/src/ThreadReadGuard.h"

//#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "CacheDataManager.h"
#include "ATSAlarmInfoManager.h"
 
//#include "MemoryAllocReset.h"


 
//using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	class DataStoreReceiver //: public TA_Base_Core::Thread
	{
	protected:
		//virtual void updateMap(CacheDataT* cacheData)=0;	
	public:
			
		DataStoreReceiver();
		virtual ~DataStoreReceiver();	 
		//virtual  DataManager* getDataManager()=0;
		virtual void initialize();
	 
		virtual std::vector< boost::shared_ptr<CacheDataT> >* getData();
	 
 
	   
	private:
         	
		DataManager* m_cacheDataManager;
	 
		bool m_isInitialized;
		 
		std::string m_receiverName;

	};


	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	 DataStoreReceiver<CacheDataKeyType,  CacheDataT,  DataManager>::DataStoreReceiver()
	{
		m_isInitialized=FALSE;
		m_cacheDataManager=NULL;		
		//m_receiverName=TA_Base_Core::gPtrToStr(this).c_str();
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
		  dlist=m_cacheDataManager->getData();	
		  return dlist;
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


	 
	 
}

#endif // !defined(AFX_DATASTORERECEIVER_H__E391380D_2782_4123_8726_44EF4B18CADA__INCLUDED_)

// DataStoreUpdater.h: interface for the DataStoreUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASTOREUPDATER_H__6A1D4794_0994_484B_BB3F_64A2330EB980__INCLUDED_)
#define AFX_DATASTOREUPDATER_H__6A1D4794_0994_484B_BB3F_64A2330EB980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>


 
// #include "core/synchronisation/src/ProcessGuard.h"
// #include "core/synchronisation/src/IProcessLockable.h"
// #include "core/synchronisation/src/NonReEntrantProcessLockable.h"

#include "boost/shared_ptr.hpp"
#include "CacheDataWraper.h"
#include "CacheDataManager.h"
 

namespace TA_Base_Bus
{

	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	class DataStoreUpdater   
	{
    private:
		std::string m_updaterName;
	public:
		DataStoreUpdater();
		virtual ~DataStoreUpdater();
		virtual void Initialize(std::vector< boost::shared_ptr<CacheDataT> >& batch);
		virtual bool onDispatchObjects(std::vector<boost::shared_ptr<CacheDataT> >&  batch);
		virtual bool clearAllData();
		 
	protected:
		DataManager* m_DataManager;
		bool m_isInitialized;
		 
		void interInitialize();
		
	 
	};

 
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::DataStoreUpdater()
	{ 
		m_isInitialized=FALSE;
		m_DataManager=NULL;
		//m_updaterName=TA_Base_Core::gPtrToStr(this).c_str();	 
	}

	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::~DataStoreUpdater()
	{
		if (m_DataManager!=NULL)
		{
			delete m_DataManager;
			m_DataManager=NULL;
			 
		}
	}

	 

	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		void DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::Initialize(std::vector< boost::shared_ptr<CacheDataT> >& batch)
	{
		interInitialize(); 
		m_DataManager->InitializeMemory(batch);
	}
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	bool DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::clearAllData( )
	{		 
		return m_DataManager->clearAllData();
	}


	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
		void DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::interInitialize()
	{
		if(m_isInitialized)
		{
			return;
		}
		//ProcessGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}
	 	if (m_DataManager==NULL)
		{			
			m_DataManager=new DataManager(DataManager::MaxItemCount, DataManager::TotalSizeLength);			
		}
           
		 

		m_isInitialized=TRUE;
 	}
	
	template<typename CacheDataKeyType, typename CacheDataT, typename DataManager>
	bool	DataStoreUpdater<CacheDataKeyType,CacheDataT,DataManager>::onDispatchObjects(std::vector<boost::shared_ptr<CacheDataT> >&  batch)
	{
		try
		{
			m_DataManager->processEvent(batch);
		}	 
		catch (std::exception& e)
		{
			std::string msg( m_updaterName );				 
			msg.append( " has std::exception:");
			msg.append( e.what() );
			//LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
		}   
		
		return TRUE;
	}
 
}

#endif // !defined(AFX_DATASTOREUPDATER_H__6A1D4794_0994_484B_BB3F_64A2330EB980__INCLUDED_)

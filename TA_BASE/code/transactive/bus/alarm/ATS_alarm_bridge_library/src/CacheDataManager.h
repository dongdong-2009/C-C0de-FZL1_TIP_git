// CacheDataManager.h: interface for the CacheDataManager class.
//
//////////////////////////////////////////////////////////////////////
//
/**
* The source code in this file is the property of
* ST Electronics and is not for redistribution
* in any form.
*
* Source:   
* @author:  Luohr
* @version: $Revision: 1.0.0.0 $
*
* Last modification: $Date: 2009/08/04 10:39:58 $
* Last modified by:  $Author: luohr $
*
 */

#if !defined(AFX_CACHEDATAMANAGER_H__AF325088_0BD4_41A9_9902_2ACBA78C334E__INCLUDED_)
#define AFX_CACHEDATAMANAGER_H__AF325088_0BD4_41A9_9902_2ACBA78C334E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4243)
#pragma warning(disable:4018)
#endif // defined (_MSC_VER)


 


#include <string>
#include <deque>
#include <map>
#include "boost/shared_ptr.hpp"
#include "core/synchronisation/src/ProcessGuard.h"
#include "core/synchronisation/src/IProcessLockable.h"
#include "core/synchronisation/src/NonReEntrantProcessLockable.h"
#include "CacheDataWraper.h"
#include "ShareMemoryHelper.h"

#include "CacheDataHead.h"

 
namespace TA_Base_Bus
{
	template<typename CacheDataKeyType, typename CacheDataT>
	class CacheDataManager  
	{     
	private:
		typedef std::vector<boost::shared_ptr<CacheDataT> > CacheDataList;
		//support a high performance method to judge a data is deleted or not.
		typedef std::map<CacheDataKeyType,  CacheDataTaker<CacheDataT> >   CacheDataMapIndex;  
	public:	
		CacheDataManager(long maxBlockCount,long blockByteSize, std::string memName);
		virtual ~CacheDataManager();
		//this function is used by DataStoreUpdater.
		//DataStoreUpdater will initialize the memory based on database while it starts.
		//it should be call at initialize method of DataStoreUpdater.
		//the function will clear all data in the memory, then put the batch data into the memory 
		//and initialization version of  the memory plus 1.		
		virtual void InitializeMemory(CacheDataList&  batch);
		
		 

		//--------------------memory operation functions----------------------------------
		 
		//write data to memory
		virtual bool writeCacheDataToMem(CacheDataT* cacheData,ShareMemoryHelper* mem, char* pos)=0;
		//read data from memory
		virtual bool readCacheDataFromMem(CacheDataT* cacheData,ShareMemoryHelper* mem, char* pos)=0; 
		//only one data
		virtual bool IsDataWithID() {return false;};
		 
	    //-----------------------------------------------------------------------------
		//this method is used by DataStoreReceiver.
		//it makes sure that DataStoreReceiver gets a right map from the memory.
		//it should be call at initialization of DataStoreReceiver.
		virtual void buildCacheDataIndex()
		{
		  initProcessLock();
		  TA_Base_Core::ProcessGuard wGuard(*m_RW_Process_Mutex);
		  openOrCreateMem();
		 
		}
	    virtual void processEvent(CacheDataList&  batch)
		{
			TA_Base_Core::ProcessGuard guard(*m_RW_Process_Mutex);
 			updateData (batch);
		}        
	 		 
		 
		virtual CacheDataList* getData();
		virtual CacheDataList* getDataWithoutLock();
		virtual bool clearAllData();

		virtual TA_Base_Core::IProcessLockable* getlock()
		{
			return m_RW_Process_Mutex;
		}
	protected:
		bool     m_PrintCacheData;
	private:
		TA_Base_Core::IProcessLockable*    m_RW_Process_Mutex;
       
	 
  
		CacheDataHead   m_lastCacheDataHead;  //helper to judge data is changed or not.

		ShareMemoryHelper*      m_sharedMem;
        long m_blockByteSize;
		long m_maxBlockCount;
	
		std::string   m_RW_Process_Mutex_Name;
		long m_cacheDataHeadSize;
		long m_cacheDataRowHeadSize;
		
		std::string m_cacheDataManagerName;		                 
        
		void initProcessLock();
		void updateData(CacheDataList& list);
		void updateData( long rowIndex,CacheDataT* cacheData, CacheDataHead& dataHead,FILETIME& ft);
		void insertNewData( CacheDataT* cacheData, CacheDataHead& dataHead,FILETIME& ft); 		
		bool openOrCreateMem(); 
	 
		void interInitializeMemory(CacheDataList&  batch);	
		//-----------------testing only------------------------------
		void printMemory();
	};

	 


	
	template<typename CacheDataKeyType, typename CacheDataT>
	bool  CacheDataManager<CacheDataKeyType, CacheDataT>::openOrCreateMem()
	{			
		if(m_sharedMem->openFile())
		{
			//somebody have build the memory, so just build it from memory
			if(m_sharedMem->mappingFile())
			{
				//LOG1(SourceInfo, DebugUtil::DebugInfo, "open Shared Mem %s", m_cacheDataManagerName.c_str());
				return TRUE;			
			}
		}
		else
		{
			//LOG1(SourceInfo, DebugUtil::DebugInfo, "create Shared Mem %s", m_cacheDataManagerName.c_str());
			m_sharedMem->createFile(m_maxBlockCount*m_blockByteSize+ m_cacheDataHeadSize);
			if(m_sharedMem->mappingFile())
			{
				CacheDataHead dataHead;
				dataHead.DataVersion=0;
				dataHead.LastInitialMemVersion=0;
				dataHead.TotalSize=0;
				m_sharedMem->writeStruct(m_sharedMem->getCurWritePos(),&dataHead, m_cacheDataHeadSize);
				return TRUE;
			}
			
		}
		return FALSE;
	}


	  
	template<typename CacheDataKeyType, typename CacheDataT>
	void	CacheDataManager<CacheDataKeyType, CacheDataT>::initProcessLock()
	{
		std::string info="initProcessLock " +   m_RW_Process_Mutex_Name;
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, info.c_str());
		try
		{			
			if(m_RW_Process_Mutex==NULL)
			{
				m_RW_Process_Mutex=(TA_Base_Core::IProcessLockable*) (new TA_Base_Core::NonReEntrantProcessLockable(m_RW_Process_Mutex_Name.c_str()));
			}
		}		 
		catch(...)
		{
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "initProcessLock error");
		}
		 
	};

	template<typename CacheDataKeyType, typename CacheDataT>
		CacheDataManager<CacheDataKeyType, CacheDataT>::CacheDataManager(long maxBlockCount,long blockByteSize, std::string memName)
	{
		 
		m_lastCacheDataHead.LastInitialMemVersion=0;
		m_lastCacheDataHead.DataVersion=0;		 
		m_cacheDataHeadSize=sizeof(CacheDataHead);
		m_cacheDataRowHeadSize=sizeof(CacheDataRowHead);		
		m_blockByteSize=blockByteSize+ m_cacheDataRowHeadSize;
		m_maxBlockCount=maxBlockCount;	 
		std::string name;		
		m_RW_Process_Mutex_Name="Global\\ProcessLock" + memName;	 
		m_RW_Process_Mutex=NULL;		
		name="Global\\CacheN"   + memName;
		m_sharedMem=new ShareMemoryHelper(name);	
        openOrCreateMem();
		initProcessLock();
		m_PrintCacheData=FALSE;
	};

	template<typename CacheDataKeyType, typename CacheDataT>
		CacheDataManager<CacheDataKeyType ,CacheDataT>::~CacheDataManager()
	{
		if (m_sharedMem!=NULL)
		{
			delete m_sharedMem;
			m_sharedMem=NULL;			 
		}
		if (m_RW_Process_Mutex!=NULL)
		{
			delete m_RW_Process_Mutex;
			m_RW_Process_Mutex=NULL;
		}
	 };

   


	template<typename CacheDataKeyType, typename CacheDataT>
		void CacheDataManager<CacheDataKeyType ,CacheDataT>::interInitializeMemory(CacheDataList&  batch)
	{			  
		//LOG1(SourceInfo, DebugUtil::DebugInfo, "interInitializeMemory in %s", m_cacheDataManagerName.c_str());	 	
		FILETIME   ft;			
		CacheDataHeadHelper::getCurrentDateTime(ft);
		 
		CacheDataHead dataHead;	 
		//clear memory
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		dataHead.TotalSize=0;
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead); 		 
		updateData(batch); 
		//add initialization version 
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
        dataHead.LastInitialMemVersion++;
		if (dataHead.LastInitialMemVersion==0) dataHead.LastInitialMemVersion++;
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);
	 
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "interInitializeMemory out");
	}


	template<typename CacheDataKeyType, typename CacheDataT>
	 void CacheDataManager<CacheDataKeyType ,CacheDataT>::InitializeMemory(CacheDataList&  batch)
	{			  
		//LOG1(SourceInfo, DebugUtil::DebugInfo, "InitializeMemory in %s", m_cacheDataManagerName.c_str());
		initProcessLock();		
	 
		TA_Base_Core::ProcessGuard wGuard(*m_RW_Process_Mutex); 
		openOrCreateMem();
	    interInitializeMemory(batch);
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "InitializeMemory out");
	}
    
	 
	template<typename CacheDataKeyType, typename CacheDataT>
	 void CacheDataManager<CacheDataKeyType ,CacheDataT>::insertNewData(CacheDataT* cacheData, CacheDataHead& dataHead,FILETIME& ft)
	{		
		char* curPos=0; 
		char* basePos=m_sharedMem->getBasePos();
		CacheDataRowHead dataRowHead;
		bool usedRecoverMem=FALSE;
		long rowIndex=0;	  
		rowIndex=dataHead.TotalSize;
		curPos=basePos + (m_blockByteSize)*(dataHead.TotalSize)+m_cacheDataHeadSize;	
		//LOG2(SourceInfo, DebugUtil::DebugDebug,"position new=%d key=%s" , rowIndex, TA_Base_Core::toString(key).c_str());		 	
		dataRowHead.DeletedFlag=0;		 
		dataRowHead.LastModifyTime=ft;
		CacheDataHeadHelper::writeDataRowHead(m_sharedMem, curPos,dataRowHead);
		curPos=curPos + m_cacheDataRowHeadSize;
		bool r=writeCacheDataToMem(cacheData, m_sharedMem, curPos);
		if(r==FALSE)
		{				 
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "new writeCacheDataToMem error");
			return;
		}		 
		dataHead.TotalSize++;
		CacheDataHeadHelper::writeDataHead(m_sharedMem,dataHead); //update the total count
	 
	}

	template<typename CacheDataKeyType, typename CacheDataT>
	 void CacheDataManager<CacheDataKeyType ,CacheDataT>::updateData( long rowIndex, CacheDataT* cacheData, CacheDataHead& dataHead,FILETIME& ft)
	{
		char* curPos=0; 
		char* basePos=m_sharedMem->getBasePos();
		CacheDataRowHead dataRowHead;
		FILETIME ftLastTime;				  
		curPos=basePos + (m_blockByteSize*rowIndex)+m_cacheDataHeadSize;//e
		CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead); //read data row head		 		 
		ftLastTime=dataRowHead.LastModifyTime;
		dataRowHead.LastModifyTime.dwHighDateTime=0;
		dataRowHead.LastModifyTime.dwLowDateTime=0;                   
		CacheDataHeadHelper::writeDataRowHead(m_sharedMem, curPos, dataRowHead);
		bool r=writeCacheDataToMem(cacheData, m_sharedMem, curPos + m_cacheDataRowHeadSize);
		if(r==FALSE)
		{				 
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "new writeCacheDataToMem error");
			return;
		}		 		
		CacheDataHeadHelper::writeDataRowTimeField(m_sharedMem, curPos,ft); //update the time
		
	}

	template<typename CacheDataKeyType, typename CacheDataT>
	 void CacheDataManager<CacheDataKeyType ,CacheDataT>::updateData(CacheDataList& datalist)
	{		
		char* curPos=0; 
		char* basePos=m_sharedMem->getBasePos();		 
		FILETIME   ft;			
		CacheDataHead dataHead; 
		CacheDataHeadHelper::getCurrentDateTime(ft);
		//update the version of the data;		
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);        
        unsigned long len=datalist.size();

		if ((dataHead.TotalSize+len)>m_maxBlockCount)
		{
			throw  "QUEUE_OVERFLOW";
		}
		dataHead.DataVersion++;
		if (dataHead.DataVersion==0)  dataHead.DataVersion++;
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);
		//LOG5(SourceInfo, DebugUtil::DebugDebug,"updateData t1=%u, t2=%u count=%u MemCount=%u %s", 
		//	 ft.dwHighDateTime,ft.dwLowDateTime, datalist.size(),dataHead.TotalSize,m_cacheDataManagerName.c_str());           
		CacheDataList::iterator iterData;		 
		for (iterData=datalist.begin();iterData!=datalist.end();iterData++)
		{			   
		   CacheDataT* cacheData=(*iterData).get();	
		   bool isfixedSize=IsDataWithID();
		   if (isfixedSize)
		   {			   
			   updateData(0,  cacheData, dataHead, ft);
			   CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);  
			   dataHead.TotalSize=1;
			   CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);
		   }
		   else //update memory data;
		   {								    
			   
			   insertNewData(cacheData, dataHead, ft);
		   }   				 
		}
		 
	}  //updateData end

     
	template<typename CacheDataKeyType, typename CacheDataT>
		void CacheDataManager<CacheDataKeyType ,CacheDataT>::printMemory()
	{
		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;
		CacheDataList* datas=new CacheDataList();
	    CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		long i=0;
		char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		char* curPos;
		FILETIME  ftCache;
		ftCache.dwLowDateTime=0;
		ftCache.dwHighDateTime=0;	    
		CacheDataT   testarr[200];;
		for (i=0;i<dataHead.TotalSize && i<200;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);			 
		    curPos=curPos+ m_cacheDataRowHeadSize;
			CacheDataT  cacheData;
			bool r=readCacheDataFromMem(&cacheData,m_sharedMem,curPos);	
			testarr[i]=cacheData;
	   }
//	   i=0;	   
// 	   CacheDataT  testarr[250];           
// 	   std::vector< boost::shared_ptr< CacheDataT > >::iterator iterTest ;
// 	   i=0;
// 	   for (iterTest=datas->begin();iterTest!=datas->end();iterTest++)
// 	   {
// 		   if (i<250)
// 		   {
// 			   testarr[i]=*(*iterTest).get();
// 		   }
// 		   
// 		   i++;
// 		 }
	}





	template<typename CacheDataKeyType, typename CacheDataT>  
	std::vector< boost::shared_ptr< CacheDataT > >* CacheDataManager<CacheDataKeyType ,CacheDataT>::getData()
	{		    
		TA_Base_Core::ProcessGuard  guard(*m_RW_Process_Mutex);		     
		return getDataWithoutLock();		 
	}


	template<typename CacheDataKeyType, typename CacheDataT>  
	std::vector< boost::shared_ptr< CacheDataT > >* CacheDataManager<CacheDataKeyType ,CacheDataT>::getDataWithoutLock()
	{		    		 
		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;
		CacheDataList* datas=new CacheDataList();
		datas->reserve(3000);
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);	 
		long i=0;
		char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		char* curPos;	 
		for (i=0;i<dataHead.TotalSize;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);					 			 
			{
				curPos=curPos+ m_cacheDataRowHeadSize;
				CacheDataT* cacheData=new CacheDataT();
				bool r=readCacheDataFromMem(cacheData,m_sharedMem,curPos);
				if (!r)
				{
					//LOG1(SourceInfo, DebugUtil::DebugError, "read mem data error %s", m_cacheDataManagerName.c_str());
					continue;
				}
				boost::shared_ptr<CacheDataT> sharedCacheData(cacheData);
				datas->push_back(sharedCacheData);						 
			}			  
		}
        bool needResetMemCount=!IsDataWithID();
		if (needResetMemCount)
		{
			dataHead.TotalSize=0;
			CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);	 
		}
		else
		{
			//dataHead.TotalSize=1;
			CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);	 
		}

		//virtual bool IsDataWithID() {return false;};
 
	 
		return datas;		 
	}
   template<typename CacheDataKeyType, typename CacheDataT>
	bool CacheDataManager<CacheDataKeyType ,CacheDataT>::clearAllData()
	{		    
		TA_Base_Core::ProcessGuard  guard(*m_RW_Process_Mutex);		     

		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;
		CacheDataList* datas=new CacheDataList();		
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);	 
		dataHead.DataVersion++;
		dataHead.TotalSize=0;
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);	 
		return true;		 
	}
}
#endif // !defined(AFX_CACHEDATAMANAGER_H__AF325088_0BD4_41A9_9902_2ACBA78C334E__INCLUDED_)



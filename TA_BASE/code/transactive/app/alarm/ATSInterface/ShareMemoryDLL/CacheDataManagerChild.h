// CacheDataManagerChild.h: interface for the CacheDataManagerChild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHEDATAMANAGERCHILD_H__F4EAE118_DF71_4D45_862D_A48846D142FF__INCLUDED_)
#define AFX_CACHEDATAMANAGERCHILD_H__F4EAE118_DF71_4D45_862D_A48846D142FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
//#pragma warning(disable:4018)
#endif // defined (_MSC_VER)

#include <string>
#include <deque>
#include <list>
#include <map>
#include "boost/shared_ptr.hpp" 
#include "core/utilities/src/TA_String.h"
#include "core/utilities/src/DebugUtil.h"
#include "ShareMemoryHelper.h"
#include "CacheDataHead.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
	class CacheDataManagerChild  
	{
	private:		 
		typedef  std::map<FatherKeyType, std::vector<unsigned long>* >  DataIndex;  //father's key, child's position;
		typedef  std::deque<CacheDataPair<FatherKeyType> >     RecycleIndex;
		typedef  std::vector<boost::shared_ptr<FatherDataType> > FatherDataList;
	public:

		//write a child data to memory
		virtual bool writeChildData(FatherKeyType& fatherKey, ShareMemoryHelper* mem, char* wPos, ChildDataType* childData)=0;
	    //get child data from memory
		virtual bool readChildData(ShareMemoryHelper* mem, char* rPos, ChildDataType* childData)=0;
	 
		//get the child data from father
		virtual std::vector<ChildDataType>* getChildDatafromFather(FatherDataType* fatherData)=0;
	    //set the child data of father
		virtual void setChildData(FatherDataType* fatherData, std::vector<ChildDataType>* childData)=0;
	    //get key of father data
		virtual FatherKeyType getFatherDataKey(FatherDataType* fatherData)=0;
		//get key of father from memory
		virtual FatherKeyType readFatherDataKeyFromMem(ShareMemoryHelper* mem, char* pos)=0;

		virtual void initializeMemory(FatherDataList*  fatherDatas);

        //this function is used on receiving data.
		virtual void getChildDataToFather(FatherDataList*  fatherDatas);
	    
		//update child data based on father data
		virtual void updateData(FILETIME* ft,FatherDataType* fatherData);
		long recycleChildData(FatherKeyType fatherKey);

		bool openOrCreateMem();

		CacheDataManagerChild(long maxBlockCount,long blockByteSize, std::string memName);
		virtual ~CacheDataManagerChild();

	private:
		DataIndex m_relationMap; //it keeps childes' position of father data	 
		RecycleIndex   m_recycleIndex;
		ShareMemoryHelper*      m_sharedMem;
        long m_blockByteSize,m_maxBlockCount;		 
		long m_cacheDataHeadSize;
		long m_cacheDataRowHeadSize;	  
		std::string m_cacheDataManagerChildName;
		void recycleMemory(long recycleCount);
		long deleteOldestData();
		void gotoRecyclePosition();
		bool interRecycleChildData(FatherKeyType& fatherKey,std::vector<unsigned long>* relationlist);
		bool updateChildData(FILETIME* ft,FatherKeyType fatherKey,std::vector<ChildDataType>*   childDatalist);
        void interInitializeMemory(FatherDataList*  fatherDatas);
		//-----------------testing only------------------------------
		void printMemory();
	};

//////////////////////////////////////////////////////////////////////////
	//function body 
//////////////////////////////////////////////////////////////////////////
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::CacheDataManagerChild(long maxBlockCount,long blockByteSize, std::string memName)
	{
		m_sharedMem=NULL;
		m_cacheDataManagerChildName=TA_Base_Core::gPtrToStr(this).c_str();
		m_cacheDataHeadSize=sizeof(CacheDataHead);
		m_cacheDataRowHeadSize=sizeof(CacheDataRowHead);
	 
		m_blockByteSize=blockByteSize+ m_cacheDataRowHeadSize;
		m_maxBlockCount=maxBlockCount;	 
		std::string name;
		name="Global\\CacheChild"  + memName;
		m_sharedMem=new ShareMemoryHelper(name);
	}

	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::~CacheDataManagerChild()
	{
		if (m_sharedMem!=NULL)
		{
			delete m_sharedMem;
			m_sharedMem=NULL;			 
		}		 
	}
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::gotoRecyclePosition()
	{
		while (m_recycleIndex.size()>0)
		{			
			CacheDataPair<FatherKeyType> avbPair=m_recycleIndex.front();
			char* curPos;
			curPos=m_sharedMem->getBasePos() + (m_blockByteSize)*(avbPair.RowIndex)+m_cacheDataHeadSize+m_cacheDataRowHeadSize;
			FatherKeyType key=readFatherDataKeyFromMem(m_sharedMem,curPos);
			if (key==avbPair.DataKey)
			{
				return;
			}
			else
			{
				m_recycleIndex.pop_front();
			}
		}
		return;
	} 

	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		long CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::recycleChildData(FatherKeyType fatherKey)
	{	 
        DataIndex::iterator iter;
		long count=0;
		iter=m_relationMap.find(fatherKey);		 
		if (iter!=m_relationMap.end())  
		{
			//recycle old data;
			std::vector<unsigned  long>*  relationlist=iter->second;
			if (relationlist!=NULL)
			{
				count=relationlist->size();
			}			
			interRecycleChildData(fatherKey, relationlist);	
			if (iter->second!=NULL)
			{		 
				iter->second->clear();
				delete iter->second;
				iter->second=NULL;
			}
			m_relationMap.erase(fatherKey);
		}
		return count;
	}


	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		bool CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::interRecycleChildData(FatherKeyType& fatherKey, std::vector<unsigned long>* relationlist)
	{
		char* curPos=0;
	    char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		CacheDataRowHead cacheDataRowHead;
		if (relationlist!=NULL)
		{
			std::vector<unsigned long>::iterator  iterOldPosition;
			for (iterOldPosition=relationlist->begin();iterOldPosition!=relationlist->end();iterOldPosition++)
			{					 
				unsigned long pos=*iterOldPosition;
				CacheDataPair<FatherKeyType> posPair;
				posPair.DataKey=fatherKey;
				posPair.RowIndex=pos;
				m_recycleIndex.push_front(posPair);
 				curPos=basePos + (m_blockByteSize)*(pos);
				CacheDataHeadHelper::readDataRowHead(m_sharedMem,curPos,cacheDataRowHead);
				cacheDataRowHead.DeletedFlag=1;
				cacheDataRowHead.LastModifyTime.dwLowDateTime=0;
				cacheDataRowHead.LastModifyTime.dwHighDateTime=0;
				CacheDataHeadHelper::writeDataRowHead(m_sharedMem,curPos,cacheDataRowHead);	
			}	
		}
		return TRUE;
	}
    
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		long CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::deleteOldestData()
	{
		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;	 
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		long i=0;
		char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		char* curPos; 
		FILETIME ft;
		ft.dwHighDateTime=0-1;
		ft.dwLowDateTime=0-1;		
		for (i=0;i<dataHead.TotalSize;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);	
			if (dataRowHead.DeletedFlag==0)
			{			
				if( (ft.dwHighDateTime==dataRowHead.LastModifyTime.dwHighDateTime 
					&& ft.dwLowDateTime>dataRowHead.LastModifyTime.dwLowDateTime)
					|| (ft.dwHighDateTime>dataRowHead.LastModifyTime.dwHighDateTime)
					)
				{
					ft.dwLowDateTime=dataRowHead.LastModifyTime.dwLowDateTime;
					ft.dwHighDateTime=dataRowHead.LastModifyTime.dwHighDateTime;
				}
			}
		}
		long count=0;
		for (i=0;i<dataHead.TotalSize;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);	
			if (dataRowHead.DeletedFlag==0)
			{
				if( (ft.dwHighDateTime==dataRowHead.LastModifyTime.dwHighDateTime 
					&& ft.dwLowDateTime==dataRowHead.LastModifyTime.dwLowDateTime)			 
				  )
				{
					 curPos=curPos+m_cacheDataRowHeadSize;
					 FatherKeyType fatherKey=readFatherDataKeyFromMem(m_sharedMem,curPos);
					 int n=recycleChildData(fatherKey);
					 count=count+1;				 
					 //count++;
				}	
			}
			else
			{
				count=count+1;	
			}
		}
		return count;
	}

	//Method:based on update time, move memory;
	//When: 1>memory is full
	//      2>schedule + size increase (based on message type)
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::recycleMemory(long recycleCount)
	{			
		LOG1(SourceInfo, DebugUtil::DebugInfo, "recycleMemoryChild in %s", m_cacheDataManagerChildName.c_str());
		typedef std::map<FatherKeyType,  std::list<ChildDataType>*  > DATAMAPFatherChilds;
		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;
		FatherKeyType fatherkey;
		long count=0;
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		long n=0; long i=0;
		FILETIME ft;
		n=deleteOldestData();
		count=count + n;
		long maxLoopCount=0;
       
		while (count<recycleCount && (dataHead.TotalSize-count)>0 && count>0 && maxLoopCount<recycleCount)
		{
            n=deleteOldestData();
			count=count + n;
			maxLoopCount++;
		}
 		m_recycleIndex.clear();
		DataIndex::iterator iterRelationMap;
		for (iterRelationMap=m_relationMap.begin();iterRelationMap!=m_relationMap.end();iterRelationMap++)
		{
			if (iterRelationMap->second!=NULL)
			{	 
				iterRelationMap->second->clear();
				delete iterRelationMap->second;
				iterRelationMap->second=NULL;
			}
		}
		m_relationMap.clear();   

        
		DATAMAPFatherChilds datamap;
        DATAMAPFatherChilds::iterator iterDataMap;
		char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		char* curPos; 
		n=0;
		for (i=0;i<dataHead.TotalSize;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);	
			if (dataRowHead.DeletedFlag==0)
			{
				curPos=curPos+m_cacheDataRowHeadSize;
				ChildDataType childData;
                fatherkey=readFatherDataKeyFromMem(m_sharedMem,curPos);
				readChildData(m_sharedMem, curPos, &childData);
				n++;
				iterDataMap=datamap.find(fatherkey);
				if (iterDataMap==datamap.end())
				{
					std::list<ChildDataType>* childdatalist=new std::list<ChildDataType>;
					datamap.insert(DATAMAPFatherChilds::value_type(fatherkey, childdatalist ));
				}
				else
				{
                   iterDataMap->second->push_back(childData);
				}				 
			}
		}
		dataHead.LastInitialMemVersion++;
		dataHead.TotalSize=n;
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead);
        
		unsigned long  pos=0;
		CacheDataHeadHelper::getCurrentDateTime(ft);
		for (iterDataMap=datamap.begin();iterDataMap!=datamap.end();iterDataMap++)
		{  
			std::list<ChildDataType>::iterator iterChildData;
			
			std::vector<unsigned long>*   posList=new  std::vector<unsigned long>();
			fatherkey=iterDataMap->first;
			std::list<ChildDataType>* listData;
			listData=(iterDataMap->second);
			for (iterChildData=listData->begin();iterChildData!=listData->end();iterChildData++)
			{
				curPos=basePos + (m_blockByteSize)*i;
				dataRowHead.LastModifyTime=ft;
				dataRowHead.DeletedFlag=0;
                ChildDataType childData=*iterChildData;
				CacheDataHeadHelper::writeDataRowHead(m_sharedMem,curPos,dataRowHead);
				curPos=curPos + m_cacheDataRowHeadSize;
				
				writeChildData(fatherkey, m_sharedMem,curPos, &childData );   
				posList->push_back(pos);
			}			
			pos++;
			m_relationMap.insert(DataIndex::value_type(iterDataMap->first,posList ));
			delete  iterDataMap->second;
		 
		}
		datamap.clear();
		
		LOG1(SourceInfo, DebugUtil::DebugInfo, "recycleMemoryChild out count=%d", count);
	}	

	
   	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		bool	CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::updateChildData(FILETIME* ft,FatherKeyType fatherKey, std::vector<ChildDataType>*   childDatalist)
	{
		char* curPos=0;
		char* basePos=m_sharedMem->getBasePos();
		CacheDataHead dataHead; CacheDataRowHead dataRowHead;
        dataRowHead.LastModifyTime.dwHighDateTime=ft->dwHighDateTime;
		dataRowHead.LastModifyTime.dwLowDateTime=ft->dwLowDateTime;
		//CacheDataHeadHelper::getCurrentDateTime(dataRowHead.LastModifyTime);
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		std::vector<ChildDataType>::iterator childiter;
		if (childDatalist==NULL)
		{
			return TRUE;
		}
		if (childDatalist->size()<=0)
		{
			return TRUE;
		}
		std::vector<unsigned long>*  relationlist=new std::vector<unsigned long>();
		relationlist->reserve(childDatalist->size());
        
		if((dataHead.TotalSize+childDatalist->size())>=(m_maxBlockCount-m_recycleIndex.size())  )
		{
			LOG1(SourceInfo, DebugUtil::DebugWarn, "data arrive max size %s", m_cacheDataManagerChildName.c_str());
			recycleMemory(m_maxBlockCount/5);
			CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		}
		
		for (childiter=childDatalist->begin();childiter!=childDatalist->end();childiter++)
		{		
					 
			bool usedRecoverMem=FALSE;
			long rowIndex=0;
			if(m_recycleIndex.size()>0) //recover position
			{                      
				CacheDataPair<FatherKeyType> avbPair=m_recycleIndex.front();
				curPos=basePos + (m_blockByteSize)*(avbPair.RowIndex)+m_cacheDataHeadSize;
				usedRecoverMem=TRUE;
				rowIndex=avbPair.RowIndex;
			}
			else
			{
				rowIndex=dataHead.TotalSize;
				curPos=basePos + (m_blockByteSize)*(dataHead.TotalSize)+m_cacheDataHeadSize;					   
			}
			dataRowHead.DeletedFlag=0;
            CacheDataHeadHelper::writeDataRowHead(m_sharedMem, curPos, dataRowHead);
			curPos=curPos+m_cacheDataRowHeadSize;
			bool r=writeChildData(fatherKey, m_sharedMem,curPos, childiter);
			if(r==FALSE)
			{				 
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "new writeCacheDataToMem error");
				continue;
			}
			relationlist->push_back(rowIndex);
			if (usedRecoverMem==FALSE)
			{				
				dataHead.TotalSize++;				
			}
			else
			{
				m_recycleIndex.pop_front(); //delete the position from recycled list

			}			
		}
		m_relationMap.insert(DataIndex::value_type(fatherKey,relationlist ));
		CacheDataHeadHelper::writeDataHead(m_sharedMem, dataHead); //update the total count
		//LOG2(SourceInfo, DebugUtil::DebugDebug,"updateChildData   MemCount=%u %s", 	dataHead.TotalSize,m_cacheDataManagerChildName.c_str());          

		return TRUE;
	}



	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
	 void	CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::updateData(FILETIME* ft,FatherDataType* fatherData)
	{		
        FatherKeyType fatherKey=getFatherDataKey(fatherData);
        //DataIndex::iterator iter;
		//iter=m_relationMap.find(fatherKey);
		std::vector<ChildDataType>*   childDatalist=getChildDatafromFather(fatherData);
 		recycleChildData(fatherKey);		 
		updateChildData(ft,fatherKey,childDatalist); 		
	}

	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::printMemory()
	{
		CacheDataHead dataHead; 
		CacheDataRowHead dataRowHead;	 
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		long i=0;
		char* basePos=m_sharedMem->getBasePos()+m_cacheDataHeadSize;
		char* curPos;
		FILETIME  ftCache;
		ftCache.dwLowDateTime=0;
		ftCache.dwHighDateTime=0;	    
		ChildDataType  testarr [2400];
		for (i=0;i<dataHead.TotalSize && i<2400;i++)
		{			   
			curPos=basePos + (m_blockByteSize)*i;			   
			CacheDataHeadHelper::readDataRowHead(m_sharedMem, curPos,dataRowHead);			 
			curPos=curPos+ m_cacheDataRowHeadSize;
			ChildDataType cacheData;
			bool r=readChildData(m_sharedMem,curPos,&cacheData);	
			testarr[i]=cacheData;
		}
		i=0;		
	}

	
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::getChildDataToFather( FatherDataList*  fatherDatas)
	{
		 
		typedef std::map<FatherKeyType, std::vector<ChildDataType>* >  ChildDataMap;
		FatherKeyType fatherKey;
		ChildDataMap  childDataMap;
		ChildDataMap::iterator  childDataIter;
		FatherDataList::iterator iter;
		

		//--------testing only
		//printMemory();
		//--------testing only

		for (iter=fatherDatas->begin();iter!=fatherDatas->end();iter++)
		{
			std::vector<ChildDataType>* lst=new std::vector<ChildDataType>;
			fatherKey=getFatherDataKey((*iter).get());
			childDataMap.insert(ChildDataMap::value_type(fatherKey, lst));
		}

		//get all child data for father , and childs savs to map;
		char* curPos=0;
		char* basePos=m_sharedMem->getBasePos();
		CacheDataHead dataHead;  CacheDataRowHead dataRowHead;
		CacheDataHeadHelper::readDataHead(m_sharedMem, dataHead);
		long count=0;
		for (int i=0; i<dataHead.TotalSize;i++)
		{
			curPos=basePos + (m_blockByteSize)*(i)+m_cacheDataHeadSize;			
			CacheDataHeadHelper::readDataRowHead(m_sharedMem,curPos, dataRowHead);
            if (dataRowHead.DeletedFlag==0)
			{
				curPos=curPos+m_cacheDataRowHeadSize;
				fatherKey=readFatherDataKeyFromMem(m_sharedMem, curPos);
				
				childDataIter=childDataMap.find(fatherKey);
				if (childDataIter!=childDataMap.end())
				{
					ChildDataType  childData;
					readChildData(m_sharedMem, curPos, &childData);
					childDataIter->second->push_back(childData);
					count++;
				}
			}
		}	 

		LOG3(SourceInfo, DebugUtil::DebugDebug,"getChildDataToFather   MemCount=%u getcount=%u %s", dataHead.TotalSize,count,m_cacheDataManagerChildName.c_str());           

        //set child datas to father
		for (iter=fatherDatas->begin();iter!=fatherDatas->end();iter++)
		{
			fatherKey=getFatherDataKey((*iter).get());
			childDataIter=childDataMap.find(fatherKey);
			if (childDataIter!=childDataMap.end())
			{
				 setChildData((*iter).get(), childDataIter->second);
			}
		}

        //clear memory
		for (childDataIter=childDataMap.begin();childDataIter!=childDataMap.end();childDataIter++)
		{
			 childDataIter->second->clear();
			 delete childDataIter->second;
			 childDataIter->second=NULL;
		}
		childDataMap.clear();

	}
	


	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::interInitializeMemory( FatherDataList*  fatherDatas)
	{
 
		CacheDataHead cacheDataHead;
		CacheDataHeadHelper::readDataHead(m_sharedMem, cacheDataHead);
		//clear all data		
		cacheDataHead.DataVersion++;
		cacheDataHead.TotalSize=0;
		CacheDataHeadHelper::writeDataHead(m_sharedMem,cacheDataHead);
		m_recycleIndex.clear();
		DataIndex::iterator iterRelationMap;
		for (iterRelationMap=m_relationMap.begin();iterRelationMap!=m_relationMap.end();iterRelationMap++)
		{
			if (iterRelationMap->second!=NULL)
			{	 
				iterRelationMap->second->clear();
				delete iterRelationMap->second;
				iterRelationMap->second=NULL;
			}
		}
		m_relationMap.clear();        
        cacheDataHead.LastInitialMemVersion++;
		CacheDataHeadHelper::writeDataHead(m_sharedMem,cacheDataHead);	 
	} 

	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		void CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::initializeMemory( FatherDataList*  fatherDatas)
	{
		//open memory
		openOrCreateMem();
		interInitializeMemory(fatherDatas);
	} 
 
	template<typename FatherKeyType, typename FatherDataType, typename ChildDataType>
		bool CacheDataManagerChild<FatherKeyType, FatherDataType, ChildDataType>::openOrCreateMem()
	{
		if(m_sharedMem->openFile())
		{				
			if(m_sharedMem->mappingFile())
			{
				LOG1(SourceInfo, DebugUtil::DebugInfo, "open Shared Mem %s",m_cacheDataManagerChildName.c_str());
				return true;
			}
		}	
		else
		{
			LOG1(SourceInfo, DebugUtil::DebugInfo, "create Shared Mem %s",m_cacheDataManagerChildName.c_str());
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
}

#endif // !defined(AFX_CACHEDATAMANAGERCHILD_H__F4EAE118_DF71_4D45_862D_A48846D142FF__INCLUDED_)

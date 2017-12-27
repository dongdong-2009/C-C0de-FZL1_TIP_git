// CacheDataHead.h: interface for the CacheDataHead class.
//
//////////////////////////////////////////////////////////////////////




#if !defined(AFX_CACHEDATAHEAD_H__C67D67F4_A328_46BC_8534_4CE3BDD07689__INCLUDED_)
#define AFX_CACHEDATAHEAD_H__C67D67F4_A328_46BC_8534_4CE3BDD07689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "boost/shared_ptr.hpp"
#include "ShareMemoryHelper.h"

namespace TA_Base_Bus
{	
	template<typename CacheDataKeyType>
	struct CacheDataPair
	{
		long RowIndex;             //position of record in shared memory
		CacheDataKeyType DataKey;  //PK of record
	};

	template<typename CacheDataT>
	struct  CacheDataTaker
	{
		long RowIndex;      //this field is used to judge quickly the data is deleted or not.
		//record content in the RowIndex position, which may be deleted and its position may be recycled.
		//keeping it in the heap, so we can create a deleting message if it had been deleted and position has been recycled.
		boost::shared_ptr<CacheDataT> data;  
	};

	struct CacheDataHead			 
	{		 
		// quantity of cache data, this field's position can not be change in the struct.
		long TotalSize;             
		// it records the times of memory initialization.
		unsigned long LastInitialMemVersion;
		// Data version, this value will be increase 1 after each modification.			  
		unsigned long DataVersion;  		 
		CacheDataHead():TotalSize(0),DataVersion(0),LastInitialMemVersion(0){}
	};

	struct  CacheDataRowHead
	{		 
		//this field is the version flag of the data row, this field position can not be change in the struct.
		FILETIME LastModifyTime;
		//this means the data row has been deleted.
		char  DeletedFlag;
		CacheDataRowHead()
		{
			DeletedFlag=0;
			LastModifyTime.dwHighDateTime=0;
			LastModifyTime.dwLowDateTime=0;			  
		}
	};     
  
	class CacheDataHeadHelper
	{
	public:
		static long CacheDataHeadSize;
		static long CacheDataRowHeadSize;
		static void readDataHead(ShareMemoryHelper* sharedMem, CacheDataHead& datahead)
		{
			sharedMem->ResetRWPoint();
			sharedMem->readStruct(sharedMem->getCurReadPos(), &datahead, CacheDataHeadSize);
		}
        static void writeDataHead(ShareMemoryHelper* sharedMem,CacheDataHead& datahead)
		{
			 sharedMem->ResetRWPoint();
			 sharedMem->writeStruct( sharedMem->getCurWritePos(), &datahead, CacheDataHeadSize);
		}
        static void readDataRowHead(ShareMemoryHelper* sharedMem,char* rPos,CacheDataRowHead& rowhead )
		{
			 sharedMem->readStruct(rPos, &rowhead,  CacheDataRowHeadSize);
		}
        static void writeDataRowHead(ShareMemoryHelper* sharedMem,char* wPos,CacheDataRowHead& rowhead)
		{
			 sharedMem->writeStruct(wPos, &rowhead, CacheDataRowHeadSize);
		}
        
		
        static void readDataRowTimeField(ShareMemoryHelper* sharedMem,char* rPos,  FILETIME& dt)
		{ 
            FILETIME data= sharedMem->readFileTime(rPos);
			dt.dwHighDateTime=data.dwHighDateTime;
			dt.dwLowDateTime=data.dwLowDateTime;
		}
        static void writeDataRowTimeField(ShareMemoryHelper* sharedMem,char* wPos,  FILETIME& dt)
		{
			 sharedMem->writeFileTime(wPos,dt);
		}


		static void getCurrentDateTime(FILETIME& dt)
		{
			SYSTEMTIME st;
			GetSystemTime(&st); 		 
			SystemTimeToFileTime(&st, &dt);     
		}

	private:
		CacheDataHeadHelper();
	};
// 	long CacheDataHeadHelper::CacheDataHeadSize=sizeof(CacheDataHead);
// 	long CacheDataHeadHelper::CacheDataRowHeadSize=sizeof(CacheDataRowHead);
}

#endif // !defined(AFX_CACHEDATAHEAD_H__C67D67F4_A328_46BC_8534_4CE3BDD07689__INCLUDED_)

// ShareMemoryHelper.h: interface for the ShareMemoryHelper class.
//
 /**
  * The source code in this file is the property of
  * ST Electronics and is not for redistribution
  * in any form.
  *
  * Source:   
  * @author:  Luohr
  * @version: $Revision: 1.0.0.1 $
  *
  * Last modification: $Date: 2009/07/04 0:39:58 $
  * Last modified by:  $Author: luohr $
  *
  */
#if !defined(AFX_SHAREMEMORYHELPERCache_H__AF8713B5_9631_43B5_92A1_D4D377599188__INCLUDED_)
#define AFX_SHAREMEMORYHELPERCache_H__AF8713B5_9631_43B5_92A1_D4D377599188__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include <string>
#include <windows.h>
namespace TA_Base_Bus
{
    enum  SharedMemoryDataSize
	{
		LongSharedMemoryData=sizeof(long),
		ULongSharedMemoryData=sizeof(unsigned long),
		DoubleSharedMemoryData=sizeof(double),
		BoolSharedMemoryData=sizeof(long),
		UShortSharedMemoryData=sizeof(unsigned short),
		UCharMemoryData=sizeof(unsigned char),
		FILETIMESharedMemoryData=sizeof(FILETIME)
	};

	class ShareMemoryHelper  
	{
	public:		
		ShareMemoryHelper();
		ShareMemoryHelper(std::string memoryName);
		virtual ~ShareMemoryHelper();
	
		bool openFile();
		bool createFile(long maxSize);
		bool mappingFile();
		bool writeStruct(char* wPos,void * data, long size);
        bool readStruct(char* rPos,void * data, long size);
		bool writeLong(char* wPos, long data);
		bool writeULong(char* wPos, unsigned long data);
		bool writeString(char* wPos,std::string data, long Length);
		bool writeDouble(char* wPos,double data);	
		bool writeFileTime(char* wPos,FILETIME& data);
		bool writeBool(char* wPos, bool data);
		bool writeChar(char* wPos, unsigned char* chr, long Length);
        bool writeUShort(char* wPos, unsigned short data);
        unsigned short readUShort(char* rPos);
		bool readChar(char*rPos, unsigned char* chr, long Length);
		FILETIME readFileTime(char* rPos);
		long readLong(char* rPos);
		unsigned long readULong(char* rPos);
		std::string  readString(char* rPos,long length);
		double readDouble(char* rPos);
		bool  readBool(char* rPos);
		bool Release();	
		char* getCurWritePos();
		char* getCurReadPos();
		char* getBasePos();
		
		void setMemoryName(std::string memoryName);
		void ResetRWPoint();		
        //static std::string DEFAULT_FILE_NAME;
	private:
		std::string m_memoryName; //name of shared memory 
		HANDLE  m_fileHandler; 
		char* m_baseAddress;
		char* m_readPosition;
		char* m_writePostion;
		unsigned long m_maxSize;
		
	};
}

#endif // !defined(AFX_SHAREMEMORYHELPER_H__AF8713B5_9631_43B5_92A1_D4D377599188__INCLUDED_)

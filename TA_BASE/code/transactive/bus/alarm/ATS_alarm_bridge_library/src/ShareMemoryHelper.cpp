// ShareMemoryHelper.cpp: implementation of the ShareMemoryHelper class.
//
//////////////////////////////////////////////////////////////////////
 
#include "ShareMemoryHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include <winnt.h>
using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	//std::string ShareMemoryHelper::DEFAULT_FILE_NAME="transactiveMonitor";
	ShareMemoryHelper::ShareMemoryHelper()
	{
		m_fileHandler=0;
		m_writePostion=NULL;
		m_readPosition=NULL;
		m_baseAddress=NULL;
	}
	void ShareMemoryHelper::setMemoryName(std::string memoryName)
	{
		m_memoryName=memoryName;
		
	}
	ShareMemoryHelper::ShareMemoryHelper(std::string memoryName)	
	{
		m_memoryName=memoryName;
		m_fileHandler=0;
		m_writePostion=NULL;
		m_readPosition=NULL;
		m_baseAddress=NULL;
	}
	ShareMemoryHelper::~ShareMemoryHelper()
	{
		Release();
	}
    bool ShareMemoryHelper::Release()
	{
		if (m_fileHandler==0)
		{
			return true;
		}
        if (m_baseAddress!=NULL)
		{
	 		if(UnmapViewOfFile(m_baseAddress))
			{
				m_baseAddress=NULL;
			}
			else
			{
				return FALSE;
			}
		}

       if (m_baseAddress==NULL && m_fileHandler!=0 )
	   {
			if(CloseHandle(m_fileHandler))
			{
				m_fileHandler=0;
			}
			else
			{
				return FALSE;
			}
	   }
	   return TRUE;
	}
    

	bool ShareMemoryHelper::openFile()
	{
		if(m_fileHandler!=0)
		{
			return TRUE;
		}
		HANDLE  fileH=OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, m_memoryName.c_str());
		if (fileH==0)
		{
			DWORD errorcode;
			errorcode= GetLastError();
			if (errorcode!=ERROR_FILE_NOT_FOUND)
			{
				LOG1(SourceInfo,DebugUtil::DebugError, "open Mem failed code=%u", errorcode );
			}			
			return FALSE;
		}		   
		m_fileHandler=fileH;
        return TRUE;
	}
	bool ShareMemoryHelper::createFile(long maxSize)
	{
        if(m_fileHandler!=0)  
		{
			return TRUE;
		}		

		SECURITY_DESCRIPTOR sd;
		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) 
		{
			DWORD errorcode;
			errorcode= GetLastError();
			LOG1(SourceInfo,DebugUtil::DebugError, "InitializeSecurityDescriptor failed code=%u", errorcode );
			return FALSE;
		};
		if (!SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE))
		{
			DWORD errorcode;
			errorcode= GetLastError();
			LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );
			return FALSE;

		};		
        SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, TRUE };
 
		HANDLE  fileH= CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0, maxSize, m_memoryName.c_str());
		if (fileH==0)
		{
			DWORD errorcode;
			errorcode= GetLastError();
			LOG1(SourceInfo,DebugUtil::DebugError, "create Mem failed code=%u", errorcode );
			return FALSE;
		}		   
		m_fileHandler=fileH;
		m_maxSize=maxSize;
        return TRUE;	 
	}
	void ShareMemoryHelper::ResetRWPoint()
	{
	 
		m_writePostion=m_baseAddress;
		m_readPosition=m_baseAddress;
	}
	bool ShareMemoryHelper::mappingFile()
	{	 
        if(m_fileHandler==0)  
		{
			return FALSE;
		}		
        if (m_baseAddress!=NULL)
		{
			return TRUE;
		}
		char* buf = (LPTSTR)MapViewOfFile(m_fileHandler, FILE_MAP_ALL_ACCESS, 0, 0, 0);		
		if (buf == NULL) 
		{ 
			return FALSE;
		}
		m_baseAddress=buf;
		m_writePostion=buf;
		m_readPosition=buf;
		return TRUE;
	}
	bool ShareMemoryHelper::writeLong(char* wPos,long data)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
        if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		 
		long size=sizeof(data);
		CopyMemory(wPos, &data,size);	 
		m_writePostion=wPos+size;
		return TRUE;
	}

	bool ShareMemoryHelper::writeStruct(char* wPos,void * data, long size)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}
		CopyMemory(wPos, data,size);
        m_writePostion=wPos+size;
		return TRUE;
	}

    bool ShareMemoryHelper::readStruct(char* rPos,void * data, long size)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		CopyMemory(data, rPos,size);
		m_readPosition=rPos+size;
		return TRUE;
	}

	bool ShareMemoryHelper::writeChar(char* wPos, unsigned char* chr, long Length)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		CopyMemory(wPos, chr,Length);	 
		m_writePostion=wPos+Length;
		return TRUE;
	}
	bool ShareMemoryHelper::readChar(char*rPos, unsigned char* chr, long Length)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		CopyMemory(chr, rPos,Length);	 
		m_readPosition=rPos+Length;
		return TRUE;

	}

	bool ShareMemoryHelper::writeBool(char* wPos,bool data)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
        long lngData=0;
		if (data)
		{
		    lngData=1;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		long size=sizeof(lngData);
		CopyMemory(wPos, &lngData,size);	 
		m_writePostion=wPos+size;
		return TRUE;
	}
	bool ShareMemoryHelper::readBool(char* rPos)
	{
		if (m_baseAddress==NULL)
		{
			return 0;
		}		 
		long data=0; 
		long size=sizeof(data);
		CopyMemory(&data,rPos,size);	 
		m_readPosition=rPos+size;
		if (data==0)
		{
		    return FALSE;
		}
		return TRUE;
	}

	bool ShareMemoryHelper::writeFileTime(char* wPos,FILETIME& data)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}	 
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

        long size=sizeof(FILETIME); 
        CopyMemory(wPos, &data,size);	 
		m_writePostion=wPos+size;
		return TRUE;
	}
	FILETIME ShareMemoryHelper::readFileTime(char* rPos)
	{
		FILETIME data; 
		data.dwLowDateTime=0;
		data.dwHighDateTime=0;
		
		if (m_baseAddress==NULL)
		{
			return data;
		}		
        long size=sizeof(FILETIME);
 		CopyMemory(&data,rPos,size);
 		m_readPosition=rPos+size;
		return data;
	}
	
	bool ShareMemoryHelper::writeDouble(char* wPos,double data)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		long size=sizeof(data);
		CopyMemory(wPos, &data,size);	 
		m_writePostion=wPos+size;
		return TRUE;
	}
	long ShareMemoryHelper::readLong(char* rPos)
	{
		if (m_baseAddress==NULL)
		{
			return 0;
		}
		long data=0; 
		long size=sizeof(data);
 		CopyMemory(&data,rPos,size); 	 
		m_readPosition=rPos+size;
		return data;
	}

	bool ShareMemoryHelper::writeUShort(char* wPos, unsigned short data)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		long size=sizeof(data);
		CopyMemory(wPos, &data,size);	 
		m_writePostion=wPos+size;
		return TRUE;

	}
    unsigned short ShareMemoryHelper::readUShort(char* rPos)
	{
		if (m_baseAddress==NULL)
		{
			return 0;
		}
		unsigned short data=0; 
		long size=sizeof(data);
		CopyMemory(&data,rPos,size); 	 
		m_readPosition=rPos+size;
		return data;
	}
	bool ShareMemoryHelper::writeString(char* wPos,std::string data, long Length)
	{
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}	
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		long size=data.size();
		if (size>(Length-LongSharedMemoryData))
		{
			size=Length-LongSharedMemoryData;
			LOG2(SourceInfo,DebugUtil::DebugError, "Data length problem R=%u A=%u", size, Length);
		}	 		 
		writeLong(wPos,size);
		CopyMemory(m_writePostion,data.c_str(),size);
		m_writePostion=wPos+Length;
		//m_writePostion=wPos;
		return TRUE;
	}
	std::string ShareMemoryHelper::readString(char* rPos,long length)
	{
		if (m_baseAddress==NULL)
		{
			return "";
		}
        long size=readLong(rPos);
		if (size>(length-LongSharedMemoryData) || size<0)
		{
			m_readPosition=rPos+length;
			LOG2(SourceInfo,DebugUtil::DebugError, "read length problem R=%u A=%u", size, length);
			return "";
		}
		if (size==0)
		{
			m_readPosition=rPos+length;
			return "";
		}
		char* arr=new char[size+1];		 
		memset(arr,0,size+1);
		    
 		CopyMemory(arr, m_readPosition ,size);
		m_readPosition=rPos+length;	
		std::string str(arr);	 
		delete[] arr;
		return str;
	}
	double ShareMemoryHelper::readDouble(char* rPos)
	{
		if (m_baseAddress==NULL)
		{
			return 0;
		}
		double data=0; 
		long size=sizeof(data);
		CopyMemory(&data,rPos,size);		 
		m_readPosition=rPos+size;
		return data;
	}
	char* ShareMemoryHelper::getCurReadPos()
	{
        return m_readPosition;
	}
	char* ShareMemoryHelper::getCurWritePos()
	{
        return m_writePostion;
	}
	char* ShareMemoryHelper::getBasePos()
	{
		return m_baseAddress;
	}


	bool ShareMemoryHelper::writeULong(char *wPos, unsigned long data)
	{		 
		if (m_baseAddress==NULL)
		{
			return FALSE;
		}
		if (m_baseAddress+m_maxSize<wPos)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"out of memory");
		}

		long size=sizeof(data);
		CopyMemory(wPos, &data,size);	 
		m_writePostion=wPos+size;
		return TRUE;
	}
	unsigned long ShareMemoryHelper::readULong(char* rPos)
	{
		if (m_baseAddress==NULL)
		{
			return 0;
		}
		unsigned long data=0; 
		long size=sizeof(data);
		CopyMemory(&data,rPos,size);	 
		m_readPosition=rPos+size;
		return data;
	}
}



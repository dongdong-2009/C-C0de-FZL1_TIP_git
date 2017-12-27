// Buffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include <string>

#include "CommonDef.h"
#include "Buffer.h"

NS_BEGIN(TA_DB_Sync)

// namespace TA_DB_Sync
// {	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffer::CBuffer(int nsize)
{
	m_nSize = 0;			
	m_nLimit = 0;			
	m_pBuffer = NULL;			
	m_pWritePos = NULL;		
	m_pReadPos = NULL;	
	
	m_nLimit = BUFFER_LEN_MAX;//268M

	if (nsize > m_nLimit) 
	{
		nsize = BUFFER_LEN_READ;
	}
	m_pBuffer = new char[nsize];
	//ZeroMemory(ptr, nsize);
	memset(m_pBuffer, 0, nsize);

	m_pWritePos = m_pBuffer;
	m_pReadPos = m_pBuffer;
	m_nSize = nsize;
}

CBuffer::~CBuffer()
{
	if (m_pBuffer) 
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	
	m_nSize = 0;			
	m_nLimit = 0;			
	m_pBuffer = NULL;			
	m_pWritePos = NULL;		
	m_pReadPos = NULL;
}


char* CBuffer::getWritePos() const 
{
	return m_pWritePos; 
}

char* CBuffer::getReadPos() const 
{
	return m_pReadPos; 
}

int CBuffer::remainDataLength() const 
{
	return m_pWritePos - m_pReadPos; 
}
int CBuffer::getBlankSize() const 
{
	return m_pBuffer + m_nSize - m_pWritePos; 
}
int CBuffer::getLimit() const 
{
	return m_nLimit; 
}
int CBuffer::getSize() const 
{ 
	return m_nSize; 
}
//////////////////////////////////////////////////////////////////////////

bool CBuffer::write(const void* src, int len)
{
	char* pPosTmp = NULL;
	const char* byteaddr = (const char*)src;
	if (len <= 0)	
	{
		return false;
	}
	
	pPosTmp = m_pWritePos + len;
	if (pPosTmp > (m_pBuffer + m_nSize))
	{
		return false;
	}

	memcpy(m_pWritePos, byteaddr, len);
	m_pWritePos = pPosTmp;
	
	pPosTmp = NULL;
	return true;
}

bool CBuffer::read(void* dest, int len)
{
	char* pPosTmp = NULL;

	if (len <= 0)
	{
		return false;
	}
	
	pPosTmp = m_pReadPos + len;
	if (pPosTmp > m_pWritePos)
	{
		return false;
	}

	memcpy(dest, m_pReadPos, len);
	m_pReadPos = pPosTmp;
	
	pPosTmp = NULL;
	return true;
}

bool CBuffer::moveWritePos(int len)
{
	char* pPosTmp = NULL;
	
	pPosTmp = m_pWritePos + len;
	
	if (pPosTmp < m_pReadPos)
	{
		return false;
	}
	
	if (pPosTmp > (m_pBuffer + m_nSize))	
	{
		return false;
	}
	m_pWritePos = pPosTmp;
	
	pPosTmp = NULL;
	return true;
}

bool CBuffer::moveReadPos(int len)
{
	char* pPosTmp = NULL;

	pPosTmp = m_pReadPos + len;
	if (pPosTmp < m_pBuffer)
	{
		return false;
	}
	
	if (pPosTmp > m_pWritePos)	
	{
		return false;
	}
	m_pReadPos = pPosTmp;
	
	pPosTmp = NULL;
	return true;
}

void CBuffer::clear()		
{
	m_pWritePos = m_pBuffer;
	m_pReadPos = m_pBuffer;
}

void CBuffer::trim()		
{
	int nDataLen = m_pWritePos - m_pReadPos;
	if (nDataLen > 0)
	{
		memcpy(m_pBuffer, m_pReadPos, nDataLen);
	}
	m_pReadPos = m_pBuffer;
	m_pWritePos = m_pBuffer + nDataLen;
}


//////////////////////////////////////////////////////////////////////////

bool CBuffer::resize(int nsize)
{
	int nDataLen = 0;
	char* pszTmp = NULL;

	if (nsize > m_nLimit)
	{
		nsize = m_nLimit;
	}
	
	nDataLen = m_pWritePos - m_pReadPos;
	if (nDataLen >= nsize)
	{
		return false;
	}
	try
	{
		pszTmp = new char[nsize];
		memset(pszTmp, 0, nsize);
		memcpy(pszTmp, m_pReadPos, nDataLen);
		delete[] m_pBuffer;
		m_pBuffer = NULL;

		m_pBuffer = pszTmp;
		pszTmp = NULL;
		m_pReadPos = m_pBuffer;
		m_pWritePos = m_pBuffer + nDataLen;
		m_nSize = nsize;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CBuffer::resizeFitData()
{
	char* pszTmp = NULL;

	int nDataLen = m_pWritePos - m_pReadPos;
	int nBufferSize = nDataLen + BUFFER_LEN_ADDEX;
	try
	{
		pszTmp = new char[nBufferSize];
		memset(pszTmp, 0, nBufferSize);
		if (nDataLen < nBufferSize)
		{
			memcpy(pszTmp, m_pReadPos, nDataLen);
		}
		delete[] m_pBuffer;
		m_pBuffer = NULL;

		m_pBuffer = pszTmp;
		pszTmp = NULL;
		m_pReadPos = m_pBuffer;
		m_pWritePos = m_pBuffer + nDataLen;
		m_nSize = nBufferSize;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void CBuffer::bufferSizePrepare( int nsize )
{
// 	if (this->getSize() > BUFFER_LEN_READ)
// 	{
// 		this->resizeFitData();
// 	}
// 	else
// 	{
// 		this->trim();
// 	}
// 

	if (this->getBlankSize() < nsize)
	{
		this->resize(this->remainDataLength() + nsize + BUFFER_LEN_ADDEX);
	}

}

//}//namespace TA_TEST_TCP_CS

NS_END(TA_DB_Sync)






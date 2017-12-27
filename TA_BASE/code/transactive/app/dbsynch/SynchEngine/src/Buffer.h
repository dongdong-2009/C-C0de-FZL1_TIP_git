// Buffer.h: interface for the CBuffer class.

#if !defined(__SOCKET_BUFFER_H__)
#define __SOCKET_BUFFER_H__

#include "CommonDef.h"


NS_BEGIN(TA_DB_Sync)




class CBuffer  
{
private:
	int		m_nSize;			
	int		m_nLimit;			
	char*	m_pBuffer;			
	char*	m_pWritePos;		
	char*	m_pReadPos;		
public:
	CBuffer(int nsize = BUFFER_LEN_READ);
	~CBuffer();
public:
	char* getWritePos() const; 
	char* getReadPos() const;
	int remainDataLength() const;
	int getBlankSize() const;
	int getSize() const;
	int getLimit() const;

public:
	bool write(const void* src, int len);
	bool moveWritePos(int len);

	bool read(void* dest, int len);
	bool moveReadPos(int len);

	void clear();		
	void trim();		

	bool resize(int nsize);
	bool resizeFitData();
	void bufferSizePrepare(int nsize);	

};


NS_END(TA_DB_Sync)


#endif //__SOCKET_BUFFER_H__

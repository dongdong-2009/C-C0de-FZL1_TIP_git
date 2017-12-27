// IecSocket.cpp: implementation of the CIecSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "IecSocket.h"
#include "Iec104Api.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#ifdef  _WIN32

#else
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#endif
#include "FepDataStruct.h"

#include "FtpException.h"
using namespace std;

bool CIecSocket::mInitDoneStatic = false;
int CIecSocket::mhInstance = 0;
int CIecSocket::mSelectTimeout= 1;
int CIecSocket::mSelectRetries = 1;
int CIecSocket::mReadRetries = 0;
CIecSocket::CIecSocket()
{
	//sendTimes=0;
}

CIecSocket::~CIecSocket()
{

}


void CIecSocket::init()
{

    return;
}

void CIecSocket::Create()
{
	init();
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) 
	{ 
		ThrowError(CFtpException::TCPIP_ERROR,"socket", errno);
    }    

	char tmp[100];
	std::string logStr;
	sprintf(tmp,"Socket %d create",mSocket);
	logStr+=tmp;
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_INFO, logStr);

	return;
}

void CIecSocket::LogDebug(const char* fmt, ...)
{
	return ;
	va_list ap;
	va_start(ap, fmt);
	char buffer[512];
	if (!mDebug) return;
#ifdef _WIN32
	_vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
#else
	vsprintf(buffer, fmt, ap);
#endif 
	va_end(ap);
	buffer[sizeof(buffer)-1] = '\0';
	(*mLogCallback)(buffer);
}

int CIecSocket::Connect(const std::string& hostAddr, int hostPort)
{
	struct sockaddr_in serverAddr;
	mConnected = false;
	mHost = hostAddr;
	mAddr = inet_addr(hostAddr.c_str());
	if (mAddr == INADDR_NONE) 
	{
		/* Not a valid internet address, assume it is a node name */
		struct hostent* hp = gethostbyname(hostAddr.c_str());
		if (hp == NULL) 
		{
           		 ThrowError(CFtpException::TCPIP_ERROR,"inet_addr", errno);
		}
		else 
		{
			mAddr = *((unsigned long*) hp->h_addr_list[0]);    
		}
	}
	serverAddr.sin_port=htons(hostPort);
	serverAddr.sin_addr.s_addr = mAddr;   
	serverAddr.sin_family = AF_INET;
#ifdef _WIN32
	fd_set rset, wset; 
	int error,ret;
	struct timeval tval;
	unsigned long ul = 1;
    ret= ioctlsocket(mSocket, FIONBIO, (unsigned long*)&ul);
	if (ret == SOCKET_ERROR)
		return -1;
	ret=connect(mSocket, (struct sockaddr*) &serverAddr,sizeof(serverAddr));
	if(ret==0)
		return 0;
       FD_ZERO(&rset); 
       FD_SET(mSocket, &rset); 
       wset = rset;  // do block copy 
       tval.tv_sec = 0; 
       tval.tv_usec = 500000; 
     
       if ((ret= select(mSocket+1, &rset, &wset, NULL,&tval)) == 0) 
	{ 
             this->Close(true); 
              errno =-1;//ETIMEOUT; 
              return (-1); 
       } 
	 else if(ret<0)
	 {
		return -1;
	 }
	 else
	 {
		if(FD_ISSET(mSocket, &rset) || FD_ISSET(mSocket, &wset)) 
		 { 
                   int  len = sizeof(error); 
                    if (getsockopt(mSocket, SOL_SOCKET, SO_ERROR,(char *) &error, (socklen_t*)&len) < 0) 
                           return (-1); 
		 }
	  }
	if (error) 
	{ 
		this->Close(true); 

              errno = error; 

              return (-1); 

       } 
       return (0); 


#else
	fd_set rset, wset; 
	int error=0,len;
	struct timeval tval;
	int flags = fcntl(mSocket, F_GETFL, 0); 
	fcntl(mSocket, F_SETFL, flags | O_NONBLOCK); 
	int n = connect(mSocket, (struct sockaddr*) &serverAddr,sizeof(serverAddr));
	
	if ( n < 0)
	{
		char tmp[100];
		std::string logStr;
		sprintf(tmp,"Socket:%d connect:%d errno:%d",mSocket, n, errno);
		logStr += tmp;
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_INFO, logStr);
		
		if (errno != EINPROGRESS) 
		{
			Close(false); 
            return (-1);
		}
	}
	else if( n == 0)
	{
		goto done; // be called by one host, usually return 0
	}

	FD_ZERO(&rset); 
	FD_SET(mSocket, &rset); 
	wset = rset;  //do  block copy 
	tval.tv_sec = 0; 
	tval.tv_usec = 800000; 
	 
	int m; 
    m = select(mSocket+1, &rset, &wset, NULL,&tval);
	
	if( m <= 0 )
	{
		char tmp[100];
		std::string logStr;
		sprintf(tmp,"Socket:%d select:%d errno:%d",mSocket, m, errno);
		logStr += tmp;
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_INFO, logStr);
	}
	
	if ( m == 0 )
	{ 
		Close(false); 
		errno = -1;//ETIMEOUT; 
		return (-1); 
	} 
	else if( m < 0 )
	{	
		Close(false); 
		return -1;
	}
	else  
	{
		if(FD_ISSET(mSocket, &rset) || FD_ISSET(mSocket, &wset))  
		{ 
			len = sizeof(error); 
			if (getsockopt(mSocket, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len) < 0) 
			{
				return (-1); 
			}
		 } 
	}
     

 done: 

       fcntl(mSocket, F_SETFL, flags); //resume the attributs of socket 

       if (error) 
	   { 
              close(mSocket); 
              errno = error; 
              return (-1); 

       } 

       return (0); 
#endif

} 



void CIecSocket::ThrowError(CFtpException::ExceptType exceptType, const char* operation, int err)
{
    int errNum;    
	
	if ((exceptType == CFtpException::TCPIP_ERROR) ||
		(exceptType == CFtpException::NO_CONNECTION))
	{
#ifdef _WIN32
		errNum = WSAGetLastError(); 
		
#else 
		errNum = err;
#endif
	}
	else
	{
		errNum = 0;
	}
	Close(true);
	throw CFtpException(exceptType, errNum, operation);
}

void CIecSocket::Close(bool ignoreError)
{
	int oldsocket;
#ifdef _WIN32
	int status = shutdown(mSocket, SD_BOTH);
	Sleep(100);
	status = closesocket(mSocket);
#else
    int status = close(mSocket);
#endif
//for debug
	char tmp[100];
	std::string logStr;
	sprintf(tmp,"Socket %d closed",mSocket);
	logStr+=tmp;
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_INFO, logStr);
//for debug end	
	oldsocket = mSocket;
	mSocket = -1;
	mConnected = false;
	if ((status != 0) && (!ignoreError))
	{
       	ThrowError(CFtpException::TCPIP_ERROR, "close", errno);
	}
	LogDebug("Socket %d closed", oldsocket);
}

void CIecSocket::Accept(CIecSocket &connectedSocket)
{
    int newSocket;
    struct sockaddr_in accSin;
	int status;
//#if defined(__unix__) || defined(_WIN32)
#if  defined(_WIN32)
    socklen_t accSinLen;
#else
    unsigned int accSinLen;
#endif
    char *pHostAddr;
    
	// Wait until we have an incoming connection waiting
	status = Select(READ_SELECT);
	if (status<0) 
	{
		ThrowError(CFtpException::TIMEOUT_ERROR, "accept", errno);
	}
    accSinLen = sizeof(accSin);
    newSocket = accept(mSocket, (struct sockaddr *)&accSin, &accSinLen);
    if (newSocket < 0) 
    {
        ThrowError(CFtpException::TCPIP_ERROR, "accept", errno);
    }
    connectedSocket.mSocket = newSocket;
    pHostAddr = inet_ntoa(accSin.sin_addr);
    connectedSocket.mHost = pHostAddr;
	connectedSocket.SetDebug(mDebug, mLogCallback);
	LogDebug("accepted new connection on socket %d", newSocket);
}

void CIecSocket::SetDebug(bool status, void ( *logCallback)(const char *))
{
	mDebug = status;
	mLogCallback = logCallback;
}

void CIecSocket::Bind(int port, std::string &localAddr)
{
 	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
 	sin.sin_family = AF_INET;
 	sin.sin_port = htons(port);
 	sin.sin_addr.s_addr = inet_addr(localAddr.c_str());
 	int status = bind(mSocket, (struct sockaddr *) &sin, sizeof(sin));
 	if (status != 0) 
 	{
 		ThrowError(CFtpException::TCPIP_ERROR, "bind", errno);
	}
	return ;
}

int CIecSocket::Select(SelectType selectType)
{
	//struct timeval timeoutVal = {mSelectTimeout, 0};
	struct timeval timeoutVal = {0, 100};
    int status;
    fd_set fds;
	
	for (int i = 0; i < mSelectRetries; i++ )
	{
		memset(&fds, 0, sizeof(fds));
		FD_SET((unsigned int)mSocket, &fds);
		
		if (selectType == READ_SELECT)
		{
			status = select(FD_SETSIZE, &fds, 0, 0, &timeoutVal);
		}
		else
		{
			status = select(FD_SETSIZE, 0, &fds, 0, &timeoutVal);
		}
		if (status > 0) 
		{
			if (FD_ISSET(mSocket, &fds)) 
			{
				return(status);
			}
			else 
			{
				char tmpBuffer[100];
				sprintf(tmpBuffer,"select %d on socket %d, not FD_ISSET",status, mSocket);
				std::string logStr = tmpBuffer;
				IEC104_API*api=IEC104_API::getInstance();
				api->logRecord(LOG_LEVEL_DEBUG, logStr);
			}
		}
		LogDebug("select timeout on socket=%d, status=%d, attempt=%d", 
			mSocket, status, i);
	}
    return(status);
}

int CIecSocket::Receive(char *buffer, int length)
{
	IEC104_API*api=IEC104_API::getInstance();
	std::string logStr;
	char tmp[200];
	int error,len=sizeof(error);
	int status = Select(READ_SELECT);
	int cnt;
	if(status>0)//can read
	{
		for(int i=0;i<mReadRetries+1;i++)
		{
			sprintf(tmp,"read times %d ",i+1);
			logStr=tmp;
			api->logRecord(LOG_LEVEL_DEBUG, logStr);
			cnt =  recv(mSocket,(char*) buffer, length, 0); 
			if(cnt>0)
				return cnt;
#ifdef _WIN32
			errno =  WSAGetLastError();
#endif
			sprintf(tmp,"socket recv return %d,errno %d, ",cnt,errno);
			logStr=tmp;
#ifdef _WIN32
			if (getsockopt(mSocket, SOL_SOCKET, SO_ERROR,(char *) &error, (socklen_t*)&len) < 0) 
#else
			if (getsockopt(mSocket, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len) < 0) 
#endif
			{
#ifdef _WIN32
			errno =  WSAGetLastError();
#endif
				sprintf(tmp,"get socket error fail,errno %d\n",errno);
				logStr+=tmp;
				api->logRecord(LOG_LEVEL_WARNING, logStr);
				return -1;
			}
			sprintf(tmp,"Socket error %d",error);
			logStr+=tmp;
			api->logRecord(LOG_LEVEL_WARNING, logStr);

		}
		return -1;	
	}
	else if(status == 0)//time out
	{
		return 0;
	}
	else
	{
#ifdef WIN32//
		if(WSAGetLastError() == WSAEINTR)
#else
		if(errno == EINTR)
#endif
			return 0;//interupt
			
			return -1;//error
	}
}

int CIecSocket::Send(const char *buffer, int length)
{

	int bytesLeft = length;
	const char* bufferPtr = buffer;	
	int cnt=0;	
	while (bytesLeft > 0)
	{
		int status = Select(WRITE_SELECT);
		if (status<0)
		{
			ThrowError(CFtpException::TIMEOUT_ERROR, "send", errno);
		}
		cnt = send(mSocket, bufferPtr, bytesLeft, 0);
		if(cnt<0)
			return cnt;
		bytesLeft -= cnt;
		bufferPtr += cnt;
	}
	return cnt;
	
	LogDebug("%d bytes sent on socket %d", length, mSocket);
}

int CIecSocket::GetLocalAddr()
{
#ifdef __VMS
	size_t len;
#else
	socklen_t len;
#endif
	struct sockaddr_in sin;
	len = sizeof(sin);
	int status = getsockname(mSocket, (struct sockaddr*) &sin, &len);
	if (status < 0)
	{
		ThrowError(CFtpException::TCPIP_ERROR, "getsockname", errno);
	}
	return(sin.sin_addr.s_addr);
}

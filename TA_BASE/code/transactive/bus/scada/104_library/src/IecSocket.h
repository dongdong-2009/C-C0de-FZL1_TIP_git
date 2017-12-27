#ifndef __IECSOCKET_H__
#define __IECSOCKET_H__

#include <string>
#include <vector>
#include "FtpException.h"


class CIecSocket  
{
public:
	int  GetLocalAddr();
	int Send(const char* buffer, int length);
	int  Receive(char* buffer, int length);
	void Bind(int port, std::string &localAddr);
	void SetDebug(bool status, void (*logCallback)(const char* msg));
	void Accept(CIecSocket& connectedSocket);
	void Close(bool ignoreError);
	int  Connect(const std::string& hostAddr, int hostPort);
	void Create();
	void init();
	CIecSocket();
	virtual ~CIecSocket();
	static void setReadRetries(int times)
	{	
		mReadRetries=times;
	};
	
private:
	// Functions
    typedef enum {
		READ_SELECT,
			WRITE_SELECT
	} SelectType;
	int Select(SelectType selectType);
    void ThrowError(CFtpException::ExceptType, const char* = "", int err = 0);
    void LogDebug(const char* fmt, ...);
	//int sendTimes;
    // Variables
	 int mSocket;
	int mAddr;
    bool mConnected;
    std::string mHost;
    std::string mLineBuffer;
	static bool mInitDoneStatic;
	static int mhInstance;
	static int mSelectTimeout;
	static int mSelectRetries;
	static int mReadRetries;
	bool mDebug;
	void (*mLogCallback)(const char* msg) ; 
};

#endif

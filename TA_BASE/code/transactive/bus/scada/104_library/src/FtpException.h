

#ifndef FTPCLIENT_FTPEXCEPTION_h
#define FTPCLIENT_FTPEXCEPTION_h

#ifdef WIN32
#pragma warning(disable : 4786)
//#ifdef FTPCLIENT_EXPORTS
//#define FTPCLIENT_API __declspec(dllexport)
//#else
//#define FTPCLIENT_API __declspec(dllimport)
//#endif
//#else
//#define FTPCLIENT_API
#endif

#include <string>

class  CFtpException
{
public:
	typedef enum
	{
		NO_CONNECTION,
		CONNECTION_LOST,
		TCPIP_ERROR,
		SERVER_ERROR,
		LOCAL_ERROR,
		TIMEOUT_ERROR
	} ExceptType;
    CFtpException(ExceptType status, 
				 int sysStatus = 0, 
				 const char* operation = "",
				 const char* sysText = "");
    ~CFtpException();
    ExceptType Status() const {return(mStatus);};
	const char* StatusText();
	int SysStatus() const {return(mSysStatus);};
	const char* Operation() const {return(mOperation);};
    const char* SysText();
    CFtpException& operator=(const CFtpException& rhs); 
    CFtpException(const CFtpException& exception);
	static void SetInstance(void* instance) {mhInstance = instance;};

protected:
    ExceptType mStatus;   // exception type
	int mSysStatus;       // operating system status (when applicable)
	char mOperation[132]; // operation being performed
	char mSysText[512];   // text associated with operating system status
	static void* mhInstance;
private:
};
#endif // Ftp_Exception_h

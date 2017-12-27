#ifdef WIN32
#pragma warning(disable : 4786)
#include "windows.h"
#include "winsock.h"
#endif

#include "FtpException.h"

void * CFtpException::mhInstance;
using namespace std;

CFtpException::CFtpException(ExceptType status, 
							 int sysStatus, 
							 const char* operation,
							 const char* sysText)
                : mStatus(status), mSysStatus(sysStatus)

{
	strncpy(mOperation, operation, sizeof(mOperation));
	mOperation[sizeof(mOperation)-1] = '\0';
	strncpy(mSysText, sysText, sizeof(mSysText));
	mSysText[sizeof(mSysText)-1] = '\0';
}
CFtpException::~CFtpException()
{
}

CFtpException& CFtpException::operator=(const CFtpException& rhs)
/*
**++
**  assignment operator
**
**  Parameters:     
**		rhs			right hand argument
**
**  Return value:   none
**
**  Description:
**
**  dd-mmm-yy init  SPR#      comments
**  --------- ----  -------   ---------------------------------------------------
**  06-jun-00 jeo             Created.
**--
*/ 
{
    mStatus = rhs.mStatus;
	mSysStatus = rhs.mSysStatus;
    strcpy(mSysText, rhs.mSysText);
	strcpy(mOperation, rhs.mOperation);
    return *this;
}

CFtpException::CFtpException(const CFtpException& exception)
                : mStatus(exception.mStatus),
				  mSysStatus(exception.mSysStatus)
/*
**++
**  Copy constructor
**
**  Parameters:     
**		exception	existing exception object
**
**  Return value:   none
**
**  Description:
**
**  dd-mmm-yy init  SPR#      comments
**  --------- ----  -------   ---------------------------------------------------
**  06-jun-00 jeo             Created.
**--
*/ 
{
	strcpy(mSysText, exception.mSysText);
	strcpy(mOperation, exception.mOperation);
}

const char* CFtpException::StatusText() 
/*
**++
**  Get text associated with the exception type
**
**  Parameters:     none
**
**  Return value:   none
**
**  Description:
**	Return text based on the exception type
**
**  dd-mmm-yy init  SPR#      comments
**  --------- ----  -------   ---------------------------------------------------
**  11-apr-01 jeo             Created.
**--
*/ 
{
	switch (mStatus) 
	{
		case NO_CONNECTION:
			return("NO CONNECTION");
			break;
		case CONNECTION_LOST:
			return("CONNECTION LOST");
			break;
		case TCPIP_ERROR:
			return("TCPIP ERROR");
			break;
		case SERVER_ERROR:
			return("SERVER ERROR");
			break;
		case LOCAL_ERROR:
			return("LOCAL ERROR");
			break;
		case TIMEOUT_ERROR:
			return("TIMEOUT ERROR");
			break;
		default:
			return("UNKNOWN");
			break;
	}
}

const char* CFtpException::SysText()
/*
**++
**  Get text associated with operating system status
**
**  Parameters:     none
**
**  Return value:   none
**
**  Description:
**	If a Windows Socket error, then get the text from the resource file. Otherwise,
**	call strerror to get the text.
**
**  dd-mmm-yy init  SPR#      comments
**  --------- ----  -------   ---------------------------------------------------
**  06-jun-00 jeo             Created.
**--
*/ 
{
	if (mStatus != SERVER_ERROR)
	{
#ifdef _WIN32
		if (mSysStatus >= WSABASEERR)
		{
			int len = LoadString((HINSTANCE) mhInstance, mSysStatus, mSysText, sizeof(mSysText));
		}
		else
#endif
		{	
			char* errText = strerror(mSysStatus);
			strncpy(mSysText, errText, sizeof(mSysText));
			mSysText[sizeof(mSysText)-1] = '\0';
		}
	}
	for (int i = strlen(mSysText)-1; (i > 0) && (mSysText[i] == ' '); i--)
	{
		mSysText[i] = '\0';
	}

	strcat(mSysText, " (");
	strcat(mSysText, mOperation);
	strcat(mSysText, ")");
	return(mSysText);
}

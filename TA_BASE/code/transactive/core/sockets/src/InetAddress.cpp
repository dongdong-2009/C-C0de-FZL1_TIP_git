/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/InetAddress.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#include "InetAddress.h"

#ifdef SOLARIS
#define INADDR_NONE ((unsigned int) -1)
#endif

// this include should not be necessary...  Unfortunately there is a circular include
// thing going on with DebugUtil.h and TransactiveException.h which could
// be solved with a forward declaration but one for later(?)
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

InetAddress::InetAddress()
{
	m_doResolve = false;
	m_isResolved = false;
	m_hostname = "";
	m_address.s_addr = INADDR_NONE;
}

InetAddress::InetAddress(const char* host)
{
	m_hostname = host;
	m_doResolve = true;
	m_isResolved = false;
	m_address.s_addr = INADDR_NONE;
}

InetAddress::InetAddress(const std::string &host)
{
	m_hostname = host;
	m_doResolve = true;
	m_isResolved = false;
	m_address.s_addr = INADDR_NONE;
}

InetAddress::InetAddress(const in_addr& address)
{
	m_doResolve = false;
	m_isResolved = true;
	m_address = address;

	m_hostname = inet_ntoa(address);
}

InetAddress::InetAddress(const InetAddress &copy)
{
	// will do for now until something exciting happens...
	*this = copy;
}

bool
InetAddress::isValid()
{
	if (m_doResolve)
	{
		getIPAddress();
	}

	return m_isResolved;
}

const std::string&
InetAddress::getHostName()
{
	return m_hostname;
}

const in_addr &
InetAddress::getIPAddress()
{
	// name resolution is expensive so only do it once.
	if (m_doResolve)
	{
//		#ifdef WIN32
//        if (WSAStartup( WINSOCK_VERSION, &winsockData) != 0)
//        {
//            TA_THROW(TA_Base_Core::SocketException("Couldn't initialise Windows sockets"));
//        }
//		#endif // !WIN32

		//
		// Determine if we already have a dot notation address.
		//
		#if defined( LINUX ) || defined( FREEBSD )

		m_address.s_addr = inet_addr(m_hostname.c_str());
		if (m_address.s_addr ==  INADDR_NONE)

		#else // !defined( LINUX ) && !defined( FREEBSD )

		m_address.s_addr = inet_addr(m_hostname.c_str());

		#if defined( SOLARIS )

		if (m_address.S_un.S_addr ==  (unsigned int)-1)

		#else // !defined( SOLARIS )

		if (m_address.s_addr ==  INADDR_NONE)

		#endif // !defined( SOLARIS )
		#endif // !defined( LINUX )
		{
			// now must invoke gethostbyname...
			char lvHostBuffer[1024];
			memset(lvHostBuffer, 0, sizeof(lvHostBuffer));

			#if defined( SOLARIS )
			struct hostent lvHostData;
			const struct hostent* lvHost = &lvHostData;
			int lvError;
			if( 0 == gethostbyname_r(m_hostname.c_str(), & lvHostData, lvHostBuffer,
							  sizeof( lvHostBuffer ), & lvError ) )
			#else // !defined( SOLARIS )
			struct hostent* lvHost;
			lvHost = gethostbyname(m_hostname.c_str());
			if (0 == lvHost)
			#endif // !defined( SOLARIS )
			{
				m_isResolved = false;
			}
			else
			{
				// copy first entry in addr_list
				struct in_addr * lvAddress = * ( struct in_addr ** )lvHost->h_addr_list;
				memcpy(&m_address, lvAddress, sizeof(struct in_addr));
				m_isResolved = true;
			}
		}
		else
		{
			m_isResolved = true;
		}

//		#ifdef WIN32
//		if (WSACleanup() != 0)
//		{
//			TA_THROW(TA_Base_Core::SocketException("Couldn't shutdown Windows sockets"));
//		}
//		#endif // !WIN32

		// let us never speak of it again...
		m_doResolve = false;
	}

	if (m_isResolved)
	{
		return m_address;
	}
	else
	{
		static in_addr a;
		a.s_addr = INADDR_NONE;
		return a;
	}
}

void
InetAddress::setIPAddress(const in_addr& address)
{
	*this = InetAddress(address);
}

}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/InetAddress.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// InetAddress.h
//
// A class that encapsulates an IP address and can do host name
// resolution.  This lookup is cached thus subsequent calls to
// getIPAddress() should be somewhat faster.  You can construct
// with an IP address as either a dotted quad string or as an
// in_addr.  Reverse lookup however won't occur.

#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//TD 14774
//zhou yuan++
#define WIN32_LEAN_AND_MEAN 
//++zhou yuan
#include <winsock2.h>
#endif // !WIN32

#include <string>
#include "core/exceptions/src/SocketException.h"

namespace TA_Base_Core
{
	class InetAddress
	{
	public:
		InetAddress();
		InetAddress(const char* host);
		InetAddress(const std::string &host);
		InetAddress(const in_addr& address);
		InetAddress(const InetAddress &copy);

		bool isValid(); // is the host resolvable?
		const std::string& getHostName();
		const in_addr& getIPAddress();
		void setIPAddress(const in_addr& address);
	
	private:
		in_addr m_address;
		bool m_isResolved, m_doResolve;
		std::string m_hostname;

//			#ifdef WIN32
//    		WSAData winsockData;
//			#endif // WIN32
	};
}
#endif

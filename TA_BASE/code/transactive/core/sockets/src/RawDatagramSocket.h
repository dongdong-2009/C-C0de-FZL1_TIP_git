/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/RawDatagramSocket.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// RawDatagramSocket.h
//
// a raw datagram socket good for sending and receiving stuff like
// ICMP messages.  I think there are some restrictions on using raw
// sockets like on Windows NT you have to have admin priviledges.  Not
// sure about Unix.
//
// You can optionally get receive to strip the IP header for you.  By
// default, this is not enabled.  Thus, the receive datagram will
// include the IP header (struct ip) at the start so you must parse it out.
// That's why it's a _raw_ socket...  To enable header stripping, construct
// with stripHeader set to true.

#ifndef RAW_DATAGRAM_SOCKET_H
#define RAW_DATAGRAM_SOCKET_H

#include "core/sockets/src/IDatagramSocket.h"

namespace TA_Base_Core
{
	class RawDatagramSocket : public IDatagramSocket
	{
	public:
		RawDatagramSocket(const std::string& protocolName, bool stripHeader = false);
		~RawDatagramSocket();

		bool send(InetAddress &addr, Datagram &datagram);

		bool receive(InetAddress &outAddr, Datagram &outDG);
		
		// create and destroy socket resources.
		bool open();
		bool close();
		bool isOpen();

	protected:
		#ifdef WIN32
		SOCKET getSocketId() {return m_socket;};
		#else
		int getSocketId() {return m_socket;};
		#endif

	private:
		bool m_isOpen;
		std::string m_protocolName;

		bool m_stripHeader;

		#ifdef WIN32
		SOCKET m_socket;
		#else
		int m_socket;
		#endif
	};
}

#endif

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/SocketSet.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// SocketSet.h
//
// A class for managing i/o for a group of sockets.  Using this may
// reduce or, in some cases, eliminate the need for using threads in
// your design.  It encapsulates select/poll functionality.
//
// SocketSet is templated to support both ISocket and IDatagramSocket
// types whilst they're not in a unified hierarchy yet.  Consequently,
// don't include this file directly but instead include the appropriate
// socket header file which should have a typedef for a suitable SocketSet.
//
// The SocketType class must have the following method:
//
//   int getSocketID()
//
// which returns the system socket handle.
//
// Often you may want to associate additional attributes to the socket
// to perhaps represent various status information.  Rather than
// extending the socket class, a decorator is a more flexible solution.
// For example, I may want to poll all the devices on the network with
// a "ping" datagram.  Now, I may send up to 3 pings to the devices.  I
// need a "m_pingSuccess" flag for each socket to indicate it's been
// pinged successfully (thus won't need additional pings).  So, a decorator
// class could be defined as follows:
//
//   class PingSocket
//   {
//   public:
//       int getSocketID() {return m_socket->getSocketID();}
//
//   private:
//       IDatagramSocket *m_socket;
//       bool m_pingSuccess;
//   };
//
// and construct the SocketSet as:
//
//   SocketSet<PingSocket> pingSet;
//
// thus the m_pingSuccess flag is now associated with the socket and
// can be manipulated when PingSockets are accessed using getSocket().

#ifndef SOCKET_SET_H
#define SOCKET_SET_H

#include <vector>

namespace TA_Base_Core
{
	template <class SocketType>
	class SocketSet
	{
	public:
		SocketSet();
		~SocketSet();
		
		void addSocket(const SocketType *socket);
		void removeSocket(const SocketType *socket);
		unsigned int getSize();

		// canRead and canWrite are only valid after a waitForIO call and they
		// are true if the socket can do either without blocking.
		SocketType* getSocket(unsigned int index, bool &canRead, bool &canWrite);

		// wait for io on only one socket.  One of read or write must be
		// true otherwise function will return immediately with false.
		static bool waitForOneIO(SocketType *socket, bool read, bool write,
								 bool &canRead, bool &canWrite,
								 unsigned int timeMillis);

		// wait for io on all in set.  One of read or write must be
		// true otherwise function will return immediately with false.
		int waitForIO(bool read, bool write, unsigned int timeMillis);

	private:
		struct SocketRecord
		{
			SocketType* socket;
			bool canRead;
			bool canWrite;
		};

		std::vector<SocketRecord> m_socketRecords;
	};
}

// the implementation...
#include "SocketSetP.h"

#endif

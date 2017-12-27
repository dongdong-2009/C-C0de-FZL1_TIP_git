/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/SocketSetP.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */


#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <windows.h>
#endif // !WIN32

namespace TA_Base_Core
{

template <class SocketType>
SocketSet<SocketType>::SocketSet()
{
}

template <class SocketType>
SocketSet<SocketType>::~SocketSet()
{
}

template <class SocketType>
void
SocketSet<SocketType>::addSocket(const SocketType *socket)
{
	SocketRecord record;
	record.socket = const_cast<SocketType *>(socket);
	record.canRead = false;
	record.canWrite = false;
	m_socketRecords.push_back(record);
}

template <class SocketType>
void
SocketSet<SocketType>::removeSocket(const SocketType *socket)
{
	typename std::vector<SocketRecord>::iterator it = m_socketRecords.begin();
	while (it != m_socketRecords.end())
	{
		if ((*it).socket == socket)
		{
			m_socketRecords.erase(it);
			break;
		}
		it++;
	}
}

template <class SocketType>
unsigned int
SocketSet<SocketType>::getSize()
{
	return m_socketRecords.size();
}

// canRead and canWrite are only valid after a waitForIO call
template <class SocketType>
SocketType* 
SocketSet<SocketType>::getSocket(unsigned int index, bool &canRead, bool &canWrite)
{
	if (index >= m_socketRecords.size())
		return 0;

	canRead = m_socketRecords[index].canRead;
	canWrite = m_socketRecords[index].canWrite;

	return m_socketRecords[index].socket;
}

// wait for io on only one socket
template <class SocketType>
bool
SocketSet<SocketType>::waitForOneIO(SocketType *socket, bool read, bool write, bool &canRead, bool &canWrite, unsigned int timeMillis)
{
	if (!(read || write))
		return false;

	// right.  Build up the file descriptor sets
	fd_set readFDS, writeFDS;

	FD_ZERO(&readFDS);
	FD_ZERO(&writeFDS);

	int id = socket->getSocketId();
	if (read)
		FD_SET(id, &readFDS);
	if (write)
		FD_SET(id, &writeFDS);

	canRead = false;
	canWrite = false;

	timeval timeout;
	timeout.tv_sec = timeMillis / 1000;
	timeout.tv_usec = (timeMillis - timeout.tv_sec * 1000) * 1000;

	// now just wait a while...
    int numReady = select(id + 1, &readFDS, &writeFDS, (fd_set *)0, &timeout);

	if (numReady <= 0)
		return false;

	if (numReady > 0)
	{
		if (read)
		{
			if (FD_ISSET(id, &readFDS))
			{
				canRead = true;
			}
		}
		if (write)
		{
			if (FD_ISSET(id, &writeFDS))
			{
				canWrite = true;
			}
		}
	}

	return true;
}


// wait for io on all in set
template <class SocketType>
int
SocketSet<SocketType>::waitForIO(bool read, bool write, unsigned int timeMillis)
{
	if (m_socketRecords.size() == 0 || (read == false && write == false))
		return 0;

	// right.  Build up the file descriptor sets
	fd_set readFDS, writeFDS;

	FD_ZERO(&readFDS);
	FD_ZERO(&writeFDS);

	int maxSocketID = 0;
	typename std::vector<SocketRecord>::iterator it = m_socketRecords.begin();
	while (it != m_socketRecords.end())
	{
		int id = (*it).socket->getSocketId();
		if (read)
			FD_SET(id, &readFDS);
		if (write)
			FD_SET(id, &writeFDS);
		if (id > maxSocketID)
			maxSocketID = id;						

		(*it).canRead = false;
		(*it).canWrite = false;
		it++;
	}

	timeval timeout;
	timeout.tv_sec = timeMillis / 1000;
	timeout.tv_usec = (timeMillis - timeout.tv_sec * 1000) * 1000;

	// now just wait a while...
    int numReady = select( maxSocketID + 1, &readFDS, &writeFDS, (fd_set *)0, &timeout);

	if (numReady > 0)
	{
		it = m_socketRecords.begin();
		while (it != m_socketRecords.end())
		{
			int id = (*it).socket->getSocketId();
			if (read)
			{
				if (FD_ISSET(id, &readFDS))
				{
					(*it).canRead = true;
				}
			}
			if (write)
			{
				if (FD_ISSET(id, &writeFDS))
				{
					(*it).canWrite = true;
				}
			}

			it++;
		}
	}

	return numReady;
}

}

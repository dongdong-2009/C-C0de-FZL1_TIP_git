/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/RawDatagramSocket.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */


#include "RawDatagramSocket.h"

#if !defined( WIN32 )
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <core/sockets/src/netinet/in_systm.h>
#include <core/sockets/src/netinet/ip.h>
#endif // !WIN32

namespace TA_Base_Core
{

    RawDatagramSocket::RawDatagramSocket(const std::string& protocolName, bool stripHeader) 
    : m_isOpen(false), m_protocolName(protocolName), m_stripHeader(stripHeader)
    {
    }
    
    RawDatagramSocket::~RawDatagramSocket()
    {
        if (m_isOpen)
            close();
    }
    
    bool 
    RawDatagramSocket::send(InetAddress &addr, Datagram &datagram)
    {
        if (!m_isOpen)
            return false;
    
        if (!addr.isValid())
            return false;
        
        // Build up this address struct
        sockaddr_in address;
        memset((char*) &address, 0, sizeof(struct sockaddr_in));	
    
        address.sin_family = AF_INET;
        address.sin_addr = addr.getIPAddress();
        address.sin_port = 0;
    
        char* buffer = 0;
        unsigned int bufSize = datagram.GetBuffer(&buffer);
            
        // Do the actual send
        int sent = sendto(m_socket, buffer, bufSize, 0, 
                         (sockaddr*) &address, sizeof(sockaddr));
    
        // Test for send errorrs
        if (sent == -1)
        {
            // error in transmission
            return false;
        }	
        else if (sent != (int) bufSize)
        {
            // not all datagram sent
            return false;
        }
    
        return true;
    }
    
    bool 
    RawDatagramSocket::receive(InetAddress &outAddr, Datagram &outDG)
    {
        // The return address information
        sockaddr_in addr;
    
        int numBytes = 0;
    #ifdef WIN32
        int sas = sizeof(sockaddr);
    #else
        socklen_t sas = sizeof(sockaddr);
    #endif
    
        // Read pending datagram
        if (!m_stripHeader)
        {
            // if we don't have to strip it's much more efficient as we can
            // simply use the datagram's buffer directly
            char* buffer = 0;
            outDG.GetBuffer(&buffer);
            outDG.SetBuffer(0, Datagram::MaxDatagramSize);
    
            numBytes = recvfrom(m_socket, buffer, Datagram::MaxDatagramSize,
                                     0, (sockaddr *) &addr, &sas);
            if (numBytes < 0)
            {
                outDG.SetBuffer(0, 0);
                return false;
            }
            outDG.SetBuffer(0, numBytes);
        }
        else
        {
            // stripping means we must have a temp buffer and pay for it :P
            char buffer[Datagram::MaxDatagramSize + sizeof(ip)];
            numBytes = recvfrom (m_socket, buffer, Datagram::MaxDatagramSize + sizeof(ip),
                                     0, (sockaddr *) &addr, &sas);
    
            if (numBytes < 0)
            {
                outDG.SetBuffer(0, 0);
                return false;
            }
    
            // Find the number of bytes for the header of the message (the payload is
            // immediately after)
            ip *ipstr = (ip*) buffer;
            int hlen = ipstr->ip_hl << 2;  // 32-bit "words"
    
            // hopefully payload has zero or more bytes...
            if (numBytes < hlen)
            {
                outDG.SetBuffer(0, 0);
                return false;
            }
    
            outDG.SetBuffer(buffer + hlen, numBytes - hlen);
        }
        
        outAddr.setIPAddress(addr.sin_addr);
        return true;
    }
    
    bool 
    RawDatagramSocket::open()
    {
        if (m_isOpen)
            return false;
    
        protoent *protocol = getprotobyname(m_protocolName.c_str());
        if (protocol)
        {
            m_socket = socket(AF_INET, SOCK_RAW, protocol->p_proto);
    
    #ifdef WIN32
            if (m_socket != INVALID_SOCKET)
            {
                m_isOpen = true;
            }
    #else
            if (m_socket != -1)
            {
                m_isOpen = true;
            }
    #endif
        }
    
        return m_isOpen;
    }
    
    bool 
    RawDatagramSocket::close()
    {
        if (!m_isOpen)
            return false;
    
        #ifndef WIN32
        ::close(m_socket);
        #else
        closesocket(m_socket);
        #endif // !WIN32
    
        return true;
    }
    
    bool 
    RawDatagramSocket::isOpen()
    {
        return m_isOpen;
    }
}

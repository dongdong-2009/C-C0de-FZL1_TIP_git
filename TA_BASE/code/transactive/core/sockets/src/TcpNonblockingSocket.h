/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/TcpNonblockingSocket.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined (__TcpNonblockingSocket_h__)
#define __TcpNonblockingSocket_h__

#include "TcpSocket.h"
#include "core/exceptions/src/TcpSocketException.h"

namespace TA_Base_Core
{

	/**
  		* This class implements a non-blocking socket by extending TcpSocket.
  		*/

	class TcpNonblockingSocket : public TcpSocket
	{
	public:

		TcpNonblockingSocket( const std::string & pHost,
       		               	  const std::string & pService )
    		: TcpSocket( pHost, pService, false )
    		{
    		}

		TcpNonblockingSocket( const std::string & pHost,
       		               	  const std::string & pService,
                              const bool ignoreme, 
                              unsigned long nonBlockingConnectTimeout = DEFAULT_CONNECT_TIMEOUT, 
                              unsigned long connectionTestTimeout = DEFAULT_CONNECTION_TEST_TIMEOUT, 
                              unsigned long readingTimeout = DEFAULT_READING_TIMEOUT  )
    		: TcpSocket( pHost, pService, false, nonBlockingConnectTimeout, connectionTestTimeout, readingTimeout )
    		{
    		}


		TcpNonblockingSocket( const std::string & pService )
    		: TcpSocket( pService, false )
    		{
    		}

		TcpNonblockingSocket( const int pSocket,
	   				          const sockaddr_in * pAddress )
			: TcpSocket( pSocket, pAddress )
			{
			}

		virtual ~TcpNonblockingSocket()
    	{
    	}
	};
}

#endif // !defined (__TcpNonblockingSocket_h__)

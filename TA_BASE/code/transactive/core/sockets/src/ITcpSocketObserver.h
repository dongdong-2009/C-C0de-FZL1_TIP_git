/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/ITcpSocketObserver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *     This file defines the interface used by socket observers.
  */

#if !defined (__ITcpSocketObserver_h__)
#define __ITcpSocketObserver_h__

#include "ISocket.h"
#include <vector>


namespace TA_Base_Core
{

	/**
  		* ITcpSocketObserver
  		*
  		* This interface defines the contract that all socket observers must
  		* adhere to.  The interface contains the mechanisms for responding to
  		* data being received by a socket, and also when data could no be written
  		* down a socket.
  		*
  		*/
	class ITcpSocketObserver
	{
	public:

		/**
 		* processReceivedData
 		*
 		* This method is called whenever data is received from the socket.
 		* Typically the implementation of this method will determine whether
 		* a complete message is stored in the received data, and remove it if
 		* there is.  The remaining data is left in the buffer.
 		*
 		* @param std::string & pData - received data
 		* @param ISocket * pSocket   - the socket that received the data
 		*
 		* @returns nothing
 		*
 		*/
		virtual void processReceivedData( std::vector<unsigned char> & pData,
                                 		  ISocket * pSocket ) = 0;

		/**
 		* writeFailed
 		*
 		* This method is called whenever writing to the socket fails.
 		*
 		* @returns nothing
 		*/
		virtual void writeFailed() = 0;

		/**
 		* connectionLost
 		*
 		* This method is called whenever the observed socket loses the 
 		* connection to the remote socket.
 		*
 		* @returns false - socket should no longer be observed
 		* @returns true  - socket should continue to be observed
 		*/
		virtual bool connectionLost() = 0;

		/**
 		* connectionEstablished
 		*
 		* This method is called whenever the observed socket establishes a
 		* connection to the remote socket.  It is used in conjunction with
 		* the connectionLost method in order to handle socket reconnection
 		* at runtime.
 		*
 		* @returns nothing
 		*/
		virtual void connectionEstablished() = 0;


        /** 
          * Destructor
          */
        virtual ~ITcpSocketObserver() {};
	};
}

#endif // !defined (__ITcpSocketObserver_h__)

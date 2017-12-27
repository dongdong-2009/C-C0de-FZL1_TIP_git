#ifndef ECHO_COMMS_OBSERVER_H
#define ECHO_COMMS_OBSERVER_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/echo_server/src/EchoCommsObserver.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssCommsObserver.h"


namespace TA_Base_Bus
{
    /**
      * EchoCommsObserver allows the GssSimulationModule to delegate incoming socket
      * connections.  Each new connection is passed to a new EchoCommsObserver object.
      *
      * EchoCommsObserver just passes commands through to the simulator
      */
    class EchoCommsObserver : public GssCommsObserver
    {
    public:

        //we have no need for a default constructor.
        EchoCommsObserver(GssCommsObserver::ObservedSocketType * socket);
        
        virtual ~EchoCommsObserver();
        
        
        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                          ITcpObserver methods                            //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * processReceivedData
          *
          * Do something with the received stream of data
          *
          * @param pData the tcp data stream
          * @param pSocket the ISocket that received the data
          */
        void processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket );

    };

} // namespace TA_Base_Bus
#endif // ifndef(ECHO_COMMS_OBSERVER_H)


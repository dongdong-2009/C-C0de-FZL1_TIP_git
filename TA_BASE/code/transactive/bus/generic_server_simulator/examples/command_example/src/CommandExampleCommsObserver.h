#ifndef CommandExampleCommsObserver_H
#define CommandExampleCommsObserver_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/command_example/src/CommandExampleCommsObserver.h $
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
      * CommandExampleCommsObserver allows the GssSimulationModule to delegate incoming socket
      * connections.  Each new connection is passed to a new CommandExampleCommsObserver object.
      *
      * CommandExampleCommsObserver just passes commands through to the simulator
      */
    class CommandExampleCommsObserver : public GssCommsObserver
    {
    public:

        //we have no need for a default constructor.
        CommandExampleCommsObserver(GssCommsObserver::ObservedSocketType * socket);
        
        virtual ~CommandExampleCommsObserver();
        
        
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

    }; // class CommandExampleCommsObserver

} // namespace TA_Base_Bus
#endif // ifndef CommandExampleCommsObserver_H


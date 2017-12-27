/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/echo_server/src/EchoCommsObserver.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * EchoCommsObserver allows the GssSimulationModule to delegate incoming socket
  * connections.  Each new connection is passed to a new EchoCommsObserver object.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <algorithm>

#include "bus/generic_server_simulator/examples/echo_server/src/EchoCommsObserver.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;


EchoCommsObserver::EchoCommsObserver(GssCommsObserver::ObservedSocketType * socket)
: GssCommsObserver(socket)
{
}

EchoCommsObserver::~EchoCommsObserver()
{
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                            ITcpObserver methods                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
/**
  * processReceivedData
  *
  * Parses the received stream of data into zero or more valid table querys
  * and adds them to the message processor's queue.
  *
  * @param pData the tcp data stream
  * @param pSocket the ISocket that received the data
  */
void EchoCommsObserver::processReceivedData
(   std::vector<unsigned char> & pData,
    TA_Base_Core::ISocket * pSocket
)
{
    FUNCTION_ENTRY("EchoCommsObserver::processReceivedData");

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
            TA_Base_Core::DebugUtil::DebugDebug, "Data received" );

    std::string hex_str;
    GssUtility::data2hex( pData, hex_str );
    std::cout << "DATA:" << hex_str << std::endl;

    std::string std_str;
    GssUtility::data2string( pData, std_str );
    std::cout << "STRING:" << std_str << std::endl;

	pSocket->write( std_str.c_str(), std_str.length() );

    pData.clear();

    FUNCTION_EXIT;
}

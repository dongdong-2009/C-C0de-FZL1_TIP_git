/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssCommsObserver.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * GssCommsObserver allows the GssSimulationModule to delegate incoming socket
  * connections.  Each new connection is passed to a new GssCommsObserver object.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <algorithm>

#include "bus/generic_server_simulator/src/GssCommsObserver.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Bus;


GssCommsObserver::GssCommsObserver(ObservedSocketType * socket)
: m_clientSocket(socket)
, m_stillConnected(true)
, m_cachedClientName()
, m_verbosity(DEFAULT_VERBOSITY)
{

    std::string param = TA_Base_Core::RunParams::getInstance().get("Verbosity");
    if (!param.empty())
    {
        m_verbosity = atoi(param.c_str());
    }

    m_clientSocket->addObserver(* this);
    m_clientSocket->start();

    GssCommsObserverClientNameDiscoveryThread * discoverer
		= new GssCommsObserverClientNameDiscoveryThread( this );
	discoverer->start();
}

GssCommsObserver::~GssCommsObserver()
{
    FUNCTION_ENTRY("~GssCommsObserver");

    if( m_clientSocket != 0 )
    {
        // terminate the thread the socket is holding.
        m_clientSocket->terminate();
        delete m_clientSocket;
    }

    FUNCTION_EXIT;
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                            ITcpObserver methods                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void GssCommsObserver::writeFailed()
{
        //should probably throw some sort of exception.
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Write failed!" );
}


bool GssCommsObserver::connectionLost()
{
    m_stillConnected = false;

    //
    // Do NOT call removeObserver. Its not threadsafe, and besides you don't
    // want to stop observing just because a connection is lost.
    // m_clientSocket->removeObserver ( * this );


    //
    //the TcpObservedSocket run() method checks this value to see 
    //if it should try to reconnect.
    //set the socket pointer to null.
    return false;
}


void GssCommsObserver::connectionEstablished()
{
    m_stillConnected = true;
}


bool GssCommsObserver::stillConnected() const
{
    return m_stillConnected;
}


std::string GssCommsObserver::getSocketClientName() const
{
    return m_cachedClientName;
}


void GssCommsObserver::setSocketClientName(const std::string & name)
{
    m_cachedClientName = name;
}


int GssCommsObserver::getSocketId() const
{
    return m_clientSocket->getSocketId();
}

int GssCommsObserver::getVerbosity() const
{
    return m_verbosity;
}

GssCommsObserverClientNameDiscoveryThread::GssCommsObserverClientNameDiscoveryThread( GssCommsObserver * gco )
: m_commsObserver( gco )
, m_threadIsRunning( false )
, m_doThreadTerminate( false )
{
}


void GssCommsObserverClientNameDiscoveryThread::terminate()
{
    //
    // only terminate if we haven't decided to already ourself
    if ( !m_doThreadTerminate )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "GssSimulationModule::terminate()");
        m_doThreadTerminate = true;
    }
}

void GssCommsObserverClientNameDiscoveryThread::run()
{
    FUNCTION_ENTRY("GssCommsObserverClientNameDiscoveryThread::run");

    std::string param = TA_Base_Core::RunParams::getInstance().get("Verbosity");
    int verbosity = DEFAULT_VERBOSITY;
    if (!param.empty())
    {
        verbosity = atoi(param.c_str());
    }

    if (verbosity >= 5)
    {
        std::cout << "Client name discovery thread started" << std::endl;
    }

    m_threadIsRunning = true;
    bool name_discovered = false;
    int attempt = 1;

    m_commsObserver->setSocketClientName("Not available yet");

    while ( (!m_doThreadTerminate) && (!name_discovered) )
    {
        try
        {
			GssCommsObserver::ObservedSocketType * socket
                = m_commsObserver->getSocket();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Client name discovery attempt #%d", attempt );
            std::string name = socket->getClientName();
            m_commsObserver->setSocketClientName(name);
            name_discovered = true;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Client name \"%s\" discovered", name.c_str() );
        }
        catch ( ... )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Unknown exception caught while retrieving socket client name");
            std::string errStr = "(Exceptions encountered, ";
            errStr.append(GssUtility::num2Str(attempt));
            errStr.append(" attempts so far)");
            m_commsObserver->setSocketClientName(errStr);
        }

        attempt++;

        //
        // Wait for a few seconds before trying again
        if (!name_discovered)
        {
            sleep(2000);
        }
    }

    m_threadIsRunning = false;

    FUNCTION_EXIT;
}

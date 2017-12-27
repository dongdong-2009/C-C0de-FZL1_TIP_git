
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssSimulationModule.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * 
  * Definition of the GssSimulationModule class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>
#include <iomanip>

#include "bus/generic_server_simulator/src/GssSimulationModule.h"
#include "bus/generic_server_simulator/src/GssCommandProcessor.h"
#include "bus/generic_server_simulator/src/GssCommsObserver.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_Bus
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GssSimulationModule::GssSimulationModule
(   const std::string & name,
    const std::string & host,
    const PortList & ports)
: m_name(name)
, m_threadIsRunning(false)
, m_doThreadTerminate(false)
, m_socketPorts(ports)
, m_socketHost(host)
, m_commandProcessor(0)
, m_newConnectionWaitTime(250) 
, m_connectionLimit(0)
, m_verbosity(DEFAULT_VERBOSITY)
{
    //
    // Normally the CommandProcessor is created now, during construction.
    // Because we wish to allow simulator creators to create other subclasses
    // of ISimulationCommandProcessor, we have provided a hook method,
    // createCommandProcessor().  Excitingly enough, because of the exact order
    // in which the class hierarchy is constructed in c++, and what is
    // available, any GssSimulationModule's subclass createCommandProcessor()
    // override will not be called.  The solution is to create the command
    // processor on demand, in getCommandProcessor()
    //
    // m_commandProcessor = new GssCommandProcessor(this);

    std::string param = TA_Base_Core::RunParams::getInstance().get("Verbosity");
    if (!param.empty())
    {
        m_verbosity = atoi(param.c_str());
    }

}


GssSimulationModule::GssSimulationModule(const std::string & name)
: m_name(name)
, m_threadIsRunning(false)
, m_doThreadTerminate(false)
, m_socketHost("")
, m_commandProcessor(0)
, m_newConnectionWaitTime(250) 
, m_connectionLimit(0)
, m_verbosity(DEFAULT_VERBOSITY)
{
    std::string param = TA_Base_Core::RunParams::getInstance().get("Verbosity");
    if (!param.empty())
    {
        m_verbosity = atoi(param.c_str());
    }

}

GssSimulationModule::~GssSimulationModule()
{
    stopSim();

    
    delete m_commandProcessor;
}

//
// run()
//
// @todo something more useful in the catch(...) blocks
void GssSimulationModule::run()
{
    FUNCTION_ENTRY("GssSimulationModule::run");
    if (m_verbosity>=3)
    {
        std::cout << "Simulator Server thread has started" << std::endl;
    }

    // Nothing to do here ?
    if ((m_socketHost.empty()) || (m_socketPorts.empty()))
    {
        m_threadIsRunning = false;
        FUNCTION_EXIT;
        return;
    }

    // the run method is really the guts of it all
    m_threadIsRunning = true;

    std::list<GssCommsObserver::ServerSocketType *>  server_sockets;
    std::string socketPort;

    try
    {
        for ( PortList::iterator port_iter = m_socketPorts.begin() ;
              port_iter != m_socketPorts.end() ;
              port_iter++
            )
        {
            socketPort = *port_iter;
            GssCommsObserver::ServerSocketType * serverSocket = new GssCommsObserver::ServerSocketType( m_socketHost, socketPort );
            serverSocket->bind();
            serverSocket->listen();
            if (m_verbosity>=3)
            {
                std::cout   << "Simulator Server listening on "
                            << m_socketHost << ":" << socketPort << std::endl;
            }
            server_sockets.push_back(serverSocket);
        }
    }
    catch ( ... )
    {
        if (m_verbosity>=1)
        {
            std::cout << "Simulator listener thread caught exception during the construction of socket at address " << m_socketHost << ":" 
                      << socketPort << ", and will be terminated." << std::endl;
        }
        m_doThreadTerminate = true;
    }

    // single-threaded connection-at-a-time
    while(!m_doThreadTerminate)
    {
        try
        {
            preLoop();

            for ( std::list<GssCommsObserver::ServerSocketType *>::iterator socket_iter = server_sockets.begin() ;
                  socket_iter != server_sockets.end() ;
                  socket_iter++
                )
            {
                GssCommsObserver::ObservedSocketType * acceptSocket = (*socket_iter)->accept(TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT);

                if ( acceptSocket != 0 )
                {
                    TA_Base_Core::ThreadGuard oguard(m_observerLock);

                    GssCommsObserver * com = createCommsObserver(acceptSocket);

                    // derived simulation classes may decide to limit the
                    // number of connected CommsObservers
                    if ( com != 0 )
                    {
                        addCommsObserver(com);
                    }

                }
            }
 
            midLoop();

            //
            // check to see if all of our Observed Sockets are still connected.
            CommsObserverList::iterator it = m_commsObservers.begin();

            while(it != m_commsObservers.end())
            {
                if(!((*it)->stillConnected()))
                {
                    TA_Base_Core::ThreadGuard oguard(m_observerLock);

                    removeCommsObserver(*it);
                    delete *it;
                    // reset the iterator
                    it = m_commsObservers.begin();
                }
                else
                {
                    it++;
                }
            }

            endLoop();
        }
        catch ( ... )
        {
            m_threadIsRunning = false;
            throw;
        }

        sleep( m_newConnectionWaitTime );
    }

    m_threadIsRunning = false;

    FUNCTION_EXIT;

}

//
// terminate()
//
void GssSimulationModule::terminate()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "GssSimulationModule::terminate()");
}

//////////////////////////////////////////////////////////////////
//                                                              //
//                   ISimulationModule stuff                    //
//                                                              //
//////////////////////////////////////////////////////////////////

//
// startSim()
//
void GssSimulationModule::startSim()
{
    m_doThreadTerminate = false;
    start();
    while( false == m_threadIsRunning )
    {
        sleep(100);
    }
}

//
// stopSim()
//
void GssSimulationModule::stopSim()
{
    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    m_doThreadTerminate = true;
    
    while( true == m_threadIsRunning )
    {
        sleep(100);
    }

    //delete m_commandProcessor;

    // it should already be dead but this is a good cleanup
    terminateAndWait();
}

//
// restartSim()
//
void GssSimulationModule::restartSim()
{
    stopSim();
    startSim();
}

//
// destroySim()
//
void GssSimulationModule::destroySim()
{
    delete this;
}

//
// getName()
//
std::string GssSimulationModule::getName()
{
    return m_name;
}

//
// getSimDescription()
//
std::string GssSimulationModule::getSimDescription()
{
    return std::string( "Generic Server Simulation Module" );
}

//
// getCommandProcessor()
//
TA_Base_Bus::ISimulationCommandProcessor& GssSimulationModule::getCommandProcessor()
{
    if (m_commandProcessor == 0)
    {
        m_commandProcessor = createCommandProcessor();
    }
    
    return *m_commandProcessor;
}

//
// createCommandProcessor()
//
TA_Base_Bus::GssCommandProcessor * GssSimulationModule::createCommandProcessor()
{
    return (new GssCommandProcessor(this));
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                Execute methods                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


bool GssSimulationModule::execStatus()
{
    int numConnections = m_commsObservers.size();
    std::cout << std::endl;
    std::cout << "Server Simulator Status. Number of Connections=" << numConnections << std::endl << std::endl;

    if (numConnections < 1) return true;

    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    std::cout << "+-----------+-----------+---------------------" << std::endl;
    std::cout << "| socket id | connected | client name         " << std::endl;
    std::cout << "+-----------+-----------+---------------------" << std::endl;

    CommsObserverList::iterator iter = m_commsObservers.begin();
    GssCommsObserver * ob = 0;

    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        try
        {
            ob = (*iter);
            char socketIdStr[16] = {0};
            sprintf(socketIdStr, "%.7d", ob->getSocketId());

            std::string connectedStr;
            GssUtility::bool2string(ob->stillConnected(), connectedStr);

            std::string cname = ob->getSocketClientName();

            // std::cout << "|  " << socketIdStr.c_str() << "  |   " << connectedStr.c_str() << "   | " << cname.c_str() << std::endl;
            std::cout << "|  " << socketIdStr
                      << "  |   " << connectedStr.c_str()
                      << "   | " << cname.c_str() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Unknown exception encountered while accessing socket information" << std::endl;
        }

    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                Simulator methods                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


const GssSimulationModule::CommsObserverList & GssSimulationModule::getCommsObservers()
{ 
    return m_commsObservers; 
}


void GssSimulationModule::addCommsObserver(GssCommsObserver * com)
{
    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    if (m_verbosity>=3)
    {
        std::cout << "New Client connected to " << m_name << " simulator" << std::endl;
    }
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New Client connected to %s simulator", m_name.c_str());

    m_commsObservers.push_back(com);
}


void GssSimulationModule::removeCommsObserver(GssCommsObserver * com)
{
    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    if (m_verbosity>=3)
    {
        std::cout << "Client disconnected from " << m_name << " simulator" << std::endl;
    }
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client disconnected from %s simulator", m_name.c_str());

    CommsObserverList::iterator it = std::find(m_commsObservers.begin(), m_commsObservers.end(), com);
    if( it != m_commsObservers.end() )
    {
        // remove the CommsObserver. DO NOT destroy, that is done elsewhere
        m_commsObservers.erase(it);
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Could not find client %p in %s simulator", com, m_name.c_str());
    }
}

void GssSimulationModule::setNewConnectionWaitTime(int t)
{
    m_newConnectionWaitTime = t;
}

void GssSimulationModule::setConnectionLimit(int limit)
{
    m_connectionLimit = limit;
}


int GssSimulationModule::getVerbosity() const
{
    return m_verbosity;
}

} // namespace TA_Base_Bus




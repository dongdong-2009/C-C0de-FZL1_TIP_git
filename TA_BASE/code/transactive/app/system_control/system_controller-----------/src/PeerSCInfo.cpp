/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class maintains information about a peer
  * System Controller (or System Controller that is
  * observing a group we are in). It contains a pointer to
  * an IHeartbeatReceiverCorbaDef that it uses to send
  * heartbeats.
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "PeerSCInfo.h"
#include "SystemController.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/timers/src/MonotonicTimer.h"

#include <string>
#include <sstream>

#include <sys/timeb.h>

#ifdef WIN32
	#include <sys/types.h>
	#define ftime _ftime   // #defines to make Windows behave like Unix.
	#define timeb _timeb
#endif

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

    //
    // Constructor
    //
    PeerSCInfo::PeerSCInfo(const std::string& myHostname, const std::string& peerHostname, int heartbeatSendPeriod, int heartbeatSendNumber)
        : m_myHostname(myHostname),
          m_peerHostname(peerHostname),
          m_isPeerResolved(false),
          m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber),
          m_systemControllerMonitor(NULL), 
		  //m_firstHeartbeatTime(0),
          //m_heartbeatCount(0), 
		  m_isRunning(false),
          m_port(0)
    {
    }

    //
    // Destructor
    //
    PeerSCInfo::~PeerSCInfo()
    {
    }

    //
    // resolvePeer
    //
    void PeerSCInfo::resolvePeer()
    {
        FUNCTION_ENTRY("resolvePeer");

        m_isPeerResolved = false;
        m_systemControllerMonitor = TA_Base_Core::IHeartbeatReceiverCorbaDef::_nil();

        // Create a string in the form: 
        // "corbaloc::hostname:port/SystemControllerMonitor"
        std::ostringstream systemControllerMonitorString;
        systemControllerMonitorString << "corbaloc::" << m_peerHostname << ":" << m_port << "/SystemControllerMonitor";

        // Attempt to resolve an IHeartbeatReceiverCorbaDef object
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolving object: %s", systemControllerMonitorString.str().c_str());

            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject(systemControllerMonitorString.str());

            if ( !CORBA::is_nil(obj) )
		    {
                omniORB::setClientCallTimeout( obj, m_heartbeatSendPeriod );
                m_systemControllerMonitor = TA_Base_Core::IHeartbeatReceiverCorbaDef::_narrow(obj);
                m_isPeerResolved = true;
                omniORB::setClientCallTimeout( m_systemControllerMonitor, m_heartbeatSendPeriod );

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolved System Controller %s", m_peerHostname.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                            "Error resolving peer SystemControllerMonitor: %s",
                            m_peerHostname.c_str());
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname + " - ";
            desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", desc);
        }
        catch (CORBA::Exception& e)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname + " - ";
            desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", desc);
        }
        catch (...)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc);
        }

        FUNCTION_EXIT;
    }

    //
    // beatHeart
    //
    void PeerSCInfo::beatHeart()
    {
        if ( !m_isPeerResolved )
        {
            resolvePeer();
        }

        if ( m_isPeerResolved )
        {
            try
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[Group offline log]sending heartbeat to peer %s", m_peerHostname.c_str());

                m_systemControllerMonitor->receiveHeartbeat(m_myHostname.c_str());
            }
            catch(const std::exception& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());

                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An error occurred while seding heartbeat");
                
                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
        }
    }

    //
    // run
    //
    void PeerSCInfo::run()
    {
        m_isRunning = true;
		m_isPeerResolved = false;
        TA_Base_Core::MonotonicTimer timer;
        m_port = TA_Base_Core::getRunParamValue(RPARAM_MGRPORT, 0);

		while ( m_isRunning )
        {
            timer.restart();

            beatHeart();

            unsigned long elapsedTime = timer.elapsed();
            unsigned long sleepTime = ( elapsedTime < m_heartbeatSendPeriod ) ? m_heartbeatSendPeriod - elapsedTime : 0;

            sleep(sleepTime);
        }
    }

    //
    // terminate
    //
    void PeerSCInfo::terminate()
    {
        m_isRunning = false;
    }
}

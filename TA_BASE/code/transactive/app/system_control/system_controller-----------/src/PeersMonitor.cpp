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
  * This class is used to check if all heartbeats have been
  * received from peer System Controllers. When a heartbeat is
  * received from a System Controller, it is passed to this 
  * class which checks to see if it's from a peer and if so 
  * timestamps it. If a heartbeat is not received from ANY peer 
  * after a certain time, the SystemControllerMonitor class 
  * handles it.
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "PeersMonitor.h"

#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <sstream>
#include <ctime>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdPubUtility.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

    //
    // Constructor
    //
    PeersMonitor::PeersMonitor( const std::vector<std::string>& peerHostnames, unsigned long heartbeatSendPeriod, unsigned long heartbeatSendNumber )
        : m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber)
    {
        FUNCTION_ENTRY("PeersMonitor::PeersMonitor");

        // Initialise the time we last received a heartbeat from each peer.
        for ( std::vector<std::string>::const_iterator  it = peerHostnames.begin(); it != peerHostnames.end(); ++it )
        {
            ::ftime( &m_lastHeartbeatTimeMap[*it].second );
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PeersMonitor::PeersMonitor(): add peer host %s.", it->c_str());
        }

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    PeersMonitor::~PeersMonitor()
    {
    }

    //
    // getFailedPeers
    //
    std::vector<std::string> PeersMonitor::getFailedPeers()
    {
        std::vector<std::string> failedPeers;
        const unsigned long timeout = m_heartbeatSendPeriod * m_heartbeatSendNumber;

        TA_THREADGUARD(m_lock);

        for ( HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.begin(); it != m_lastHeartbeatTimeMap.end(); ++it )
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PeersMonitor::getFailedPeers(): peer [%s] "
                "last heartbeat:[%s]; elapsed:[%lu]; "
                "m_heartbeatSendPeriod:[%lu]; m_heartbeatSendNumber:[%lu]",
                it->first.c_str(),
                TA_Base_Core::CStdPubUtility::getTimeStr( &it->second.second ).c_str(), it->second.first.elapsed(),
				m_heartbeatSendPeriod, m_heartbeatSendNumber);

			if (  timeout < it->second.first.elapsed() )
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "PeersMonitor::getFailedPeers(): got a failed peer [%s]; "
                    "last heartbeat:[%s]; elapsed:[%lu]; "
                    "m_heartbeatSendPeriod:[%lu]; m_heartbeatSendNumber:[%lu]",
					it->first.c_str(),
                    TA_Base_Core::CStdPubUtility::getTimeStr( &it->second.second ).c_str(), it->second.first.elapsed(),
                    m_heartbeatSendPeriod, m_heartbeatSendNumber);

                // Timestamp not up-to-date. Add this peer to vector.
                failedPeers.push_back( it->first );
            }
        }

        return failedPeers;
    }

    //
    // notifyHeartbeat
    //
    void PeersMonitor::notifyHeartbeat( const std::string& host )
    {
        TA_THREADGUARD(m_lock);

        // Check to see if the heartbeat we just received is
        // from one of our peers.
        HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.find( host );
        
        if ( it != m_lastHeartbeatTimeMap.end() )
        {
            // Host found - heartbeat must have been from peer.
            // Record the time it was received.
            it->second.first.restart();
            ::ftime( &it->second.second );
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PeersMonitor::notifyHeartbeat(): Host %s found.", host.c_str() );
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PeersMonitor::notifyHeartbeat(): Host %s is not in peer monitor group, ignored.", host.c_str());
        }
    }

} // namespace TA_Base_App

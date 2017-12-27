/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton class implements the IHeartbeatReceiverCorbaDef
  * interface to allow the System Controller to receive a heartbeat
  * from other System Controllers. When a heartbeat is received, it
  * is passed to the PeersMonitor and GroupMonitor classes to store
  * who the heartbeat came from. Additionally, this class sends a
  * heartbeat to all peers, as well as the System Controllers that 
  * are observing the group it is in (if it is in fact in any group).
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "SystemControllerMonitor.h"
#include "PeersMonitor.h"
#include "GroupMonitor.h"
#include "ProcessManager.h"
#include "ProcessUtilities.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h"
#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ServerEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/SystemAlarms_MessageTypes.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/timers/src/MonotonicTimer.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;

TA_Base_App::SystemControllerMonitor* TA_Base_App::SystemControllerMonitor::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::SystemControllerMonitor::m_lock;

const unsigned long TA_Base_App::SystemControllerMonitor::DEFAULT_POLL_CHECK_INTERVALMS  = 500;

namespace TA_Base_App
{
    //
    // getInstance
    //
    SystemControllerMonitor& SystemControllerMonitor::getInstance()
    {
        // Make this threadsafe.
        TA_Base_Core::ThreadGuard guard(m_lock);

	    if (s_instance == NULL)
        {
            s_instance = new SystemControllerMonitor();
            if (s_instance == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                        "New failed to allocate SystemControllerMonitor");
                exit(1);
            }
        }
        return *s_instance;
    }

    // 
    // Constructor
    //
    SystemControllerMonitor::SystemControllerMonitor()
		: m_groupProcessesAllStarted(false)
    {
        FUNCTION_ENTRY("Constructor");

        m_myHostname = ProcessUtilities::getHostname();

        try
        {
            // Get the hostnames of my peers from the database.
            std::vector<std::string> peerHostnames = 
                TA_Base_Core::ManagedProcessAccessFactory::getInstance().
                                            getPeersOfSystemController(m_myHostname);

            // Get the hostname of the group I am in, and the interval between
            // heartbeats.
            TA_Base_Core::ISystemControllerData* myData = TA_Base_Core::SystemControllerAccessFactory:: getInstance().getSystemControllerDataByName(m_myHostname);
            std::string myGroupName = myData->getGroupByName();
            m_heartbeatSendPeriod = myData->getHeartbeatSendPeriod() * 1000;
            m_heartbeatSendNumber = myData->getHeartbeatSendNumber();
            delete myData;
            myData = NULL;

            // Create the PeersMonitor object that will check heartbeats are received
            // from each peer.
			// yanrong++: CL-21195, 2010-01-22, use milliseconds in PeersMonirot
            //m_peersMonitor = new PeersMonitor(peerHostnames, m_heartbeatSendPeriod / 1000);
			m_peersMonitor = new PeersMonitor(peerHostnames, m_heartbeatSendPeriod, m_heartbeatSendNumber);
			//
            if (m_peersMonitor == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                            "New failed to allocate PeersMonitor");
                exit(1);
            }

            std::vector<std::string>::const_iterator iter;
            for (iter = peerHostnames.begin(); iter != peerHostnames.end(); iter++)
            {
                // Get the entity data for each peer.
                TA_Base_Core::ISystemControllerData* peerData = TA_Base_Core::SystemControllerAccessFactory:: getInstance().getSystemControllerDataByName(*iter);
                unsigned long entityKey = peerData->getServerEntityKey();
				TA_Base_Core::IEntityData* entityData	= TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
                m_peerEntityData[*iter] = new TA_Base_Core::ServerEntityData( entityData->getKey() ); 
                delete peerData;
                peerData = NULL;

                // Create a PeerSCInfo object for each peer, so we send heartbeats
                // to each one. Mark each peer as being initially uncontactable.

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
                    "Will be sending heartbeats to peer %s, heartbeat period: %lu ms, heartbeat number: %lu", (*iter).c_str(), /*yanrong++:CL-21195*/ m_heartbeatSendPeriod, m_heartbeatSendNumber);
                m_peerSCsToPulsate.push_back(new PeerSCInfo(m_myHostname, *iter, m_heartbeatSendPeriod, m_heartbeatSendNumber));
                if (m_peerSCsToPulsate.back() == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                                "New failed to allocate PeerSCInfo object");
                    exit(1);
                }
                m_failedPeers.push_back(*iter);
                raisePeerFailedAlarm(*iter);
                delete entityData; //++RenQiang
                entityData = NULL;
            }

            // Get the names of the groups that are monitoring us
			if (!myGroupName.empty())
            {
                // I'm in a group! 
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Am in group %s",
                            myGroupName.c_str());

                // Get the hostnames of the System Controllers that are
                // monitoring my group.
                m_monitorsOfMyGroup = TA_Base_Core::GroupMonitorAccessFactory::
                                 getInstance().getSystemControllersMonitoringGroup(myGroupName);
            }
            else
            {
                 // Not in a group.
                 LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Am not in a group");
            }

            // Get the names of the groups we are monitoring
            std::vector<std::string> groupsToMonitor = TA_Base_Core::GroupMonitorAccessFactory::
                getInstance().getGroupsMonitoredBySystemController(m_myHostname);

            for (iter = groupsToMonitor.begin(); iter != groupsToMonitor.end(); iter++)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Monitoring group %s", (*iter).c_str());
            
                m_hasGroupFailed.push_back(false);

                std::vector<std::string> hostsInGroup = TA_Base_Core::SystemControllerAccessFactory::
                    getInstance().getSystemControllersInGroup(*iter);
				// CL-20810 zn++
				// yanrong++: CL-21195 -- GroupMonitor use heartbeat period in milliseconds
                m_groupMonitors.push_back(new GroupMonitor(*iter, hostsInGroup, m_heartbeatSendPeriod, m_heartbeatSendNumber));
				//m_groupMonitors.push_back(new GroupMonitor(*iter, hostsInGroup, m_heartbeatSendPeriod / 1000));
				// ++yanrong
				// ++zn
                if (m_groupMonitors.back() == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                                "New failed to allocate GroupMonitor");
                    exit(1);
                }
            }
        }
        catch(TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating SystemControllerMonitor"));
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating SystemControllerMonitor"));
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "");
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating SystemControllerMonitor"));
        }

        startHeartBeating();

        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerMonitor");
        activateServant();

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    SystemControllerMonitor::~SystemControllerMonitor()
    {
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    }

    //
    // cleanUp
    //
    void SystemControllerMonitor::cleanUp()
    {
        FUNCTION_ENTRY("cleanUp");

        stopHeartBeating();

        // Delete the vector of PeerSCInfo's for Peers
		std::vector<PeerSCInfo*>::iterator i;
        for (i = m_peerSCsToPulsate.begin(); 
            i != m_peerSCsToPulsate.end(); i++)
        {
            delete *i;
            *i = NULL;
        }
        m_peerSCsToPulsate.clear();

		// Delete the vector of PeerSCInfo's for Groups.
		std::vector<PeerSCInfo*>::iterator k;
        for (k = m_groupSCsToPulsate.begin(); 
            k != m_groupSCsToPulsate.end(); k++)
        {
            delete *k;
            *k = NULL;
        }
        m_groupSCsToPulsate.clear();

        // Delete the PeersMonitor
        delete m_peersMonitor;
        m_peersMonitor = NULL;

        // Delete the vector of GroupMonitor's.
        for (std::vector<GroupMonitor*>::iterator j = m_groupMonitors.begin(); 
            j != m_groupMonitors.end(); j++)
        {
            delete *j;
            *j = NULL;
        }
        m_groupMonitors.clear();

        // Clean up the entity data
        for (std::map<std::string, TA_Base_Core::ServerEntityData*>::iterator iter = m_peerEntityData.begin();
            iter != m_peerEntityData.end(); iter++)
        {
            delete iter->second;
            iter->second = NULL;
        }
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        FUNCTION_EXIT;
    }

    //
    // receiveHeartbeat
    //
    void SystemControllerMonitor::receiveHeartbeat(const char* host)
    {
        // TA_Base_Core::ThreadGuard guard(m_lock);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Notifying Monitor of heartbeat...");

        if ( m_peersMonitor != NULL )
        {
            // Notify the peer monitor of this heartbeat.
            m_peersMonitor->notifyHeartbeat(std::string(host));
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[Group offline log]Notifying Group Monitor(s) of heartbeat...");

        // Notify the group monitor(s) of this heartbeat.
        for (std::vector<GroupMonitor*>::iterator iter = m_groupMonitors.begin(); 
            iter != m_groupMonitors.end(); iter++)
        {
            (*iter)->notifyHeartbeat(std::string(host));
        }
    }

    //
    // startHeartBeating
    //
    void SystemControllerMonitor::startHeartBeating()
    {
        FUNCTION_ENTRY("startHeartBeating");

		// TD7009 - Start the Peer heartbearts right from the start, but
		// only start the Group heartbeats once all of the processes have
		// started correctly.
		if (m_groupProcessesAllStarted == false)
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Starting Peer heartbeats...");

			// Start Peer heartbeats
			for (std::vector<PeerSCInfo*>::iterator iter = m_peerSCsToPulsate.begin(); 
				iter != m_peerSCsToPulsate.end(); iter++)
			{
				(*iter)->start();
			}
		}
		else
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Starting Group heartbeats...");

			// Start Group heartbeats
			for (std::vector<PeerSCInfo*>::iterator iter = m_groupSCsToPulsate.begin(); 
				iter != m_groupSCsToPulsate.end(); iter++)
			{
				(*iter)->start();
			}			
		}

        FUNCTION_EXIT;
    }

    //
    // stopHeartBeating
    // 
    void SystemControllerMonitor::stopHeartBeating()
    {
        FUNCTION_ENTRY("stopHeartBeating");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Stopping Peer heartbeats...");

		// TD7009 - Stop the all the heartbeats
        for (std::vector<PeerSCInfo*>::iterator iter = m_peerSCsToPulsate.begin(); 
            iter != m_peerSCsToPulsate.end(); iter++)
        {
            (*iter)->terminateAndWait();
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Stopping Group heartbeats...");

		for (std::vector<PeerSCInfo*>::iterator i = m_groupSCsToPulsate.begin(); 
		i != m_groupSCsToPulsate.end(); i++)
		{
			(*i)->terminateAndWait();
		}

        FUNCTION_EXIT;
    }

    //
    // run
    //
	void SystemControllerMonitor::run()
    {
        FUNCTION_ENTRY("run");

        m_isRunning = true;

        unsigned long diffTimeMsecs = 0;  // Difference in time between the two, in milliseconds.
        signed long sleepTimeMsecs = 0;   // Time to sleep for.
        unsigned int heartBeatTimeoutSecs = ((m_heartbeatSendPeriod / 1000) * m_heartbeatSendNumber); //TD15949: Heartbeats must be received within double the heartbeat rate.
        unsigned int heartBeatTimeoutMsecs = m_heartbeatSendPeriod * m_heartbeatSendNumber;
        TA_Base_Core::MonotonicTimer timer;

        while (m_isRunning)
        {
            timer.restart();

			if (!m_groupProcessesAllStarted)
			{
				checkForGroupProcesses();
			}

            checkForFailedPeers();
            checkForFailedGroups();
         
            diffTimeMsecs = timer.elapsed();
            sleepTimeMsecs = m_heartbeatSendPeriod - diffTimeMsecs;
            sleepTimeMsecs = (sleepTimeMsecs > 0 ) ? sleepTimeMsecs : 0;
            // if m_heartbeatSendPeriod is too large e.g. 5 seconds, we don't want to check every 5 seconds, default to every 500 Miliseconds.
            sleepTimeMsecs = (sleepTimeMsecs > DEFAULT_POLL_CHECK_INTERVALMS ) ? DEFAULT_POLL_CHECK_INTERVALMS : sleepTimeMsecs;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Would be sleeping for %ld milliseconds.", sleepTimeMsecs);

			//TD15949++
			// Sleep for the right amount of time - careful not to overflow...
			//
			//   <--------heartbeat time( 2* heartbeat send period)----------->
			//   <----process time--->
			//                        <----------sleep period----------------->
			// Sleeping for too long risks invalidating valid heartbeats and mistakenly sending OCC groups offline
			// Note: process time means processing checkForGroupProcesses,checkForFailedPeers and checkForFailedGroups

			if ( diffTimeMsecs < heartBeatTimeoutMsecs )
			{
				//Processing time falls within the heartbeat interval
                m_timedWaitForNextPoll.timedWait(sleepTimeMsecs);
			}
			else
			{
				//Highly unlikely case but just in case...
				//Processing time falls outside the heartbeat interval
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Checking for failed groups took longer than %d seconds(heat beat send timeout). Actual processing time took %d seconds",heartBeatTimeoutSecs, sleepTimeMsecs);
			}
			//++TD15949
        }

        FUNCTION_EXIT;
    }

    //
    // terminate
    // 
    void SystemControllerMonitor::terminate()
    {
        m_isRunning = false;
        m_timedWaitForNextPoll.signal();
    }

    //
    // checkForFailedPeers
    //
    void SystemControllerMonitor::checkForFailedPeers()
    {
        // currentFailedPeers will contain all SCs that have failed.
        // newFailures will contain SCs that have just been detected as failed.
        // newRecoveries will contain SCs that have just recovered from a failure.
        std::vector<std::string> currentFailedPeers;
        std::vector<std::string> newFailures;
        std::vector<std::string> newRecoveries;

        if ( m_peersMonitor != NULL )
        {
            currentFailedPeers = m_peersMonitor->getFailedPeers();
        }

        if ( m_failedPeers == currentFailedPeers )
        {
            return;
        }

        std::sort(currentFailedPeers.begin(), currentFailedPeers.end());

        // Check if any new peers have failed.
        std::set_difference(currentFailedPeers.begin(), currentFailedPeers.end(), 
                            m_failedPeers.begin(), m_failedPeers.end(),
                            std::insert_iterator<std::vector<std::string> >
                            (newFailures, newFailures.begin()));

        if (!newFailures.empty())
        {
            // Peers have failed!
            for (std::vector<std::string>::iterator iter = newFailures.begin(); 
                iter != newFailures.end(); iter++)
            {
				//TD16585++
				//ProcessManager::getInstance().waitForOfflineDatabase();
				//++TD16585
                // when peer has failed, we set localDB to offline first 
                // and proceed with server failover by accessing centralDB,
                // during next database poll the localDB status will be updated.
				//ProcessManager::getInstance().setLocalDatabseStatusOffline();

                ProcessManager::getInstance().respondToPeerSCFailure(*iter);
                // can we raise peer failure alarm after respondToPeerSCFailure 
                // to increase server failover performance?
                raisePeerFailedAlarm(*iter);
            }
        }

        // Check if any previously failed peers have recovered.
        std::set_difference(m_failedPeers.begin(), m_failedPeers.end(), 
                            currentFailedPeers.begin(), currentFailedPeers.end(),
                            std::insert_iterator<std::vector<std::string> >
                            (newRecoveries, newRecoveries.begin()));

        if (!newRecoveries.empty())
        {
            // Peers have recovered!
            for (std::vector<std::string>::iterator iter = newRecoveries.begin(); 
                iter != newRecoveries.end(); iter++)
            {
                closePeerFailedAlarm(*iter);
                ProcessManager::getInstance().respondtoPeerSCRestored(*iter);
            }
        }

        // Now store the vector of all failed peers.
        m_failedPeers = currentFailedPeers;
    }

    //
    // checkForFailedGroups
    //
    void SystemControllerMonitor::checkForFailedGroups()
    {
		FUNCTION_ENTRY("checkForFailedGroups");

		// gongzong++ CL-21238. deal with this case: group's amounts != 1

        //std::vector<bool> currentFailedGroups;
        //currentFailedGroups.resize(m_groupMonitors.size());

        //// Iterators for the groups and the previous failures.
        //std::vector<GroupMonitor*>::iterator  groupIter = m_groupMonitors.begin();
        //std::vector<bool>::iterator  prevFailIter = m_hasGroupFailed.begin();

        //for (std::vector<bool>::iterator curFailIter = currentFailedGroups.begin(); 
        //    curFailIter != currentFailedGroups.end(); curFailIter++)
        //{
        //    *curFailIter = (*groupIter)->hasGroupFailed();

        //    if (*curFailIter == true && *prevFailIter == false)
        //    {
        //        // Group has just failed.
        //        raiseGroupFailedAlarm((*groupIter)->getGroupName());
        //        ProcessManager::getInstance().respondToGroupOffline((*groupIter)->getGroupName());
        //    }

        //    if (*curFailIter == false && *prevFailIter == true)
        //    {
        //        // Group has just recovered.
        //        closeGroupFailedAlarm((*groupIter)->getGroupName());
        //        ProcessManager::getInstance().respondToGroupOnline((*groupIter)->getGroupName());
        //    }
        //}

        //// Now store the vector of bools for the next time step.
        //m_hasGroupFailed = currentFailedGroups;

		std::vector<bool> curFailedGroups;
		for (unsigned int i = 0; i != m_groupMonitors.size(); ++i)
		{
			std::string groupname = m_groupMonitors[i]->getGroupName();

			curFailedGroups.push_back( m_groupMonitors[i]->hasGroupFailed() );

			if (curFailedGroups[i] == true && m_hasGroupFailed[i] == false)
			{
                raiseGroupFailedAlarm(groupname);
                ProcessManager::getInstance().respondToGroupOffline(groupname);
			}
			else if( curFailedGroups[i] == true && m_groupMonitors[i]->groupStartFailed())
			{
				raiseGroupFailedAlarm(groupname);
				ProcessManager::getInstance().respondToGroupOffline(groupname);
			}

			if (curFailedGroups[i] == false && m_hasGroupFailed[i] == true)
			{
                closeGroupFailedAlarm(groupname);
                ProcessManager::getInstance().respondToGroupOnline(groupname);
			}
		}

		m_hasGroupFailed = curFailedGroups;
		// ++gongzong

		FUNCTION_EXIT;
    }

    //
    // raisePeerFailedAlarm
    //
    void SystemControllerMonitor::raisePeerFailedAlarm(const std::string& hostname)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
           "The following peer has failed: %s. Raising alarm.", hostname.c_str());

        try
        {
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair name("HostId", hostname);
        
	        dp.push_back(&name);
	        
            // Need data about the peer's server entity to raise an alarm.
            std::map<std::string, TA_Base_Core::ServerEntityData*>::iterator entityIter = 
                m_peerEntityData.find(hostname);
            TA_ASSERT(entityIter != m_peerEntityData.end(), "Could not find peer entity data");
            TA_Base_Core::ServerEntityData* serverData = entityIter->second;
            //TD11293,hongzhi, PISF/TST/814 - Communication failure alarm not raised at station level when occa and occb are offline
			TA_Base_Core::ServerEntityData* ownServerData = 
                ProcessManager::getInstance().getServerEntityData();

            m_failedPeerAlarmIds[hostname] = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
                    TA_Base_Core::SystemAlarms::PeerCommunicationsFailure, 
                    serverData->getKey(), serverData->getTypeKey(), dp, 
                    serverData->getName(), ownServerData->getLocation(), 
                    serverData->getSubsystem(), serverData->getAssetName(),
					TA_Base_Core::AlarmConstants::defaultTime);
        }
        catch(TA_Base_Core::TransactiveException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
        }
    }

    //
    // raiseGroupFailedAlarm
    //
    void SystemControllerMonitor::raiseGroupFailedAlarm(const std::string& groupName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
            "The following group has failed: %s. Raising alarm.", groupName.c_str());

        try
        {
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair name("GroupId", groupName);
        
	        dp.push_back(&name);
	        
            // Need data about the Server entity to raise an alarm.
            TA_Base_Core::ServerEntityData* serverData = 
                ProcessManager::getInstance().getServerEntityData();
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"raise GroupCommunicationsFailure alarm for group fail: key:%d, typekey:%d, location:%d, subsystem:%d",
				serverData->getKey(), serverData->getTypeKey(), serverData->getLocation(), serverData->getSubsystem());

			// Need data about the Server entity to raise an alarm.
            m_failedGroupAlarmIds[groupName] = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
                    TA_Base_Core::SystemAlarms::GroupCommunicationsFailure, 
                    serverData->getKey(), serverData->getTypeKey(), dp, 
                    serverData->getName(), serverData->getLocation(), 
                    serverData->getSubsystem(), serverData->getAssetName(), 
					TA_Base_Core::AlarmConstants::defaultTime);
        }
        catch(TA_Base_Core::TransactiveException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
        }
    }

    //
    // closePeerFailedAlarm
    //
    void SystemControllerMonitor::closePeerFailedAlarm(const std::string& hostname)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "The following peer is now online: %s. Closing alarm.", hostname.c_str());

        // Close the alarm if this isn't the first time the peer has been
        // contacted.
        std::string alarmId = m_failedPeerAlarmIds[hostname];
        if (alarmId != "")
        {
            try
            {
                // Need the Server entity's location to close the alarm.
                TA_Base_Core::ServerEntityData* serverData = 
                    ProcessManager::getInstance().getServerEntityData();

				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmId, serverData->getLocation());

                // Delete the alarm Id from the map.
                m_failedPeerAlarmIds[hostname] = "";
            }
            catch(TA_Base_Core::TransactiveException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
            }
        }
    }

    //
    // closeGroupFailedAlarm
    //
    void SystemControllerMonitor::closeGroupFailedAlarm(const std::string& groupName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
            "The following group is now online: %s. Closing alarm.", groupName.c_str()); 

        // Close the alarm if this isn't the first time the group has been
        // contacted.
        std::string alarmId = m_failedGroupAlarmIds[groupName];
        if (alarmId != "")
        {
            try
            {
				// Need the Server entity's location to close the alarm.
                TA_Base_Core::ServerEntityData* serverData = 
                    ProcessManager::getInstance().getServerEntityData();

				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmId, serverData->getLocation());

                // Delete the alarm Id from the map.
                m_failedGroupAlarmIds[groupName] = "";
            }
            catch(TA_Base_Core::TransactiveException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
            }
        }
    }

	void SystemControllerMonitor::checkForGroupProcesses()
	{
		FUNCTION_ENTRY("checkForGroupProcesses");

		// Call the process manager and acertain if all of the processes
		// are running as configured
		if (ProcessManager::getInstance().haveAllAgentsStartedAtLeastOnce())
		{
			// The sun is shining
			m_groupProcessesAllStarted = true;

			// TD7009 - Need to check that all processes are either in RunningControl
			// or RunningMonitor before we start the heartbeat.
			std::vector<std::string>::const_iterator iter;
            for (iter = m_monitorsOfMyGroup.begin(); iter != m_monitorsOfMyGroup.end(); iter++)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Will be sending heartbeats to group monitor %s", (*iter).c_str());
                m_groupSCsToPulsate.push_back(new PeerSCInfo(m_myHostname, *iter, m_heartbeatSendPeriod, m_heartbeatSendNumber));
                if (m_groupSCsToPulsate.back() == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                                "New failed to allocate PeerSCInfo object");
                    exit(1);
                }
            }

			// Start heartbeat for the group
			startHeartBeating();
		}
		else
		{
			m_groupProcessesAllStarted = false;
		}

		FUNCTION_EXIT;
	}

	void SystemControllerMonitor::setGroupStarting( const char* hostName )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SystemControllerMonitor:setGroupStarting. hostName: %s", hostName);

		// Notify the group monitor(s) of this heartbeat.
		for (std::vector<GroupMonitor*>::iterator iter = m_groupMonitors.begin(); 
			iter != m_groupMonitors.end(); iter++)
		{
			if((*iter)->contains(std::string(hostName)))
			{
				(*iter)->setGroupStarting();
			}
		}
	}
} // namespace TA_Base_App


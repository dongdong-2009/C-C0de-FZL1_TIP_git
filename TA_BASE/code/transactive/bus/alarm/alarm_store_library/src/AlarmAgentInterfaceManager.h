/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmAgentInterfaceManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmAgentInterfaceManager.h
//  Implementation of the Class AlarmAgentInterfaceManager
//  Created on:      24-Jan-2004 04:16:57 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmAgentInterfaceManager_1EC55F5F_0789_4f5c_802A_7B3680428743__INCLUDED_)
#define AlarmAgentInterfaceManager_1EC55F5F_0789_4f5c_802A_7B3680428743__INCLUDED_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IAlarmUpdate.h"
#include <vector>
#include <string>

namespace TA_Base_Bus
{
	class AlarmAgentInterface;

	/**
	 * This class is responsible for instantiating the appropriate
	 * AlarmAgentInterfaces and controlling their reloading.
	 * @version 1.0
	 * @created 24-Jan-2004 04:16:57 PM
	 */
	class AlarmAgentInterfaceManager
	{

		public:

			/**
			 * Constructor
			 * @param callback
			 * 
			 */
			explicit AlarmAgentInterfaceManager(IAlarmUpdate& callback);

			virtual ~AlarmAgentInterfaceManager();

			/**
			 * start the monitoring a specified agent.  It will have no effect if the agent is
			 * already being monitored.
			 * @param agentEntityName    Entity name of alarm agent
			 * 
			 */
			void startAgentMonitoring(const std::string &agentEntityName,long location,bool alarmStoreStart);

			/**
			 * Stop the monitoring of all alarm agents that are currently being monitored.
			 */
			void stopAllAgentMonitoring();

			/**
			 * This will stop all agent monitoring except for the location specified.  This
			 * will be used when switching from all agents to a single agent mode.
			 */
			void stopAllAgentMonitoringExcept(const std::vector<std::string> &agentEntityName);


			/**
			 * This will get all the alarms reloaded from each agent.  This will
			 * be necessary if the alarm store pre-filter is changed.  For example
			 * a profile change may affect the pre-filtering
			 */
			void reloadAllAlarms();

		private:

			AlarmAgentInterfaceManager(const AlarmAgentInterfaceManager& theAlarmAgentInterfaceManager);
			AlarmAgentInterfaceManager& operator = (const AlarmAgentInterfaceManager& theAlarmAgentInterfaceManager);

			std::vector<AlarmAgentInterface*>	   m_alarmAgentInterfaceList;
			TA_Base_Core::ReEntrantThreadLockable  m_interfaceListLock;
			IAlarmUpdate&						   m_callback;
            unsigned long                          m_myLocation; // Our location key
	};
};  // namespace TA_Base_App

#endif // !defined(AlarmAgentInterfaceManager_1EC55F5F_0789_4f5c_802A_7B3680428743__INCLUDED_)

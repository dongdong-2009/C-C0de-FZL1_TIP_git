/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/alarm/shared_alarm/src/AllAlarmManager.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  */

#if !defined(__ALL_ALARM_MANAGER_INCLUDED__)
#define __ALL_ALARM_MANAGER_INCLUDED__

#pragma once
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include "bus/alarm/shared_alarm/src/ICheckVisibility.h"
#include "bus/alarm/shared_alarm/src/IDutyAlarmAccess.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	// forward declaration
	class TA_SharedAlarm;

	typedef std::map<DWORD, AlarmDataItem*> DutyTempList;
	typedef std::map<unsigned long, std::list<unsigned long> > LocationSubSytemList;
	
	/* AllAlarmManager
	 *
	 * Class manages all the alarm that are stored in in-memory 
	 */
	class SHAREDALARM_API AllAlarmManager 
	{
	public:
		
		/* getInstance
		 *
		 * returns the instance of this class
		 *
		 * @Param	: none
		 *
		 * @Return	: AllAlarmManager& a reference to the instance of this class
		 */
		static AllAlarmManager& getInstance();
		
		/* freeInstance
		 *
		 * destroys the object instance of this class.
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		static void freeInstance();
		
		/* ~AllAlarmManager
		 *
		 * destructor
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		~AllAlarmManager(void);


		/* deleteAllAlarmItems
		 *
		 * delete and deallocates all the in-memory alarms
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		void deleteAllAlarmItems(); 
		
		/* getAllAlarmCount
		 *
		 * retrieves the current count of AllAlarms 
		 *
		 * @Param	: none
		 *
		 * @Return	: unsigned long - count of the alarms contained in this class
		 */
		unsigned long getAllAlarmCount();
		
		/* insertAlarm
		 *
		 * Inserts a single Alarm into the in-memory container. If alarmID is already
					existing function will return false.
		 *
		 * @Param	: AlarmDataItem* pData	- a pointer to the alarm to insert
					: DutyTempList * pDutyList - a pointer to the temporay duty list, this 
									variable is optional and only use during duty change.
		 *
		 * @Return	: returns true on success otherwise false.
		 */
		virtual bool insertAlarm(AlarmDataItem* pData, DutyTempList * pDutyList = NULL);
		
		/* updateAlarm
		 *
		 * updates the alarm with the corresponding changes specified in the AlarmUpdateCorbaDef
		   function returns false if update fails.
		 *
		 * @Param	: AlarmDataItem* pData	- a pointer to the alarm to be updated
					: const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate - a reference to 
					alarmUpdatecorbadef which contains update information.									
		 *
		 * @Return	: returns true on success otherwise false.
		 */
		virtual bool updateAlarm(AlarmDataItem* pData, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
	
		/* deleteAlarm
		 *
		 * removes the specific alarm matching the alarmID.
		 *
		 * @Param	: std::string& strAlarmID - ID of the alarm to delete								
		 *
		 * @Return	: returns true on success otherwise false.
		 */
		virtual bool deleteAlarm(std::string& strAlarmID);

		/* removeLocationAlarms
		 *
		 * removes the alarms matching the location key.
		 *
		 * @Param	: unsigned long locationKey - location key of the alarms to remove							
		 *
		 * @Return	: unsigned long - returns the number of alarms that was removed.
		 */
		unsigned long removeLocationAlarms(unsigned long locationKey, unsigned long dataSource = ISCS_SUBSYSTEM_ID);
		
		unsigned long removeDataSourceAlarms(unsigned long dataSource);
		/* buildIndexWithDuty
		 *
		 * this function checks the visibility of all the alarms inside the in-memory
			container and inserts all the visible alarm to the shared memory through the use
			of the interface pointer IDutyAlarmAccess.
		 *
		 * @Param	: ICheckVisibility* - Interface pointer to the functions for checking the visibility.
					: IDutyAlarmAccess* - Interface pointer to DutyAlarmAccess for shared memory access.
		 *
		 * @Return	: none
		 */
		void buildIndexWithDuty(ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess);
		
		/* UpdateDutyIndex
		 *
		 * Function re-evaluate the alarm visibility based on the location and subsystems.
		 *
		 * @Param	: LocationSubSytemList& - map containing the list of locations and its subsystems
					: ICheckVisibility* - Interface pointer to the functions for checking the visibility.
					: IDutyAlarmAccess* - Interface pointer to DutyAlarmAccess for shared memory access.
		 *
		 * @Return	: none
		 */
		void UpdateDutyIndex(LocationSubSytemList& changes, ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess);

		/* findAlarm
		 *
		 * Function searched and return the alarm matching the alarm ID
		 *
		 * @Param	: std::string - alarm ID to search
		 *
		 * @Return	: AlarmDataItem* - pointer to the alarm on success otherwise returns NULL
		 */
		virtual AlarmDataItem* findAlarm(std::string alarmID);
		
		/* setAllAlarmsMaxCount
		 *
		 * Function sets the maximum allowable count of alarms in in-memory container.
		 *
		 * @Param	: unsigned long - maximum alarm count
		 *
		 * @Return	: none
		 */
		void setAllAlarmsMaxCount(unsigned long ulMaxCount);

		/* deleteItem
		 *
		 * deletes the alarm and sets the pointer to NULL
		 *
		 * @Param	: TA_Base_Bus::AlarmDataItem* - pointer to the alarm to delete
		 *
		 * @Return	: 0
		 */
		int deleteItem(TA_Base_Bus::AlarmDataItem* pAlarmDataItem);

	private:	// functions
		// Constructor
		AllAlarmManager(void);
		
		// Internal function for removing alarm that matches the location key
		unsigned long removeLocationAlarmsInternal(unsigned long locationKey, unsigned long dataSource = ISCS_SUBSYSTEM_ID);

		// Function is use to convert time in long to string format
		std::string _formatTime(long ulTime);
		
	private:	// variables
		// pointer to this class to hold the single instance
		static AllAlarmManager * m_pInstance;

		// This a thread lock used to ensure that only 1 instance will be created during instantiation.
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;	 
		
		// This a thread lock used to synchronize access to this class instance.
		TA_Base_Core::NonReEntrantThreadLockable m_MapLock;

		// Set this Variable to enforce limit on the container.
		unsigned long m_ulMaxAlarmCount;
	};

}

#endif // __ALL_ALARM_MANAGER_INCLUDED__
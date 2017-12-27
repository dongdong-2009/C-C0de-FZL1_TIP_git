/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_Review_Branch/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmStoreManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/19 10:41:03 $
  * Last modified by:  $Author: ouyang,zhilin $
  *
  */
///////////////////////////////////////////////////////////
//  SharedAlarmMonitor.h
//  Implementation of the Class SharedAlarmMonitor
//  Created on:      19-Jan-2011 10:41:03 AM
///////////////////////////////////////////////////////////

#if !defined(SharedAlarmMonitor_81506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_)
#define SharedAlarmMonitor_81506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_

#include <map> 
 
 
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#define  defMonitorInterval       10*1000      // unit: million seconds
#define  defNormalTime            180          // unit: seconds  AlarmAgentInterface::receiveAlarmMessage to AlarmStoreManager::newAlarm
#define  defDutyChangeTime        600          // unit: seconds  AlarmStoreServant::dutyChanged
#define  defLoadCurrentAlarms     360          // unit: seconds  AlarmAgentInterface. remove & insert alarms on shared memory
#define  defSeqFailedTimeOut      180          // unit: seconds  AlarmStoreManager::newAlarm
#define  defSeqFailedTimes        1           // sequence failed times
#define  defTryTimes              2            // the exception try times


// #define  defNormalTime            30 //180          // unit: seconds  AlarmAgentInterface::receiveAlarmMessage to AlarmStoreManager::newAlarm
// #define  defDutyChangeTime        30 //480          // unit: seconds  AlarmStoreServant::dutyChanged
// #define  defLoadCurrentAlarms     30 //360          // unit: seconds  AlarmAgentInterface. remove & insert alarms on shared memory



namespace TA_Base_Bus{

	/**
	 * This class will monitor all the alarm agent Interfaces.  It will
	 * ensure that the application work well when the alarm store processing
	 * messages. However, when one of the alarm agent interface blocked more 
	 * than the expected time, then the monitor will restart the application
	 * to workaround the problem
	 * 
	 * @version 1.0
	 * @created 19-Jan-2011 11:47:50 AM
	 */
    class SharedAlarmMonitor  
	{
	public:
		/**
	    * getInstance
	    * 
    	* SharedAlarmMonitor is a singleton, so this method returns a reference  
    	* to the active SharedAlarmMonitor object and creates a new SharedAlarmMonitor
    	*  if one does not exist already.
    	*
    	* @return  a reference to the active SharedAlarmMonitor singleton object
		*/
		static SharedAlarmMonitor& getInstance();

		/**
		* removeInstance
		* 
		* SharedAlarmMonitor is a singleton, so it wont delete itself
		* this should be called in the cleanup of your app 
		*
		*/
		static void removeInstance();

		/*
		* UpdateTimeBegin
		*
		* update the begin time of processing alarm store messages
		*
		* @ ulLocationKey: the identifier of the location/thread
		* @ ulTime: the begin time of the location/thread
		*/
		void updateTimeBegin(unsigned long ulLocationKey);

		/*
		* UpdateTimeEnd
		* 
		* update the end time of processing alarm messages
		* the rules as: normally all the alarm processing should completed
		* within 4 minutes, for duty change and reload current alarms, however,
		* the completed time adjust to 5 minutes
		*
		* @ ulLocationKey: the identifier of the location/thread
		* @ ulTime: the begin time of the location/thread
		*/
		void updateTimeEnd(unsigned long ulLocationKey);

		/*
		* updateDutyChange
		* 
		* update timeout value of all the locations/threads to defined 
		* duty change timeout value.
		* the rules as: normally all the alarm processing should completed
		* within 4 minutes, for duty change and reload current alarms, however,
		* the completed time adjust to 5 minutes
		*
		* @ ulLocationKey: the identifier of the location/thread
		* @ ulTime: the begin time of the location/thread
		*/
		void updateDutyChange(bool blnStart);


		void updateLoad(bool blnStart, unsigned long ulLocationKey );

		// for sequence exceptions workaround	 
		void updateSeqTime();

		int MonitorThreadsState();

	protected:
	 

		/**
		* The terminate() method should cause run() to return.
		* NOTE: probably not a good idea
		* to call this method directly - call terminateAndWait() instead
		*/
		virtual void terminate();
	

		// constructor and destructor
		SharedAlarmMonitor();
		virtual ~SharedAlarmMonitor();
        
	private:
		
		struct LocationKeyItem 
		{
			unsigned int  nTryTimes;
			unsigned long ulBeginTime;			
			unsigned long ulTimeOut;
			bool   enableNormalCheck;
			
			unsigned long ulLoadBeginTime;		 
			unsigned long ulLoadTimeOut;
            bool   enableLoadCheck;
			unsigned int  nTryLoadTimes;

			


			
			unsigned long ulSeqBeginTime;
			unsigned long ulSeqEndTime;
			unsigned long ulSeqFailedTimeOut;
			unsigned long ulSeqFailedTimes;

			
		 

			LocationKeyItem()
				: nTryTimes(0)	
				, ulBeginTime(0)
				, ulTimeOut(defNormalTime)	

				, nTryLoadTimes(0)
				, ulLoadBeginTime(0)
				, ulLoadTimeOut(defLoadCurrentAlarms)

				, ulSeqBeginTime(0)
				, ulSeqEndTime(0)
				, ulSeqFailedTimeOut(defSeqFailedTimeOut)
				, ulSeqFailedTimes(0), enableNormalCheck(false),enableLoadCheck(false)
			{
			 
			}
		};
        
	private:
		
	 
		
		int MonitorState();
		 

		bool MonitorNormal(unsigned long location,LocationKeyItem* item);
		bool MonitorLoad(unsigned long location,LocationKeyItem* item);
		bool MonitorDutyChange();
		bool MonitorSeqError();
		//void MonitorNormal(LocationKeyItem* item);
				
        typedef std::map<unsigned long, LocationKeyItem* > LocationKeyMap;
		typedef std::map<unsigned long, LocationKeyItem* >::value_type  mapValueType;
		typedef std::map<unsigned long, LocationKeyItem* >::iterator mapIterType;
 		typedef std::map<unsigned long, LocationKeyItem* >::const_iterator conIterType;

		// the container to store various locations/threads processing message
		// time. Location key plus pair Begin time & End time
	 	LocationKeyMap m_LocationKeyMap;

		// the only install of the class
		static SharedAlarmMonitor* m_pSharedAlarmMonitor;

		TA_Base_Core::ReEntrantThreadLockable m_lock;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		bool	m_terminate;

		unsigned long m_ulDutyBeginTime;		 
		//unsigned long ulDutyTimeOut;
		bool   m_enableDutyCheck;
		unsigned int  m_nTryDutyTimes;

		unsigned long m_unSeqErrorTimes;


 //      unsigned long   m_defNormalTimeOut;       // unit: seconds  AlarmAgentInterface::receiveAlarmMessage to AlarmStoreManager::newAlarm
       unsigned long   m_defDutyChangeTimeOut;           // unit: seconds  AlarmStoreServant::dutyChanged
 //      unsigned long   m_defLoadCurrentAlarmsTimeOut;        // unit: seconds  AlarmAgentInterface. remove & insert alarms on shared memory		 

	};  // end of class SharedAlarmMonitor

}  // end of namespace TA_Base_Bus


#endif // SharedAlarmMonitor_81506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_
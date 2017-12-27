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
//  AlarmStoreMonitor.h
//  Implementation of the Class AlarmStoreMonitor
//  Created on:      19-Jan-2011 10:41:03 AM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreMonitor_91506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_)
#define AlarmStoreMonitor_91506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_

#include <map> 
 
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

 


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
    class AlarmStoreMonitor : public TA_Base_Core::Thread
	{
	public:
		/**
	    * getInstance
	    * 
    	* AlarmStoreMonitor is a singleton, so this method returns a reference  
    	* to the active AlarmStoreMonitor object and creates a new AlarmStoreMonitor
    	*  if one does not exist already.
    	*
    	* @return  a reference to the active AlarmStoreMonitor singleton object
		*/
		static AlarmStoreMonitor& getInstance();

		/**
		* removeInstance
		* 
		* AlarmStoreMonitor is a singleton, so it wont delete itself
		* this should be called in the cleanup of your app 
		*
		*/
		static void removeInstance();

	 
 
	protected:
		/**
	    * This is the work method for the thread.  It will create monitor all 
		* locations/threads worked appropriate
		*/
	    virtual void run();

		/**
		* The terminate() method should cause run() to return.
		* NOTE: probably not a good idea
		* to call this method directly - call terminateAndWait() instead
		*/
		virtual void terminate();
	

		// constructor and destructor
		AlarmStoreMonitor();
		virtual ~AlarmStoreMonitor();
 
	private:
		
	 
	 
		// the only install of the class
		static AlarmStoreMonitor* m_pAlarmStoreMonitor;

	 
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		bool	m_terminate;

	 
		 

	};  // end of class AlarmStoreMonitor

}  // end of namespace TA_Base_Bus


#endif // AlarmStoreMonitor_91506E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreIndexCallbackManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreIndexCallbackManager.h
//  Implementation of the Class AlarmStoreIndexCallbackManager
//  Created on:      24-Jan-2004 04:17:27 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreIndexCallbackManager_D4A8CA2A_E426_45cd_A539_2C8A59C9F4FD__INCLUDED_)
#define AlarmStoreIndexCallbackManager_D4A8CA2A_E426_45cd_A539_2C8A59C9F4FD__INCLUDED_

#include "IAlarmStoreIndexChanges.h"
#include "IAlarmStoreIndexDelete.h"
#include "core/alarm/IDL/src/AlarmStoreCallbackCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockableWithMutex.h"
#include "core/threads/src/Thread.h"
#include <map>
#include <string>


namespace TA_Base_Bus
{
	/**
	 * This class is responsible for managing a collection of corba objects which
	 * require notification of changes to a selected index via a callback mechanism.
	 * This is used in preference to the Notification service as it may be a local
	 * process and this mechanism avoids a trip to the server.
	 * 
	 * It also allows the complete index to be sent when the callback is first added
	 * making the initialisation process very simple for the application.
	 * 
	 * @version 1.0
	 * @created 24-Jan-2004 04:17:27 PM
	 */
	class AlarmStoreIndexCallbackManager : virtual public IAlarmStoreIndexChanges,
										   virtual public TA_Base_Core::Thread
	{

		public:
			explicit AlarmStoreIndexCallbackManager(IAlarmStoreIndexDelete& deleteCallback);
			virtual ~AlarmStoreIndexCallbackManager();

		public:
			/**
			 * Add a new callback for this filter.  Returns the unique id for the callback.
			 * The alarm store will update the indexID to be associated with the callbackID
			 * once the index has been identified or created.
			 */
			unsigned long addCallback(TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback);

			/**
			 * Remove a callback for this filter
			 * @param callbackID    ID for callback to be removed.
			 * 
			 */

			unsigned long getCallback(TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback);

			void removeCallback(unsigned long callbackID);
		protected:
			/**
			 * @param indexID    Identity of index being updated
			 * 
			 */
			virtual void indexCleared(unsigned long callbackID, long indexID);

			/**
			 * Index deleted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index
			 * @param alarmID    Alarm UUID
			 * 
			 */
			virtual void indexDeleted(unsigned long callbackID, long indexID,unsigned long position, std::string alarmID);

			/**
			 * Called when an index entry is inserted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index where alarm inserted
			 * @param alarmID
			 * 
			 */
			virtual void indexMoved(unsigned long callbackID, long indexID, unsigned long oldPosition,
				                    unsigned long newPosition, std::string alarmID);

			/**
			 * Called when an index entry is inserted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index where alarm inserted
			 * @param alarmID
			 * 
			 */
			virtual void indexInserted(unsigned long callbackID, long indexID, unsigned long position, std::string alarmID);


			/**
			 * This is the work method for the thread.  It will monitor the callbacks
			 * to ensure they are still ok.
			 */
			virtual void run();

			/**
			 * The terminate() method should cause run() to return. eg: CorbaUtil::
			 * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
			 * to call this method directly - call terminateAndWait() instead
			 */
			virtual void terminate();


		private:

			AlarmStoreIndexCallbackManager(const AlarmStoreIndexCallbackManager& theAlarmStoreIndexCallbackManager);
			AlarmStoreIndexCallbackManager& operator = (const AlarmStoreIndexCallbackManager& theAlarmStoreIndexCallbackManager);

			/**
			 * List of callbackIDs each with their associated corba call back pointer.
			 */
			typedef std::map<unsigned long,TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr>  CorbaCallbackList;

			CorbaCallbackList  m_corbaCallbackList;
			unsigned long m_nextCallbackID;

			IAlarmStoreIndexDelete& m_deleteCallback;

			/*
			 * Lock to protect generation and removal of callbacks.
			 */
			TA_Base_Core::ReEntrantThreadLockableWithMutex       m_callbackLock;
			

			bool									m_terminate;


	};
};
#endif // !defined(AlarmStoreIndexCallbackManager_D4A8CA2A_E426_45cd_A539_2C8A59C9F4FD__INCLUDED_)

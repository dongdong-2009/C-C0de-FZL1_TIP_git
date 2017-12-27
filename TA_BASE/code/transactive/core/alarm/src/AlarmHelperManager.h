#if !defined(AlarmHelperManager_D72626D1_6819_4640_9BA0_97379530746F__INCLUDED_)
#define AlarmHelperManager_D72626D1_6819_4640_9BA0_97379530746F__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelperManager.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class AlarmHelper;
	class NonUniqueAlarmHelper;
    class AlarmModificationHelper;
	class AlarmHelperBase;

	class AlarmHelperManager
	{
	public:

		/**
		* destructor
		*/
		virtual ~AlarmHelperManager();

		/**
		* getInstance
		*
		* Get the AlarmHelperManager  object
		* 
		* @return	AlarmHelperManager	the alarm helper object
		*/
		static AlarmHelperManager& getInstance();

		/**
		* cleanUp
		*
		* clean up the necessary bits
		*/
		void cleanUp();

		/**
		* getAlarmHelper
		*
		* Get an alarm helper object. It is used for submitting alarms, and closing alarms.
		* Generally used by agents that will generate alarms that are unique(alarmTypeKey and entityKey)
		*
		* @return	AlarmHelper	an alarm helper
		*/
		AlarmHelper& getAlarmHelper();

		/**
		* getNonUniqueAlarmHelper
		*
		* Get a Non Unique alarm helper. This is used for submitting and closing alarms that 
		* may not be unique in the system, Alarms need to be closed by specifying the Alarm ID
		*
		* @return	NonUniqueAlarmHelper	a NonUniqueAlarmHelper
		*/
		NonUniqueAlarmHelper& getNonUniqueAlarmHelper();


		/**
		* getModificationAlarmHelper
		*
		* Get an alarm helper object. It is used for acking alarms, closing alarms, modification alarm
        * comments etc.
        *
        * NOTE: This should ONLY be used by the alarm list control at this time. This is not
        * a helper for any application to use. It should only be used by applications that allow the user
        * to acknowledge and close alarms.
		*
		* @return	AlarmModificationHelper	an alarm helper
		*/
    	AlarmModificationHelper& AlarmHelperManager::getModificationAlarmHelper();


	private:
		/**
		* The constructor is declared private as this is a singleton
		*/
		AlarmHelperManager();

		/**
		* disable default copy constructor
		*/
		AlarmHelperManager( const AlarmHelperManager& theAlarmHelperManager);
		AlarmHelperManager& operator = ( const AlarmHelperManager& theAlarmHelperManager);

	private:
        /** 
          * initialiseAlarmHelperBase
          *
          * This function will create the AlarmHelperBase in a thread safe manner
          */
		void initialiseAlarmHelperBase();

		/**
		* the one and only AlarmHelperManager
		*/
		static AlarmHelperManager* m_theAlarmHelperManager;

		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		/**
		* protect the alarmHelperBase creation
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_alarmHelperBaseLock;

		/**
		* store the one & only alarmHelperBase class
		*/
		AlarmHelperBase* m_alarmHelperBase;

		//TD17994,jianghp
		AlarmHelper * m_alarmHelper;

		NonUniqueAlarmHelper * m_nonUniqueAlarmHelper;

		AlarmModificationHelper * m_alarmModificationHelper;
		//TD17994,jianghp
	};


}; // namespace TA_Base_Core

#endif // !defined(AlarmHelperManager_D72626D1_6819_4640_9BA0_97379530746F__INCLUDED_)

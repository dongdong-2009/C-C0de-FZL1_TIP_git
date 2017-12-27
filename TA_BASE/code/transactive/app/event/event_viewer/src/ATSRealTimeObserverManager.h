/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSEventObserverManager.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/21 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSEventObserverManager is for ATS real time refresh data. When the ATS real time refresh data comes, it will call
  * a global callback function. And in that function, we need it notify the observer. But since we can not use 
  * IEventWorkObserver::EventNotification, because it's called by a local instance, which is not allowed in a global 
  * function. To solve this, we use this class and make it as singleton. Then we call use the static instance to call 
  * the notify function.
  *
  */
#pragma once

#include "IEventWorkObserver.h"
#include "FilterCriteria.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include <vector>

namespace TA_Base_App{
	class ATSRealTimeObserverManager
	{
	public:

		/** 
		 * constructor
		 */
		ATSRealTimeObserverManager(void);

		/** 
		 * desctructor
		 */
		~ATSRealTimeObserverManager(void);

		/** 
		 * addObserver
		 *
		 * add the given IEventWorkObserver to the static list m_observerList
		 *
		 * @param observer, the given point of IEventWorkObserver.
		 */
		void addObserver( IEventWorkObserver* observer );

		void removeObserver();

		/** 
		 * getInstance
		 *
		 * get the point of the singleton of the class if it's already created. 
		 * If it hasn't been created, then create it first then return it.
		 *
		 * @return ATSRealTimeObserverManager * , the point of the singleton 
		 */
		static ATSRealTimeObserverManager* getInstance();

		/** 
		 * freeInstance
		 *
		 * free the singleton of the class if it's already created. 
		 * If it hasn't been created, do nothing.
		 */
		static void freeInstance();

		/** 
		 * setTaskInfo
		 *
		 * set the static member curTaskID and filterCriteria by the given parameter.
		 *
		 * @param taskID, the new value to be set to static member: curTaskID
		 * @param filter, the new value to be set to static member: filterCriteria
		 */
		void setTaskInfo(unsigned long taskID, FilterCriteria filter);

		/** 
		 * getFilterCriteria
		 *
		 * return the value of static member filterCriteria 
		 *
		 * @return FilterCriteria, the current value of the static member filterCriteria
		 */
		FilterCriteria getFilterCriteria ();

		/** 
		 * getTaskID
		 *
		 * return the value of static member curTaskID 
		 *
		 * @return unsigned long, the current value of the static member curTaskID
		 */
		unsigned long getTaskID ();

		/** 
		 * notifyObserverList
		 *
		 * for each IEventWorkObserver* in m_observerList (actually it will be only one), let them call 
		 * IEventWorkObserver::EventNotification with the given parameter.
		 *
		 * @param command, the error code for the current notification data.
		 * @param events, the vctor of ICombinedEventData* to be passed to the observer
		 * @param taskID, the current taskID.
		 */
		void notifyObserverList(const ActionCommand command, std::vector<TA_Base_Core::ICombinedEventData*>& events, unsigned long taskID );

		void setATSSubscriptionErr(long error);

	private:
		static ATSRealTimeObserverManager* m_instance;
		static std::vector< IEventWorkObserver* > m_observerList;
		static unsigned long curTaskID;
		static FilterCriteria filterCriteria;
	};

};

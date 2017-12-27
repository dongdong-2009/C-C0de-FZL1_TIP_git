#include "ATSRealTimeObserverManager.h"


namespace TA_Base_App{
	ATSRealTimeObserverManager* ATSRealTimeObserverManager::m_instance = NULL;
	std::vector< IEventWorkObserver* > ATSRealTimeObserverManager::m_observerList;
	unsigned long ATSRealTimeObserverManager::curTaskID = 0;
	FilterCriteria ATSRealTimeObserverManager::filterCriteria ;

	ATSRealTimeObserverManager::ATSRealTimeObserverManager(void)
	{
	}

	ATSRealTimeObserverManager::~ATSRealTimeObserverManager(void)
	{
	}

	ATSRealTimeObserverManager* ATSRealTimeObserverManager::getInstance(){
		if(m_instance ==NULL){
			m_instance = new ATSRealTimeObserverManager();
		}
		return m_instance;
	}

	void ATSRealTimeObserverManager::freeInstance()
	{
		if(m_instance!=NULL)
		{
			delete m_instance;
			m_instance = NULL;
		}

	}
	void ATSRealTimeObserverManager::addObserver( IEventWorkObserver* observer ){
		m_observerList.push_back(observer);
	}

	void ATSRealTimeObserverManager::removeObserver()
	{
		m_observerList.clear();
	}
	void ATSRealTimeObserverManager::setATSSubscriptionErr(long error)
	{
		for(int i=0; i< (int)m_observerList.size();i++){
			if(m_observerList[i]!= NULL)
			{
				m_observerList[i]->setATSSubscriptionErr(error);
			}

		}

	}

	void ATSRealTimeObserverManager::notifyObserverList(const ActionCommand command, std::vector<TA_Base_Core::ICombinedEventData*>& events, unsigned long taskID )
	{
		for(int i=0; i<(int)m_observerList.size();i++){
			if(m_observerList[i]!= NULL)
			{
				m_observerList[i]->EventNotification(TA_Base_Core::ATSCategory, events,command, taskID);
			}
			
		}
	}

	void ATSRealTimeObserverManager::setTaskInfo( unsigned long taskID, FilterCriteria filter)
	{
		curTaskID = taskID;
		filterCriteria = filter;
	}

	FilterCriteria ATSRealTimeObserverManager::getFilterCriteria ()
	{
		return filterCriteria;
	}

	unsigned long ATSRealTimeObserverManager::getTaskID ()
	{
		return curTaskID;
	}

};

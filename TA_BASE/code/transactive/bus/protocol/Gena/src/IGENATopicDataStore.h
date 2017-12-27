
/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: IGenaDataStore.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the interface definition for Gena Data Store.
  */

#ifndef __IGENA_TOPIC_DATA_STORE_H_INCLUDED__
#define __IGENA_TOPIC_DATA_STORE_H_INCLUDED__

#include "bus/protocol/Gena/src/GENATopicData.h"
#include "bus/protocol/Gena/src/IGENADataStoreObserver.h"
#include <set>

namespace TA_Base_Bus
{
	class IGENATopicDataStore
		//: public ITopicListener
	{
	public:
		/* 
		 * updateData
		 * @purpose: notifies all observers for any changes on the data.
		 * @param:	topicName - name of the Gena topic.
		 *			pData - pointer to GenaTopicData
		 *
		 * @return: none
		 */
		void updateData(const std::string& topicName, GENATopicDataPtr pData)
		{
			updateDataImpl(topicName, pData);
			notifyObservers(topicName, pData);
		}

		/* 
		 * getAllData
		 * @purpose: get all available data 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: all the data with topic name equal to the parameter.
					empty list is returned if no data is available.
		 */
		virtual T_GENATopicDataList getAllData(const std::string& topicName ) = 0;
		
		/* 
		 * synchronizeBegin
		 * @purpose: an interface called when we start synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeBegin(const std::string& topicName) = 0;

		/*  
		 * synchronizeEnd
		 * @purpose: an interface called when we receive an end synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeEnd(const std::string& topicName) = 0;

		/* 
		 * registerDataChangeObserver
		 * @purpose: function is used for registration of an observer to this class to start receiving
		 * @param: pObserver - Interface pointer to IGENADataStoreObserver
		 *
		 * @return: none
		 */
		void registerDataChangeObserver(IGENADataStoreObserver* pObserver)
		{
			std::set<IGENADataStoreObserver*>::iterator itr = m_observers.find(pObserver);
			if (itr == m_observers.end())
			{
				m_observers.insert(pObserver);
			}
		}

		/* 
		 * unRegisterDataChangeObserver
		 * @purpose: function is used for unregistration of an observer to this class
		 * @param: pObserver - Interface pointer to IGENADataStoreObserver
		 *
		 * @return: none
		 */
		void unRegisterDataChangeObserver(IGENADataStoreObserver* pObserver)
		{
			std::set<IGENADataStoreObserver*>::iterator itr = m_observers.find(pObserver);
			if (itr != m_observers.end())
			{
				m_observers.erase(itr);
			}
		}

	protected:
		/* 
		 * updateDataImpl
		 * @purpose: updates the data store and sends out notification to notify observers
		 * @param: topicName - name of the Gena topic.
		 * @param: pData - data containing the update
		 *
		 * @return: none
		 */
		virtual void updateDataImpl(const std::string& topicName, GENATopicDataPtr pData) = 0;

	private: // Methods
		/* 
		 * notifyObservers
		 * @purpose: notifies all observers for any changes on the data.
		 * @param:	topicName - name of the Gena topic.
		 *			pData - pointer to GenaTopicData
		 *
		 * @return: none
		 */
		void notifyObservers(const std::string& topicName, GENATopicDataPtr pData)
		{
			std::set<IGENADataStoreObserver*>::iterator itr = m_observers.begin();
			for (itr; itr!=m_observers.end(); itr++)
			{
				(*itr)->onDataChange(topicName, pData);
			}
		}

	private: // Variables
		std::set<IGENADataStoreObserver*> m_observers;
	};
}
#endif //__IGENA_TOPIC_DATA_STORE_H_INCLUDED__
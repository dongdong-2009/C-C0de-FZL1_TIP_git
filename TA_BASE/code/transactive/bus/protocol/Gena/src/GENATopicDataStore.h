
#ifndef __GENA_DATA_STORE_H_INCLUDED__
#define __GENA_DATA_STORE_H_INCLUDED__

#include "IGENATopicDataStore.h"

namespace TA_Base_Bus
{
	typedef std::map<std::string,T_GENATopicDataList>	T_DataStoreMap;
	typedef T_DataStoreMap::iterator					T_DatatStoreIter; 

	class GENATopicDataStore
		: public IGENATopicDataStore
	{
	public:
		GENATopicDataStore();
		~GENATopicDataStore();

		/* 
		 * getAllData
		 * @purpose: get all available data 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: all the data with topic name equal to the parameter.
					empty list is returned if no data is available.
		 */
		virtual T_GENATopicDataList getAllData(const std::string& topicName );
		
		/* 
		 * synchronizeBegin
		 * @purpose: an interface called when we start synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeBegin(const std::string& topicName);

		/*  
		 * synchronizeEnd
		 * @purpose: an interface called when we receive an end synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeEnd(const std::string& topicName);

	protected:
		/* 
		 * updateDataImpl
		 * @purpose: updates the data store and sends out notification to notify observers
		 * @param: topicName - name of the Gena topic.
		 * @param: pData - data containing the update
		 *
		 * @return: none
		 */
		virtual void updateDataImpl(const std::string& topicName, GENATopicDataPtr pData);

	private:
		T_DataStoreMap m_DataStoreMap;
	};

	
}

#endif //__GENA_DATA_STORE_H_INCLUDED__
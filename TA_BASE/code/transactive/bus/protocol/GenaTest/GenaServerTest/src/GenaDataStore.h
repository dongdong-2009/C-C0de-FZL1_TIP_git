
#pragma once

#include "bus/protocol/Gena/src/IGENATopicDataStore.h"
#include "bus/protocol/Gena/src/GENATopicDataStore.h"
using namespace TA_Base_Bus;

class GenaDataStore
	: public IGENATopicDataStore
{
public:
	GenaDataStore(void);
	~GenaDataStore(void);

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

	void setTopicData(std::string topicName, T_GENATopicDataList objTopicData)
	{
		m_dataStoreMap[topicName] = objTopicData;
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
	virtual void updateDataImpl(const std::string& topicName, GENATopicDataPtr pData);

private:
	TA_Base_Bus::T_DataStoreMap m_dataStoreMap;
};

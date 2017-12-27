///////////////////////////////////////////////////////////
//  DataLogQueue.cpp
//  Implementation of the Class DataLogQueue
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:17
//  Original author: luohuirong
///////////////////////////////////////////////////////////
#include <time.h>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h" 

#include "core/data_access_interface/datalog/src/DatalogRecordingData.h"
#include "core/data_access_interface/datalog/src/DatalogRecordingAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "DataLogQueue.h"
#include "LocalRunParamDefinitions.h"
#include "DataLogManager.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;


using namespace  TA_Base_Core;
namespace TA_Base_Bus
{
	const int DataLogQueue::SLEEP_TIME = 300; //ms

	DataLogQueue::DataLogQueue(unsigned long locationKey,std::string dbLogTableName):
	m_terminated(false),m_locationKey(locationKey),m_dbLogTableName(dbLogTableName)
	{

		m_batchUpdateSize = DataLogManager::getInstance()->getBatchUpdateSize();
		
		m_dbWriteInterval = SLEEP_TIME;

		m_timeout = DataLogManager::getInstance()->getDataMaxTimeout();

		setLastSavedTime(0);
	}

	void DataLogQueue::setBatchSize(int batchSize)
	{
		m_batchUpdateSize = batchSize;
	}

	void DataLogQueue::setTimeout(int timeout)
	{
		m_timeout = timeout;
	}

	DataLogQueue::~DataLogQueue()
	{
		terminateAndWait();
		freeCache();
	}

	void DataLogQueue::insertDataLogToCache(DataLogValues & values)
	{
		if (values.size()>0)
		{
			ThreadGuard guard(m_datasLock);
			m_datas.insert(m_datas.end(),values.begin(), values.end());
		}
	}

	void DataLogQueue::deleteLogFromCacheByTime(time_t lastSavedTime)
	{
		ThreadGuard guard(m_datasLock);
		time_t topRecordTime;
		DataLogValue * tempValue = NULL;

		while ( m_datas.size()>0 )
		{
			topRecordTime = mktime(&( m_datas.front()->getCreateTime()));

			// If the createtime is before lastSavedTime, then delete this record
			if(  topRecordTime != (-1) && topRecordTime < lastSavedTime )
			{
				//release the top record:
				tempValue = m_datas.front();
				if(tempValue!=NULL)
				{
					delete tempValue;
					tempValue = NULL;
				}
				m_datas.pop_front() ;
			}
			else if ( topRecordTime >= lastSavedTime )
			{
				break;
			}

		}

	}

	void DataLogQueue::run()
	{
		while(!m_terminated)
		{
			try
			{
				if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL))
				{
					ThreadGuard guard(m_datasLock);
					if(m_datas.size()>0)
					{
						writeCacheToDB();

					}
				}
				else if (0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR))
				{
					deleteTimeOutRecordsFromCache();
				}
			}			 
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoints is failed to saving.");

			}			
			sleep( m_dbWriteInterval   ); 
			
		}
	}


	void DataLogQueue::terminate()
	{
		m_terminated= true;

	}

	//save by batch size
	void DataLogQueue::writeCacheToDB()
	{
		ThreadGuard guard(m_datasLock);
		
		if(m_datas.size()==0)
		{
			return;
		}
		
		std::vector<TA_Base_Core::DatalogRecordingData*> records;

		while ( m_datas.size()>0)
		{
			// Pop up a batch from m_datas to records, from top
			popupABatch ( records);

			// save this batch to DB:
			bool ret=false;
			ret=TA_Base_Core::DatalogRecordingAccessFactory::getInstance().insertRecords(records,m_dbLogTableName);
			if(ret)
			{
				//insert to DB successfully, save the latest record's create time
				int batchSize = records.size();
				if (batchSize>0)
				{				 
					if (records[batchSize-1] != NULL)
					{
						tm lastSavedTime = records[batchSize-1]->getCreateTime();
						time_t lastSavedTime_timet = mktime(&lastSavedTime);
						if( lastSavedTime_timet != -1 )
						{
							setLastSavedTime( lastSavedTime_timet );
						}
					}
				}				 
			}

			// free the temp vector
			freeDatalogRecordingDatas(records);

		}
	}

	// Pop up a batch from m_datas to records, from top
	void DataLogQueue::popupABatch ( std::vector<TA_Base_Core::DatalogRecordingData*> & dest)
	{
		if(dest.size()>0)
		{
			freeDatalogRecordingDatas(dest);
		}

		ThreadGuard guard(m_datasLock);

		//DataLogValues::iterator itr= m_datas.begin();
		DatalogRecordingData * tempDestData = NULL;
		DataLogValue * tempSourceData = NULL;
		int count=0;
		while ( count<m_batchUpdateSize && m_datas.size()>0 )
		{
			// create a new item and push it to destination
			tempSourceData = m_datas.front();
			tempDestData = new DatalogRecordingData(tempSourceData->getCreateTime(), tempSourceData->getEntityKey(),
				tempSourceData->getValue(), m_locationKey , tempSourceData->getPlanLogTime());

			dest.push_back(tempDestData);
			count++;

			// free and pop up the first item from souce 
			delete tempSourceData;
			tempSourceData = NULL;
			m_datas.pop_front();
		}
		
	}

	time_t DataLogQueue::getLastSavedTime()
	{
		ThreadGuard guard(m_lastSavedTimeLock);
		return m_lastSavedTime;
	}

	void DataLogQueue::setLastSavedTime ( time_t time)
	{
		ThreadGuard guard(m_lastSavedTimeLock);
		m_lastSavedTime = time;
	}
	void DataLogQueue::freeDatalogRecordingDatas(std::vector<DatalogRecordingData*>& records)
	{
		unsigned long i=0;
		for (i=0; i<records.size(); i++)
		{
			DatalogRecordingData* tempRecord = records[i];
			if (tempRecord != NULL)
			{
				delete tempRecord;
				tempRecord = NULL;
			}
		}
		records.clear();
	}

	void DataLogQueue::freeCache()
	{
		ThreadGuard guard(m_datasLock);
		for (DataLogValues::iterator itr= m_datas.begin(); itr!= m_datas.end() ; itr++)
		{
			DataLogValue* tempValue = *itr;
			if (tempValue != NULL)
			{
				delete tempValue;
				tempValue = NULL;
			}
		}
		m_datas.clear();
	}

	void DataLogQueue::deleteTimeOutRecordsFromCache()
	{
		struct timeb currentTime;
		ftime( &currentTime );

		time_t topRecordTime;

		ThreadGuard guard(m_datasLock);
		while ( m_datas.size()>0 )
		{
			topRecordTime = mktime(&( m_datas.front()->getCreateTime()));

			if( currentTime.time - topRecordTime > m_timeout )
			{
				//release the top record:
				DataLogValue * tempValue = m_datas.front();
				if(tempValue!=NULL)
				{
					delete tempValue;
					tempValue = NULL;
				}
				m_datas.pop_front() ;
			}
			else 
			{
				break;
			}
			
		}

	}

}
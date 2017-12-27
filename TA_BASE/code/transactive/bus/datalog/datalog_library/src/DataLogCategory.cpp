///////////////////////////////////////////////////////////
//  DataLogCategory.cpp
//  Implementation of the Class DataLogCategory
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:15
//  Original author: luohuirong
///////////////////////////////////////////////////////////
#include "ace/OS.h" 
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "DataLogCategory.h"
#include "ScadaDataProxy.h"
#include "LocalRunParamDefinitions.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/datalog/src/DatalogSettingsAccessFactory.h"
#include <vector>

using namespace TA_Base_Core;
namespace TA_Base_Bus
{

	static const int SLEEP_TIME = 300;    //ms
	static const int TIME_CHANGED_SPAN_TIME = 3600;    //1 hour
	
	DataLogCategory::~DataLogCategory()
	{
		// free  m_DataLogItemMap
	    terminateAndWait();
		freeDataLogItemMap();

		// free m_dataLogQueue
		if(m_dataLogQueue!= NULL)
		{
			delete m_dataLogQueue;
			m_dataLogQueue = NULL;
		}
	}

	
	DataLogCategory::DataLogCategory(
		unsigned long pkey,
		unsigned long locationKey, 
		TA_Base_Core::ConfigType configType, 
		std::string& dpTableName,
		std::string& dpGrpTableName, 
		std::string& dpLogTableName,
		unsigned long versionNum,
		std::string& dpViewTableName,
		int maxDelayTimes ,
		int timeoutDelay )
		:m_pkey(pkey), m_locationKey(locationKey), m_configType(configType),m_dpTableName(dpTableName),
		m_dpGrpTableName(dpGrpTableName), m_dpLogTableName(dpLogTableName), m_versionNum(versionNum),
		m_dpViewTableName(dpViewTableName), m_maxDealyTimes(maxDelayTimes), m_timeoutDelay(timeoutDelay),m_terminated(false)
	{
		m_sleepTime = SLEEP_TIME;
		m_dataLogQueue=new DataLogQueue( m_locationKey, m_dpLogTableName );
		
	}	


	void DataLogCategory::deleteLogFromCacheByTime(time_t lastSavedTime)
	{
		m_dataLogQueue->deleteLogFromCacheByTime(lastSavedTime);
	}

	time_t DataLogCategory::getLastSavedTimeFromQueue ()
	{
		if(m_dataLogQueue!=NULL)
		{
			return m_dataLogQueue->getLastSavedTime();
		}
		else
		{
			return 0;
		}
		
	}

	void DataLogCategory::setLastSavedTime( time_t time)
	{
		m_lastSavedTime = time;
	}

	/**
	 * load monitored datalog item from database
	 * create datalogDPItem
	 */
	bool DataLogCategory::Init()
	{
        //load data from database

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading DP data from view: %s",m_dpViewTableName.c_str());
		std::vector <DatalogDPData *> datalogDPDatas = TA_Base_Core::DatalogSettingsAccessFactory::getInstance().getDatalogDPData(m_locationKey,m_dpViewTableName); 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loaded  DP data from view: %s. total %d DP datas.", m_dpViewTableName.c_str(), datalogDPDatas.size());

	    //create DataLogDPItem, and initialize attributes like "nextlogtime" ...
		ThreadGuard guard (m_DataLogItemMapLock);
		unsigned long i=0;
		for(i=0; i<datalogDPDatas.size(); i++)
		{
			DataLogDPItem * newItem = new DataLogDPItem( datalogDPDatas[i],m_timeoutDelay);
			newItem->initNextLogTime();

			time_t firstPlanLogTime = newItem->getPlanLogTime();
			struct tm firstPlanLogTime_tm ;
			ACE_OS::localtime_r( &firstPlanLogTime, &firstPlanLogTime_tm);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "A DP's planlogTime has been initialized: DP.entitykey = %ld, DP.planlogTime is %02d/%02d/%04d %02d:%02d:%02d", 
				newItem->getEntityKey(),
				firstPlanLogTime_tm.tm_mday, firstPlanLogTime_tm.tm_mon + 1,
				firstPlanLogTime_tm.tm_year + 1900, firstPlanLogTime_tm.tm_hour, firstPlanLogTime_tm.tm_min, firstPlanLogTime_tm.tm_sec);

			m_DPItemMap.insert(std::map<unsigned,DataLogDPItem*>::value_type(newItem->getPKey(), newItem) );
			
			ScadaDataProxy::getInstance()->addDataPoint(newItem->getEntityKey());
		}
		
		return  true;
	}

	bool DataLogCategory::entityKeyExits(unsigned long entityKey)
	{
		bool exist = false;
		ThreadGuard guard(m_DataLogItemMapLock);
		DataLogDPItem * dpItem = NULL;
		for(DataLogDPItemMap::iterator itr = m_DPItemMap.begin(); itr != m_DPItemMap.end(); itr++ )
		{
			dpItem = itr->second;
			if( dpItem->getEntityKey() == entityKey)
			{
				exist = true;
				return exist;
			}			
		}
		return exist;
	}
	std::vector<unsigned long> DataLogCategory::getEntityKeys()
	{
		std::vector<unsigned long> entityKeys;
		ThreadGuard guard(m_DataLogItemMapLock);
		DataLogDPItem * dpItem = NULL;
		for(DataLogDPItemMap::iterator itr = m_DPItemMap.begin(); itr != m_DPItemMap.end(); itr++ )
		{
			dpItem = itr->second;
			entityKeys.push_back(dpItem->getEntityKey());
		}
		return entityKeys;
		
	}

	//todo: to be deleted, Just for testing
	std::map<unsigned long, DataLogDPItem*> DataLogCategory::getDPItemMap()
	{
		return m_DPItemMap;
	}

	bool DataLogCategory::reInit()
	{
		
		freeDataLogItemMap();		
		return Init();
	}


	
	/**
	 * start the log thread
	 */
	void DataLogCategory::StartLog()
	{
		//starting log thread
		start();
		m_dataLogQueue->start();
	
	}


	void DataLogCategory::StopLog()
	{
	    //stop log process
		m_dataLogQueue->terminateAndWait();
		terminateAndWait();
		
	}


	void DataLogCategory::terminate()
	{
		m_terminated=true;
	}

	void DataLogCategory::pullDataLogValuesForEachDP ( std::list<DataLogValue*> &datalogValues )
	{

		double DPvalue;
		DataLogDPItem * dpItem = NULL;
		unsigned long entityKey=0;
		struct timeb currentTime_timeb;

		//for each datalogDPItem in the map
		ThreadGuard guard(m_DataLogItemMapLock);
		for ( DataLogDPItemMap::iterator itr = m_DPItemMap.begin(); itr != m_DPItemMap.end(); itr++ )
		{
			dpItem = itr->second;

			ftime( &currentTime_timeb ); // get current time			

			// if plan log time of this data point has reach.
			if( dpItem->getPlanLogTimeAddByDelay()  <= currentTime_timeb.time )
			{
				try
				{
					// get this data point value and log to datalogQueue // get dp value successfully: 
					bool ret=false;
					entityKey=dpItem->getEntityKey();
					ret=ScadaDataProxy::getInstance()->getDataPointValue( entityKey,DPvalue );					
					if( ret)
					{
						//if |value - previousValue|>=delta, use this , otherwise ignore it
						if( dpItem->checkNewValueForDelta( DPvalue)) 
						{
							addNewRecordToDatalogValues( datalogValues, 
								dpItem->getPlanLogTime(),dpItem->getEntityKey(), DPvalue);						
							dpItem->setCurrentValue( DPvalue );
						}

						dpItem->moveToNextPlanLogTime();

					}				
					else
					{
						// failed to get data point value , and delayed times haven't reach maximum, 
						// and next planlogtime won't be met after adding delay time.
						if( dpItem->getDelayedTimes() < m_maxDealyTimes
							&& dpItem->getPlanLogTimeAddByDelay() + m_timeoutDelay <= dpItem->calculateNextPlanLogTime()) 
						{
							// delayed times ++
							dpItem->delay();						
						}
						else // has reached maxDelayTimes, or next Plan log time will be missed if we delay it.
						{
							// set nextlogtime as next planlogtime							
							dpItem->moveToNextPlanLogTime();
							//dpItem->initNextLogTime();
						}
					}
				}
				catch (...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Exception caught in DataLogCategory::pullDataLogValuesForEachDP");

				}
			}
		}
	}

	void DataLogCategory::addNewRecordToDatalogValues( std::list<DataLogValue*> & datalogValues, 
		time_t planLogTime_timet ,unsigned long entityKey, double value)
	{
		tm currentTime_tm, planLogTime_tm;

		// getCurrentTime and change the current time to tm type:
		struct timeb currentTime_timeb;
		ftime( &currentTime_timeb );   
		ACE_OS::localtime_r( &(currentTime_timeb.time), &currentTime_tm );

		// change the planlogtime to tm type:
		ACE_OS::localtime_r( &planLogTime_timet, &planLogTime_tm );

		datalogValues.push_back(new DataLogValue( currentTime_tm, entityKey, value, planLogTime_tm));

	}

	void DataLogCategory::run()
	{
		timeb lastPullTime;
		ftime(&lastPullTime);
		//start logging data point value
		while (!m_terminated)
		{

			timeb currentTime;
			ftime(&currentTime);
			try
			{
				ThreadGuard guard(m_DataLogItemMapLock);

				//to prevent the system time from being changed dynamically.
				if(currentTime.time - lastPullTime.time > TIME_CHANGED_SPAN_TIME || currentTime.time - lastPullTime.time < 0)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "System time has been changed!");
					reInit();
				}

				DataLogValues datalogValues;
				pullDataLogValuesForEachDP(datalogValues);

				m_dataLogQueue->insertDataLogToCache( datalogValues );

			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Exception caught in DataLogCategory run()");
				
			}

			lastPullTime = currentTime;

			//freeDatalogValues( datalogValues);

			sleep(m_sleepTime);
	
		}
	}

	void DataLogCategory::freeDatalogValues( DataLogValues & datalogValues)
	{
		for (DataLogValues::iterator itr= datalogValues.begin(); itr!= datalogValues.end() ; itr++)
		{
			DataLogValue* tempValue = *itr;
			if (tempValue != NULL)
			{
				delete tempValue;
				tempValue = NULL;
			}
		}
		datalogValues.clear();
	}

	void DataLogCategory::freeDataLogItemMap ()
	{
		ThreadGuard guard(m_DataLogItemMapLock);
		for( DataLogDPItemMap::iterator itr = m_DPItemMap.begin(); itr!=m_DPItemMap.end(); itr++)
		{
			DataLogDPItem * tempDPItem = itr->second;
			if( tempDPItem != NULL )
			{
				delete tempDPItem;
				tempDPItem = NULL;
			}
		}

		m_DPItemMap.clear();
	}

	unsigned long DataLogCategory::getPKey()
	{
		return m_pkey;
	}
	unsigned long DataLogCategory::getLocationKey()
	{
		return m_locationKey;
	}
	TA_Base_Core::ConfigType DataLogCategory::getConfigType()
	{
		return m_configType;
	}
	std::string DataLogCategory::getDPTableName()
	{
		return m_dpTableName;
	}
	std::string DataLogCategory::getDPGrpTableName()
	{
		return m_dpGrpTableName;
	}
	std::string DataLogCategory::getDPLogTableName()
	{
		return m_dpLogTableName;
	}
	unsigned long DataLogCategory::getVersionNum()
	{
		return m_versionNum;
	}
	void DataLogCategory::setVersionNum(unsigned long verNum)
	{
		m_versionNum = verNum;
	}

	bool DataLogCategory::CheckDPChanged()
	{
		bool bChanged = false;
		//Load new configuration from DB
		std::vector <DatalogDPData *> datalogDPDatas = TA_Base_Core::DatalogSettingsAccessFactory::getInstance().getDatalogDPData(m_locationKey,m_dpViewTableName); 
		//check difference 
		//test multithread condition
		ThreadGuard guard (m_DataLogItemMapLock);
		if(datalogDPDatas.size() != m_DPItemMap.size())
		{
			return true;
		}

		unsigned long i=0;
		for(i=0; i<datalogDPDatas.size(); i++)
		{
			TA_Base_Core::DatalogDPData* dpData = datalogDPDatas[i];
			DataLogDPItem * dpItem = m_DPItemMap[dpData->getDPKey()];
			if(dpItem == NULL)
			{
				bChanged = true;
				break;
			}
			else
			{
				double delta = dpData->getDeltaValue();
				if(delta < 0)
				{
					delta = (-1) * delta;
				}
				if((dpItem->getEntityKey() != dpData->getEntityKey())
					|| (delta != dpItem->getDeltValue()) 
					|| (dpData->getIntervalType() != dpItem->getIntervalType())
					|| (dpData->getSampleInterval() != dpItem->getSampleInterval())
					|| (dpData->getStartTime() != dpItem->getStartTime())
					|| (dpData->getDPGrpKey() != dpItem->getGrpID())
					|| (dpData->getDPGrpEnabled() != dpItem->getGroupEnabled())
					)
				{
					bChanged = true;
					break;
				}
			}
		}

		//delete temp data
		for(i=0; i<datalogDPDatas.size(); i++)
		{
			delete datalogDPDatas[i];
			datalogDPDatas[i] = NULL;
		}
		datalogDPDatas.clear();

		return bChanged;
		
	}
}
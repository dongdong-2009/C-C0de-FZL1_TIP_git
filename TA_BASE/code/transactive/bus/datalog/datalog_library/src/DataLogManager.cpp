///////////////////////////////////////////////////////////
//  DataLogManager.cpp
//  Implementation of the Class DataLogManager
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:13
//  Original author: luohuirong
///////////////////////////////////////////////////////////


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/NameValuePair.h" //20448++
#include "core/data_access_interface/entity_access/src/DataLogAgentEntityData.h"
#include "core/data_access_interface/datalog/src/DatalogSettingsAccessFactory.h"
#include "DataLogAgentImpl.h"
#include "DataLogCategory.h"
#include "DataLogManager.h"
#include "ScadaDataProxy.h"
#include "LocalRunParamDefinitions.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DataLogSyncSequence;
using TA_Base_Core::DatalogSettingsAccessFactory;
using TA_Base_Core::DataLogSyncCorbaDef;

namespace
{
	static const int DATA_MAX_TIMEOUT_DEFAULT(100); //sec. Will only save records( within latest 100secs) in Monitor Agent's cache
	static const int TIME_OUT_DELAY_DEFAULT(5);  //sec, delay 5 secs if scadaproxy didn't get DP value successfully
	static const int MAX_DELAY_TIMES_DEFAULT(3); // can only delay 3 times at most for each planed DP value
	static const int BATCH_UPDATE_SIZE_DEFAULT(1); // has 1 records in each batch when insert to DB

	static const short DATALOG_AGENT_TIMEOUT(300); // secs
	static const int MONITOR_SLEEP_TIME (10000);      // ms

}

namespace TA_Base_Bus
{

	DataLogManager* DataLogManager::m_instance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable DataLogManager::m_singletonLock;

	//
	// getInstance
	//
	DataLogManager* DataLogManager::getInstance()
	{
		ThreadGuard guard(m_singletonLock);

		if( m_instance == NULL )
		{
			m_instance = new DataLogManager();
		}
		return m_instance;
	}

	void DataLogManager::removeInstance()
	{
		ThreadGuard guard(m_singletonLock);

		if( m_instance != NULL )
		{
			delete m_instance;
			m_instance=NULL;
		}

		ScadaDataProxy::removeInstance();
		 
	}

	void DataLogManager::setupDataLogAgent(std::string name)
	{
		std::string corbaName=name + TA_Base_Bus::TA_DataLogAgent::DATALOG_ACCESS_INTERFACE;
		m_DataLogAgentAgentImpl = new TA_Base_Bus::DataLogAgentImpl(name);
		
		m_DataLogAgentAgentImpl->activateServantWithName(corbaName);
		m_controlDataLogAgent.setCorbaName(name, corbaName);
		//getAgentCorbaName();
		
	}

	void DataLogManager::getAgentCorbaName()
	{
		const std::string dataLogAgentType(TA_Base_Core::DataLogAgentEntityData::getStaticType());
		const unsigned long agentLocationKey = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

		TA_Base_Core::CorbaNameList dataLogAgentNames(
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(dataLogAgentType, agentLocationKey,true));

		if (dataLogAgentType.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is no datalog agent entity type");
			return;
		}
		else if (dataLogAgentNames.size() > 1)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is more than one datalog agent servant at location %li", agentLocationKey );
			return;
		}   
		// Set the named objects' corba name so that the DataLog Agent can be resolved correctly.
		m_controlDataLogAgent.setCorbaName(dataLogAgentNames[0]);
		// Extend the default corba timeout.
		m_controlDataLogAgent.setObjectTimeout(m_agentTimeout);

	}

	DataLogManager::DataLogManager()
	{
		m_terminated=false;
		m_moniter_sleep_time = MONITOR_SLEEP_TIME ;
		m_agentTimeout = DATALOG_AGENT_TIMEOUT;
		m_dataMaxTimeout=DATA_MAX_TIMEOUT_DEFAULT;
		m_timeoutDelay=TIME_OUT_DELAY_DEFAULT;
		m_batchUpdateSize=BATCH_UPDATE_SIZE_DEFAULT;		 
		m_maxDelayTimes=MAX_DELAY_TIMES_DEFAULT;
		//get corba name of DataLog Agent
		//getAgentCorbaName();		
	}

	void DataLogManager::terminate()
	{
		m_terminated=true;
	}

	DataLogManager::~DataLogManager()
	{
          this->terminateAndWait();
		  freeCategoryMap();

	}

	void DataLogManager::freeCategoryMap()
	{
		ThreadGuard guard(m_categorymapLock);
		DataLogCategory * temp = NULL;
		for( DataLogCategoryMap::iterator itr= m_CategoryMap.begin(); itr!=m_CategoryMap.end(); itr++)
		{
			temp = itr->second;			 
			delete temp;
			temp = NULL;
		}

		m_CategoryMap.clear();
	}
	void DataLogManager::handleStateMesssage(TA_Base_Core::DataLogSyncSequence*  datalogSync)
	{
		ThreadGuard guard(m_categorymapLock);
		if( datalogSync!=NULL)
		{			 
			unsigned long i=0;
			for ( i=0; i<datalogSync->length(); i++)
			{
				//configType is a redundant info in DataLogSyncSequence, currently we don't use it

				DataLogCategoryMap::iterator itr = m_CategoryMap.find( (*datalogSync)[i].pkey);
				if (itr != m_CategoryMap.end())
				{
					itr->second->deleteLogFromCacheByTime((*datalogSync)[i].latestLogTime);
				}
			}
		}

	}

	/**
	 * load monitored data point settings from database
	 */
	bool DataLogManager::init()
	{
		//load configuration setting from database
		const unsigned long agentLocationKey = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading Config Setting data from DB starts.");
		/*std::vector<DatalogConfigSettingsData*> configSettings = 
			DatalogSettingsAccessFactory::getInstance().getDatalogConfigSettingsData(agentLocationKey);*/
		
		std::vector<DatalogConfigSettingsData*> configSettings = 
			DatalogSettingsAccessFactory::getInstance().getDatalogConfigSettingsData();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading Config Setting data from DB ends. Get %d configSettings", configSettings.size());

		//create DataLogCategories, load & create datalog item from database 
		ThreadGuard guard(m_categorymapLock);
		DataLogCategory* configData = NULL;
		unsigned long i=0;
		for (i=0; i<configSettings.size(); i++)
		{	
			configData = createDataLogCategoryByConfigSetting(configSettings[i]);
			m_CategoryMap.insert(std::map<unsigned long,DataLogCategory*>::value_type(configData->getPKey(), configData) );
			configData->Init();

			//start log process
			configData->StartLog();	
		}

		// free the temperory data
		freeDataLogConfiSettings(configSettings);

        //start to monitor data points
        starConfigureSettingMonitor();
		
		return  true;
	}

	bool DataLogManager::addNewConfigSettings()
	{
		FUNCTION_ENTRY("addNewConfigSettings");

		//load configuration setting from database
		const unsigned long agentLocationKey = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

		std::vector<DatalogConfigSettingsData*> newConfigSettings = 
			DatalogSettingsAccessFactory::getInstance().getNewDatalogConfigSettingsData(agentLocationKey, getExistedPkeys());

		if(newConfigSettings.size()==0)
		{
			FUNCTION_EXIT;
			return  true;
		}

		//create DataLogCategories, load & create datalog item from database 
		ThreadGuard guard(m_categorymapLock);
		DataLogCategory* configData = NULL;
		unsigned long i=0;
		for (i=0; i<newConfigSettings.size(); i++)
		{	
			configData = createDataLogCategoryByConfigSetting(newConfigSettings[i]);

			m_CategoryMap.insert(std::map<unsigned long,DataLogCategory*>::value_type(configData->getPKey(), configData) );
			configData->Init();

			//start log process
			configData->StartLog();	
		}

		// free the temperory data
		freeDataLogConfiSettings(newConfigSettings);

		FUNCTION_EXIT;
		return  true;
	}

	void DataLogManager::deleteConfigSettingByPkeys(std::vector<unsigned long> pkeys)
	{
		for(int i=0; i<pkeys.size(); i++)
		{
			deleteConfigSettingByPkey( pkeys[i]);
		}
	}

	void DataLogManager::deleteConfigSettingByPkey( unsigned long pkey)
	{
		FUNCTION_ENTRY("deleteConfigSettingByPkey");

		//create DataLogCategories, load & create datalog item from database 
		ThreadGuard guard(m_categorymapLock);

		DataLogCategoryMap::iterator itr = m_CategoryMap.find(pkey);
		if( itr!=m_CategoryMap.end())
		{

			DataLogCategory* configData =itr->second;
			configData->StopLog();
			delete configData;
			configData = NULL;

			m_CategoryMap.erase(pkey);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "One DatalogConfigsetting with pkey: %ld has been deleted from DatalogManager. ", pkey);
		}

		FUNCTION_EXIT;

	}
	void DataLogManager::freeDataLogConfiSettings(std::vector<DatalogConfigSettingsData*> &configSettings)
	{
		DatalogConfigSettingsData* temp = NULL;
		unsigned long i=0;
		for(i=0 ; i<configSettings.size(); i++)
		{
			temp = configSettings[i];
			delete temp;
			temp = NULL;
		}
		configSettings.clear();
	}

	DataLogCategory * DataLogManager::createDataLogCategoryByConfigSetting(DatalogConfigSettingsData* configSetting)
	{
		unsigned long pkey = configSetting->getPkey();
		//unsigned long locationKey = configSetting->getLocationKey();
		unsigned long locationKey = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );
		TA_Base_Core::ConfigType configType = configSetting->getConfigType();
		std::string dpTableName = configSetting->getDPTableName();
		std::string dpGrpTableName = configSetting->getDPGrpTableName();
		std::string dpLogTableName = configSetting->getDPLogTableName();
		unsigned long versionNum = configSetting->getVersionNum();
		std::string dpViewTableName = configSetting->getDPViewTableName();

		return new DataLogCategory(pkey,locationKey, configType, dpTableName, dpGrpTableName, 
			dpLogTableName, versionNum, dpViewTableName,m_maxDelayTimes,m_timeoutDelay);
	}

	void DataLogManager::updateSyncInfo ()
	{
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{
			DataLogCategory * category = NULL;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "update control agent's latest sync time");

			ThreadGuard guard(m_categorymapLock);
			for( DataLogCategoryMap::iterator itr = m_CategoryMap.begin() ; itr!=m_CategoryMap.end() ; itr++)
			{
				category = itr->second;
				category->setLastSavedTime ( category->getLastSavedTimeFromQueue());
			}
		}
	}

	void DataLogManager::synclatestDataLog()
	{
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "call to control agent and get latest sync time");
			TA_Base_Core::DataLogSyncSequence* dataLogSyncSeq=NULL;
			//DataLogNamedObject m_controlDataLogAgent;
			try
			{
// 				m_controlDataLogAgent.setCorbaName("OccDataLogAgent", "OccDataLogAgent" );
// 				m_controlDataLogAgent.setObjectTimeout(m_agentTimeout);


				if ( m_controlDataLogAgent.getAgentName().empty() == false)
				{
					CORBA_CALL_RETURN( dataLogSyncSeq, (m_controlDataLogAgent), getAllDataLogSync, () );
					//CORBA_CALL( (m_controlDataLogAgent), callTest, () );

					

					handleStateMesssage(dataLogSyncSeq);

					// delete the sequence after use
					if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
				}

			}
			catch(const CORBA::SystemException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
			}
			catch(TA_Base_Core::ObjectResolutionException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the datalog agent");
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
				
			}
			catch(TA_Base_Core::OperationModeException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't load Alarm Cache or Queue from Control datalog Agent");
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
			}
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Can't load Alarm Cache or Queue from Control Alarm Agent.	Caught TransactiveException: %s", ex.what());
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
					" Caught Corba Exception");
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Unknown exception caught:Caught General Exception" );
				if (dataLogSyncSeq!=NULL) delete dataLogSyncSeq;
			}


		}
	}


	//monitor configure changes
	void DataLogManager::run()
	{	
		while (!m_terminated)
		{
			try
			{
				{
					ThreadGuard guard(m_categorymapLock);

					addNewConfigSettings();

					// for each category, check version number in DB
					DataLogCategory * category = NULL;
					std::vector<unsigned long> pkeysToBeDeleted;
					std::vector<unsigned long> oldEntityKeiesMightBeDeleted;

					for ( DataLogCategoryMap::iterator itr = m_CategoryMap.begin(); itr != m_CategoryMap.end(); itr++)
					{	
						// check the current version number in DB:
						category = itr->second;
						unsigned long newVersion ;
						bool res = DatalogSettingsAccessFactory::getInstance().loadVersionNumByKey( category->getPKey(), newVersion );

						if( res == false) // if this config data has been deleted
						{
							pkeysToBeDeleted.push_back(category->getPKey()); 
							collectOldEntityKeysFromCategory(category->getPKey(), oldEntityKeiesMightBeDeleted);
						}
						// if version updated
						else if( newVersion != category->getVersionNum() )
						{
							//ScadaDataProxy::getInstance()->releaseAllDataPoints();   // todo!  not correct here. to be modified
							// update related category
							category->setVersionNum(newVersion);
							if(category->CheckDPChanged())
							{
								collectOldEntityKeysFromCategory(category->getPKey(), oldEntityKeiesMightBeDeleted);
								category->reInit();
							}
						}
					}

					if(pkeysToBeDeleted.size()>0)
					{
						deleteConfigSettingByPkeys( pkeysToBeDeleted);
					}

					deleteEntityFromScadaProxyWhenNecessary(oldEntityKeiesMightBeDeleted);
				}

				updateSyncInfo();
				synclatestDataLog(); 
			}

			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Exception caught in DataLogManger run()");
			}
			
			sleep( m_moniter_sleep_time );
		}
	}


	void DataLogManager::collectOldEntityKeysFromCategory(unsigned long pkey, std::vector<unsigned long> & oldEntityKeiesMightBeDeleted)
	{
		ThreadGuard guard(m_categorymapLock);

		DataLogCategoryMap::iterator itr = m_CategoryMap.find(pkey);
		std::vector<unsigned long> entityKeys;
		if(itr!=m_CategoryMap.end())
		{
			DataLogCategory *category = itr->second;
			entityKeys = category->getEntityKeys();
			for (int i=0; i<entityKeys.size(); i++)
			{
				oldEntityKeiesMightBeDeleted.push_back(entityKeys[i]);
			}

		}
	}

	void DataLogManager::deleteEntityFromScadaProxyWhenNecessary(std::vector<unsigned long> oldEntityKeiesMightBeDeleted)
	{
		unsigned long entityKey=0;
		for(int i=0; i<oldEntityKeiesMightBeDeleted.size(); i++)
		{
			entityKey = oldEntityKeiesMightBeDeleted[i];
			if( !entityKeyExistInCategory(entityKey))
			{
				ScadaDataProxy::getInstance()->delDataPoint(entityKey);
			}
		}

	}

	bool DataLogManager::entityKeyExistInCategory(unsigned long entityKey)
	{
		bool exist = false;
		ThreadGuard guard(m_categorymapLock);
		for( DataLogCategoryMap::iterator itr= m_CategoryMap.begin(); itr!=m_CategoryMap.end(); itr++)
		{
			if( itr->second->entityKeyExits(entityKey))
			{
				exist = true;
				return exist;
			}
		}

		return exist;
	}

	std::vector<unsigned long> DataLogManager::getExistedPkeys()
	{
		std::vector<unsigned long> existedPkeys;

		ThreadGuard guard(m_categorymapLock);

		for( DataLogCategoryMap::iterator itr= m_CategoryMap.begin(); itr!=m_CategoryMap.end(); itr++)
		{
			existedPkeys.push_back(itr->second->getPKey());
		}
		return existedPkeys;

	}

	TA_Base_Core::DataLogSyncSequence* DataLogManager::getAllDataLogSync()
	{
		DataLogSyncSequence* seq = NULL;

		seq=new DataLogSyncSequence();

		ThreadGuard guard(m_categorymapLock);
		seq->length(m_CategoryMap.size());
		int i=0;

		for( std::map<unsigned long, DataLogCategory*>::iterator itr = m_CategoryMap.begin();itr!=m_CategoryMap.end(); itr++)
		{
			DataLogSyncCorbaDef* syncInfo=new DataLogSyncCorbaDef();
			(*seq)[i]=*syncInfo;
			
			//seq[i].pkey
			(*seq)[i].pkey = itr->second->getPKey();

			//seq[i].latestLogTime 
			(*seq)[i].latestLogTime = itr->second->getLastSavedTimeFromQueue();

			//seq[i].categoryType			 
			(*seq)[i].categoryType = CORBA::string_dup("");

			i++ ;
		}

		return seq;
	}

	void DataLogManager::starConfigureSettingMonitor()
	{
			m_terminated=false;
			start();
	}

	void DataLogManager::stopConfigureSettingMonitor()
	{
		terminateAndWait();		
	}

	void DataLogManager::setDataMaxTimeout( int dataMaxTimeout)
	{
		if(dataMaxTimeout<0)
		{
			m_dataMaxTimeout = DATA_MAX_TIMEOUT_DEFAULT;
		}
		else
		{
			m_dataMaxTimeout = dataMaxTimeout;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set parameter: DataMaxTimeout: %d", m_dataMaxTimeout);
		
	}

	int DataLogManager::getDataMaxTimeout()
	{
		return m_dataMaxTimeout;
	}
	void DataLogManager::setTimeoutDelay(int timeoutDelay)
	{
		if(timeoutDelay<0)
		{
			m_timeoutDelay = TIME_OUT_DELAY_DEFAULT;
		}
		else
		{
			m_timeoutDelay = timeoutDelay;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set parameter: TimeoutDelay: %d", m_timeoutDelay);
	}
	int DataLogManager::getTimeoutDelay()
	{
		return m_timeoutDelay;
	}
	void DataLogManager::setBatchUpdateSize(int batchUpdateSize)
	{
		if(batchUpdateSize<0)
		{
			m_batchUpdateSize = BATCH_UPDATE_SIZE_DEFAULT;
		}
		else
		{
			m_batchUpdateSize = batchUpdateSize;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set parameter: BatchUpdateSize: %d", m_batchUpdateSize);
		
	}

	int DataLogManager::getBatchUpdateSize()
	{
		return m_batchUpdateSize;
	}

	void DataLogManager::setMaxDelayTimes(int maxDelayTimes)
	{
		if(maxDelayTimes<0)
		{
			m_maxDelayTimes = MAX_DELAY_TIMES_DEFAULT;
		}
		else
		{
			m_maxDelayTimes = maxDelayTimes;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set parameter: MaxDelayTimes: %d", m_maxDelayTimes);
		
	}

	int DataLogManager::getMaxDelayTimes()
	{
		return m_maxDelayTimes;
	}

}
///////////////////////////////////////////////////////////
//  DataLogManager.h
//  Implementation of the Class DataLogManager
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:13
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_495E982A_A0FF_4ae8_B79E_198EC2ED2DF8__INCLUDED_)
#define EA_495E982A_A0FF_4ae8_B79E_198EC2ED2DF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/datalog/IDL/src/DataLogMessageCorbaDef.h"
#include "core/datalog/IDL/src/DataLogAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/datalog/src/DatalogConfigSettingsData.h"


using namespace TA_Base_Core;
namespace TA_Base_Bus
{
	class DataLogCategory; // Forward Declaration
	class DataLogAgentImpl;
	class DataLogManager : public TA_Base_Core::Thread
	{
	public:		
		static DataLogManager* getInstance();			
		static void removeInstance();
		virtual ~DataLogManager();

		
		/**
		*  setupDataLogAgent
		*  initilize CORBA Servant
		*/
		void setupDataLogAgent(std::string name);

		/**
		*  getAgentCorbaName
		*  get Corba name of agent from database
        */
		void getAgentCorbaName();

		/**
		*  init
		*  load configuration setting from database
		*  create DataLogCategories
		*  start to monitor data points		 
		*/
		bool init();

		bool addNewConfigSettings();

		void deleteConfigSettingByPkeys(std::vector<unsigned long> pkeys);
		void deleteConfigSettingByPkey( unsigned long pkey);


		/**
		* 
		*Start and Stop configure setting monitor
		* 
		*/
		void starConfigureSettingMonitor();		
		void stopConfigureSettingMonitor();

		/**
		* 
		*Implement interface of Thread
		* 
		*/
		 
		void run();
		void terminate();


		/*** 
		/*       
		*/
		void updateSyncInfo ();

		void synclatestDataLog();

		DataLogCategory * createDataLogCategoryByConfigSetting(DatalogConfigSettingsData* configSetting);

		void freeDataLogConfiSettings(std::vector<DatalogConfigSettingsData*> &configSettings);

		void freeCategoryMap();

		TA_Base_Core::DataLogSyncSequence* getAllDataLogSync();

		std::vector<unsigned long> getExistedPkeys();
		bool entityKeyExistInCategory(unsigned long entityKey);

		void collectOldEntityKeysFromCategory(unsigned long pkey, std::vector<unsigned long> & oldEntityKeiesMightBeDeleted);

		void deleteEntityFromScadaProxyWhenNecessary(std::vector<unsigned long> oldEntityKeiesMightBeDeleted);
		/**
		*  handleStateMesssage
		*
		*  handle state update message 
		*/

		void handleStateMesssage(TA_Base_Core::DataLogSyncSequence*  datalogSync);

		void setDataMaxTimeout( int dataMaxTimeout);
		int getDataMaxTimeout();
		void setTimeoutDelay(int timeoutDelay);
		int getTimeoutDelay();
		void setBatchUpdateSize(int batchUpdateSize);
		int getBatchUpdateSize();
		void setMaxDelayTimes(int maxDelayTimes);
		int getMaxDelayTimes();
		
	private:
		DataLogManager();
		static DataLogManager* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		bool m_terminated;

		typedef std::map<unsigned long, DataLogCategory*> DataLogCategoryMap;
		DataLogCategoryMap m_CategoryMap;
		TA_Base_Core::ReEntrantThreadLockable m_categorymapLock;


// 		typedef TA_Base_Core::NamedObject< TA_Base_Core::AlarmAgentCorbaDef, 
// 			TA_Base_Core::AlarmAgentCorbaDef_ptr, 
// 			TA_Base_Core::AlarmAgentCorbaDef_var > AlarmAgentNamedObject;


		// TD 14082 this is the Control AlarmAgent's address for callback
		typedef TA_Base_Core::NamedObject< TA_Base_Core::DataLogAgentCorbaDef, 
			TA_Base_Core::DataLogAgentCorbaDef_ptr, 
			TA_Base_Core::DataLogAgentCorbaDef_var > DataLogNamedObject;

		//TA_Base_Core::CorbaName m_datalogAgentCorbaName;
		DataLogNamedObject m_controlDataLogAgent;

		short m_agentTimeout; // (300)secs
		int m_dataMaxTimeout;
		int m_timeoutDelay;
		int m_batchUpdateSize;
		int m_moniter_sleep_time;     //(10000)ms 
		int m_maxDelayTimes;

		DataLogAgentImpl* m_DataLogAgentAgentImpl;

	};
}
#endif // !defined(EA_495E982A_A0FF_4ae8_B79E_198EC2ED2DF8__INCLUDED_)

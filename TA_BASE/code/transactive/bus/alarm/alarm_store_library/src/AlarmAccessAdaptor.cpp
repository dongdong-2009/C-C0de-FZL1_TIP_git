// AlarmAccessAdaptor.cpp: implementation of the AlarmAccessAdaptor class.
//
//////////////////////////////////////////////////////////////////////

/*
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_store_library/src/AllAlarmManager.h"
#include "bus/alarm/alarm_store_library/src/DutyAlarmManager.h"
#include "bus/alarm/alarm_store_library/src/AlarmAccessAdaptor.h"
*/
#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "AlarmAccessAdaptor.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{
	AlarmAccessAdaptor* AlarmAccessAdaptor::m_pInstance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable AlarmAccessAdaptor::m_singletonLock;
		
	// This is used in retrieving the data from map to vector
	// use in call to std::transform
	template<typename Pair>
	struct select2nd
	{
		typedef Pair argument_type ;
		typedef typename Pair::second_type result_type ;

		const result_type &
			operator()(const argument_type &p) const
		{
			return p.second ;
		}
	};

	AlarmAccessAdaptor& AlarmAccessAdaptor::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_pInstance == NULL)
		{
			m_pInstance = new AlarmAccessAdaptor();
		}
		
		FUNCTION_EXIT;
		return *m_pInstance;
	}

	void AlarmAccessAdaptor::freeInstance()
	{
		FUNCTION_ENTRY("freeInstance");
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_pInstance != NULL)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
		FUNCTION_EXIT;
	}

	AlarmAccessAdaptor::AlarmAccessAdaptor() : m_pAllAlarmManager(0), m_pDutyAlarmManager(0)
	{
		FUNCTION_ENTRY("Constructor");
		m_pAllAlarmManager = &AllAlarmManager::getInstance();		
		m_pDutyAlarmManager = &DutyAlarmManager::getInstance();

		initLocationEntityMap();
		FUNCTION_EXIT;
	}
	
	AlarmAccessAdaptor::~AlarmAccessAdaptor()
	{
		FUNCTION_ENTRY("Destructor");
		DutyAlarmManager::freeInstance();
		AllAlarmManager::freeInstance();
		FUNCTION_EXIT;
	}

	void AlarmAccessAdaptor::init(ULONG ulMaxAllAlarmCount, bool alwaysRecreateMapfile /*=false*/)
	{
		FUNCTION_ENTRY("init");
		m_pAllAlarmManager->setAllAlarmsMaxCount(ulMaxAllAlarmCount);
		m_pDutyAlarmManager->init(alwaysRecreateMapfile);
		FUNCTION_EXIT;
	}

	unsigned long AlarmAccessAdaptor::removeLocationAlarms(unsigned long locationKey)
	{
		FUNCTION_ENTRY("removeLocationAlarms");
		TA_Base_Core::ThreadGuard guard(m_lock);

		unsigned long lRes = 0;
		// remove from all alarms // local memory
		lRes = m_pAllAlarmManager->removeLocationAlarms(locationKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed location alarms: location key = %ld, alarms = %ld", locationKey, lRes);

		// remove from duty alarms // shared memory
		if (m_pDutyAlarmManager != NULL)
		{
			m_pDutyAlarmManager->removeLocationAlarms(locationKey);
		}
		
		FUNCTION_EXIT;
		return lRes;
	}

	unsigned long AlarmAccessAdaptor::removeDataSourceAlarms(unsigned long dataSource)
	{
		FUNCTION_ENTRY("removeLocationAlarms");
		TA_Base_Core::ThreadGuard guard(m_lock);

		unsigned long lRes = 0;
		// remove from all alarms // local memory
		lRes = m_pAllAlarmManager->removeDataSourceAlarms(dataSource);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed data source alarms: SubSystem key = %ld, alarms = %ld", dataSource, lRes);

		// remove from duty alarms // shared memory
		if (m_pDutyAlarmManager != NULL)
		{
			m_pDutyAlarmManager->removeATSAlarms();
		}

		FUNCTION_EXIT;
		return lRes;
	}
	
	void AlarmAccessAdaptor::refreshAlarmbyAgent(unsigned long locationKey,TA_Base_Core::AlarmSequence* alarmList, std::vector<bool>& isPassPreFilters)
	{
		FUNCTION_ENTRY("refreshAlarmbyAgent");		
		TA_Base_Core::ThreadGuard guard(m_lock);

		// validate arguments
		if (locationKey == -1 || NULL == alarmList) return;
		if (alarmList->length() != isPassPreFilters.size()) return;

		DutyTempList dutyAlarmlist;		
		AlarmDataItem* alarmData = NULL;

		//Remove all alarms belonged to the location;
		m_pDutyAlarmManager->removeLocationAlarms(locationKey);
        unsigned long ulRes = m_pAllAlarmManager->removeLocationAlarms(locationKey);
		if (ulRes > 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed locations alarms count = %ld", ulRes);
		}

		int nCounter = 0;
		for (unsigned int i=0; i < alarmList->length(); ++i)
		{						
			//transfer from CORBA object to AlarmItem;								
			alarmData = _newAlarmItem((*alarmList)[i]);

			if (alarmData != NULL)
			{
				//insert into all alarm manager
				bool bRes = m_pAllAlarmManager->insertAlarm(alarmData, &dutyAlarmlist);
				if (false == bRes)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert alarm into All Alarm container.");
					delete alarmData;
					alarmData = NULL;
					continue;
				}

				if (isPassPreFilters[i])
				{
					nCounter++;
					alarmData->isVisible = true;
					dutyAlarmlist.insert(DutyTempList::value_type((DWORD)alarmData, alarmData));
				}
				else
				{
					alarmData->isVisible = false;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isPassPreFilter: false");
				}
			}		

			if (m_pDutyAlarmManager != NULL && nCounter >= MAXBUILDCOUNT)
			{
				std::vector<AlarmDataItem*> alarmVecList;
				std::transform(dutyAlarmlist.begin()
								, dutyAlarmlist.end()
								, std::back_inserter(alarmVecList)
								, select2nd<DutyTempList::value_type>());

				m_pDutyAlarmManager->insertAlarmlist(alarmVecList);

				alarmVecList.clear();
				dutyAlarmlist.clear();
				nCounter = 0;
			}
		}

		if (m_pDutyAlarmManager != NULL && nCounter != 0)
		{
			std::vector<AlarmDataItem*> alarmVecList;
			std::transform(dutyAlarmlist.begin()
				, dutyAlarmlist.end()
				, std::back_inserter(alarmVecList)
				, select2nd<DutyTempList::value_type>());

			m_pDutyAlarmManager->insertAlarmlist(alarmVecList);

			alarmVecList.clear();
			dutyAlarmlist.clear();
			nCounter = 0;
		}

		// notify shared memory even if the vector list is zero so it can remove the alarms from the location key//
// 		if (m_pDutyAlarmManager != NULL)
// 		{
// 			m_pDutyAlarmManager->refreshAlarmbyAgent(locationKey, dutyAlarmlist);
// 		}
		
		FUNCTION_EXIT;
	}


	void AlarmAccessAdaptor::insertAlarm(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail, bool isPassPreFilter)
	{
		FUNCTION_ENTRY("insertAlarm");
		TA_Base_Core::ThreadGuard guard(m_lock);	 
		AlarmDataItem* alarmData=NULL;
		
		if (m_pAllAlarmManager->findAlarm(std::string(alarmDetail.alarmID.in())) == NULL)
		{
			alarmData = _newAlarmItem(alarmDetail);
			
			if (alarmData != NULL)
			{			
				bool bRes = m_pAllAlarmManager->insertAlarm(alarmData);
				
				if (bRes)
				{
					if (isPassPreFilter)
					{
						// Put in the shared memory
						alarmData->isVisible = true;
						m_pDutyAlarmManager->insertAlarm(alarmData);		 
					}
					else
					{
						alarmData->isVisible = false;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isPassPreFilter: false.");
					}
				}
				else
				{
					// we failed to insert into all alarm //
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert new Alarm to AllAlarmManager, deallocating new data created. ID = %s", alarmData->alarmID.c_str());
					delete alarmData;
					alarmData = NULL;
				}
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Alarm item already exist!!! Alarm ID = %s", alarmDetail.alarmID.in());
		}
		FUNCTION_EXIT;
	}

	void AlarmAccessAdaptor::updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		AlarmDataItem* alarmData=NULL;

		try 
		{
			std::string alarmID = alarmUpdate.alarmID.in();				
			
			alarmData = m_pAllAlarmManager->findAlarm(alarmID);

			if (alarmData != NULL)
			{				
				if ((alarmUpdate.alarmHasBeenAcknowledged  && (alarmData->state == TA_Base_Core::AlarmClosed))||
					(alarmUpdate.alarmHasBeenClosed && strlen(alarmData->alarmAcknowledgedBy.c_str())>0) ||
					((alarmUpdate.alarmHasBeenClosed) && (alarmUpdate.alarmHasBeenAcknowledged)))
				{
					if (alarmData->isVisible)
					{
						m_pDutyAlarmManager->deleteAlarm(alarmID.c_str());					
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Alarm Visibility FALSE");
					}

					m_pAllAlarmManager->deleteAlarm(alarmID);
				}
				else 
				{
					//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
					// Note: We need to call the allAlarmManager first to modify the data and forward the alarm //
					// to the dutyAlarmManager. Data Modification was move to the allAlarmManager to ensure the //
					// data is synchronize properly in allAlarmManager. See AllAlarmManager::updateAlarm for	//
					// detailed information on data synchronization.											//
					//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
					m_pAllAlarmManager->updateAlarm(alarmData, alarmUpdate);
					
					if (alarmData->isVisible)
					{
						// Update shared memory
						m_pDutyAlarmManager->updateAlarm(alarmData);
					}

				}		
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Update alarm failed: Unable to find alarm %s", alarmID.c_str());
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred when trying to update alarm.");
		}			
	}
	
	void AlarmAccessAdaptor::buildIndexWithDuty(TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager )
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		// Clean all data in shared memory
		// m_pDutyAlarmManager->deleteAllAlarmItems();
		m_pDutyAlarmManager->removeISCSAlarms();
		
		// Re-build all alarms
		m_pAllAlarmManager->buildIndexWithDuty(pAlarmStoreManager, m_pDutyAlarmManager);
	}

	void AlarmAccessAdaptor::UpdateDutyIndex(LocationSubSytemList& changes, TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		m_pAllAlarmManager->UpdateDutyIndex(changes, pAlarmStoreManager, m_pDutyAlarmManager);
	}
	
	void AlarmAccessAdaptor::clean()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		
		// Clean the local memory
		m_pAllAlarmManager->deleteAllAlarmItems();

		// Clean the shared memory
		m_pDutyAlarmManager->deleteAllAlarmItems();
	}
	

	AlarmDataItem* AlarmAccessAdaptor::_newAlarmItem(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail)
	{
		AlarmDataItem * pNewAlarm = NULL;
		if (alarmDetail.state == TA_Base_Core::AlarmSuppressed) return pNewAlarm;

		try 
		{
// 			std::string strSystemName = TA_Base_Bus::SubsystemDataCache::getInstance().getSystemNameFromSubsystem(alarmDetail.subsystemKey);
// 			std::string strSubSystemName = TA_Base_Bus::SubsystemDataCache::getInstance().getSubSystemName(alarmDetail.subsystemKey);
			const char * OperatorMaintenanceFlag = "OMFlag:";
			std::string strList(alarmDetail.alarmParameterList.in());
			char const* flagPosition = strstr(alarmDetail.alarmParameterList,OperatorMaintenanceFlag);
			char flagValue=' ';
			if (flagPosition!=NULL)
			{
				// Get the setting (OMFlag:M, or OMFlag:O, or OMFlag:B, or OMFlag:)
				flagValue = *(flagPosition+strlen(OperatorMaintenanceFlag));
				if(flagValue!='M'&&flagValue!='O'&&flagValue!='B')
				{
					flagValue=' ';
				}
			}

			pNewAlarm = new AlarmDataItem(
				alarmDetail.alarmID, 
				TA_Base_Core::ItemMillisecondTime(alarmDetail.displayTime.time, alarmDetail.displayTime.time),
				alarmDetail.alarmSeverity, 
				alarmDetail.assetName, 
				alarmDetail.alarmDescription, 
				alarmDetail.alarmValue, 
				alarmDetail.mmsState, 
				alarmDetail.state,
				alarmDetail.decisionSupportState,
				alarmDetail.alarmAcknowledgedBy,
				alarmDetail.alarmComments,
				alarmDetail.isHeadOfAvalanche,
				alarmDetail.isChildOfAvalanche,
				alarmDetail.subsystemKey,
				alarmDetail.messageTypeKey,
				alarmDetail.locationKey,
				alarmDetail.assocEntityTypeKey,
				alarmDetail.assocEntityKey,
				alarmDetail.entityName,
				&flagValue,
				alarmDetail.avalancheHeadID,
				"",
				"",//strSubSystemName.c_str(),
				"",//strSystemName.c_str(),
				ISCS_SUBSYSTEM_ID
				);
			pNewAlarm->isExternalAlarm = false;
			pNewAlarm->isVisible = false;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to allocate data.");
		}

		return pNewAlarm;
	}

	AlarmDataItem* AlarmAccessAdaptor::_newAlarmItem(I_ALARM_PROVIDER::AlarmDetail* alarmDetail)
	{
		AlarmDataItem * pNewAlarm = NULL;

		try 
		{
			std::string strTemp="";
			std::string ackby;
			ackby=alarmDetail->alarmAcknowledgeBy;
			char flagValue=alarmDetail->omAlarm;
			// Get the setting (OMFlag:M, or OMFlag:O, or OMFlag:B, or OMFlag:)
			if(flagValue!='M'&&flagValue!='O'&&flagValue!='B')
			{
				flagValue=' ';
			}

			// convert I_ATS_ISCS_ALARM::AlarmState to TA_Base_Core::AlarmState
			TA_Base_Core::AlarmState alarmState;
			switch (alarmDetail->state)
			{
			case I_ALARM_PROVIDER::AlarmOpen:
			case I_ALARM_PROVIDER::AlarmAcknowledged:
				alarmState = TA_Base_Core::AlarmOpen;
				break;
			case I_ALARM_PROVIDER::AlarmReturned:
				alarmState = TA_Base_Core::AlarmClosed;
				break;
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid AlarmState: state is either close or unknown. State = %ld", alarmDetail->state);
				throw;
				break;
			}


			//Set Entity
			unsigned long ulEntityKey = 0;
			unsigned long ulEntityTypeKey = 0;
			unsigned long ulSubsystemKey = 0;
			LocationEntityMapT::iterator itEntity = m_locEntityMap.find(alarmDetail->locationKey);
			if (itEntity != m_locEntityMap.end())
			{
				TA_Base_Core::IEntityData* pEntity = itEntity->second;
				ulEntityKey = pEntity->getKey();
				ulEntityTypeKey = pEntity->getTypeKey();
				ulSubsystemKey = pEntity->getSubsystem();
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable find assoc Entity key for location name: %s with AlarmID: %s", alarmDetail->locationId.c_str(), alarmDetail->alarmID.c_str());
			}

			//Message Type key
			unsigned long ulMessageTypekey = atol(alarmDetail->strAlarmType.c_str());
			
			pNewAlarm = new AlarmDataItem(
				alarmDetail->alarmID.c_str(),
				TA_Base_Core::ItemMillisecondTime(alarmDetail->sourceTime.time,alarmDetail->sourceTime.millitm),
				alarmDetail->alarmSeverity, 
				alarmDetail->assetName.c_str(), 
				alarmDetail->alarmDescription.c_str(), 
				alarmDetail->alarmValue.c_str(), 
				(TA_Base_Core::MmsStateType)alarmDetail->mmsState, 
				alarmState,
				TA_Base_Core::NoRelatedPlan,
				ackby.c_str(),
				strTemp.c_str(),
				alarmDetail->isHeadOfAvalanche,
				alarmDetail->isChildOfAvalanche,
				ulSubsystemKey,  //subsystemKey
				ulMessageTypekey,  //messageTypeKey
				alarmDetail->locationKey,  //DBG // location
				ulEntityTypeKey,  //assocEntityTypeKey
				ulEntityKey,  //assocEntityKey
				strTemp.c_str(),
				&flagValue,
				alarmDetail->avalancheHeadID.c_str(),
				alarmDetail->strAlarmType.c_str(),
				alarmDetail->subsytemType.c_str(),
				alarmDetail->systemkeyType.c_str(),
				alarmDetail->alarmSource
				);
			pNewAlarm->isExternalAlarm=true;
			pNewAlarm->isVisible = false;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to allocate data.");
		}

		return pNewAlarm;
	}

	void AlarmAccessAdaptor::notifyClientRemap()
	{
		m_pDutyAlarmManager->notifyClientRemap();
	}
	
	bool AlarmAccessAdaptor::RemoveAlarm(std::string strAlarmID)
	{
		return m_pDutyAlarmManager->deleteAlarm(strAlarmID);
	}
		
	void AlarmAccessAdaptor::buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault)
	{
		m_pDutyAlarmManager->buildIndexWithFilter(pAlarmFilterSet, vfp, cs_pos, isDefault);
	}
	
	bool AlarmAccessAdaptor::aliveCheck()
	{
		return m_pDutyAlarmManager->aliveCheck();
	}
	
	bool AlarmAccessAdaptor::isUpdating()
	{
		return m_pDutyAlarmManager->isUpdating();
	}
	
	int AlarmAccessAdaptor::healthMonitor()
	{
		return m_pDutyAlarmManager->healthMonitor();
	}

	unsigned long AlarmAccessAdaptor::removeISCSDutyAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_pDutyAlarmManager->removeISCSAlarms();
	}

	TA_Base_Core::AlarmDetailCorbaDef* AlarmAccessAdaptor::getAlarm(std::string alarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);		
		TA_Base_Core::AlarmDetailCorbaDef* pAlarmCorbadef = NULL;
		std::auto_ptr<AlarmDataItem> pFindItem(0);

		pFindItem.reset(m_pDutyAlarmManager->getAlarm(alarmID));

		if (NULL != pFindItem.get())
		{
			// Convert alarmDataItem to corbadef
			pAlarmCorbadef = new TA_Base_Core::AlarmDetailCorbaDef;

			pAlarmCorbadef->alarmID = CORBA::string_dup(pFindItem->alarmID.c_str());
			pAlarmCorbadef->sourceTime.time = pFindItem->transactiveTime.time;
			pAlarmCorbadef->sourceTime.millitm = pFindItem->transactiveTime.millitm;
			pAlarmCorbadef->alarmSeverity = pFindItem->alarmSeverity;
			pAlarmCorbadef->assetName = CORBA::string_dup(pFindItem->assetName.c_str());
			pAlarmCorbadef->alarmDescription =CORBA::string_dup(pFindItem->alarmDescription.c_str());
			pAlarmCorbadef->alarmValue = CORBA::string_dup(pFindItem->alarmValue.c_str()), 
			pAlarmCorbadef->mmsState = TA_Base_Core::Utilities::getInstance().convertString2MMS(pFindItem->mmsState); 
			pAlarmCorbadef->state = TA_Base_Core::AlarmState(pFindItem->state);
			pAlarmCorbadef->decisionSupportState = TA_Base_Core::Utilities::getInstance().convertString2DDS(pFindItem->decisionSupportState);
			pAlarmCorbadef->alarmAcknowledgedBy = CORBA::string_dup(pFindItem->alarmAcknowledgedBy.c_str());
			pAlarmCorbadef->alarmComments = CORBA::string_dup(pFindItem->alarmComments.c_str());
			
			bool bIsAVLHead = false;
			bool bIsAVLChild = false;
			TA_Base_Core::Utilities::getInstance().convertString2Avalanche(pFindItem->avalanche, bIsAVLHead, bIsAVLChild);

			pAlarmCorbadef->isHeadOfAvalanche = bIsAVLHead;
			pAlarmCorbadef->isChildOfAvalanche = bIsAVLChild;
			pAlarmCorbadef->subsystemKey = pFindItem->subsystemKey;
			pAlarmCorbadef->messageTypeKey = pFindItem->messageTypeKey;
			pAlarmCorbadef->locationKey = pFindItem->locationKey;
			pAlarmCorbadef->assocEntityTypeKey = pFindItem->assocEntityTypeKey;
			pAlarmCorbadef->assocEntityKey = pFindItem->assocEntityKey;
			pAlarmCorbadef->entityName = CORBA::string_dup(pFindItem->entityName.c_str());
			pAlarmCorbadef->avalancheHeadID = CORBA::string_dup(pFindItem->avalancheHeadID.c_str());
				
		}

		return pAlarmCorbadef;
	}

	void AlarmAccessAdaptor::setLatestFASAlarm(unsigned long entityKey)
	{
		m_pDutyAlarmManager->setLatestFASAlarm(entityKey);
	}

	unsigned long AlarmAccessAdaptor::getLatestFASAlarm()
	{
		return m_pDutyAlarmManager->getLatestFASAlarm();
	}

	void AlarmAccessAdaptor::insertOrUpdateAlarm( std::vector<I_ALARM_PROVIDER::AlarmDetail* >& extAlarms, TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager)
	{
		for (int nIdx=0; nIdx<extAlarms.size(); nIdx++)
		{
			AlarmDataItem* pNewAlarm = NULL;
			I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = extAlarms[nIdx];
			if (NULL == pAlarmDetail)
			{ // Should not happen
				continue;
			}

			AlarmDataItem* pFind = m_pAllAlarmManager->findAlarm(pAlarmDetail->alarmID.c_str());
			if (pFind != NULL)
			{
				if (pFind->isVisible)
				{
					m_pDutyAlarmManager->deleteAlarm(pAlarmDetail->alarmID);
				}

				m_pAllAlarmManager->deleteAlarm(pAlarmDetail->alarmID);
			}	

			// Alarm is closed
			if (pAlarmDetail->state == I_ALARM_PROVIDER::AlarmClosed)
				continue;

			pNewAlarm = _newAlarmItem(pAlarmDetail);
			bool bRes = m_pAllAlarmManager->insertAlarm(pNewAlarm);

			if (bRes)
			{
				if (pAlarmStoreManager->isAlarmVisible(pNewAlarm))
				{
					// Put in the shared memory
					pNewAlarm->isVisible = true;
					m_pDutyAlarmManager->insertAlarm(pNewAlarm);		 
				}
				else
				{
					pNewAlarm->isVisible = false;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isPassPreFilter: false.");
				}
			}
			else
			{
				// we failed to insert into all alarm //
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert new Alarm to AllAlarmManager, deallocating new data created. ID = %s", pAlarmDetail->alarmID.c_str());
				delete pNewAlarm;
				pNewAlarm = NULL;
			}
			
		}

	}

	void AlarmAccessAdaptor::initLocationEntityMap()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "initLocationEntityMap coming in");
		std::vector<TA_Base_Core::IEntityData*> allVAEntity;
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retrieving Video Analyzer Entities.");

			allVAEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("Video Analyzer");
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving operator locations. There were problems communicating with the database");
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving operator locations. The data could not be converted to the required format");
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving Operator locations.");
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving location information."));
		}

		// store the locations into the map and delete the temporary structure
		m_locEntityMap.clear();
		for (unsigned int i = 0;i < allVAEntity.size(); ++i)
		{
			if(allVAEntity[i]->getKey() != 0)
			{
				m_locEntityMap.insert(LocationEntityMapT::value_type(allVAEntity[i]->getLocation(), allVAEntity[i]));
			}
		}		

		allVAEntity.clear();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "initLocationEntityMap coming out");
	}
}

// AlarmItem.h: interface for the AlarmItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMITEM_H__78FDE137_5ABE_423C_A87D_43CADA58C6D5__INCLUDED_)
#define AFX_ALARMITEM_H__78FDE137_5ABE_423C_A87D_43CADA58C6D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/alarm/src/Utilities.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
//#include "core/alarm/src/Shmem.h"

#define CLIENTS_NUM 128
#define CHARBITS 8
#define ISCS_SUBSYSTEM_ID	0
#define ATS_SUBSYSTEM_ID	10000
#define VA_SUBSYSTEM_ID		10001

namespace TA_Base_Core
{
	// difine the size of alarm columns
	const int AVALANCHE_SIZE				= 2;
	const int DECISION_SUPPORT_STATE_STATE	= 2;
	const int MMS_STATE_SIZE				= 2;
	const int ALARM_OPERATOR_MODE_SIZE		= 2;
	const int ENTITY_NAME_SIZE				= 64;
	const int ASSET_NAME_SIZE				= 128;
	const int AVALANCHE_HEAD_ID_SIZE		= 64;
	const int ALARM_ACK_BY_SIZE				= 64;
	const int ALARM_VALUE_SIZE				= 128;
	const int ALARM_ID_SIZE					= 64;  //from 28 bytes to 64 bytes, luohr
	const int ALARM_TYPE_SIZE				= 100;
	const int ALARM_SUBSYSTEM_SIZE			= 100;
	const int ALARM_SYSTEM_SIZE				= 100;
	const int ALARM_COMMENTS_SIZE			= 800;
	const int ALARM_DESC_SIZE				= 800; 

	typedef unsigned long Ul;
	struct ItemMillisecondTime 
	{
		unsigned long time;
		unsigned short millitm;
		ItemMillisecondTime(unsigned long _time, unsigned short _millitm):time(_time),millitm(_millitm)
		{
		}
		ItemMillisecondTime(const ItemMillisecondTime& rhs):time(rhs.time),millitm(rhs.millitm)
		{
		}
		/*************************************************************************/
		/* Please note that these overload comparison operators are being use in */
		/* sorting method, for the STL containers, if changes are needed please  */
		/* consider the sort order.												 */
		/*************************************************************************/
		bool operator < (const ItemMillisecondTime& rhs) const
		{
			if(time == rhs.time)
			{
				return millitm<rhs.millitm;
			}
			return time < rhs.time;
		}
		// liuyang++
		bool operator > (const ItemMillisecondTime& rhs) const
		{
			if(time == rhs.time)
			{
				return millitm>rhs.millitm;
			}
			return time > rhs.time;
		}
		// ++liuyang
		
		// ++Noel
		bool operator == (const ItemMillisecondTime& rhs) const
		{
			if (time == rhs.time)
			{
				return millitm == rhs.millitm;
			}
			
			return false;
		}
	};
	
	class AlarmItem  
	{
	public:
		//++ added to accept data from alarmDataItem
		inline AlarmItem(const char* _alarmID,									
			const ItemMillisecondTime& _transactiveTime,
			unsigned long _alarmSeverity,
			const char* _assetName,
			const char* _alarmDescription,
			const char* _alarmValue,
			const char* _mmsState,
			unsigned short _state,
			const char* _decisionSupportState,
			const char* _alarmAcknowledgedBy,
			const char* _alarmComments,
			const char* _avalanche,
			unsigned long _subsystemKey,
			unsigned long _messageTypeKey,
			unsigned long _locationKey,
			unsigned long _assocEntityTypeKey,
			unsigned long _assocEntityKey,
			const char* _entityName,
			const char* _alarmOperatorMode,
			const char* _avalancheHeadID,
			const char* _strAlarmType,
			const char* _subsytemType,
			const char* _systemType,
			bool _bIsExternalAlarm,
			unsigned long _dataSource
			);

		inline AlarmItem(const char* _alarmID,
			const ItemMillisecondTime& _transactiveTime,
			unsigned long _alarmSeverity,
			const char* _assetName,
			const char* _alarmDescription,
			const char* _alarmValue,
			TA_Base_Core::MmsStateType _mmsState,
			unsigned short _state,
			TA_Base_Core::DecisionSupportStateType _decisionSupportState,
			const char* _alarmAcknowledgedBy,
			const char* _alarmComments,
			bool _isHeadOfAvalanche,
			bool _isChildOfAvalanche,
			unsigned long _subsystemKey,
			unsigned long _messageTypeKey,
			unsigned long _locationKey,
			unsigned long _assocEntityTypeKey,
			unsigned long _assocEntityKey,
			const char* _entityName,
			const char* _avalancheHeadID,
			const char* _alarmOperatorMode,
			const char* _strAlarmType,
			const char* _subsytemType,
			const char* _systemType,
			unsigned long _dataSource
			);

		inline AlarmItem(const AlarmItem &alarmItem);
		
		inline ~AlarmItem();
		
		inline void select(int client)
		{
			selbits[client / CHARBITS] |= (1 << client % CHARBITS);
		}

		inline void unSelect(int client)
		{
			selbits[client / CHARBITS] &= ~(1 << client % CHARBITS);
		}

		inline bool isSelect(int client)
		{
			return 	((selbits[client / CHARBITS] & (1 << client % CHARBITS)) != 0);
		}

		ItemMillisecondTime transactiveTime;
		unsigned short state;
		unsigned long alarmSeverity;
		unsigned long subsystemKey;
		unsigned long assocEntityKey;
		unsigned long assocEntityTypeKey;
		unsigned long messageTypeKey;
		unsigned long locationKey;
		unsigned long dataSource;
		bool isExternalAlarm;

		char* entityName;
		char* avalanche;
		char* assetName;
		char* alarmID;
		char* avalancheHeadID;
		char* alarmDescription;
		char* alarmAcknowledgedBy;
		char* alarmComments;
		char* decisionSupportState;
		char* alarmOperatorMode;
		char* mmsState;
		char* alarmValue;
		char* strAlarmType;
		char* subsytemType;
		char* systemType;

	private:
		inline size_t assign(char buf[], const char* value, size_t bufSize);

		char m_entityName[ENTITY_NAME_SIZE];
		char m_avalanche[AVALANCHE_SIZE];
		char m_assetName[ASSET_NAME_SIZE];
		char m_alarmID[ALARM_ID_SIZE];
		char m_avalancheHeadID[AVALANCHE_HEAD_ID_SIZE];
		char m_alarmDescription[ALARM_DESC_SIZE];
		char m_alarmAcknowledgedBy[ALARM_ACK_BY_SIZE];
		char m_alarmComments[ALARM_COMMENTS_SIZE];
		char m_decisionSupportState[DECISION_SUPPORT_STATE_STATE];
		char m_mmsState[MMS_STATE_SIZE];
		char m_alarmOperatorMode[ALARM_OPERATOR_MODE_SIZE];
		char m_alarmValue[ALARM_VALUE_SIZE];
		char m_strAlarmType[ALARM_TYPE_SIZE];
		char m_subsytemType[ALARM_SUBSYSTEM_SIZE];
		char m_systemType[ALARM_SYSTEM_SIZE];

	private:
 		Ul selbits[(CLIENTS_NUM + sizeof(Ul) -1) / sizeof(Ul)];  //selection flag
 		//AlarmItem(const AlarmItem& rhs);
		AlarmItem();
	};

	AlarmItem::AlarmItem(const AlarmItem &alarmItem
		):transactiveTime(alarmItem.transactiveTime),alarmSeverity(alarmItem.alarmSeverity),state(alarmItem.state),
		subsystemKey(alarmItem.subsystemKey),messageTypeKey(alarmItem.messageTypeKey),locationKey(alarmItem.locationKey),
		assocEntityTypeKey(alarmItem.assocEntityTypeKey),assocEntityKey(alarmItem.assocEntityKey), dataSource(alarmItem.dataSource)
		, entityName(m_entityName)
		, avalanche(m_avalanche)
		, assetName(m_assetName)
		, alarmID(m_alarmID)
		, avalancheHeadID(m_avalancheHeadID)
		, alarmDescription(m_alarmDescription)
		, alarmAcknowledgedBy(m_alarmAcknowledgedBy)
		, alarmComments(m_alarmComments)
		, decisionSupportState(m_decisionSupportState)
		, alarmOperatorMode(m_alarmOperatorMode)
		, mmsState(m_mmsState)
		, alarmValue(m_alarmValue)
		, isExternalAlarm(alarmItem.isExternalAlarm)
		, strAlarmType(m_strAlarmType)
		, subsytemType(m_subsytemType)
		, systemType(m_systemType)
	{
	
		memset(selbits, 0, sizeof(selbits));
		
		assign(assetName, alarmItem.assetName, ASSET_NAME_SIZE); 	
		assign(alarmID, alarmItem.alarmID, ALARM_ID_SIZE);
		assign(alarmDescription, alarmItem.alarmDescription, ALARM_DESC_SIZE);
		assign(alarmAcknowledgedBy, alarmItem.alarmAcknowledgedBy, ALARM_ACK_BY_SIZE);
		assign(alarmComments, alarmItem.alarmComments, ALARM_COMMENTS_SIZE);
		assign(alarmValue, alarmItem.alarmValue, ALARM_VALUE_SIZE);
		assign(entityName, alarmItem.entityName, ENTITY_NAME_SIZE);
		assign(alarmOperatorMode,alarmItem.alarmOperatorMode,ALARM_OPERATOR_MODE_SIZE);
		assign(avalancheHeadID, alarmItem.avalancheHeadID, AVALANCHE_HEAD_ID_SIZE);
		assign(strAlarmType,alarmItem.strAlarmType,ALARM_TYPE_SIZE);
		assign(subsytemType,alarmItem.subsytemType,ALARM_SUBSYSTEM_SIZE);
		assign(systemType,alarmItem.systemType,ALARM_SYSTEM_SIZE);
		assign(avalanche, alarmItem.avalanche, 2);
		assign(mmsState, alarmItem.mmsState, 2);
		assign(decisionSupportState, alarmItem.decisionSupportState, 2);
	}

	AlarmItem::AlarmItem(const char* _alarmID,
		const ItemMillisecondTime& _transactiveTime,
		unsigned long _alarmSeverity,
		const char* _assetName,
		const char* _alarmDescription,
		const char* _alarmValue,
		TA_Base_Core::MmsStateType _mmsState,
		unsigned short _state,
		TA_Base_Core::DecisionSupportStateType _decisionSupportState,
		const char* _alarmAcknowledgedBy,
		const char* _alarmComments,
		bool _isHeadOfAvalanche,
		bool _isChildOfAvalanche,
		unsigned long _subsystemKey,
		unsigned long _messageTypeKey,
		unsigned long _locationKey,
		unsigned long _assocEntityTypeKey,
		unsigned long _assocEntityKey,
		const char* _entityName,
		const char* _avalancheHeadID,
		const char* _alarmOperatorMode,
		const char* _strAlarmType,
		const char* _subsytemType,
		const char* _systemType,
		unsigned long _dataSource
		):transactiveTime(_transactiveTime),alarmSeverity(_alarmSeverity),state(_state),
		subsystemKey(_subsystemKey),messageTypeKey(_messageTypeKey),locationKey(_locationKey),
		assocEntityTypeKey(_assocEntityTypeKey),assocEntityKey(_assocEntityKey), dataSource(_dataSource)
		, entityName(m_entityName)
		, avalanche(m_avalanche)
		, assetName(m_assetName)
		, alarmID(m_alarmID)
		, avalancheHeadID(m_avalancheHeadID)
		, alarmDescription(m_alarmDescription)
		, alarmAcknowledgedBy(m_alarmAcknowledgedBy)
		, alarmComments(m_alarmComments)
		, decisionSupportState(m_decisionSupportState)
		, alarmOperatorMode(m_alarmOperatorMode)
		, mmsState(m_mmsState)
		, alarmValue(m_alarmValue)
		, isExternalAlarm(false)
		, strAlarmType(m_strAlarmType)
		, subsytemType(m_subsytemType)
		, systemType(m_systemType)
	{
 


		memset(selbits, 0, sizeof(selbits));

		assign(assetName, _assetName, ASSET_NAME_SIZE);		
		assign(alarmID, _alarmID, ALARM_ID_SIZE);
		assign(alarmDescription, _alarmDescription, ALARM_DESC_SIZE);
		assign(alarmAcknowledgedBy, _alarmAcknowledgedBy, ALARM_ACK_BY_SIZE);
		assign(alarmComments, _alarmComments, ALARM_COMMENTS_SIZE);
		assign(alarmValue, _alarmValue, ALARM_VALUE_SIZE);
		assign(entityName, _entityName, ENTITY_NAME_SIZE);
		assign(alarmOperatorMode,_alarmOperatorMode,ALARM_OPERATOR_MODE_SIZE);
		assign(avalancheHeadID, _avalancheHeadID, AVALANCHE_HEAD_ID_SIZE);
		assign(strAlarmType,_strAlarmType,ALARM_TYPE_SIZE);
		assign(subsytemType,_subsytemType,ALARM_SUBSYSTEM_SIZE);
		assign(systemType,_systemType,ALARM_SYSTEM_SIZE);

		std::string strAvalanche = "";
		strAvalanche = Utilities::getInstance().convertAvalanche2String(_isHeadOfAvalanche, _isChildOfAvalanche);
		assign(avalanche, strAvalanche.c_str(), 2);
		
		std::string strMMSState = "";
		strMMSState = Utilities::getInstance().convertMMS2String(_mmsState);
		assign(mmsState, strMMSState.c_str(), 2);
		
		std::string strDDSType = "";
		strDDSType = Utilities::getInstance().convertDSS2String(_decisionSupportState);
		assign(decisionSupportState, strDDSType.c_str(), 2);
 	}


	size_t AlarmItem::assign(char buf[], const char* value, size_t bufSize)
	{
		size_t cpLen = strlen(value) >= bufSize ? bufSize - 1 : strlen(value);
		strncpy(buf, value, cpLen);
		buf[cpLen] = 0;
		
		return cpLen;
	}

	AlarmItem::~AlarmItem()
	{
	}

	//++ added to accept data from alarmDataItem
	AlarmItem::AlarmItem(const char* _alarmID,									
		const ItemMillisecondTime& _transactiveTime,
		unsigned long _alarmSeverity,
		const char* _assetName,
		const char* _alarmDescription,
		const char* _alarmValue,
		const char* _mmsState,
		unsigned short _state,
		const char* _decisionSupportState,
		const char* _alarmAcknowledgedBy,
		const char* _alarmComments,
		const char* _avalanche,
		unsigned long _subsystemKey,
		unsigned long _messageTypeKey,
		unsigned long _locationKey,
		unsigned long _assocEntityTypeKey,
		unsigned long _assocEntityKey,
		const char* _entityName,
		const char* _alarmOperatorMode,
		const char* _avalancheHeadID,
		const char* _strAlarmType,
		const char* _subsytemType,
		const char* _systemType,
		bool _bIsExternalAlarm,
		unsigned long _dataSource
		):transactiveTime(_transactiveTime),alarmSeverity(_alarmSeverity),state(_state),
		subsystemKey(_subsystemKey),messageTypeKey(_messageTypeKey),locationKey(_locationKey),
		assocEntityTypeKey(_assocEntityTypeKey),assocEntityKey(_assocEntityKey), dataSource(_dataSource)
		, entityName(m_entityName)
		, avalanche(m_avalanche)
		, assetName(m_assetName)
		, alarmID(m_alarmID)
		, avalancheHeadID(m_avalancheHeadID)
		, alarmDescription(m_alarmDescription)
		, alarmAcknowledgedBy(m_alarmAcknowledgedBy)
		, alarmComments(m_alarmComments)
		, decisionSupportState(m_decisionSupportState)
		, mmsState(m_mmsState)
		, alarmValue(m_alarmValue)
		, alarmOperatorMode(m_alarmOperatorMode)
		, isExternalAlarm(_bIsExternalAlarm)
		, strAlarmType(m_strAlarmType)
		, subsytemType(m_subsytemType)
		, systemType(m_systemType)
	{
		assign(assetName, _assetName, ASSET_NAME_SIZE);		
		assign(alarmID, _alarmID, ALARM_ID_SIZE);
		assign(alarmDescription, _alarmDescription, ALARM_DESC_SIZE);
		assign(alarmAcknowledgedBy, _alarmAcknowledgedBy, ALARM_ACK_BY_SIZE);
		assign(alarmComments, _alarmComments, ALARM_COMMENTS_SIZE);
		assign(alarmValue, _alarmValue, ALARM_VALUE_SIZE);
		assign(entityName, _entityName, ENTITY_NAME_SIZE);
		assign(avalancheHeadID, _avalancheHeadID, AVALANCHE_HEAD_ID_SIZE);
		assign(avalanche, _avalanche, AVALANCHE_SIZE);
		assign(mmsState, _mmsState, MMS_STATE_SIZE);
		assign(decisionSupportState, _decisionSupportState, DECISION_SUPPORT_STATE_STATE);
		assign(alarmOperatorMode,_alarmOperatorMode,ALARM_OPERATOR_MODE_SIZE);
		assign(strAlarmType,_strAlarmType,ALARM_TYPE_SIZE);
		assign(subsytemType,_subsytemType,ALARM_SUBSYSTEM_SIZE);
		assign(systemType,_systemType,ALARM_SYSTEM_SIZE);
	}
}
#endif // !defined(AFX_ALARMITEM_H__78FDE137_5ABE_423C_A87D_43CADA58C6D5__INCLUDED_)

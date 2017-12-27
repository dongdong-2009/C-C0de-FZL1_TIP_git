#if !defined(__AFX_ALARMDATA_INCLUDED__)
#define __AFX_ALARMDATA_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <time.h>
#include <vector>
#include "core/alarm/src/AlarmItem.h"

namespace TA_Base_Bus
{
	class AlarmDataItem
	{
	public:
		AlarmDataItem(): transactiveTime(0, 0) {};
		inline AlarmDataItem(const char* _alarmID,
			const TA_Base_Core::ItemMillisecondTime& _transactiveTime,
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
			const char* _alarmOperatorMode,
			const char* _avalancheHeadID,
			const char* _strAlarmType,
			const char* _subSystemType,
			const char* _systemType,
			unsigned long _dataSource
			);

		inline AlarmDataItem(const AlarmDataItem& rhs) : 
			transactiveTime(rhs.transactiveTime)
			, state(rhs.state)
			, alarmSeverity(rhs.alarmSeverity)
			, subsystemKey(rhs.subsystemKey)
			, assocEntityKey(rhs.assocEntityKey)
			, assocEntityTypeKey(rhs.assocEntityTypeKey)
			, messageTypeKey(rhs.messageTypeKey)
			, locationKey(rhs.locationKey)
			, entityName(rhs.entityName.c_str())
			, avalanche(rhs.avalanche.c_str())
			, assetName(rhs.assetName.c_str())
			, alarmID(rhs.alarmID.c_str())
			, avalancheHeadID(rhs.avalancheHeadID.c_str())
			, alarmDescription(rhs.alarmDescription.c_str())
			, alarmAcknowledgedBy(rhs.alarmAcknowledgedBy.c_str())
			, alarmComments(rhs.alarmComments.c_str())
			, decisionSupportState(rhs.decisionSupportState.c_str())
			, mmsState(rhs.mmsState.c_str())
			, alarmValue(rhs.alarmValue.c_str())
			, isVisible(rhs.isVisible)
			, strAlarmType(rhs.strAlarmType)
			, subSystemType(rhs.subSystemType)
			, systemType(rhs.systemType)
			, isExternalAlarm(rhs.isExternalAlarm)
			, alarmOperatorMode(rhs.alarmOperatorMode)
			, dataSource(rhs.dataSource)
//			, version(rhs.version)
			
		{			
		};

		inline ~AlarmDataItem();

		TA_Base_Core::ItemMillisecondTime transactiveTime;
		unsigned short state;
		unsigned long alarmSeverity;
		unsigned long subsystemKey;
		unsigned long assocEntityKey;
		unsigned long assocEntityTypeKey;
		unsigned long messageTypeKey;
		unsigned long locationKey;
		unsigned long dataSource;
//		TA_Base_Core::Item_Version  version;
		bool isVisible;
		bool isExternalAlarm;
		
		std::string entityName;
		std::string avalanche;
		std::string assetName;
		std::string alarmID;
		std::string avalancheHeadID;
		std::string alarmDescription;
		std::string alarmAcknowledgedBy;
		std::string alarmComments;
		std::string decisionSupportState;
		std::string mmsState;
		std::string alarmValue;			
		std::string strAlarmType;
		std::string subSystemType;
		std::string systemType;
		std::string alarmOperatorMode;
	private:
		//AlarmItem(const AlarmItem& rhs);
		inline size_t assign(std::string, const char* value, size_t bufSize);
	};

	typedef std::vector<TA_Base_Bus::AlarmDataItem*> AlarmVector;

	AlarmDataItem::AlarmDataItem(const char* _alarmID,
		const TA_Base_Core::ItemMillisecondTime& _transactiveTime,
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
		const char* _alarmOperatorMode,
		const char* _avalancheHeadID,
		const char* _strAlarmType,
		const char* _subSystemType,
		const char* _systemType,
		unsigned long _dataSource
		):
		  transactiveTime(_transactiveTime)
		, alarmSeverity(_alarmSeverity)
		, state(_state)
		, subsystemKey(_subsystemKey)
		, messageTypeKey(_messageTypeKey)
		, locationKey(_locationKey)
		, assocEntityTypeKey(_assocEntityTypeKey)
		, assocEntityKey(_assocEntityKey)
		, entityName(_entityName)
		, assetName(_assetName)
		, alarmID(_alarmID)
		, avalancheHeadID(_avalancheHeadID)
		, alarmDescription(_alarmDescription)
		, alarmAcknowledgedBy(_alarmAcknowledgedBy)
		, alarmComments(_alarmComments)
		, decisionSupportState("")
		, avalanche("")
		, mmsState("")
		, alarmValue(_alarmValue)
		, strAlarmType(_strAlarmType)
		, subSystemType(_subSystemType)
		, systemType(_systemType)
		, alarmOperatorMode(_alarmOperatorMode)
		, isExternalAlarm(false)
		, isVisible(false)
		, dataSource(_dataSource)
	{
		avalanche = TA_Base_Core::Utilities::getInstance().convertAvalanche2String(_isHeadOfAvalanche, _isChildOfAvalanche);
		mmsState =  TA_Base_Core::Utilities::getInstance().convertMMS2String(_mmsState);
		decisionSupportState = TA_Base_Core::Utilities::getInstance().convertDSS2String(_decisionSupportState);	
 	}

	AlarmDataItem::~AlarmDataItem()
	{
	}
}

#endif //__AFX_ALARMDATA_INCLUDED__

// SimAlarmAgent.h: interface for the SimAlarmAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMALARMAGENTIMPL_H__323A0548_0144_4ECC_93C9_5538DE239156__INCLUDED_)
#define AFX_SIMALARMAGENTIMPL_H__323A0548_0144_4ECC_93C9_5538DE239156__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmDetail.h"
#include "core/alarm/src/AlarmSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
// #include "AlarmStoreUser.h"
#include "bus/alarm/alarm_list_control/src/AlarmStore.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"

#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"




const int Item_ackAlarm			=	1;
const int Item_closeAlarm		=	2;
const int Item_updateComments	=	3;
const int Item_updateMmsState	=	4;
const int Item_updateAlarmValue	=	5;
const int Item_updateAlarmPlan	=	6;

const int fileNumberColumn		=	12;


class AlarmStoreAction
{
public:
	virtual void NewAlarm( TA_Base_Core::AlarmDetailCorbaDef& detail ) = 0;
	virtual void UpdateAlarm( TA_Base_Core::AlarmUpdateCorbaDef& update ) = 0;
	virtual void RebuildAlarms() = 0;
};


class AlarmStoreHarnessImpl 
{
public:
	AlarmStoreHarnessImpl();
	virtual ~AlarmStoreHarnessImpl();

	void init();

	void setCallback(AlarmStoreAction* callback);
	std::string newAlarm( AlarmMessageDetail & detail );
	void updateAlarm(int index, const AlarmMessageDetail & detail);
	void rebuildAlarms();
	void checkSharedMemory();
	void removeLocationAlarms(unsigned long locationKey);

	static int getNewAlarmCount() ;
	static int getAckedAlarmCount() ;
	static int getClosedAlarmCount() ;

	static int getUpdateCommentsCount();
	static int getUpdateMmsStateCount();
	static int getPlanAssociatedWithAlarmWasPostponedCount();

private:
	std::string generateAlarmId();
	unsigned long getNextMessageNumber();

private:
	std::auto_ptr<TA_Base_Core::AlarmSender> m_alarmSender;
	std::auto_ptr<TA_Base_Core::StateUpdateMessageSender> m_stateSender;
	unsigned long m_messageSequenceCount;

	bool m_init;

	static int m_newAlarmCount;
	static int m_ackedAlarmCount;
	static int m_closedAlarmCount;
	static int m_updateCommentsCount;
	static int m_updateMmsStateCount;
	static int m_planAssociatedWithAlarmWasPostponedCount;

	AlarmStoreAction* m_alarmStoreAction;
};


class DataParser
{
public:
	static DataParser& getInstance();

	bool populateFromFile( const std::string & fileName );
	bool nextAlarmDetail( AlarmMessageDetail & data );
	bool hasData();

private:
	bool privHasData() const;
	void fillupAlarmDate( int index, const std::string & value, AlarmMessageDetail & data );
	bool nextValidLine( std::istream& file, std::string& line );
	void split( const std::string &line, const char flag, std::vector<std::string> &result );
	bool alarmIdIncrease( const std::string &alarmId , std::string& newID, int count = 1);

	DataParser();
	~DataParser();

	static DataParser* m_instance;

private:
	std::vector< AlarmMessageDetail > m_newAlarms;
	std::vector< AlarmMessageDetail > m_updateAlarms;
	std::vector< AlarmMessageDetail > m_loadedAlarms;

	std::vector< AlarmMessageDetail > m_alarmDatas;
	std::vector< SendingType > m_sendingTypes;

	volatile unsigned int m_currentIndex;
	volatile unsigned int m_circleTimes;
	volatile unsigned int m_totalIncreaseCount;
	volatile unsigned int m_increaseIndex;

	TA_Base_Core::ReEntrantThreadLockable m_paserLock;
	TA_Base_Core::ReEntrantThreadLockable m_lock;
	//unsigned long m_locationKey;
};


class DataOutputSaver
{
private:
	DataOutputSaver();
	virtual ~DataOutputSaver();

public:
	static DataOutputSaver& Instance();
	void OutputResultTxt( const std::string &fileName, displayVec* vec );
	void OutputResultHTML( const std::string &fileName, displayVec* vec ,bool );

private:
	static DataOutputSaver* _instance;
	
};

#endif // !defined(AFX_SIMALARMAGENTIMPL_H__323A0548_0144_4ECC_93C9_5538DE239156__INCLUDED_)

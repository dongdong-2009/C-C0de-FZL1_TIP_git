// AlarmDetail.h: interface for the AlarmDetail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMDETAIL_H__AA4DA608_0939_4B00_A43C_7F50ED8D0979__INCLUDED_)
#define AFX_ALARMDETAIL_H__AA4DA608_0939_4B00_A43C_7F50ED8D0979__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>

enum SendingType
{
	SENDING_NewAlarm,
	SENDING_UpdateAlarm,
	SENDING_UnsupressAlarm
};

struct AlarmMessageDetail
{
	AlarmMessageDetail()
		: increaseSequence(false), alarmIncreaseCount(0)
		, assocEntityKey(0), severity(0), messageTypeKey(0)
		, locationKey(0) 
		, alarmId(""), state(""), alarmDesc(""), assetName("")
		, entityName(""), comment(""), alarmValue(""), MssState("")
		, isUnique(false)
	{};

	unsigned long	alarmIncreaseCount; // save the value of 'TestAlarmIncrease' flag
	bool	increaseSequence;
	std::string		alarmId;
	std::string		state;
	unsigned long	assocEntityKey;
	unsigned long	severity;
	std::string		alarmDesc;
	std::string		assetName;
	std::string		entityName;
	std::string		comment;
	std::string		alarmValue;
	std::string		MssState;
	unsigned long	messageTypeKey;
	unsigned long	locationKey;
	bool	isUnique;

};

extern std::string printAMD(const AlarmMessageDetail& detail);


class CorbaParameterDetail
{
public:
	CorbaParameterDetail();
	virtual ~CorbaParameterDetail(); 

	unsigned int type() const { return m_type; }

private:
	unsigned int m_type;
};


#endif // !defined(AFX_ALARMDETAIL_H__AA4DA608_0939_4B00_A43C_7F50ED8D0979__INCLUDED_)

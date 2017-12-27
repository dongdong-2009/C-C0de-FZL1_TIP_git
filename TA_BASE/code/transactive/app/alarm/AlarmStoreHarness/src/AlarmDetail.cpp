// AlarmDetail.cpp: implementation of the AlarmDetail class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmDetail.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string printAMD(const AlarmMessageDetail& detail)
{
	std::stringstream output;
	output << "alarmId:" << detail.alarmId << ";"
		<< "state:" << detail.state << ";"
		<< "entityKey:" << detail.assocEntityKey << ";"
		<< "severity:" << detail.severity << ";"
		<< "desc:" << detail.alarmDesc << ";"
		<< "asset:" << detail.assetName << ";"
		<< "entityName:" << detail.entityName << ";"
		<< "comment:" << detail.comment << ";"
		<< "value:" << detail.alarmValue << ";"
		<< "MssState:" << detail.MssState << ";"
		<< "msgTypeKey:" << detail.messageTypeKey << ";"
		<< "alarmIncreaseCount:" << detail.alarmIncreaseCount << ";"
		<< "locationKey:" << detail.locationKey << ";"
		<< "increaseSequence:" << (int)detail.increaseSequence << std::endl;
	
	return output.str().c_str();
}


CorbaParameterDetail::CorbaParameterDetail()
{

}


CorbaParameterDetail::~CorbaParameterDetail()
{

}


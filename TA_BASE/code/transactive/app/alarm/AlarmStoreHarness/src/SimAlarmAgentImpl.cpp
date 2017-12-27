// SimAlarmAgent.cpp: implementation of the SimAlarmAgent class.
//
//////////////////////////////////////////////////////////////////////
#include <afx.h>
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
#include "SimAlarmAgentImpl.h"
#include "omniORB4/CORBA.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/AlarmStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
// #include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
// #include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/utilities/src/DebugUtilMacros.h"

#include "bus/alarm/alarm_store_library/src/SharedAlarmAccessFactory.h"

#include <iomanip>
#include <fstream>
#include <boost/tokenizer.hpp>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

const int Item_State		= 0;
const int Item_AlarmId		= 1;
const int Item_EntityKey	= 2;
const int Item_EntityName	= 3;
const int Item_Severity		= 4;
const int Item_AssetName	= 5;
const int Item_AlarmValue	= 6;
const int Item_Description	= 7;
const int Item_Comments		= 8; 
const int Item_MssState		= 9; 
const int Item_MessageTypeKey = 10;
const int Item_LocationKey	= 11;
const int Item_isUnique		= 12;

/*
const int Item_ackAlarm			= 0;
const int Item_closeAlarm		= 1;
const int Item_updateComments	= 2;
const int Item_updateMmsState	= 3;
const int Item_updateAlarmValue = 4;
const int Item_updateAlarmPlan  = 5;
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int AlarmStoreHarnessImpl::m_newAlarmCount = 0;
int AlarmStoreHarnessImpl::m_ackedAlarmCount = 0;
int AlarmStoreHarnessImpl::m_closedAlarmCount = 0;
int AlarmStoreHarnessImpl::m_updateCommentsCount = 0;
int AlarmStoreHarnessImpl::m_updateMmsStateCount = 0;
int AlarmStoreHarnessImpl::m_planAssociatedWithAlarmWasPostponedCount = 0;


AlarmStoreHarnessImpl::AlarmStoreHarnessImpl()
: m_alarmSender(TA_Base_Core::AlarmPublicationManager::getInstance().getAlarmSender(TA_Base_Core::Alarm::Context))
, m_stateSender(TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(TA_Base_Core::AlarmStateUpdate::Context))
, m_messageSequenceCount(1)
, m_init(false)
, m_alarmStoreAction(NULL)
{
	srand(GetTickCount());
}


AlarmStoreHarnessImpl::~AlarmStoreHarnessImpl()
{

}


std::string AlarmStoreHarnessImpl::generateAlarmId()
{
	//unsigned long field1;
	unsigned long field2, field3, field4;
	//field1 = rand() * rand();
	field2 = rand() * rand();
	field3 = rand() * rand();
	field4 = rand() * rand();

	std::stringstream idstr;
	idstr << std::setw(8) << std::setbase(16) << std::setfill('0')
		//<< std::setw(8) << field1
		<< std::setw(8) << field2
		<< std::setw(8) << field3
		<< std::setw(8) << field4;

	std::string result = "0xfafafafa-";
	result += idstr.str();

	return result;
}


void AlarmStoreHarnessImpl::rebuildAlarms()
{
// 	TA_Base_Bus::SharedAlarmAccessFactory::getInstance().rebuildIndexWithDuty();
	if(m_alarmStoreAction != NULL)
	{
		m_alarmStoreAction->RebuildAlarms();
	}

}

void AlarmStoreHarnessImpl::checkSharedMemory()
{
	TA_Base_Bus::TA_SharedAlarm sa;
	sa.checkSharedMemory();
}

void AlarmStoreHarnessImpl::removeLocationAlarms(unsigned long locationKey)
{
 	TA_Base_Bus::TA_SharedAlarm sa;
 	sa.removeLocationAlarms(locationKey);
}


void AlarmStoreHarnessImpl::init()
{
	if( !m_init )
	{
		// TODO: Nothing here
	}
}

void AlarmStoreHarnessImpl::setCallback(AlarmStoreAction* callback)
{
	m_alarmStoreAction = callback;
}


unsigned long AlarmStoreHarnessImpl::getNextMessageNumber()
{
// 	return 0xFFFFFFFF; // Note: Only this value can pass through AlarmStoreService sequence check.

	m_messageSequenceCount++;
	
	if( m_messageSequenceCount == ULONG_MAX )
	{
		m_messageSequenceCount = 1;
	}
	
	return m_messageSequenceCount;
}


int AlarmStoreHarnessImpl::getNewAlarmCount()
{
	return m_newAlarmCount;
}


int AlarmStoreHarnessImpl::getAckedAlarmCount()
{
	return m_ackedAlarmCount;
}


int AlarmStoreHarnessImpl::getClosedAlarmCount()
{
	return m_closedAlarmCount;
}


int AlarmStoreHarnessImpl::getUpdateCommentsCount()
{
	return m_updateCommentsCount;
}

int AlarmStoreHarnessImpl::getUpdateMmsStateCount()
{
	return m_updateMmsStateCount;
}

int AlarmStoreHarnessImpl::getPlanAssociatedWithAlarmWasPostponedCount()
{
	return m_planAssociatedWithAlarmWasPostponedCount;
}


std::string AlarmStoreHarnessImpl::newAlarm( AlarmMessageDetail & detail )
{
	TA_Base_Core::AlarmDetailCorbaDef alarm;

	try
	{
		if( detail.increaseSequence )
		{
			getNextMessageNumber();
			return "";
		}
		
		timeb taTime;
		ftime( &taTime );
		alarm.transactiveTime.time = taTime.time;
		alarm.transactiveTime.millitm = taTime.millitm;
		
		//alarm.alarmID = CORBA::string_dup( generateAlarmId().c_str() );
		alarm.alarmID = CORBA::string_dup( detail.alarmId.c_str() );
		
		//alarm.locationKey = atol(TA_Base_Core::RunParams::getInstance().get("LocationKey").c_str());
		alarm.locationKey = detail.locationKey;
		alarm.assocEntityKey = detail.assocEntityKey;
		
		alarm.alarmAcknowledgedBy = CORBA::string_dup("");
		alarm.alarmSeverity = detail.severity;
		alarm.alarmComments = CORBA::string_dup( detail.comment.c_str() );
		alarm.alarmDescription = CORBA::string_dup( detail.alarmDesc.c_str() );
		alarm.alarmValue = CORBA::string_dup( detail.alarmValue.c_str() );
		alarm.assetName = CORBA::string_dup( detail.assetName.c_str() );
		alarm.entityName = CORBA::string_dup( detail.entityName.c_str() );
		alarm.mmsState = TA_Base_Core::MMS_AUTO;
		alarm.state = TA_Base_Core::AlarmOpen;
		
		alarm.messageTypeKey = detail.messageTypeKey;
		alarm.messageSequenceNumber = getNextMessageNumber();
		alarm.decisionSupportState = TA_Base_Core::NoRelatedPlan;
		
		alarm.isUniqueAlarm = detail.isUnique;
		// 	m_alarmSender->sendAlarm( alarm );
		//	TA_Base_Bus::SharedAlarmAccessFactory::getInstance().insertAlarm(alarm, true);
		// 	TA_Base_Bus::TA_SharedAlarm cs;
		// 	cs.insertAlarm(alarm, true);
		if(m_alarmStoreAction != NULL)
		{
			m_alarmStoreAction->NewAlarm(alarm);
		}
		
		m_newAlarmCount++;

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== Caught a unknown exception in AlarmStoreHarnessImpl::newAlarm");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== %s", printAMD(detail).c_str());
	}

	return alarm.alarmID;
}


void AlarmStoreHarnessImpl::updateAlarm(int index, const AlarmMessageDetail & detail)
{
	try
	{
		if( detail.increaseSequence )
		{
			getNextMessageNumber();
			return;
		}

		timeb taTime;
		ftime( &taTime );
		TA_Base_Core::AlarmUpdateCorbaDef update;
		update.alarmID = CORBA::string_dup( detail.alarmId.c_str() );
		//update.locationKey = atol(TA_Base_Core::RunParams::getInstance().get("LocationKey").c_str());
		update.locationKey = detail.locationKey;
		update.assocEntityKey = detail.assocEntityKey;

		update.alarmHasBeenClosed = false;
		update.alarmHasBeenOpened = false;
		update.alarmHasBeenAcknowledged = false;
		update.commentsWereAdded = false;
		update.mmsStateHasChanged = false;
		update.decisionSupportStateHasChanged = false;

		update.alarmAcknowledgedBy = CORBA::string_dup( "Harness" );
		update.ackTime = time( NULL );

		update.alarmDescription = CORBA::string_dup( detail.alarmDesc.c_str() );
		update.assetName = CORBA::string_dup( detail.assetName.c_str() );

		update.messageSequenceNumber = getNextMessageNumber();
		update.messageTypeKey = detail.messageTypeKey;

		update.mmsState = TA_Base_Core::MMS_AUTO;
		update.decisionSupportState = TA_Base_Core::NoRelatedPlan;
		
		switch(index)
		{
		case Item_ackAlarm:
			update.alarmHasBeenAcknowledged = true;
			update.alarmAcknowledgedBy = CORBA::string_dup("AckTester");
			m_ackedAlarmCount++;
			break;
		case Item_closeAlarm:
			update.alarmHasBeenAcknowledged = true;
			update.alarmHasBeenClosed = true;
			m_closedAlarmCount++;
			break;
		case Item_updateComments:
			update.commentsWereAdded = true;
			update.alarmComments = CORBA::string_dup(detail.comment.c_str());
			m_updateCommentsCount++;

			break;
		case Item_updateMmsState:
			update.mmsStateHasChanged = true;
			try
			{
				update.mmsState = (TA_Base_Core::MmsStateType)atoi(detail.MssState.c_str());
			}
			catch (...) 
			{
				// TODO: 
				std::stringstream testMmsState;
				testMmsState << "mmsState input error ";
			}
			m_updateMmsStateCount++;
			break;
			
		case Item_updateAlarmValue:
			//update.closeValue =  CORBA::string_dup("888");
			break;	
		/*	
		case Item_updateAlarmPlan:
			m_planAssociatedWithAlarmWasPostponedCount++;
			break;
		*/	
		default:
			break;
		}

	// 	m_alarmSender->sendAlarmUpdate( update );
	//	TA_Base_Bus::SharedAlarmAccessFactory::getInstance().updateAlarm(update);
	// 	TA_Base_Bus::TA_SharedAlarm cs;
	// 	cs.updateAlarm(update);
		if(m_alarmStoreAction != NULL)
		{
			m_alarmStoreAction->UpdateAlarm(update);
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== Caught a unknown exception in AlarmStoreHarnessImpl::updateAlarm");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== %s", printAMD(detail).c_str());
	}
	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// DataParser
DataParser* DataParser::m_instance = NULL;
DataParser::DataParser()
: m_currentIndex(0)
, m_circleTimes(0)
, m_totalIncreaseCount(0)
, m_increaseIndex(0)
{
	srand(GetTickCount());
	m_sendingTypes.push_back(SENDING_NewAlarm);
	m_sendingTypes.push_back(SENDING_UpdateAlarm);
	m_sendingTypes.push_back(SENDING_UnsupressAlarm);

// 	m_locationKey = 1;
// 	if(TA_Base_Core::RunParams::getInstance().isSet("LocationKey"))
// 	{
// 		m_locationKey = atol(TA_Base_Core::RunParams::getInstance().get("LocationKey").c_str());
// 	}
}


DataParser::~DataParser()
{

}


DataParser& DataParser::getInstance()
{
	if( m_instance == NULL )
		m_instance = new DataParser();

	return *m_instance;
}

int seh_handle(EXCEPTION_POINTERS *ep)
{
	DWORD ecode = ep->ExceptionRecord->ExceptionCode;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught SEH. SEH exception code: [%d]", ecode);

	return EXCEPTION_EXECUTE_HANDLER;
}

bool DataParser::alarmIdIncrease(const std::string &alarmId, std::string& newID, int count /* =1 */)
{
// 		std::vector< std::string > alarmIdSection;
// 		split( alarmId, '-', alarmIdSection );
// 		ASSERT( alarmIdSection.size() == 3 );


// 		int idNum = atoi(alarmIdSection[2].c_str());
// 		idNum += count;
// 		
// 		std::stringstream ss_alarmId;
// 		ss_alarmId << alarmIdSection[0] << "-"
// 			<< alarmIdSection[1] << "-"
// 			<< std::setw(4) << std::setfill('0') << idNum;
// 		
// 		newID = ss_alarmId.str().c_str();
	


// 	__try
// 	{
	char idBuff[16] = {0};
	try
	{
		int idNum = atoi(alarmId.c_str()) + count;
		sprintf(idBuff,"%010li", idNum);
		
		newID = idBuff;
	}
	catch (std::bad_alloc& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "!!!!!!!!! %s ", e.what());
		newID = idBuff;
	}
// 	}
// 	__except(seh_handle(GetExceptionInformation()))
// 	{
// 		return false;
// 	}

	return true;
}


bool DataParser::nextValidLine( std::istream& file, std::string& line )
{
	while( std::getline( file, line ) )
	{
		if( *line.c_str() != '#' && !line.empty() )
			return true;
	}
	
	return false;
}


void DataParser::split(const std::string &line, const char flag, std::vector<std::string> &result )
{
	// End Flag
	std::string::const_iterator iter = line.begin();
	std::string::const_iterator iterBegin = iter;
	std::string::const_iterator iterEnd = iter;

	std::vector< std::string > params;

	bool newItem = true;
	std::string paramValue = "";

	AlarmMessageDetail alarmData;

	for( ; iter != line.end(); iterEnd = ++iter )
	{
		// ignore space character
		if( *iter == ' ' || *iter == 9 /* tab */ ) continue;
		if( newItem )
		{
			iterBegin = iter;
			newItem = false;
		}

		if( *iter == flag )
		{
			newItem = true;
			paramValue = std::string( iterBegin, iterBegin );
			result.push_back( paramValue );
		}
	}

	if( iterBegin != iterEnd )
	{
		paramValue = std::string( iterBegin, iterEnd );
		result.push_back( paramValue );
	}
}


bool DataParser::populateFromFile( const std::string & fileName )
{
	std::ifstream file(fileName.c_str());
	if( file.fail() )
		return false;

	ThreadGuard guard(m_paserLock);

	m_alarmDatas.clear();
	m_currentIndex = 0;
	m_totalIncreaseCount = 0;
	m_increaseIndex = 0;

	std::string line;
	bool validDataLine =false;
	while( nextValidLine(file, line)/*std::getline( file, line )*/ )
	{
		// extract message type
		if( *line.c_str() != '#' && !line.empty() )
		{
			// remove the space of the beginning and the end of line
			line = std::string( line.c_str(), line.find_first_not_of(" "), line.find_last_not_of(" ") + 1 );

			// Find Start Flag
			if( line.compare("TestInputSendStart") == 0 )
			{
				validDataLine = true;
				continue;
			}

			if( !validDataLine )
				continue;

			// Got End Flag
			if( line.compare("TestInputSendEnd") == 0 )
			{
				if( nextValidLine(file, line)/*std::getline( file, line )*/ )
				{					
					if( line.find("TestCircleNumber:") != std::string::npos )
					{
						int pos = line.find_last_of(":");
						line = line.substr(pos + 1, line.length() - pos); 
						m_circleTimes = atoi(line.c_str());
					}
				}
				break;
			}


			if( line.compare( "IncreaseSequence" ) == 0 )
			{
				AlarmMessageDetail incData;
				incData.increaseSequence = true;
				m_alarmDatas.push_back(incData);
			}
			else if( line.find("TestAlarmIncrease:") != std::string::npos )
			{
				int pos = line.find_last_of(":");
				line = line.substr(pos + 1, line.length() - pos); 

				std::vector< std::string > tempResult;
				split( line, ',', tempResult );
				ASSERT( tempResult.size() == 1 );
				int increaseCount = atoi(tempResult[0].c_str());
				AlarmMessageDetail& preData = m_alarmDatas[m_alarmDatas.size() - 1];
				preData.alarmIncreaseCount = increaseCount; // save the increase count
				m_totalIncreaseCount += (increaseCount-1);
			}
			else
			{
				std::vector< std::string > parseResult;
				split( line, ',', parseResult );
				AlarmMessageDetail alarmData;
			
				for( int i = 0; i < (int)parseResult.size(); ++i )
				{
					fillupAlarmDate( i, parseResult[i], alarmData );
				}
				m_alarmDatas.push_back(alarmData);
			}	
		}
	}

	file.close();

	return true;
}


void DataParser::fillupAlarmDate( int index, const std::string & value, AlarmMessageDetail & data )
{
	switch (index)
	{
	case Item_State:
		data.state = value.c_str();
		break;
	case Item_AlarmId:
		data.alarmId = value.c_str();
		break;
	case Item_EntityKey:
		data.assocEntityKey = atol( value.c_str() );
		break;
	case Item_EntityName:
		data.entityName = value.c_str();
		break;
	case Item_Severity:
		data.severity = atol( value.c_str() );
		break;
	case Item_AssetName:
		data.assetName = value.c_str();
		break;
	case Item_AlarmValue:
		data.alarmValue = value.c_str();
		break;
	case Item_Description:
		data.alarmDesc = value.c_str();
		break;
	case Item_Comments:
		data.comment = value.c_str();
		break;
	case Item_MssState:
		data. MssState = value.c_str();
		break;
	case Item_MessageTypeKey:
		data.messageTypeKey = atol(value.c_str());
		break;
	case Item_LocationKey:
		data.locationKey = atol(value.c_str());
		break;
	case Item_isUnique:
		data.isUnique = atoi( value.c_str() ) != 0;
	default:
		break;
	}
}


bool DataParser::nextAlarmDetail( AlarmMessageDetail & data )
{
	ThreadGuard guard(m_paserLock);
	bool gotData = false;
	try
	{
		if( privHasData() ) 
		{
			unsigned int maxTimes = m_alarmDatas.size();
			
			if( m_circleTimes > 0 )
			{
				maxTimes = maxTimes * m_circleTimes;
			}
			
			if( m_currentIndex < maxTimes )
			{
				data = m_alarmDatas[m_currentIndex % m_alarmDatas.size()];
				
				if(data.alarmIncreaseCount > 1)
				{
					if(m_increaseIndex > 0)
					{
						if(!alarmIdIncrease(data.alarmId, data.alarmId, m_increaseIndex))
						{
							return false;
						}
					}
					
					if(m_increaseIndex >= data.alarmIncreaseCount-1)
					{
						m_increaseIndex = 0;
						++m_currentIndex;
					}
					else
					{
						++m_increaseIndex;
					}
				}
				else
				{
					m_currentIndex = 0;
					++m_currentIndex;
				}
				
				gotData = true;
			}
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== Caught a unknown exception in nextAlarmDetail");
	}

	return gotData;
}

bool DataParser::hasData()
{
	ThreadGuard guard(m_paserLock);
	return privHasData();
}

bool DataParser::privHasData() const
{
	return m_alarmDatas.size() != 0;
}

//////////////////////////////////////////////////////////////////////////////////////
///

DataOutputSaver* DataOutputSaver::_instance = NULL;
DataOutputSaver::DataOutputSaver()
{
}

DataOutputSaver::~DataOutputSaver()
{
}

DataOutputSaver& DataOutputSaver::Instance()
{
	if( _instance == NULL )
	{
		_instance = new DataOutputSaver();
	}

	return *_instance;
}

void DataOutputSaver::OutputResultTxt( const std::string &fileName, displayVec* vec )
{
	if( vec == NULL )
		return;

	std::ofstream file(fileName.c_str());
	if( file.fail() )
		return;

	std::string inputInfo = "#===================== input action ===============\r\n";
	file << inputInfo;

	std::stringstream ssAction;
	ssAction << "NewAlarm: " << AlarmStoreHarnessImpl::getNewAlarmCount() << "\r\n"	
		<< "AckedAlarm: " << AlarmStoreHarnessImpl::getAckedAlarmCount() << "\r\n"
		<< "UpdateComments: " << AlarmStoreHarnessImpl::getUpdateCommentsCount() << "\r\n"
		<< "UpdateMmsState: " << AlarmStoreHarnessImpl::getUpdateMmsStateCount() << "\r\n"
		<< "ClosedAlarm: " << AlarmStoreHarnessImpl::getClosedAlarmCount() << "\r\n\r\n";


		
	file << ssAction.str();

	std::string startLine = "#=========================Start output alarm message==========================\r\n";
	file << startLine;
	std::string columnNameLine = "#AlarmId,\t\t\t\t\t\t\t\tState,\tMessageTypeKey, AlarmSeverity, AssocEntityKey, EntityName, AssetName, ";
	columnNameLine += "AlarmValue, AlarmComments, transactiveTime, avalancheHeadID, alarmAcknowledgedBy, decisionSupportState, ",
	columnNameLine += "mmsState, subsystemKey, assocEntityTypeKey, avalanche\r\n\r\n";
	file << columnNameLine;

	int newAlarmCount = 0, AckedAlarmCount = 0, ClosedAlarmCount = 0;


	int testAlarmCount = 0;
	displayVec::iterator iter = vec->begin();
	for( ; iter != vec->end(); ++iter )
	{
		TA_Base_Core::AlarmItem* alarmItem = *iter;
		std::string alarmId = alarmItem->alarmID;

		// filter
		std::string tmp(alarmId.c_str(),0,11);
		if( tmp.compare("0xfafafafa-") != 0 )
			continue;

		unsigned long tm = alarmItem->transactiveTime.time;
		CTime time(tm);
		CString str = time.Format("%d/%m/%Y-%H:%M:%S");
		std::string timeString = str.GetBuffer(str.GetLength());

		std::stringstream ss;
		ss << alarmId << ", "
			<< alarmItem->state << ", "
			<< alarmItem->messageTypeKey << ", "
			<< alarmItem->alarmSeverity << ", "
			<< alarmItem->assocEntityKey << ", "
			<< alarmItem->entityName << ", "
			<< alarmItem->assetName << ", "
			<< alarmItem->alarmValue << ", "
			<< alarmItem->alarmComments << ", "
			<< timeString << ", "
			<< alarmItem->avalancheHeadID << ", "
			<< alarmItem->alarmAcknowledgedBy << ", "
			<< alarmItem->decisionSupportState << ", "
			<< alarmItem->mmsState << ", "
			<< alarmItem->subsystemKey << ", "
			<< alarmItem->assocEntityTypeKey << ", "
			<< alarmItem->avalanche ;

		++testAlarmCount;
		file << ss.str() << "\r\n";
		
	}

	std::string endLine = "#==========================End output alarm message============================";
	file << endLine << "\r\n";
	std::stringstream summary;
	summary << "AllTestAlarmCount " << testAlarmCount << "\r\n" 
		<< "#UserNewAlarmNumber : " << newAlarmCount << "\r\n"
		<< "#UserAckAlarmNumber : " << AckedAlarmCount << "\r\n"
		<< "#UserCloseAlarmNumber : " << ClosedAlarmCount << "\r\n";

	file << summary.str();

	file.flush();
	file.close();
}

void DataOutputSaver::OutputResultHTML( const std::string &fileName, displayVec* vec ,bool isAllData)
{
	if( vec == NULL )
		return;

	std::ofstream file(fileName.c_str());
	if( file.fail() )
		return;

	std::string inputInfo = "<h1>Input data count action</h1>";
	file << inputInfo;

	std::stringstream ssAction;
	ssAction << "NewAlarm: " << AlarmStoreHarnessImpl::getNewAlarmCount() << "<BR>"	
		<< "AckedAlarm: " << AlarmStoreHarnessImpl::getAckedAlarmCount() << "<BR>"
		<< "UpdateComments: " << AlarmStoreHarnessImpl::getUpdateCommentsCount() << "<BR>"
		<< "UpdateMmsState: " << AlarmStoreHarnessImpl::getUpdateMmsStateCount() << "<BR>"
		<< "ClosedAlarm: " << AlarmStoreHarnessImpl::getClosedAlarmCount() << "<BR>\r\n";
		
	file << ssAction.str();

	//std::string startLine = "#=========================Start output alarm message==========================<BR>";
	std::string startLine = "<h1>Start output share memory alarm message</h1>";

	file << startLine;
	std::string columnNameLine = "<TABLE border=1 BORDERCOLOR=red style=\"border-collapse:collapse;\
		border-color:red\"><THEAD><TR><TD width=150>AlarmId<font color=white>.......................\
...........</font></TD><TD>State</TD><TD>MessageTypeKey</TD><TD>AlarmSeverity</TD><TD>AssocEntityKey</TD>\
		<TD>EntityName</TD><TD>AssetName</TD><TD>";
	columnNameLine += "AlarmValue</TD><TD>AlarmComments</TD><TD>transactiveTime<font color=white>........\
........</font></TD><TD>avalancheHeadID</TD><TD>alarmAcknowledgedBy</TD><TD>decisionSupportState</TD>\
	<TD>",\
	columnNameLine += "mmsState</TD><TD>subsystemKey</TD><TD>assocEntityTypeKey</TD><TD>avalanche</TD>\
	</TR></THEAD><TBODY>";
	file << columnNameLine;

	int newAlarmCount = 0, AckedAlarmCount = 0, ClosedAlarmCount = 0;


	int testAlarmCount = 0;
	displayVec::iterator iter = vec->begin();
	for( ; iter != vec->end(); ++iter )
	{
		TA_Base_Core::AlarmItem* alarmItem = *iter;
		
		std::string alarmId = alarmItem->alarmID;

		// filter
		std::string tmp(alarmId.c_str(),0,11);
		if( !isAllData && (tmp.compare("0xfafafafa-") != 0 ))
			continue;

		
		unsigned long tm = alarmItem->transactiveTime.time;
		CTime time(tm);
		CString str = time.Format("%d/%m/%Y-%H:%M:%S");
		std::string timeString = str.GetBuffer(str.GetLength());
		
		std::stringstream ss;
		ss <<"<TR>"
			<<"<TD>"<< alarmId << "</TD> "
			<<"<TD>"<< alarmItem->state << "</TD> "
			<<"<TD>"<< alarmItem->messageTypeKey << "</TD> "
			<<"<TD>"<< alarmItem->alarmSeverity << "</TD> "
			<<"<TD>"<< alarmItem->assocEntityKey << "</TD> "
			<<"<TD>"<< alarmItem->entityName << "</TD> "
			<<"<TD>"<< alarmItem->assetName << "</TD> "
			<<"<TD>"<< alarmItem->alarmValue << "</TD> "
			<<"<TD>"<< alarmItem->alarmComments << "</TD> "
			<<"<TD>"<< timeString << "</TD> "
			<<"<TD>"<< alarmItem->avalancheHeadID << "</TD> "
			<<"<TD>"<< alarmItem->alarmAcknowledgedBy << "</TD> "
			<<"<TD>"<< alarmItem->decisionSupportState << "</TD> "
			<<"<TD>"<< alarmItem->mmsState << "</TD> "
			<<"<TD>"<< alarmItem->subsystemKey << "</TD> "
			<<"<TD>"<< alarmItem->assocEntityTypeKey << "</TD> "
			<<"<TD>"<< alarmItem->avalanche<<"</TD>" 
			<<"</TR>";

		++testAlarmCount;
		file << ss.str() <<"\r\n";
		
	}
	
	std::string endLine = "</TBODY></TABLE><h1>Ouput real result count</h1>";
	file << endLine << "<BR>";
	std::stringstream summary;
	summary << "TestAlarmCount " << testAlarmCount << "<BR>" ;
		/*
		<< "UserNewAlarmNumber : " << newAlarmCount << "<BR>"
		<< "UserAckAlarmNumber : " << AckedAlarmCount << "<BR>"
		<< "UserCloseAlarmNumber : " << ClosedAlarmCount << "<BR>";
		*/
	file << summary.str();

	file.flush();
	file.close();
}

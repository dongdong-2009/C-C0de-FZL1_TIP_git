#include "StdAfx.h"
#include "DataGenerator.h"
#include "CommonDef.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\message\types\MessageTypes.h"
#include "core\message\src\MessageConfig.h"

#include "core\alarm\src\AlarmConfig.h"
#include "core\data_access_interface\src\DataTypeEnums.h"
#include "core\data_access_interface\src\DatabaseFactory.h"
#include "core\data_access_interface\src\AlarmSeverityAccessFactory.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core/uuid/src/TAuuid.h"
#include <boost/regex.hpp>

CDataGenerator* CDataGenerator::m_pInstance=0;
TA_Base_Core::NonReEntrantThreadLockable CDataGenerator::m_singletonLock;

using namespace TA_Base_Core;

CDataGenerator::CDataGenerator(void)
:m_ulCurrSeqNum(0)
{
}

CDataGenerator::~CDataGenerator(void)
{
}


CDataGenerator& CDataGenerator::getInstance()
{
	if (0 != m_pInstance)
	{
		return *m_pInstance;
	}
	
	ThreadGuard guard(m_singletonLock);
	if (0==m_pInstance)
	{
		m_pInstance = new CDataGenerator();
	}

	return *m_pInstance;
}

void CDataGenerator::releaseInstance()
{
	ThreadGuard guard(m_singletonLock);
	if (0!=m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=0;
	}
}

void CDataGenerator::initialize(std::vector<std::string>& vi)
{
	ThreadGuard guard(m_threadLock);
	// access database
	std::vector<std::string> columnNames;

	columnNames.push_back("mat.type_description");
	columnNames.push_back("mat.meatyp_id");
	columnNames.push_back("mmc.context_name");
	columnNames.push_back("mec.channel_name");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
	std::string sql = "SELECT mat.type_description, mat.meatyp_id,mmc.context_name, mec.channel_name "\
		"FROM ME_ALARM_TYPE mat, ME_MESSAGE_CONTEXT mmc,ME_EVENT_CHANNEL mec "\
		"WHERE mat.isvisible=1 and mat.memcon_id = mmc.memcon_id and mmc.meecha_id = mec.meecha_id";

	//agDebug("SQL Exec: ",sql.c_str());

	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data =  databaseConnection->executeQuery(sqlCmd,columnNames);
	std::string tempString;
	bool hasMore = true;
	while ( hasMore && data->getNumRows() > 0 )
	{
		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			param_vec_ptr tmp_param = getParams(data->getStringData(i,"mat.type_description"));
			mt_ptr tmp_mt= new mt(*(new TA_Base_Core::MessageContext(data->getStringData(i, "mec.channel_name").c_str(), 
				data->getStringData(i, "mmc.context_name").c_str(), TA_Base_Core::AlarmMessage)), 
				data->getStringData(i, "mat.meatyp_id").c_str());
			msg_ptr tmp_msg = new s_msg;
			tmp_msg->p_mt = tmp_mt;
			tmp_msg->p_param = tmp_param;
			m_autoalarm.push_back(tmp_msg);
		}
		hasMore = databaseConnection->moreData(data);
	}

	//agDebug("SQL Done.");

	columnNames.clear();
	columnNames.push_back("pkey");
	columnNames.push_back("typekey");
	columnNames.push_back("locationkey");
	columnNames.push_back("subsystemkey");
	columnNames.push_back("name");
	sql = "SELECT pkey, typekey, locationkey, subsystemkey, name FROM ENTITY where locationkey in (";
	std::vector<std::string>::iterator bgn= vi.begin();
	while(bgn != vi.end())
	{
		sql.append(*bgn);
		sql.append(",");
		++ bgn;
	}
	sql.erase(sql.find_last_of(','));
	sql.append(")");


	//YM1 20091023 : 
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "YM LOG: log sql script");
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, sql.c_str());

//	agDebug("SQL Exec: ",sql.c_str());

	TA_Base_Core::SQLStatement sqlCmd2;
	sqlCmd2.strCommon = sql;

	data =  databaseConnection->executeQuery(sqlCmd2,columnNames);
	hasMore = true;
	while ( hasMore && data->getNumRows() > 0 )
	{
		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			entity_ptr tmp_entity = new s_entity;
			tmp_entity->entity_key =data->getIntegerData(i, "pkey");
			tmp_entity->entity_type = data->getIntegerData(i, "typekey");
			tmp_entity->location = data->getIntegerData(i, "locationkey");
			tmp_entity->name = data->getStringData(i, "name");
			tmp_entity->subsys = data->getIntegerData(i, "subsystemkey");
			m_allentity.push_back(tmp_entity);
		}
		hasMore = databaseConnection->moreData(data);
	}

	m_ulEventSize = m_autoalarm.size();
	m_ulEntitySize = m_allentity.size();

}

void CDataGenerator::setSessionID(std::string strSessionID)
{
	m_strSessionID = strSessionID;
}

std::string CDataGenerator::getSessionID()
{
	return m_strSessionID;
}

void CDataGenerator::setEventType(std::string strEventType)
{
	m_nEventTypeIdx = -1;
	
	//Verify if event type is existing
	for (int idx=0; idx!=m_autoalarm.size(); idx++)
	{
		if (m_autoalarm[idx]->p_mt->getTypeKey() == strEventType)
		{
			m_nEventTypeIdx = idx;
			break;
		}
	}
}

void CDataGenerator::setPrefix(std::string strPrefix)
{
	if (strPrefix == "")
	{
		m_strPrefix = DEFAULT_PREFIX;
	}
	else
	{
		m_strPrefix = strPrefix;
	}
}

CDataGenerator::param_vec_ptr CDataGenerator::getParams(const std::string& paramstr)
{
	boost::regex expr("\\[([a-zA-Z]+)]");
	boost::match_results<std::string::const_iterator> res;
	boost::match_flag_type flags = boost::match_default;
	std::string::const_iterator start = paramstr.begin();
	std::string::const_iterator end = paramstr.end(); 
	param_vec_ptr param_p = NULL;
	
	param_p= new param_vec;
	while(boost::regex_search(start, end, res, expr, flags))
	{	
		param_p->push_back(std::string(res[1].first, res[1].second));
		start = res[0].second;      
		// update flags:
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
	return param_p;
}


TA_Base_Core::EventItem* CDataGenerator::createRandomEvent()
{
	TA_Base_Core::EventItem* pRetVal = NULL;

	try
	{
		unsigned long ulSeqNum = getCurSeqNumber();
		srand( (unsigned)time( NULL ) );
		TA_Base_Core::DescriptionParameters m_descriptionParameters;
	
		TA_Base_Core::NameValuePair* ptr[10];
		int curalarmindex = m_nEventTypeIdx;
		if (-1 == curalarmindex)
		{
			curalarmindex = rand() % m_autoalarm.size();
		}
		int curentityindex = rand() % m_allentity.size();

		int i = 0;
		if( m_autoalarm[curalarmindex]->p_param != NULL )
		{
			param_vec::iterator pb = m_autoalarm[curalarmindex]->p_param->begin();
			param_vec::iterator pe = m_autoalarm[curalarmindex]->p_param->end();

			while(pb != pe)
			{
				ptr[i] = new TA_Base_Core::NameValuePair(*pb, "test");
				m_descriptionParameters.push_back(ptr[i]);
				++ pb;
				++ i;
			}
		}	
		mt_ptr tt = m_autoalarm[curalarmindex]->p_mt; 
		std::string AssetName;
		switch(rand()%2)
		{
		case 0:
			AssetName=(m_allentity[curentityindex]->name).c_str();
			break;
		case 1:
			AssetName="tester";
			break;
		default:
			AfxMessageBox("rand error!");	
		}

		std::string SourcTable;
		switch(rand()%2)
		{
		case 0:
			SourcTable="100";
			break;
		case 1:
			SourcTable="200";
			break;
		default:
			AfxMessageBox("rand error!");	
		}
		unsigned long alarmSeverityKey( TA_Base_Core::AlarmTypeTableAccessor::getInstance().getSeverityKey(*(m_autoalarm[curalarmindex]->p_mt)) );

		unsigned long messageTypekey = atol(m_autoalarm[curalarmindex]->p_mt->getTypeKey().c_str());
		std::string description = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( *(m_autoalarm[curalarmindex]->p_mt),  m_descriptionParameters);
		std::string value = getValueFromParameters( *(m_autoalarm[curalarmindex]->p_mt), m_descriptionParameters ).c_str();
		TA_Base_Core::MillisecondTimeEx sourceItem(time(0), 0);
		//description = _formatTime(sourceItem.time) + m_strPrefix + description;

 		CString strDesc; 
		strDesc.Format("%s: Seq:%ld %s desc: %s", m_strPrefix.c_str(), ulSeqNum, _formatTime(sourceItem.time).c_str(), description.c_str());

		pRetVal = new TA_Base_Core::EventItem(
			TA_Base_Core::TA_Uuid::generateUUID().c_str()
			,SourcTable.c_str()
			,sourceItem
			,m_allentity[curentityindex]->subsys
			,10 //Todo
			,m_allentity[curentityindex]->location
			,alarmSeverityKey
			,"" // Todo
			,AssetName.c_str()
			,strDesc.GetBuffer(0)
			,messageTypekey
			,m_autoalarm[curalarmindex]->p_mt->getTypeKey().c_str()
			,TA_Base_Core::TA_Uuid::generateUUID().c_str()
			,10050
			,value.c_str()
			,"S"
			,"D"
			,"Test"
			,622
			,"Larry lim"
			,"No Comment"
			,1
			,""
			,"Alarm Test"
			,m_strSessionID.c_str()
			,m_allentity[curentityindex]->location
			,0
			,2202
			,"1"
			,m_allentity[curentityindex]->entity_key
			,"1"
			,99
			,TA_Base_Core::EvMgsSequenceNumber(0 , 0));

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Generate event with ID %s", pRetVal->eventID);
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error Creating random event");
	}

	return pRetVal;
}

std::string CDataGenerator::getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description)
{
	std::string value("");

	// check whether it is system alarm return ALARM if yes		
	bool isSystem = TA_Base_Core::AlarmTypeTableAccessor::getInstance().isSystemAlarm(type);
	std::string alarmType = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeName(type);


	if (isSystem)
	{
		// Obtain the value from DB configuration
		// Attribute's name = "AlarmValue" in alarm type data
		value = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmValue(type);
	}
	//jinhua++TD17255
	else if (alarmType.find( "CctvTriggeringAlarm", 0  ) != std::string::npos)// this is CCTVTriggeringAlarm** type 		
	{
		value = TA_Base_Core::AlarmTypeTableAccessor::DEFAULT_SYSTEM_ALARM_ALARM_VALUE;
	}
	//++jinhua TD17255
	else // this is not system alarm and can be a datapoint type
	{        
		TA_Base_Core::ConstDescriptionParametersIterator valueParam;
		bool found=false;
		for (valueParam=description.begin(); valueParam!=description.end(); ++valueParam)
		{
			if ( (*valueParam)->name.compare("DataPointValue") == 0)
			{
				found = true;
				break;
			}
		}

		// TD 12785 Only display the value without the alarm type if there is parameter value
		// if not, use the alarm type description
		if(found==true && valueParam != description.end())
		{
			// Check if the text contains DpGeneral, DpFalse and DpTrue
			// Take them out // this is a quick fix for TD 12785
			bool isBoolDatapt = false;

			//TD16813++
			if(alarmType.find("DpGeneral",0) != std::string::npos || alarmType.find("DpFalse",0) !=std::string::npos || alarmType.find("DpTrue",0) !=std::string::npos )
				isBoolDatapt = true;


			if (isBoolDatapt)
			{
				// value is just the value of the bool datapoint
				value = (*valueParam)->value.c_str();
			}
			else
			{
				// value = alarm type + " "(space) + value of the datapoint
				value = alarmType + " ";
				value += (*valueParam)->value.c_str();
			}


		}
		// there is no value
		else 
		{
			// append the alarm type to value
			value = alarmType;
		}

	}

	return value;
}

std::string CDataGenerator::_formatTime(long ulTime)
{
	char szTime[256];
	CTime time(ulTime);
	CString str = time.Format("%d-%b-%y - %H:%M:%S");
	strncpy(szTime, str.GetBuffer(0), sizeof(szTime) - 1);
	return std::string(szTime);
}

// void CDataGenerator::populateEventTypes(CComboBox& cboEventTypes)
// {
// 	msg_vec::iterator itr = m_autoalarm.begin();
// 	for (itr; itr!= m_autoalarm.end(); ++itr)
// 	{
// 		msg_ptr pMsg = *itr;
// 		std::string strEntry = pMsg->p_mt->getTypeKey() + std::string(": ") + pMsg->p_mt->getTypeName() ->getContextObj().getContextName();
// 		cboEventTypes.AddString(strEntry.c_str());
// 		
// 	}
// }

void CDataGenerator::populateEventTypes(CComboBox& cboEventTypes)
{
	// set up the column names vector
	std::vector<std::string> columnNames;

	columnNames.push_back("meatyp_id");
	columnNames.push_back("type_name");
	columnNames.push_back("print_alarm");
	columnNames.push_back("persist_alarm");

	try
	{

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);

		std::string sql = "SELECT meatyp_id, type_name, print_alarm, persist_alarm FROM ME_ALARM_TYPE where ME_ALARM_TYPE.isvisible=1";

		try
		{
			TA_Base_Core::SQLStatement sqlCmd;
			sqlCmd.strCommon = sql;

			IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

			std::string tempString;
			BOOL hasMore = TRUE;
			while ( hasMore && data->getNumRows() > 0 )
			{
				for(int i=0; (unsigned int)i<data->getNumRows(); i++)
				{
					tempString.erase();
					unsigned long key = data->getUnsignedLongData(i, "meatyp_id");
					char buffer[20];
					itoa(key, buffer, 10);
					tempString += buffer;
					tempString += ">";
					tempString += data->getStringData(i,"type_name");;
					tempString += ">To be printed: ";
					data->getBooleanData(i,"print_alarm") ? tempString += "Yes" : tempString += "No";
					tempString += ">To be persisted: ";
					data->getBooleanData(i,"persist_alarm") ?  tempString += "Yes" : tempString += "No";
					cboEventTypes.AddString( tempString.c_str() );
				}
				hasMore = databaseConnection->moreData(data);
			}

			delete data;
			data = NULL;
		}
		catch(TA_Base_Core::DataException& ex)
		{
			AfxMessageBox(ex.what());
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
		}

	}
	catch(TA_Base_Core::DatabaseException& ex)
	{
		AfxMessageBox("Database connection failed\r\nplease check the connection string and try again", MB_ICONSTOP);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
	}

}

void CDataGenerator::resetSeqNumber()
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	m_ulCurrSeqNum = 0;
}

unsigned long CDataGenerator::getCurSeqNumber()
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	m_ulCurrSeqNum +=1;

	return m_ulCurrSeqNum;
}
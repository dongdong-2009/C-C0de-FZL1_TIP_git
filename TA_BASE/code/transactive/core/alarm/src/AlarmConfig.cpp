/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmConfig.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/03/24 16:58:15 $
  * Last modified by:  $Author: Noel $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/alarm/src/AlarmConfig.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/AlarmTypeData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessageConfig.h"

namespace TA_Base_Core
{
	const std::string AlarmTypeTableAccessor::DEFAULT_SYSTEM_ALARM_ALARM_VALUE = "±¨¾¯";
	const std::string AlarmTypeTableAccessor::DEFAULT_SYSTEM_ALARM_NORMAL_VALUE = "Õý³£";
	
	AlarmTypeTableAccessor * AlarmTypeTableAccessor::m_theAlarmConfig = NULL;
	TA_Base_Core::NonReEntrantThreadLockable AlarmTypeTableAccessor::m_singletonLock;

	AlarmTypeTableAccessor::~AlarmTypeTableAccessor()
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		AlarmTypeMap::iterator it = m_alarmTypeMap.begin();
		for ( ; it != m_alarmTypeMap.end(); it++ )
		{
			delete it->second;
		}
		m_alarmTypeMap.clear();
	}


	AlarmTypeTableAccessor& AlarmTypeTableAccessor::getInstance()
	{
		//TD17994,jianghp
		// guard the queue
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		
		if (NULL == m_theAlarmConfig)
		{
			m_theAlarmConfig = new AlarmTypeTableAccessor();
		}
		return *m_theAlarmConfig;
		//TD17994,jianghp
	}

	void AlarmTypeTableAccessor::removeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		
		if (NULL != m_theAlarmConfig)
		{
			delete m_theAlarmConfig;
			m_theAlarmConfig = NULL;
		}
	}

	bool AlarmTypeTableAccessor::isLoggable(const TA_Base_Core::MessageType& messageType)
	{
		// All alarms are logged
		return true;
	}


	bool AlarmTypeTableAccessor::isPrintable(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return false;
		}
		
		try
		{
			return alarmTypeData->toBePrinted();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was printable" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return false;
	}

	
	bool AlarmTypeTableAccessor::isPersistent(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		// Load the message Type data
			TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return false;
		}
		
		try
		{
			return alarmTypeData->toBePersisted();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was persistent" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return false;
	}

	bool AlarmTypeTableAccessor::isSystemAlarm(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		// Load the message Type data
			TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return false;
		}
		
		try
		{
			return alarmTypeData->isSystemAlarm();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was persistent" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return false;
	}

	//TD17862 marc_bugfix support for mms types for alarms
	IAlarmData::EMmsState AlarmTypeTableAccessor::getRelatedMMSType(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		
		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData = getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return IAlarmData::TYPE_NONE;
		}
		try
		{
			return alarmTypeData->getRelatedMMSType();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine mms type for alarm " );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return IAlarmData::TYPE_NONE;

	}

    //Mintao++ TD15302
	bool AlarmTypeTableAccessor::isMMSEnabled(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		// Load the message Type data
			TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return false;
		}
		
		try
		{
			return alarmTypeData->isMMSEnabled();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was MMS enabled" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return false;
	}
    //Mintao++ TD15302
	bool AlarmTypeTableAccessor::isVisible(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		
		// Load the message Type data
			TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return true;
		}
		
		try
		{
			return alarmTypeData->isVisible();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was persistent" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return true; // default
	}

	std::string AlarmTypeTableAccessor::getAlarmTypeName(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		std::string alarmTypeName("Unknown");

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData = getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return alarmTypeName;
		}
		
		try
		{
			alarmTypeName = alarmTypeData->getName();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was persistent" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return alarmTypeName;
	}

	short AlarmTypeTableAccessor::getPriority(const TA_Base_Core::MessageType& messageType)
	{
		// There is no priority for alarms
		return 0;
	}

	
	unsigned long AlarmTypeTableAccessor::getSeverityKey(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return 1;
		}
		
		try
		{
			return alarmTypeData->getSeverityKey();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine Alarm severity" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return 1;
	}

	std::string AlarmTypeTableAccessor::getAlarmValue(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return DEFAULT_SYSTEM_ALARM_ALARM_VALUE;
		}
		
		try
		{
			return alarmTypeData->getAlarmValue();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was printable" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return DEFAULT_SYSTEM_ALARM_ALARM_VALUE;
	}

	std::string AlarmTypeTableAccessor::getNormalValue(const TA_Base_Core::MessageType& messageType)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return DEFAULT_SYSTEM_ALARM_NORMAL_VALUE;
		}
		
		try
		{
			return alarmTypeData->getNormalValue();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was printable" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return DEFAULT_SYSTEM_ALARM_NORMAL_VALUE;
	}

	std::string AlarmTypeTableAccessor::getExpandedDescription(const TA_Base_Core::MessageType& messageType,
			const TA_Base_Core::DescriptionParameters& parameters)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageType.getTypeKey() );

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return "Default";
		}
		
		try
		{
			// The description
			std::string description("");
			// Appended the AlarmType to the front for system alarms' description as required by 4669
			if ( alarmTypeData->isSystemAlarm() )
			{
				description = alarmTypeData->getName() + "- ";
				description += alarmTypeData->getDescription();
				//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Expanded Description: %s", description.c_str() );
			}
			else
			{
				description = alarmTypeData->getDescription();
			}
			
			return gFindAndReplaceParams( description, parameters );
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to format Alarm description" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return "Default";
	}

	std::string AlarmTypeTableAccessor::getExpandedDescription(unsigned long messageTypeKey,
		const std::string& parametersStr)
	{
        

		TA_Base_Core::ThreadGuard guard(m_mapLock);		

		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageTypeKey);

		// Return default if null
		if (NULL == alarmTypeData)
		{
			return "Default";
		}		

		try
		{			
			//memory leak issue luohuirong 24/11/2009
			std::string strExpandedDescription="Default";
			
			TA_Base_Core::DescriptionParameters parameters;
		    gGetParametersFromString( parametersStr, parameters);
			
			// The description
			std::string description("");
			// Appended the AlarmType to the front for system alarms' description as required by 4669
			if ( alarmTypeData->isSystemAlarm() )
			{
				description = alarmTypeData->getDisplayName() + "- ";
				description += alarmTypeData->getDescription();
				//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Expanded Description: %s", description.c_str() );
			}
			else
			{
				description = alarmTypeData->getDescription();
			}
			
			//memory leak issue luohuirong 24/11/2009
			strExpandedDescription= gFindAndReplaceParams( description, parameters );
    
			if (!parameters.empty())
			{
				for (int index = 0; index < static_cast<int>(parameters.size());index++)
				{
					delete parameters[index];
					parameters[index] = NULL;	
				}
			}
			parameters.clear();
            //LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "fix memory issue desc=%s", strExpandedDescription.c_str());
			return strExpandedDescription;
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to format Alarm description" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return "Default";
	}


	TA_Base_Core::IAlarmTypeData* AlarmTypeTableAccessor::getAlarmTypeRecord(
		const std::string& typeKey)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		
		std::stringstream stringStream;
		stringStream << typeKey;

		unsigned long alarmTypeKey = 0;
		stringStream >> alarmTypeKey; 

		// Find the object in the map
		AlarmTypeMap::iterator it = m_alarmTypeMap.find(alarmTypeKey);

		//did we find it?
		if (it != m_alarmTypeMap.end())
		{
			// Return the object
			return it->second;
		}
		else
		{
			try
			{
				// load from the database
					TA_Base_Core::IAlarmTypeData* alarmTypeData = 
						TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
					typeKey, false);

				// Add to the map
				m_alarmTypeMap[alarmTypeKey] = alarmTypeData;
				
				return alarmTypeData;
			}
			catch ( const TA_Base_Core::DataException& /*ex */)
			{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					"DataException", "Failed to load AlarmTypeData" );
				return NULL;
			}
			catch ( const TA_Base_Core::DatabaseException& /*ex */)
			{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
				return NULL;
			}
		}

		return NULL;
	}

	TA_Base_Core::IAlarmTypeData* AlarmTypeTableAccessor::getAlarmTypeRecord(
		unsigned long alarmTypeKey)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		std::stringstream stringStream;
		stringStream << alarmTypeKey;

		std::string typeKey = "0";
		stringStream >> typeKey;

		// Find the object in the map
		AlarmTypeMap::iterator it = m_alarmTypeMap.find(alarmTypeKey);

		//did we find it?
		if (it != m_alarmTypeMap.end())
		{
			// Return the object
			return it->second;
		}
		else
		{
			try
			{
				// load from the database
					TA_Base_Core::IAlarmTypeData* alarmTypeData = 
						TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
					typeKey, false);

				// Add to the map
				m_alarmTypeMap[alarmTypeKey] = alarmTypeData;
				
				return alarmTypeData;
			}
			catch ( const TA_Base_Core::DataException& /*ex */)
			{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					"DataException", "Failed to load AlarmTypeData" );
				return NULL;
			}
			catch ( const TA_Base_Core::DatabaseException& /*ex */)
			{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
				return NULL;
			}
		}

		return NULL;
	}

	// Wenguang TD18266
	void AlarmTypeTableAccessor::loadAllAlarmTypes()
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);

		std::vector<TA_Base_Core::IAlarmTypeData*> allAlarmTypeData =
			TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypesAndAttributes();

		std::vector<TA_Base_Core::IAlarmTypeData*>::iterator itr;

		for( itr = allAlarmTypeData.begin(); itr != allAlarmTypeData.end(); itr++)
		{
			AlarmTypeMap::iterator it = m_alarmTypeMap.find((*itr)->getKey());
			if( it != m_alarmTypeMap.end())
			{
				// The alarm type with this key has already been in the map
				delete *itr;
				*itr = NULL;
			}
			else
			{
				m_alarmTypeMap[(*itr)->getKey()] = *itr;
			}
		}


	

	}
	
	std::string AlarmTypeTableAccessor::GetAlarmDescriptionModel(unsigned long messageTypeKey)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		
		// Load the message Type data
		TA_Base_Core::IAlarmTypeData* alarmTypeData =
			getAlarmTypeRecord( messageTypeKey );
		
		// Return default if null
		if (NULL == alarmTypeData)
		{
			return "Default";
		}
		
		try
		{
			// The description
			std::string description("");
			// Appended the AlarmType to the front for system alarms' description as required by 4669
			if ( alarmTypeData->isSystemAlarm() )
			{
				description = alarmTypeData->getName() + "- ";
				description += alarmTypeData->getDescription();
				//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Expanded Description: %s", description.c_str() );
			}
			else
			{
				description = alarmTypeData->getDescription();
			}
			
			return description;
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to get Alarm description Model" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		return "Default";
	}
}; // TA_Base_Core



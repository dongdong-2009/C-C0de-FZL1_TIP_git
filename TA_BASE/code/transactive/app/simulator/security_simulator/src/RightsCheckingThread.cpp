// RightsCheckingThread.cpp: implementation of the RightsCheckingThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "security_simulator.h"
#include "RightsCheckingThread.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/Action.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DataTypeEnums.h"

#include "core/exceptions/src/DataException.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RightsCheckingThread::RightsCheckingThread(IRightsChecker& rightsChecker, IResultObserver& resultObserver, 
										   std::string& sessionId, std::string location, 
										   unsigned long resourceKey, unsigned long actionKey, 
										   unsigned long profile)
										   : m_rightsChecker(rightsChecker),
										     m_resultObserver(resultObserver),
										     m_sessionId(sessionId),
										     m_location(location),
											 m_resourceKey(resourceKey),
											 m_actionKey(actionKey),
											 m_profile(profile),
											 m_isInitSuccess(false)

{
	try
	{
		TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(resourceKey);
		m_locationKey = entity->getLocation();
		m_subsystem = entity->getPhysicalSubsystem();

		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		std::stringstream sql;
        sql << "select SEAGRO_ID from se_action_group_map where SEACTI_ID = " << m_actionKey;		       
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
		IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
				
        do
        {
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long actionGroup = data->getUnsignedLongData(i, columnNames[0]);
					m_actionGroups.push_back(actionGroup);
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );



		m_isInitSuccess = true;
	}
	catch (...)
	{
	}	
}

RightsCheckingThread::~RightsCheckingThread()
{
	terminateAndWait();
}

void RightsCheckingThread::run()
{
	if(!m_isInitSuccess)
	{
		std::string result("RightsCheckingThread Init fail");
		m_resultObserver.notify(false, result);
	}
	else
	{
		m_isRunning = true;
		
		while (m_isRunning )
		{
			if( m_rightsChecker.checkRights(m_location, m_sessionId, m_locationKey, 
											m_subsystem, m_profile, m_resourceKey, 
											m_actionKey, m_actionGroups) )
			{
				std::stringstream result;
				bool rightLibResult;
				bool rightAgentResult;
				bool dutyResult;
				m_rightsChecker.getReslt(rightLibResult, rightAgentResult, dutyResult);
				
				result << " rightLibResult : " << (rightLibResult ? " true " : " false ") << "\n";
				result << " rightAgentResult : " << (rightAgentResult ? " true " : " false ") << "\n";
				result << " dutyResult : " << (dutyResult ? " true " : " false ") << "\n";
				
				m_resultObserver.notify(true, result.str());
			}
			else
			{
				std::stringstream result;
				bool rightLibResult;
				bool rightAgentResult;
				bool dutyResult;
				m_rightsChecker.getReslt(rightLibResult, rightAgentResult, dutyResult);
				
				result << " rightLibResult : " << (rightLibResult ? " true " : " false ") << "\n";
				result << " rightAgentResult : " << (rightAgentResult ? " true " : " false ") << "\n";
				result << " dutyResult : " << (dutyResult ? " true " : " false ") << "\n";

				m_resultObserver.notify(false, result.str());
				
			}

			sleep(10000);
		}
	}
}

void RightsCheckingThread::terminate()
{
	m_isRunning = false;
}
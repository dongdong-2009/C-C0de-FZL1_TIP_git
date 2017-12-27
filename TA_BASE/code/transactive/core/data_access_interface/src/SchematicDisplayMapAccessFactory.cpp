#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"
#include "core/data_access_interface/src/SchematicDisplayMaps.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	SchematicDisplayMapAccessFactory* SchematicDisplayMapAccessFactory::m_instance = NULL;
	NonReEntrantThreadLockable SchematicDisplayMapAccessFactory::m_singletonLock;

	SchematicDisplayMapAccessFactory& SchematicDisplayMapAccessFactory::getInstance()
	{
		TA_THREADGUARD(m_singletonLock);

		if (NULL == m_instance)
		{
			m_instance = new SchematicDisplayMapAccessFactory();
		}

		return *m_instance;
	}

	SchematicDisplayMapAccessFactory::SchematicDisplayMapAccessFactory()
	{

	}
	SchematicDisplayMapAccessFactory::~SchematicDisplayMapAccessFactory(void)
	{
	}

	std::string SchematicDisplayMapAccessFactory::getSchematicDisplay(unsigned long locationKey, unsigned long subSystemKey, long subTypeKey)
	{
		FUNCTION_ENTRY("getSchematicDisplay");

		TA_ASSERT(locationKey != 0, "Location must not be equal to zero");
		
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		const std::string STARTUP_DISPLAY_COL = "STARTUP_DISPLAY";

		std::vector<std::string> columnNames;
		columnNames.push_back(STARTUP_DISPLAY_COL);

		/*
		std::ostringstream sql;
		sql << "SELECT STARTUP_DISPLAY FROM SCHEMATIC_DISPLAY_MAP WHERE LOCATION_ID = '" << locationKey <<"'";
		sql << " AND SUBSYSTEM_ID = '" << subSystemKey << "'";
		if(subTypeKey >= 0)
		{
			sql << " AND SUBTYPE_ID = '" << subTypeKey << "'";
		}*/
		/*
		SELECT STARTUP_DISPLAY FROM SCHEMATIC_DISPLAY_MAP WHERE LOCATION_ID=%s AND SUBSYSTEM_ID =%s
		SELECT STARTUP_DISPLAY FROM SCHEMATIC_DISPLAY_MAP WHERE LOCATION_ID=%s AND SUBSYSTEM_ID =%s AND SUBTYPE_ID=%s
		*/
		TA_Base_Core::SQLStatement sql;
		if(subTypeKey >= 0)
		{
			databaseConnection->prepareSQLStatement(sql, SCHEMATIC_DISPLAY_MAP_SELECT_106002, locationKey, subSystemKey, subTypeKey);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, SCHEMATIC_DISPLAY_MAP_SELECT_106001, locationKey, subSystemKey);
		}

		//std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( sql.str(), columnNames ));
		std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( sql, columnNames ));
		if( NULL == data.get() || (0 == data->getNumRows()) ) 
		{
			// If there is no entry then this is an error as the key as accurate before
			std::ostringstream reasonMessage;
			reasonMessage << "No data found for location with locationKey = " << locationKey << " subSystemKey = " << subSystemKey;
			TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, "") );
		}

		FUNCTION_EXIT

		return data->getStringData(0, STARTUP_DISPLAY_COL);
	}

	std::string SchematicDisplayMapAccessFactory::getSchematicDisplay(unsigned long locationKey, unsigned long subSystemKey)
	{
		return getSchematicDisplay(locationKey, subSystemKey, -1);
	}

	int SchematicDisplayMapAccessFactory::getSchematicDisplayAndLabel(
		unsigned long locationKey, unsigned long subSystemKey,
		std::vector<ISchematicDisplayMaps*>& rVecOutput)
	{
		FUNCTION_ENTRY("getSchematicDisplayAndLabel");
		int nFunRes = 0;

		TA_ASSERT(locationKey != 0, "Location must not be equal to zero");

		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		const std::string SUBTYPE_ID_COL = "SUBTYPE_ID";
		const std::string STARTUP_DISPLAY_COL = "STARTUP_DISPLAY";
		const std::string SUBTYPE_LABEL_COL = "SUBTYPE_LABEL";

		std::vector<std::string> columnNames;
		columnNames.push_back(SUBTYPE_ID_COL);
		columnNames.push_back(STARTUP_DISPLAY_COL);
		columnNames.push_back(SUBTYPE_LABEL_COL);

		/*std::ostringstream sql;
		sql << "SELECT SUBTYPE_ID,STARTUP_DISPLAY,SUBTYPE_LABEL FROM SCHEMATIC_DISPLAY_MAP WHERE LOCATION_ID = '" << locationKey <<"'";
		sql << " AND SUBSYSTEM_ID = '" << subSystemKey << "'";*/
		/*SELECT SUBTYPE_ID,STARTUP_DISPLAY,SUBTYPE_LABEL FROM SCHEMATIC_DISPLAY_MAP WHERE LOCATION_ID=%s AND SUBSYSTEM_ID=%s*/
		//
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SCHEMATIC_DISPLAY_MAP_SELECT_106003, locationKey, subSystemKey);
		//TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
		TA_Base_Core::IData* data = databaseConnection->executeQuery( sql, columnNames );
		if( NULL == data || (0 == data->getNumRows()) ) 
		{
			// If there is no entry then this is an error as the key as accurate before
			std::ostringstream reasonMessage;
			reasonMessage << "No data found for location with locationKey = " << locationKey << " subSystemKey = " << subSystemKey;
			delete data;
			data = NULL;
			nFunRes = -1;
			TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, "") );
		}


		// Loop for each row returned
		do
		{
			for (unsigned long nIndex = 0; nIndex < data->getNumRows(); nIndex++)
			{
				// The getUnsignedLongData() call can throw an exception. Need to catch
				// it to do pointer clean up.
				try
				{
					// Create the ISchematicDisplayMaps
					int nSubTypeIDColValue = data->getIntegerData(nIndex, SUBTYPE_ID_COL);
					std::string strStartupDisplayColValue = data->getStringData(nIndex, STARTUP_DISPLAY_COL);
					std::string strSubTypeLableColValue = data->getStringData(nIndex, SUBTYPE_LABEL_COL);
					ISchematicDisplayMaps* pSchematicDisplayMaps = new SchematicDisplayMaps(
						nSubTypeIDColValue, strStartupDisplayColValue, strSubTypeLableColValue);

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
					"nIndex=%d, SUBTYPE_ID=%d, STARTUP_DISPLAY = %s, SUBTYPE_LABEL = %s",
						nIndex, nSubTypeIDColValue, strStartupDisplayColValue.c_str(), strSubTypeLableColValue.c_str());

					rVecOutput.push_back(pSchematicDisplayMaps);
				}
				catch (const TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					nFunRes = -1;
					throw;
				}
			}
			delete data;
			data = NULL;

		}while ( databaseConnection->moreData(data) );

		FUNCTION_EXIT
		return nFunRes;
	}

}
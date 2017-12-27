/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * DerivedDataPointAccessFactory is a singleton that is used to retrieve derived datapoint objects
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointConfigCache.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/threads/src/Thread.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::ReEntrantThreadLockable DerivedDataPointAccessFactory::s_singletonLock;
    DerivedDataPointAccessFactory* DerivedDataPointAccessFactory::s_instance = NULL;


    DerivedDataPointAccessFactory& DerivedDataPointAccessFactory::getInstance()
    {
    	if ( s_instance == NULL )
    	{
			TA_Base_Core::ThreadGuard guard( s_singletonLock );

			if ( s_instance == NULL )
			{
				s_instance = new DerivedDataPointAccessFactory();
			}
    	}

		return *s_instance;
    }


    DerivedDataPointAccessFactory::~DerivedDataPointAccessFactory()
    {

    }


    IConfigDerivedDataPoint* DerivedDataPointAccessFactory::getDerivedDataPoint(unsigned long pkey,
                                                                               bool isWritable /*= false*/)
    {
        FUNCTION_ENTRY("getDerivedDataPoint");
		
		if(!isValidEntityKey(pkey))
		{
	        FUNCTION_EXIT;
			return NULL;
		}
		
        if (isWritable)
        {
            IConfigDerivedDataPoint* dataPoint = new ConfigDerivedDataPoint(pkey,isWritable);
            FUNCTION_EXIT;
            return dataPoint;
        }
        else
        {
            //TA_ASSERT(false, "Still to be written");
            //TODO: Temporary patch for Gilles SDV platform testing
            //TD: To return a read only datapoint
            IConfigDerivedDataPoint* dataPoint = new ConfigDerivedDataPoint(pkey,isWritable);
            FUNCTION_EXIT;
            return dataPoint;
        }
    }



    IConfigDerivedDataPoint* DerivedDataPointAccessFactory::newDerivedDataPoint()
    {
        FUNCTION_ENTRY("newDerivedDataPoint");

        IConfigDerivedDataPoint* dataPoint = new ConfigDerivedDataPoint();

        FUNCTION_EXIT;
        return dataPoint;
    }


    IConfigDerivedDataPoint* DerivedDataPointAccessFactory::copyDerivedDataPoint(IConfigDerivedDataPoint* dpToCopy)
    {
        FUNCTION_ENTRY("copyDerivedDataPoint");

        ConfigDerivedDataPoint* castDataPoint = dynamic_cast<ConfigDerivedDataPoint*>(dpToCopy);

        // Perform a call to ensure the item is fully loaded before we copy it
        castDataPoint->getInputAssociations();

        IConfigDerivedDataPoint* dataPoint = new ConfigDerivedDataPoint( *castDataPoint );
        

        FUNCTION_EXIT;
        return dataPoint;
    }

	void DerivedDataPointAccessFactory::bulkLoadByAgentKey(unsigned long agentKey)
	{
		FUNCTION_ENTRY("bulkLoadByAgentKey");

		// load entities for checking
		try
        {
            // Build the list of required column names.			
            std::vector<std::string> columnNames;
            columnNames.push_back("PKEY");
			
            /*std::ostringstream sql;
            sql << "SELECT PKEY FROM ENTITY WHERE ADDRESS = 'VIRTUAL' AND DELETED != 1 AND AGENTKEY = ";
			sql << agentKey;*/
					
            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4506, agentKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4506, agentKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4506, agentKey);

			//LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, sql.str().c_str());
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
			
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());
				
                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {					
                        unsigned long lKey = data->getUnsignedLongData(i, "PKEY");
                        m_loadEntityKeyList.insert(lKey);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;
						
                        throw;
                    }
                }
				
                // We are now finished with this data set. Destroy it before asking for more.
				delete data;
				data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
        }
		
        DerivedDataPointConfigCache::getInstance().bulkLoadByAgentKey( agentKey );

        FUNCTION_EXIT;
	}

    void DerivedDataPointAccessFactory::bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey)
    {
		FUNCTION_ENTRY("bulkLoadByAgentKey");

		// load entities for checking
		try
        {
            // Build the list of required column names.			
            std::vector<std::string> columnNames;
            columnNames.push_back("PKEY");
			
           /* std::ostringstream sql;
            sql << "SELECT PKEY FROM ENTITY WHERE ADDRESS = 'VIRTUAL' AND DELETED != 1 AND PARENTKEY = ";
			sql << dataNodeKey;*/
					
            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4507,dataNodeKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4507,dataNodeKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4507,dataNodeKey);

			//LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, sql.str().c_str());
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
			
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());
				
                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {					
                        unsigned long lKey = data->getUnsignedLongData(i, "PKEY");
                        m_loadEntityKeyList.insert(lKey);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;
						
                        throw;
                    }
                }
				
                // We are now finished with this data set. Destroy it before asking for more.
				delete data;
				data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
        }

        DerivedDataPointConfigCache::getInstance().bulkLoadByEquipmentDataNodeKey( dataNodeKey );

        FUNCTION_EXIT;
    }

	bool DerivedDataPointAccessFactory::isValidEntityKey(unsigned long entityKey)
	{
		if(m_loadEntityKeyList.end() != m_loadEntityKeyList.find( entityKey ) )
		{
			return true;
		}

		// TOTO use other method to check validity
		// assume that all the entitykey passed in for getDerivedDataPoint is already valid and must be derived dp
		return true;
	}

/*        std::ostringstream sql;
        sql << "select e.pkey from entity e, entityparameter ep, entityparametervalue epv where e.pkey = ";
        sql << entityKey << "and ep.name = 'DataPointType' and ep.pkey = epv.parameterkey and epv.entitykey = ";
        sql << entityKey;
		
		// get handle to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
		
		// set up columnNames vector to be passed to executeQuery()
		std::vector < std::string > columnNames;
		columnNames.push_back ( "pkey" );
		
		// execute the query. The method can throw a DatabaseException.
        IData* data = NULL;
        try
        {
			IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
			
            if (data->getNumRows() == 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This datapoint is not a derived datapoint so returning NULL");
                delete data;
                data = NULL;
				
                return false;
            }
        }
        catch ( ... )
        {
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
            throw;
        }

		return true;
	}*/
} //end namespace TA_Base_Core

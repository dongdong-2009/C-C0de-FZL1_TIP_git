#if !defined(DataPointStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define DataPointStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * DataPointStore.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointStore.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements IItemStore and keeps a store of data point items.
  *
  */

#pragma warning(disable: 4355)  // 'this' : used in base member initializer list

#include <algorithm>

#include "app/trending/trend_viewer/src/DataPointStore.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/ILoaderObserver.h"
#include "app/trending/trend_viewer/src/ItemLoaderThread.h"
#include "app/trending/trend_viewer/src/DataPointUpdateSubscriber.h"
#include "app/trending/trend_viewer/src/Globals.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/IDatabase.h"

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/database/src/DBException.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/ScadaProxyException.h"

#include "core/threads/src/Thread.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/common_library/src/DpValue.h"


using TA_Base_Core::DebugUtil;
    
namespace TA_Base_App
{

    const short  MAX_NUM_BOOLS = 10;    // Number of booleans to stack
    const double BOOL_OFFSET = 0.5;        // Offset from 0 in percentage units
    
    const char* DataPointStore::DRLP_DATA_POINT_IDENTIFIER = "DATAPOINT";


    //
    // DataPointStore
    //
    DataPointStore::DataPointStore(ILoaderObserver* loaderObserver)
        : m_itemLoaderThread(new ItemLoaderThread(loaderObserver, this)),
          m_boolCount(0),
          m_isTerminating(false)
    {
        FUNCTION_ENTRY("DataPointStore");

        TA_ASSERT(m_itemLoaderThread.get() != NULL, "ItemLoaderThread is NULL");
        
        TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();
        
        FUNCTION_EXIT;
    }


    //
    // ~DataPointStore
    //
    DataPointStore::~DataPointStore()
    {
        FUNCTION_ENTRY("~DataPointStore");

        m_isTerminating = true;

        if(m_itemLoaderThread.get() != NULL)
        {
            m_itemLoaderThread->terminateAndWait();
        }

		// TD13724 - proper clean up is needed for the following objects 
		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator it;
		for(it=m_dataPointProxyMap.begin(); it!=m_dataPointProxyMap.end(); it++)
		{
			TA_Base_Bus::DataPointProxySmartPtr * dp = it->second;
			delete dp;
		}

		m_dataPointProxyMap.clear();          
		//TA_Base_Bus::ScadaProxyFactory::getInstance().removeInstance();
		TA_Base_Bus::ScadaProxyFactory::removeInstance(); 
		// TD13724 - lizettejl
        
        FUNCTION_EXIT;
    }


    //
    // loadItems
    //
    void DataPointStore::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        m_itemLoaderThread->start();

        FUNCTION_EXIT;
    }


    //
    // getAllItems
    //
    std::vector<std::string> DataPointStore::getAllItems()
    {
        FUNCTION_ENTRY("getAllItems");

        TA_Base_Core::ThreadGuard guard(m_lock);

        std::vector<std::string> items;

        std::map<std::string, boost::shared_ptr<GenericItem> >::iterator it = m_dataPoints.begin();
        for(; it != m_dataPoints.end(); ++it)
        {
            items.push_back(it->first);
        }

        FUNCTION_EXIT;
        return items;
    }


    //
    // getItem
    //
    GenericItem* DataPointStore::getItem(const std::string& itemName)
    {
        FUNCTION_ENTRY("getItem");

        TA_Base_Core::ThreadGuard guard(m_lock);

        std::map<std::string, boost::shared_ptr<GenericItem> >::iterator it = m_dataPoints.find(itemName);

        if(it == m_dataPoints.end())
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::ITEM_NOT_FOUND, itemName.c_str()));
        }

        FUNCTION_EXIT;
        return (it->second).get();
    }
    

    //
    // loadParameterItems
    //
    void DataPointStore::loadItems(std::vector<std::string> itemNames)
    {
        FUNCTION_ENTRY("loadItems");

		//TD17812 - if itemNames is empty, then there's no item to load
		if (itemNames.empty())
		{
			return;
		}
		//TD17812

	    std::string dataPointNames("");
        std::vector<std::string>::const_iterator iter = itemNames.begin();
        for(; iter != itemNames.end(); ++iter)
        {
            if(!dataPointNames.empty())
            {
                dataPointNames += ", ";
            }

            dataPointNames += "'";
            dataPointNames += *iter;
            dataPointNames += "'";
        }

        //std::string sql("SELECT pkey, name, engineeringlimitlow, engineeringlimithigh, datapointtype, unitlabel, \
        //     agentname, locationkey, precision FROM DATAPOINT_EV WHERE name IN (");
        //sql += dataPointNames;
        //sql += ")";

        std::vector<std::string> columnNames;
        columnNames.push_back("EntityKey");
        columnNames.push_back("EntityName");
        columnNames.push_back("EngineeringLimitLow");
        columnNames.push_back("EngineeringLimitHigh");
        columnNames.push_back("DataPointType");
        columnNames.push_back("UnitLabel");
        columnNames.push_back("AgentName");
        columnNames.push_back("LocationKey");
        columnNames.push_back("Precision");

        // Get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);
        //TA_Base_Core::IData* data = getData(databaseConnection, sql, columnNames);
		TA_Base_Core::SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, DATAPOINT_EV_SELECT_40002, dataPointNames);   
		
        TA_Base_Core::IData* data = getData(databaseConnection, strSql, columnNames);
		
		//TD17662++
		if (data == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error when retrieving databasedata. An exception should be thrown already.");
			FUNCTION_EXIT;
			return;
		}
		//++TD17662
		
		std::ostringstream dbError;
		std::ostringstream engUnitError;
		std::string entityName("");
		std::vector<std::string> loadedItems;
		
		do
		{
			for(unsigned int i = 0; i < data->getNumRows(); ++i)
			{
				double engineeringLimitLow = 0;
				double engineeringLimitHigh = 0;
				unsigned long entityKey = 0;
				std::string dataPointType("");
				std::string label("");
				std::string agentName("");
				unsigned long locationKey = 0;
				int precision = 0;
				
				try
				{
					dataPointType = data->getStringData(i, "DataPointType");
					entityKey = data->getUnsignedLongData(i, "EntityKey");
					entityName = data->getStringData(i, "EntityName");
					label = data->getStringData(i, "UnitLabel");
					//TD16558++ - must display the correct label for temperature
					CString degreeString = label.c_str();
					if (degreeString.Find("?C", 0) >= 0)
					{
//						degreeString.Replace(_T('?'), _T('?));
						label = degreeString;
					}
					//++TD16558 (lizettejl)
					
					agentName = data->getStringData(i, "AgentName");
					locationKey = data->getUnsignedLongData(i, "LocationKey");
                                        //TD17883 - set precision correctly, must have it's ownn handling for exceptions
					//precision =atoi(data->getStringData(i, "Precision").c_str());
				}
				catch(TA_Base_Core::DatabaseException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
					dbError << entityName << "\n";
					continue;
				}
				catch(TA_Base_Core::DataException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", ex.what());
					dbError << entityName << "\n";
					continue;
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
					dbError << entityName << "\n";
					continue;
				}
                                //TD17883 - set precision correctly, must have it's ownn handling for exceptions
				try
				{
			 		precision =atoi(data->getStringData(i, "Precision").c_str());
				}
				catch(...)
				{
				   precision = 0; 
				}
								
				// The EngineeringLimit parameters are put in a separate try/catch so we can distinguish
				// whether the error is caused by these. These parameters are essential in scaling the
				// plots, so if these parameters don't have values, the trend cannot be plotted.
				try
				{
					// If the datapoint type is boolean, then engineering limits will not have been 
					// retrieved from the database. Set these to 0 and 100.
					
					if(dataPointType == DP_BOOLEAN_TYPE)
					{
						engineeringLimitLow = 0;
						engineeringLimitHigh = 100;
					}
					//TD17605++
					else if (dataPointType == DP_DERIVED_TYPE)
					{
						 
						engineeringLimitLow = 0.0;
						engineeringLimitHigh = 3.0; // assume that most derived datapoints have 4 states
												  // just reset value if the given datapoint is added to
						                          // the graph once a refernce to the datapoint proxy is
												  // retrieved				
					}
					//++TD17605
					else
					{
						engineeringLimitLow = data->getRealData(i, "EngineeringLimitLow");
						engineeringLimitHigh = data->getRealData(i, "EngineeringLimitHigh");
					}
					
					processAndStoreItem(entityKey, entityName, dataPointType, engineeringLimitLow, engineeringLimitHigh, label, precision, agentName, locationKey);
					loadedItems.push_back(entityName);
				}
				catch(TA_Base_Core::DatabaseException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
					
					engUnitError << entityName << "\n";
				}
				catch(TA_Base_Core::DataException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", ex.what());
					
					engUnitError << entityName << "\n";
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
					
					engUnitError << entityName << "\n";
				}
			}
			
			delete data; //TD17884
			data = NULL;
			
			TA_Base_Core::Thread::sleep(10);
			
		} while(databaseConnection->moreData(data));
		
		
		
		if(!engUnitError.str().empty())
		{
			std::ostringstream msg;
			msg << "Engineering Limit parameter values have not been set for the following Data Points: \n\n";
			msg << engUnitError.str();
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s", msg.str().c_str());
		}
		
		if(!dbError.str().empty())
		{
			std::ostringstream msg;
			msg << "Errors occured while loading the following Data Points: \n\n";
			msg << dbError.str();
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s", msg.str().c_str());
		}
		
		if(!engUnitError.str().empty() || !dbError.str().empty())
		{
			TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, 
				"some datapoints have not been properly configured"));
		}
		
		//itemNames = loadedItems;
		
        
        FUNCTION_EXIT;
    }


    //
    // retrieveFromDatabase
    //
    void DataPointStore::retrieveFromDatabase()
    {
        FUNCTION_ENTRY("retrieveFromDatabase");

        //TA_Base_Core::ThreadGuard guard(m_lock);

        // If a new type is added that does not have the engineering limit low and high parameters,
        // it needs to be added here
		//TD17605 - reimplemented this code snippet here. no need to declare an array of string
		// and just concatenate it right after. simply define the string with the required datapoint types
		// that have no engineering limit parameters
		std::string typesWithoutLimitsExceptBoolean("'Text', 'CombinedOutput'"); 
		//++TD17605 
   

        /*std::string sql("SELECT pkey, name, engineeringlimitlow, engineeringlimithigh, datapointtype, unitlabel, \
            '""' AS falselabel, '""' AS truelabel FROM DATAPOINT_EV WHERE datapointtype NOT IN (");
        sql += typesWithoutLimits;
        sql += ") and deleted <> 1 UNION ";
        sql += "SELECT pkey, name, '0' AS engineeringlimitlow, '100' AS engineeringlimithigh, datapointtype, ";
        sql += "unitlabel, falselabel, truelabel FROM DATAPOINT_EV WHERE datapointtype IN (";
        sql += "'Boolean'";
        sql += ") and deleted <> 1 order by name";
		*/
		
		//delete by lin
		// optimised SQL - liborm - 10/01/05		
        //std::ostringstream sql;
        //sql << "SELECT pkey, name, DECODE(datapointtype,'Boolean','0',engineeringlimitlow) AS engineeringlimitlow, ";
        //sql << "                   DECODE(datapointtype,'Boolean','100',engineeringlimithigh) AS engineeringlimithigh, ";
        //sql << "                   datapointtype, unitlabel, agentname, locationkey, precision, enabletrending ";
        //sql << "FROM DATAPOINT_EV ";
        //sql << "WHERE datapointtype NOT IN (" ;
        //sql << typesWithoutLimitsExceptBoolean;
        //sql << " ) AND deleted <> 1 order by name";
        //

        std::vector<std::string> columnNames;
        columnNames.push_back("EntityKey");
        columnNames.push_back("EntityName");
        columnNames.push_back("EngineeringLimitLow");
        columnNames.push_back("EngineeringLimitHigh");
        columnNames.push_back("DataPointType");
        columnNames.push_back("UnitLabel");
        columnNames.push_back("AgentName");
        columnNames.push_back("LocationKey");
        columnNames.push_back("Precision");
		columnNames.push_back("EnableTrending"); //TD17767

        // Get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);
        //TA_Base_Core::IData* data = getData(databaseConnection, sql.str(), columnNames);
        
        

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DATAPOINT_EV_SELECT_40003, 
			typesWithoutLimitsExceptBoolean);
        TA_Base_Core::IData* data = getData(databaseConnection, strSql, columnNames);
        
        
        
        std::ostringstream dbError;
        std::ostringstream engUnitError;
        std::string entityName("");
        std::vector<std::string> dbErrorNames;
        std::vector<std::string> engErrorNames;
            
        do
        {
            for(unsigned int i = 0; i < data->getNumRows() && !m_isTerminating; ++i)
            {
                double engineeringLimitLow = 0;
                double engineeringLimitHigh = 0;
                unsigned long entityKey = 0;
                std::string dataPointType("");
                std::string label("");
                std::string agentName("");
                unsigned long locationKey = 0;
                int precision = 0;

                try
                {
					//TD17767
					std::string enableTrending = data->getStringData(i,"EnableTrending");
					if (enableTrending == "0")
					{
						continue;
					}

                    dataPointType = data->getStringData(i, "DataPointType");
                    
					//TD17605 - DP_TEXT_TYPE should not be returned from the query
                    // Can't trend Text data points, so don't store them
                    //if(dataPointType == DP_TEXT_TYPE)
                    //{
                    //    continue;
                    //}
                    //++TD17605

                    entityKey = data->getUnsignedLongData(i, "EntityKey");
                    entityName = data->getStringData(i, "EntityName");
                    label = data->getStringData(i, "UnitLabel");
					//TD16558++ - must display the correct label for temperature
					CString degreeString = label.c_str();
					if (degreeString.Find("?C", 0) >= 0)
					{
//						degreeString.Replace(_T('?'), _T('?));
						label = degreeString;
					}
					//++TD16558 (lizettejl)
                    agentName = data->getStringData(i, "AgentName");
                    locationKey = data->getUnsignedLongData(i, "LocationKey");
                     //TD17883 - set precision correctly, must have it's ownn handling for exceptions
                    //precision = atoi(data->getStringData(i, "Precision").c_str());
                }
                catch(TA_Base_Core::DatabaseException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
                    dbErrorNames.push_back(entityName);
                    continue;
                }
                catch(TA_Base_Core::DataException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", ex.what());
                    dbErrorNames.push_back(entityName);
                    continue;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
                    dbErrorNames.push_back(entityName);
                    continue;
                }

                                //TD17883 - set precision correctly, must have it's ownn handling for exceptions
				try
				{
			 		precision =atoi(data->getStringData(i, "Precision").c_str());
				}
				catch(...)
				{
				   precision = 0; 
				}

                // The EngineeringLimit parameters are put in a separate try/catch so we can distinguish
                // whether the error is caused by these. These parameters are essential in scaling the
                // plots, so if these parameters don't have values, the trend cannot be plotted.
                try
                {
					//TD17605 - set default engineering limits for datapoints of boolean type		
					if(dataPointType == DP_BOOLEAN_TYPE)
					{
						engineeringLimitLow = 0;
						engineeringLimitHigh = 100;
					}
					//TD17605++
					else if (dataPointType == DP_DERIVED_TYPE)
					{
						engineeringLimitLow = 0.0;
						engineeringLimitHigh = 3.0; // assume that most derived datapoints have 4 states
												  // just reset value if the given datapoint is added to
						                          // the graph once a refernce to the datapoint proxy is
												  // retrieved
					}
					//++TD17605
					else
					{
						engineeringLimitLow = data->getRealData(i, "EngineeringLimitLow");
						engineeringLimitHigh = data->getRealData(i, "EngineeringLimitHigh");
					}

                    processAndStoreItem(entityKey, entityName, dataPointType, engineeringLimitLow, 
                        engineeringLimitHigh, label, precision, agentName, locationKey);
                }
                catch(TA_Base_Core::DatabaseException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
                    engErrorNames.push_back(entityName);
                    continue;
                }
                catch(TA_Base_Core::DataException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", ex.what());
                    engErrorNames.push_back(entityName);
                    continue;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
                    engErrorNames.push_back(entityName);
                    continue;
                }
            }
            
			delete data; //TD17884
            data = NULL;

            TA_Base_Core::Thread::sleep(10);

        } while(!m_isTerminating && databaseConnection->moreData(data));
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loaded %d datapoints.", m_dataPoints.size());
        
        if(!engErrorNames.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Engineering Limit parameter values have not been set for the following Data Points:");
                
            std::vector<std::string>::iterator it = engErrorNames.begin();
            for(; it != engErrorNames.end(); ++it)
            {
                LOGMORE(SourceInfo, "%s", (*it).c_str());
            }
        }
        
        if(!dbErrorNames.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Errors occured while loading the following Data Points:");
                
            std::vector<std::string>::iterator it = dbErrorNames.begin();
            for(; it != dbErrorNames.end(); ++it)
            {
                LOGMORE(SourceInfo, "%s", (*it).c_str());
            }
        }

        if(!engErrorNames.empty() || !dbErrorNames.empty())
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, 
                "some datapoints have not been properly configured"));
        }

        FUNCTION_EXIT;
    }
    

    //
    // processAndStoreItem
    //
    void DataPointStore::processAndStoreItem(unsigned long entityKey, const std::string& entityName, const std::string& dataPointType,
        double engineeringLimitLow, double engineeringLimitHigh, const std::string& unitLabel, int precision, const std::string& agentName, unsigned long locationKey)
    {
        if(m_isTerminating)
        {
            return;
        }

        bool isBooleanType = false;
        if(dataPointType == DP_BOOLEAN_TYPE)// || dataPointType == DP_DERIVED_TYPE || dataPointType == DP_ENUM_TYPE)
        {
            //LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Boolean data point.");

            isBooleanType = true;
        
            // Make the range slightly greater so that lines at 0 and 1 don't
            // disappear into the graph borders.
            
            // PercentSize is the size in percentage unit
            double percentSize = 100.0 / static_cast <double> (MAX_NUM_BOOLS);

            const double MIN_PERCENT_SIZE = 2.0;

            // The offset is the offset from the line as it needs to be off the line to
            // avoid overlap.  The minimum size is 2 percent

            // AL 22/02/05: Commented out the following line and just set the newOffset to a constant because
            // when there are a large number of boolean datapoints loaded, the offset becomes a very large 
            // number and many of the boolean datapoints are plotted above the chart area. By setting it as
            // a constant, every 10th boolean data point will be plotted on top of each other, if they are
            // all added to the graph. However, they can still be seen by toggling the top trend on/off. Leave
            // like this until there is a better solution.
            //double newOffset = std::min(BOOL_OFFSET + BOOL_OFFSET * static_cast <int> (m_boolCount / MAX_NUM_BOOLS), (percentSize - MIN_PERCENT_SIZE) / 2.0);
            double newOffset = BOOL_OFFSET;

            // Given the size and offset, calculate the new range
            double desiredRange = percentSize - MIN_PERCENT_SIZE * newOffset;

            // The resulting scale factor
            double scaleFactor = 100 / desiredRange;

            // Calculate the engineering low
            engineeringLimitLow = -(static_cast <double> (m_boolCount % MAX_NUM_BOOLS) * percentSize + newOffset) / desiredRange;

            // Calculate the engineering high
            engineeringLimitHigh = engineeringLimitLow + scaleFactor;

            // Increment the counter
            ++m_boolCount;
        }

        // Subscriber is created here, but is deleted in the GenericItem destructor
        std::auto_ptr<IUpdateSubscriber> updateSubscriber(new DataPointUpdateSubscriber(entityKey));

        // Retrieve the type identifier - this is used to determine the table name
        std::string typeIdentifier = retrieveTypeIdentifier();

        // Create new generic item and store
        boost::shared_ptr<GenericItem> genericItem(new GenericItem(entityKey, entityName, typeIdentifier, 
            engineeringLimitLow, engineeringLimitHigh, unitLabel, isBooleanType, agentName, locationKey, 
            precision, updateSubscriber));


        // Set the state labels depending on the data point type
        if(dataPointType == DP_BOOLEAN_TYPE || dataPointType == DP_ENUM_TYPE || dataPointType == DP_DERIVED_TYPE)
        {
            genericItem->setShouldDisplayLabel(true);
        }

        m_lock.acquire();

        if(m_dataPoints.find(entityName) != m_dataPoints.end())
        {
            // Data point already exists
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Datapoint %s already added.", entityName.c_str());

            m_lock.release();
            return;
        }

        m_dataPoints[entityName] = genericItem;

        m_lock.release();
    }


    //
    // retrieveTypeIdentifier
    //
    std::string DataPointStore::retrieveTypeIdentifier()
    {
        FUNCTION_ENTRY("retrieveTypeIdentifier");

        return DRLP_DATA_POINT_IDENTIFIER;
    }


    //
    // getData
    //
    TA_Base_Core::IData* DataPointStore::getData(TA_Base_Core::IDatabase* databaseConnection, const TA_Base_Core::SQLStatement& query, std::vector<std::string> columnNames)
    {
        TA_Base_Core::IData* data = NULL;
        try
        {
            data = databaseConnection->executeQuery(query,columnNames);
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());

            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, "Unknown error occured while retrieving data points."));
        }

        return data;
    }


    //
    // setStateLabels
    //
    void DataPointStore::setStateLabels(IGenericItem* genericItem)
    {
        // Create corba name that is needed to create a data point proxy
        TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = new TA_Base_Bus::DataPointProxySmartPtr;
		TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(genericItem->getEntityKey(), *this, *dataPointProxy);
        m_dataPointProxyMap[genericItem->getEntityKey()] = dataPointProxy;

		// TD13724 - remove a reference for the ScadaProxyFactory after use 
		TA_Base_Bus::ScadaProxyFactory::removeInstance(); //lizettejl
    }   


    void DataPointStore::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        FUNCTION_ENTRY("processEntityUpdateEvent");

        std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator dppIter = m_dataPointProxyMap.find(entityKey);
        if(dppIter != m_dataPointProxyMap.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Retrieving label information for entity: %lu", entityKey);

            TA_Base_Bus::DataPointProxySmartPtr& dataPointProxy = *(dppIter->second);

            std::string dataPointName = dataPointProxy->getEntityName();

            boost::shared_ptr<GenericItem> genericItem = m_dataPoints[dataPointName];
            if(genericItem->getLabelsRetrieved())
            {
                FUNCTION_EXIT;
                return;
            }
            
            // Go through the data point proxy to retrieve the labels
			TA_Base_Bus::DpvEnumLabelsMap stateLabels;
            try
            {
        
                if(dataPointProxy->isBoolean())
                {
                    std::string trueLabel = dataPointProxy->getBooleanLabel(true);
                    std::string falseLabel = dataPointProxy->getBooleanLabel(false);
                    
                    stateLabels[0] = falseLabel;
                    stateLabels[1] = trueLabel;
                }
                else
                {
                    stateLabels = dataPointProxy->getDerivedStatesAsMap();
                }
            }
            catch(const TA_Base_Core::ScadaProxyException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", ex.what());

                // Can't get the labels so don't display them
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot retrieve labels for datapoint: %lu", entityKey);
                return;
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                // Can't get the labels so don't display them
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot retrieve labels for datapoint: %lu", entityKey);
                return;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Labels saved:");

            TA_Base_Bus::DpvEnumLabelsMap::iterator iter = stateLabels.begin();
            for(; iter != stateLabels.end(); ++iter)
            {
                int state = iter->first;
                std::string label = iter->second;
                genericItem->setStateLabel(state, label);
                LOGMORE(SourceInfo, "State: %d, Label: %s", state, label.c_str());   
            }

            genericItem->setLabelsRetrieved(true);
        }
        
        FUNCTION_EXIT;
    }
}

#endif // !defined(DataPointStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/NamedFilterAccessFactory.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * NamedFilterAccessFactory is a singleton that is used to access named filters saved by an operator
  * for a specified application. It also provides an interface to allow static filters to be registered
  * which apply for all users. These static filters can be made to change their state depending on the
  * current state of the running application.
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/NamedFilter.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;

namespace
{
    // Columns in the NAMED_FILTER table.
    const std::string COLUMN_FILTER_NAME( "FILTER_NAME" );
    const std::string COLUMN_LIST_NAME( "LIST_NAME" );
    const std::string COLUMN_LIST_TYPE( "LIST_TYPE" );
    const std::string COLUMN_LIST_ITEM( "LIST_ITEM" );
}

namespace TA_Base_Core
{
    NamedFilterAccessFactory* NamedFilterAccessFactory::m_instance = NULL;

    NamedFilterAccessFactory& NamedFilterAccessFactory::getInstance()
    {
        // Create a new instance if it doesn't exist.
        if ( 0 == m_instance )
        {
            m_instance = new NamedFilterAccessFactory();
        }

        return *m_instance;
    }

    NamedFilterAccessFactory::NamedFilterAccessFactory()
    {
    }

    NamedFilterAccessFactory::~NamedFilterAccessFactory()
    {
        TA_Base_Core::ThreadGuard guard( m_staticFiltersLock );

        // Delete all static filters.
        StaticFilterMap::iterator it;
        for ( it=m_staticFilters.begin(); it!=m_staticFilters.end(); it++ )
        {
            delete it->second;
            it->second = NULL;
        }
        m_staticFilters.clear();
    }
    
    void NamedFilterAccessFactory::registerStaticNamedFilter( std::auto_ptr<NamedFilter> namedFilter )
    {
        TA_ASSERT( NULL != namedFilter.get(), "Cannot register a NULL named filter." );
        
        TA_Base_Core::ThreadGuard guard( m_staticFiltersLock );

        StaticFilterMap::iterator it = m_staticFilters.find( namedFilter->getName() );
        if ( it != m_staticFilters.end() )
        {
            // There was already a filter with this name, so delete the old one.
            delete it->second;
            it->second = NULL;
        }

        // Add the filter to the map.
        m_staticFilters[namedFilter->getName()] = namedFilter.release();
    }

    NamedFilterAccessFactory::FilterNameList NamedFilterAccessFactory::getAllFilterNames()
    {
        // Get a connection to the database.
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(AlarmsLocal_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), named_filter_STD_SELECT_17501,
//		std::string strSql  = databaseConnection->prepareSQLStatement(NAMED_FILTER_SELECT_17501,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, NAMED_FILTER_SELECT_17501,
			getApplicationKey(), databaseConnection->escapeQueryString(getSessionId()));
//		TA_ASSERT(strSql.size() > 0, "NamedFilterAccessFactory::getAllFilterNames(): Prepare SQL Statement error");

        // Construct the query.
        /*std::ostringstream sql;
        sql << "SELECT UNIQUE filter_name FROM named_filter WHERE applicationkey=" << getApplicationKey()
            << " AND operatorkey=( SELECT operatorkey FROM ta_session WHERE pkey='" 
            << databaseConnection->escapeQueryString(getSessionId()) << "' )";*/

        // Now create the columns we want to retrieve.
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_FILTER_NAME );

        // Execute the query.
        std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

        // Extract the results.
        FilterNameList filterNames;
        for ( unsigned long i=0; i<data->getNumRows(); i++ )
        {
            filterNames.push_back( FilterName( data->getStringData( i, COLUMN_FILTER_NAME ), true ) );
        }

        // Now add values from the static filters.
        TA_Base_Core::ThreadGuard guard( m_staticFiltersLock );

        StaticFilterMap::iterator it;
        for ( it=m_staticFilters.begin(); it!=m_staticFilters.end(); it++ )
        {
            filterNames.push_back( FilterName( it->second->getName(), false ) );
        }

        return filterNames;
    }

    std::auto_ptr<NamedFilter> NamedFilterAccessFactory::getNamedFilter( const std::string& filterName )
    {
        {
            // Check the static filters first.
            TA_Base_Core::ThreadGuard guard( m_staticFiltersLock );

            StaticFilterMap::iterator it = m_staticFilters.find( filterName );
            if ( it != m_staticFilters.end() )
            {
                // Its a static filter.
                return std::auto_ptr<NamedFilter>( it->second->clone() );
            }
        }

        // It wasn't a static filter so query the database.

        // Get a connection to the database.
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(AlarmsLocal_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), named_filter_STD_SELECT_17502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(NAMED_FILTER_SELECT_17502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, NAMED_FILTER_SELECT_17502,
			getApplicationKey(),databaseConnection->escapeQueryString(getSessionId()),databaseConnection->escapeQueryString(filterName));
//		TA_ASSERT(strSql.size() > 0, "NamedFilterAccessFactory::getNamedFilter(): Prepare SQL Statement error");

        // Construct the query.
        /*std::ostringstream sql;
        sql << "SELECT list_name, list_type, list_item FROM named_filter WHERE applicationkey=" << getApplicationKey()
            << " AND operatorkey=( SELECT operatorkey FROM ta_session WHERE pkey='" 
            << databaseConnection->escapeQueryString(getSessionId()) << "' )"
            << " AND filter_name='" << databaseConnection->escapeQueryString(filterName) << "'";*/

        // Now create the columns we want to retrieve.
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_LIST_NAME );
        columnNames.push_back( COLUMN_LIST_TYPE );
        columnNames.push_back( COLUMN_LIST_ITEM );

        // Execute the query.
        std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
        IData* dataPtr = data.get();

        // We should have at least one row.
        if ( NULL == dataPtr || 0 == data->getNumRows() )
        {
            throw DataException( "The specified filter could not be found.", DataException::NO_VALUE,
                                 getSessionId()+"::"+filterName );
        }

        // Now the create the NamedFilter.
        std::auto_ptr<NamedFilter> newFilter( new NamedFilter() );
        do
        {
            // Make sure the new data object is cleaned up when required.
            if ( dataPtr != data.get() )
            {
                data = std::auto_ptr<IData>( dataPtr );
            }

            // Iterate through all of the rows returned.
            for ( unsigned long i=0; i<data->getNumRows(); i++ )
            {
                std::string listName = data->getStringData( i, COLUMN_LIST_NAME );
                unsigned long listType = data->getUnsignedLongData( i, COLUMN_LIST_TYPE );
            
                // SimpleDb uses strings internally, so we can get it to convert our numeric
                // values for us.
                if ( NamedFilter::LIST_TYPE_NUMERIC == listType )
                {
                    newFilter->addNumericFilter( listName, data->getUnsignedLongData( i, COLUMN_LIST_ITEM ) );
                }
                else
                {
                    newFilter->addStringFilter( listName, data->getStringData( i, COLUMN_LIST_ITEM ) );
                }
            }

            dataPtr = NULL;
        } while ( databaseConnection->moreData(dataPtr) );

        // The filter has been created, now it just needs to be named.
        newFilter->setName( filterName );

        // Release the new filter object to the caller.
        return newFilter;
    }

    unsigned long NamedFilterAccessFactory::getApplicationKey()
    {
        // Convert the type to a number using a string stream.
        unsigned long applicationKey;
        std::stringstream appType( TA_Base_Core::RunParams::getInstance().get( RPARAM_APPTYPE ) );
        appType >> applicationKey;

        return applicationKey;
    }

    std::string NamedFilterAccessFactory::getSessionId()
    {
        return TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ); 
    }
};


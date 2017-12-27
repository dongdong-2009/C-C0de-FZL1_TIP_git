/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/NamedFilter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 16:39:13 $
  * Last modified by:  $Author: shurui.liu $
  *
  * NamedFilter is a container class used to store filterable information under a
  * specific name. It contains no database access code, instead it relies on the
  * NamedFilterAccessFactory for all loading, saving, and deleting.
  */

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/NamedFilter.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;
typedef TA_Base_Core::NamedFilterAccessFactory::FilterNameList FilterNameList;
//using TA_Base_Core::NamedFilterAccessFactory::FilterNameList;

namespace TA_Base_Core
{
    
    const unsigned long NamedFilter::LIST_TYPE_STRING(0);
    const unsigned long NamedFilter::LIST_TYPE_NUMERIC(1);
     const std::string NamedFilter::UNNAMED( "自定义过滤器" );

    NamedFilter::NamedFilter()
    {
        clear();
    }


    NamedFilter::NamedFilter( const NamedFilter& param )
    {
        m_filterName = param.m_filterName;
        m_stringFilters = param.m_stringFilters;
        m_numericFilters = param.m_numericFilters;
    }


    NamedFilter::~NamedFilter()
    {
    }


    NamedFilter& NamedFilter::operator=( const NamedFilter& rhs )
    {
        m_filterName = rhs.m_filterName;
        m_stringFilters = rhs.m_stringFilters;
        m_numericFilters = rhs.m_numericFilters;

        return *this;
    }


    NamedFilter* NamedFilter::clone()
    {
        // Create a new NamedFilter.
        std::auto_ptr<NamedFilter> newFilter( new NamedFilter() );

        // Copy the contents into the new filter.
        newFilter->m_filterName = m_filterName;
        newFilter->m_stringFilters = m_stringFilters;
        newFilter->m_numericFilters = m_numericFilters;

        // Release the new filter to the caller.
        return newFilter.release();
    }


    void NamedFilter::clear()
    {
        // Reset the filter to its original state.
        m_filterName = UNNAMED;
        m_stringFilters.clear();
        m_numericFilters.clear();
    }


    const std::string& NamedFilter::getName() const
    {
        return m_filterName;
    }


    void NamedFilter::setName( const std::string& filterName )
    {
        m_filterName = filterName;
    }


    NamedFilter::StringList NamedFilter::getStringFilterList() const
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        StringList stringFilters;
        
        // Iterate through the string map and pull out all the names.
        StringListMap::const_iterator it;
        for ( it=m_stringFilters.begin(); it!=m_stringFilters.end(); it++ )
        {
            stringFilters.push_back( it->first );
        }

        return stringFilters;
    }

    
    NamedFilter::StringList NamedFilter::getStringFilter( const std::string& listName ) const
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        // Find the requested list.
        StringListMap::const_iterator it = m_stringFilters.find( listName );
        if ( it != m_stringFilters.end() )
        {
            return it->second;
        }

        // If no list was found return an empty one.
        StringList emptyList;
        return emptyList;
    }


    void NamedFilter::setStringFilter( const std::string& listName, const StringList& stringList )
    {
        // Remove the existing filter.
        removeStringFilter( listName );

        // If the new list isn't empty, add it.
        if ( !stringList.empty() )
        {
            m_stringFilters.insert( StringListMap::value_type(listName,stringList) );
        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    void NamedFilter::addStringFilter( const std::string& listName, const std::string& value )
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        StringListMap::iterator it = m_stringFilters.find( listName );
        if ( it != m_stringFilters.end() )
        {
            // If the list already exists just add the value.
            it->second.push_back( value );
        }
        else
        {
            // Otherwise create the list.
            StringList newList;
            newList.push_back( value );
            m_stringFilters.insert( StringListMap::value_type( listName, newList ) );
        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    void NamedFilter::removeStringFilter( const std::string& listName )
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        StringListMap::iterator it = m_stringFilters.find( listName );
        if ( it != m_stringFilters.end() )
        {
            // If the list exists delete it.
            m_stringFilters.erase( it );
        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    NamedFilter::StringList NamedFilter::getNumericFilterList() const
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        StringList numericFilters;
        
        // Iterate through the numeric map and pull out all the names.
        NumericListMap::const_iterator it;
        for ( it=m_numericFilters.begin(); it!=m_numericFilters.end(); it++ )
        {
            numericFilters.push_back( it->first );
        }

        return numericFilters;
    }


    NamedFilter::NumericList NamedFilter::getNumericFilter( const std::string& listName ) const
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        // Find the requested list.
        NumericListMap::const_iterator it = m_numericFilters.find( listName );
        if ( it != m_numericFilters.end() )
        {
            return it->second;
        }

        // If no list was found return an empty one.
        NumericList emptyList;
        return emptyList;
    }


    void NamedFilter::setNumericFilter( const std::string& listName, const NumericList& numericList )
    {
        // Remove the existing filter.
        removeNumericFilter( listName );

        // If the new list isn't empty, add it.
        if ( !numericList.empty() )
        {
            // Column has not yet been added, so need to add it to the map before adding the filter data to the vector
            std::pair<NumericListMap::iterator,bool> result = m_numericFilters.insert(
                NumericListMap::value_type(listName,numericList) );

            // Make sure the list is sorted.
            std::sort( result.first->second.begin(), result.first->second.end() );

        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    void NamedFilter::addNumericFilter( const std::string& listName, const unsigned long value )
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        NumericListMap::iterator it = m_numericFilters.find( listName );
        if ( it != m_numericFilters.end() )
        {
            // If the list already exists just insert the value in the correct position.
            NumericList::iterator insertPt = std::find_if( it->second.begin(), it->second.end(),
                std::bind2nd( std::greater<unsigned long>(), value ) );
            it->second.insert( insertPt, value );
        }
        else
        {
            // Otherwise create the list.
            NumericList newList;
            newList.push_back( value );
            m_numericFilters.insert( NumericListMap::value_type( listName, newList ) );
        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    void NamedFilter::removeNumericFilter( const std::string& listName )
    {
        TA_Base_Core::ThreadGuard guard( m_filterLock );

        NumericListMap::iterator it = m_numericFilters.find( listName );
        if ( it != m_numericFilters.end() )
        {
            // If the list exists delete it.
            m_numericFilters.erase( it );
        }

        // The filter was changed so it should now be unnamed.
        m_filterName = UNNAMED;
    }


    void NamedFilter::saveNamedFilter()
    {
        // It's not possible to save an unnamed filter.
        if ( ( 0 == m_filterName.size() ) || ( UNNAMED == m_filterName ) )
        {
            throw DataException( "An unnamed filter cannot be saved.", DataException::INVALID_VALUE,
                                 getSessionId()+"::"+UNNAMED );
        }
        
        // Make sure a filter of this name doesn't already exist.
        FilterNameList filterNames = NamedFilterAccessFactory::getInstance().getAllFilterNames();
        FilterNameList::iterator it;
        for ( it=filterNames.begin(); it!=filterNames.end(); it++ )
        {
            if ( it->first == m_filterName )
            {
                // Can't save when a filter with the same name already exists.
                throw DataException( "A filter with already exists with this name.", DataException::NOT_UNIQUE,
                                     getSessionId()+"::"+m_filterName );
            }
        }

        // First save the string filters.
        StringListMap::iterator strIt;
        for ( strIt=m_stringFilters.begin(); strIt!=m_stringFilters.end(); strIt++ )
        {
            StringList::iterator listIt;
            for ( listIt=strIt->second.begin(); listIt!=strIt->second.end(); listIt++ )
            {
                saveFilterItem( strIt->first, LIST_TYPE_STRING, *listIt );
            }
        }

        // Now save the numeric filters.
        NumericListMap::iterator numIt;
        for ( numIt=m_numericFilters.begin(); numIt!=m_numericFilters.end(); numIt++ )
        {
            NumericList::iterator listIt;
            for ( listIt=numIt->second.begin(); listIt!=numIt->second.end(); listIt++ )
            {
                std::ostringstream numericToString;
                numericToString << *listIt;
                saveFilterItem( numIt->first, LIST_TYPE_NUMERIC, numericToString.str() );
            }
        }
    }


    void NamedFilter::deleteNamedFilter()
    {
        // Make sure a filter exists and can be deleted.
        FilterNameList filterNames = NamedFilterAccessFactory::getInstance().getAllFilterNames();
        FilterNameList::iterator it;
        for ( it=filterNames.begin(); it!=filterNames.end(); it++ )
        {
            if ( it->first == m_filterName )
            {
                if ( it->second )
                {
                    // Get a connection to the database.
                    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(AlarmsLocal_Cd, Write);

                    // Construct the modification.
                    /*std::ostringstream sql;
                    sql << "DELETE FROM named_filter WHERE applicationkey=" << getApplicationKey()
                        << " AND operatorkey=( SELECT operatorkey FROM ta_session WHERE pkey='" << getSessionId() << "' )"
                        << " AND filter_name='" << databaseConnection->escapeInsertString(m_filterName) << "'";*/
//					std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), named_filter_STD_DELETE_17901, getApplicationKey(),
//					std::string strSql  = databaseConnection->prepareSQLStatement(NAMED_FILTER_DELETE_17901, getApplicationKey(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, NAMED_FILTER_DELETE_17901, getApplicationKey(),
						getSessionId(), databaseConnection->escapeInsertString(m_filterName) );

                    // Execute the modification.
                    databaseConnection->executeModification( strSql );

                    // The filter is now deleted, so it should be made unnamed.
                    m_filterName = UNNAMED;

                    // Deleting is complete.
                    return;
                }
                else
                {
                    // The filter is read-only.
                    throw DataException( "Cannot delete because the filter is read-only.", DataException::CANNOT_DELETE,
                                         getSessionId()+"::"+m_filterName );
                }
            }
        }

        // If we get to here then the named filter wasn't found.
        throw DataException( "Cannot delete because the filter was not found.", DataException::CANNOT_DELETE,
                             getSessionId()+"::"+m_filterName );
    }


    unsigned long NamedFilter::getApplicationKey()
    {
        // Convert the type to a number using a string stream.
        unsigned long applicationKey;
        std::stringstream appType( TA_Base_Core::RunParams::getInstance().get( RPARAM_APPTYPE ) );
        appType >> applicationKey;

        return applicationKey;
    }


    std::string NamedFilter::getSessionId()
    {
        return TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ); 
    }


    void NamedFilter::saveFilterItem( const std::string& listName, const unsigned long listType, const std::string& listItem )
    {
        // Get a connection to the database.
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(AlarmsLocal_Cd, Write);

        // Construct the modification.
       /* std::ostringstream sql;
        sql << "INSERT INTO named_filter VALUES ( ( SELECT operatorkey FROM ta_session WHERE pkey='" << getSessionId() << "' ), "
            << getApplicationKey() << ", '" << databaseConnection->escapeInsertString(m_filterName) << "', '"
            << databaseConnection->escapeInsertString(listName) << "', " << listType << ", '"
            << databaseConnection->escapeInsertString(listItem) << "' )";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), named_filter_STD_INSERT_17401, getSessionId(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(NAMED_FILTER_INSERT_17401, getSessionId(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, NAMED_FILTER_INSERT_17401, getSessionId(),
			getApplicationKey(), databaseConnection->escapeInsertString(m_filterName), databaseConnection->escapeInsertString(listName),
			listType, databaseConnection->escapeInsertString(listItem) );

        // Execute the modification.
        databaseConnection->executeModification( strSql );
    }
};


/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <memory>
#include <sstream>
#include <vector>

#include "core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Core
{
    const std::string    LivePropertyDataHelper::TABLE_NAME                   = "dr_live_property";

    const std::string    LivePropertyDataHelper::FIELD_TYPEKEY                = "DRLP_TYPEKEY";
    const std::string    LivePropertyDataHelper::FIELD_CATEGORY               = "DRLP_CATEGORY";
    const std::string    LivePropertyDataHelper::FIELD_NAME                   = "DRLP_NAME";
    const std::string    LivePropertyDataHelper::FIELD_IDENTIFIER             = "DRLP_IDENTIFIER";
    const std::string    LivePropertyDataHelper::FIELD_TABLE                  = "DRLP_TABLE";
    const std::string    LivePropertyDataHelper::FIELD_COLUMN                 = "DRLP_COLUMN";
    const std::string    LivePropertyDataHelper::FIELD_COLUMNTYPE             = "DRLP_COLUMN_TYPE";
    const std::string    LivePropertyDataHelper::FIELD_ENGMAX                 = "DRLP_ENG_MAX";
    const std::string    LivePropertyDataHelper::FIELD_ENGMIN                 = "DRLP_ENG_MIN";
    const std::string    LivePropertyDataHelper::FIELD_STATUS                 = "DRLP_STATUS";
    const std::string    LivePropertyDataHelper::FIELD_INTERVALBETWEENWRITES  = "INTERVAL_BETWEEN_WRITES";
    const std::string    LivePropertyDataHelper::FIELD_MINSUMMARYINTERVAL     = "MIN_SUMMARY_INTERVAL";

	bool LivePropertyDataHelper::s_Initialize = false;
	std::map<std::string, LivePropertyDataHelper::LivePropertyPtr>	LivePropertyDataHelper::s_mapProperty;	

    LivePropertyDataHelper::LivePropertyDataHelper()
    : m_isValid( false ), m_isNew( true ), m_typeKey( 0 ), m_category( "" ), m_name( "" ), 
      m_identifier( "" ), m_table( "" ), m_column( "" ), m_columnType( "" ),
      m_engineeringMax( 0.0 ), m_engineeringMin( 0.0 ), m_status( "" ), 
      m_intervalBetweenWrites( 0 ), m_minSummaryInterval( 0 )
    {
        // Nothing else
    }


    LivePropertyDataHelper::LivePropertyDataHelper( unsigned long typeKey, const std::string& name )
    : m_isValid( false ), m_isNew( false ), m_typeKey( typeKey ), m_category( "" ), m_name( name ), 
      m_identifier( "" ), m_table( "" ), m_column( "" ), m_columnType( "" ),
      m_engineeringMax( 0.0 ), m_engineeringMin( 0.0 ), m_status( "" ), 
      m_intervalBetweenWrites( 0 ), m_minSummaryInterval( 0 )
    {
        // Nothing else
    }


    LivePropertyDataHelper::~LivePropertyDataHelper()
    {
        // Nothing
    }

    
    unsigned long LivePropertyDataHelper::getTypeKey()
    {
        return m_typeKey;
    }


    std::string LivePropertyDataHelper::getCategory()
    {
        testForReload();
        return m_category;
    }
    
     
    std::string LivePropertyDataHelper::getName()
    {
        testForReload();
        return m_name;
    }

    
    std::string LivePropertyDataHelper::getIdentifier()
    {
        testForReload();
        return m_identifier;
    }

    
     
    std::string LivePropertyDataHelper::getTable()
    {
        testForReload();
        return m_table;
    }

    
     
    std::string LivePropertyDataHelper::getColumn()
    {
        testForReload();
        return m_column;
    }

    
     
    std::string LivePropertyDataHelper::getColumnType()
    {
        testForReload();
        return m_columnType; 
    }

    
     
    double LivePropertyDataHelper::getEngineeringMax()
    {
        testForReload();
        return m_engineeringMax;
    }

    
     
    double LivePropertyDataHelper::getEngineeringMin()
    {
        testForReload();
        return m_engineeringMin;
    }

    

    std::string LivePropertyDataHelper::getStatus()
    {
        testForReload();
        return m_status;
    }
    

    unsigned long LivePropertyDataHelper::getIntervalBetweenWrites()
    {
        testForReload();
        return m_intervalBetweenWrites;
    }


    unsigned long LivePropertyDataHelper::getMinSummaryInterval()
    {
        testForReload();
        return m_minSummaryInterval;
    }


    void LivePropertyDataHelper::setTypeKey( unsigned long typeKey )
    {
        FUNCTION_ENTRY( "setTypeKey" );
        TA_ASSERT( m_isNew, "Can't change type key for existing record" );

        testForReload();
        m_typeKey = typeKey;
    }


    void LivePropertyDataHelper::setCategory( const std::string& category )
    {
        FUNCTION_ENTRY( "setCategory" );

        testForReload();
        m_category = category;
    }


    void LivePropertyDataHelper::setName( const std::string& name )
    {
        FUNCTION_ENTRY( "setName" );
        TA_ASSERT( m_isNew, "Can't change name for existing record" );

        testForReload();
        m_name = name;
    }


    void LivePropertyDataHelper::setIdentifier( const std::string& id )
    {
        FUNCTION_ENTRY( "setIdentifier" );

        testForReload();
        m_identifier = id;
    }


    void LivePropertyDataHelper::setTable( const std::string& tableName )
    {
        FUNCTION_ENTRY( "setTable" );

        testForReload();
        m_table = tableName;
    }


    void LivePropertyDataHelper::setColumn( const std::string& columnName )
    {
        FUNCTION_ENTRY( "setColumn" );

        testForReload();
        m_column = columnName;
    }


    void LivePropertyDataHelper::setColumnType( const std::string& columnType )
    {
        FUNCTION_ENTRY( "setColumnType" );

        testForReload();
        m_columnType = columnType;
    }
    
    
    void LivePropertyDataHelper::setEngineeringMax( double engMax )
    {
        FUNCTION_ENTRY( "setEngineeringMax" );

        testForReload();
        m_engineeringMax = engMax;
    }


    void LivePropertyDataHelper::setEngineeringMin( double engMin )
    {
        FUNCTION_ENTRY( "setEngineeringMin" );

        testForReload();
        m_engineeringMin = engMin;
    }

    
    void LivePropertyDataHelper::setStatus( const std::string& status )
    {
        FUNCTION_ENTRY( "setStatus" );

        testForReload();
        m_status = status;
    }

    
    void LivePropertyDataHelper::setIntervalBetweenWrites( unsigned long interval )
    {
        FUNCTION_ENTRY( "setIntervalBetweenWrites" );

        testForReload();
        m_intervalBetweenWrites = interval;
    }


    void LivePropertyDataHelper::setMinSummaryInterval( unsigned long interval )
    {
        FUNCTION_ENTRY( "setMinSummaryInterval" );

        testForReload();
        m_minSummaryInterval = interval;
    }

    
    void LivePropertyDataHelper::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        m_isValid = false;
    }

    
    void LivePropertyDataHelper::testForReload()
    {
        if ( ! m_isValid && ! m_isNew )
        {
            reload();
        }
    }


    void LivePropertyDataHelper::reload()
    {
        FUNCTION_ENTRY( "reload" );

		if( !s_Initialize)
		{
			bulkLoadTotalTable();
		}
	
        TA_ASSERT( m_typeKey > 0 && m_name.length() > 0, "Type key and/or name has not been set" );
		std::string strFirst = m_name + boost::lexical_cast<std::string>( m_typeKey );
		
		std::map<std::string, LivePropertyPtr>::iterator it(LivePropertyDataHelper::s_mapProperty.find(strFirst));
		if( it == s_mapProperty.end() )
		{
			return;
		}

		m_isValid = true;
		LivePropertyPtr pData = (LivePropertyPtr)it->second;
		m_category = pData->category;
		m_identifier = pData->identifier;

		m_table = pData->table;
		m_column = pData->column;
		m_columnType = pData->columnType;
		m_status = pData->status;
		
		m_intervalBetweenWrites = pData->intervalBetweenWrites;
		
		m_minSummaryInterval = pData->minSummaryInterval;
		
		m_engineeringMax = pData->engineeringMax;
		
		m_engineeringMin = pData->engineeringMin;
		/*	
        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);

        std::string sql = "SELECT " + FIELD_CATEGORY + "," + FIELD_IDENTIFIER + "," + FIELD_TABLE + ",";
        sql += FIELD_COLUMN + "," + FIELD_COLUMNTYPE + "," + FIELD_ENGMAX + "," + FIELD_ENGMIN + ",";
        sql += FIELD_STATUS + "," + FIELD_INTERVALBETWEENWRITES + "," + FIELD_MINSUMMARYINTERVAL + " ";
        sql += "FROM " + TABLE_NAME + " WHERE " + FIELD_TYPEKEY + "=" + boost::lexical_cast<std::string>( m_typeKey );
        sql += " AND " + FIELD_NAME + "='" + databaseConnection->escapeQueryString(m_name) + "'";

        std::vector<std::string> columnNames;
        columnNames.push_back( FIELD_CATEGORY );
        columnNames.push_back( FIELD_IDENTIFIER );
        columnNames.push_back( FIELD_TABLE );
        columnNames.push_back( FIELD_COLUMN );
        columnNames.push_back( FIELD_COLUMNTYPE );
        columnNames.push_back( FIELD_ENGMAX );
        columnNames.push_back( FIELD_ENGMIN );
        columnNames.push_back( FIELD_STATUS );
        columnNames.push_back( FIELD_INTERVALBETWEENWRITES );
        columnNames.push_back( FIELD_MINSUMMARYINTERVAL );

        std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.c_str(), columnNames ) );
        if ( data.get() != NULL )
        {
            if ( data->getNumRows() == 0 )
            {
                // Do nothing
            }
            else if ( data->getNumRows() > 1 )
            {
                // Multiple records for the same key/name - throw exception

                std::stringstream errorMessage;
                errorMessage << "Multiple records found for key=" << m_typeKey << ", name=" << m_name;
                TA_THROW( DataException( errorMessage.str().c_str(), DataException::NOT_UNIQUE, "" ) );
            }
            else
            {
                try
                {
                    m_category = data->getStringData( 0, FIELD_CATEGORY );
                    m_identifier = data->getStringData( 0, FIELD_IDENTIFIER );
                    m_table = data->getStringData( 0, FIELD_TABLE );
                    m_column = data->getStringData( 0, FIELD_COLUMN );
                    m_columnType = data->getStringData( 0, FIELD_COLUMNTYPE );
                    m_status = data->getStringData( 0, FIELD_STATUS );

                    std::string writeInterval = data->getStringData( 0, FIELD_INTERVALBETWEENWRITES );
                    m_intervalBetweenWrites = writeInterval.length() == 0 ? 0 : boost::lexical_cast<unsigned long>( writeInterval );

                    std::string minSummary = data->getStringData( 0, FIELD_MINSUMMARYINTERVAL );
                    m_minSummaryInterval = minSummary.length() == 0 ? 0 : boost::lexical_cast<unsigned long>( minSummary );

                    std::string engMax = data->getStringData( 0, FIELD_ENGMAX );
                    m_engineeringMax = engMax.length() == 0 ? 0 : boost::lexical_cast<double>( engMax );

                    std::string engMin = data->getStringData( 0, FIELD_ENGMIN );
                    m_engineeringMin = engMin.length() == 0 ? 0 : boost::lexical_cast<double>( engMin );
                }
                catch( boost::bad_lexical_cast& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "boost::bad_lexical_cast", 
                                         "Bad numerical format for one or more data values" );
                }

                m_isValid = true;
            }
        }
		*/
    }


    void LivePropertyDataHelper::applyChanges()
    {
        FUNCTION_ENTRY( "applyChanges" );

		SQLStatement sql;

        // Get the database connection and execute SQL Statement.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Write);
        
        if ( m_isNew )
        {
            /*sql = "INSERT INTO " + TABLE_NAME + " ( " + FIELD_TYPEKEY + "," + FIELD_CATEGORY + "," + FIELD_NAME + ",";
            sql += FIELD_IDENTIFIER + "," + FIELD_TABLE + "," + FIELD_COLUMN + "," + FIELD_COLUMNTYPE + ",";
            sql += FIELD_ENGMAX + "," + FIELD_ENGMIN + "," + FIELD_STATUS + "," + FIELD_INTERVALBETWEENWRITES + ",";
            sql += FIELD_MINSUMMARYINTERVAL + ") VALUES (" + boost::lexical_cast<std::string>( m_typeKey ) + ",'" +
            sql += databaseConnection->escapeQueryString(m_category) + "','" + databaseConnection->escapeQueryString(m_name);
            sql += "','" + databaseConnection->escapeQueryString(m_identifier) + "','" + databaseConnection->escapeQueryString(m_table);
            sql += "','" + databaseConnection->escapeQueryString(m_column);
            sql += "','" + databaseConnection->escapeQueryString(m_columnType) + "," + boost::lexical_cast<std::string>( m_engineeringMax ) + ",";
            sql += boost::lexical_cast<std::string>( m_engineeringMin ) + "," + databaseConnection->escapeQueryString(m_status) + ",";
            sql += boost::lexical_cast<std::string>( m_intervalBetweenWrites ) + ",";
            sql += boost::lexical_cast<std::string>( m_minSummaryInterval ) + ")";*/
//			sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_STD_INSERT_27801, boost::lexical_cast<std::string>( m_typeKey ),
//			sql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_INSERT_27801, boost::lexical_cast<std::string>( m_typeKey ),

		databaseConnection->prepareSQLStatement(sql, DR_LIVE_PROPERTY_INSERT_27801, boost::lexical_cast<std::string>( m_typeKey ),
				databaseConnection->escapeQueryString(m_category), databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_identifier),
				databaseConnection->escapeQueryString(m_table), databaseConnection->escapeQueryString(m_column), databaseConnection->escapeQueryString(m_columnType),
				boost::lexical_cast<std::string>( m_engineeringMax ), boost::lexical_cast<std::string>( m_engineeringMin ), databaseConnection->escapeQueryString(m_status),
				boost::lexical_cast<std::string>( m_intervalBetweenWrites ), boost::lexical_cast<std::string>( m_minSummaryInterval ));
        }
        else
        {
            testForReload();

            /*sql = "UPDATE " + TABLE_NAME + " SET " + FIELD_CATEGORY + "='" + databaseConnection->escapeInsertString(m_category) + "',";
            sql += FIELD_IDENTIFIER + "='" + m_identifier + "'," + FIELD_TABLE + "='" + databaseConnection->escapeInsertString(m_table) + "',";
            sql += FIELD_COLUMN + "='" + databaseConnection->escapeInsertString(m_column) + "'," + FIELD_COLUMNTYPE + "='";
            sql += databaseConnection->escapeInsertString(m_columnType) + "',";
            sql += FIELD_ENGMAX + "=" + boost::lexical_cast<std::string>( m_engineeringMax ) + ",";
            sql += FIELD_ENGMIN + "=" + boost::lexical_cast<std::string>( m_engineeringMin ) + ",";
            sql += FIELD_STATUS + "='" + databaseConnection->escapeInsertString(m_status) + "',"; 
            sql += FIELD_INTERVALBETWEENWRITES + "=" + boost::lexical_cast<std::string>( m_intervalBetweenWrites ) + ",";
            sql += FIELD_MINSUMMARYINTERVAL + "=" + boost::lexical_cast<std::string>( m_minSummaryInterval );
            sql += " WHERE " + FIELD_TYPEKEY + "=" + boost::lexical_cast<std::string>( m_typeKey ) + " AND ";
            sql += FIELD_NAME + "='" + databaseConnection->escapeInsertString(m_name) + "'";*/
//			sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_STD_UPDATE_27651, databaseConnection->escapeInsertString(m_category),
//			sql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_UPDATE_27651, databaseConnection->escapeInsertString(m_category),

		databaseConnection->prepareSQLStatement(sql, DR_LIVE_PROPERTY_UPDATE_27651, databaseConnection->escapeInsertString(m_category),
				m_identifier, databaseConnection->escapeInsertString(m_table), databaseConnection->escapeInsertString(m_column), databaseConnection->escapeInsertString(m_columnType),
				boost::lexical_cast<std::string>( m_engineeringMax ), boost::lexical_cast<std::string>( m_engineeringMin ), databaseConnection->escapeInsertString(m_status),
				boost::lexical_cast<std::string>( m_intervalBetweenWrites ), boost::lexical_cast<std::string>( m_minSummaryInterval ), boost::lexical_cast<std::string>( m_typeKey ),
				databaseConnection->escapeInsertString(m_name));
        }

        try
        {
            if ( databaseConnection != NULL )
            {
                databaseConnection->executeModification( sql );
    
                m_isValid = true;
                m_isNew = false;
            }
        }
        catch( DatabaseException& dbex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbex.what() );
        }
        catch( DataException& dex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", dex.what() );
        }
    }


    void LivePropertyDataHelper::remove()
    {
        FUNCTION_ENTRY( "remove" );

        if ( ! m_isNew )
        {
            // Get the database connection and execute SQL Statement.
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Write);
                
            /*std::string sql = "DELETE FROM " + TABLE_NAME + " WHERE " + FIELD_TYPEKEY + "=";
            sql += boost::lexical_cast<std::string>( m_typeKey ) + " AND " + FIELD_NAME + "='";
            sql += databaseConnection->escapeInsertString(m_name) + "'";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_STD_DELETE_27901,
//			std::string strSql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_DELETE_27901,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_LIVE_PROPERTY_DELETE_27901,
				boost::lexical_cast<std::string>( m_typeKey ), databaseConnection->escapeInsertString(m_name));

            try
            {
                if ( databaseConnection != NULL )
                {
                    databaseConnection->executeModification( strSql );
                }
            }
            catch( DatabaseException& dbex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbex.what() );
            }
            catch( DataException& dex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", dex.what() );
            }
        }

        invalidate(); 
    }

	void LivePropertyDataHelper::bulkLoadTotalTable()
	{				
        /*std::string sql = "SELECT " + FIELD_NAME + "," + FIELD_TYPEKEY + "," + FIELD_CATEGORY + "," + FIELD_IDENTIFIER + "," + FIELD_TABLE + ",";
        sql += FIELD_COLUMN + "," + FIELD_COLUMNTYPE + "," + FIELD_ENGMAX + "," + FIELD_ENGMIN + ",";
        sql += FIELD_STATUS + "," + FIELD_INTERVALBETWEENWRITES + "," + FIELD_MINSUMMARYINTERVAL + " ";
        sql += "FROM " + TABLE_NAME;*/
		
        std::vector<std::string> columnNames;
        columnNames.push_back( FIELD_NAME );
        columnNames.push_back( FIELD_TYPEKEY );
        columnNames.push_back( FIELD_CATEGORY );
        columnNames.push_back( FIELD_IDENTIFIER );
        columnNames.push_back( FIELD_TABLE );
        columnNames.push_back( FIELD_COLUMN );
        columnNames.push_back( FIELD_COLUMNTYPE );
        columnNames.push_back( FIELD_ENGMAX );
        columnNames.push_back( FIELD_ENGMIN );
        columnNames.push_back( FIELD_STATUS );
        columnNames.push_back( FIELD_INTERVALBETWEENWRITES );
        columnNames.push_back( FIELD_MINSUMMARYINTERVAL );
		
        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_STD_SELECT_27501);
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_SELECT_27501);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_LIVE_PROPERTY_SELECT_27501);
		IData* data = databaseConnection->executeQuery( strSql, columnNames );

        if ( data != NULL )
        {		
			do
			{
				for( unsigned long i = 0; i < data->getNumRows(); ++i )
				{
					LivePropertyPtr pTemp = LivePropertyPtr( new _LIVE_PROPERTY );
					pTemp->name = data->getStringData( i, FIELD_NAME );
					pTemp->category = data->getStringData( i, FIELD_CATEGORY );
					pTemp->identifier = data->getStringData( i, FIELD_IDENTIFIER );
					pTemp->table = data->getStringData( i, FIELD_TABLE );
					pTemp->column = data->getStringData( i, FIELD_COLUMN );
					pTemp->columnType = data->getStringData( i, FIELD_COLUMNTYPE );
					pTemp->status = data->getStringData( i, FIELD_STATUS );
					
					std::string writeInterval = data->getStringData( i, FIELD_INTERVALBETWEENWRITES );
					pTemp->intervalBetweenWrites = writeInterval.length() == 0 ? 0 : boost::lexical_cast<unsigned long>( writeInterval );
					
					std::string minSummary = data->getStringData( i, FIELD_MINSUMMARYINTERVAL );
					pTemp->minSummaryInterval = minSummary.length() == 0 ? 0 : boost::lexical_cast<unsigned long>( minSummary );
					
					std::string typeKey = data->getStringData( i, FIELD_TYPEKEY );
					pTemp->typeKey = typeKey.length() == 0 ? 0 : boost::lexical_cast<unsigned long>( typeKey );
					
					std::string engMax = data->getStringData( i, FIELD_ENGMAX );
					pTemp->engineeringMax = engMax.length() == 0 ? 0 : boost::lexical_cast<double>( engMax );
					
					std::string engMin = data->getStringData( i, FIELD_ENGMIN );
					pTemp->engineeringMin = engMin.length() == 0 ? 0 : boost::lexical_cast<double>( engMin );
					
					std::string strFirst = pTemp->name + boost::lexical_cast<std::string>( pTemp->typeKey );
					
					LivePropertyDataHelper::s_mapProperty[strFirst] = pTemp;
				}			
				delete data;
				data = NULL;
				
			}
			while( databaseConnection->moreData( data ) );
			
			s_Initialize = true;
        }		
	}

}

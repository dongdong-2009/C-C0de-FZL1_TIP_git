#include <ace/OS.h>

#include <sstream>

#include "SOEEvent.h"
#include "SOEAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include <time.h>
#include <algorithm>
#include <core/exceptions/src/DataException.h>
#include <core/utilities/src/RunParams.h>
#include <core/data_access_interface/src/DatabaseFactory.h>
#include <core/data_access_interface/src/IData.h>
#include <core/data_access_interface/src/DbConnectionStrings.h>
#include "core/data_access_interface/src/SQLCode.h"

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::IDatabase;


namespace TA_Base_Core
{
	TA_Base_Core::NonReEntrantThreadLockable SOEAccessFactory::m_singletonLock;
	SOEAccessFactory* SOEAccessFactory::m_self = NULL;


	SOEAccessFactory::SOEAccessFactory()
	{
		m_maxReturnRow=10000;
		m_columns.push_back( SOEEvent::COL_NAME_PKEY );
		m_columns.push_back( SOEEvent::COL_NAME_SOURCE_TIME );
		m_columns.push_back( SOEEvent::COL_NAME_TRANSACTIVE_TIME );
		m_columns.push_back( SOEEvent::COL_NAME_ENTITY_KEY );
		m_columns.push_back( SOEEvent::COL_NAME_ASSET_NAME );
		m_columns.push_back( SOEEvent::COL_NAME_VALUE_DATA );
		m_columns.push_back( SOEEvent::COL_NAME_DISPLAY_VALUE );
		m_columns.push_back( SOEEvent::COL_NAME_LOCATION_KEY );
		m_columns.push_back( SOEEvent::COL_NAME_LOCATION_NAME );
	}

	std::string SOEAccessFactory::generateORList( const std::string& columnName,
		const std::vector<unsigned long>& keyList,
		IDatabase& databaseConnection)
	{
		// The string to store the list in.
		std::stringstream expression;

		std::vector<unsigned long>::const_iterator keyIt;
		for ( keyIt=keyList.begin(); keyIt!=keyList.end(); keyIt++ )
		{
			// If this isn't the first element then it needs an OR added first..
			if ( keyIt != keyList.begin() )
			{
				expression << " OR ";
			}

			// Add the expression to the list.
			expression << databaseConnection.escapeInsertString(columnName) << "=" << *keyIt;
		}

		return expression.str();
	}

	SOEAccessFactory& SOEAccessFactory::getInstance()
	{
		if( m_self ) return *m_self;
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if(! m_self ) m_self = new SOEAccessFactory();
		return *m_self;
	}


	unsigned long  SOEAccessFactory::getMaxPKey()
	{

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		// Create the SQL statement to execute.
		//std::string sql("SELECT last_number as maxpkey, cache_size as cachesize From user_sequences where upper(sequence_name) in ('EV_SOE_SEQ')");
 		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SOE_SELECT_213051);
		// Set up the columnNames vector to be passed to executeQuery()
		std::string maxPkeyColumn = "maxpkey";
		std::string cacheSizeColumn = "cachesize";
		std::vector<std::string> columnNames;
		columnNames.push_back( maxPkeyColumn );
		columnNames.push_back( cacheSizeColumn );
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );

		if (0 == data->getNumRows())
		{
			std::string reasonMessage = "Could not retrieve maxpkey. getNumRows() return 0";
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, reasonMessage.c_str());
			TA_THROW(DataException(reasonMessage.c_str(),DataException::NO_VALUE,""));
		}

		unsigned long maxpkey= data->getUnsignedLongData(0 , maxPkeyColumn );
		unsigned long cacheSize= data->getUnsignedLongData(0 , cacheSizeColumn );
		cacheSize=cacheSize*2+1;
		//std::stringstream sqlbuffer;
		columnNames.clear();
		columnNames.push_back( maxPkeyColumn );

		SQLStatement strSql;
		//sqlbuffer<<"select /*+  index (ev_soe EVSOE_PKEY_INDX ) */  max(pkey) as maxpkey from ev_soe where pkey between "<< maxpkey-cacheSize << " and " <<maxpkey + cacheSize << " order by Pkey desc" ;
		//std::auto_ptr<IData> data2( databaseConnection->executeQuery( sqlbuffer.str(), columnNames ) );
		databaseConnection->prepareSQLStatement(strSql, SOE_SELECT_213002, maxpkey>=cacheSize? (maxpkey-cacheSize):0, maxpkey + cacheSize);
		std::auto_ptr<IData> data2( databaseConnection->executeQuery( strSql, columnNames ) );
		
		if( 0 < data2->getNumRows())
		{
			maxpkey= data2->getUnsignedLongData(0 , maxPkeyColumn );
		}
		else
		{
			return maxpkey;
		}
		return maxpkey;
	}
	
	void SOEAccessFactory::saveSOE( SOEEvent& evt )
	{
		//std::stringstream sql;
		//std::stringstream sqlSourceTime;
		//std::stringstream sqlTransactiveTime;

		// get database obj
		TA_Base_Core::IDatabase* db = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Event_Ad, Write );

		// convert time format
		struct tm timeTemp;
		char logSourceTimeString[24] = { '\0', };
		char logTransactiveTimeString[24] = { '\0', };
		timeb t = (const timeb&)( evt.getSourceTime() );

		if (ACE_OS::localtime_r(&t.time, &timeTemp) != NULL)
		{
			sprintf(logSourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", timeTemp.tm_mday, timeTemp.tm_mon + 1,
				timeTemp.tm_year + 1900, timeTemp.tm_hour, timeTemp.tm_min, timeTemp.tm_sec,
				t.millitm);
		} 
		//sqlSourceTime << "TO_TIMESTAMP(''" << strTime << "'',''dd/mm/yyyy hh24:mi:ss.ff3'')";
		
		t = evt.getTransactiveTime();
		if (ACE_OS::localtime_r(&t.time, &timeTemp) != NULL)
		{
			sprintf(logTransactiveTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", timeTemp.tm_mday, timeTemp.tm_mon + 1,
				timeTemp.tm_year + 1900, timeTemp.tm_hour, timeTemp.tm_min, timeTemp.tm_sec,
				t.millitm);
		}		
		//sqlTransactiveTime << "TO_TIMESTAMP(''" << strTime << "'',''dd/mm/yyyy hh24:mi:ss.ff3'')";
		
		// construct sql
		if( evt.isNew() )
		{
			// insert new soe event
// 			 sql<<"BEGIN audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','GROUP', '"
// 				<< "INSERT INTO EV_SOE"
// 				<< "( SOURCE_TIME, TRANSACTIVE_TIME, ENTITY_KEY, ASSET_NAME, "
// 				<< "VALUE_DATA, DISPLAY_VALUE, LOCATION_KEY,DESCRIPTION,LOCATION_NAME) "
// 				<<  "VALUES("
// 				<< sqlSourceTime.str() << ","
// 				<< sqlTransactiveTime.str() << ","
// 				<< evt.getEntityKey() << ",''"
// 				<< db->escapeAQSQLString( evt.getAssetName() ) << "'',''"
// 				<< db->escapeAQSQLString( evt.getValueData() ) << "'',''"
// 				<< db->escapeAQSQLString( evt.getDisplayValue() ) << "'',"
// 				<< evt.getLocationKey() << ",''"
// 				<< db->escapeAQSQLString( evt.getDescription() ) << "'',''"
// 				<< db->escapeAQSQLString( evt.getLocationName() ) << "'')"				 
// 				<<"');END;";
			SQLStatement strSql;
			db->prepareSQLStatement(strSql, SOE_INSERT_217353, getLocalDatabaseName(), db->escapeAQSQLString(logSourceTimeString),
				db->escapeAQSQLString(logTransactiveTimeString), evt.getEntityKey(),
				  db->escapeAQSQLString(evt.getAssetName()), db->escapeAQSQLString(evt.getValueData()),
				db->escapeAQSQLString( evt.getDisplayValue() ),evt.getLocationKey(), db->escapeAQSQLString( evt.getDescription() ),
				db->escapeAQSQLString(evt.getLocationName() ));
			 db->executeModification(strSql);
		}
		else
		{
			// update existing soe event
			 /*sql<<"BEGIN audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','GROUP', '"
				 <<"UPDATE ev_soe"
				 <<" SET source_time = "<< sqlSourceTime.str() <<", transactive_time = "<< sqlTransactiveTime.str() <<", "
				 <<" entity_key = "<< evt.getEntityKey() <<", "
				 <<" asset_name = ''"<< db->escapeAQSQLString(evt.getAssetName()) <<"'', "
				 <<" value_data = ''"<< db->escapeAQSQLString(evt.getValueData()) <<"'', "
				 <<" display_value = ''"<< db->escapeAQSQLString(evt.getDisplayValue()) <<"'', "
				 <<" location_key ="<< evt.getLocationKey() <<", "
				 <<" DESCRIPTION = ''"<< db->escapeAQSQLString(evt.getDescription()) <<"'', "
				 <<" location_name = ''"<< db->escapeAQSQLString(evt.getLocationName()) <<"'' "
				 <<" WHERE pkey = "<< evt.getPKey() <<"');END;";*/
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "should not have update request!");
		}
		
		// execute sql
		

	}

	void SOEAccessFactory::generateWhereClause(SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection, const SOEEventFilter& filter, const std::string& timeRestrictionClause)
	{

		std::stringstream whereClause;
		
		std::stringstream timeString;
		std::string strTimeRange, strPageTimeSQL, strLocationSQL, strAssetSQL, strDisplayValueSQL, strEventIDSQL;
		std::string strSortingSQL;
		timeString<<timeRestrictionClause;
		unsigned int i = 0;		
		// Add the time filters 
		if( strcmp( timeRestrictionClause.c_str(), "" ) == 0 )
		{
			// if the timeRestrictionClause is empty string
			// generate the time restriction base on the information in the filter

			//std::string toTime;
			std::string strToTimeOra, strToTimeMy;
			if ( 0 != filter.tmTo )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmToTime = *localtime(&filter.tmTo);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				strToTimeOra = buf;
				strToTimeMy = buf;

				//toTime = buf;
				//toTime = "TO_TIMESTAMP('" + toTime + "','DDMMYYYYHH24MISS')";
				strToTimeOra = "TO_TIMESTAMP('" + strToTimeOra + "','DDMMYYYYHH24MISS')";
				strToTimeMy = "STR_TO_DATE('" + strToTimeMy + "','%d%m%Y%H%i%S')";
			}

			//std::string fromTime="";
			std::string strFromTimeOra, strFromTimeMy;
			if ( 0 == filter.tmFrom )
			{
				// Default is 24 hours before to.
				//fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
				strFromTimeOra = "SYSDATE - INTERVAL '1' DAY";
				strFromTimeMy = "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)";
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm tmFromTime = *localtime(&filter.tmFrom);

				strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

				//fromTime = buf;
				strFromTimeOra = buf;
				strFromTimeMy = buf;
				//fromTime = "TO_TIMESTAMP('" + fromTime + "','DDMMYYYYHH24MISS')";
				strFromTimeOra = "TO_TIMESTAMP('" + strFromTimeOra + "','DDMMYYYYHH24MISS')";
				strFromTimeMy = "STR_TO_DATE('" + strFromTimeMy + "','%d%m%Y%H%i%S')";
			}

			// Now construct the time portion of the where clause.
			if ( !strFromTimeOra.empty() )
			{			
				//timeString << " source_time >= " << fromTime;
				//rSqlParam.add(std::string("( ( source_time >= "));
				rSqlParam.add(std::string(" source_time >= "));
				rSqlParam.add(strFromTimeOra, strFromTimeMy);
			}
			if ( !strToTimeOra.empty() )
			{
				if (!strFromTimeOra.empty())
				{
					//timeString << " AND  source_time <= " << toTime;
					rSqlParam.add(std::string(" AND source_time <= "));
					rSqlParam.add(strToTimeOra, strToTimeMy);
					//rSqlParam.add(std::string(")"));
				}
				else
				{
					//timeString << "  e source_time <= " << toTime;
					//TODO: can remove the "e"?
					rSqlParam.add(std::string(" e source_time <= "));
					rSqlParam.add(strToTimeOra, strToTimeMy);
					//rSqlParam.add(std::string(")"));
				}

			}
		}

		//std::string str=timeString.str();
		//if (str.size()>0)
		//{
		//	whereClause << str;
		//}

        //Duplicated code as above, so remove it.
		// time range
		//strTimeRange =  generateTimeRangeSQL(filter.tmFrom, filter.tmTo);
		
		//if( !strTimeRange.empty()){
		//	if( !whereClause.str().empty() )
		//	{
		//		whereClause<<" AND ";
		//	}
		//	whereClause<<strTimeRange;
		//}		 

		// location restriction
		strLocationSQL =  generateLocationSQL( databaseConnection, filter.locations );
		if( !strLocationSQL.empty() )
		{
			whereClause<<" AND ";
			whereClause<<strLocationSQL;
		}


		if( !filter.strDescription.empty() )
		{
			
			whereClause<<" AND ";
			std::string strdesc=filter.strDescription.c_str();
			std::transform( strdesc.begin(), strdesc.end(), strdesc.begin(),
				::tolower );
			whereClause<<"   lower(description) LIKE '%"<<databaseConnection.escapeInsertString(strdesc)<<"%' ";

		}		 

		// asset
		strAssetSQL = generateAssetSQL( databaseConnection, filter.strAssetName );
		if( !strAssetSQL.empty() )
		{
			whereClause<<" AND ";
			whereClause<<strAssetSQL;
		}

		// display value
		strDisplayValueSQL = generateDisplayValueSQL( databaseConnection,filter.strDisplayValue );
		if( !strDisplayValueSQL.empty() )
		{
			whereClause<<" AND ";
			whereClause<<strDisplayValueSQL;
		}

        rSqlParam.add(whereClause.str());

	}

	std::vector<SOEEvent*> SOEAccessFactory::loadDesignatedEvents(const SOEEventFilter& filter, const std::string pageStartTime, const unsigned long endRow)
	{
		//std::stringstream sql;
		unsigned long nRows = 0;
		std::vector<SOEEvent*> events;
		//std::ostringstream strWhere;
		std::string strWhereClause="";
		std::string strFrom;
		std::string strTimeRange, strPageTimeSQL, strLocationSQL, strAssetSQL, strDisplayValueSQL, strEventIDSQL;
		std::string strSortingSQL;
		int i = 0;
		int iLoop = 0;
		m_maxReturnRow=0;
		double recentEventID = -1;
		//checkDaytable is used to judge whether need to check the day table (ev_soe_v)
		bool checkDaytable = true;

		// get database
		IDatabase* db = DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Event_Ad, Read );


		bool isloadLatestRecord=false;
		if ( 0 == filter.tmFrom  && 0==filter.tmTo ) 
		{
			isloadLatestRecord=true;
			m_curMaxPkey=0;
		}


		// must check the day table when loading first page, the aim for checking day table is for getting the newest soe event
		if(filter.currentPage == 0)
			isloadLatestRecord = true;

		//if(checkDaytable == true && (!*pAuditPKey))
		//	recentEventID = getRecentEventID(db);
		SQLTypeAdapter sqlParamWhereClause;
		generateWhereClause(sqlParamWhereClause,*db,filter, "");
		//strWhereClause=generateWhereClause(filter,"");
		//m_lastWhereClause=strWhereClause.c_str();
		m_lastFilter=filter;

        //std::ostringstream selectSql;
		SQLStatement strSelectSql;
	    if (isloadLatestRecord)
		{			
// 			selectSql << "select /*+  index (ev_soe EVSOE_PKEY_INDX ) */ PKEY, TO_CHAR(source_time,'YYYYMMDDHH24MISSFF3') AS SOURCE_TIME, "
// 				<< " TO_CHAR(transactive_time,'YYYYMMDDHH24MISSFF3') AS TRANSACTIVE_TIME,"
// 				<< " entity_key, asset_name, value_data, display_value, location_key, location_name,DESCRIPTION"
// 				<<"  from ev_soe   where";

			db->prepareSQLStatement(strSelectSql, SOE_SELECT_213057, sqlParamWhereClause);
		}
		else
		{
			/*selectSql << "select PKEY, TO_CHAR(source_time,'YYYYMMDDHH24MISSFF3') AS SOURCE_TIME, "
				<< " TO_CHAR(transactive_time,'YYYYMMDDHH24MISSFF3') AS TRANSACTIVE_TIME,"
				<< " entity_key, asset_name, value_data, display_value, location_key, location_name,DESCRIPTION"
				<<"  from ev_soe_full_v  where";*/
			db->prepareSQLStatement(strSelectSql, SOE_SELECT_214057, sqlParamWhereClause);
		}

		//sql << selectSql.str() << strWhereClause;
		SQLTypeAdapter sqlParamPageTime;
		if( pageStartTime != "")
		{
			//sql << " AND  source_time <= TO_TIMESTAMP('" << pageStartTime << "', 'YYYYMMDDHH24MISS') ";
			sqlParamPageTime.add(std::string(" AND "));
			sqlParamPageTime.add(std::string("source_time "), pageStartTime, enumDateFormat_4, enumFuncFormat_8, enumOperKey_LETHAN);
		}	 

		//sql << " AND ROWNUM  <= " << endRow << " order by source_time desc";
		SQLStatement strSql;
		db->prepareSQLStatement(strSql, SOE_SELECT_213063, strSelectSql, sqlParamPageTime, endRow);

		std::string strExSql;
		int nExDbType= db->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}

		for (unsigned long sqlCnt=0; sqlCnt<strExSql.size()/100+1;sqlCnt++)
		{
			std::string newstr=strExSql.substr(sqlCnt*100,100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}

// 		for (unsigned long sqlCnt=0; sqlCnt<sql.str().size()/100+1;sqlCnt++)
// 		{
// 			std::string newstr=sql.str().substr(sqlCnt*100,100);
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
// 		}
		  
        std::vector<std::string> columnNames;
		columnNames.push_back( SOEEvent::COL_NAME_PKEY );
		columnNames.push_back( SOEEvent::COL_NAME_SOURCE_TIME );
		columnNames.push_back( SOEEvent::COL_NAME_TRANSACTIVE_TIME );
		columnNames.push_back( SOEEvent::COL_NAME_ENTITY_KEY );
		columnNames.push_back( SOEEvent::COL_NAME_ASSET_NAME );
		columnNames.push_back( SOEEvent::COL_NAME_VALUE_DATA );
		columnNames.push_back( SOEEvent::COL_NAME_DISPLAY_VALUE );
		columnNames.push_back( SOEEvent::COL_NAME_LOCATION_KEY );
		columnNames.push_back( SOEEvent::COL_NAME_LOCATION_NAME );
		columnNames.push_back( SOEEvent::COL_NAME_DESCRIPTION );
	 
		std::auto_ptr<IData> data( db->executeQuery( strSql, columnNames ) );

		nRows = data->getNumRows();

		if( nRows>0 )
		{			 
			events.reserve(nRows);
			// get data
			for(unsigned long i = 0; i< nRows; i++)
			{				 
				unsigned long pkey=data->getUnsignedLongData(i,SOEEvent::COL_NAME_PKEY);
				SOEEvent* evt = new SOEEvent( pkey );
				evt->setSourceTime( data->getTimestampData( i, SOEEvent::COL_NAME_SOURCE_TIME ) );
				evt->setTransactiveTime( data->getTimestampData( i, SOEEvent::COL_NAME_TRANSACTIVE_TIME ) );
				evt->setAssetName( data->getStringData( i, SOEEvent::COL_NAME_ASSET_NAME ) );
				evt->setDisplayValue( data->getStringData( i, SOEEvent::COL_NAME_DISPLAY_VALUE ) );
				evt->setLocationName( data->getStringData( i, SOEEvent::COL_NAME_LOCATION_NAME ) );
				evt->setDescription ( data->getStringData( i, SOEEvent::COL_NAME_DESCRIPTION ) );
				
				if (pkey>m_curMaxPkey) m_curMaxPkey=pkey;
				events.push_back(evt);
			}
		}
		else		 
		{
			unsigned long maxPkey=0;
			// Get the latest audit pkey from ev_combined
			maxPkey=getMaxPKey();
			if (maxPkey>m_curMaxPkey)
			{
				m_curMaxPkey=maxPkey;
			}			 
		}
		return events;		

	}



	std::vector<SOEEvent*> SOEAccessFactory::getDesignatedEventsSinceLastLoad()
	{
		std::vector<SOEEvent*> newEvents;
		 
		// First the event table needs to be checked for new elements.
		unsigned long  latestKey = getMaxPKey();

		// If the latest pkeys match ours then don't load. The keys will never match exactly, rather
		// they are considered to match if the latest is exactly one ahead of the last loaded.
		if (  latestKey <= m_curMaxPkey )
		{
			return newEvents;
		}		 

		// Now generate the query.
// 		std::stringstream sql;
		SQLStatement strSql;
		SQLStatement strSelectSql;
// 
// 		sql << "select /*+  index (ev_soe EVSOE_PKEY_INDX ) */ PKEY, TO_CHAR(source_time,'YYYYMMDDHH24MISSFF3') AS SOURCE_TIME, "
// 			<< " TO_CHAR(transactive_time,'YYYYMMDDHH24MISSFF3') AS TRANSACTIVE_TIME,"
// 			<< " entity_key, asset_name, value_data, display_value, location_key,DESCRIPTION , location_name"
// 			<<"  from ev_soe "
// 			<< "WHERE (  pkey > " << m_curMaxPkey			
// 			<< " " << " and " << "  pkey <= " << latestKey << "  ) AND "
// 			<< m_lastWhereClause << " and rownum<=2000 order by  source_time desc" ;	

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		databaseConnection->prepareSQLStatement(strSelectSql, SOE_SELECT_213060,m_curMaxPkey, latestKey);

		SQLTypeAdapter sqlParamWhereClause;
		generateWhereClause(sqlParamWhereClause,*databaseConnection, m_lastFilter,"" );
		
		databaseConnection->prepareSQLStatement(strSql, SOE_SELECT_213062, strSelectSql, sqlParamWhereClause);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( SOEEvent::COL_NAME_PKEY );
		columnNames.push_back( SOEEvent::COL_NAME_SOURCE_TIME );
		columnNames.push_back( SOEEvent::COL_NAME_TRANSACTIVE_TIME );
		columnNames.push_back( SOEEvent::COL_NAME_ENTITY_KEY );
		columnNames.push_back( SOEEvent::COL_NAME_ASSET_NAME );
		columnNames.push_back( SOEEvent::COL_NAME_VALUE_DATA );
		columnNames.push_back( SOEEvent::COL_NAME_DISPLAY_VALUE );
		columnNames.push_back( SOEEvent::COL_NAME_LOCATION_KEY );
		columnNames.push_back( SOEEvent::COL_NAME_DESCRIPTION);
		columnNames.push_back( SOEEvent::COL_NAME_LOCATION_NAME );

		// Execute the SQL.
		std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

		m_curMaxPkey = latestKey;
		//m_latestDatapointPkey = latest.datapointPkey;

		for( unsigned int i=0; i< data->getNumRows(); i++ )
		{
			unsigned long pkey=data->getUnsignedLongData(i,SOEEvent::COL_NAME_PKEY);
			SOEEvent* evt = new SOEEvent( pkey );
			evt->setSourceTime( data->getTimestampData( i, SOEEvent::COL_NAME_SOURCE_TIME ) );
			evt->setTransactiveTime( data->getTimestampData( i, SOEEvent::COL_NAME_TRANSACTIVE_TIME ) );
			evt->setAssetName( data->getStringData( i, SOEEvent::COL_NAME_ASSET_NAME ) );
			evt->setDisplayValue( data->getStringData( i, SOEEvent::COL_NAME_DISPLAY_VALUE ) );
			evt->setDescription( data->getStringData( i, SOEEvent::COL_NAME_DESCRIPTION ) );	
			evt->setLocationName( data->getStringData( i, SOEEvent::COL_NAME_LOCATION_NAME ) );				
			newEvents.push_back(evt);
		}

		return newEvents;
	}
	
 	


 
 
	
	
	std::vector<SOEEvent*> SOEAccessFactory::retrieveMoreSOEEvents()
	{
		IDatabase* db = DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Event_Ad, Read);
		IData* pData = NULL;
		std::vector<SOEEvent*> events;
		unsigned long nRows = 0;

		if( db->moreData(pData) && (nRows = pData->getNumRows()) )
		{
			events.reserve(nRows);
			std::auto_ptr<IData> data( pData );
			for( unsigned int i = 0; i<nRows; i++ )
			{
				SOEEvent* evt = new SOEEvent();
				evt->setPKey( data->getUnsignedLongData( i, SOEEvent::COL_NAME_PKEY ) );
				evt->setSourceTime( data->getTimestampData( i, SOEEvent::COL_NAME_SOURCE_TIME ) );
				timeb t;// = data->getTimestampData( i, SOEEvent::COL_NAME_SOURCE_TIME );
				t = data->getTimestampData( i, SOEEvent::COL_NAME_TRANSACTIVE_TIME );
				evt->setTransactiveTime( t );
				evt->setAssetName( data->getStringData( i, SOEEvent::COL_NAME_ASSET_NAME ) );
				evt->setDisplayValue( data->getStringData( i, SOEEvent::COL_NAME_DISPLAY_VALUE ) );
				evt->setDescription( data->getStringData( i, SOEEvent::COL_NAME_DESCRIPTION ) );	
				evt->setLocationName( data->getStringData( i, SOEEvent::COL_NAME_LOCATION_NAME ) );
				
				events.push_back(evt);
			}
		}
		
		return events;
	}

	

// 	std::string SOEAccessFactory::generateTimeRangeSQL(SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection, time_t from, time_t to )
// 	{
// 		std::stringstream str;
// 		char strTime[18];
// 		tm* t;
// 
// 		// from
// 		if( from )
// 		{
// 			t = localtime( &from );
// 			strftime( strTime, 18, "%Y%m%d %H%M%S", t );
// 			str<<" source_time >= TO_TIMESTAMP('"<< strTime <<"','YYYYMMDD HH24MISS') ";
// 		}
// 
// 		// to
// 		if( to )
// 		{
// 			if( from )
// 			{
// 				str<<" AND ";
// 			}
// 			t = localtime( &to );
// 			strftime( strTime, 18, "%Y%m%d %H%M%S", t );
// 			str<<" source_time <= TO_TIMESTAMP('"<< strTime <<"','YYYYMMDD HH24MISS') ";
// 		}
// 
// 		return str.str();
// 	}
// 	
	
// 	std::string SOEAccessFactory::generatePageTimeSQL( std::string strPageTime, bool bAscend )
// 	{
// 		std::stringstream ss;
// 		
// 		if( !strPageTime.empty() )
// 		{
// 			ss<<" source_time "<<(bAscend?">":"<")<<"= TO_TIMESTAMP('"<< strPageTime <<"','YYYY/MM/DD HH24:MI:SS') ";
// 		}
// 		
// 		return ss.str();
// 	}



	std::string SOEAccessFactory::generateLocationSQL( TA_Base_Core::IDatabase& databaseConnection,const std::vector<unsigned long>& locations )
	{
		//std::stringstream str;
		
		std::string locationExpr = generateORList("location_key", locations, databaseConnection );

		/*if( !locations.empty() )
		{
			str<<" ( ";
		}
		for( std::vector<unsigned long>::iterator it = locations.begin(); it != locations.end(); it++ )
		{
			str<<" location_key =  "<<*it;
			if( (it+1) != locations.end() )
			{
				str<<" OR ";
			}
		}
		if( !locations.empty() )
		{
			str<<" ) ";
		}*/
		
		return locationExpr.c_str();
	}

	std::string SOEAccessFactory::generateAssetSQL(TA_Base_Core::IDatabase& databaseConnection, std::string strAsset )
	{
		std::stringstream str;
		std::transform( strAsset.begin(), strAsset.end(), strAsset.begin(),
			::tolower );

		if( !strAsset.empty() )
		{
			str<<" lower(asset_name) LIKE '%"<<databaseConnection.escapeInsertString(strAsset)<<"%' ";
		}
		return str.str();
	}

	std::string SOEAccessFactory::generateDisplayValueSQL(TA_Base_Core::IDatabase& databaseConnection, std::string strDisplayValue )
	{
		std::stringstream str;	 
		std::transform( strDisplayValue.begin(), strDisplayValue.end(), strDisplayValue.begin(),
			::tolower );
		if( !strDisplayValue.empty() )
		{
			str<<" lower(display_value) LIKE '%"<<databaseConnection.escapeInsertString(strDisplayValue)<<"%' ";
		}
		return str.str();
	}
		



// 	std::string SOEAccessFactory::generateSortingSQL(SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection, std::string strSortColumn, bool bAscend )
// 	{
// 		std::stringstream ss;
// 
// 		if( strSortColumn.empty() )
// 		{
// 			ss<<" order by pkey desc";
// 		}
// 		else
// 		{
// 			ss<<" order by "<<strSortColumn<<(bAscend?"":" desc ");
// 		}
// 
// 		return " order by pkey desc ";
// 		return ss.str();
// 	}

	std::string SOEAccessFactory::generateEventIDBoundSQL(double bottomEventId, bool bAscend)
	{
		std::stringstream ss;
		if(bottomEventId > 0){
			//ss << " PKEY " << ( bAscend ? ">" : "<" ) << "=" << bottomEventId ;
			ss << " PKEY " << "<=" << bottomEventId ;
		}
		return ss.str();
	}

 

	std::string SOEAccessFactory::getLocalDatabaseName()
	{
        if (m_strLocalDatabaseName.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
                DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Event_Ad, Write);
                m_strLocalDatabaseName = connections[0].first;  
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection information");
            }

        }
        return m_strLocalDatabaseName;
	}

}	// namespace TA_Base_Core

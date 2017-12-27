#if !defined(AFX_HISTORICALDATALOADERCPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_HISTORICALDATALOADERCPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * HistoricalDataLoader.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoricalDataLoader.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is a loader for retrieving historical data from the database.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>

#include "app/trending/trend_viewer/src/HistoricalDataLoader.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/TrendConfigDatabaseAccess.h"
#include "app/trending/trend_viewer/src/Utilities.h" //TD17718

#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/trends/src/TrendAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // retrieveDataFromDatabase
    //
    PlotData HistoricalDataLoader::retrieveDataFromDatabase(unsigned long entityKey, time_t startTime, time_t endTime, 
		                                         const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("retrieveDataFromDatabase");

		//TD17883 - defined a constant for the boundary increment
        unsigned long boundaryIncrement = (samplePeriod == LIVE_FIELD_UPDATES) ? GRAPH_BOUNDARY_INCREMENT : 0; 

        // Convert times to UTC. Also increase the range boundaries 
        // to ensure that data is retrieved for the start and end times.
		//TD17883 - time stored by PMS agent is in GMT format, so no need to convert to UTC
		// when retrieving data 
        //time_t temp = startTime - boundaryIncrement;
        //time_t start = TimeFunction::getInstance().ConvertToUTCTimeT(temp);
        //temp = endTime;// + boundaryIncrement;
        //time_t end = TimeFunction::getInstance().ConvertToUTCTimeT(temp);
		startTime -= (samplePeriod + boundaryIncrement);
		endTime += (samplePeriod + boundaryIncrement);

        COleDateTime t1(startTime);

        // change to date format 		
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);  //TD12474        
        CString startString = t1.Format(dateFormatStr.c_str()); 
        COleDateTime t2(endTime);
        CString endString = t2.Format(dateFormatStr.c_str());

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Retrieveing data between %s and %s ", startString.GetBuffer(0), endString.GetBuffer(0));
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Range in time_t format:  %ld - %ld (time_t)", startTime, endTime);

        // The SQL query is written in such a way that the time component of the date/date is decomposed
        // into seconds of the day in order to optimize the retrieval time.
        
        PlotData data;
		DayComponents dayComponents = decomposeTimes(startTime, endTime); 

		//TD17883++ 
		//std::string sql = generateSql(dayComponents,entityKey,typeIdentifier, columnName, samplePeriod);
		//data = executeSql(sql, entityKey, startTime, endTime, samplePeriod);
		

		TA_Base_Core::SQLStatement strSql;
		generateSql(strSql, dayComponents,entityKey,typeIdentifier, columnName, samplePeriod);
		data = executeSql(strSql, entityKey, startTime, endTime, samplePeriod);
		//++TD17883

        FUNCTION_EXIT;
        return data;
    } 
 
    //
    // TD17884 - executeSql
    //
    PlotData HistoricalDataLoader::executeSql(const TA_Base_Core::SQLStatement& sql, unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("executeSql");		

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Executing query: %s", sql.c_str());//TD17935 

        std::vector<std::string> columnNames;
        columnNames.push_back("Timestamp");
        columnNames.push_back("Value");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);
        TA_Base_Core::IData* dbData = executeQuery(databaseConnection, sql, columnNames); //TD17884

        PlotData theData;
        if(dbData == NULL)
        {
            // Don't continue if the query couldn't be executed
            FUNCTION_EXIT;
            return theData;
        }

        // TD 9782: If there is not data within the given range, retrieve the last known value (we know there will
        // definately be a value at midnight and plot this point, as well as this value at the latest time in the
        // given range. This way, there will definately be a line in the graph.		
		//TD17883++
		int numRows = dbData->getNumRows();		
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Retrieving %d data", numRows); //TD17935 

        if(numRows == 0) 
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No data was retrieved from the given range");	
			theData = analyzeNoHistoricalData(entityKey,startTime, endTime, samplePeriod);
			FUNCTION_EXIT;
			return theData;
        }       
	 
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);  
        try
        {			
			double value = 0.0;
			double prevValue = 0.0;
			bool bNonNullValueFound = false;
			std::ostringstream log;
	 
			PlotData tempData;
			
            do
            {
				//TD17884 - reimplemented fix for TD14799  					
                for(unsigned int i = 0; i < dbData->getNumRows(); ++i)
                {
					time_t timestamp = dbData->getDateData(i, "Timestamp");					
					
					if (!(dbData->isNull(i,"Value")))
					{
						value = dbData->getRealData(i, "Value");	 
						prevValue = value;
						bNonNullValueFound = true;
					}
					else
					{
						value = prevValue;
					}
					// do not add the point unless it's the first non-null value retrieved 
					// from the database
					if (bNonNullValueFound)
					{
						Point point;
						point.x = timestamp;
						point.y = value;					
						tempData.push_back(point);	
					}
                }

				delete dbData; 
                dbData = NULL;
				//++TD17884

            } while(databaseConnection->moreData(dbData));

			theData = analyzeHistoricalData(tempData, entityKey, startTime, endTime, samplePeriod);

			if (theData.empty())
			{
				// all values within the given range has either not changed or 
				// no data was stored into the database
				// only one way to find out, retrieved last stored data
				theData = analyzeNoHistoricalData(entityKey, startTime,endTime,samplePeriod);					
			}
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: %s", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: Unexpected error");
        }

		delete dbData; //TD17884
        dbData = NULL;
		//++TD17883
	
        FUNCTION_EXIT;
        return theData;
    }

    PlotData HistoricalDataLoader::analyzeNoHistoricalData(unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod)
	{ 
		FUNCTION_ENTRY("analyzeNoHistoricalData");

		PlotData theData;		
			
		double oldestStoredValue = 0.0;
		try
		{
			oldestStoredValue = retrieveLastKnownValue(entityKey, startTime);			
		}
		catch(TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
			// if there was no stored value in the database, then the data must be an archived data
            if(ex.getType() == TA_Base_Core::TrendViewerException::NO_DATA_RETRIEVED)
			{
				// if not in DR_DATAPOINT_DATA, must reload some archived data
				// then reload data in database		 
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No valid data could be retrieved in database.");
			}
			FUNCTION_EXIT;	 
			return theData;
		}

		if (samplePeriod == LIVE_FIELD_UPDATES)
		{
			// if we have live field updates, plot the last known values on the start and end time
			// recorded given
			Point startPoint;
			startPoint.x = startTime;
			startPoint.y = oldestStoredValue;
			theData.push_back(startPoint);
			logPlotTime("No_Data dummy start time", startTime);
			Point endPoint;
			endPoint.x = endTime;
			endPoint.y = oldestStoredValue;
			theData.push_back(endPoint);				
			logPlotTime("No_Data dummy end time", endTime);
		}
		else
		{
			//mimic the last known values such that it is plotted in the graph on the given sample rates
			logPlotTime("No_Data dummy start time", startTime);
			fillPlotData(theData, oldestStoredValue, startTime, endTime, samplePeriod);		
		}		
	
		FUNCTION_EXIT;	 
		return theData;
	}

	PlotData HistoricalDataLoader::analyzeHistoricalData(PlotData restoredData, unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod)
	{
		FUNCTION_ENTRY("analyzeHistoricalData");

		// the restored data must not be empty
		PlotData theData;
		PlotData::iterator pIter = restoredData.begin();	  

        // change to date format 
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);  

		if (pIter != restoredData.end())
		{
			Point p = (*pIter);

			bool bPrevValueFound = true;
			time_t firstStoredTime = (time_t)p.x;
			double prevValue = 0.0;
			try
			{
				//search for values stored older than the first given point
				prevValue = retrieveLastKnownValue(entityKey, firstStoredTime);
			}
			catch(TA_Base_Core::TrendViewerException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
				if(ex.getType() == TA_Base_Core::TrendViewerException::NO_DATA_RETRIEVED)
				{
					// if there was no stored value in the database, then the current point 
					// is the first value recorded in the database, no line/data should be displayed
					// before it, otherwise, data must be restored thru archive manager					
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No previous data could be retrieved in database.");
					bPrevValueFound = false;
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Unknown Error occured.");
				bPrevValueFound = false;
			}

			if (bPrevValueFound == true)
			{				
				//before creating the point data of the first retrieved datapoint update,
				//must first create a dummy point that would connect the value of the datapoint
				//from the given startTime to the first recorded update of that datapoint
				if (samplePeriod == LIVE_FIELD_UPDATES)
				{
					Point prevPoint;
					prevPoint.x = startTime;
					prevPoint.y = prevValue;
					theData.push_back(prevPoint);
					logPlotTime("creating point at dummy start time for live field updates", startTime);
					
				}
				else
				{   // if sample period is not zero, then mimic a behavior such that the 
					// previous points were retrieved by its sample rates.
					
					if (startTime < firstStoredTime)
					{					
						logPlotTime("creating point at dummy start time", startTime);
						fillPlotData(theData, prevValue, startTime, firstStoredTime, samplePeriod);
					}
				}
			}
			
			

			// after drawing the dummy point, store the mirrored point
			// and the rest of the retrieved datapoint updates
			time_t lastRecordedTime = 0;
			double lastValueToStore = 0;
			do
			{				
				p = *(pIter++);
				lastRecordedTime = (time_t)p.x;
				lastValueToStore = p.y;
				theData.push_back(p);				
			}while (pIter != restoredData.end());


			if (samplePeriod == LIVE_FIELD_UPDATES)
			{
				// for live field updates, there is only one point that connects the 
				// last restored value to the current time
				Point endPoint;
				endPoint.x = endTime;
				endPoint.y = lastValueToStore;
				theData.push_back(endPoint);
			}
			else
			{				
				// if the given endTime is less than the actual last recorded time for the given entity,
				// we are retrieving non-live historical data, so we must show data upto the given end time,
				// thus, no need to add some dummy points to the current end time
				// otherwise, we are displaying live updates, so we must show only up to the 
				// current time	
				if (lastRecordedTime < endTime)
				{			 
					logPlotTime("creating points from last recorded time", lastRecordedTime);
					fillPlotData(theData, lastValueToStore, lastRecordedTime, endTime, samplePeriod);
					logPlotTime("up to the end time", endTime);
				}
			}
		}
		FUNCTION_EXIT;
		return theData;
	}

    void HistoricalDataLoader::fillPlotData(PlotData &theData, double valueToStore, time_t startTime, time_t endTime, unsigned long samplePeriod)
	{
        FUNCTION_ENTRY("fillPlotData");				
		
		// should not fill some data for sample period == zero (or Live field updates)
		// otherwise, infinite loop will occur
		time_t curTime = (samplePeriod != 0) ?  startTime : endTime;
		Point point;
		
		while (curTime < endTime)	
		{	point.x = (double)curTime;
			point.y = valueToStore;
			theData.push_back(point);			
			curTime += samplePeriod;
		} 
		FUNCTION_EXIT;
	}

	void HistoricalDataLoader::generateSql(TA_Base_Core::SQLStatement& strSql,DayComponents& dayComponents, unsigned long entityKey, 
												const std::string& typeIdentifier,const std::string& columnName,
												unsigned long samplePeriod)
	{	
		FUNCTION_ENTRY("generateSql");
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);

		
        DayComponents::iterator it = dayComponents.begin();	 
		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat1b);//"DD/MM/YYYY"
        std::string tableName = getTableName(typeIdentifier, samplePeriod);

        std::ostringstream sqlInput;
        //sql << "SELECT * FROM (";  
		TA_Base_Core::SQLTypeAdapter sqlParams;  
        for(; it != dayComponents.end(); ++it)
        {
            if(it != dayComponents.begin())
            {
                //sql << " UNION ALL";
                sqlParams.add(std::string(" UNION ALL"));
            }

            //sql << " SELECT TO_CHAR(STARTDATETIME, 'YYYYMMDDHH24MISS'), ";
            sqlParams.add(std::string(" SELECT "));
            sqlParams.add(std::string(" STARTDATETIME "), TA_Base_Core::enumDateFormat_4, TA_Base_Core::enumFuncFormat_10);
           /* sql << columnName;
            sql << " FROM ";
            sql << tableName;*/
            sqlParams.add(columnName + std::string(" FROM ") + tableName + std::string(" WHERE ("));
         /*   sql << " WHERE (SOURCEDATE = TO_DATE('";
            sql << it->date;
            sql << "','";
			sql << "DD/MM/YYYY";
			sql << "') AND STARTSECOND BETWEEN ";*/
			sqlParams.add(std::string("SOURCEDATE"), it->date, TA_Base_Core::enumDateFormat_8, TA_Base_Core::enumFuncFormat_2, TA_Base_Core::enumOperKey_EQUAL);
			sqlParams.add(std::string(" AND STARTSECOND BETWEEN "));
            sqlInput << it->startSeconds;             
            sqlInput << " AND ";
            sqlInput << it->endSeconds;
            sqlInput << ") AND ENTITYKEY = ";
            sqlInput << entityKey;
            sqlParams.add(sqlInput.str());
            sqlInput.str() = "";
        }
        //sql << ") WHERE ROWNUM <= ";
		//sql << Utilities::getMaxPointsToDisplay();
        //sql << " ORDER BY 1";
        databaseConnection->prepareSQLStatement(strSql, DR_DATAPOINT_DATA_SELECT_78003,
			  sqlParams, Utilities::getMaxPointsToDisplay());
        
		FUNCTION_EXIT;

		return ;
	}

    //
    // decomposeTimes
    //
    DayComponents HistoricalDataLoader::decomposeTimes(time_t startTime, time_t endTime)
    {
        TA_ASSERT(endTime > startTime, "Invalid range: endTime < startTime");

        DayComponents dayComponents;
        
        CTime startCTime(startTime);
        int startYear = startCTime.GetYear();
        int startMonth = startCTime.GetMonth();
        int startDay = startCTime.GetDay();

        // Only want the date so set hour, minute and second parameters to 0
        CTime startTimeDayOnly(startYear, startMonth, startDay, 0, 0, 0);

        int startDiffSeconds = startTime - startTimeDayOnly.GetTime();
        
        return getDayComponents(startTime, endTime, startDiffSeconds);
    }


    //
    // decomposeTimes
    //
    std::string HistoricalDataLoader::getTableName(const std::string& typeIdentifier, unsigned long samplePeriod)
    { 
        return TA_Base_Core::TrendAccessFactory::getInstance().getSamplePeriodTableName(typeIdentifier, samplePeriod);
    }


    DayComponents HistoricalDataLoader::getTimeFromMidnight(time_t startTime, time_t endTime)
    {
        TA_ASSERT(endTime > startTime, "Invalid range: endTime < startTime");

        return getDayComponents(startTime, endTime, 0);
    }


    DayComponents HistoricalDataLoader::getDayComponents(time_t startTime, time_t endTime, long startDiffSeconds)
    {
        TA_ASSERT(endTime > startTime, "Invalid range: endTime < startTime");

        DayComponents dayComponents;
        
        CTime startCTime(startTime);
        int startYear = startCTime.GetYear();
        int startMonth = startCTime.GetMonth();
        int startDay = startCTime.GetDay();

        // Only want the date so set hour, minute and second parameters to 0
        CTime startTimeDayOnly(startYear, startMonth, startDay, 0, 0, 0);

        CTime endCTime(endTime);
        int endYear = endCTime.GetYear();
        int endMonth = endCTime.GetMonth();
        int endDay = endCTime.GetDay();

        // Only want the date so set hour, minute and second parameters to 0
        CTime endTimeDayOnly(endYear, endMonth, endDay, 0, 0, 0);

        int endDiffSeconds = endTime - endTimeDayOnly.GetTime();

        //const char* DATE_FORMAT = "%d/%m/%Y"; //TD12474 
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat1);  //TD12474
        
        if(startYear == endYear && startMonth == endMonth && startDay == endDay)
        {
            // Same day
            OneDayData oneDay;

			// TD12474 ++
			//oneDay.date = startTimeDayOnly.Format(DATE_FORMAT).GetBuffer(0);
			oneDay.date = startTimeDayOnly.Format(dateFormatStr.c_str()).GetBuffer(0);
			// ++ TD12474

            oneDay.startSeconds = startDiffSeconds;
            oneDay.endSeconds = endDiffSeconds;

            dayComponents.push_back(oneDay);
            return dayComponents;
        }

        // Add start day to vector
        OneDayData startDayData;

		// TD12474 ++
		//startDayData.date = startTimeDayOnly.Format(DATE_FORMAT).GetBuffer(0);
		startDayData.date = startTimeDayOnly.Format(dateFormatStr.c_str()).GetBuffer(0);
		// ++ TD12474

        startDayData.startSeconds = startDiffSeconds;
        startDayData.endSeconds = SECONDS_IN_DAY;

        dayComponents.push_back(startDayData);

        int numberOfDaysInBetween = ((endTimeDayOnly.GetTime() - startTimeDayOnly.GetTime()) / SECONDS_IN_DAY) - 1;

        // Add days in between to vector
        time_t newTime = startTimeDayOnly.GetTime();
        for(int i = 0; i < numberOfDaysInBetween; ++i)
        {
            newTime += SECONDS_IN_DAY;
            CTime theTime(newTime);
            
            OneDayData oneDay;

			// TD12474 ++
			//oneDay.date = theTime.Format(DATE_FORMAT).GetBuffer(0);
			oneDay.date = theTime.Format(dateFormatStr.c_str()).GetBuffer(0);
			// ++ TD12474

            oneDay.startSeconds = 0;
            oneDay.endSeconds = SECONDS_IN_DAY;

            dayComponents.push_back(oneDay);
        }

        // Add end day to vector
        OneDayData endDayData;

		// TD12474 ++
		//endDayData.date = endTimeDayOnly.Format(DATE_FORMAT).GetBuffer(0);
		endDayData.date = endTimeDayOnly.Format(dateFormatStr.c_str()).GetBuffer(0);
		// ++ TD12474 

        endDayData.startSeconds = 0;
        endDayData.endSeconds = endDiffSeconds;

        dayComponents.push_back(endDayData);

        return dayComponents;
    }

	TA_Base_Core::IData* HistoricalDataLoader::executeQuery(TA_Base_Core::IDatabase* dbConnection, const TA_Base_Core::SQLStatement& sql, std::vector<std::string>& columnNames)
	{
		//TD17935 
		FUNCTION_ENTRY("executeQuery");
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Executing query: %s", sql.c_str());
		
		std::string strExSql;
		int nExDbType= dbConnection->getDbServerType();
		switch (nExDbType)
		{
		case TA_Base_Core::enumOracleDb:
			strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
			break;
		case TA_Base_Core::enumMysqlDb:
			strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
			break;		
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Executing query: %s", strExSql.c_str());

		//++TD17935 

        TA_Base_Core::IData* dbData = NULL;
        try
        {
            dbData = dbConnection->executeQuery(sql, columnNames);
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
			delete dbData;
			dbData = NULL;
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: %s", ex.what());
        }
        catch(...)
        {
			delete dbData;
			dbData = NULL;
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: Unexpected error");
        }

		FUNCTION_EXIT; //TD17935 

		return dbData;
	}

	/* 
    * TD17883 - modified to include a reference time to retrieve any previously stored datapoint value
	* TD17884  - retrieveLastKnownValue
	*/
	double HistoricalDataLoader::retrieveLastKnownValue(unsigned long entityKey, time_t startTime /*= 0*/)
	{
		FUNCTION_ENTRY("retrieveLastKnownValue");

		double oldestStoredValue = 0.0;

		//delete by lin
		//std::ostringstream sql;
		
		TA_Base_Core::IDatabase* dbConnection =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);
		std::vector<std::string> columnNames;
        columnNames.push_back("Value");		
		
		std::string strMarkTime = TimeFunction::getInstance().ConvertToString(startTime);
		//TD17935 - rephrase sql statement since current one is too slow
		//sql << "SELECT VALUE FROM ";
		//sql << "(SELECT * FROM DR_DATAPOINT_DATA WHERE  ENTITYKEY = " << entityKey;
		//sql << " AND SOURCETIME < TO_DATE('" << strMarkTime << "','YYYYMMDDHH24MISS')";
		//sql << " ORDER BY SOURCETIME DESC) WHERE ROWNUM = 1";
		
		//delete by lin
		//sql << "SELECT VALUE FROM DR_DATAPOINT_DATA WHERE SOURCETIME = (SELECT MAX(SOURCETIME) FROM DR_DATAPOINT_DATA"; 
		//sql << " WHERE SOURCETIME < TO_DATE('"<< strMarkTime <<"','YYYYMMDDHH24MISS') AND ENTITYKEY="<< entityKey << ")"; 
		//sql << " AND ENTITYKEY = " << entityKey;
		//++TD17935
		//TA_Base_Core::IData* dbData = executeQuery(dbConnection, sql.str(), columnNames);
		

		TA_Base_Core::SQLStatement strSql;
		dbConnection->prepareSQLStatement(strSql, DR_DATAPOINT_DATA_SELECT_78001,
			  strMarkTime,	entityKey, entityKey);
		TA_Base_Core::IData* dbData = executeQuery(dbConnection, strSql, columnNames);
		
		 
		if ((dbData == NULL) || (dbData->getNumRows() == 0))
		{
			delete dbData;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Datapoint has no recorded value in the database before %s", strMarkTime.c_str());
			TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::NO_DATA_RETRIEVED));
		}

		oldestStoredValue = dbData->getRealData(0, "Value");	 
		//TD17935 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Retrieve last known value = %.2f on %s", oldestStoredValue, strMarkTime);	 
		
		delete dbData;
		dbData = NULL;
		FUNCTION_EXIT;
		return oldestStoredValue;
	}
     
    //
    // retrieveDataFromDatabase
    //
    PlotData HistoricalDataLoader::retrieveDataFromDatabase(unsigned long entityKey, time_t dateTime, const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("retrieveDataFromDatabase");

		//TD-TimeConversion
        // Convert time to UTC
        //time_t newDateTime = TimeFunction::getInstance().ConvertToUTCTimeT(dateTime);

        COleDateTime t(dateTime);

		// TD12474 ++
		//CString timeString = t.Format("%d/%m/%Y %H:%M:00");
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat5);
		CString timeString = t.Format(dateFormatStr.c_str());
		// ++ TD12474

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Retrieveing data at %s", timeString.GetBuffer(0));

        CTime theTime(dateTime); 
        int year = theTime.GetYear();
        int month = theTime.GetMonth();
        int day = theTime.GetDay();

        // Only want the date, so set the hour, minute and second parameters to 0
        CTime theDateOnly(year, month, day, 0, 0, 0);
        
        int hour = theTime.GetHour();
        int minute = theTime.GetMinute();
        int startSecond = hour * SECONDS_IN_HOUR + minute * SECONDS_IN_MINUTE;

        std::string tableName = getTableName(typeIdentifier, samplePeriod);

        // The SQL query is written in such a way that the time component of the date/date is decomposed
        // into seconds of the day in order to optimize the retrieval time.
        
        //delete by lin
        //std::ostringstream sql;
        //sql << "SELECT TO_CHAR(STARTDATETIME, 'YYYYMMDDHH24MISS'), ";
        //sql << columnName;
        //sql << " FROM ";
        //sql << tableName;
        //sql << " WHERE (SOURCEDATE = TO_DATE('";
        //sql << theDateOnly.Format("%d/%m/%Y").GetBuffer(0);
        //sql << "','DD/MM/YYYY') AND STARTSECOND = ";
        //sql << startSecond;
        //sql << ") AND ENTITYKEY = ";
        //sql << entityKey;
        //sql << " ORDER BY 1";
        
        std::ostringstream strParamDate;
        std::string strTheDateOnly;
        strParamDate<< theDateOnly.Format("%d/%m/%Y").GetBuffer(0);
        strTheDateOnly = strParamDate.str();

        FUNCTION_EXIT;
        //return executeSql(sql.str());
        
		//waitforcheck delete by lin
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_DATAPOINT_DATA_SELECT_78002,
			columnName, tableName, strTheDateOnly, startSecond, entityKey);
		
		return executeSql(strSql);
    }

     //
    // executeSql
    //
    PlotData HistoricalDataLoader::executeSql(const TA_Base_Core::SQLStatement& sql)
    {
        FUNCTION_ENTRY("executeSql");

        std::vector<std::string> columnNames;
        columnNames.push_back("Timestamp");
        columnNames.push_back("Value");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);
        TA_Base_Core::IData* dbData = NULL;
        try
        {
            dbData = databaseConnection->executeQuery(sql, columnNames);
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: %s", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: Unexpected error");
        }

        PlotData theData;
        if(dbData == NULL)
        {
            // Don't continue if the query couldn't be executed
            FUNCTION_EXIT;
            return theData;
        }

        // TD 9782: If there is not data within the given range, retrieve the last known value (we know there will
        // definately be a value at midnight and plot this point, as well as this value at the latest time in the
        // given range. This way, there will definately be a line in the graph.
        if(dbData->getNumRows() == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No data was retrieved for the given range");
        }
        
        try
        {
            do
            {
                for(unsigned int i = 0; i < dbData->getNumRows(); ++i)
                {
					// TD14799 azenitha++
					double value = 0.0;
					if (!(dbData->isNull(i,"Value")))
					{
						time_t timestamp = dbData->getDateData(i, "Timestamp");
						value = dbData->getRealData(i, "Value");

						Point point;
						point.x = timestamp;
						point.y = value;
						theData.push_back(point);
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "IAVG is empty in DB...proceed with next data");
					}
					// TD14799 ++azenitha
                }

                dbData = NULL;

            } while(databaseConnection->moreData(dbData));
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: %s", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data: Unexpected error");
        }

        FUNCTION_EXIT;
        return theData;
    }

	void HistoricalDataLoader::logPlotTime(const std::string strLog, const time_t t)
	{		
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);  
		COleDateTime plotTime(t);
		CString startStr = plotTime.Format(dateFormatStr.c_str());				
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s : %s ", strLog.c_str(), (LPCSTR)startStr);		
	}

	void HistoricalDataLoader::logPlottedPoint(const std::string strLog, const time_t timeRecorded, const double value)
	{		
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);  
		COleDateTime plotTime(timeRecorded);
		CString startStr = plotTime.Format(dateFormatStr.c_str());				
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s : time = %s, %.2f", 
			                                 strLog.c_str(), (LPCSTR)startStr, value);		
	}
}

#endif // !defined(AFX_HISTORICALDATALOADERCPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)


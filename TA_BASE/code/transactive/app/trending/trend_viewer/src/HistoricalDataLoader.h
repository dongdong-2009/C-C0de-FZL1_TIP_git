#if !defined(AFX_HISTORICALDATALOADER__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_HISTORICALDATALOADER__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * HistoricalDataLoader.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoricalDataLoader.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements the IDataLoader interface and is a loader for retrieving historical 
  * data from the database.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 


#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "app/trending/trend_viewer/src/IDataLoader.h"

#include "core/utilities/src/DateFormat.h" // TD12474 

//TD17884 
namespace TA_Base_Core
{
	class IDatabase;
	class IData;
}

namespace TA_Base_App
{
    struct OneDayData
    {
        std::string date;
        int startSeconds;
        int endSeconds;
    };

    typedef std::vector<OneDayData> DayComponents;

    //
    // HistoricalDataLoader
    //
    class HistoricalDataLoader : public IDataLoader
    {
    public:
        
        /**
          * HistoricalDataLoader
          * 
          * Constructor.
          */
        HistoricalDataLoader() {}

        /**
          * ~HistoricalDataLoader
          * 
          * Destructor.
          */
        virtual ~HistoricalDataLoader() {}

         /**
          * retrieveDataFromDatabase
          * 
          * Retrieve historical data between the specified times for the given item from the given table.
          *
          * @param name - item name
          * @param startTime - start date/time
          * @param endTime - start date/time
          * @param typeIdentifier - type of the item, eg. DATAPOINT, AIDVOLUME. Used to create the table 
          *        name from which to retrieve historical data
          * @param columnName - Column from which to retrieve data
          * @param samplePeriod - Sample period configured for graph
          *
          * @return PlotData - vector containing historical data (timestamp, value)
          */
        virtual PlotData retrieveDataFromDatabase(unsigned long entityKey, time_t startTime, time_t endTime, const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod);
                
        /**
          * retrieveDataFromDatabase
          * 
          * Retrieve one record of historical data for the given date/time.
          *
          * @param name - item name
          * @param dateTime - date/time for which to receive data
          * @param typeIdentifier - type of the item, eg. DATAPOINT, AIDVOLUME. Used to create the table 
          *        name from which to retrieve historical data
          * @param columnName - Column from which to retrieve data
          * @param samplePeriod - Sample period configured for graph
          *
          * @return PlotData - vector containing historical data (timestamp, value)
          */
        virtual PlotData retrieveDataFromDatabase(unsigned long entityKey, time_t dateTime, const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod);

		

    private:

        HistoricalDataLoader( const HistoricalDataLoader& theHistoricalDataLoader);
        HistoricalDataLoader& operator=(const HistoricalDataLoader &);

		//TD17884 
		TA_Base_Core::IData* executeQuery(TA_Base_Core::IDatabase* dbConnection, const TA_Base_Core::SQLStatement& sql, std::vector<std::string>& columnNames);

        //TD17883 - needed to display trend values correctly
        PlotData executeSql(const TA_Base_Core::SQLStatement& sql);		
		PlotData executeSql(const TA_Base_Core::SQLStatement& sql, unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod);
        DayComponents decomposeTimes(time_t startTime, time_t endTime);
        std::string getTableName(const std::string& typeIdentifier, unsigned long samplePeriod);
        DayComponents getTimeFromMidnight(time_t startTime, time_t endTime);
        DayComponents getDayComponents(time_t startTime, time_t endTime, long startDiffSeconds);
		void fillPlotData(PlotData &theData, double valueToStore, time_t startTime, time_t endTime, unsigned long samplePeriod);
		PlotData analyzeHistoricalData(PlotData restoredData, unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod);
		PlotData analyzeNoHistoricalData(unsigned long entityKey, time_t startTime, time_t endTime, unsigned long samplePeriod);
		void generateSql(TA_Base_Core::SQLStatement& strSql, DayComponents& dayComponents, unsigned long entityKey, 
			 const std::string& typeIdentifier,const std::string& columnName,unsigned long samplePeriod);
		void logPlotTime(const std::string strLog, const time_t t);
		void logPlottedPoint(const std::string strLog, const  time_t timeRecorded, const double value);
 		double retrieveLastKnownValue(unsigned long entityKey, time_t startTime = 0);
		//++TD17883

		TA_Base_Core::DateFormat m_DateFormat; // TD12474
    };
}

#endif // !defined(AFX_HISTORICALDATALOADER__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





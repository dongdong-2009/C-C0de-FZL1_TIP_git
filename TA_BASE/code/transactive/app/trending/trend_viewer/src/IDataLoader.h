#if !defined(IDataLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
#define IDataLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_

/**
  * IDataLoader.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IDataLoader.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between a Generic Item and an actual data loader. The generic item
  * call methods on this interface to retrieve historical data from the database for a specific
  * item at specific times.
  *
  */

#include <vector>
#include <string>
#include <ctime>

namespace TA_Base_App
{
    typedef struct
    {
        double x;
        double y;
    } Point;

    typedef std::vector<Point> PlotData;

    //
    // IDataLoader
    //
    class IDataLoader
    {

    public:

        /**
          * ~IDataLoader
          * 
          * Destructor.
          */
        virtual ~IDataLoader() {}

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
        virtual PlotData retrieveDataFromDatabase(unsigned long entityKey, time_t startTime, time_t endTime, const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod) =0;

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
        virtual PlotData retrieveDataFromDatabase(unsigned long entityKey, time_t dateTime, const std::string& typeIdentifier, const std::string& columnName, unsigned long samplePeriod) =0;		
	
    };

}

#endif // !defined(IDataLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)

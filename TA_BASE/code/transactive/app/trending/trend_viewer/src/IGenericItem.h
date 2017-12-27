#if !defined(IGenericItem_AC2416C7_CF5E_4f16_BC2F_F27119AAC62A__INCLUDED_)
#define IGenericItem_AC2416C7_CF5E_4f16_BC2F_F27119AAC62A__INCLUDED_

/**
  * IGenericItem.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IGenericItem.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between a Graphed Item and an actual Generic Item. Every item 
  * loaded from the database that can be trended will be represented by a Generic Item object that 
  * implements this interface. When an item is added to the Graph, a graphed item will be created which
  * makes calls on this interface to retrieve data specific to the item.
  *
  */

#include <vector>

#include "app/trending/trend_viewer/src/IDataLoader.h"

namespace TA_Base_App
{
    //
    // Forward Declaration
    //
    class IDataUpdateObserver;

    //
    // IGenericItem
    //
    class IGenericItem
    {

    public:

        /**
          * ~IGenericItem
          * 
          * Destructor.
          */
        virtual ~IGenericItem() {}

        /**
          * retrieveHistoricalData
          * 
          * Retrieves historical data from the database between the given times and from the given table.
          *
          * @param startDateTime - Time from which to retrieve the data
          * @param endDateTime - Data is retrieved to this time
          * @param tableName - Table from which to retrieve the data
          * @param columnName - Column from which to retrieve data
          * @param samplePeriod - Sample period configured for graph
          *
          * @return PlotData - vector of data retrieved (timestamp, value)
          */
        virtual PlotData retrieveHistoricalData(time_t startDateTime, time_t endDateTime, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod) =0;

        /**
          * retrieveHistoricalData
          * 
          * Retrieves historical data from the database between the given times and from the given table.
          *
          * @param time - Time from which to retrieve the data
          * @param tableName - Table from which to retrieve the data
          * @param columnName - Column from which to retrieve data
          * @param samplePeriod - Sample period configured for graph
          *
          * @return PlotData - vector of data retrieved (timestamp, value)
          */
        virtual PlotData retrieveHistoricalData(time_t time, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod) =0;

        /**
          * setLiveMode
          * 
          * Toggle live mode on/off.
          *
          * @param isLive - flag indicating whether graph is in live mode.
          */
        virtual void setLiveMode(bool isLive) =0;

        /**
          * getMinimumLimit
          * 
          * Get the minimum limit of the item.
          *
          * @return double - minimum limit of the item
          */
        virtual double getMinimumLimit() =0;

        /**
          * getMaximumLimit
          * 
          * Get the maximum limit of the item.
          *
          * @return double - maximum limit of the item
          */
        virtual double getMaximumLimit() =0;

        /**
          * getScaleFactor
          * 
          * Get the scale factor of the item.
          *
          * @return double - scale factor of the item
          */
        virtual double getScaleFactor() =0;

        /**
          * getName
          * 
          * Get the name of the item.
          *
          * @return string - name of the item
          */
        virtual std::string getName() =0;


        virtual unsigned long getEntityKey() =0;

        /**
          * isBooleanType
          * 
          * Determine if the item is a boolean type.
          *
          * @return bool - flag indicating whether item is a boolean type
          */
        virtual bool isBooleanType() =0;

        /**
          * getTypeIdentifier
          * 
          * Get the type identifier of the item.
          *
          * @return string - type identifier of the item
          */
        virtual std::string getTypeIdentifier() =0;

        /**
          * getCustomAxisLabel
          * 
          * Get the string used for the custom axis.
          *
          * @return string - custom axis label of the item
          */
        virtual std::string getCustomAxisLabel() =0;

        /**
          * setUpdateObserver
          * 
          * Set the update observer for the generic item.
          *
          * @param updateObserver - pointer to update observer object
          */
        virtual void setUpdateObserver(IDataUpdateObserver* updateObserver) =0;

        /**
          * setStateLabel
          * 
          * Set the state label for a given state for the generic item. If there a label
          * has already been set for a state, it will be overwritten.
          *
          * @param state - a number representing the state
          * @param label - the description of the state
          */
        virtual void setStateLabel(int state, const std::string& label) = 0;

        /**
          * getStateLabels
          * 
          * Get the state labels and their values for the generic item.
          *
          * @return std::map<int, std::string> - state labels, indexed by their value
          *
          */
        virtual std::map<int, std::string> getStateLabels() = 0;

        /**
          * setShouldDisplayLabel
          * 
          * Sets whether the generic item should display string labels instead
          * of the numeric values.
          *
          */
        virtual void setShouldDisplayLabel(bool shouldDisplay) = 0;

        /**
          * getShouldDisplayLabel
          * 
          * Returns whether the generic item should display string labels instead
          * of the numeric values.
          *
          */
        virtual bool getShouldDisplayLabel() const = 0;

        /**
          * getAgentName
          * 
          * Get the agent name for the item.
          *
          * @return string - agent name for the item
          */
        virtual std::string getAgentName() const =0;

        /**
          * getLocationKey
          * 
          * Get the location key for the item.
          *
          * @return string - location key for the item
          */
        virtual unsigned long getLocationKey() const = 0;

        /**
          * getPrecision
          * 
          * Get precision for the item. This indicates to how many decimal places to
          * display the value for the item.
          *
          * @return int - precision for the item
          */
        virtual int getPrecision() const = 0;	 
    };
}

#endif // !defined(IGenericItem_AC2416C7_CF5E_4f16_BC2F_F27119AAC62A__INCLUDED_)

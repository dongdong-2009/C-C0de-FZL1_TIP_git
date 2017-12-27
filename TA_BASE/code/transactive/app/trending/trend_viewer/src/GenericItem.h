#if !defined(GenericItem_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)
#define GenericItem_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_

/**
  * GenericItem.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GenericItem.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class represents an item that can be trended. It stores all the attributes of the item
  * that are needed for the item to be graphed and contains methods to retrieve historical
  * data from the database.
  *
  */

#include <map>

#include "app/trending/trend_viewer/src/IUpdateSubscriber.h"
#include "app/trending/trend_viewer/src/IGenericItem.h"
#include "app/trending/trend_viewer/src/IDataLoader.h"


namespace TA_Base_App
{
    //
    // Forward Declaration
    //
    class IDataUpdateObserver;

    //
    // GenericItem
    //
    class GenericItem : public IGenericItem
    {

    public:

        /**
          * GraphedItem
          * 
          * Constructor.
          *
          */
        GenericItem(unsigned long entityKey, const std::string& itemName, const std::string& typeIdentifier, 
            double minimumLimit, double maximumLimit, const std::string& unitLabel, bool isBooleanType, 
            const std::string& agentName, unsigned long locationKey, int precision, std::auto_ptr<IUpdateSubscriber> subscriber);
            
        /**
          * ~GraphedItem
          * 
          * Destructor.
          *
          */
        virtual ~GenericItem();

        /**
          * retrieveHistoricalData
          * 
          * Retrieves historical data between the given dates (inclusive) from the given table.
          *
          * @param startDateTime - Time from which to retrieve the data
          * @param endDateTime - Data is retrieved to this time
          * @param tableName - Table from which to retrieve the data
          * @param columnName - Column from which to retrieve data
          * @param samplePeriod - Sample period configured for graph
          *
          * @return PlotData - vector of data retrieved (timestamp, value)
          */
        virtual PlotData retrieveHistoricalData(time_t startDateTime, time_t endDateTime, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod);

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
        virtual PlotData retrieveHistoricalData(time_t time, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod);

        /**
          * setLiveMode
          * 
          * Toggle live mode on/off. If live mode is on, subscribers are created for all graphed items.
          *
          * @param isLive - flag indicating whether graph is in live mode
          */
        virtual void setLiveMode(bool isLive);

        /**
          * setUpdateObserver
          * 
          * Set the update observer for the generic item.
          *
          * @param updateObserver - pointer to update observer object
          */
        virtual void setUpdateObserver(IDataUpdateObserver* updateObserver);

        /**
          * setStateLabel
          * 
          * Set the state label for a given state for the generic item. If there a label
          * has already been set for a state, it will be overwritten.
          *
          * @param state - a number representing the state
          * @param label - the description of the state
          */
        virtual void setStateLabel(int state, const std::string& label);

        /**
          * getStateLabels
          * 
          * Get the state labels and their values for the generic item.
          *
          * @return std::map<int, std::string> - state labels, indexed by their value
          *
          */
        virtual std::map<int, std::string> getStateLabels();	   

    public:

        // Inlines

        virtual double getMinimumLimit()
        {
            return m_minimumLimit;
        }

        virtual double getMaximumLimit()
        {
            return m_maximumLimit;
        }

        virtual double getScaleFactor()
        {
            return m_scaleFactor;
        }

        virtual std::string getName()
        {
            return m_name;
        }

        virtual bool isBooleanType()
        {
            return m_isBoolean;
        }

        virtual std::string getTypeIdentifier()
        {
            return m_typeIdentifier;
        }

        virtual std::string getCustomAxisLabel()
        {
            return m_customAxisLabel;
        }

        virtual unsigned long getEntityKey()
        {
            return m_entityKey;
        }

        virtual void setShouldDisplayLabel(bool shouldDisplay)
        {
            m_shouldDisplayLabel = shouldDisplay;
        }
        
        virtual bool getShouldDisplayLabel() const
        {
            return m_shouldDisplayLabel;
        }

        virtual std::string getAgentName() const
        {
            return m_agentName;
        }

        virtual unsigned long getLocationKey() const
        {
            return m_locationKey;
        }

        void setLabelsRetrieved(bool labelsRetrieved)
        {
            m_labelsRetrieved = labelsRetrieved;
        }

        bool getLabelsRetrieved() const
        {
            return m_labelsRetrieved;
        }

        virtual int getPrecision() const
        {
            return m_precision;
        }

    private:

        GenericItem( const GenericItem& theGenericItem);
        GenericItem& operator=(const GenericItem &);

    private:

        std::string m_name;                                     // Name of item
        unsigned long m_entityKey;                              // Entity key for item
        std::string m_typeIdentifier;                           // Type Identifier for item retrieved from database
                                                                // and used to determine which table is used to retrieve
                                                                // historical data.

        double m_minimumLimit;                                  // Minimum value of item
        double m_maximumLimit;                                  // Maximum value of item
        double m_scaleFactor;                                   // Scale value used to plot values on percentage axis
        bool m_isBoolean;                                       // Indicates whether the item is of boolean type. These
                                                                // items will be plotted differently as they will only
                                                                // have two values: 0 and 1.
        
        std::auto_ptr<IUpdateSubscriber> m_updateSubscriber;    // Update subscriber for the item - listens for update messages
        std::auto_ptr<IDataLoader> m_dataLoader;                // Data loader for the item - loads historical data from database
        std::string m_customAxisLabel;                          // Label to print on custom axis

        std::map<int, std::string> m_stateLabelMap;             // Maps state value to a label (eg, 0 - FALSE, 1 - TRUE)

        bool m_shouldDisplayLabel;

        std::string m_agentName;
        unsigned long m_locationKey;

        bool m_labelsRetrieved;

        int m_precision;
    };

}

#endif // !defined(GenericItem_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)

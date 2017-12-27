#if !defined(GraphedItem_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_)
#define GraphedItem_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_

/**
  * GraphedItem.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphedItem.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class represents an item that has been added to the graph. It contains a reference
  * to the generic item that it represents and a refresh thread that will update the graph
  * periodically if it is in live mode. This object will be created when an item is added
  * to the graphed and deleted when it is removed.
  *
  */

// TeeChart includes

#include "app/trending/trend_viewer/src/IDataUpdateObserver.h"
#include "app/trending/trend_viewer/src/IGraph.h"
#include "app/trending/trend_viewer/src/IGenericItem.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "boost/smart_ptr.hpp"

#include "cots/TeeChart/tchart.h"
#include <series.h>


namespace TA_Base_App
{
    //
    // Forward declaration
    //
    class IArchiveDataStore;
    class GraphRefreshThread;

    //
    // GraphedItem
    //
    class GraphedItem : public IDataUpdateObserver
    {

    public:

        /**
          * GraphedItem
          * 
          * Constructor.
          *
          */
        GraphedItem(CSeries& series, long seriesNumber, IGraph& graph, 
            IGenericItem& itemData, COLORREF penColour, time_t startDateTime, time_t endDateTime);

        /**
          * ~GraphedItem
          * 
          * Destructor.
          *
          */
        virtual ~GraphedItem(); 
        
        /**
          * emptySeries
          * 
          * Remove all the points from the series. This is generally called before the graph is reloaded.
          */        
        void emptySeries();
        
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
        PlotData retrieveHistoricalData(time_t startDateTime, time_t endDateTime, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod);

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
        PlotData retrieveHistoricalData(time_t time, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod);

        /**
          * setLiveMode
          * 
          * Toggle live mode on/off. If live mode is on, subscribers are created for all graphed items.
          *
          * @param isLive - flag indicating whether graph is in live mode
          * @param samplePeriod - sample period configured for the graph
          */
        void setLiveMode(bool isLive, long samplePeriod);
        
        /**
          * getActualValue
          * 
          * Convert the scaled value (plotted value) to the actual value retrieved from the database.
          *
          * @param value - value plotted on graph.
          *
          * @return double - the actual value
          */
        double getActualValue(double value);

        /**
          * newDataReceived
          * 
          * This method is called when live data has been received. The new value for the given
          * time will be plotted on the graph.
          *
          * @param timestamp - time the data was received
          * @param value - new value received
          * @param status - status of the data received
          */
        virtual void newDataReceived(time_t timestamp, double value, bool status);

        /**
          * updateWithCurrentData
          * 
          * This method is called when it it time to refresh the graph. The last known values
          * are plotted on the graph for the given time, and the graph moved so the new time
          * is visible.
          *
          * @param timestamp - time at which the current data will be plotted
          * @param samplePeriod - sample period configured for the graph
          */
        virtual void updateWithCurrentData(time_t timestamp, long samplePeriod);

    public:

        long getSeriesNumber() const
        {
            return m_seriesNumber;
        }

        COLORREF getPenColour() const
        {
            return m_penColour;
        }

        double getLastXValue() const
        {
            return m_lastXValue;
        }

        double getLastYValue() const
        {
            return m_lastYValue;
        }

        long getCustomAxisNumber() const
        {
            return m_customAxisNumber;
        }

        void setSeriesNumber(long seriesNum)
        {
            m_seriesNumber = seriesNum;
        }

        void setPenColour(COLORREF colour)
        {
            m_penColour = colour;
        }

        void setLastYValue(double y)
        {
            m_lastYValue = y;
        }

        void setLastXValue(double x)
        {
            m_lastXValue = x;
        }

        void setCustomAxisNumber(long axisNumber)
        {
            m_customAxisNumber = axisNumber;
        }

        long getBoolCustomAxisNumber() const
        {
            return m_boolCustomAxisNumber;
        }

        void setBoolCustomAxisNumber(long axisNumber)
        {
            m_boolCustomAxisNumber = axisNumber;
        }
        
        void setBoolSeriesNumber(long seriesNumber)
        {
            m_boolSeriesNumber = seriesNumber;
        }

        long getBoolSeriesNumber() const
        {
            return m_boolSeriesNumber;
        }

        //
        // Generic Item methods
        //

        bool isBooleanType() const
        {
            return m_genericItem.isBooleanType();
        }

        double getMinimumLimit() const
        {
            return m_genericItem.getMinimumLimit();
        }

        double getMaximumLimit() const
        {
            return m_genericItem.getMaximumLimit();
        }

        std::string getName() const
        {
            return m_genericItem.getName();
        }

        unsigned long getEntityKey() const
        {
            return m_genericItem.getEntityKey();
        }

        double getScaleFactor() const
        {
            return m_genericItem.getScaleFactor();
        }

        std::string getTypeIdentifier() const
        {
            return m_genericItem.getTypeIdentifier();
        }
   
        std::string getCustomAxisLabel() const
        {
            return m_genericItem.getCustomAxisLabel();
        }

        std::string getAgentName() const
        {
            return m_genericItem.getAgentName();
        }

        unsigned long getLocationKey() const
        {
            return m_genericItem.getLocationKey();
        }

        void setStateLabel(int state, const std::string& label)
        {
            m_genericItem.setStateLabel(state, label);
        }

        bool getShouldDisplayLabel()
        {
            return m_genericItem.getShouldDisplayLabel();
        }

        std::map<int, std::string> getStateLabels()
        {
            return m_genericItem.getStateLabels();
        }

        int getPrecision() const
        {
            return m_genericItem.getPrecision();
        }


    private:

        GraphedItem();
        GraphedItem( const GraphedItem& theGraphedItem);
        GraphedItem& operator=(const GraphedItem &);

    private:

        COLORREF m_penColour;                                           // Colour of line plotted on graph
        CSeries m_series;                                               // Series object associated with the graphed item
        double m_lastYValue;                                            // Last plotted Y value
        double m_lastXValue;                                            // Last plotted X value
        long m_seriesNumber;                                            // Series number for series associated with the graphed item.
        IGenericItem& m_genericItem;                                    // Generic item associated with graphed item
        IGraph& m_graph;                                                // Graph object this graphed item belongs to
        boost::scoped_ptr<GraphRefreshThread> m_graphRefreshThread;     // Refresh thread that updates the graph periodically
                                                                        // when graph is in live mode
        long m_customAxisNumber;                                        
                                                                        // Custom axis index for custom axis associated
                                                                        // with this Graphed item.

        long m_boolCustomAxisNumber;                                    // Second custom axis for boolean items which displays
                                                                        // the TRUE/FALSE labels.

        long m_boolSeriesNumber;

        TA_Base_Core::ReEntrantThreadLockable m_liveUpdateLock;
        TA_Base_Core::ReEntrantThreadLockable m_refreshLock;
                //TD17883 - no need for this attribute
		//bool m_bUseFieldUpdates; //TD17884
    };
}

#endif // !defined(GraphedItem_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_)

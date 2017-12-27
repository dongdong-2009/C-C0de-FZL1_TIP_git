#if !defined(AbstractGraph_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define AbstractGraph_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * AbstractGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/AbstractGraph.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements IGraph and represents all graph types. This class contains methods to
  * manipulate the graph itself, such as setting the axes ticks and values, adding and
  * removing series' to the graph, adding points to a series, scrolling, zooming etc. The
  * methods are implemented based on the assumption that the x-axis is in date/time format. If
  * this is not the case, the methods will need to be overwritten.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 


#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/IGraph.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IGraphObserver.h"
#include "app/trending/trend_viewer/src/GraphedItem.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"

#include "core/utilities/src/TAAssert.h"

#include "cots/TeeChart/tchart.h" // For TeeChart control

#include "boost/smart_ptr.hpp"

#include <cots/teechart/axis.h>

#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_App
{
    class AbstractGraph : public IGraph
    {

    public:

        /**
          * AbstractGraph
          * 
          * Constructor.
          *
          */
        AbstractGraph(CTChart& chart, const std::string& name, const std::string& description, 
            time_t startTime, time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver) 
            : m_minItems(0),
              m_maxItems(0),
              m_chart(chart), 
              m_graphName(name), 
              m_description(description), 
              m_startTime(startTime), 
              m_endTime(endTime),
              m_isLive(isLive), 
              m_samplePeriod(samplePeriod),
              m_sampleType(DEFAULT_SAMPLE_TYPE), 
              m_itemStore(itemStore), 
              m_archiveDataStore(archiveDataStore), 
              m_graphObserver(graphObserver), 
              m_isGraphZoomed(false), 
              m_lastCustomAxisDisplayed(-1) 
        {
            m_rangeSeconds = m_endTime - m_startTime;
        }

        /**
          * ~AbstractGraph
          * 
          * Destructor.
          *
          */
        virtual ~AbstractGraph();

        /**
          * adjustXAxisTicks
          * 
          * Adjust the minor increment for the label increments.
          */
        virtual void adjustXAxisTicks();

        /**
          * adjustXAxisFormat
          * 
          * Adjust the labels of the X-axis. This could be setting the date/time
          * format, changing it to a percentage scale, or using engineering units.
          */
        virtual void adjustXAxisFormat();

        /**
          * addToGraph
          * 
          * Adds a new line to the graph.
          *
          * @param index - index of item taken from Trend Properties Dialog
          * @param itemName - name of item 
          * @param penColour - colour of the line on the graph
          * @param displayName - display name for the item
          */
        virtual void addToGraph(int index, const std::string& itemName, COLORREF penColour, const std::string& displayName);

        /**
          * deleteFromGraph
          * 
          * Removes a line from the graph.
          *
          * @param index - index of item taken from Trend Properties Dialog
          */
        virtual void deleteFromGraph(int index);

        /**
          * scaleValue
          * 
          * Scale the value based on the minimum and maximum engineering values and
          * whether the item is a boolean type.
          *
          * @param value - raw value
          * @param graphedItem - pointer to graphed representation of item
          *
          * @return scaled value
          */
        virtual double scaleValue(double value, const GraphedItem* graphedItem) const = 0;
        
        /**
          * moveGraph
          * 
          * Moves the graph given that the X-axis is of date/time format.
          *
          * @param timestamp - new time to set the right-hand side of the graph to.
          */
        virtual void moveGraph(time_t timestamp);
        
        /**
          * setLiveMode
          * 
          * Turn live mode on or off.
          *
          * @param isLive - flag to indicate whether live mode is on or off
          * @param newStartTime - new time shown on right mode side of graph
          * @param bAfterFirstLoad - flag to indicate that setLiveMode was called right after the items were added into the graph //TD17935
          */
        virtual void setLiveMode(bool liveState, time_t& newStartTime, bool bAfterFirstLoad = false); //TD17935
        
        /**
          * reloadGraph
          * 
          * Re-read all the trend properties and display the graph again.
          *
          * @exception TrendViewerException - thrown when there was a problem loading 
          * historical data from the database.
          */
        virtual void reloadGraph();

        /**
          * plotPoint
          * 
          * Plot a point on the graph. The graph will also be moved forward to display the 
          * point if the x value (time) is after the latest time shown.
          * NOTE: This implementation is for graphs that have a date/time X-axis. For the
          * graph types that don't, this method will need to be overridden.
          * 
          * @param seriesNumber - Series number of series to add point to
          * @param x - x value
          * @param y - y value
          * @param penColour - colour of point
          */
        virtual void plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour);

        /**
          * displayCursor
          * 
          * Toggles cursor on/off.
          *
          * @param showCursor - flag indicating whether cursor is on or off
          * @param toolNumber - tool number for the cursor
          */
        virtual void displayCursor(bool showCursor, long toolNumber);

        /**
          * onScroll
          * 
          * Called when the graph has been scrolled and returns the new time and range properties.
          * 
          * @param startDateTime - new start date time property
          * @param rangeDays - new range day property
          * @param rangeTime - new range time property
          * @param axis - index of axis that has been scrolled
          */
        virtual void onScroll(time_t& startDateTime, long& rangeSeconds, long axis);

        /**
          * onZoom
          * 
          * Called when the graph has been zoomed or unzoomed.
          * 
          * @param startDateTime - new start date time property
          * @param rangeDays - new range days property
          * @param rangeTime - new range time property
          * @param x0 - x coord of top left corner of drag rectangle
          * @param y0 - y coord of top left corner of drag rectangle
          * @param x1 - x coord of bottom right corner of drag rectangle
          * @param y1 - y coord of bottom right corner of drag rectangle
          * @param reset - indicates whether the zoom is a zoom undo
          */
        virtual void onZoom(time_t& startDateTime, long& rangeSeconds, long x0, long y0, long x1, long y1, bool reset = false);

        /**
          * onCursorChange
          * 
          * Called when the cursor has been moved.
          * This implementation assumes that the X-Axis is of date/time format.
          * If it isn't, this method needs to be overridden.
          * 
          * @param seriesNumber - series number
          * @param x - x value at the cursor
          * @param y - y value at the cursor
          * @param valueIndex - index of the point intersecting with the cursor in the series
          */
        virtual void onCursorChange(long seriesNumber, double x, double y, long valueIndex);

        /**
          * onCursorToolChange
          * 
          * Called when the cursor moves.
          *
          * @param Tool - Index of the tool that has changed
          * @param x - X Screen value
          * @param y - Y screen value
          * @param xVal - x axis value
          * @param yVal - y axis value
          * @param series - series the cursor intersects with
          */
        virtual void onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series);

         /**
          * onClickSeries
          * 
          * Called when a series has been clicked on.
          * 
          * @param seriesNumber - series number
          */
        virtual void onClickSeries(long seriesNumber);

        /**
          * onClickBackground
          * 
          * Called when the background has been clicked on.
          */
        virtual void onClickBackground();

        /**
          * onClickLegend
          * 
          * Called when a series has been toggled on/off in the legend.
          *
          * @param nonActiveSeriesNumber - vector of series numbers of the non active series.
          */
        virtual void onClickLegend(const std::vector<long>& nonActiveSeriesNumbers);

        /**
          * getMaximumLimit
          * 
          * Get the maximum limit of the item associated with the series number.
          *
          * @param seriesNumber - series number
          *
          * @return - double maximum limit of item range
          */
        virtual double getMaximumLimit(long seriesNumber) const;

        /**
          * getScrollableAxesIndexes
          * 
          * Get the indexes of the scrollable axes. A graph can have a number of axes, including
          * the custom axes and of these axes have an index. However we only want to limit
          * which axes can be scrolled. This method will return the axes we have allowed to be
          * scrolled. Currently, it is just the bottom default axis (time axis).
          *
          * @return - indexes of scrollable axes
          */
        virtual std::vector<long> getScrollableAxesIndexes() const;

        /**
          * setPenColour
          * 
          * Set the pen colour for each item in the trend.
          *
          * @param penColour - RBG (hex) value for the colour
          * @param index - index for the item taken from the Trend Properties Dialog
          */
        void setPenColour(COLORREF penColour, int index);

        /**
          * adjustSeriesNumber
          * 
          * Adjust the series numbering for series added after the deleted one.
          *
          * @param seriesNum - New series number
          */
        void adjustSeriesNumbers(long seriesNum);

        /**
          * setTitle
          * 
          * Set the title of the graph
          *
          * @param title - New title for the graph
          */
        void setTitle(const std::string& title);

        /**
          * setUpDummySeries
          * 
          * This method sets up a dummy series to associate with the given custom axis
          * and adds to points to the series so that the labels will show on the Y axis.
          *
          * @param customAxisNum - Index of the custom y axis
          * @param graphedItem - Pointer to graphed item object the dummy series is
          *        associated with.
          * @param startTime - the left most time in the graph
          * @param rangeSeconds - the range currently displayed in seconds
          */
        virtual void setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds) = 0;

        
    protected:

        /**
          * adjustCustomAxisNumbers
          * 
          * Adjust the numbering for custom axes added after the deleted one.
          *
          * @param axisIndex - Axis index number
          */
        void adjustCustomAxisNumbers(long axisIndex);

        /**
          * createNewItem
          * 
          * Creates a new Graphed item for a new item that has been added to the graph.
          *
          * @param index - Index of the item taken from the trend properties dialog.
          * @param itemName - Name of the item
          * @param penColour - Colour configured for item
          * @param startDateTime - Start date/time configured for graph
          * @param endDateTime - End date/time of graph
          */
        virtual void createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName) = 0;

        /**
          * removeItem
          * 
          * Remove an item from the graph.
          *
          * @param index - AIndex of item taken from the trend properties dialog.
          */
        virtual void removeItem(int index) = 0;

        /**
          * plotData
          * 
          * Retrieves the historical data for the graphed item between the times configured 
          * for the graph and plots the values.
          *
          * @param graphedItem - Pointer to the graphed item object representing the item
          */
        virtual void plotData(GraphedItem* graphedItem);

        /**
          * nullValue
          * 
          * Looks at the latest time and compares it to the time of the last plotted point.
          * If the gap is too large, it will break the line.
          *
          * @param latestTime - Time of the new point
          * @param seriesNumber - Series number of the series the point is added to
          * @param samplePeriod - Sample period configured for the graph
          */
        virtual void nullValue(time_t latestTime, long seriesNumber, long samplePeriod);

        /**
          * addCustomAxis
          * 
          * Adds a custom axis for a graphed item.
          *
          * @param index - Index of the graphed item
          */
        virtual void addCustomAxis(int index);

        /**
          * getGraphedItem
          * 
          * Retrieve a pointer to a graphed item object associated with a series.
          *
          * @param seriesNumber - Index of the series on the graph
          *
          * @return Pointer to grpahed item object
          */
        GraphedItem* getGraphedItem(long seriesNumber) const;

        /**
          * isSeriesVisible
          * 
          * Determine if the series is visible or not (if it has been turned off via the legend)
          *
          * @param seriesNumber - Index of the series on the graph
          *
          * @return True: Series is visible, False: Series is not visible
          */
        bool isSeriesVisible(long seriesNumber);

        /**
          * getAxisLabel
          * 
          * Retrieve the corresponding label to display on the Y axis for the given series and value
          * index.
          *
          * @param axis - Axis index of custom axis to label
          * @param seriesIndex - Series index of series corresponding to custom axis
          * @param valueIndex - Value index of value in series
          *
          * @return Engineering unit label
          */
        virtual std::string getAxisLabel(long axis, long seriesIndex, long valueIndex);

        /**
          * adjustCustomAxisForBoolean
          * 
          * Adjust the custom axis for a boolean item after a zoom or scroll event.
          *
          */
        virtual void adjustCustomAxisForBoolean(double minYValue, double maxYValue, const GraphedItem* graphedItem, CAxis& theAxis);

        void setUpCustomAxis(CAxis& theAxis, long index);

    public:

        //
        // Inlines
        //
        
        virtual void setStartDateTime(time_t startDateTime)
        {
            m_startTime = startDateTime;
        }

        void setStartTime(time_t startTime)
        {
            m_startTime = startTime;
        }
        
        time_t getStartTime() const
        {
            return m_startTime;
        }
        
        bool isLive() const
        {
            return m_isLive;
        }
        
        void setSamplePeriod(long samplePeriod)
        {
            m_samplePeriod = samplePeriod;
        }
        
        long getSamplePeriod() const
        {
            return m_samplePeriod;
        }
        
        void setGraphName(const std::string& name)
        {
            m_graphName = name;
        }
        
        std::string getGraphName() const
        {
            return m_graphName;
        }
        
        void setDescription(const std::string& description)
        {
            m_description = description;
        }
        
        std::string getDescription() const
        {
            return m_description;
        }

        void setEndTime(time_t endTime)
        {
            m_endTime = endTime;
        }

        time_t getEndTime() const
        {
            return m_endTime;
        }

        long getRangeSeconds() const
        {
            return m_rangeSeconds;
        }
    
    protected:

        // Inlines

        CTChart& getChart() const
        {
            return m_chart;
        }

        IItemStore* getItemStore() const
        {
            return m_itemStore;
        }

        IArchiveDataStore* getArchiveDataStore() const
        {
            return m_archiveDataStore;
        }

        IGraphObserver* getGraphObserver() const
        {
            return m_graphObserver;
        }

        bool isGraphZoomed() const
        {
            return m_isGraphZoomed;
        }

        void setZoom(bool isZoomed)
        {
            m_isGraphZoomed = isZoomed;
        }

        std::string getSampleType() const
        {
            return m_sampleType;
        }

        long getLastCustomAxisDisplayed() const
        {
            return m_lastCustomAxisDisplayed;
        }

        void setLastCustomAxisDisplayed(long axisNumber)
        {
            m_lastCustomAxisDisplayed = axisNumber;
        }

        
      
    private:

        AbstractGraph();
        AbstractGraph( const AbstractGraph& abstractGraph);
        AbstractGraph& operator=(const AbstractGraph &);

        
        
    protected:

        unsigned int m_minItems;
        unsigned int m_maxItems;
        std::map<int, boost::shared_ptr<GraphedItem> > m_graphedItems;

    private:

        CTChart& m_chart;                       // Reference to the chart object
        std::string m_graphName;                
        std::string m_description;
        time_t m_startTime; 
        time_t m_endTime;
        long m_rangeSeconds;
        bool m_isLive;
        long m_samplePeriod;
        std::string m_sampleType;               // Type of sample: average, minimum or maximum
        IItemStore* m_itemStore;                // Holds all the trendable items
        IArchiveDataStore* m_archiveDataStore;  // Holds all archived data dates that have been restored
        IGraphObserver* m_graphObserver;        
        bool m_isGraphZoomed;
        long m_lastCustomAxisDisplayed;
        
        static TA_Base_Core::ReEntrantThreadLockable m_graphLock;

		TA_Base_Core::DateFormat m_DateFormat; // TD12474
    };
}

#endif // !defined(AbstractGraph_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)

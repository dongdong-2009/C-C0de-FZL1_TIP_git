#if !defined(ComparisonGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define ComparisonGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * ComparisonGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ComparisonGraph.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from AbstractGraph and represents a Comparison Graph. Some AbstractGraph
  * methods have been overwritten especially for a Comparison Graph. The main difference is the 
  * Comparison Graph has two x-axes and two y_axes. The set-up and plotting of points has been
  * changed to accomodate for this.
  *
  */

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/AbstractGraph.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "cots/teechart/tchart.h" // For TeeChart control

#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_App
{
    class ComparisonGraph : public AbstractGraph
    {

    public:

        /**
          * ComparisonGraph
          * 
          * Constructor.
          *
          */
        ComparisonGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
            time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, time_t offset, int leftChartMargin,
            long cursorToolIndex, long axisArrowToolIndex);

        /**
          * ~ComparisonGraph
          * 
          * Destructor
          *
          */
        virtual ~ComparisonGraph();

        /**
          * setUpGraph
          * 
          * Retrieves all the graph properties and sets up the line graph.
          */
        virtual void setUpGraph();

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
          * updatePenStyle
          * 
          * Update the pen styles for each graphed item.
          *
          * @param originalPenStyle - flag to indicate whether to use the original pen styles or to use
          * different ones.
          */
        virtual void updatePenStyle(bool originalPenStyle);

        /**
          * scaleValue
          * 
          * Scale the value based on the minimum and maximum engineering values and
          * whether the item is a boolean type.
          *
          * @param value - raw value
          * @param graphedItem - pointer to graphedItem representation of item
          *
          * @return scaled value
          */
        virtual double scaleValue(double value, const GraphedItem* graphedItem) const;

        /**
          * onClickBackground
          * 
          * Called when the background has been clicked on.
          */
        virtual void onClickBackground();

        /**
          * onClickSeries
          * 
          * Called when a series has been clicked on.
          * 
          * @param seriesNumber - series number
          */
        virtual void onClickSeries(long seriesNumber);

        /**
          * onClickLegend
          * 
          * Called when a series has been toggled on/off in the legend.
          *
          * @param nonActiveSeriesNumber - vector of series numbers of the non active series.
          */
        virtual void onClickLegend(const std::vector<long>& nonActiveSeriesNumbers);

        /**
          * onScroll
          * 
          * Called when the graph has been scrolled.
          * 
          * @param days - new range days property
          * @param time - new range time property
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
          * getScrollableAxesIndexes
          * 
          * Get the indexes of the scrollable axes.
          *
          * @return - indexes of scrollable axes
          */
        virtual std::vector<long> getScrollableAxesIndexes() const;

        /**
          * moveGraph
          * 
          * Moves the graph given that the X-axis is of date/time format.
          *
          * @param timestamp - new time to set the right-hand side of the graph to.
          */
        virtual void moveGraph(time_t timestamp);

        /**
          * plotPoint
          * 
          * Plot a point on the graph.
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
          * onCursorChange
          * 
          * Called when the cursor has been moved.
          * Not needed for comparison graph. This method will assert if called.
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
          * @param tool - Index of the tool that has changed
          * @param x - X Screen value
          * @param y - Y screen value
          * @param xVal - x axis value
          * @param yVal - y axis value
          * @param series - series the cursor intersects with
          */
        virtual void onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series);

        /**
          * onCursorChange
          * 
          * Called when the cursor has been moved.
          * 
          * @param seriesNumber - series number
          * @param x - x value at the cursor
          * @param y - y value at the cursor
          * @param valueIndex - index of the point intersecting with the cursor in the series
          * @param mouseX - Screen coordinates X value at the cursor
          */
        void onCursorChange(long seriesNumber, double x, double y, long valueIndex, double mouseX);

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
        virtual void setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds);

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
        
    protected:

        /**
          * addCustomAxis
          * 
          * Adds a custom axis for a graphed item.
          * Not needed for Comparison Graph. This method will assert if called.
          *
          * @param index - Index of the graphed item
          */
        virtual void addCustomAxis(int index);

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
          * createNewItem
          * 
          * Creates a new Graphed item for a new item that has been added to the graph.
          *
          * @param index - Index of the item taken from the trend properties dialog.
          * @param itemName - Name of the item
          * @param penColour - Colour configured for item
          * @param startDateTime - Start date/time configured for graph
          * @param endDateTime - End date/time of graph
          * @param displayName - Display name for the item
          */
        virtual void createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName);

        /**
          * removeItem
          * 
          * Remove an item from the graph.
          *
          * @param index - AIndex of item taken from the trend properties dialog.
          */
        virtual void removeItem(int index);
        
    private:

        ComparisonGraph( const ComparisonGraph& theComparisonGraph);
        ComparisonGraph& operator=(const ComparisonGraph &);

        void addCustomAxis(long seriesNumber, const std::string& verticalLabel, const std::string& horizontalLabel, 
        double verticalMin, double verticalMax, time_t horizontalMin, time_t horizontalMax, bool isDefault, bool isTopGraph, bool isBooleanType);
        void retrieveData(long seriesNumber, time_t timestamp, double& dataValue, COLORREF& colour, bool isOffsetSeries);
        void setUpCursorTools();
                
    private:

        time_t m_offset;                                    // Time difference between the top and bottom graph. Bottom
                                                            // graph is always behind top graph.

        long m_defaultVerticalAxisIndex;                    // Index of vertical axis on top graph
        long m_defaultOffsetVerticalAxisIndex;              // Index of vertical axis on bottom graph
        long m_defaultHorizontalAxisIndex;                  // Index of horizontal axis on top graph
        long m_defaultOffsetHorizontalAxisIndex;            // Index of horizontal axis on bottom graph
        std::map<long, long> m_topBottomSeriesNumberMap;    // Map linking the series numbers of the top and bottom 
                                                            // trends for the one graphed item.

        int m_leftChartMargin;                              // Value of the original position of the left margin of chart
        long m_topArrowToolIndex;                           // Index of the arrow tool that displays the scroll arrows 
                                                            // on the top graph

        long m_bottomArrowToolIndex;                        // Index of the arrow tool that displays the scroll arrows
                                                            // on the bottom graph

        long m_cursorToolIndex;                             // Index of the cursor tool
        long m_defaultAxisArrowToolIndex;                   // Index of the default axis arrow tool
        bool m_cursorDisplayed;                             // Indicates whether the cursor is currently displayed
        long m_startBottomChart;                            // Y-value of the screen coordinates of where the
                                                            // bottom graph starts. This is used to determine which
                                                            // part of the graph has been zoomed and whether to zoom in.

        std::map<long, long> m_boolTopBottomAxisIndexMap;
        
        TA_Base_Core::ReEntrantThreadLockable m_lock;
        TA_Base_Core::ReEntrantThreadLockable m_graphLock;

		TA_Base_Core::DateFormat m_DateFormat; // TD12474
    };
}

#endif // !defined(ComparisonGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)

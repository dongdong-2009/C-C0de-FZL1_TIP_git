#if !defined(XYGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define XYGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * XYGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/XYGraph.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from AbstractGraph and represents an XY Graph. Some AbstractGraph
  * methods have been overwritten especially for an XY Graph. The main difference is the 
  * XY Graph plots two items against each other as opposed to value against time. The set-up and 
  * plotting of points has been changed to accomodate for this.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include "app/trending/trend_viewer/src/stdafx.h"
#include "tchart.h" // For TeeChart control

#include "app/trending/trend_viewer/src/AbstractGraph.h"

namespace TA_Base_App
{
    class XYGraph : public AbstractGraph
    {

    public:

        /**
          * XYGraph
          * 
          * Constructor.
          *
          */
        XYGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
            time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, long axisArrowToolIndex,
            int leftChartMargin);

        /**
          * ~XYGraph
          * 
          * Destructor
          *
          */
        virtual ~XYGraph();

        /**
          * setUpGraph
          * 
          * Retrieves all the graph properties and sets up the line graph.
          */
        virtual void setUpGraph();

        /**
          * addToGraph
          * 
          * Adds a new line to the graph.
          *
          * @param index - index of item taken from Trend Properties Dialog
          * @param itemName - name of item 
          * @param penColour - colour of the line on the graph
          * @param displayName - display name for the item
          * @param displayName - display name for the item
          */
        virtual void addToGraph(int index, const std::string& itemName, COLORREF penColour, const std::string& displayName);

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
          * onScroll
          * 
          * Called when the graph has been scrolled. Overrides AbstractGraph::onScroll.
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
          * Called when the graph has been zoomed or unzoomed. Overrides AbstractGraph::onZoom.
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
          * Called when the cursor has been moved. Overrides AbstractGraph::onCursorChange.
          * 
          * @param seriesNumber - series number
          * @param x - x value at the cursor
          * @param y - y value at the cursor
          * @param valueIndex - index of the point intersecting with the cursor in the series
          */
        virtual void onCursorChange(long seriesNumber, double x, double y, long valueIndex);

        /**
          * onClickSeries
          * 
          * Called when a series has been clicked on. Overrides AbstractGraph::onClickSeries.
          * 
          * @param seriesNumber - series number
          */
        virtual void onClickSeries(long seriesNumber);

        /**
          * displayCursor
          * 
          * Toggles cursor on/off. Overrides AbstractGraph::displayCursor.
          *
          * @param showCursor - flag indicating whether cursor is on or off
          * @param toolNumber - tool number for the cursor
          */
        virtual void displayCursor(bool showCursor, long toolNumber);

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
        virtual void onCursorToolChange(long Tool, long X, long Y, double XVal, double YVal, long Series);

        /**
          * plotPoint
          * 
          * Plot a point on the graph.
          * Overrides AbstractGraph::plotPoint.
          * 
          * @param seriesNumber - Series number of series to add point to
          * @param x - x value
          * @param y - y value
          * @param penColour - colour of point
          */
        virtual void plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour);

        /**
          * moveGraph
          * 
          * Moves the graph given that the X-axis is of date/time format.
          *
          * @param timestamp - new time to set the right-hand side of the graph to.
          */
        virtual void moveGraph(time_t timestamp);

        /**
          * getScrollableAxesIndexes
          * 
          * Get the indexes of the scrollable axes.
          *
          * @return - indexes of scrollable axes
          */
        virtual std::vector<long> getScrollableAxesIndexes() const;

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

        /**
          * onClickBackground
          * 
          * Called when the background has been clicked on.
          */
        virtual void onClickBackground();

    protected:

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

        XYGraph(XYGraph&);
        XYGraph& operator=(const XYGraph&);
        
    private:

        long m_seriesNumber;                        // Series number for the single line on this graph
        std::map<int, time_t> m_plottedDataTimes;   // Store of the times for each point plotted
        long m_defaultAxisArrowToolIndex;           // This is the index of the arrow tool used on the default graph

        std::string m_displayName1;                 // Display name for the first item
        std::string m_displayName2;                 // Display name for the second item

        std::string m_unitLabel1;                   // Engineering unit label for the first item
        std::string m_unitLabel2;                   // Engineering unit label for the second item

        int m_precision1;                           // Engineering value precision for first item
        int m_precision2;                           // Engineering value precision for second item

        int m_leftChartMargin;                      // Original position of the left side of the graph 

        std::map<long, GraphedItem*> m_indexToItemMap; // Maps boolean series number to GraphedItem
    };
}

#endif // !defined(XYGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)

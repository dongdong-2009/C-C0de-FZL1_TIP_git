#if !defined(AFX_IGRAPH_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_IGRAPH_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * IGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IGraph.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between the main GUI class and an actual Graph object. The 
  * GUI will make calls on this interface to manipulate the graph.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"

// TeeChart includes
#include <series.h>


namespace TA_Base_App
{
    
    //
    // IGraph
    //
    class IGraph  
    {
    public:

        /**
          * ~IGraph
          * 
          * Destructor.
          */
        virtual ~IGraph() {}
        
        /**
          * setUpGraph
          * 
          * Retrieves all the graph properties and sets up the graph.
          */
        virtual void setUpGraph() = 0;

        /**
          * addToGraph
          * 
          * Adds an item to the graph.
          *
          * @param index - index of item taken from Trend Properties Dialog
          * @param itemName - name of item 
          * @param penColour - colour of the line on the graph
          * @param displayName - display name for the item
          */
        virtual void addToGraph(int index, const std::string& name, COLORREF penColour, const std::string& displayName) = 0;

        /**
          * deleteFromGraph
          * 
          * Delete an item from the graph.
          *
          * @param index - index taken from the Trend Properties Dialog
          */
        virtual void deleteFromGraph(int index) = 0;

        /**
          * adjustXAxisTicks
          * 
          * Adjust the minor increment for the label increments.
          */
        virtual void adjustXAxisTicks() = 0;

        /**
          * adjustXAxisFormat
          * 
          * Adjust the labels of the X-axis. This could be setting the date/time
          * format, changing it to a percentage scale, or using engineering units.
          */
        virtual void adjustXAxisFormat() = 0;

        /**
          * displayCursor
          * 
          * Turn the cursor on or off.
          *
          * @param showCursor - flag to indicate whether the cursor should be shown
          */
        virtual void displayCursor(bool showCursor, long toolNumber) = 0;

        /**
          * setLiveMode
          * 
          * Turn live mode on or off.
          *
          * @param isLive - flag to indicate whether live mode is on or off
          * @param newStartTime - new time shown on right mode side of graph
          * @param bAfterFirstLoad - flag to indicate that setLiveMode was called right after the items were added into the graph //TD17935
          */
        virtual void setLiveMode(bool isLive, time_t& newStartTime, bool bAfterFirstLoad = false) = 0; //TD17935

        /**
          * reloadGraph
          * 
          * Re-read all the trend properties and display the graph again.
          */
        virtual void reloadGraph() = 0;

        /**
          * setStartDateTime
          * 
          * Called when the left-hand side of the graph has a new time, eg. when
          * the graph has been scrolled.
          *
          * @param startDateTime - new left-hand side date/time
          */
        virtual void setStartDateTime(time_t startDateTime) = 0;

        /**
          * plotPoint
          * 
          * Plot a point on the graph.
          * 
          * @param seriesNumber - Series number of series to add point to
          * @param x - x value
          * @param y - y value
          * @param penColour - colour of point
          */
        virtual void plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour) = 0;

        /**
          * moveGraph
          * 
          * Moves the graph given that the X-axis is of date/time format.
          *
          * @param timestamp - new time to set the right-hand side of the graph to.
          */
        virtual void moveGraph(time_t timestamp) = 0;

        /**
          * onScroll
          * 
          * Called when the graph has been scrolled.
          * 
          * @param startDateTime - new start date time property
          * @param rangeSeconds - new range seconds property
          * @param axis - index of axis that has been scrolled
          */
        virtual void onScroll(time_t& startDateTime, long& rangeSeconds, long axis) = 0;

        /**
          * onZoom
          * 
          * Called when the graph has been zoomed or unzoomed.
          * 
          * @param startDateTime - new start date time property
          * @param rangeSeconds - new range seconds property
          * @param reset - indicates whether it is a zoom in (false) or zoom out (true)
          * @param x0 - x coordinate of the start drag
          * @param y0 - y coordinate of the start drag
          * @param x1 - x coordinate of the end drag
          * @param y1 - y coordinate of the end drag
          * @param reset - indicates whether the zoom is a zoom undo
          */
        virtual void onZoom(time_t& startDateTime, long& rangeSeconds, long x0, long y0, long x1, long y1, bool reset = false) = 0;

        /**
          * onCursorChange
          * 
          * Called when the cursor has been moved.
          * 
          * @param seriesNumber - series number
          * @param x - x value at the cursor
          * @param y - y value at the cursor
          * @param valueIndex - index of the point intersecting with the cursor in the series
          */
        virtual void onCursorChange(long seriesNumber, double x, double y, long valueIndex) = 0;

        /**
          * onClickSeries
          * 
          * Called when a series has been clicked on.
          * 
          * @param seriesNumber - series number
          */
        virtual void onClickSeries(long seriesNumber) = 0;

        /**
          * onClickBackground
          * 
          * Called when the background has been clicked on.
          */
        virtual void onClickBackground() = 0;

        /**
          * onClickLegend
          * 
          * Called when a series has been toggled on/off in the legend.
          *
          * @param nonActiveSeriesNumber - vector of series numbers of the non active series.
          */
        virtual void onClickLegend(const std::vector<long>& nonActiveSeriesNumbers) = 0;

        /**
          * onCursorToolChange
          * 
          * Called when the cursor moves.
          *
          * @param Tool - Index of the tool that has changed
          * @param X - X Screen value
          * @param Y - Y screen value
          * @param XVal - x axis value
          * @param YVal - y axis value
          * @param Series - series index
          */
        virtual void onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series) = 0;

        /**
          * updatePenStyle
          * 
          * Update the pen styles for each graphed item.
          *
          * @param originalPenStyle - flag to indicate whether to use the original pen styles or to use
          * different ones.
          */
        virtual void updatePenStyle(bool originalPenStyle) = 0;

        /**
          * getMaximumLimit
          * 
          * Get the maximum limit of the item associated with the series number.
          *
          * @param seriesNumber - series number
          *
          * @return - double maximum limit of item range
          */
        virtual double getMaximumLimit(long seriesNumber) const = 0;

        /**
          * getScrollableAxesIndexes
          * 
          * Get the indexes of the scrollable axes.
          *
          * @return - indexes of scrollable axes
          */
        virtual std::vector<long> getScrollableAxesIndexes() const = 0;

        virtual std::string getAxisLabel(long axis, long seriesIndex, long valueIndex) = 0;
    };
}

#endif // !defined(AFX_IGRAPH_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





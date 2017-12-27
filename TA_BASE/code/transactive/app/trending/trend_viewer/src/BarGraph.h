#if !defined(BarGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define BarGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * BarGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/BarGraph.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from AbstractGraph and represents a Bar Graph. Some AbstractGraph
  * methods have been overwritten especially for a Bar Graph.
  *
  */

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/AbstractGraph.h"

#include "cots/TeeChart/tchart.h" // For TeeChart control

namespace TA_Base_App
{
    class BarGraph : public AbstractGraph
    {

    public:

        /**
		  * BarGraph
		  * 
		  * Constructor.
		  *
		  */
         BarGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
             time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, bool isFieldUpdates);

        /**
		  * ~BarGraph
		  * 
		  * Destructor
		  *
		  */
        virtual ~BarGraph();

        /**
		  * setUpGraph
		  * 
		  * Retrieves all the graph properties and sets up the bar graph.
          */
	    virtual void setUpGraph();

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
		  * onCursorToolChange
		  * 
		  * Called when the cursor moves.
          *
          * @param Tool - Index of the tool that has changed
          * @param X - X Screen value
          * @param Y - Y screen value
          * @param XVal - x axis value
          * @param YVal - y axis value
          * @param series - series the cursor intersects with
          */
        virtual void onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series);

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

    protected:

        /**
          * addCustomAxis
          * 
          * Adds a custom axis for a graphed item.
          *
          * @param index - Index of the graphed item
          */
        virtual void addCustomAxis(int index);

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

        /**
          * adjustCustomAxisForBoolean
          * 
          * Adjust the custom axis for a boolean item after a zoom or scroll event.
          *
          */
        virtual void adjustCustomAxisForBoolean(double minYValue, double maxYValue, const GraphedItem* graphedItem, CAxis& theAxis);


    private:

        BarGraph( const BarGraph& theBarGraph);
	    BarGraph& operator=(const BarGraph &);

        bool m_isFieldUpdates;                  // Flag indicating whether the live mode is field updates
                                                // or sample updates.

    };
}

#endif // !defined(BarGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)

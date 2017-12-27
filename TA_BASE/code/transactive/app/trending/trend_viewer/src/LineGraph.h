#if !defined(LineGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define LineGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * LineGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/LineGraph.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from AbstractGraph and represents a Line Graph.
  *
  */

#include "app/trending/trend_viewer/src/stdafx.h"
#include "tchart.h" // For TeeChart control

#include "app/trending/trend_viewer/src/AbstractGraph.h"

namespace TA_Base_App
{
    class LineGraph : public AbstractGraph
    {

    public:

        /**
          * LineGraph
          * 
          * Constructor.
          *
          */
        LineGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
            time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver);

        /**
          * ~LineGraph
          * 
          * Destructor
          *
          */
        virtual ~LineGraph();

        /**
          * setUpGraph
          * 
          * Retrieves all the graph properties and sets up the line graph.
          */
        virtual void setUpGraph();

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

        LineGraph();
        LineGraph( const LineGraph& theLineGraph);
        LineGraph& operator=(const LineGraph &);

    };
}

#endif // !defined(LineGraph_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)

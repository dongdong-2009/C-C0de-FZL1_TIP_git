#if !defined(XYGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define XYGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * XYGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/XYGraph.cpp $
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

#include "app/trending/trend_viewer/src/XYGraph.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/Utilities.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/ScadaProxyException.h"

#include "core/threads/src/Thread.h"

#include "cots/TeeChart/TeechartDefines.h"

// TeeChart includes
//TD17179 - update include files to specify location
#include <cots/teechart/marks.h>
#include <cots/teechart/valuelist.h>
#include <cots/teechart/axis.h>
#include <cots/teechart/axes.h>
#include <cots/teechart/axislabels.h>
#include <cots/teechart/axistitle.h>
#include <cots/teechart/lineseries.h>
#include <cots/teechart/pen.h>
#include <cots/teechart/pointer.h>
#include <cots/teechart/toollist.h>
#include <cots/teechart/tools.h>
#include <cots/teechart/cursortool.h>
#include <cots/teechart/zoom.h>
#include <cots/teechart/panel.h>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    //
    // XYGraph
    //
    XYGraph::XYGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
        time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
        IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, long axisArrowToolIndex, 
        int leftChartMargin) 
        : AbstractGraph(chart, name, description, startTime, endTime, isLive, 
          samplePeriod, itemStore, archiveDataStore, graphObserver),
          m_seriesNumber(0),
          m_defaultAxisArrowToolIndex(axisArrowToolIndex),
          m_leftChartMargin(leftChartMargin)
    {
        m_minItems = XY_MIN_ITEMS;
        m_maxItems = XY_MAX_ITEMS;
    }


    //
    // ~XYGraph
    //
    XYGraph::~XYGraph()
    {
        getChart().GetAxis().GetBottom().GetLabels().Clear();
        getChart().GetAxis().GetLeft().GetLabels().Clear();

        // If graph is currently in live mode, turn it off
        if(isLive())
        {
            time_t startTime;
            setLiveMode(false, startTime);
        }

        for(unsigned int i = 0; i < m_maxItems; ++i)
        {
            deleteFromGraph(i+1);
        }

        // Need to undo the zoom here because when Zoom.Undo() is called, it
        // resets both left and bottom axes to their original min and max. If
        // the new graph type is not an XY graph, the bottom axes will get
        // confused as it is of a different type when a zoom undo is done.
        if(isGraphZoomed() == true)
        {
            getChart().GetZoom().Undo();
            setZoom(false);
        }

    }


    //
    // setUpGraph
    //
    void XYGraph::setUpGraph()
    {
        FUNCTION_ENTRY("setUpGraph");

        getChart().RemoveAllSeries();

        getChart().GetTools().GetItems(m_defaultAxisArrowToolIndex).SetActive(false);
        
        //
        // Add a dummy series so that the graph lines show
        //
        long seriesNum = getChart().AddSeries(LINE_GRAPH); 
        getChart().Series(seriesNum).GetXValues().SetDateTime(false);
        getChart().Series(seriesNum).SetShowInLegend(false);

        setTitle(getGraphName().c_str());
        
        // 
        // Don't set the min and max of axes here. The X and Y axes will be set using the 
        // engineering values of each item in addToGraph().
        //

        //
        // Set the start and end times on the GUI
        //
        getGraphObserver()->setStartEndTime(getStartTime(), getEndTime());

        //
        // Add the series here as we know there is only going to be one line
        //
        m_seriesNumber = getChart().AddSeries(LINE_GRAPH);
        CSeries thisSeries = getChart().Series(m_seriesNumber);

        thisSeries.GetAsLine().GetLinePen().SetWidth(LINE_PEN_WIDTH);
        thisSeries.GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);
        thisSeries.SetTitle(getGraphName().c_str());
        thisSeries.GetXValues().SetDateTime(false);

        thisSeries.GetAsLine().GetPointer().SetVisible(true);
        thisSeries.GetAsLine().GetPointer().SetStyle(LINE_POINT_STYLE);
        thisSeries.GetAsLine().GetPointer().SetVerticalSize(LINE_POINT_HORIZONTAL_SIZE);
        thisSeries.GetAsLine().GetPointer().SetHorizontalSize(LINE_POINT_VERTICAL_SIZE);
        thisSeries.GetAsLine().SetColorEachLine(FALSE);
        thisSeries.GetAsLine().GetPointer().GetPen().SetVisible(false);

        //getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);
        
        FUNCTION_EXIT;
    }


    //
    // addToGraph
    //
    void XYGraph::addToGraph(int index,const std::string& itemName, COLORREF penColour, const std::string& displayName)
    {
        FUNCTION_ENTRY("addToGraph");

        TA_ASSERT(m_graphedItems.size() + 1 <= m_maxItems, "Maximum items exceeded");

        getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);

        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.find(index);

        if(it == m_graphedItems.end())
        {
            // To get the state labels, the location key must be valid. Before creating the graphed item,
            // see if it is possible to retrieve the labels. If the location key is invalid, then don't
            // create the Graphed Item.
            try
            {
                IGenericItem* genericItem = getItemStore()->getItem(itemName);
                if(genericItem->getShouldDisplayLabel())
                {
                    getItemStore()->setStateLabels(genericItem);
                }
            }
            catch(const TA_Base_Core::ScadaProxyException& ex)
            {
                if(std::string(ex.what()).find("does not have a valid location") != std::string::npos)
                {
                    TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, 
                        "Datapoint has not been properly configured: Invalid location"));
                }
            }

            createNewItem(index, itemName, penColour, getStartTime(), getEndTime(), displayName);

            boost::shared_ptr<GraphedItem> graphedItem = m_graphedItems[index];
            
            double minValue = graphedItem->getMinimumLimit();
            double maxValue = graphedItem->getMaximumLimit();

            if(graphedItem->isBooleanType() == true)
            {
                // These values were calculated using a margin of ten on the edges of the axis
                // so the boolean values aren't plotted on the edge of the graph.
                minValue = -0.125;
                maxValue = 1.125;
            }

            // Create the axis label
            std::string axisLabel = displayName;
            axisLabel += " (";
            axisLabel += graphedItem->getName();
            axisLabel += ")";
            if(graphedItem->getCustomAxisLabel().empty() == false)
            {
                axisLabel += " - ";
                axisLabel += graphedItem->getCustomAxisLabel();
            }
                
            
            // There should only be 2 graphed items. Index 1 will be the X-axis and 
            // Index 2 will be the Y-axis.
            if(index == 1)
            {
                // Set up X-Axis
                getChart().GetAxis().GetBottom().SetMinMax(minValue, maxValue);
                
                if(graphedItem->getShouldDisplayLabel())
                {
                    // Set the labels on the axis since there will only be a true/false
                    getChart().GetAxis().GetBottom().GetLabels().SetOnAxis(true);
                }

                getChart().GetAxis().GetBottom().GetTitle().SetCaption(axisLabel.c_str());

                m_displayName1 = displayName;
                m_unitLabel1 = graphedItem->getCustomAxisLabel();
                m_precision1 = graphedItem->getPrecision();
            }
            
            if(index == 2)
            {
                // Set up Y-Axis
                getChart().GetAxis().GetLeft().SetMinMax(minValue, maxValue);

                if(graphedItem->getShouldDisplayLabel())
                {
                    // Set the labels on the axis since there will only be a true/false
                    getChart().GetAxis().GetLeft().GetLabels().SetOnAxis(true);
                }

                getChart().GetAxis().GetLeft().GetTitle().SetCaption(axisLabel.c_str());

                m_displayName2 = displayName;
                m_unitLabel2 = graphedItem->getCustomAxisLabel();
                m_precision2 = graphedItem->getPrecision();
            }

            // Want to display 0 and 1 in the display area for boolean items
            if(graphedItem->isBooleanType())
            {
                minValue = 0;
                maxValue = 1;
            }

            getGraphObserver()->addItem(index, displayName, minValue, maxValue, graphedItem->getCustomAxisLabel(), graphedItem->getPrecision()); 

            // Plot points on the graph
            plotData(graphedItem.get());
        }
        else
        {
            // Item has already been added
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "An item with index %d has already been added.", index);
        }

        FUNCTION_EXIT;
    }


    //
    // adjustXAxisTicks
    //
    void XYGraph::adjustXAxisTicks()
    {
        // Don't need to do anything as the X-Axis is not date/time format
    }


    //
    // adjustXAxisFormat
    //
    void XYGraph::adjustXAxisFormat()
    {
        // Don't need to do anything as the X-Axis is not date/time format
    }


    //
    // updatePenStyle
    //
    void XYGraph::updatePenStyle(bool originalPenStyle)
    {
        // Don't need to do anything as there is only one line
    }


    //
    // scaleValue
    //
    double XYGraph::scaleValue(double value, const GraphedItem* graphedItem) const
    {
        // Values do not need to be scaled for XY Graph
        return value;
    }

    
    //
    // plotData
    //
    void XYGraph::plotData(GraphedItem* graphedItem)
    {
        FUNCTION_ENTRY("plotData");

        if(m_graphedItems.size() != 2)
        {
            // Wait until there are two items to plot against each other
            return;
        }

        //
        // Retrieve data for X-axis item
        //
        boost::shared_ptr<GraphedItem> graphedItemX = m_graphedItems[1];
        PlotData dataX = graphedItemX->retrieveHistoricalData(getStartTime(), getEndTime(), graphedItemX->getTypeIdentifier(), getSampleType(), getSamplePeriod());

        //
        // Retrieve data for Y-axis item
        //
        boost::shared_ptr<GraphedItem> graphedItemY = m_graphedItems[2];
        PlotData dataY = graphedItemY->retrieveHistoricalData(getStartTime(), getEndTime(), graphedItemY->getTypeIdentifier(), getSampleType(), getSamplePeriod());

        //
        // Combine the item data and plot against each other
        //
        time_t newestTime = 0;
        double newestX = 0;
        double newestY = 0;
        int index = 0;
        m_plottedDataTimes.clear();
        PlotData::iterator itX = dataX.begin();
        for(; itX != dataX.end(); ++itX)
        {
            PlotData::iterator itY = dataY.begin();
            for(; itY != dataY.end(); ++itY)
            {
                // Compare the timestamps. If they are the same, combine the x values.
                if(itX->x == itY->x)
                {
                    m_plottedDataTimes[index] = static_cast<time_t>(itX->x);
                    ++index;

                    // Add the point to the graph
                    long seriesNum = graphedItem->getSeriesNumber();
                    getChart().Series(seriesNum).AddXY(itX->y, itY->y, "", graphedItemX->getPenColour());
                    
                    if(itX->x > newestTime)
                    {
                        newestTime = static_cast<time_t>(itX->x);
                        newestX = itX->y;
                        newestY = itY->y;
                    }

                    dataY.erase(itY);
                    break;
                }
            }
        }

        // Add two dummy points so the labels will display
        // on the custom axis
        if(graphedItemY->getShouldDisplayLabel())
        {
            setUpDummySeries(atLeft, graphedItemY.get(), 0, 0);
        }

        if(graphedItemX->getShouldDisplayLabel())
        {
            setUpDummySeries(atBottom, graphedItemX.get(), 0, 0);
        }

        graphedItemX->setLastXValue(newestX);
        graphedItemX->setLastYValue(newestY);
        graphedItemY->setLastXValue(newestX);
        graphedItemY->setLastYValue(newestY);

        //
        // Set name in legend
        //
        // Can't put the name and display name in the legend because if two datapoints with long names
        // are graphed, the name becomes too long and the legend won't even appear. So just display 
        // the two display names instead. The real names and display names will be shown as the
        // axis label instead.
        std::string legendName("");
        legendName = m_displayName1;
        legendName += " - ";
        legendName += m_displayName2;
        CSeries series = getChart().Series(m_seriesNumber);
        series.SetTitle(legendName.c_str());
        
        FUNCTION_EXIT;
    }


    //
    // onScroll
    //
    void XYGraph::onScroll(time_t& startDateTime, long& rangeSeconds, long axis)
    {
        // No need to update anything for XY Graph
        startDateTime = getStartTime();
        rangeSeconds = getRangeSeconds();
    }


    //
    // onZoom
    //
    void XYGraph::onZoom(time_t& startDateTime, long& rangeSeconds, long x0, long y0, long x1, long y1, bool reset)
    {
        setZoom(!reset);

        // No need to update anything for XY Graph
        startDateTime = getStartTime();
        rangeSeconds = getRangeSeconds();
    }


    //
    // onCursorChange
    //
    void XYGraph::onCursorChange(long seriesNumber, double x, double y, long valueIndex)
    {
        time_t cursorTime = 0;
        std::map<int, time_t>::iterator it = m_plottedDataTimes.find(valueIndex);
        if(it == m_plottedDataTimes.end())
        {
            cursorTime = 0;
        }
        else
        {
            cursorTime = m_plottedDataTimes[valueIndex];
        }
        
        // Notify the GUI of the cursor values
        getGraphObserver()->setCursorTime(cursorTime);
        
        getGraphObserver()->setCursorValue(1, x, m_precision1, m_unitLabel1);
        getGraphObserver()->setCursorValue(2, y, m_precision2, m_unitLabel2);

		std::ostringstream log;
		log << "x value = " << x << ", y value = " << y;		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s ", log.str().c_str());
    }


    //
    // onClickSeries
    //
    void XYGraph::onClickSeries(long seriesNumber)
    {
        // No custom axes for XY graph

        // Turn the default axis axes back on as there are no custom axies for XY graph
        getChart().GetAxis().GetLeft().GetLabels().SetVisible(true);
        getChart().GetAxis().GetLeft().GetTitle().SetVisible(true);

        //getChart().GetPanel().SetMarginLeft(m_leftChartMargin);
    }


    //
    // onClickBackground
    //
    void XYGraph::onClickBackground()
    {
        // Margin has been reset, move it back in
        getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);
    }


    //
    // displayCursor
    //
    void XYGraph::displayCursor(bool showCursor, long toolNumber)
    {
        FUNCTION_ENTRY("displayCursor");

        if(showCursor == true)
        {
            CCursorTool cursorTool = getChart().GetTools().GetItems(toolNumber).GetAsTeeCursor();
            cursorTool.SetStyle(cssBoth);
            cursorTool.SetFollowMouse(false);
            cursorTool.SetSnap(true);
            getChart().GetTools().GetItems(toolNumber).SetActive(true);

            double minValue = getChart().GetAxis().GetBottom().GetMinimum();
            double maxValue = getChart().GetAxis().GetBottom().GetMaximum();
            double mid = maxValue - (maxValue - minValue) / 2;

            getChart().GetTools().GetItems(toolNumber).GetAsTeeCursor().SetXVal(mid);
            
            minValue = getChart().GetAxis().GetLeft().GetMinimum();
            maxValue = getChart().GetAxis().GetLeft().GetMaximum();
            mid = maxValue - (maxValue - minValue) / 2;

            getChart().GetTools().GetItems(toolNumber).GetAsTeeCursor().SetYVal(mid);
            
            // Notify the GUI of the cursor value
            getGraphObserver()->setCursorTime(0);
        }
        else
        {
            getChart().GetTools().GetItems(toolNumber).SetActive(false);
        }

		//TD17179 - disable engineering values to be shown once cursor is disabled
		std::map<int, boost::shared_ptr<GraphedItem> >::const_iterator it = m_graphedItems.begin();
		for(; it != m_graphedItems.end(); ++it)
		{
			getChart().Series(it->second->getSeriesNumber()).GetMarks().SetVisible(showCursor);				
		}
		//++TD17179 - lizettejl	
        
        FUNCTION_EXIT;
    }


    void XYGraph::onCursorToolChange(long Tool, long X, long Y, double XVal, double YVal, long Series)
    {
        if(getChart().GetSeriesCount() == 1)
        {
            // No series showing
        
            getGraphObserver()->setCursorTime(0);
            return;
        }

        if(isSeriesVisible(1) == false)
        {
            getGraphObserver()->setCursorTime(0);
            getGraphObserver()->setCursorValue(1, NO_VALUE, m_precision1, "");
            getGraphObserver()->setCursorValue(2, NO_VALUE, m_precision2, "");
            return;
        }

        // For XY there will only be one series showing
        long clicked = getChart().Series(1).Clicked(X, Y);
        if(clicked != -1)
        {
            onCursorChange(1, XVal, YVal, clicked);
        }
        else
        {
            // Cursor does not intersect with series
            getGraphObserver()->setCursorTime(0);
            getGraphObserver()->setCursorValue(1, NO_VALUE, m_precision1, "");
            getGraphObserver()->setCursorValue(2, NO_VALUE, m_precision2, "");
        }
    }


    //
    // getAxisLabel
    //
    std::string XYGraph::getAxisLabel(long axis, long seriesIndex, long valueIndex)
    {
        double value = 0;
        double minValue = 0;
        double maxValue = 0;

        if(axis == atBottom)
        {
            value = getChart().Series(seriesIndex).GetXValues().GetValue(valueIndex);
            minValue = getChart().Series(seriesIndex).GetXValues().GetMinimum();
            maxValue = getChart().Series(seriesIndex).GetXValues().GetMaximum();
        }
        else if(axis == atLeft)
        {
            value = getChart().Series(seriesIndex).GetYValues().GetValue(valueIndex);
            minValue = getChart().Series(seriesIndex).GetYValues().GetMinimum();
            maxValue = getChart().Series(seriesIndex).GetYValues().GetMaximum();
        }
        else
        {
            return "";
        }
        
        GraphedItem* graphedItem = NULL;

        // This map will be filled by setUpDummySeries if there are boolean items
        if(m_indexToItemMap.size() != 0)
        {
            std::map<long, GraphedItem*>::iterator iter = m_indexToItemMap.find(axis);
        
            if(iter == m_indexToItemMap.end())
            {
                return "";
            }

            graphedItem = iter->second;
        }
       
        std::map<int, std::string> labels = graphedItem->getStateLabels();
        std::map<int, std::string>::iterator iter = labels.find(value);
        if(iter != labels.end())
        {
            return iter->second;
        }
     
        return "";
    }


    //
    // plotPoint
    //
    void XYGraph::plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour)
    {
        FUNCTION_ENTRY("plotPoint");

		//time_t timestamp = static_cast<time_t>(x);
		//TD17883 -no need to convert time to local time
        //time_t timestamp = TimeFunction::getInstance().ConvertToLocalTimeT(static_cast <long> (x));
		time_t timestamp = static_cast<time_t>(x);

        double dataValueX = 0;
        double dataValueY = 0;
        int graphedItemIndex = 0;
        COLORREF pointColour = penColour;

        moveGraph(timestamp);
        
        // Retrieve the index number for the GraphedItem associated with the seriesNumber
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getSeriesNumber() == seriesNumber)
            {
                graphedItemIndex = it->first;
                break;
            }
        }

        if(it == m_graphedItems.end())
        {
            // Series number wasn't found
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "No GraphedItem associated with series number: %lu. Cannot plot point.", seriesNumber);
            return;
        }

        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point. Series number: %ld does not exist", seriesNumber);
            return;
        }

        if(getSamplePeriod() == LIVE_FIELD_UPDATES)
        {
            if(graphedItemIndex == 1)
            {
                // X-axis value
                dataValueX = y;
                dataValueY = it->second->getLastYValue();
            }
            else // index == 2
            {
                // Y-axis value
                dataValueX = it->second->getLastXValue();
                dataValueY = y;
            }

            // Add the point to the graph
            CSeries& series = getChart().Series(seriesNumber);
            series.AddXY(dataValueX, dataValueY, "", pointColour);

            graphedItem->setLastXValue(dataValueX);
            graphedItem->setLastYValue(dataValueY);

            m_plottedDataTimes[m_plottedDataTimes.size()] = timestamp;
       
			// TD17718 - use the configured max points in DB 
            if(series.GetCount() > Utilities::getMaxPointsToDisplay())
			//++TD17718
            {
                // Remove the oldest point
                series.Delete(0); 

                // Decrement the index of each point
                std::map<int, time_t> newDataTimes;
                std::map<int, time_t>::iterator it2 = m_plottedDataTimes.begin();
                for(; it2 != m_plottedDataTimes.end(); ++it2)
                {
                    int index = it2->first;
                    if(index != 0)
                    {
                        newDataTimes[--index] = it2->second;
                    }
                }

                m_plottedDataTimes.clear();
                it2 = newDataTimes.begin();
                for(; it2 != newDataTimes.end(); ++it2)
                {
                    m_plottedDataTimes[it2->first] = it2->second;
                }
            }
        }
        else
        {
            // Reload data for this series. Chosen to reload here as opposed to retrieving the data
            // from the database for the given time in case there is a time lag.

            getChart().Series(seriesNumber).Clear();
            //getChart().Series(graphedItem->getBoolSeriesNumber()).Clear();

            try
            {
                plotData(graphedItem.get());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, 
                "Error loading historical data. Display may be incomplete"));
            }   
        }

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }

        FUNCTION_EXIT;
    }


    //
    // moveGraph
    //
    void XYGraph::moveGraph(time_t timestamp)
    {
        // XY Graph does not move. Just update the start and end time displays
        time_t startTime = timestamp - getRangeSeconds();

        setStartTime(startTime);
        setEndTime(timestamp);

        getGraphObserver()->setStartEndTime(startTime, timestamp);
    }


    //
    // getScrollableAxesIndexes
    //
    std::vector<long> XYGraph::getScrollableAxesIndexes() const
    {
        std::vector<long> axes;
        return axes;
    }


    //
    // setUpDummySeries
    //
    void XYGraph::setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds)
    {
        // This is set here because AbstractGraph::reloadGraph will attempt to clear the dummy series
        // if it is an item that displays labels.
        long boolSeriesNumber = graphedItem->getBoolSeriesNumber();

        // Check if there is already a dummy series associated with and remove it if there is.
        if(boolSeriesNumber >= 0)
        {
            getChart().Series(boolSeriesNumber).Clear();
        }

        if(boolSeriesNumber < 0)
        {
            // Series for vertical axis
            long seriesNum = getChart().AddSeries(LINE_GRAPH); // 0 for line graph
            getChart().Series(seriesNum).GetXValues().SetDateTime(false);
            getChart().Series(seriesNum).SetShowInLegend(false);
            getChart().Series(seriesNum).GetAsLine().GetLinePen().SetVisible(false);
            boolSeriesNumber = seriesNum;
        }

        if(customAxisNum == atLeft)
        {
            std::map<int, std::string> labels = graphedItem->getStateLabels();
            std::map<int, std::string>::iterator iter = labels.begin();

            for(; iter != labels.end(); ++iter)
            {
                getChart().GetAxis().GetLeft().GetLabels().Add(iter->first, iter->second.c_str());
            }
        }
        else if(customAxisNum == atBottom)
        {
           std::map<int, std::string> labels = graphedItem->getStateLabels();
            std::map<int, std::string>::iterator iter = labels.begin();

            for(; iter != labels.end(); ++iter)
            {
                getChart().GetAxis().GetBottom().GetLabels().Add(iter->first, iter->second.c_str());
            }
        }
        else
        {
            TA_ASSERT(false, "Invalid axis index");
        }

        // This is set here because AbstractGraph::reloadGraph will attempt to clear the dummy series
        // if it is an item that displays labels.
        graphedItem->setBoolSeriesNumber(boolSeriesNumber);
        m_indexToItemMap[customAxisNum] = graphedItem;
    }


    //
    // createNewItem
    //
    void XYGraph::createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName)
    {
        FUNCTION_ENTRY("createNewItem");

        // Create a graphed item
        IItemStore* itemStore = getItemStore();
        IGenericItem* genericItem = itemStore->getItem(itemName);

        CSeries thisSeries = getChart().Series(m_seriesNumber);
        thisSeries.SetColor(penColour); 

        boost::shared_ptr<GraphedItem> graphedItem(new GraphedItem(thisSeries, m_seriesNumber, *this, *genericItem, penColour, startDateTime, endDateTime)); 
        m_graphedItems[index] = graphedItem;

        FUNCTION_EXIT;
    }


    //
    // removeItem
    //
    void XYGraph::removeItem(int index)
    {
        FUNCTION_ENTRY("removeItem");

        // The same series number is associated with both items. We don't want
        // to remove it the second time.
        static bool seriesRemoved = false;

        if(seriesRemoved == true)
        {
            return;
        }

        long seriesNum = m_graphedItems[index]->getSeriesNumber();

        // Currently the Trend Viewer will reload the whole trend template if something
        // is changed. Before reloading, the whole graph is cleared so there is currently
        // no need to remove one item. The following code is kept here because it would 
        // be more efficient to just reload the properties of the graph that have changed
        // rather than reloading the whole template. This means that if items have been
        // removed, there would be a need to remove one item at a time. This code can be
        // uncommented in the future when the Trend Viewer template loading is made more
        // efficient.

        // Disconnect the custom axis from the series
        //m_ChartCtrl.Series( seriesNum ).DisconnectAxisCustom( false );

        //int numCustomAxes = m_ChartCtrl.GetAxis().GetCustomCount();

        // Remove the custom axis
        //long axisIndex = (*gdp_it).GetAxisIndex();
        //m_ChartCtrl.GetAxis().RemoveCustom( axisIndex );

        // Set the grid back to default if the deleted trend was displaying
        // a custom axis
        //if(m_customAxisDisplayed == axisIndex)
        //{
        //    m_ChartCtrl.GetAxis().GetLeft().GetGridPen().SetVisible(true);
        //    m_customAxisDisplayed = NO_CUSTOM_AXIS_DISPLAYED;
        //    m_ChartCtrl.GetPanel().SetMarginRight(m_rightMarginInitial);
        //}

        getChart().RemoveSeries(seriesNum);

        seriesRemoved = true;

        //
        // Adjust the series numbering for series added after the deleted one
        //
        //adjustSeriesNumbers(seriesNum);

        FUNCTION_EXIT;
    }

}

#endif // XYGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

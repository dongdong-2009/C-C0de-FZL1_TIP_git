#if !defined(LineGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define LineGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * LineGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/LineGraph.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from AbstractGraph and represents a Line Graph.
  *
  */

#include "app/trending/trend_viewer/src/LineGraph.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/exceptions/src/TrendViewerException.h"

// TeeChart includes
#include <cots/teechart/valuelist.h>
#include <cots/teechart/axis.h>
#include <cots/teechart/axes.h>
#include <cots/teechart/axistitle.h>
#include <cots/teechart/axislabels.h>
#include <cots/teechart/lineseries.h>
#include <cots/teechart/marks.h>
#include <cots/teechart/pen.h>
#include <cots/teechart/pointer.h>
#include <cots/teechart/zoom.h>
#include <cots/teechart/teeChartDefines.h>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    //
    // LineGraph
    //
    LineGraph::LineGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
            time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver) 
            : AbstractGraph(chart, name, description, startTime, endTime, isLive, 
              samplePeriod, itemStore, archiveDataStore, graphObserver)
    {
        m_minItems = LINE_MIN_ITEMS;
        m_maxItems = LINE_MAX_ITEMS;
    }


    //
    // ~LineGraph
    //
    LineGraph::~LineGraph()
    {
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

        if(isGraphZoomed() == true)
        {
            getChart().GetZoom().Undo();
            setZoom(false);
        }
    }


    //
    // setUpGraph
    //
    void LineGraph::setUpGraph()
    {
        FUNCTION_ENTRY("setUpGraph");

        getChart().RemoveAllSeries();

        //
        // Add a dummy series so that the graph lines show
        //
        long seriesNum = getChart().AddSeries(LINE_GRAPH); // 0 for line graph
        getChart().Series(seriesNum).GetXValues().SetDateTime(true);
        getChart().Series(seriesNum).SetShowInLegend(false);
        getChart().GetAxis().GetLeft().GetTitle().SetCaption(PERCENTAGE_TITLE);
        getChart().GetAxis().GetBottom().GetTitle().SetCaption(TIME_TITLE);
        setTitle(getGraphName().c_str());

		//TD17179 - disable engineering values to show as markup texts when setting up the 
		// graph until the cursor tool is enabled
        // fix defect 14171
        //getChart().Series(seriesNum).GetMarks().SetVisible(true); 
		//++TD17179 - lizettejl		
        
        //
        // Set the times on the X-Axis
        //
        COleDateTime startTime(getStartTime());
        COleDateTime endTime(getEndTime());
       
        getChart().GetAxis().GetBottom().SetMinMax(DATE(startTime), DATE(endTime));
        getChart().GetAxis().GetLeft().SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

        // Set the start and end times on the GUI
        getGraphObserver()->setStartEndTime(getStartTime(), getEndTime());
        
        FUNCTION_EXIT;
    }


    //
    // updatePenStyle
    //
    void LineGraph::updatePenStyle(bool originalPenStyle)
    {
        if(originalPenStyle == true)
        {
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);
            }
        }
        else
        {
            int penStyle = 0;
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
				 // TES 594: Remove styles that don't have connecting lines
				// TD18006 - do not include smallDots pen pattern
                while(penStyle % MAX_PEN_STYLES == psDot || penStyle % MAX_PEN_STYLES == psClear || penStyle % MAX_PEN_STYLES == psSmallDots)
                {
                    ++ penStyle;
                }
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsLine().GetLinePen().SetStyle(penStyle % MAX_PEN_STYLES);
                ++penStyle;

               
            }
        }
    }


    //
    // scaleValue
    //
    double LineGraph::scaleValue(double value, const GraphedItem* graphedItem) const
    {		
        //TD17940 - modified scaling computation for given value, for all datapoint types, simply return
		return value;
    }


    //
    // setUpDummySeries
    //
    void LineGraph::setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds)
    {
        CWaitCursor wait;

        long boolSeriesNumber = graphedItem->getBoolSeriesNumber();
        
        //  TD 10456: Set up just one dummy series per graphed series.
        // Check if there is already a dummy series associated with and clear the
        // series if there is.
        if(boolSeriesNumber >= 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Clearing series: %d", boolSeriesNumber);
            getChart().Series(boolSeriesNumber).Clear();
            //getChart().RemoveSeries(graphedItem->getBoolSeriesNumber());
            //adjustSeriesNumbers(graphedItem->getBoolSeriesNumber());
        }

        // If a dummy series hasn't been added, then add one
        if(boolSeriesNumber < 0)
        {
            long seriesNum = getChart().AddSeries(LINE_GRAPH); // 0 for line graph
            getChart().Series(seriesNum).GetXValues().SetDateTime(true);
            getChart().Series(seriesNum).SetShowInLegend(false);
            getChart().Series(seriesNum).SetVerticalAxisCustom(customAxisNum);
            getChart().Series(seriesNum).GetAsLine().GetLinePen().SetVisible(false);

            // fix defect 14171
            getChart().Series(seriesNum).GetMarks().SetVisible(false); // TD14799 azenitha
            graphedItem->setBoolSeriesNumber(seriesNum);
            boolSeriesNumber = seriesNum;
        }

        std::map<int, std::string> labels = graphedItem->getStateLabels();

        // Get the total range in seconds so that we can divide this by the number of states there
        // are so the dummy points are definately in the graph
        long secondsDiff = rangeSeconds / (labels.size() + 1);

        std::map<int, std::string>::iterator iter = labels.begin();

        time_t newTime = startTime + secondsDiff;
        for(; iter != labels.end(); ++iter)
        {
            double value = scaleValue(iter->first, graphedItem);
            COleDateTime date(newTime);
            getChart().Series(boolSeriesNumber).AddXY(DATE(date), value, "", graphedItem->getPenColour());

            newTime += secondsDiff;
        }

    }


    //
    // createNewItem
    //
    void LineGraph::createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName)
    {
        FUNCTION_ENTRY("createNewItem");

        // Create a graphed item
        IItemStore* itemStore = getItemStore();
        IGenericItem* genericItem = itemStore->getItem(itemName);

        long seriesNum = getChart().AddSeries(LINE_GRAPH);
        CSeries thisSeries = getChart().Series(seriesNum);

        boost::shared_ptr<GraphedItem> graphedItem(new GraphedItem(thisSeries, seriesNum, *this, *genericItem, penColour, startDateTime, endDateTime)); 
        m_graphedItems[index] = graphedItem;

        thisSeries.GetAsLine().GetLinePen().SetWidth(LINE_PEN_WIDTH);
        thisSeries.GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);

        std::string title = displayName + " (" + itemName + ")";
        thisSeries.SetTitle(title.c_str());
        thisSeries.GetXValues().SetDateTime(true);

        thisSeries.GetAsLine().GetPointer().SetVisible(true);
        thisSeries.GetAsLine().GetPointer().SetStyle(LINE_POINT_STYLE);
        thisSeries.GetAsLine().GetPointer().SetVerticalSize(LINE_POINT_HORIZONTAL_SIZE);
        thisSeries.GetAsLine().GetPointer().SetHorizontalSize(LINE_POINT_VERTICAL_SIZE);
        thisSeries.GetAsLine().SetColorEachLine(FALSE);
        thisSeries.GetAsLine().GetPointer().GetPen().SetVisible(false);
        thisSeries.SetColor(penColour);
        
        // fix defect 14171
        //thisSeries.GetMarks().SetVisible(true);
		thisSeries.GetMarks().SetVisible(false); //TD17178
        addCustomAxis(index);

        // TD 9781: Step line should be used for all items, not just boolean items. This is because we don't want
        // to interpolate between two points as we can't make assumptions of what the value of the item was at
        // the times in between the known ones.
        //if(genericItem->isBooleanType() == true)
        //{
        thisSeries.GetAsLine().SetStairs(true);
        //}

        FUNCTION_EXIT;
    }


    void LineGraph::removeItem(int index)
    {
        FUNCTION_ENTRY("removeItem");

        long seriesNum = m_graphedItems[index]->getSeriesNumber();

        // Commented out for the moment but may want it later if the Trend Viewer
        // is changed so that it doesn't reload the whole template when one
        // property is changed. Ie. If one item is removed, just remove the item
        // instead of reloading the whole template.

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

        // Set the grid back to default if the deleted trend was displaying
        // a custom axis
        onClickBackground();

        getChart().RemoveSeries(seriesNum);

        //
        // Adjust the series numbering for series added after the deleted one
        //
        adjustSeriesNumbers(seriesNum);

        FUNCTION_EXIT;
    }
}

#endif // LineGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

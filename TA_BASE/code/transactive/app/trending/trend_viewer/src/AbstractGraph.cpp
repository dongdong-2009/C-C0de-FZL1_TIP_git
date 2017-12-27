#if !defined(AbstractGraphCPP_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define AbstractGraphCPP_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * AbstractGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/AbstractGraph.cpp $
  * @author:  Craig Grant
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


#include "app/trending/trend_viewer/src/AbstractGraph.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/TrendConfigDatabaseAccess.h"
#include "app/trending/trend_viewer/src/ColourDefs.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/FunctionEntry.h"

#include "core/threads/src/Thread.h"

#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/ScadaProxyException.h"

#include "cots/teechart/TeeChartDefines.h"
#include "ace/OS.h"

// TeeChart includes
#include <cots/teechart/titles.h>
#include <cots/teechart/strings.h>
#include <cots/teechart/teefont.h>
#include <cots/teechart/axes.h>
#include <cots/teechart/axis.h>
#include <cots/teechart/axislabels.h>
#include <cots/teechart/axistitle.h>
#include <cots/teechart/axisarrowtool.h>
#include <cots/teechart/pen.h>
#include <cots/teechart/toollist.h>
#include <cots/teechart/tools.h>
#include <cots/teechart/cursortool.h>
#include <cots/teechart/valuelist.h>
#include <cots/teechart/lineseries.h>
#include <cots/teechart/marks.h> //TD17179


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    TA_Base_Core::ReEntrantThreadLockable AbstractGraph::m_graphLock;

    //
    // ~AbstractGraph
    //
    AbstractGraph::~AbstractGraph()
    {
        FUNCTION_ENTRY("~AbstractGraph");

		//TD-NEW
		std::map<int, boost::shared_ptr <GraphedItem> >::iterator it = m_graphedItems.begin();
		for (; it != m_graphedItems.end(); ++it)
		{
			boost::shared_ptr<GraphedItem> graphedItem = it->second;
			graphedItem->emptySeries();
			graphedItem.reset();

		}
		//++TD-NEW
        FUNCTION_EXIT;
    }


    //
    // adjustXAxisTicks
    //
    void AbstractGraph::adjustXAxisTicks()
    {
        FUNCTION_ENTRY("adjustXAxisTicks");

        double bottomIncrement = getChart().GetAxis().GetBottom().CalcIncrement();

        // For 5 or 10 second label increments
        if(bottomIncrement < TICKS_5_OR_10_SECONDS)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_4);
        }
        // For 30 or 15 second label increments
        else if(bottomIncrement < TICKS_15_OR_30_SECONDS)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_2);
        }
        // For 1 minute label increments
        else if(bottomIncrement < TICKS_1_MINUTE)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_5);
        }
        // For 5 or 10 minute label increments
        else if(bottomIncrement < TICKS_5_OR_10_MINUTES)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_4);
        }
        // For 15 or 30 minute label increments
        else if(bottomIncrement < TICKS_15_OR_30_MINUTES)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_2);
        }
        // For 1 hour label increments
        else if(bottomIncrement < TICKS_1_HOUR)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_3);
        }
        // For 6 or 12 hour label increments
        else if(bottomIncrement < TICKS_6_OR_12_HOUR)
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_5);
        }
        else
        {
            getChart().GetAxis().GetBottom().SetMinorTickCount(MINOR_TICK_COUNT_3);
        }

        FUNCTION_EXIT;
    }


    //
    // adjustXAxisFormat
    //
    void AbstractGraph::adjustXAxisFormat()
    {
        FUNCTION_ENTRY("adjustXAxisFormat");

        //
        // Check the time scale we are using and change if appropriate
        //
        double minValue = getChart().GetAxis().GetBottom().GetMinimum();
        double maxValue = getChart().GetAxis().GetBottom().GetMaximum();

        if((maxValue - minValue) < TIME_AXIS_15_MINUTES)
        {
            // Less than 15 minutes
            getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( TIME_ONLY_WITH_SECONDS );
        }
        else if((maxValue - minValue) <= TIME_AXIS_1_DAY)
        {
			// TD17174 ++
            // Less than a day
            //getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( TIME_ONLY_WITHOUT_SECONDS );
			getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( TIME_ONLY_WITH_SECONDS );
			// ++TD17174
        }
		//TD17174++
        else if((maxValue - minValue) <= TIME_AXIS_4_DAYS)
        {
            // Less than a week
			getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( DATE_TIME_WITH_SECONDS );
        }
		//++TD17174
        else if((maxValue - minValue) <= TIME_AXIS_7_DAYS)
        {
            // Less than seven days
			//TD12474 ++
			//getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( DATE_AND_TIME );
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat9);
			getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );
			//++ TD12474
        }
        else if((maxValue - minValue) <= TIME_AXIS_1_YEAR)
        {
            // Less than a year
			//TD12474 ++
			//getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( DATE_ONLY_WITH_DAYS );
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat5);
			getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );
			//++ TD12474
        }
        else
        {
			//TD12474 ++
			//getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( DATE_ONLY_WITHOUT_DAYS );
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec6);
			getChart().GetAxis().GetBottom().GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );
			//++ TD12474
        }

        FUNCTION_EXIT;
    }


    //
    // addToGraph
    //
    void AbstractGraph::addToGraph(int index,const std::string& itemName, COLORREF penColour, const std::string& displayName)
    {
        FUNCTION_ENTRY("addToGraph");

        TA_ASSERT(m_graphedItems.size() + 1 <= m_maxItems, "Maximum items exceeded");

        std::map<int, boost::shared_ptr <GraphedItem> >::iterator it = m_graphedItems.find(index);

        if(it == m_graphedItems.end())
        {
            // To get the state labels, the location key must be valid. Before creating the graphed item,
            // see if it is possible to retrieve the labels. If the location key is invalid, then don't
            // create the Graphed Item.
            try
            {
                IGenericItem* genericItem = m_itemStore->getItem(itemName);
                if(genericItem->getShouldDisplayLabel())
                {
                    m_itemStore->setStateLabels(genericItem);
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

            // Create the Graphed Item
            createNewItem(index, itemName, penColour, m_startTime, m_endTime, displayName);
            boost::shared_ptr<GraphedItem> graphedItem = m_graphedItems[index];
            
            double minLimit = 0;
            double maxLimit = 0;

            if(graphedItem->isBooleanType() == true)
            {
                maxLimit = 1;
            }
            else
            {
                minLimit = graphedItem->getMinimumLimit();
                maxLimit = graphedItem->getMaximumLimit();
            }

            getGraphObserver()->addItem(index, displayName, minLimit, maxLimit, graphedItem->getCustomAxisLabel(), graphedItem->getPrecision()); 

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
    // deleteFromGraph
    //
    void AbstractGraph::deleteFromGraph(int index)
    {
        FUNCTION_ENTRY("deleteFromGraph");

        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.find(index);

        if(it == m_graphedItems.end())
        {
            // Item doesn't exist
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "An item with index %d does not exist.", index);
        }
        else
        {
            removeItem(index);

            boost::shared_ptr<GraphedItem> graphedItem = m_graphedItems[index];
            getGraphObserver()->removeItem(graphedItem->getName());

            m_graphedItems.erase(it);
        }

        FUNCTION_EXIT;
    }


    //
    // setLiveMode
    //TD17935 - added new parameter
    void AbstractGraph::setLiveMode(bool liveState, time_t& newStartTime, bool bAfterFirstLoad /*= false*/)
    {
        FUNCTION_ENTRY("setLiveMode");
        
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();

        m_isLive = liveState;
        
        for(; it != m_graphedItems.end(); ++it)
        {
            it->second->setLiveMode(liveState, m_samplePeriod);
        }

        if(liveState == true)
        {
            // Move the graph to show the current time
            moveGraph(CTime::GetCurrentTime().GetTime());

            m_startTime = CTime::GetCurrentTime().GetTime() - m_rangeSeconds;
            m_endTime = m_startTime + m_rangeSeconds;
            newStartTime = m_startTime;
           //TD17935 - if the items were just recently added, no need to reload the graph since they were just plotted.
           //this is to speed up the loading of trend viewer
			if (!bAfterFirstLoad)
			{
				reloadGraph();
			}
           //++TD17935

            // TD 8622: Remove the left arrow from the arrow tool
            for(int i = 0; i < m_chart.GetTools().GetCount(); ++i)
            {
                if(m_chart.GetTools().GetItems(i).GetToolType() == tcAxisArrow)
                {
                    CAxisArrowTool arrowTool = m_chart.GetTools().GetItems(i).GetAsAxisArrow();
                    arrowTool.SetPosition(aaStart);
                }
            }
        }
        else
        {
            // TD 8622: Redisplay the left arrow on the arrow tool
            for(int i = 0; i < m_chart.GetTools().GetCount(); ++i)
            {
                if(m_chart.GetTools().GetItems(i).GetToolType() == tcAxisArrow)
                {
                    CAxisArrowTool arrowTool = m_chart.GetTools().GetItems(i).GetAsAxisArrow();
                    arrowTool.SetPosition(aaBoth);
                }
            }
        }

        
        FUNCTION_EXIT;
    }


    //
    // reloadGraph
    //
    void AbstractGraph::reloadGraph()
    {
        FUNCTION_ENTRY("reloadGraph");

        bool errorLoading(false);
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            getChart().Series(it->second->getSeriesNumber()).Clear();

            if(it->second->getShouldDisplayLabel())
            {
                getChart().Series(it->second->getBoolSeriesNumber()).Clear();
            }

            try
            {
                plotData(it->second.get());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                errorLoading = true;
            }
        }

        if(errorLoading == true)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, 
                "Error loading historical data. Display may be incomplete"));
        }

		//TD17752 - reset the default axis so that it will be redrawn 
		 m_graphObserver->resetDefaultAxis();
         onClickBackground();

        FUNCTION_EXIT;
    }


    //
    // onScroll
    //
    void AbstractGraph::onScroll(time_t& startDateTime, long& rangeSeconds, long axis)
    {
        //
        // Get the new min and max value
        //
        double minValue = m_chart.GetAxis().GetBottom().GetMinimum();
        double maxValue = m_chart.GetAxis().GetBottom().GetMaximum();

        time_t minTime = TimeFunction::getInstance().ConvertToTimeT(minValue);
        time_t maxTime = TimeFunction::getInstance().ConvertToTimeT(maxValue);
        
        m_startTime = minTime;
        m_endTime = maxTime;
        startDateTime = m_startTime;
        rangeSeconds = (maxTime - minTime);

        reloadGraph();

        // Notify the GUI so it can update the start and end time display
        //m_endTime = m_startTime + rangeSeconds;
        m_graphObserver->setStartEndTime(m_startTime, m_endTime);
    }


    //
    // onZoom
    //
    void AbstractGraph::onZoom(time_t& startDateTime, long& rangeSeconds, long x0, long y0, long x1, long y1, bool reset)
    {
        m_isGraphZoomed = !reset;
        adjustXAxisFormat();

        //
        // Get the new min and max value
        //
        COleDateTime minTime = m_chart.GetAxis().GetBottom().GetMinimum();
        COleDateTime maxTime = m_chart.GetAxis().GetBottom().GetMaximum();

        time_t minValue = TimeFunction::getInstance().ConvertToTimeT(minTime);
        time_t maxValue = TimeFunction::getInstance().ConvertToTimeT(maxTime);

        //if(reset == true)
        //{
        //    m_startTime = minValue;
        //    reloadGraph();
        //}
        
        rangeSeconds = maxValue - minValue;
        
        //
        // Can only zoom down to 1 minute minimum
        //
        if(SECONDS_IN_MINUTE > rangeSeconds)
        {
            //
            // Adjust to one minute
            //
            time_t centreValue = minValue + (maxValue - minValue) / 2;
            minValue = centreValue - 30;
            maxValue = centreValue + 30;
            rangeSeconds = 60;
            m_chart.GetAxis().GetBottom().SetMinMax(DATE(COleDateTime(minValue)), DATE(COleDateTime(maxValue)));
        }

        //
        // Adjust custom axes
        //
        double minValueLeft = m_chart.GetAxis().GetLeft().GetMinimum();
        double maxValueLeft = m_chart.GetAxis().GetLeft().GetMaximum();
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            CAxis theAxis = m_chart.GetAxis().GetCustom(it->second->getCustomAxisNumber());

            if(it->second->getShouldDisplayLabel() == false)
            {
                const double scaleFactor = it->second->getScaleFactor();
                theAxis.SetMinMax(minValueLeft / scaleFactor, maxValueLeft / scaleFactor);
            }
            else
            {
                adjustCustomAxisForBoolean(minValueLeft, maxValueLeft, it->second.get(), theAxis);

                // Also adjust the custom axis with dummy series
                CAxis otherAxis = m_chart.GetAxis().GetCustom(it->second->getBoolCustomAxisNumber());
                adjustCustomAxisForBoolean(minValueLeft, maxValueLeft, it->second.get(), otherAxis); 

                // If a dummy series is not set up then the axes won't show. We might have zoomed in on
                // a part of a graph that doesn't contain dummy series data, so temporarily set one up.
                // This will be removed each time a new zoom is done, or the zoom is reset.
                if(!reset)
                {
                    setUpDummySeries(it->second->getBoolCustomAxisNumber(), it->second.get(), minValue, rangeSeconds);
                }
            }
        }

        if(reset)
        {
            m_startTime = minValue;
            m_endTime = maxValue;
            reloadGraph();
        }

        startDateTime = minValue;
        
        // Notify the GUI so it can update the start and end time display
        m_graphObserver->setStartEndTime(startDateTime, startDateTime + rangeSeconds);
    }


    //
    // onCursorChange
    //
    void AbstractGraph::onCursorChange(long seriesNumber, double x, double y, long valueIndex)
    {
        // This implementation assumes that the X-Axis is of date/time format.
        // If it isn't, this method needs to be overridden.

        int index = -1;
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getSeriesNumber() == seriesNumber)
            {
                index = it->first;
                break;
            }
        }

        if(it == m_graphedItems.end())
        {
            // The series number doesn't exist, doesn't matter - just don't update anything
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Cursor returned series number that doesn't exist: %lu", seriesNumber);
            return;
        }

        COleDateTime time = x;
        CTime cursorTime(time.GetYear(), time.GetMonth(), time.GetDay(),
            time.GetHour(), time.GetMinute(), time.GetSecond());

        double value = NO_VALUE;
        if(y != NO_VALUE)
        {
            value = it->second->getActualValue(y);
        }
        
        // Notify the GUI of the cursor values
        m_graphObserver->setCursorTime(cursorTime.GetTime());
        m_graphObserver->setCursorValue(index, value, it->second->getPrecision(), it->second->getCustomAxisLabel());
    }


    void AbstractGraph::onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series)
    {
        const long NO_SERIES_DISPLAYED = 1; // Only the dummy series has been added
        if(getChart().GetSeriesCount() == NO_SERIES_DISPLAYED)
        {
            COleDateTime time = xVal;
            CTime cursorTime(time.GetYear(), time.GetMonth(), time.GetDay(),
                time.GetHour(), time.GetMinute(), time.GetSecond());

            getGraphObserver()->setCursorTime(cursorTime.GetTime());
            return;
        }

        
        for(long j = 1; j < getChart().GetSeriesCount(); ++j)
        {
            if(isSeriesVisible(j) == false)
            {
                onCursorChange(j, xVal, NO_VALUE, 0);
                continue;
            }

            try
            {
                double axisMin = getChart().GetAxis().GetCustom(getChart().Series(j).GetVerticalAxisCustom()).GetMinimum();
                double axisMax = getChart().GetAxis().GetCustom(getChart().Series(j).GetVerticalAxisCustom()).GetMaximum();

                long aMax = getChart().Series(j).CalcYPosValue(axisMin);
                long aMin = getChart().Series(j).CalcYPosValue(axisMax);

                double y = 0;
                int cnt = 0;
                std::vector<double> values;
                for(long i = aMin; i < aMax; ++i)
                {
                    long clicked = getChart().Series(j).Clicked(x, i);
                    if(clicked != -1)
                    {
                        if(getChart().Series(j).IsNull(clicked))
                        {
                            onCursorChange(j, xVal, NO_VALUE, 0);
                            continue;
                        }

                        double value = getChart().GetAxis().GetLeft().CalcPosPoint(i);
                        values.push_back(value);
                        ++cnt;
                    }
                }

                if(values.size() > 0)
                {
                    // Find the median
                    if(cnt % 2 == 0)
                    {
                        int valueIndex = cnt / 2;
                        y = (values[valueIndex - 1] + values[valueIndex]) / 2;
                    }
                    else
                    {
                        int valueIndex = cnt / 2;
                        y = values[valueIndex];
                    }

                    onCursorChange(j, xVal, y, 0);
                }
            }
            catch(const TA_Base_Core::TrendViewerException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

                if(ex.getType() == TA_Base_Core::TrendViewerException::SERIES_NUMBER_NOT_FOUND)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Series number: %d not found", j);
                }
                else
                {
                    throw;
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error ocurred");
                return;
            }
        }
    }


    //
    // onClickSeries
    //
    void AbstractGraph::onClickSeries(long seriesNumber)
    {
        if(m_lastCustomAxisDisplayed != NO_CUSTOM_AXIS_DISPLAYED)
        {
            CAxis oldAxis = m_chart.GetAxis().GetCustom(m_lastCustomAxisDisplayed);
            oldAxis.SetVisible(false);
        }

        // Ignore if the series number belongs to a dummy series
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator iter = m_graphedItems.begin();
        for(; iter != m_graphedItems.end(); ++iter)
        {
            if(iter->second->getBoolSeriesNumber() == seriesNumber)
            {
                return;
            }
        }

        GraphedItem* graphedItem = getGraphedItem(seriesNumber);
        CSeries series = m_chart.Series(seriesNumber);

        long axisIndex = -1;
        if(graphedItem->getShouldDisplayLabel())
        {
            axisIndex = graphedItem->getBoolCustomAxisNumber();
        }
        else
        {
            axisIndex = series.GetVerticalAxisCustom();
        }

        CAxis theAxis = m_chart.GetAxis().GetCustom(axisIndex);
        theAxis.SetVisible(true);

        m_lastCustomAxisDisplayed = axisIndex;
    }


    //
    // onClickBackground
    //
    void AbstractGraph::onClickBackground()
    {
        if(m_lastCustomAxisDisplayed != NO_CUSTOM_AXIS_DISPLAYED)
        {
            CAxis oldAxis = m_chart.GetAxis().GetCustom(m_lastCustomAxisDisplayed);
            oldAxis.SetVisible(false);

            m_lastCustomAxisDisplayed = NO_CUSTOM_AXIS_DISPLAYED;
        }
    }


    //
    // onClickLegend
    //
    void AbstractGraph::onClickLegend(const std::vector<long>& nonActiveSeriesNumbers)
    {
        // If no custom axis is currently displayed, then do nothing
        if(m_lastCustomAxisDisplayed == NO_CUSTOM_AXIS_DISPLAYED)
        {
            return;
        }

        std::vector<long>::const_iterator it = nonActiveSeriesNumbers.begin();
        for(; it != nonActiveSeriesNumbers.end(); ++it)
        {
            // Find the GraphedItem for the given series number
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator itGI = m_graphedItems.begin();
            for(; itGI != m_graphedItems.end(); ++itGI)
            {
                if(itGI->second->getSeriesNumber() == *it)
                {
                    // Check the custom axis number belonging to this series. If it
                    // is currently displaying, turn it off and reset the default axis.
                    if(itGI->second->getCustomAxisNumber() == m_lastCustomAxisDisplayed)
                    {
                        m_graphObserver->resetDefaultAxis();
                        onClickBackground();
                        return;
                    }
                }
            }

        }
    }


    //
    // getAxisLabel
    //
    std::string AbstractGraph::getAxisLabel(long axis, long seriesIndex, long valueIndex)
    {
        if(axis == atBottom)
        {
            return "";
        }

        boost::shared_ptr<GraphedItem> graphedItem;
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator iter = m_graphedItems.begin();
        for(; iter != m_graphedItems.end(); ++iter)
        {
            if(iter->second->getBoolSeriesNumber() == seriesIndex)
            {
                graphedItem = iter->second;
                break;
            }
        }

        if(iter == m_graphedItems.end() || graphedItem.get() == NULL)
        {
            return "";
        }

        // Only want to change the label if it is a custom axis for a boolean/enum/derived item
        double value = m_chart.Series(seriesIndex).GetYValues().GetValue(valueIndex);
        if(graphedItem->getShouldDisplayLabel())
        {
            std::map<int, std::string> labels = graphedItem->getStateLabels();

            // There are meant to be labels but none were retrieved, so just diplay the numerical value
            if(labels.size() == 0)
            {
                std::ostringstream str;
                str << value;
                return str.str();
            }

            std::map<int, std::string>::iterator iter = labels.find(value);
            if(iter != labels.end())
            {
                return iter->second;
            }
        }
        
        return "";
    }


    //
    // getMaximumLimit
    //
    double AbstractGraph::getMaximumLimit(long seriesNumber) const
    {
        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::SERIES_NUMBER_NOT_FOUND, ""));
        }
        
        double maxLimit = graphedItem->getMaximumLimit();

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }

        return maxLimit;
    }
    
    
    //
    // displayCursor
    //
    void AbstractGraph::displayCursor(bool showCursor, long toolNumber)
    {
        FUNCTION_ENTRY("displayCursor");

        if(showCursor == true)
        {
            double minValue = getChart().GetAxis().GetBottom().GetMinimum();
            double maxValue = getChart().GetAxis().GetBottom().GetMaximum();
            double mid = maxValue - (maxValue - minValue) / 2;
            
            getChart().GetTools().GetItems(toolNumber).GetAsTeeCursor().SetXVal(mid);
            getChart().GetTools().GetItems(toolNumber).SetActive(true);
            
            time_t time = TimeFunction::getInstance().ConvertToTimeT(mid);
            
            // Notify the GUI of the cursor value
            getGraphObserver()->setCursorTime(time);
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


    //
    // getScrollableAxesIndexes
    //
    std::vector<long> AbstractGraph::getScrollableAxesIndexes() const
    {
        std::vector<long> axes;
        axes.push_back(atBottom);
        return axes;
    }


    //
    // setPenColour
    //
    void AbstractGraph::setPenColour(COLORREF penColour, int index)
    {
        FUNCTION_ENTRY("setPenColour");

        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.find(index);
        it->second->setPenColour(penColour);

        FUNCTION_EXIT;
    }


    //
    // plotPoint
    //
    void AbstractGraph::plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour)
    {
        // NOTE: This implementation is for graphs that have a date/time X-axis. For the
        // graph types that don't, this method will need to be overridden.

        // Convert the y value into a time
		//TD17883
        //time_t timestamp = TimeFunction::getInstance().ConvertToLocalTimeT(static_cast <long> (x));
		time_t timestamp = static_cast<time_t>(x);
                //++TD17883
        double dataValue = 0;
        COLORREF pointColour = penColour;

        // Move the graph so this timestamp is displayed
        try
        {
            moveGraph(timestamp);
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point: %s", ex.what());
            return;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point");
            return;
        }

        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point. Series number: %ld does not exist", seriesNumber);
            return;
        }
            
        if(m_samplePeriod == LIVE_FIELD_UPDATES)
        {
            // Plot with the Live Update value, which is the value that has been passed in
            dataValue = y;
            
            // Scale the value
            dataValue = scaleValue(dataValue, graphedItem.get());

            // Because the times are stored in database as GMT time, need to convert
            // the time to local
            COleDateTime date(timestamp);
			//TD17883
            //TimeFunction::getInstance().ConvertToLocal(timestamp, date);

            CTime timeWithoutSecs(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), 0);			
			COleDateTime modifiedDate(timeWithoutSecs.GetTime());
            
        
			// TD12474 ++
            //CString testTime = timeWithoutSecs.Format( DATE_TIME_FORMAT );
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
			CString testTime = timeWithoutSecs.Format( dateFormatStr.c_str() );
			// ++ TD12474

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting %.2f at:  %s", dataValue, testTime); //TD17935

            // Lock the graph
            m_graphLock.acquire();

            // Add the point to the graph
            getChart().Series(seriesNumber).AddXY(DATE(modifiedDate), dataValue, "", pointColour);

            //nullValue(timeWithoutSecs.GetTime(), seriesNumber, DEFAULT_SAMPLE_PERIOD);
        
            m_graphLock.release();

            graphedItem->setLastXValue(x);
            graphedItem->setLastYValue(y);
        }
        else
        {
            // Reload data for this series. Chosen to reload here as opposed to retrieving the data
            // from the database for the given time in case there is a time lag.

            getChart().Series(seriesNumber).Clear();

            if(graphedItem->getShouldDisplayLabel())
            {
                getChart().Series(graphedItem->getBoolSeriesNumber()).Clear();
            }

            try
            {
                plotData(graphedItem.get());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                // Instead of throwing an exception here, just fail silently and log an error.
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                    "An unknown error occurred while loading historical data. Display may be incomplete");
            }   
        }

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }
    }


    //
    // AdjustSeriesNumber
    //
    void AbstractGraph::adjustSeriesNumbers(long seriesNum)
    {
        FUNCTION_ENTRY("AdjustSeriesNumber");
        
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getSeriesNumber() > seriesNum) 
            {
                long newSeriesNumber = it->second->getSeriesNumber();
                it->second->setSeriesNumber(newSeriesNumber - 1);
            }

            // If the graphed item doesn't have a bool series number, it will be set to -1.
            if(it->second->getBoolSeriesNumber() > seriesNum) 
            {
                long newSeriesNumber = it->second->getBoolSeriesNumber();
                it->second->setBoolSeriesNumber(newSeriesNumber - 1);
            }
        }

        FUNCTION_EXIT;
    }


    //
    // adjustCustomAxisNumbers
    //
    void AbstractGraph::adjustCustomAxisNumbers(long axisIndex)
    {
        FUNCTION_ENTRY("adjustCustomAxisNumbers");

        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getCustomAxisNumber() > axisIndex) 
            {
                long axisNumber = it->second->getCustomAxisNumber();
                it->second->setCustomAxisNumber(axisNumber - 1);
            }

            // If the graphed item doesn't have a bool axis number, it will be set to -1.
            if(it->second->getBoolCustomAxisNumber() > axisIndex)
            {
                long axisNumber = it->second->getBoolCustomAxisNumber();
                it->second->setBoolCustomAxisNumber(axisNumber - 1);
            }
        }

        // If the custom axis currently displaying belongs to the deleted 
        // series, reset the axis to the default
        if(m_lastCustomAxisDisplayed == axisIndex)
        {
            onClickBackground();
        }

        if(m_lastCustomAxisDisplayed > axisIndex)
        {
            --m_lastCustomAxisDisplayed;
        }

        FUNCTION_EXIT;
    }

    
    //
    // setTitle
    //
    void AbstractGraph::setTitle(const std::string& title)
    {
        FUNCTION_ENTRY("setTitle");

        if(title.empty() == false)
        {
            m_chart.GetHeader().GetText().Clear();

            const int MAX_TITLE_LENGTH = 31;
            
            VARIANT item;
            VariantInit(&item);
            item.vt = VT_BSTR;
            OLECHAR oleChar[MAX_TITLE_LENGTH];

            LPCSTR titleStore = title.c_str();

            int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, titleStore, -1, oleChar, MAX_TITLE_LENGTH);
            _bstr_t bstrMsg = oleChar;
            item.bstrVal = bstrMsg;
            m_chart.GetHeader().GetText().Add(item);

            m_chart.GetHeader().GetFont().SetBold(true);
            m_chart.GetHeader().SetColor(TITLE_COLOUR);
            m_chart.GetHeader().SetVisible(true);
        }
        else
        {
            m_chart.GetHeader().SetVisible(false);
        }

        FUNCTION_EXIT;
    }


    //
    // plotData
    //
    void AbstractGraph::plotData(GraphedItem* graphedItem)
    {
        FUNCTION_ENTRY("plotData");

        time_t newestTime = 0;
        double newestValue = 0;
        long samplePeriod = getSamplePeriod();

        if(samplePeriod == LIVE_FIELD_UPDATES)
        {
            samplePeriod = DEFAULT_SAMPLE_PERIOD;
        }
        
        PlotData data = graphedItem->retrieveHistoricalData(m_startTime, m_endTime, graphedItem->getTypeIdentifier(), m_sampleType, m_samplePeriod);
 
        PlotData::iterator it = data.begin();
        for(; it != data.end(); ++it)
        {
            time_t x = static_cast<time_t>(it->x);

            double y = it->y;

            double dataValue = scaleValue(y, graphedItem);
            
            // Because the times are stored in database as GMT time, need to convert
            // the time to local
			//TD17883
            COleDateTime date(x);			
            //TimeFunction::getInstance().ConvertToLocal(x, date);

            // TD12474 ++
            //CString testTime = date.Format(DATE_TIME_FORMAT);
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
			CString testTime = date.Format(dateFormatStr.c_str());
            // ++ TD12474

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting %.2f (%.2f) at: %s",dataValue, y, testTime); //TD17935

            // Add the point to the graph
            long seriesNum = graphedItem->getSeriesNumber();

            // Lock the graph
            m_graphLock.acquire();
            getChart().Series(seriesNum).AddXY(DATE(date), dataValue, "", graphedItem->getPenColour());
			//TD17883
			//time_t localTime = TimeFunction::getInstance().ConvertToLocalTimeT(x);
			//time_t localTime =static_cast<time_t>(x);
            //nullValue(localTime, seriesNum, samplePeriod);

            m_graphLock.release();

            if(x > newestTime)
            {
                newestTime = x;
                newestValue = y;
            }
        }

        // Add two dummy points so the labels will display
        // on the custom axis
        if(graphedItem->getShouldDisplayLabel())
        {
            time_t rangeSeconds = Utilities::getRangeSeconds(COleDateTime(m_chart.GetAxis().GetBottom().GetMinimum()),
                COleDateTime(m_chart.GetAxis().GetBottom().GetMaximum()));

            setUpDummySeries(graphedItem->getBoolCustomAxisNumber(), graphedItem, m_startTime, rangeSeconds);
        }

        graphedItem->setLastXValue(newestTime);
        graphedItem->setLastYValue(newestValue);

        FUNCTION_EXIT;
    }


    //
    // moveGraph
    //
    void AbstractGraph::moveGraph(time_t timestamp)
    {
        FUNCTION_ENTRY("moveGraph");

        TA_Base_Core::ThreadGuard guard(m_graphLock);

        //
        // Check if the timestamp is valid
        //
        struct tm t;
        
        if(ACE_OS::localtime_r(&timestamp, &t) == NULL)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TIMESTAMP_INVALID, "Received an invalid timestamp"));
        }

        ++timestamp; // Add a second so we definately get the value
        COleDateTime endTime(timestamp); 

		// TD12474 ++
        //CString testTime = endTime.Format(DATE_TIME_FORMAT);
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
		CString testTime = endTime.Format(dateFormatStr.c_str());
        // ++ TD12474

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Graph update time: %s", testTime);

        //
        // Only move the graph if the timestamp is later than the last time shown 
        // or if the graph has not been zoomed
        //
        COleDateTime latestTime = getChart().GetAxis().GetBottom().GetMaximum();

        time_t leftMostTime = TimeFunction::getInstance().ConvertToTimeT(latestTime);


        if(timestamp > leftMostTime && isLive() && !isGraphZoomed())
        {
            //
            // Set the times on the X-Axis
            //
            m_startTime = timestamp - m_rangeSeconds + 1;
            COleDateTime startDateTime(m_startTime);
            m_endTime = timestamp + 1;
            COleDateTime endDateTime(m_endTime);

			// TD12474 ++
			//CString minTime = startDateTime.Format(DATE_TIME_FORMAT);
            //CString maxTime = endDateTime.Format(DATE_TIME_FORMAT);
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 
			CString minTime = startDateTime.Format(dateFormatStr.c_str());
            CString maxTime = endDateTime.Format(dateFormatStr.c_str());
			// ++ TD12474

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New start time: %s, New end time: %s", minTime.GetBuffer(0), maxTime.GetBuffer(0));

            getChart().GetAxis().GetBottom().SetMinMax(DATE(startDateTime), DATE(endDateTime)); // Add 1 second so the time is definately shown
            getChart().GetAxis().GetLeft().SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

            adjustXAxisFormat();

            // For each item, reload the dummy series
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator iter = m_graphedItems.begin();
            for(; iter != m_graphedItems.end(); ++iter)
            {
                if(iter->second->getShouldDisplayLabel())
                {
                    time_t rangeSeconds = Utilities::getRangeSeconds(COleDateTime(m_chart.GetAxis().GetBottom().GetMinimum()),
                        COleDateTime(m_chart.GetAxis().GetBottom().GetMaximum()));
            
                    setUpDummySeries(iter->second->getBoolCustomAxisNumber(), iter->second.get(), m_startTime, Utilities::getRangeSeconds(startDateTime, endDateTime));
                }
            }

            // Notify the GUI so it can update the start and end time display
            m_graphObserver->setStartEndTime(m_startTime, timestamp);
        }
        
        FUNCTION_EXIT;
    }


    //
    // getGraphedItem
    //
    GraphedItem* AbstractGraph::getGraphedItem(long seriesNumber) const
    {
        std::map<int, boost::shared_ptr<GraphedItem> >::const_iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getSeriesNumber() == seriesNumber)
            {
                return it->second.get();
            }
        }

        return(static_cast<GraphedItem*> (NULL));
    }


    //
    // isSeriesVisible
    //
    bool AbstractGraph::isSeriesVisible(long seriesNumber)
    {
        return getChart().Series(seriesNumber).GetActive() != 0;
    }


    //
    // nullValue
    //
    void AbstractGraph::nullValue(time_t latestTime, long seriesNumber, long samplePeriod)
    {
        // This is implemented assuming the X-Axis is in date/time format. If this is not
        // the case, this method must be overridden.

        CSeries series = getChart().Series(seriesNumber);

        CValueList valList = series.GetXValues();
        int count = valList.GetCount();

        if(count > 1)
        {
            COleDateTime currTime(latestTime);
            COleDateTime prevTime = valList.GetValue(count - 2);

            COleDateTimeSpan timeInterval;

            timeInterval = currTime - prevTime;

            const double MIN_SAMPLE_GAP = 1.5;
            //const double MIN_SAMPLE_GAP = 2;
            COleDateTimeSpan minInterval( ((double) MIN_SAMPLE_GAP * (double) samplePeriod) / (double) SECONDS_IN_DAY );
            //COleDateTimeSpan addInterval( (double) samplePeriod / (double) SECONDS_IN_DAY );
            //COleDateTimeSpan addInterval( 8 );
            COleDateTimeSpan addInterval( 0, 0, 0, 1 );

            minInterval += addInterval;

            // If the time interval is too big, add null value to break line
            if( timeInterval > minInterval )
            {
                //COleDateTime addTime = prevTime + addInterval;
                COleDateTime addTime = prevTime + addInterval;
                series.AddNullXY(DATE(addTime), 0.0, "");
            }
        }
    }


    //
    // addCustomAxis
    //
    void AbstractGraph::addCustomAxis(int index)
    {
        long axisIndex = m_chart.GetAxis().AddCustom(false); // False - for Vertical axis
        CAxis theAxis = m_chart.GetAxis().GetCustom(axisIndex);
        theAxis.SetPositionPercent(DEFAULT_AXIS_POSITION_PERCENT);
        theAxis.SetVisible(false);

		//TD17940 - modified computation for scale factors to display more accurate values
		boost::shared_ptr<GraphedItem> graphedItem = m_graphedItems[index];

        double minValue = graphedItem->getMinimumLimit();
        double maxValue = graphedItem->getMaximumLimit();

		double leftAxisMin = m_chart.GetAxis().GetLeft().GetMinimum();
        double leftAxisMax = m_chart.GetAxis().GetLeft().GetMaximum();

		double scaleFactor = graphedItem->isBooleanType() ? ((2.0 - 0.0) / 100.0) : (maxValue - minValue) / 100.0;  
        //++TD17940

        if(graphedItem->getShouldDisplayLabel())
        {
			//TD17940 - positions the dummy series at the center
			theAxis.SetStartPosition(20); //TD18102
			theAxis.SetEndPosition(80); //TD18102
			//++TD17940 
            // TD 8743: Create another custom axis for displaying the boolean labels
            long axisIndex2 = m_chart.GetAxis().AddCustom(false); // False - for Vertical axis
            CAxis theAxis2 = m_chart.GetAxis().GetCustom(axisIndex2);
            //theAxis2.SetPositionPercent(DEFAULT_AXIS_POSITION_PERCENT);
			//TD17940 - positions the dummy series at the center
			theAxis2.SetStartPosition(20); //TD18102
			theAxis2.SetEndPosition(80); //TD18102
			//++TD17940 
            theAxis2.SetVisible(false);
            theAxis2.GetLabels().SetStyle(talMark);

            setUpCustomAxis(theAxis2, index);

            //
            // These calculations are based on the way the scale factor and plotted
            // values are calculated in DataPointInfo and GraphDataPoint.
            //
			//TD17940 - modified computation for scale factors to display more accurate values
            //theAxis2.SetMinMax( minValue, maxValue );
			theAxis2.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);
			//++TD17940
            graphedItem->setBoolCustomAxisNumber(axisIndex2);

        }
        else
        {
            setUpCustomAxis(theAxis, index);
        }

		//TD17940 - modified computation for scale factors to display more accurate values
		theAxis.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);		
        
        long seriesNumber = graphedItem->getSeriesNumber();
        CSeries series = m_chart.Series(seriesNumber);
        series.SetVerticalAxisCustom(axisIndex);
        graphedItem->setCustomAxisNumber(axisIndex);
    }

    
    //
    // setUpCustomAxis
    //
    void AbstractGraph::setUpCustomAxis(CAxis& theAxis, long index)
    {
        theAxis.GetLabels().GetFont().SetBold(true);
        theAxis.GetLabels().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);

        COLORREF colour = m_graphedItems[index]->getPenColour();
        theAxis.GetLabels().GetFont().SetColor(colour);
        theAxis.GetAxisPen().SetColor(colour);
    
        std::string label = m_graphedItems[index]->getCustomAxisLabel();
        theAxis.GetTitle().SetCaption(label.c_str());
        theAxis.GetTitle().GetFont().SetBold(true);
        theAxis.GetTitle().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
        theAxis.GetTitle().GetFont().SetColor(colour);
        theAxis.GetTitle().SetAngle(DEFAULT_VERTICAL_AXIS_ANGLE);
        theAxis.GetTitle().SetVisible(true);

        //theAxis.GetLabels().Add(0, "Test");
    }


    //
    // adjustCustomAxisForBoolean
    //
    void AbstractGraph::adjustCustomAxisForBoolean(double minYValue, double maxYValue, const GraphedItem* graphedItem, CAxis& theAxis)
    {
        /*const double BOOL_MIN = 0;
        const double BOOL_MAX = 1;

        double scaleFactor = graphedItem->getMaximumLimit() - graphedItem->getMinimumLimit();
        double lowValue = (BOOL_MIN - graphedItem->getMinimumLimit() ) * scaleFactor;
        double highValue = (BOOL_MAX - graphedItem->getMinimumLimit() ) * scaleFactor;
        double diff = highValue - lowValue;
        double min = (BOOL_MAX / diff) * minYValue - (BOOL_MAX / diff) * lowValue;
        double max = (BOOL_MAX / diff) * maxYValue - (BOOL_MAX / diff) * lowValue;
        */

		//TD17940 - modified computation for scale factors to display more accurate values
        //const double scaleFactor = (100 / (graphedItem->getMaximumLimit() - graphedItem->getMinimumLimit()));
		const double scaleFactor = (100.0 - 0.0) / (2.0 - 0.0);
        const double offset = 100.0 - scaleFactor * 2.0;

        double axisMin = (minYValue - offset) / scaleFactor;
        double axisMax = (maxYValue - offset) / scaleFactor;
       
        //theAxis.SetMinMax(minValueLeft / scaleFactor, maxValueLeft / scaleFactor);
        theAxis.SetMinMax(axisMin, axisMax);
     
        //theAxis.SetMinMax(min, max);
    }
}

#endif // AbstractGraphCPP_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

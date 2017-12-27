#if !defined(ComparisonGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define ComparisonGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * ComparisonGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ComparisonGraph.cpp $
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

#include "app/trending/trend_viewer/src/ComparisonGraph.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/ColourDefs.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/exceptions/src/TrendViewerException.h"

#include "core/threads/src/Thread.h"

#include "cots/teechart/TeeChartDefines.h"
#include "ace/OS.h"

// TeeChart includes
#include <cots/teechart/valuelist.h>
#include <cots/teechart/axis.h>
#include <cots/teechart/axes.h>
#include <cots/teechart/axislabels.h>
#include <cots/teechart/axistitle.h>
#include <cots/teechart/lineseries.h>
#include <cots/teechart/marks.h>
#include <cots/teechart/pen.h>
#include <cots/teechart/pointer.h>
#include <cots/teechart/teefont.h>
#include <cots/teechart/panel.h>
#include <cots/teechart/axisarrowtool.h>
#include <cots/teechart/toollist.h>
#include <cots/teechart/tools.h>
#include <cots/teechart/scroll.h>
#include <cots/teechart/cursortool.h>


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    //
    // ComparisonGraph
    //
    ComparisonGraph::ComparisonGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime,
        time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
        IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, time_t offset, int leftChartMargin,
        long cursorToolIndex, long axisArrowToolIndex) 
        : AbstractGraph(chart, name, description, startTime, endTime, isLive, 
          samplePeriod, itemStore, archiveDataStore, graphObserver),
          m_offset(offset), 
          m_defaultVerticalAxisIndex(0),
          m_defaultOffsetVerticalAxisIndex(0),
          m_defaultHorizontalAxisIndex(0),
          m_defaultOffsetHorizontalAxisIndex(0),
          m_leftChartMargin(leftChartMargin),
          m_topArrowToolIndex(0),
          m_bottomArrowToolIndex(0),
          m_cursorToolIndex(cursorToolIndex),
          m_defaultAxisArrowToolIndex(axisArrowToolIndex),
          m_cursorDisplayed(false),
          m_startBottomChart(0)
              
    {
        m_minItems = COMPARISON_MIN_ITEMS;
        m_maxItems = COMPARISON_MAX_ITEMS;
    }

    //
    // ~ComparisonGraph
    //
    ComparisonGraph::~ComparisonGraph()
    {
        // If graph is currently in live mode, turn it off
        if(isLive())
        {
            time_t startTime;
            setLiveMode(false, startTime);
        }

        // Remove the arrow tools - these need to be deleted in opposite order
        // of creating them, otherwise the indexes need to be adjusted before
        // deleting them.

        getChart().GetTools().GetItems(m_bottomArrowToolIndex).SetActive(false);
        getChart().GetTools().Delete(m_bottomArrowToolIndex);

        getChart().GetTools().GetItems(m_topArrowToolIndex).SetActive(false);
        getChart().GetTools().Delete(m_topArrowToolIndex);

        for(unsigned int i = 0; i < m_maxItems; ++i)
        {
            deleteFromGraph(i+1);
        }

        // Reset bottom margin
        long bottomChartMargin = getChart().GetPanel().GetMarginBottom();
        getChart().GetPanel().SetMarginBottom(bottomChartMargin - CUSTOM_AXIS_CHART_OFFSET);

        // Reset scroll to both
        getChart().GetScroll().SetEnable(SCROLL_BOTH);

    }


    //
    // setUpGraph
    //
    void ComparisonGraph::setUpGraph()
    {
        FUNCTION_ENTRY("setUpGraph");

        getChart().RemoveAllSeries();

        // For comparison graph, only allow horizontal scroll
        getChart().GetScroll().SetEnable(SCROLL_HORIZONTAL);

        //
        // Turn of the default axes
        //
        getChart().GetAxis().GetLeft().GetLabels().SetVisible(false);
        getChart().GetAxis().GetLeft().GetTitle().SetVisible(false);
        //getChart().GetAxis().GetBottom().GetLabels().SetVisible(true);
        getChart().GetAxis().GetBottom().GetLabels().SetVisible(false);
        getChart().GetAxis().GetBottom().GetTitle().SetVisible(false);

        getChart().GetTools().GetItems(m_defaultAxisArrowToolIndex).SetActive(false);
        
        //
        // Turing off the default axis and title will shift the chart to the left.
        // Reset the left margin of the chart to the original position so the custom
        // axis labels are visible.
        //
        getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);
        long bottomChartMargin = getChart().GetPanel().GetMarginBottom();
        getChart().GetPanel().SetMarginBottom(bottomChartMargin + CUSTOM_AXIS_CHART_OFFSET);

        //
        // Add a two dummy series with custom axes each. Use the custom axes as the
        // default (percentage) axes. These custom axes are used instead of the default
        // chart axes because we need two left and two bottom axes, but the chart only
        // provides one of each.
        //
        long seriesNum = getChart().AddSeries(LINE_GRAPH); 
        CSeries thisSeries = getChart().Series(seriesNum);

        setTitle(getGraphName().c_str());
        thisSeries.GetXValues().SetDateTime(true);
        thisSeries.SetShowInLegend(false);

        //TD17179 - disable engineering values to show as markup texts when setting up the 
		// graph until the cursor tool is enabled
        // fix defect 14171
        //thisSeries.GetMarks().SetVisible(true);
		//++TD17179 - lizettejl        
        addCustomAxis(seriesNum, PERCENTAGE_TITLE, TIME_TITLE, PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX, getStartTime(), getEndTime(), true, true, false);

        seriesNum = getChart().AddSeries(LINE_GRAPH); 
        thisSeries = getChart().Series(seriesNum);
        thisSeries.GetXValues().SetDateTime(true);
        thisSeries.SetShowInLegend(false);

        // fix defect 14171
        thisSeries.GetMarks().SetVisible(false); //TD17179
        addCustomAxis(seriesNum, PERCENTAGE_TITLE, TIME_TITLE, PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX, getStartTime() - m_offset, getEndTime() - m_offset, true, false, false);

        // Set the start and end times on the GUI
        getGraphObserver()->setStartEndTime(getStartTime(), getEndTime());
        
        // Set up the invisible default axes needed for zooming
        getChart().GetAxis().GetBottom().SetMinMax(DATE(COleDateTime(getStartTime())), DATE(COleDateTime(getEndTime())));

        long chartHeight = getChart().GetHeight();
        long chartTop = getChart().GetPanel().GetMarginTop();
        long chartBottom = chartHeight - getChart().GetPanel().GetMarginBottom();
        long graphHeight = chartBottom - chartTop;

        m_startBottomChart = static_cast<long>(chartTop + (COMPARISON_TOP_GRAPH_HOR_POSITION_PERCENTAGE / 100 * graphHeight));

        FUNCTION_EXIT;
    }


    //
    // adjustXAxisTicks
    //
    void ComparisonGraph::adjustXAxisTicks()
    {
        FUNCTION_ENTRY("adjustXAxisTicks");

        const int NUMBER_HORIZONTAL_AXES = 2;
        long horizontalAxes[] = 
        { 
            m_defaultHorizontalAxisIndex, 
            m_defaultOffsetHorizontalAxisIndex 
        };

        for(int i = 0; i < NUMBER_HORIZONTAL_AXES; ++i)
        {
            CAxis axis = getChart().GetAxis().GetCustom(horizontalAxes[i]);
            double bottomIncrement = axis.CalcIncrement();
        
            // For 5 or 10 second label increments
            if(bottomIncrement < TICKS_5_OR_10_SECONDS)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_4);
            }
            // For 30 or 15 second label increments
            else if(bottomIncrement < TICKS_15_OR_30_SECONDS)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_2);
            }
            // For 1 minute label increments
            else if(bottomIncrement < TICKS_1_MINUTE)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_5);
            }
            // For 5 or 10 minute label increments
            else if(bottomIncrement < TICKS_5_OR_10_MINUTES)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_4);
            }
            // For 15 or 30 minute label increments
            else if(bottomIncrement < TICKS_15_OR_30_MINUTES)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_2);
            }
            // For 1 hour label increments
            else if(bottomIncrement < TICKS_1_HOUR)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_3);
            }
            // For 6 or 12 hour label increments
            else if( bottomIncrement < TICKS_6_OR_12_HOUR)
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_5);
            }
            else
            {
	            axis.SetMinorTickCount(MINOR_TICK_COUNT_3);
            }
        }

        FUNCTION_EXIT;
    }


    //
    // adjustXAxisFormat
    //
    void ComparisonGraph::adjustXAxisFormat()
    {
        FUNCTION_ENTRY("adjustXAxisFormat");

        //
        // Check the time scale we are using and change if appropriate
        //

        const int NUMBER_HORIZONTAL_AXES = 2;
        long horizontalAxes[] = 
        { 
            m_defaultHorizontalAxisIndex, 
            m_defaultOffsetHorizontalAxisIndex 
        };

        for(int i = 0; i < NUMBER_HORIZONTAL_AXES; ++i)
        {
            CAxis axis = getChart().GetAxis().GetCustom(horizontalAxes[i]);
            double minValue = axis.GetMinimum();
            double maxValue = axis.GetMaximum();

            if((maxValue - minValue) < TIME_AXIS_15_MINUTES)
            {
                // Less than 15 minutes
	            axis.GetLabels().SetDateTimeFormat( TIME_ONLY_WITH_SECONDS );
            }
            else if((maxValue - minValue) <= TIME_AXIS_1_DAY)
            {
                // Less than a day
                if(m_offset < SECONDS_IN_DAY)
                {
					// TD17174++		
	                //axis.GetLabels().SetDateTimeFormat( TIME_ONLY_WITHOUT_SECONDS );
					axis.GetLabels().SetDateTimeFormat( TIME_ONLY_WITH_SECONDS );
					// ++TD17174
                }
                else
                {
					//TD12474 ++
					//axis.GetLabels().SetDateTimeFormat( DATE_AND_TIME );
					//TD17174++
					//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat9); 
					//axis.GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );					
					axis.GetLabels().SetDateTimeFormat( DATE_TIME_WITH_SECONDS);
					//++TD17174				
					//++ TD12474
                }
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
				//axis.GetLabels().SetDateTimeFormat( DATE_AND_TIME );
				std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat9); 
				axis.GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );
				//++ TD12474
            }
            else if((maxValue - minValue) <= TIME_AXIS_1_YEAR)
            {
                // Less than a year
				//TD12474 ++
				//axis.GetLabels().SetDateTimeFormat( DATE_ONLY_WITH_DAYS );
				std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat5); 
				axis.GetLabels().SetDateTimeFormat( dateFormatStr.c_str()  );
				//++ TD12474
            }
            else
            {
				//TD12474 ++
				//axis.GetLabels().SetDateTimeFormat( DATE_ONLY_WITHOUT_DAYS );
				std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec6); 
				axis.GetLabels().SetDateTimeFormat( dateFormatStr.c_str() );
				//++ TD12474
            }
        }

        FUNCTION_EXIT;
    }


    //
    // plotData
    //
    void ComparisonGraph::plotData(GraphedItem* graphedItem)
    {
        FUNCTION_ENTRY("plotData");

        time_t newestTime = 0;
        double newestValue = 0;
        long samplePeriod = getSamplePeriod();
        if(samplePeriod == LIVE_FIELD_UPDATES)
        {
            samplePeriod = DEFAULT_SAMPLE_PERIOD;
        }

        // Get the series number of the offset series and plot data
        long seriesNum = graphedItem->getSeriesNumber();
        std::map<long, long>::iterator bottomIt = m_topBottomSeriesNumberMap.find(seriesNum);
        TA_ASSERT(bottomIt != m_topBottomSeriesNumberMap.end(), "Offset series has not been added");
        long bottomSeriesNum = bottomIt->second;

        // Clear the offset series
        getChart().Series(bottomSeriesNum).Clear();

        if(graphedItem->getShouldDisplayLabel())
        {
            std::map<long, long>::iterator bottomIt = m_topBottomSeriesNumberMap.find(graphedItem->getBoolSeriesNumber());
            
            if(bottomIt != m_topBottomSeriesNumberMap.end())
            {
                long bottomSeriesNum = bottomIt->second;
                getChart().Series(bottomSeriesNum).Clear();
            }
        }
        
        PlotData data = graphedItem->retrieveHistoricalData(getStartTime(), getEndTime(), graphedItem->getTypeIdentifier(), getSampleType(), getSamplePeriod());
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
            // ++TD12474

		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting %lf at: %s", dataValue, testTime);

            // Add the point to the graph
            getChart().Series(seriesNum).AddXY(DATE(date), dataValue, "", graphedItem->getPenColour());

			//TD17883
            //time_t localTime = TimeFunction::getInstance().ConvertToLocalTimeT(x);
			time_t localTime =static_cast<time_t>(x);
            //nullValue(localTime, seriesNum, samplePeriod);

            if(x > newestTime)
            {
                newestTime = x;
                newestValue = y;
            }
        }

        time_t offsetStart = getStartTime() - m_offset;
        time_t offsetEnd = getEndTime() - m_offset;
        PlotData offsetData = graphedItem->retrieveHistoricalData(offsetStart, offsetEnd, graphedItem->getTypeIdentifier(), getSampleType(), getSamplePeriod());
        PlotData::iterator offsetIt = offsetData.begin();
        for(; offsetIt != offsetData.end(); ++offsetIt)
        {
            time_t x = static_cast<time_t>(offsetIt->x);
            double y = offsetIt->y;

            double dataValue = scaleValue(y, graphedItem);
            //TD17883
            COleDateTime date(x);
            //TimeFunction::getInstance().ConvertToLocal(x, date);
            
            // TD12474 ++
            //CString testTime = date.Format(DATE_TIME_FORMAT);
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
            CString testTime = date.Format(dateFormatStr.c_str());
            // ++ TD12474

		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting Offset at: %s", testTime);

            // Add the point to the graph
            getChart().Series(bottomSeriesNum).AddXY(DATE(date), dataValue, "", graphedItem->getPenColour());

			//TD17883
            //time_t localTime = TimeFunction::getInstance().ConvertToLocalTimeT(x);
			time_t localTime = static_cast<time_t>(x);
            //nullValue(localTime, bottomSeriesNum, samplePeriod);
        }

        // Add two dummy points so the labels will display
        // on the custom axis
        if(graphedItem->getShouldDisplayLabel())
        {
            time_t rangeSeconds = Utilities::getRangeSeconds(COleDateTime(getChart().GetAxis().GetBottom().GetMinimum()),
                COleDateTime(getChart().GetAxis().GetBottom().GetMaximum()));

            setUpDummySeries(graphedItem->getBoolCustomAxisNumber(), graphedItem, getStartTime(), rangeSeconds);
            //setUpDummySeries(m_boolTopBottomAxisIndexMap[graphedItem->getBoolCustomAxisNumber()], graphedItem);
        }

        graphedItem->setLastXValue(newestTime);
        graphedItem->setLastYValue(newestValue);

        FUNCTION_EXIT;
    }
    
    
    //
    // plotPoint
    //
    void ComparisonGraph::plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour)
    {
        // Convert the y value into a time
		//TD17883
        //time_t timestamp = TimeFunction::getInstance().ConvertToLocalTimeT(static_cast <long> (x));
		time_t timestamp = static_cast<time_t>(x);
        double dataValue = y;
        COLORREF pointColour = penColour;

        try
        {
            moveGraph(timestamp);
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point: %s", ex.what());
            return;
        }

        long offsetSeriesNum = m_topBottomSeriesNumberMap[seriesNumber];

        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point. Series number: %ld does not exist", seriesNumber);
            return;
        }

        if(getSamplePeriod() == LIVE_FIELD_UPDATES)
        {
            // Plot with the latest known value, which is the value that has been passed in
            dataValue = y;

            // Scale the value
            dataValue = scaleValue(dataValue, graphedItem.get());

            COleDateTime date(timestamp);
            CTime timeWithoutSecs(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), 0);
            COleDateTime modifiedDate(timeWithoutSecs.GetTime());
        
            // TD12474 ++
            //CString testTime = timeWithoutSecs.Format( DATE_TIME_FORMAT );
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
            CString testTime = timeWithoutSecs.Format( dateFormatStr.c_str() );
            // ++ TD12474

		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting at: %s", testTime);

            // Because the times are stored in database as GMT time, need to convert
            // the time to local
            //TimeFunction::getInstance().ConvertToLocal(timeWithoutSecs.GetTime(), date);

            // Add the point to the graph
            getChart().Series(seriesNumber).AddXY(DATE(modifiedDate), dataValue, "", pointColour);

            //nullValue(timeWithoutSecs.GetTime(), seriesNumber, DEFAULT_SAMPLE_PERIOD);

            // Add the point to offset graph
            time_t offsetTime = timestamp - m_offset;

            COleDateTime offsetDate(offsetTime);
            CTime offsetTimeWithoutSecs(offsetDate.GetYear(), offsetDate.GetMonth(), offsetDate.GetDay(), 
                offsetDate.GetHour(), offsetDate.GetMinute(), 0);
            COleDateTime modifiedOffsetDate(offsetTimeWithoutSecs.GetTime());

            double offsetValue = 0;
            COLORREF offsetColour = penColour;
            retrieveData(offsetSeriesNum, offsetTime, offsetValue, offsetColour, true);

            // Scale the value
            offsetValue = scaleValue(offsetValue, graphedItem.get());

            getChart().Series(offsetSeriesNum).AddXY(DATE(modifiedOffsetDate), offsetValue, "", offsetColour);

            //nullValue(offsetTimeWithoutSecs.GetTime(), offsetSeriesNum, DEFAULT_SAMPLE_PERIOD);
        
            graphedItem->setLastXValue(x);
            graphedItem->setLastYValue(y);
        }
        else
        {
            // Reload data for this series. Chosen to reload here as opposed to retrieving the data
            // from the database for the given time in case there is a time lag.

            getChart().Series(seriesNumber).Clear();
            getChart().Series(offsetSeriesNum).Clear();

            if(graphedItem->getShouldDisplayLabel())
            {
                long topSeriesNumber = graphedItem->getBoolSeriesNumber();
                getChart().Series(topSeriesNumber).Clear();
                getChart().Series(m_topBottomSeriesNumberMap[topSeriesNumber]).Clear();
            }

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
    }
    

    //
    // updatePenStyle
    //
    void ComparisonGraph::updatePenStyle(bool originalPenStyle)
    {
        if(originalPenStyle == true)
        {
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);

                long offsetSeriesNum = m_topBottomSeriesNumberMap[seriesNum];
                getChart().Series(offsetSeriesNum).GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);
            }
        }
        else
        {
            int penStyle = 0;
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsLine().GetLinePen().SetStyle(penStyle % MAX_PEN_STYLES);

                long offsetSeriesNum = m_topBottomSeriesNumberMap[seriesNum];
                getChart().Series(offsetSeriesNum).GetAsLine().GetLinePen().SetStyle(penStyle % MAX_PEN_STYLES);

                ++penStyle;

                // TES 594: Remove styles that don't have connecting lines
				// TD18006 - do not include smallDots pen pattern
                if(penStyle == psDot || penStyle == psClear || penStyle == psSmallDots)

                {
                    ++penStyle;
                }
            }
        }
    }


    //
    // onClickBackground
    //
    void ComparisonGraph::onClickBackground()
    {
        getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);

        long customAxisNumber = getLastCustomAxisDisplayed();
        if(customAxisNumber != NO_CUSTOM_AXIS_DISPLAYED)
        {
            CAxis oldAxis = getChart().GetAxis().GetCustom(customAxisNumber);
            oldAxis.SetVisible(false);

            // Get the series number associated with the axis
            long seriesNumber = -1;
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                if(it->second->getCustomAxisNumber() == customAxisNumber)
                {
                    seriesNumber = it->second->getSeriesNumber();
                    break;
                }

                if(it->second->getBoolCustomAxisNumber() == customAxisNumber)
                {
                    seriesNumber = it->second->getSeriesNumber();
                    break;
                }
            }

            TA_ASSERT(it != m_graphedItems.end(), "There was no series number associated with the custom axis.");

            // Hide the custom axis of the offset graph
            long offsetAxisNumber = -1;
            if(it->second->getShouldDisplayLabel())
            {
                std::map<long, long>::iterator iter = m_boolTopBottomAxisIndexMap.find(customAxisNumber);
                
                TA_ASSERT(iter != m_boolTopBottomAxisIndexMap.end(), "No corresponding bottom boolean axis.");

                offsetAxisNumber = iter->second;
            }
            else
            {
                long offsetSeriesNumber = m_topBottomSeriesNumberMap[seriesNumber];
                offsetAxisNumber = getChart().Series(offsetSeriesNumber).GetVerticalAxisCustom();
            }

            CAxis offsetAxis = getChart().GetAxis().GetCustom(offsetAxisNumber);
            offsetAxis.SetVisible(false);

            setLastCustomAxisDisplayed(NO_CUSTOM_AXIS_DISPLAYED);

            // Display the default axes
            CAxis defaultVerticalAxis = getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex);
            defaultVerticalAxis.SetVisible(true);

            CAxis defaultOffsetVerticalAxis = getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex);
            defaultOffsetVerticalAxis.SetVisible(true);
        }
    }


    //
    // onClickSeries
    //
    void ComparisonGraph::onClickSeries(long seriesNumber)
    {
        // If the seriesNumber is "1" then we know that this was called from addItems() in HistoryViewerDlg
        // because there is only 1 item displayed. Because the comparison graph has 2 series per item, the
        // dummy series numbers will be 0 and 1, so change the series number to "2" so we are dealing with
        // the correct series.
        long actualSeriesNumber = seriesNumber;
        if(seriesNumber == 1)
        {
            actualSeriesNumber = 2;
        }

        // Increase the left margin so the custom axis label will show
        getChart().GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET + 2);

        bool topClicked = true;
		//TD18010++ - correct the display for engg values
		long topSeriesNumber = 0; 
		long bottomSeriesNumber = 1;
		//++TD18010
        // Determine whether a series in the top or bottom graph was clicked
        std::map<long, long>::iterator seriesIt = m_topBottomSeriesNumberMap.find(actualSeriesNumber);
        if(seriesIt == m_topBottomSeriesNumberMap.end())
        {
            topClicked = false;
        }

        GraphedItem* graphedItem = NULL;
        if(topClicked)
        {
            graphedItem = getGraphedItem(actualSeriesNumber);
			//TD18010 - initialize the top and bottom series numbers
			topSeriesNumber = actualSeriesNumber;
			bottomSeriesNumber = m_topBottomSeriesNumberMap[actualSeriesNumber];
        }
        else
        {
            // Bottom series clicked
            std::map<long,long >::iterator it = m_topBottomSeriesNumberMap.begin();
            for(; it != m_topBottomSeriesNumberMap.end(); ++it)
            {
                if(it->second == actualSeriesNumber)
                {
					//TD18010 - initialize the top and bottom series numbers
                    topSeriesNumber = it->first;
					bottomSeriesNumber = actualSeriesNumber;
                    graphedItem = getGraphedItem(topSeriesNumber);
                    break;
                }
            }

            TA_ASSERT(it != m_topBottomSeriesNumberMap.end(), "Series number does not exist.");
        }
        

		//TD18010 - identify the last custom axis and the corresponding offset axis 
        long lastCustomAxisNumber = getLastCustomAxisDisplayed();
		long lastOffsetAxisNumber = -1;
        if(lastCustomAxisNumber == NO_CUSTOM_AXIS_DISPLAYED)
        {
            lastCustomAxisNumber = m_defaultVerticalAxisIndex;
			lastOffsetAxisNumber = m_defaultOffsetVerticalAxisIndex;            
        }
		else
		{
			 // Get the series number associated with the axis
            long lastAssociatedSeries = -1;
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                if(it->second->getCustomAxisNumber() == lastCustomAxisNumber)
                {
                    lastAssociatedSeries = it->second->getSeriesNumber();
                    break;
                }

                if(it->second->getBoolCustomAxisNumber() == lastCustomAxisNumber)
                {
                    lastAssociatedSeries = it->second->getSeriesNumber();
                    break;
                }
            }

            TA_ASSERT(it != m_graphedItems.end(), "There was no series number associated with the custom axis");
            
			// the last custom axis stored is always the axis number of the top series
            if(graphedItem->getShouldDisplayLabel())
            {
                lastCustomAxisNumber = graphedItem->getBoolCustomAxisNumber();
                lastOffsetAxisNumber = m_boolTopBottomAxisIndexMap[lastCustomAxisNumber];
            }
            else
            {
				lastCustomAxisNumber = getChart().Series(lastAssociatedSeries).GetVerticalAxisCustom();
                lastOffsetAxisNumber = m_topBottomSeriesNumberMap[lastAssociatedSeries];
            }
        }
        //TD18010 - Turn off both vertical axes
        getChart().GetAxis().GetCustom(lastCustomAxisNumber).SetVisible(false);
        getChart().GetAxis().GetCustom(lastOffsetAxisNumber).SetVisible(false);


        long axisIndex = -1;		

		//TD18010 - whether the top or bottom series is clicked, the corresponding series
		// on the other graph would also be highlighted/displayed
        if(graphedItem->getShouldDisplayLabel())
        {
			axisIndex = graphedItem->getBoolCustomAxisNumber();

			// Display top
            CAxis theAxis = getChart().GetAxis().GetCustom(axisIndex);
			theAxis.SetVisible(true);

			// Display bottom
			CAxis bottomAxis = getChart().GetAxis().GetCustom(m_boolTopBottomAxisIndexMap[axisIndex]);
			bottomAxis.SetVisible(true);
        }
        else
        {            
            CSeries series = getChart().Series(topSeriesNumber);
            axisIndex = series.GetVerticalAxisCustom();

			// Display top
			CAxis theAxis = getChart().GetAxis().GetCustom(axisIndex);
			theAxis.SetVisible(true);           

			// Display bottom
			CSeries bottomSeries = getChart().Series(bottomSeriesNumber);
			long bottomAxisIndex = bottomSeries.GetVerticalAxisCustom();
			CAxis theBottomAxis = getChart().GetAxis().GetCustom(bottomAxisIndex);
			theBottomAxis.SetVisible(true);   			
        }
		//++TD18010
        setLastCustomAxisDisplayed(axisIndex);
    }


    //
    // onClickLegend
    //
    void ComparisonGraph::onClickLegend(const std::vector<long>& nonActiveSeriesNumbers)
    {
        // Temporarily show all the offset series. The appropriate ones will be turned off.
        std::map<long, long>::iterator seriesIt = m_topBottomSeriesNumberMap.begin();
        for(; seriesIt != m_topBottomSeriesNumberMap.end(); ++seriesIt)
        {
            getChart().Series(seriesIt->second).SetActive(true);
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
                    if(itGI->second->getCustomAxisNumber() == getLastCustomAxisDisplayed())
                    {
                        onClickBackground();
                    }

                    // Also turn off the offset series
                    long offsetSeriesNum = m_topBottomSeriesNumberMap[*it];
                    getChart().Series(offsetSeriesNum).SetActive(false);
                }
            }
        }
    }


    //
    // onScroll
    //
    void ComparisonGraph::onScroll(time_t& startDateTime, long& rangeSeconds, long axis)
    {
        TA_Base_Core::ThreadGuard guard(m_graphLock);

        time_t startTime = getStartTime();
        if(axis == static_cast<long>(m_defaultHorizontalAxisIndex + atCustom))
        {
            // Top graph has been scrolled
            
            // Get the new min and max value
            double minValue = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMinimum();
            double maxValue = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMaximum();

            // Set Bottom axis - used for zooming
            getChart().GetAxis().GetBottom().SetMinMax(DATE(COleDateTime(minValue)), DATE(COleDateTime(maxValue)));
            
            time_t minTime = TimeFunction::getInstance().ConvertToTimeT(minValue);
            time_t maxTime = TimeFunction::getInstance().ConvertToTimeT(maxValue);

            COleDateTime newMin(minTime - m_offset);
            COleDateTime newMax(maxTime - m_offset);

            // Set the new min and max for the bottom graph
            getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).SetMinMax(DATE(newMin), DATE(newMax));
            
            startTime = minTime;

            // Scroll the custom axes
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNumber = it->second->getSeriesNumber();
                long axisIndex = getChart().Series(seriesNumber).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(axisIndex).SetMinMax(minValue, maxValue);

                long offsetSeriesIndex = m_topBottomSeriesNumberMap[seriesNumber];
                long offsetAxisIndex = getChart().Series(offsetSeriesIndex).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(offsetAxisIndex).SetMinMax(DATE(newMin), DATE(newMax));
            }

        }
        else if(axis == static_cast<long>(m_defaultOffsetHorizontalAxisIndex + atCustom))
        {
            // Bottom graph has been scrolled
            
            // Get the new min and max value
            double minValue = getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).GetMinimum();
            double maxValue = getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).GetMaximum();
        
            time_t minTime = TimeFunction::getInstance().ConvertToTimeT(minValue);
            time_t maxTime = TimeFunction::getInstance().ConvertToTimeT(maxValue);

            COleDateTime newMin(minTime + m_offset);
            COleDateTime newMax(maxTime + m_offset);

            // Set the new min and max for the top graph
            getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).SetMinMax(DATE(newMin), DATE(newMax));

            // Set Bottom axis - used for zooming
            getChart().GetAxis().GetBottom().SetMinMax(DATE(newMin), DATE(newMax));

            startTime = minTime + m_offset;

            // Scroll the custom axes
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNumber = it->second->getSeriesNumber();
                long axisIndex = getChart().Series(seriesNumber).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(axisIndex).SetMinMax(DATE(newMin), DATE(newMax));
                
                long offsetSeriesIndex = m_topBottomSeriesNumberMap[seriesNumber];
                long offsetAxisIndex = getChart().Series(offsetSeriesIndex).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(offsetAxisIndex).SetMinMax(minValue, maxValue);
            }
        }
        else if(axis == 0)
        {
            double minValue = getChart().GetAxis().GetBottom().GetMinimum();
            double maxValue = getChart().GetAxis().GetBottom().GetMaximum();

            // Set the default axis - used for zooming
            getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).SetMinMax(DATE(COleDateTime(minValue)), DATE(COleDateTime(maxValue)));

            time_t minTime = TimeFunction::getInstance().ConvertToTimeT(minValue);
            time_t maxTime = TimeFunction::getInstance().ConvertToTimeT(maxValue);

            COleDateTime newMin(minTime - m_offset);
            COleDateTime newMax(maxTime - m_offset);

            // Set the new min and max for the bottom graph
            getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).SetMinMax(DATE(newMin), DATE(newMax));
            
            startTime = minTime;

            // Scroll the custom axes
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNumber = it->second->getSeriesNumber();
                long axisIndex = getChart().Series(seriesNumber).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(axisIndex).SetMinMax(minValue, maxValue);

                long offsetSeriesIndex = m_topBottomSeriesNumberMap[seriesNumber];
                long offsetAxisIndex = getChart().Series(offsetSeriesIndex).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(offsetAxisIndex).SetMinMax(DATE(newMin), DATE(newMax));
            }
        }

        setStartTime(startTime);
        setEndTime(startTime + getRangeSeconds());
        startDateTime = startTime;
        rangeSeconds = getRangeSeconds();
        
        reloadGraph();

        // Notify the GUI so it can update the start and end time display
        getGraphObserver()->setStartEndTime(getStartTime(), getEndTime());
    }
    
    
    //
    // onZoom
    //
    void ComparisonGraph::onZoom(time_t& startDateTime, long& rangeSeconds, long x0, long y0, long x1, long y1, bool reset)
    {
        TA_Base_Core::ThreadGuard guard(m_graphLock);

        setZoom(!reset);
        adjustXAxisFormat();

        time_t minValue = 0;
        time_t maxValue = 0;

        //
        // Get the new min and max value for horizontal axis
        //
        COleDateTime minTime = getChart().GetAxis().GetBottom().GetMinimum();
        COleDateTime maxTime = getChart().GetAxis().GetBottom().GetMaximum();
        
        minValue = TimeFunction::getInstance().ConvertToTimeT(minTime);
        maxValue = TimeFunction::getInstance().ConvertToTimeT(maxTime);

        if(reset == true)
        {
            setStartTime(minValue);
            setEndTime(maxValue);
            reloadGraph();
        }

        rangeSeconds = maxValue - minValue;

        //
        // Can only zoom down to 1 minute minimum
        //
        if(SECONDS_IN_MINUTE > rangeSeconds)
        {
            //
	        // Adjust to one minute
            //
	        double centreValue = minValue + (maxValue - minValue) / 2.0;

            minValue = static_cast<long>(centreValue - SECONDS_IN_MINUTE / 2.0);
	        maxValue = static_cast<long>(centreValue + SECONDS_IN_MINUTE / 2.0);
	        rangeSeconds = static_cast<long>(SECONDS_IN_MINUTE);
	        
            getChart().GetAxis().GetBottom().SetMinMax(DATE(COleDateTime(minValue)), DATE(COleDateTime(maxValue)));
        }

        COleDateTime adjustedMin = getChart().GetAxis().GetBottom().GetMinimum();
        COleDateTime adjustedMax = getChart().GetAxis().GetBottom().GetMaximum();
        
        time_t offsettedTimeMin = TimeFunction::getInstance().ConvertToTimeT(adjustedMin) - m_offset;
        time_t offsettedTimeMax = TimeFunction::getInstance().ConvertToTimeT(adjustedMax) - m_offset;

        if(reset == true)
        {
            getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);
            getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
	        for(; it != m_graphedItems.end(); ++it)
	        {
                double min = it->second->getMinimumLimit();
                double max = it->second->getMaximumLimit();
                
                long axisIndex = -1;
                long offsetAxisIndex = -1;
                if(it->second->getShouldDisplayLabel())
                {
                    axisIndex = it->second->getBoolCustomAxisNumber();
                    offsetAxisIndex = m_boolTopBottomAxisIndexMap[axisIndex];
                }
                else
                {
                    long seriesNum = it->second->getSeriesNumber();
                    axisIndex = getChart().Series(seriesNum).GetVerticalAxisCustom();

                    long offsetSeriesNum = m_topBottomSeriesNumberMap[seriesNum];
                    offsetAxisIndex = getChart().Series(offsetSeriesNum).GetVerticalAxisCustom();
                }
                
                getChart().GetAxis().GetCustom(axisIndex).SetMinMax(min, max);
                getChart().GetAxis().GetCustom(offsetAxisIndex).SetMinMax(min, max);
            }
        }
        else
        {
            //
            // Get the new min and max value for vertical axis
            //

            double vertMin = 0;
            double vertMax = 0;
        
            if(y0 >= m_startBottomChart)
            {
                // bottom graph zoomed
                vertMin = getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).CalcPosPoint(y1);
                vertMax = getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).CalcPosPoint(y0);

            }
        
            if(y0 < m_startBottomChart && y1 < m_startBottomChart)
            {
                // top graph zoomed
                vertMin = getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).CalcPosPoint(y1);
                vertMax = getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).CalcPosPoint(y0);
                //vertMin = getChart().GetAxis().GetLeft().GetMinimum();
                //vertMax = getChart().GetAxis().GetLeft().GetMaximum();
            }

            if(y0 < m_startBottomChart && y1 > m_startBottomChart)
            {
                // Zoomed in between - don't zoom - reset left and bottom axes
                COleDateTime minTime = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMinimum();
                COleDateTime maxTime = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMaximum();
                //getChart().GetAxis().GetBottom().SetMinMax(DATE(COleDateTime(minValue)), DATE(COleDateTime(maxValue)));
                getChart().GetAxis().GetBottom().SetMinMax(DATE(minTime), DATE(maxTime));
            
                getChart().GetAxis().GetLeft().SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

                return;
            }
        
            // Adjust default Horizontal & Vertical axes
            getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).SetMinMax(DATE(adjustedMin), DATE(adjustedMax));
            getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).SetMinMax(vertMin, vertMax);
            getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).SetMinMax(DATE(COleDateTime(offsettedTimeMin)), DATE(COleDateTime(offsettedTimeMax)));
            getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).SetMinMax(vertMin, vertMax);

            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
	        for(; it != m_graphedItems.end(); ++it)
	        {
                // Adjust the vertical scales depending on the scale factors
                double adjustedVertMin = vertMin;
                double adjustedVertMax = vertMax;
                if(it->second->isBooleanType() == false)
	            {
                    const double scaleFactor = it->second->getScaleFactor();
                    adjustedVertMin = vertMin / scaleFactor;
                    adjustedVertMax = vertMax / scaleFactor;
                }
                else
                {
					//TD17940++
                    const double BOOL_MIN = 0;
                    const double BOOL_MAX = 1;

                    double scaleFactor = it->second->getMaximumLimit() - it->second->getMinimumLimit();
                    double lowValue = (BOOL_MIN - it->second->getMinimumLimit() ) * scaleFactor;
                    double highValue = (BOOL_MAX - it->second->getMinimumLimit() ) * scaleFactor;
                    double diff = highValue - lowValue;
                    adjustedVertMin = (BOOL_MAX / diff) * vertMin - (BOOL_MAX / diff) * lowValue;
                    adjustedVertMax = (BOOL_MAX / diff) * adjustedVertMax - (BOOL_MAX / diff) * lowValue;
					//const double scaleFactor = (100.0 - 0.0) / (2.0 - 0.0);
					//const double offset = 100.0 - scaleFactor * 2.0;
					//adjustedVertMin = (vertMin - offset) / scaleFactor;
					//adjustedVertMax = (vertMax - offset) / scaleFactor;
					//++TD17940
                }

                long seriesNum = it->second->getSeriesNumber();

                // Adjust the top graph axes
                long horAxisIndex = getChart().Series(seriesNum).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(horAxisIndex).SetMinMax(DATE(adjustedMin), DATE(adjustedMax));

                long vertAxisIndex = -1;
                if(it->second->getShouldDisplayLabel())
                {
                    vertAxisIndex = it->second->getBoolCustomAxisNumber();
                }
                else
                {
                    vertAxisIndex = getChart().Series(seriesNum).GetVerticalAxisCustom();
                }

                getChart().GetAxis().GetCustom(vertAxisIndex).SetMinMax(adjustedVertMin, adjustedVertMax);

                // Adjust the offset axes
                long offsetSeriesnum = m_topBottomSeriesNumberMap[seriesNum];

                long offsetHorAxisIndex = getChart().Series(offsetSeriesnum).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(offsetHorAxisIndex).SetMinMax(DATE(COleDateTime(offsettedTimeMin)), DATE(COleDateTime(offsettedTimeMax)));

                long offsetVertAxisIndex = -1;
                if(it->second->getShouldDisplayLabel())
                {
                    offsetVertAxisIndex = m_boolTopBottomAxisIndexMap[vertAxisIndex];
                }
                else
                {
                    offsetVertAxisIndex = getChart().Series(seriesNum).GetVerticalAxisCustom();
                }
                
                getChart().GetAxis().GetCustom(offsetVertAxisIndex).SetMinMax(adjustedVertMin, adjustedVertMax);
            }
        }

        startDateTime = minValue;
        // Notify the GUI so it can update the start and end time display
        getGraphObserver()->setStartEndTime(startDateTime, startDateTime + rangeSeconds);
    }


    //
    // getScrollableAxesIndexes
    //
    std::vector<long> ComparisonGraph::getScrollableAxesIndexes() const
    {
        std::vector<long> axes;
        axes.push_back(m_defaultHorizontalAxisIndex + atCustom);
        axes.push_back(m_defaultOffsetHorizontalAxisIndex + atCustom);
        return axes;
    }


    //
    // displayCursor
    //
    void ComparisonGraph::displayCursor(bool showCursor, long toolNumber)
    {
        FUNCTION_ENTRY("displayCursor");

        if(showCursor == true)
        {
            m_cursorDisplayed = true;

            double minValue = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMinimum();
            double maxValue = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMaximum();
            double mid = maxValue - (maxValue - minValue) / 2;
            
            getChart().GetTools().GetItems(m_cursorToolIndex).GetAsTeeCursor().SetXVal(mid);
            getChart().GetTools().GetItems(m_cursorToolIndex).SetActive(true);
            
            time_t time = TimeFunction::getInstance().ConvertToTimeT(mid);
            
            // Notify the GUI of the cursor value            			
			getGraphObserver()->setCursorTime(time);
			//TD18010
            //getGraphObserver()->setCursorTime(time - m_offset);			
			getGraphObserver()->setOffsetTime(time - m_offset);
			//++TD18010
        }
        else
        {
            m_cursorDisplayed = false;

            getChart().GetTools().GetItems(m_cursorToolIndex).SetActive(false);
        }

		//TD17179 - disable engineering values to be shown once cursor is disabled
		std::map<int, boost::shared_ptr<GraphedItem> >::const_iterator it = m_graphedItems.begin();
		for(; it != m_graphedItems.end(); ++it)
		{
			//TD17940 - lower graph must also display tootlips on cursor 
			long seriesNum = it->second->getSeriesNumber();
			getChart().Series(seriesNum).GetMarks().SetVisible(showCursor);				
			std::map<long, long>::iterator seriesIt = m_topBottomSeriesNumberMap.find(seriesNum);
			if(seriesIt != m_topBottomSeriesNumberMap.end())
			{
				getChart().Series(seriesIt->second).GetMarks().SetVisible(showCursor);
			}
			//++TD17940
		}
		//++TD17179 - lizettejl	

        FUNCTION_EXIT;
    }


    //
    // getMaximumLimit
    //
    double ComparisonGraph::getMaximumLimit(long seriesNumber) const
    {
        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::SERIES_NUMBER_NOT_FOUND, ""));
        }

        double maxLimit(0);

        std::map<long, long>::const_iterator it = m_topBottomSeriesNumberMap.find(seriesNumber);
        if(it == m_topBottomSeriesNumberMap.end())
        {
            it = m_topBottomSeriesNumberMap.begin();
            for(; it != m_topBottomSeriesNumberMap.end(); ++it)
            {
                if(it->second == seriesNumber)
                {
                    graphedItem.release();
                    graphedItem = std::auto_ptr<GraphedItem> (getGraphedItem(it->first));
                    maxLimit = graphedItem->getMaximumLimit();
                }
            }
        }
        else
        {
            maxLimit = graphedItem->getMaximumLimit();
        }

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }

        return maxLimit;
    }


    //
    // onCursorChange
    //
    void ComparisonGraph::onCursorChange(long seriesNumber, double x, double y, long valueIndex)
    {
        TA_ASSERT(false, "Called wrong method");
    }


    //
    // onCursorChange
    //
    void ComparisonGraph::onCursorChange(long seriesNumber, double x, double y, long valueIndex, double mouseX)
    {
        // This implementation assumes that the X-Axis is of date/time format.
        // If it isn't, this method needs to be overridden.

        long graphedItemSeriesNum = seriesNumber;

        std::map<long, long>::iterator seriesIt = m_topBottomSeriesNumberMap.find(seriesNumber);
        if(seriesIt == m_topBottomSeriesNumberMap.end())
        {
            std::map<long, long>::iterator seriesIt = m_topBottomSeriesNumberMap.begin();
            for(; seriesIt != m_topBottomSeriesNumberMap.end(); ++seriesIt)
            {
                if(seriesIt->second == seriesNumber)
                {
                    graphedItemSeriesNum = seriesIt->first;
                    break;
                }
            }
        }

        int index = -1;
        std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
        for(; it != m_graphedItems.end(); ++it)
        {
            if(it->second->getSeriesNumber() == graphedItemSeriesNum)
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

        std::map<long, long>::iterator it2 = m_topBottomSeriesNumberMap.find(seriesNumber);
        if(it2 != m_topBottomSeriesNumberMap.end())
        {
            // Retrieve the offset value
            long offsetSeriesNum = it2->second;

            if(isSeriesVisible(offsetSeriesNum) == false)
            {
                getGraphObserver()->setOffsetValue(index, value, NO_VALUE, it->second->getPrecision(), it->second->getCustomAxisLabel());
            }
            else
            {          
				
				double axisMin = getChart().GetAxis().GetCustom(getChart().Series(offsetSeriesNum).GetVerticalAxisCustom()).GetMinimum();
                double axisMax = getChart().GetAxis().GetCustom(getChart().Series(offsetSeriesNum).GetVerticalAxisCustom()).GetMaximum();

                long aMin = getChart().Series(offsetSeriesNum).CalcYPosValue(axisMax);
                long aMax = getChart().Series(offsetSeriesNum).CalcYPosValue(axisMin);

                double yValue = NO_VALUE;
                int cnt = 0;
                std::vector<double> values;
                for(int i = aMin; i < aMax; ++i)
                {
                    long clicked = getChart().Series(offsetSeriesNum).Clicked(static_cast<long>(mouseX), i);
                    if(clicked != -1)
                    {
                        if(getChart().Series(offsetSeriesNum).IsNull(clicked))
                        {
                            // Set offset value to No Value
                            break;
                        }                        
                        values.push_back(getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).CalcPosPoint(i));						
                        ++cnt;
                    }
                }

                if(values.size() > 0)
                {
                    // Find the median
                    if(cnt % 2 == 0)
                    {
                        int valueIndex = cnt / 2;
                        yValue = (values[valueIndex - 1] + values[valueIndex]) / 2;
                    }
                    else
                    {
                        int valueIndex = cnt / 2;
                        yValue = values[valueIndex];
                    }
                }

                double offsetValue = NO_VALUE;
                if(yValue != NO_VALUE)
                {
                    offsetValue = it->second->getActualValue(yValue);
                }

                getGraphObserver()->setOffsetValue(index, value, offsetValue, it->second->getPrecision(), it->second->getCustomAxisLabel());
            }

            // Notify the GUI of the cursor values
            getGraphObserver()->setCursorTime(cursorTime.GetTime());
            getGraphObserver()->setOffsetTime(cursorTime.GetTime() - m_offset);
        }
    }


    //
    // onCursorToolChange
    //
    void ComparisonGraph::onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series)
    {
        if(m_cursorDisplayed == false)
        {
            return;
        }

        // If the graph is empty, the series count should return 2 as there were 2 dummy
        // series added as part of setting up the graph. 
        if(getChart().GetSeriesCount() == 2)
        {
            COleDateTime time = xVal;
            CTime cursorTime(time.GetYear(), time.GetMonth(), time.GetDay(),
                time.GetHour(), time.GetMinute(), time.GetSecond());

            getGraphObserver()->setCursorTime(cursorTime.GetTime());
    
            return;
        }
        
        for(long j = 2; j < getChart().GetSeriesCount(); ++j)
        {
            if(isSeriesVisible(j) == false)
            {
                onCursorChange(j, xVal, NO_VALUE, tool, x);
                continue;
            }

            try
            {
                double axisMin = getChart().GetAxis().GetCustom(getChart().Series(j).GetVerticalAxisCustom()).GetMinimum();
                double axisMax = getChart().GetAxis().GetCustom(getChart().Series(j).GetVerticalAxisCustom()).GetMaximum();

                long aMin = getChart().Series(j).CalcYPosValue(axisMax);
                long aMax = getChart().Series(j).CalcYPosValue(axisMin);

                
                double yValue = 0;
                int cnt = 0;
                std::vector<double> values;
                for(int i = aMin; i < aMax; ++i)
                {
                    long clicked = getChart().Series(j).Clicked(x, i);
                    if(clicked != -1)
                    {
                        if(getChart().Series(j).IsNull(clicked))
                        {
                            onCursorChange(j, xVal, NO_VALUE, tool, x);
                            continue;
                        }

                        values.push_back(getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).CalcPosPoint(i));
                        ++cnt;
                    }
                }

                if(values.size() > 0)
                {
                    // Find the median
                    if(cnt % 2 == 0)
                    {
                        int valueIndex = cnt / 2;
                        yValue = (values[valueIndex - 1] + values[valueIndex]) / 2;
                    }
                    else
                    {
                        int valueIndex = cnt / 2;
                        yValue = values[valueIndex];
                    }

                    onCursorChange(j, xVal, yValue, tool, x);
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
        }
    }


    //
    // getAxisLabel
    //
    std::string ComparisonGraph::getAxisLabel(long axis, long seriesIndex, long valueIndex)
    {
        if(axis == atBottom)
        {
            return "";
        }

        // Get the graphed item assuimg the series index is in top graph
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

        // If graphed item wasn't found, assume it is in bottom graph
        if(iter == m_graphedItems.end())
        {
            std::map<long, long>::iterator iter2 = m_topBottomSeriesNumberMap.begin();
            for(; iter2 != m_topBottomSeriesNumberMap.end(); ++iter2)
            {
                if(iter2->second == seriesIndex)
                {
                    long boolSeriesNumber = iter2->first;
                    std::map<int, boost::shared_ptr<GraphedItem> >::iterator iter3 = m_graphedItems.begin();
                    for(; iter3 != m_graphedItems.end(); ++iter3)
                    {
                        if(iter3->second->getBoolSeriesNumber() == boolSeriesNumber)
                        {
                            graphedItem = iter3->second;
                            break;
                        }
                    }

                    if(iter3 == m_graphedItems.end())
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "No graphed item for index: %ld", seriesIndex);

                        // No graphed item corresonding to given series index
                        return "";
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if(iter2 == m_topBottomSeriesNumberMap.end())
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "No graphed item for index: %ld", seriesIndex);
                
                // No graphed item corresonding to given series index
                return "";
            }
        }

        // Only want to change the label if it is a custom axis for a boolean/enum/derived item
        if(graphedItem->getShouldDisplayLabel())
        {
            double value = getChart().Series(seriesIndex).GetYValues().GetValue(valueIndex);
            std::string label("");

            
            std::map<int, std::string> labels = graphedItem->getStateLabels();
            std::map<int, std::string>::iterator iter = labels.find(value);
            if(iter != labels.end())
            {
                return iter->second;
            }
        }

        return "";
    }


    //
    // addCustomAxis
    //
    void ComparisonGraph::addCustomAxis(int index)
    {
        // Shouldn't call this method
        TA_ASSERT(false, "Incorrect method called");
    }


    //
    // createNewItem
    //
    void ComparisonGraph::createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName)
    {
        FUNCTION_ENTRY("createNewItem");

        //
        // Create a graphed item
        //
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
        thisSeries.GetMarks().SetVisible(false); //TD17179

        addCustomAxis(seriesNum, genericItem->getCustomAxisLabel().c_str(), TIME_TITLE, genericItem->getMinimumLimit(), 
            genericItem->getMaximumLimit(), startDateTime, endDateTime, false, true, genericItem->isBooleanType());
        
        //
        // Add another series for the bottom (offset) graph
        //
        long bottomSeriesNum = getChart().AddSeries(LINE_GRAPH);

        m_topBottomSeriesNumberMap[seriesNum] = bottomSeriesNum;

        CSeries bottomSeries = getChart().Series(bottomSeriesNum);
        bottomSeries.GetAsLine().GetLinePen().SetWidth(LINE_PEN_WIDTH);
        bottomSeries.GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);
        bottomSeries.SetTitle(itemName.c_str());
        bottomSeries.GetXValues().SetDateTime(true);

        bottomSeries.GetAsLine().GetPointer().SetVisible(true);
        bottomSeries.GetAsLine().GetPointer().SetStyle(LINE_POINT_STYLE);
        bottomSeries.GetAsLine().GetPointer().SetVerticalSize(LINE_POINT_HORIZONTAL_SIZE);
        bottomSeries.GetAsLine().GetPointer().SetHorizontalSize(LINE_POINT_VERTICAL_SIZE);
        bottomSeries.GetAsLine().SetColorEachLine(FALSE);
        bottomSeries.GetAsLine().GetPointer().GetPen().SetVisible(false);
        bottomSeries.SetColor(penColour); 
        bottomSeries.SetShowInLegend(false);    // Don't show the item in legend twice

        // fix defect 14171
        bottomSeries.GetMarks().SetVisible(false); //TD17179

        time_t startTime = startDateTime - m_offset;
        time_t endTime = endDateTime - m_offset;
        addCustomAxis(bottomSeriesNum, genericItem->getCustomAxisLabel().c_str(), TIME_TITLE, genericItem->getMinimumLimit(), 
            genericItem->getMaximumLimit(), startTime, endTime, false, false, genericItem->isBooleanType());

        // TD 9781: Step line should be used for all items, not just boolean items. This is because we don't want
        // to interpolate between two points as we can't make assumptions of what the value of the item was at
        // the times in between the known ones.
        //if(genericItem->isBooleanType() == true)
        //{
        thisSeries.GetAsLine().SetStairs(true);
        bottomSeries.GetAsLine().SetStairs(true);
        //}

        
        FUNCTION_EXIT;
    }


    //
    // removeItem
    //
    void ComparisonGraph::removeItem(int index)
    {
        FUNCTION_ENTRY("removeItem");

        // Currently the Trend Viewer will reload the whole trend template if something
        // is changed. Before reloading, the whole graph is cleared so there is currently
        // no need to remove one item. The following code is kept here because it would 
        // be more efficient to just reload the properties of the graph that have changed
        // rather than reloading the whole template. This means that if items have been
        // removed, there would be a need to remove one item at a time. This code can be
        // uncommented in the future when the Trend Viewer template loading is made more
        // efficient.

        /*long seriesNum = m_graphedItems[index]->getSeriesNumber();

		// Disconnect the custom axis from the series and remove it.
		getChart().Series(seriesNum).DisconnectAxisCustom(false);
        getChart().Series(seriesNum).DisconnectAxisCustom(true);
        long verticalAxisIndex = m_graphedItems[index]->getCustomAxisNumber();
        long horizontalAxisIndex = getChart().Series(seriesNum).GetHorizontalAxisCustom();
		getChart().GetAxis().RemoveCustom(verticalAxisIndex);
        getChart().GetAxis().RemoveCustom(horizontalAxisIndex);

        // Disconnect the offset custom axis
        std::map<long, long>::iterator bottomIt = m_topBottomSeriesNumberMap.find(seriesNum);
        TA_ASSERT(bottomIt != m_topBottomSeriesNumberMap.end(), "Offset series has not been added");
        long bottomSeriesNum = bottomIt->second;

        getChart().Series(bottomSeriesNum).DisconnectAxisCustom(false);
        getChart().Series(bottomSeriesNum).DisconnectAxisCustom(true);
        long offsetVerticalAxisIndex = getChart().Series(bottomSeriesNum).GetVerticalAxisCustom();
        long offsetHorizontalAxisIndex = getChart().Series(seriesNum).GetHorizontalAxisCustom();
		getChart().GetAxis().RemoveCustom(offsetVerticalAxisIndex);
        getChart().GetAxis().RemoveCustom(offsetHorizontalAxisIndex);
		
        // Set the grid back to default if the deleted trend was displaying
        // a custom axis
        onClickBackground();

		getChart().RemoveSeries(seriesNum);
        getChart().RemoveSeries(bottomSeriesNum);
        m_topBottomSeriesNumberMap.erase(seriesNum);
        
        // Adjust the series numbering for series added after the deleted one
        adjustSeriesNumbers(seriesNum);
        adjustSeriesNumbers(bottomSeriesNum);

        // Adjust the custom axis numbering for axes added after the deleted one
        adjustCustomAxisNumbers(verticalAxisIndex);
        adjustCustomAxisNumbers(horizontalAxisIndex);
        adjustCustomAxisNumbers(offsetVerticalAxisIndex);
        adjustCustomAxisNumbers(offsetHorizontalAxisIndex);
        */

        FUNCTION_EXIT;
    }

    
    //
    // scaleValue
    //
    double ComparisonGraph::scaleValue(double value, const GraphedItem* graphedItem) const
    {
		//TD17940 - modified scaling computation for given value, for all datapoint types, simply return
		// the value
		return value;
    }


    //
    // moveGraph
    //
    void ComparisonGraph::moveGraph(time_t timestamp)
    {
        FUNCTION_ENTRY("moveGraph");

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
        //CString testTime = endTime.Format( DATE_TIME_FORMAT );
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
        CString testTime = endTime.Format(dateFormatStr.c_str());
        // ++ TD12474

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Graph update time: %s", testTime);

        //
        // Only move the graph if the timestamp is later than the last time shown 
        // or if the graph has not been zoomed
        //
        COleDateTime latestTime = getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).GetMaximum();
        time_t leftMostTime = TimeFunction::getInstance().ConvertToTimeT(latestTime);

        if(timestamp > leftMostTime && isLive() == true && isGraphZoomed() == false)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Moving Graph");

            //
		    // Set the times on the X-Axis
            //
            setStartTime(timestamp - getRangeSeconds() + 1);
            COleDateTime startDateTime(getStartTime());
            setEndTime(timestamp + 1);
            COleDateTime endDateTime(getEndTime());
            getChart().GetAxis().GetCustom(m_defaultHorizontalAxisIndex).SetMinMax(DATE(startDateTime), DATE(endDateTime)); // Add 1 second so the time is definately shown
            getChart().GetAxis().GetCustom(m_defaultVerticalAxisIndex).SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

            //
            // Set the times on the offset X-Axis
            //
            COleDateTime offsetStart(getStartTime() - m_offset);
            COleDateTime offsetEnd(timestamp - m_offset);
            getChart().GetAxis().GetCustom(m_defaultOffsetHorizontalAxisIndex).SetMinMax(DATE(offsetStart), DATE(offsetEnd)); // Add 1 second so the time is definately shown
            getChart().GetAxis().GetCustom(m_defaultOffsetVerticalAxisIndex).SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

            //
            // Scroll the custom axes
            //
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNumber = it->second->getSeriesNumber();
                long axisIndex = getChart().Series(seriesNumber).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(axisIndex).SetMinMax(DATE(startDateTime), DATE(endDateTime));

                long offsetSeriesIndex = m_topBottomSeriesNumberMap[seriesNumber];
                long offsetAxisIndex = getChart().Series(offsetSeriesIndex).GetHorizontalAxisCustom();
                getChart().GetAxis().GetCustom(offsetAxisIndex).SetMinMax(DATE(offsetStart), DATE(offsetEnd));
            }

            adjustXAxisFormat();

            // For each item, reload the dummy series
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator iter = m_graphedItems.begin();
            for(; iter != m_graphedItems.end(); ++iter)
            {
                if(iter->second->getShouldDisplayLabel())
                {
                    time_t rangeSeconds = Utilities::getRangeSeconds(COleDateTime(getChart().GetAxis().GetBottom().GetMinimum()),
                        COleDateTime(getChart().GetAxis().GetBottom().GetMaximum()));
            
                    setUpDummySeries(iter->second->getBoolCustomAxisNumber(), iter->second.get(), getStartTime(), Utilities::getRangeSeconds(startDateTime, endDateTime));
                }
            }

            // Notify the GUI so it can update the start and end time display
            getGraphObserver()->setStartEndTime(getStartTime(), timestamp);
        }


        FUNCTION_EXIT;
    }


    //
    // setUpDummySeries
    //
    void ComparisonGraph::setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds)
    {
        CWaitCursor wait;

        long topBoolSeriesNum = graphedItem->getBoolSeriesNumber();
        long bottomBoolSeriesNum = -1;
        long graphedItemSeriesNumber = graphedItem->getSeriesNumber();
        
        // Check if there is already a dummy series associated with and remove it if there is.
        // Remove the bottom series first because the bottom series will have a higher index than the top
        // series, and when a series is removed, teeChart adjusts all the indexes higher than the one
        // removed to be decremented by one. If the top series is removed first, and then the bottom
        // series index is retrieved using the m_topBottomSeriesNumberMap, the wrong series could be
        // removed.
        std::map<long, long>::iterator topBotIter = m_topBottomSeriesNumberMap.find(topBoolSeriesNum);
        if(topBotIter != m_topBottomSeriesNumberMap.end())
        {
            bottomBoolSeriesNum = topBotIter->second;
            if(bottomBoolSeriesNum >= 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Clearing series (bottom): %d", bottomBoolSeriesNum);
                getChart().Series(bottomBoolSeriesNum).Clear();
            }
        }

        // Check if there is already a dummy series associated with and remove it if there is.
        if(topBoolSeriesNum >= 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Clearing series (top): %d", topBoolSeriesNum);
            getChart().Series(topBoolSeriesNum).Clear();
        }

        if(topBoolSeriesNum < 0)
        {
            // Retrieve the horizontal axis index to use for the new series
            long horizontalAxisNumber = getChart().Series(graphedItemSeriesNumber).GetHorizontalAxisCustom();

            // Top graph dummy series
            long seriesNum = getChart().AddSeries(LINE_GRAPH); // 0 for line graph
            getChart().Series(seriesNum).GetXValues().SetDateTime(true);
            getChart().Series(seriesNum).SetShowInLegend(false);
            getChart().Series(seriesNum).SetVerticalAxisCustom(customAxisNum);
            getChart().Series(seriesNum).SetHorizontalAxisCustom(horizontalAxisNumber);
            getChart().Series(seriesNum).GetAsLine().GetLinePen().SetVisible(false);

            // fix defect 14171
            getChart().Series(seriesNum).GetMarks().SetVisible(false); // TD14799 azenitha
        
            graphedItem->setBoolSeriesNumber(seriesNum);
            topBoolSeriesNum = seriesNum;
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
            getChart().Series(topBoolSeriesNum).AddXY(DATE(date), value, "", graphedItem->getPenColour());

            newTime += secondsDiff;
        }

        // Bottom graph dummy series
        long bottomSeriesNumber = m_topBottomSeriesNumberMap[graphedItemSeriesNumber];

        if(bottomBoolSeriesNum < 0)
        {
            long horizontalAxisNumber = getChart().Series(bottomSeriesNumber).GetHorizontalAxisCustom();

            long seriesNum2 = getChart().AddSeries(LINE_GRAPH); // 0 for line graph
            getChart().Series(seriesNum2).GetXValues().SetDateTime(true);
            getChart().Series(seriesNum2).SetShowInLegend(false);
            getChart().Series(seriesNum2).SetVerticalAxisCustom(m_boolTopBottomAxisIndexMap[customAxisNum]);
            getChart().Series(seriesNum2).SetHorizontalAxisCustom(horizontalAxisNumber);
            getChart().Series(seriesNum2).GetAsLine().GetLinePen().SetVisible(false);

            // fix defect 14171
            getChart().Series(seriesNum2).GetMarks().SetVisible(false); // TD14799 azenitha
        
            m_topBottomSeriesNumberMap[topBoolSeriesNum] = seriesNum2;
            bottomBoolSeriesNum = seriesNum2;
        }

        iter = labels.begin();

        newTime = startTime + secondsDiff - m_offset;
        for(; iter != labels.end(); ++iter)
        {
            double value = scaleValue(iter->first, graphedItem);
            COleDateTime date(newTime);
            getChart().Series(bottomBoolSeriesNum).AddXY(DATE(date), value, "", graphedItem->getPenColour());

            newTime += secondsDiff;
        }
    }


    //
    // addCustomAxis
    //
    void ComparisonGraph::addCustomAxis(long seriesNumber, const std::string& verticalLabel, const std::string& horizontalLabel, 
        double verticalMin, double verticalMax, time_t horizontalMin, time_t horizontalMax, bool isDefault, bool isTopGraph, bool isBooleanType)
    {
        FUNCTION_ENTRY("addCustomAxis");

        bool setVisible = false;
        COLORREF penColour = DEFAULT_PEN_COLOUR;
        long seriesNum = seriesNumber;

        std::auto_ptr<GraphedItem> graphedItem = std::auto_ptr<GraphedItem>(static_cast<GraphedItem*>(NULL));
        
        if(isDefault == true)
        {
            setVisible = true;
        }
        else
        {
            if(isTopGraph == false)
            {
                std::map<long, long>::iterator it = m_topBottomSeriesNumberMap.begin();
                for(; it != m_topBottomSeriesNumberMap.end(); ++it)
                {
                    if(it->second == seriesNumber)
                    {
                        seriesNum = it->first;
                        break;
                    }
                }
            }

            graphedItem = std::auto_ptr<GraphedItem> (getGraphedItem(seriesNum));

            if(graphedItem.get() == NULL)
            {
                std::ostringstream str;
                str << "Invalid series number: " << seriesNum;
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::SERIES_NUMBER_NOT_FOUND, str.str().c_str()));
            }

            penColour = graphedItem->getPenColour();
        }

        //
        // Add a custom vertical axis
        //
        long axisIndex = getChart().GetAxis().AddCustom(false); // False - for Vertical axis
	    CAxis theAxis = getChart().GetAxis().GetCustom(axisIndex);
	    theAxis.SetPositionPercent(PERCENTAGE_AXIS_MIN);
        
        //TD17940 - modified computation for scale factors to display more accurate values
		double leftAxisMin = getChart().GetAxis().GetLeft().GetMinimum();
        double leftAxisMax = getChart().GetAxis().GetLeft().GetMaximum();		
		double scaleFactor =(isBooleanType == true) ? ((2.0 - 0.0) / 100.0) : (verticalMax - verticalMin) / 100.0;  
		//++TD17940
        // TD 8743: If the item is boolean, set the axis to display true/false labels 
        
        // If the series number is 0 or 1, then it is a dummy series, so don't create a boolean axis.
        if(seriesNumber != 0 && seriesNumber != 1 && graphedItem->getShouldDisplayLabel()) 		
        {
            // TD 8743: Create another custom axis for displaying the boolean labels
            long axisIndex2 = getChart().GetAxis().AddCustom(false); // False - for Vertical axis
            CAxis theAxis2 = getChart().GetAxis().GetCustom(axisIndex2);
            theAxis2.SetPositionPercent(DEFAULT_AXIS_POSITION_PERCENT);
            theAxis2.SetVisible(false);
            theAxis2.GetLabels().SetStyle(talText);
            theAxis2.GetLabels().SetOnAxis(true);

            theAxis2.GetLabels().GetFont().SetBold(true);
            theAxis2.GetLabels().GetFont().SetSize(AXIS_LABEL_FONT_SIZE - 2);
            theAxis2.GetLabels().GetFont().SetColor(penColour);

            theAxis2.GetAxisPen().SetColor(penColour);
            
            theAxis2.GetTitle().SetCaption(verticalLabel.c_str());
            theAxis2.GetTitle().GetFont().SetBold(true);
            theAxis2.GetTitle().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
            theAxis2.GetTitle().GetFont().SetColor(penColour);
            theAxis2.GetTitle().SetAngle(DEFAULT_VERTICAL_AXIS_ANGLE);
            theAxis2.GetTitle().SetVisible(true);

            //
            // Set the vertical axis position
            //
            if(isTopGraph == true)
            {
                theAxis2.SetStartPosition(DEFAULT_AXIS_START_POSITION);
                theAxis2.SetEndPosition(COMPARISON_TOP_GRAPH_END_POSITION_PERCENTAGE);
            }
            else
            {
                theAxis2.SetStartPosition(COMPARISON_BOTTOM_GRAPH_START_POSITION_PERCENTAGE);
                theAxis2.SetEndPosition(DEFAULT_AXIS_END_POSITION);
            }

            //
            // These calculations are based on the way the scale factor and plotted
            // values are calculated in DataPointInfo and GraphDataPoint.
            //
			//TD17940 - modified computation for scale factors to display more accurate values
            //theAxis2.SetMinMax( verticalMin, verticalMax );			
			theAxis2.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);
			//++TD17940

            if(isDefault == false)
	        {
                if(isTopGraph == true)
                {
                    graphedItem->setBoolCustomAxisNumber(axisIndex2);
                }
                else
                {
                    long topAxisIndex = graphedItem->getBoolCustomAxisNumber();
                    m_boolTopBottomAxisIndexMap[topAxisIndex] = axisIndex2;
                }
            }
        }
        else
        {
            theAxis.GetLabels().GetFont().SetBold(true);
            theAxis.GetLabels().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
            theAxis.GetLabels().GetFont().SetColor(penColour);

            theAxis.GetAxisPen().SetColor(penColour);

            theAxis.GetTitle().SetCaption(verticalLabel.c_str());
            theAxis.GetTitle().GetFont().SetBold(true);
            theAxis.GetTitle().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
            theAxis.GetTitle().GetFont().SetColor(penColour);
            theAxis.GetTitle().SetAngle(DEFAULT_VERTICAL_AXIS_ANGLE);
            theAxis.GetTitle().SetVisible(true);
		
        }

		
        theAxis.SetVisible(setVisible);
        
        //
        // Set the vertical axis position
        //
        if(isTopGraph == true)
        {
            theAxis.SetStartPosition(DEFAULT_AXIS_START_POSITION);
            theAxis.SetEndPosition(COMPARISON_TOP_GRAPH_END_POSITION_PERCENTAGE);
        }
        else
        {
            theAxis.SetStartPosition(COMPARISON_BOTTOM_GRAPH_START_POSITION_PERCENTAGE);
            theAxis.SetEndPosition(DEFAULT_AXIS_END_POSITION);
        }

        if(isDefault == true && isTopGraph == true)
        {
            m_defaultVerticalAxisIndex = axisIndex;
        }

        if(isDefault == true && isTopGraph == false)
        {
            m_defaultOffsetVerticalAxisIndex = axisIndex;
        }

		//TD17940 - modified computation for scale factors to display more accurate values
		//theAxis.SetMinMax(verticalMin, verticalMax);
		theAxis.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);
        //TD17940

        if(isDefault == false)
	    {
            if(isTopGraph == true)
            {
                graphedItem->setCustomAxisNumber(axisIndex);
            }
        }

        CSeries series = getChart().Series(seriesNumber);
        series.SetVerticalAxisCustom(axisIndex);

        //
        // Add a custom horizontal axis
        //
        axisIndex = getChart().GetAxis().AddCustom(true); 
	    theAxis = getChart().GetAxis().GetCustom(axisIndex);
        theAxis.GetLabels().GetFont().SetBold(true);
        theAxis.GetLabels().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
        theAxis.GetLabels().SetOnAxis(false);

        theAxis.GetLabels().GetFont().SetColor(penColour);
        theAxis.GetAxisPen().SetColor(penColour);
        theAxis.SetVisible(setVisible);

        theAxis.GetTitle().SetCaption(horizontalLabel.c_str());
        theAxis.GetTitle().GetFont().SetBold(true);
        theAxis.GetTitle().GetFont().SetSize(AXIS_LABEL_FONT_SIZE);
        theAxis.GetTitle().GetFont().SetColor(penColour);
        theAxis.GetTitle().SetVisible(setVisible);
        theAxis.SetStartPosition(DEFAULT_AXIS_START_POSITION);
        theAxis.SetEndPosition(DEFAULT_AXIS_END_POSITION);

        //
        // Set the horizontal axis position
        //
        if(isTopGraph == true)
        {
            theAxis.SetPositionPercent(COMPARISON_TOP_GRAPH_HOR_POSITION_PERCENTAGE);
        }
        else
        {
            theAxis.SetPositionPercent(DEFAULT_AXIS_START_POSITION);
        }
        
        theAxis.SetMinMax(DATE(COleDateTime(horizontalMin)), DATE(COleDateTime(horizontalMax)));

        series = getChart().Series(seriesNumber);
        series.SetHorizontalAxisCustom(axisIndex);
        
        if(isDefault == true && isTopGraph == true)
        {
            m_defaultHorizontalAxisIndex = axisIndex;
        }

        if(isDefault == true && isTopGraph == false)
        {
            m_defaultOffsetHorizontalAxisIndex = axisIndex;
        }

        if(isDefault == true)
        {
            // Add the axis arrows
            long arrowToolIndex = getChart().GetTools().Add(tcAxisArrow);
            getChart().GetTools().GetItems(arrowToolIndex).SetActive(true);
            CAxisArrowTool arrowTool = getChart().GetTools().GetItems(arrowToolIndex).GetAsAxisArrow();
    
            VARIANT item;
            VariantInit(&item);
            item.vt = VT_I2;
            item.iVal = static_cast<short>(axisIndex + atCustom);
        
            arrowTool.SetAxis(item);
            arrowTool.SetScrollPercent(SCROLL_PERCENTAGE);
            arrowTool.SetPosition(aaBoth);
            arrowTool.SetLength(AXIS_ARROW_LENGTH);

            if(isTopGraph == true)
            {
                m_topArrowToolIndex = arrowToolIndex - 1;
            }
            else
            {
                m_bottomArrowToolIndex = arrowToolIndex - 1;
            }
        }

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }

        FUNCTION_EXIT;
    }


    //
    // retrieveData
    //
    void ComparisonGraph::retrieveData(long seriesNumber, time_t timestamp, double& dataValue, COLORREF& colour, bool isOffsetSeries)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

        long genericItemSeriesNumber = seriesNumber;
        if(isOffsetSeries == true)
        {
            std::map<long, long>::iterator it = m_topBottomSeriesNumberMap.begin();
            for(; it != m_topBottomSeriesNumberMap.end(); ++it)
            {
                if(it->second == seriesNumber)
                {
                    genericItemSeriesNumber = it->first;
                    break;
                }
            }

            TA_ASSERT(it != m_topBottomSeriesNumberMap.end(), "Series number does not have a match");
        }

        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(genericItemSeriesNumber));
        if(graphedItem.get() == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "retrieveData() failed because seriesNumber: %ld does not exist", genericItemSeriesNumber);
            dataValue = 0;
            colour = BAD_STATUS_COLOUR;
            return;
        }

        // Retrieve the value from the database for the given time
        PlotData data = graphedItem->retrieveHistoricalData(timestamp, graphedItem->getTypeIdentifier(), getSampleType(), getSamplePeriod());

        // If data wasn't retrieved, the data may not have been written to the database yet. Try again.
        const int MAX_ATTEMPTS = 3;
        int timesTried = 0;
        while(data.size() == 0 && timesTried < MAX_ATTEMPTS)
        {
            data = graphedItem->retrieveHistoricalData(timestamp, graphedItem->getTypeIdentifier(), getSampleType(), getSamplePeriod());

            const int SLEEP_IN_MILLISECS = 500;
            TA_Base_Core::Thread::sleep(SLEEP_IN_MILLISECS);

            ++timesTried;
        }

        if(timesTried == 3)
        {
            // Couldn't retrieve point from the database. Plot with the last value and bad status
            std::string name = graphedItem->getName();
            CTime time(timestamp);

            // TD12474 ++
            //CString timeStr = time.Format(DATE_TIME_FORMAT);
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 			
            CString timeStr = time.Format(dateFormatStr.c_str());
            // ++ TD12474

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve data from database for entity: %s at time: %s", name.c_str(), timeStr.GetBuffer(0));
            
            // Couldn't retrieve point from the database. Plot with the last value and bad status
            colour = BAD_STATUS_COLOUR;
        }
        else
        {
            Point point = data[0];
            dataValue = point.y;
        }

        if(graphedItem.get() != NULL)
        {
            graphedItem.release();
        }
    }


    //
    // setUpCursorTools
    //
    void ComparisonGraph::setUpCursorTools()
    {
        m_cursorToolIndex = getChart().GetTools().Add(tcCursor);
        CCursorTool cursorTool = getChart().GetTools().GetItems(m_cursorToolIndex).GetAsTeeCursor();
        cursorTool.SetStyle(cssVertical);
        cursorTool.SetFollowMouse(false);
        cursorTool.SetSnap(true);
        cursorTool.GetPen().SetColor(CURSOR_COLOUR);

        // Set the cursor tool to Series 0 so it will appear in the top graph
        VARIANT item;
        VariantInit(&item);
        item.vt = VT_I2;
        item.iVal = 0;
        cursorTool.SetSeries(item);
    }

}

#endif // ComparisonGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

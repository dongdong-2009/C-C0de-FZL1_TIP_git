#if !defined(BarGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define BarGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * BarGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/BarGraph.cpp $
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

#include "app/trending/trend_viewer/src/BarGraph.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/exceptions/src/TrendViewerException.h"

#include "cots/teechart/TeeChartDefines.h"

#include "core/utilities/src/DateFormat.h" // TD12474 
// TeeChart includes
#include <cots/teechart/valuelist.h>
#include <cots/teechart/axis.h>
#include <cots/teechart/axes.h>
#include <cots/teechart/axislabels.h>
#include <cots/teechart/axistitle.h>
#include <cots/teechart/barseries.h>
#include <cots/teechart/pen.h>
#include <cots/teechart/pointer.h>
#include <cots/teechart/marks.h>
#include <cots/teechart/brush.h>
#include <cots/teechart/teefont.h>
#include <cots/teechart/zoom.h>


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    //
    // BarGraph
    //
    BarGraph::BarGraph(CTChart& chart, const std::string& name, const std::string& description, time_t startTime, 
            time_t endTime, bool isLive, long samplePeriod, IItemStore* itemStore, 
            IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver, bool isFieldUpdates) 
            : AbstractGraph(chart, name, description, startTime, endTime, isLive, 
              samplePeriod, itemStore, archiveDataStore, graphObserver),
              m_isFieldUpdates(isFieldUpdates)
    {
        m_minItems = BAR_MIN_ITEMS;
        m_maxItems = BAR_MAX_ITEMS;
    }


    //
    // ~BarGraph
    //
    BarGraph::~BarGraph()
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
    void BarGraph::setUpGraph()
    {
        FUNCTION_ENTRY("setUpGraph");

        getChart().RemoveAllSeries();

        //
        // Add a dummy series so that the graph lines show
        //
        long seriesNum = getChart().AddSeries(BAR_GRAPH);
        CSeries thisSeries = getChart().Series(seriesNum);
        
        thisSeries.GetXValues().SetDateTime(true);
        thisSeries.SetShowInLegend(false);
        
		// graph until the cursor tool is enabled
        // fix defect 14171
        thisSeries.GetMarks().SetVisible(false);  
		//++TD17179 - lizettejl
        
        getChart().GetAxis().GetLeft().GetTitle().SetCaption(PERCENTAGE_TITLE);
        getChart().GetAxis().GetBottom().GetTitle().SetCaption(TIME_TITLE);

        setTitle(getGraphName().c_str());
        
        //
        // Set the times on the X-Axis
        //
        COleDateTime startTime(getStartTime());
        COleDateTime endTime(getEndTime());
       
        getChart().GetAxis().GetBottom().SetMinMax(DATE(startTime), DATE(endTime));
        getChart().GetAxis().GetLeft().SetMinMax(PERCENTAGE_AXIS_MIN, PERCENTAGE_AXIS_MAX);

        //
        // Set the start and end times on the GUI
        //
        getGraphObserver()->setStartEndTime(getStartTime(), getEndTime());
        
        FUNCTION_EXIT;
    }


    //
    // plotPoint
    //
    void BarGraph::plotPoint(long seriesNumber, double x, double y, const COLORREF& penColour)
    {
        // Convert the y value into a time
		//TD17883
        //time_t timestamp = TimeFunction::getInstance().ConvertToLocalTimeT(static_cast <long> (x));
		time_t timestamp = static_cast<time_t>(x);
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

        std::auto_ptr<GraphedItem> graphedItem(getGraphedItem(seriesNumber));

        if(graphedItem.get() == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point. Series number: %ld does not exist", seriesNumber);
            return;
        }

        if(m_isFieldUpdates == true)
        {
            // Plot with the Live Update value, which is the value that has been passed in
            
            // Scale the value
            dataValue = scaleValue(y, graphedItem.get());

            // Because the times are stored in database as GMT time, need to convert
            // the time to local
            COleDateTime date(timestamp);
            
            // Don't care about seconds, so set it to zero
            CTime timeWithoutSecs(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), 0);
            COleDateTime modifiedDate(timeWithoutSecs.GetTime());
        
		    // TD12474 ++
            //CString testTime = timeWithoutSecs.Format( DATE_TIME_FORMAT );
			TA_Base_Core::DateFormat dateFormat; 
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);
            CString testTime = timeWithoutSecs.Format( dateFormatStr.c_str() );
            // ++ TD12474

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting at: %s", testTime);

            // Add the point to the graph
            getChart().Series(seriesNumber).AddXY(DATE(modifiedDate), dataValue, "", pointColour);

            //nullValue(timeWithoutSecs.GetTime(), seriesNumber, DEFAULT_SAMPLE_PERIOD);
        
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
    void BarGraph::updatePenStyle(bool originalPenStyle)
    {
        if(originalPenStyle == true)
        {
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsBar().GetBarBrush().SetStyle(ORIGINAL_BRUSH_STYLE);
            }
        }
        else
        {
            int brushStyle = 0;
            std::map<int, boost::shared_ptr<GraphedItem> >::iterator it = m_graphedItems.begin();
            for(; it != m_graphedItems.end(); ++it)
            {
                int style = brushStyle%NUMBER_AVAILABLE_BRUSHES;

                // Don't use style 1 because it does not have a pattern
                if(style >= psDash)
                {
                    style = brushStyle%NUMBER_AVAILABLE_BRUSHES + 1;
                }
                
                long seriesNum = it->second->getSeriesNumber();
                getChart().Series(seriesNum).GetAsBar().GetBarBrush().SetStyle(style);
                ++brushStyle;
            }
        }
    }


    void BarGraph::onCursorToolChange(long tool, long x, long y, double xVal, double yVal, long series)
    {
        // If there aren't any series displayed, there will only be the dummy series that has been added
        if(getChart().GetSeriesCount() == 1)
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

                bool isClicked = false;
                double yMax = 0;
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

                        // For BAR graph, there will be multiple y values for the one value
                        // index. We want the maximum value, which will be the y value at the
                        // top of the bar.
                        yMax = max(yMax, getChart().GetAxis().GetLeft().CalcPosPoint(i));
                        isClicked = true;    
                    }
                }

                if(yMax == 0 && isClicked == false)
                {
                    onCursorChange(j, xVal, NO_VALUE, 0);
                }
                else
                {
                    onCursorChange(j, xVal, yMax, 0);
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

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "An unexpected error ocurred.");
            }
        }
    }


    //
    // addCustomAxis
    //
    void BarGraph::addCustomAxis(int index)
    {
        long axisIndex = getChart().GetAxis().AddCustom(false); // False - for Vertical axis
        CAxis theAxis = getChart().GetAxis().GetCustom(axisIndex);
        theAxis.SetPositionPercent(DEFAULT_AXIS_POSITION_PERCENT);
        theAxis.SetVisible(false);

		//TD17940 - modified computation for scale factors to display more accurate values
        boost::shared_ptr<GraphedItem> graphedItem = m_graphedItems[index];

        double minValue = graphedItem->getMinimumLimit();
        double maxValue = graphedItem->getMaximumLimit();
        
        double leftAxisMin = getChart().GetAxis().GetLeft().GetMinimum();
        double leftAxisMax = getChart().GetAxis().GetLeft().GetMaximum();
		double scaleFactor = graphedItem->isBooleanType() ? ((2.0 - 0.0) / 100.0) : (maxValue - minValue) / 100.0;  
		//++TD17940

        if(graphedItem->getShouldDisplayLabel())
        {
            // TD 8743: Create another custom axis for displaying the boolean labels
            long axisIndex2 = getChart().GetAxis().AddCustom(false); // False - for Vertical axis
            CAxis theAxis2 = getChart().GetAxis().GetCustom(axisIndex2);
            theAxis2.SetPositionPercent(DEFAULT_AXIS_POSITION_PERCENT);
            theAxis2.SetVisible(false);
            theAxis2.GetLabels().SetStyle(talMark);

            setUpCustomAxis(theAxis2, index);
		
            theAxis2.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);

            graphedItem->setBoolCustomAxisNumber(axisIndex2);
        }
        else
        {
            setUpCustomAxis(theAxis, index);
        }

		//TD17940 - modified computation for scale factors to display more accurate values
        theAxis.SetMinMax(scaleFactor * leftAxisMin, scaleFactor * leftAxisMax);        
		  
        CSeries series = getChart().Series(graphedItem->getSeriesNumber());
        series.SetVerticalAxisCustom(axisIndex);
        graphedItem->setCustomAxisNumber(axisIndex);
    }


    //
    // createNewItem
    //
    void BarGraph::createNewItem(int index, const std::string& itemName, COLORREF penColour, time_t startDateTime, time_t endDateTime, const std::string& displayName)
    {
        FUNCTION_ENTRY("createNewItem");

        // Create a graphed item
        IItemStore* itemStore = getItemStore();
        IGenericItem* genericItem = itemStore->getItem(itemName);

        long seriesNum = getChart().AddSeries(BAR_GRAPH);
        CSeries thisSeries = getChart().Series(seriesNum);

        boost::shared_ptr<GraphedItem> graphedItem(new GraphedItem(thisSeries, seriesNum, *this, *genericItem, penColour, startDateTime, endDateTime)); 
        m_graphedItems[index] = graphedItem;

        thisSeries.GetAsBar().GetBarPen().SetVisible(false);

        std::string title = displayName + " (" + itemName + ")";
        thisSeries.SetTitle(title.c_str());
        thisSeries.GetXValues().SetDateTime(true);

      	//TD17179 - disable engineering values to show as markup texts when setting up the 
		// graph until the cursor tool is enabled
        // fix defect 14171
        getChart().Series(seriesNum).GetMarks().SetVisible(false);  
		//++TD17179 - lizettejl
        
        thisSeries.GetAsBar().SetBarStyle(BAR_RECTANGLE_STYLE);
        thisSeries.GetAsBar().SetBarWidth(BAR_WIDTH_PERCENT);
        thisSeries.GetAsBar().SetMultiBar(mbSide);
        thisSeries.GetAsBar().SetOffsetPercent(0);
        thisSeries.SetColor(penColour); 

        addCustomAxis(index);

        FUNCTION_EXIT;
    }


    void BarGraph::removeItem(int index)
    {
        FUNCTION_ENTRY("removeItem");

        long seriesNum = m_graphedItems[index]->getSeriesNumber();

        // Set the grid back to default if the deleted trend was displaying
        // a custom axis
        onClickBackground();

        // Disconnect the custom axis from the series and remove it.
        getChart().Series(seriesNum).DisconnectAxisCustom(false);
        long axisIndex = m_graphedItems[index]->getCustomAxisNumber();
        getChart().GetAxis().RemoveCustom(axisIndex);

        getChart().RemoveSeries(seriesNum);

        // Adjust the series numbering for series added after the deleted one
        adjustSeriesNumbers(seriesNum);

        // Adjust the custom axis numbering for axes added after the deleted one
        adjustCustomAxisNumbers(axisIndex);

        if(m_graphedItems[index]->getShouldDisplayLabel())
        {
            long boolSeriesNum = m_graphedItems[index]->getBoolSeriesNumber();

            // Disconnect the custom axis from the series and remove it.
            getChart().Series(boolSeriesNum).DisconnectAxisCustom(false);
            long boolAxisIndex = m_graphedItems[index]->getBoolCustomAxisNumber();
            getChart().GetAxis().RemoveCustom(boolAxisIndex);

            getChart().RemoveSeries(boolSeriesNum);

            // Adjust the series numbering for series added after the deleted one
            adjustSeriesNumbers(boolSeriesNum);

            // Adjust the custom axis numbering for axes added after the deleted one
            adjustCustomAxisNumbers(boolAxisIndex);
        }

        FUNCTION_EXIT;
    }

    //
    // scaleValue
    //
    double BarGraph::scaleValue(double value, const GraphedItem* graphedItem) const
    {
        return value;
    }


    //
    // setUpDummySeries
    //
    void BarGraph::setUpDummySeries(long customAxisNum, GraphedItem* graphedItem, time_t startTime, long rangeSeconds)
    {
        CWaitCursor wait;

        long boolSeriesNumber = graphedItem->getBoolSeriesNumber();

        // Check if there is already a dummy series associated with and remove it if there is.
        if(boolSeriesNumber >= 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Clearing series: %d", boolSeriesNumber);
            getChart().Series(boolSeriesNumber).Clear();
        }

        // If a dummy series hasn't been added, then add one
        if(boolSeriesNumber < 0)
        {
            long seriesNum = getChart().AddSeries(BAR_GRAPH); // 0 for line graph
            CSeries thisSeries = getChart().Series(seriesNum);

            thisSeries.GetXValues().SetDateTime(true);
            thisSeries.SetShowInLegend(false);
            thisSeries.SetVerticalAxisCustom(customAxisNum);

             // fix defect 14171
            thisSeries.GetMarks().SetVisible(false); // TD14799 azenitha

            thisSeries.GetAsBar().SetBarStyle(BAR_RECTANGLE_STYLE);
            thisSeries.GetAsBar().SetBarWidth(BAR_WIDTH_PERCENT);
            thisSeries.GetAsBar().SetMultiBar(mbSide);
            thisSeries.GetAsBar().SetOffsetPercent(0);
            thisSeries.GetAsBar().GetBarPen().SetVisible(false);
            thisSeries.GetAsBar().GetBarBrush().SetStyle(bsClear);

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
    // adjustCustomAxisForBoolean
    //
    void BarGraph::adjustCustomAxisForBoolean(double minYValue, double maxYValue, const GraphedItem* graphedItem, CAxis& theAxis)
    {
        // scale factor = OrigMax - OrigMin / NewMax - NewMin
        const double scaleFactor = (100.0 - 0.0) / (2.0 - 0.0);
        const double offset = 100.0 - scaleFactor * 2.0;

        double axisMin = (minYValue - offset) / scaleFactor;
        double axisMax = (maxYValue - offset) / scaleFactor;
       
        theAxis.SetMinMax(axisMin, axisMax);
    }
}

#endif // BarGraphCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

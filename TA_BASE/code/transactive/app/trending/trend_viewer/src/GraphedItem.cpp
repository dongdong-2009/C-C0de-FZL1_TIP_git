#if !defined(GraphedItemCPP_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_)
#define GraphedItemCPP_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_

/**
  * GraphedItem.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphedItem.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class represents an item that has been added to the graph. It contains a reference
  * to the generic item that it represents and a refresh thread that will update the graph
  * periodically if it is in live mode. This object will be created when an item is added
  * to the graphed and deleted when it is removed.
  *
  */

#include "app/trending/trend_viewer/src/StdAfx.h"
#include "app/trending/trend_viewer/src/GraphedItem.h"
#include "app/trending/trend_viewer/src/GraphRefreshThread.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/ColourDefs.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/Utilities.h" //TD17529

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // GraphedItem
    //
    GraphedItem::GraphedItem(CSeries& series, long seriesNumber, IGraph& graph, IGenericItem& itemData, 
            COLORREF penColour, time_t startDateTime, time_t endDateTime) 
            : m_penColour(penColour), 
              m_series(series), 
              m_lastYValue(0), 
              m_lastXValue(0), 
              m_seriesNumber(seriesNumber), 
              m_genericItem(itemData), 
              m_graph(graph), 
              m_graphRefreshThread(NULL), 
              m_customAxisNumber(-1),
              m_boolCustomAxisNumber(-1),
              m_boolSeriesNumber(-1)
    {
        m_genericItem.setUpdateObserver(this);
    }


    //
    // ~GraphedItem
    //
    GraphedItem::~GraphedItem()
    {
        FUNCTION_ENTRY("~GraphedItem");

        if(m_graphRefreshThread.get() != NULL)
        {
            m_graphRefreshThread->terminateAndWait();
        }

        // Reset the update observer for the generic item
        m_genericItem.setUpdateObserver(NULL);

        FUNCTION_EXIT;
    }


    //
    // emptySeries
    //
    void GraphedItem::emptySeries()
    {
        FUNCTION_ENTRY("emptySeries");

        m_series.Clear();
        m_lastXValue = 0.0;
        m_lastYValue = 0.0;
        
        FUNCTION_EXIT;
    }

   
    //
    // retrieveHistoricalData
    //
    PlotData GraphedItem::retrieveHistoricalData(time_t startDateTime,time_t endDateTime,const std::string& tableName, const std::string& columnName, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("retrieveHistoricalData");

        FUNCTION_EXIT;
        return m_genericItem.retrieveHistoricalData(startDateTime, endDateTime, tableName, columnName, samplePeriod);
    }


    //
    // retrieveHistoricalData
    //
    PlotData GraphedItem::retrieveHistoricalData(time_t time, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("retrieveHistoricalData");

        FUNCTION_EXIT;
        return m_genericItem.retrieveHistoricalData(time, tableName, columnName, samplePeriod);
    }


    //
    // setLiveMode
    //
    void GraphedItem::setLiveMode(bool isLive, long samplePeriod)
    {
        FUNCTION_ENTRY("setLiveMode");

        //
        // If the sample period is set to 0, then the graph will be updated with live
        // field updates. The graph will be refreshed with time every minute.
        // If the sample period has been set to anything other than 0, the graph will 
        // be updated with the new database value at the frequency of the sample period.
        // 

        bool fieldUpdates = false;
        long refreshInterval = samplePeriod;
        if(samplePeriod == LIVE_FIELD_UPDATES)
        {
            m_genericItem.setLiveMode(isLive);
            refreshInterval = Utilities::getGraphRefreshInterval(); //TD17529
			fieldUpdates = true;
        }
        
        if(isLive == true)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Listening for Sample Updates");

            if(m_graphRefreshThread.get() == NULL)
            {
                m_graphRefreshThread.reset(new GraphRefreshThread(*this));
                m_graphRefreshThread->setRefreshInterval(refreshInterval, fieldUpdates);
                m_graphRefreshThread->start();
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No longer listening for Sample Updates");

            if(m_graphRefreshThread.get() != NULL)
            {
                m_graphRefreshThread->terminateAndWait();
                m_graphRefreshThread.reset(NULL);
            }
        }
        

        FUNCTION_EXIT;
    }


    //
    // getActualValue
    //
    double GraphedItem::getActualValue(double value)
    {   
		//TD17940 - modified computation for scale factors to display more accurate values
	    double scaleFactor = m_genericItem.getScaleFactor();
		//TD17883 - must compute the actualvalue correctly without reference to it's minimum value
		double actualValue = value / scaleFactor;
	
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			    "point value = %.2f, actual value = %.2f, scalefactor = %.2f",
				value, actualValue, scaleFactor);		
        return actualValue;
		//return (value / scaleFactor + minimum);
		//++TD17940
    }


    //
    // newDataReceived
    //
    void GraphedItem::newDataReceived(time_t timestamp, double value, bool status)
    {
        FUNCTION_ENTRY("newDataReceived");

        TA_Base_Core::ThreadGuard guard(m_liveUpdateLock);

        // Because the time received is local time, convert it to gm time as plotPoint()
        // assumes the time is in gm and converts it back to local
		//TD17599++ - illegal time stamp causes app to crash
        //time_t gmTimestamp = TimeFunction::getInstance().ConvertToUTCTimeT(timestamp);
		//TD17883 - no need to conver time to UTC format
		//time_t gmTimestamp = (timestamp != 0) ?  TimeFunction::getInstance().ConvertToUTCTimeT(timestamp) : timestamp;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New Data Received on %d", timestamp);

		//++TD17599 (lizettejl)

        COLORREF penColour = m_penColour;
        if(!status)
        {
            penColour = BAD_STATUS_COLOUR;
        }

        // If the value falls outside of the possible values, then set the colour to
        // indicate this is a bad value.
        if(isBooleanType())
        {
            if(value != 0 && value != 1)
            {
                penColour = BAD_STATUS_COLOUR;
            }
        }
        else
        {
            if(value > getMaximumLimit() || value < getMinimumLimit())
            {
                penColour = BAD_STATUS_COLOUR;
            }
        }
        
        // Add the point to the graph
		//TD17883
		m_graph.plotPoint(m_seriesNumber, timestamp, value, penColour);

        // m_lastXValue and m_lastYValue are updated in AbstractGraph::plotPoint()
        
        FUNCTION_EXIT;
    }


    //
    // updateWithCurrentData
    //
    void GraphedItem::updateWithCurrentData(time_t timestamp, long samplePeriod)
    {
        FUNCTION_ENTRY("updateWithCurrentData");

        TA_Base_Core::ThreadGuard guard(m_refreshLock);

        //m_graph.plotPoint(m_seriesNumber, timestamp, m_lastYValue, m_penColour);

        if(samplePeriod == LIVE_FIELD_UPDATES)
        {
            // Don't plot with the last value. Just move the graph to the current time.
			//TD17883 - must not plot the last value here, causes the whole graph to be redrawn		 
			//m_graph.plotPoint(m_seriesNumber, timestamp, m_genericItem.retrieveLastKnownValue(), m_penColour);			 			
            m_graph.moveGraph(timestamp);
        }
        else
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Reload live sample updates");
            // If the sample period is anything but 0, the series will be reloaded
            // with the new timestamp in the range
            newDataReceived(timestamp, 0, true);
        }
        
        m_lastXValue = timestamp;

        FUNCTION_EXIT;
    }

}

#endif // GraphedItemCPP_2AA190CC_D9E4_4eb9_91B0_D009A95E1275__INCLUDED_

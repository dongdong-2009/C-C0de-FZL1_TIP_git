#if !defined(GraphTypeFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define GraphTypeFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * GraphTypeFactory.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphTypeFactory.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 16:11:24 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is a factory to create concrete classes for all the interfaces.
  *
  */

#include "app/trending/trend_viewer/src/GraphTypeFactory.h"
#include "app/trending/trend_viewer/src/IGraph.h"
#include "app/trending/trend_viewer/src/LineGraph.h"
#include "app/trending/trend_viewer/src/BarGraph.h"
#include "app/trending/trend_viewer/src/ComparisonGraph.h"
#include "app/trending/trend_viewer/src/XYGraph.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_App
{
    GraphTypeFactory* GraphTypeFactory::s_graphTypeFactory = NULL;	
     //TA_Base_Core::NonReEntrantThreadLockable GraphTypeFactory::s_singletonLock; //TD17935

    //
    // getInstance
    //
    GraphTypeFactory& GraphTypeFactory::getInstance()
    {
        // protect the instance check / creation
		// TA_Base_Core::ThreadGuard guard(s_singletonLock); //TD17935

        if(s_graphTypeFactory == 0)
        {
            s_graphTypeFactory = new GraphTypeFactory();
        }
        
        return *s_graphTypeFactory;
    }


    //
    // getLineGraph
    //
    std::auto_ptr<IGraph> GraphTypeFactory::createLineGraph(CTChart& chart, const std::string& name, 
        const std::string& description, time_t startTime, time_t endTime,
        bool isLive, long samplePeriod, IItemStore* itemStore, IArchiveDataStore* archiveDataStore,
        IGraphObserver* graphObserver)
    {
        FUNCTION_ENTRY("createLineGraph");

        return std::auto_ptr<IGraph>(new LineGraph(chart, 
                            name,
                            description,
                            startTime,
                            endTime,
                            isLive,
                            samplePeriod,
                            itemStore,
                            archiveDataStore,
                            graphObserver));

        FUNCTION_EXIT;
    }

    
    //
    // getBarGraph
    //
    std::auto_ptr<IGraph> GraphTypeFactory::createBarGraph(CTChart& chart, const std::string& name, 
        const std::string& description, time_t startTime, time_t endTime,
        bool isLive, long samplePeriod, IItemStore* itemStore, IArchiveDataStore* archiveDataStore,
        IGraphObserver* graphObserver, bool isFieldUpdates)
    {
        FUNCTION_ENTRY("createBarGraph");

        return std::auto_ptr<IGraph>(new BarGraph(chart, 
                            name,
                            description,
                            startTime,
                            endTime,
                            isLive,
                            samplePeriod,
                            itemStore,
                            archiveDataStore, 
                            graphObserver,
                            isFieldUpdates));

        FUNCTION_EXIT;
    }


    //
    // getComparisonGraph
    //
    std::auto_ptr<IGraph> GraphTypeFactory::createComparisonGraph(CTChart& chart, const std::string& name, 
        const std::string& description, time_t startTime, time_t endTime,
        bool isLive, long samplePeriod, IItemStore* itemStore, IArchiveDataStore* archiveDataStore,
        IGraphObserver* graphObserver, time_t offset, int leftChartMargin, long cursorToolIndex,
        long axisArrowToolIndex)
    {
        FUNCTION_ENTRY("createComparisonGraph");

        return std::auto_ptr<IGraph>(new ComparisonGraph(chart, 
                            name,
                            description,
                            startTime,
                            endTime,
                            isLive,
                            samplePeriod,
                            itemStore,
                            archiveDataStore,
                            graphObserver,
                            offset, 
                            leftChartMargin,
                            cursorToolIndex,
                            axisArrowToolIndex));

        FUNCTION_EXIT;
    }


    //
    // getXYGraph
    //
    std::auto_ptr<IGraph> GraphTypeFactory::createXYGraph(CTChart& chart, const std::string& name, 
        const std::string& description, time_t startTime, time_t endTime,
        bool isLive, long samplePeriod, IItemStore* itemStore, IArchiveDataStore* archiveDataStore,
        IGraphObserver* graphObserver, long axisArrowToolIndex, int leftChartMargin)
    {
        FUNCTION_ENTRY("createXYGraph");

        return std::auto_ptr<IGraph>(new XYGraph(chart, 
                            name,
                            description,
                            startTime,
                            endTime,
                            isLive,
                            samplePeriod,
                            itemStore,
                            archiveDataStore,
                            graphObserver,
                            axisArrowToolIndex,
                            leftChartMargin));

        FUNCTION_EXIT;
    }
    
}

#endif // GraphTypeFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

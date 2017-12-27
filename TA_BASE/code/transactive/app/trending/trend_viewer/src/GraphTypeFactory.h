#if !defined(GraphTypeFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define GraphTypeFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * GraphTypeFactory.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphTypeFactory.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a factory to create concrete classes for all the interfaces.
  *
  */

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"
#include "app/trending/trend_viewer/src/IGraphObserver.h"
#include "app/trending/trend_viewer/src/IItemStore.h"

//#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD17935

// TeeChart includes
#include "cots/TeeChart/tchart.h"

namespace TA_Base_App
{
    // 
    // Forward declaration
    //
    class IGraph;
    
    //
    // GraphTypeFactory
    //
    class GraphTypeFactory 
    {

    public:

        /**
          * getInstance
          * 
          * Returns the one and only GraphTypeFactory instance.
          *
          * @return The GraphTypeFactory instance
          *
          */
        static GraphTypeFactory& getInstance();
    
        /**
          * createLineGraph
          * 
          * Creates a Line Graph object and return a pointer to it.
          *
          * @return Pointer to LineGraph object
          *
          */
        std::auto_ptr<IGraph> createLineGraph(CTChart& chart, const std::string& name, const std::string& description, 
            time_t startTime, time_t endTime, bool isLive, long samplePeriod, 
            IItemStore* itemStore, IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver);
        
        /**
          * createBarGraph
          * 
          * Creates a Bar Graph object and return a pointer to it.
          *
          * @return Pointer to BarGraph object
          *
          */
        std::auto_ptr<IGraph> createBarGraph(CTChart& chart, const std::string& name, const std::string& description, 
            time_t startTime, time_t endTime, bool isLive, long samplePeriod, 
            IItemStore* itemStore, IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver,
            bool isFieldUpdates);
        
        /**
          * createComparisonGraph
          * 
          * Creates a Comparison Graph object and return a pointer to it.
          *
          * @return Pointer to ComparisonGraph object
          *
          */
        std::auto_ptr<IGraph> createComparisonGraph(CTChart& chart, const std::string& name, 
            const std::string& description, time_t startTime, time_t endTime, bool isLive, 
            long samplePeriod, IItemStore* itemStore, IArchiveDataStore* archiveDataStore, 
            IGraphObserver* graphObserver, time_t offset, int leftChartMargin, long cursorToolIndex, 
            long axisArrowToolIndex);
        
        /**
          * createXYGraph
          * 
          * Creates a XY Graph object and return a pointer to it.
          *
          * @return Pointer to XYGraph object
          *
          */
        std::auto_ptr<IGraph> createXYGraph(CTChart& chart, const std::string& name, const std::string& description, 
            time_t startTime, time_t endTime, bool isLive, long samplePeriod, 
            IItemStore* itemStore, IArchiveDataStore* archiveDataStore, IGraphObserver* graphObserver,
            long axisArrowToolIndex, int leftChartMargin);
        

    private:

        GraphTypeFactory() {}
        GraphTypeFactory(const GraphTypeFactory& theGraphTypeFactory);
        GraphTypeFactory& operator=(const GraphTypeFactory&);

    private:

        // the one and only class instance
        static GraphTypeFactory* s_graphTypeFactory;

        // protect singleton creation
		//TD17935 - no need to protect singleton creation thru a thread
		// static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock; 
    };

}

#endif // GraphTypeFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

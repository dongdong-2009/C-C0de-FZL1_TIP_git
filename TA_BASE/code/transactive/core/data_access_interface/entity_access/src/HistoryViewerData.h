#ifndef HISTORYVIEWERDATA_H_INCLUDED
#define HISTORYVIEWERDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/HistoryViewerData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4250 4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IHistoryViewerData.h"


namespace TA_Base_Core
{
	class HistoryViewerData : public IHistoryViewerData, public EntityData
    {
    public:

        HistoryViewerData() {}

        HistoryViewerData(unsigned long key) 
            : EntityData( key, getStaticType() )
		{
		}

        static IEntityData* clone(unsigned long key)
        {
            return new HistoryViewerData(key);        
        }

        void invalidate();

        std::string getType();
		static std::string getStaticType();


        /**
         * getGraphRefreshInterval
         *
         * Returns the Graph Refresh Interval for this entity
         *
         * @return The frequency at which to refresh the graph
         *
         */
        int getGraphRefreshInterval();

        /**
         * getMaxTrendsDisplayed
         *
         * Returns the maximum number of trends allowed to be displayed
         *
         * @return The maximum number of trends allowed
         *
         */
        int getMaxTrendsDisplayed();

        /**
         * getMaxPointsForOtherGraphs
         *
         * Returns the maximum number of points allowed to be plotted for line, XY and comparison graphs
         *
         * @return The maximum number of points allowed
         *
         */
        int getMaxPointsForOtherGraphs(); //TD17718

        /**
         * getMaxPointsForBarGraph
         *
         * Returns the maximum number of points allowed to be plotted for a bar graph
         *
         * @return The maximum number of points allowed
         *
         */
        int getMaxPointsForBarGraph();	//TD17718

    /**
     * implement methods as required by the interfaces from
     * which this class inherits
     */
    private:

        HistoryViewerData(HistoryViewerData&);

        HistoryViewerData& operator=(HistoryViewerData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string GRAPHREFRESHINTERVAL;
        static const std::string MAXTRENDSDISPLAYED;
		//TD17718
        static const std::string MAXPOINTSFOROTHERGRAPHS;
        static const std::string MAXPOINTSFORBARGRAPH;
		//++TD17718

    };
    
    typedef boost::shared_ptr<HistoryViewerData> HistoryViewerDataPtr;
}

#endif // HISTORYVIEWERDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/HistoryViewerData.cpp $
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
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string HistoryViewerData::ENTITY_TYPE                = "TrendViewer";
    const std::string HistoryViewerData::GRAPHREFRESHINTERVAL       = "GraphRefreshInterval";
    const std::string HistoryViewerData::MAXTRENDSDISPLAYED         = "MaxTrendsDisplayed";
	//TD17718
    const std::string HistoryViewerData::MAXPOINTSFOROTHERGRAPHS    = "MaxPointsForOtherGraphs";
    const std::string HistoryViewerData::MAXPOINTSFORBARGRAPH       = "MaxPointsForBarGraph";
	//++TD17718

    void HistoryViewerData::invalidate()
    {
        getHelper()->invalidate();
    }

    
    int HistoryViewerData::getGraphRefreshInterval()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        int graphRefreshInterval = 0;
		std::string value = getHelper()->getParameter(GRAPHREFRESHINTERVAL);
		if (!value.empty())
		{
			graphRefreshInterval = EntityHelper::getIntegerData(value);
		}

        return graphRefreshInterval;
    }

    
    int HistoryViewerData::getMaxTrendsDisplayed()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
		int maxTrendsDisplayed = 0;
		std::string value = getHelper()->getParameter(MAXTRENDSDISPLAYED);
		if (!value.empty())
		{
			maxTrendsDisplayed = EntityHelper::getIntegerData(value);
		}
        return maxTrendsDisplayed;
    }

	//TD17718
    int HistoryViewerData::getMaxPointsForOtherGraphs()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        int maxPointsForOtherGraphs = 0;
		std::string value = getHelper()->getParameter(MAXPOINTSFOROTHERGRAPHS);
		if (!value.empty())
		{
			maxPointsForOtherGraphs = EntityHelper::getIntegerData(value);
		}

        return maxPointsForOtherGraphs;
    }

    //TD17718
    int HistoryViewerData::getMaxPointsForBarGraph()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
		int maxPointsForBarGraph = 0;
		std::string value = getHelper()->getParameter(MAXPOINTSFORBARGRAPH);
		if (!value.empty())
		{
			maxPointsForBarGraph = EntityHelper::getIntegerData(value);
		}
        return maxPointsForBarGraph;
    }

    std::string HistoryViewerData::getType()
    {
        return getStaticType();
    }

    std::string HistoryViewerData::getStaticType()
    {
        return ENTITY_TYPE;
    }

} // end TA_Base_Core


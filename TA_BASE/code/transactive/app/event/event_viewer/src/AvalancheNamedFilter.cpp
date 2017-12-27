/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/AvalancheNamedFilter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class implements the dynamic filtering ability required to filter
  * by the alarm ID of the currently selected event.
  *
  */

#include "stdafx.h"

#include <string>
#include <vector>

#include "app/event/event_viewer/eventlistcomponent/src/Filter.h"
#include "app/event/event_viewer/src/AvalancheNamedFilter.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"

namespace
{
    const std::string TEXT_NO_ALARMID( "No Alarm Filter" );
}

const std::string AvalancheNamedFilter::NAMEDFILTER_AVALANCHE( "Avalanche Child Events" );

AvalancheNamedFilter::AvalancheNamedFilter( Filter* filter ) : m_filter(filter)
{
}


AvalancheNamedFilter::~AvalancheNamedFilter()
{
}


TA_Base_Core::NamedFilter* AvalancheNamedFilter::clone()
{
    // Prepare the state of the filter.
    removeStringFilter( CombinedEventFilter::FILTER_ALARMID );

    // Set the default value for the filter.
    std::string alarmIdFilter = TEXT_NO_ALARMID;

    // Now get the alarm ID of the first event.
    const std::vector<unsigned long>& selected = m_filter->getSelectedItems();
    if ( 0 < selected.size() )
    {
        // Get the first selected item.
        std::string alarmId = m_filter->getItemData( selected[0], CombinedEventFilter::FILTER_ALARMID );

        // If the alarm ID exists, use it.
        if ( 0 < alarmId.size() )
        {
            alarmIdFilter = alarmId;
        }
    }

    // Add the alarm id to the filter.
    addStringFilter( CombinedEventFilter::FILTER_ALARMID, alarmIdFilter );

    // Reapply the filter name.
    setName( NAMEDFILTER_AVALANCHE );

    // Finally, clone the filter.
    return NamedFilter::clone();
}

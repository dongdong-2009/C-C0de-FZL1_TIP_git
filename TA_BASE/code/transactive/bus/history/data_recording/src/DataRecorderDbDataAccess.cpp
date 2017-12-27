/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataRecorderDbDataAccess.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 *
 */

#ifdef __WIN32__
#pragma warning (disable : 4786)
#endif

#include <sstream>

#include "bus/history/data_recording/src/DataRecorderDbDataAccess.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"


namespace TA_Base_Bus
{

    std::vector<TA_Base_Core::IRecordableDataPtr> DataRecorderDbDataAccess::getRecordableDataByType( unsigned long typeKey )
	{
        FUNCTION_ENTRY("getRecordableDataByType");

        std::vector<TA_Base_Core::ILivePropertyDataPtr> liveProperties = TA_Base_Core::LivePropertyAccessFactory::getInstance().getLivePropertiesByType( typeKey );
        std::vector<TA_Base_Core::ILivePropertyDataPtr>::iterator iter = liveProperties.begin();

        // Cast ILivePropertyDataPtr to IRecordableDataPtr
        std::vector<TA_Base_Core::IRecordableDataPtr> data;
        for(; iter != liveProperties.end(); ++iter)
        {
            TA_Base_Core::IRecordableDataPtr recordableData = *iter;
            data.push_back(recordableData);
        }

        FUNCTION_EXIT;
        return data;
	}


    std::vector<TA_Base_Core::IRecordableItemPtr> DataRecorderDbDataAccess::getRecordableItemsByType( unsigned long typeKey )
	{
        FUNCTION_ENTRY("getRecordableItemsByType");
        std::vector<TA_Base_Core::IRecordableItemPtr> items = TA_Base_Core::RecordableItemAccessFactory::getInstance().getRecordableItemsByType( typeKey );;

        FUNCTION_EXIT;
        return items;
	}
}


/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/IConfigDataObserver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class should be implemented by clients who want to be notified when any
  * of the config data held by DataCache is changed in the database.
  * Note that DataCache does not automatically update its contents when config
  * changes occur, it merely passes the notification onwards via this interface.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


IConfigDataObserver::IConfigDataObserver(DataCache& dataCache) :
    SeObserver<DataCache>(dataCache)
{
    FUNCTION_ENTRY("IConfigDataObserver(DataCache& dataCache)");
    FUNCTION_EXIT;
}


IConfigDataObserver::~IConfigDataObserver()
{
    FUNCTION_ENTRY("~IConfigDataObserver()");
    FUNCTION_EXIT;
}


void IConfigDataObserver::update(DataCache& dataCache)
{
    FUNCTION_ENTRY("update(DataCache& dataCache)");

    const DataUpdate* dataUpdate = dataCache.getDataUpdate();

    if (dataUpdate != NULL)
    {
        const DataUpdate dataUpdateCopy(*dataUpdate);

        dataChanged(dataUpdateCopy);
    }

    FUNCTION_EXIT;
}

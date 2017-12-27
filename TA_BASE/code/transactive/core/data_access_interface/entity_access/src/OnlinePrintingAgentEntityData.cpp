/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/OnlinePrintingAgentEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Online Printing Agent entity configuration access.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/OnlinePrintingAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string OnlinePrintingAgentEntityData::ENTITY_TYPE                    = "OnlinePrintingAgent";
    const std::string OnlinePrintingAgentEntityData::EVENT_VIEW_PARAM               = "EventView";
    const std::string OnlinePrintingAgentEntityData::VIEW_POLL_FREQUENCY_PARAM      = "ViewPollFrequency";
    const std::string OnlinePrintingAgentEntityData::OUTPUT_COLUMNS_PARAM           = "OutputColumns";
    const std::string OnlinePrintingAgentEntityData::PRINT_COMMAND_PARAM            = "PrintCommand";
    const std::string OnlinePrintingAgentEntityData::IS_PRINTING_ENABLED_PARAM      = "IsPrintingEnabled";
    const std::string OnlinePrintingAgentEntityData::PRINT_ALARM_EVENTS_ONLY_PARAM  = "PrintAlarmEventsOnly";
    const std::string OnlinePrintingAgentEntityData::ASSET_NAME                     = "AssetName";


OnlinePrintingAgentEntityData::OnlinePrintingAgentEntityData()
{
}


OnlinePrintingAgentEntityData::OnlinePrintingAgentEntityData(unsigned long key) : 
    EntityData(key, getStaticType())
{
}


OnlinePrintingAgentEntityData::~OnlinePrintingAgentEntityData()
{ 
    // Nothing to clean up
}


IEntityData* OnlinePrintingAgentEntityData::clone(unsigned long key)
{
    return new OnlinePrintingAgentEntityData(key);        
}


std::string OnlinePrintingAgentEntityData::getType()
{
    return getStaticType();
}


std::string OnlinePrintingAgentEntityData::getStaticType()
{
        return ENTITY_TYPE;
}


std::string OnlinePrintingAgentEntityData::getEventView()
{
	return getHelper()->getParameter(EVENT_VIEW_PARAM);
}

unsigned long OnlinePrintingAgentEntityData::getViewPollFrequency()
{
    unsigned long viewPollFrequency = 0;
	std::string value = getHelper()->getParameter(VIEW_POLL_FREQUENCY_PARAM);
	if (!value.empty())
	{
		viewPollFrequency = EntityHelper::getUnsignedLongData(value);
	}

    return viewPollFrequency;
}


std::string OnlinePrintingAgentEntityData::getOutputColumns()
{
	return getHelper()->getParameter(OUTPUT_COLUMNS_PARAM);
}


std::string OnlinePrintingAgentEntityData::getPrintCommand()
{
	return getHelper()->getParameter(PRINT_COMMAND_PARAM);
}


bool OnlinePrintingAgentEntityData::isPrintingEnabled()
{
	bool isPrintingEnabled = false;
	std::string value = getHelper()->getParameter(IS_PRINTING_ENABLED_PARAM);
	if (!value.empty())
	{
		isPrintingEnabled = EntityHelper::getBooleanData(value);
	}

    return isPrintingEnabled;
}


bool OnlinePrintingAgentEntityData::printAlarmEventsOnly()
{
    bool printAlarmEventsOnly = false;
	std::string value = getHelper()->getParameter(PRINT_ALARM_EVENTS_ONLY_PARAM);
	if (!value.empty())
	{
		printAlarmEventsOnly = EntityHelper::getBooleanData(value);
	}

    return printAlarmEventsOnly;
}

std::string OnlinePrintingAgentEntityData::getAssetName()
{
	return getHelper()->getParameter(ASSET_NAME);
}


void OnlinePrintingAgentEntityData::invalidate()
{
	getHelper()->invalidate();
}

}

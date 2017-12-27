/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPriorityStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a message priority parameter in a Station/Train TIS step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "Resource.h"
#include "TisPriorityStepParameter.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisPriorityStepParameter

TisPriorityStepParameter::TisPriorityStepParameter(PlanStep& parent, std::string name, unsigned short priority, bool isHidden) :
    NumericStepParameter(parent, name, priority, isHidden)
{
    FUNCTION_ENTRY( "TisPriorityStepParameter" );

    std::set<unsigned short> normalReservedPriorities;
    normalReservedPriorities.insert(6); // used for RATIS

    m_priorityMap.insert(std::make_pair(TA_Base_Core::EMERGENCY_SECTION, PriorityRange(1, 3)));
    m_priorityMap.insert(std::make_pair(TA_Base_Core::NORMAL_SECTION, PriorityRange(4, 8, normalReservedPriorities)));

    FUNCTION_EXIT;
}


TisPriorityStepParameter::TisPriorityStepParameter(PlanStep& parent, const TisPriorityStepParameter& parameter) :
    NumericStepParameter(parent, parameter),
    m_priorityMap(parameter.m_priorityMap)
{
    FUNCTION_ENTRY( "TisPriorityStepParameter" );
    FUNCTION_EXIT;
}


TisPriorityStepParameter::TisPriorityStepParameter(const TisPriorityStepParameter& parameter) :
    NumericStepParameter(parameter),
    m_priorityMap(parameter.m_priorityMap)
{
    FUNCTION_ENTRY( "TisPriorityStepParameter" );
    FUNCTION_EXIT;
}


TisPriorityStepParameter::~TisPriorityStepParameter()
{
    FUNCTION_ENTRY( "~TisPriorityStepParameter" );
    FUNCTION_EXIT;
}


void TisPriorityStepParameter::addToParameterList(CPlanStepParameterListCtrl& listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    unsigned short priority(static_cast<unsigned short>(getNumericValue()));

    listCtrl.AddParameter(getName().c_str(), (isHiddenValue()? "" : getPriorityLabel(priority).c_str()), this);

    FUNCTION_EXIT;
}


std::vector<unsigned short> TisPriorityStepParameter::getTisPriorities(TA_Base_Core::ELibrarySection librarySection) const
{
    FUNCTION_ENTRY( "getTisPriorities" );

    std::vector<unsigned short> priorities;
    PriorityMap::const_iterator pIt(m_priorityMap.find(librarySection));

    if (pIt != m_priorityMap.end())
    {
        // Iterate over the entire priority range and collect all unreserved priorities.
        const std::set<unsigned short>& reserved = pIt->second.reserved;

        for (unsigned short p(pIt->second.minPriority); p <= pIt->second.maxPriority; p++)
        {
            if (reserved.find(p) == reserved.end())
            {
                priorities.push_back(p);
            }
        }
    }
    else
    {
        std::ostringstream error;
        error << "Unsupported library section type " << librarySection;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.str().c_str());
    }

    FUNCTION_EXIT;
    return priorities;
}


std::string TisPriorityStepParameter::getPriorityLabel(unsigned short priority) const
{
    FUNCTION_ENTRY( "getPriorityLabel" );

    for (PriorityMap::const_iterator pIt(m_priorityMap.begin()); pIt != m_priorityMap.end(); pIt++)
    {
        if (priority >= pIt->second.minPriority && priority <= pIt->second.maxPriority)
        {
            std::ostringstream priorityStr;
            priorityStr << Utilities::convertToString(pIt->first) << " - " << priority;

            FUNCTION_EXIT;
            return priorityStr.str();
        }
    }

    FUNCTION_EXIT;
    return "Undefined priority";
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

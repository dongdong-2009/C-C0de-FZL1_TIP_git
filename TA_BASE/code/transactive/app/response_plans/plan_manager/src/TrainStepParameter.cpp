/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Represents the Train parameter of a Station TIS/PA step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "TrainStepParameter.h"
#include "Utilities.h"
#include "PlanStepParameterEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TrainStepParameter

TrainStepParameter::TrainStepParameter(PlanStep& parent, const std::string& name) :
    PlanStepParameter(parent, name)
{
    FUNCTION_ENTRY( "TrainStepParameter" );

    m_destination.trainId = TA_Base_Core::TIS_UNDEFINED_TRAIN_ID;
    m_destination.trainGroup = TA_Base_Core::TG_UNDEFINED;

    FUNCTION_EXIT;
}


TrainStepParameter::TrainStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TrainDestination& destination) :
    PlanStepParameter(parent, name),
    m_destination(destination)
{
    FUNCTION_ENTRY( "TrainStepParameter" );
    FUNCTION_EXIT;
}


TrainStepParameter::TrainStepParameter(PlanStep& parent, const TrainStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_destination(parameter.getDestination())
{
    FUNCTION_ENTRY( "TrainStepParameter" );
    FUNCTION_EXIT;
}


TrainStepParameter::TrainStepParameter(const TrainStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_destination(parameter.getDestination())
{
    FUNCTION_ENTRY( "TrainStepParameter" );
    FUNCTION_EXIT;
}


TrainStepParameter::~TrainStepParameter()
{
    FUNCTION_ENTRY( "~TrainStepParameter" );
    FUNCTION_EXIT;
}


void TrainStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    std::ostringstream destStr;

    if (m_destination.trainGroup == TA_Base_Core::TG_UNDEFINED)
    {
        if (m_destination.trainId != TA_Base_Core::TIS_UNDEFINED_TRAIN_ID)
        {
            destStr << "ID " << m_destination.trainId;
        }
    }
    else
    {
        destStr << Utilities::convertToString(m_destination.trainGroup);
    }

    listCtrl.AddParameter(getName().c_str(), destStr.str().c_str(), this);

    FUNCTION_EXIT;
}


void TrainStepParameter::setDestination(const TA_Base_Core::TrainDestination& destination)
{
    FUNCTION_ENTRY( "setDestination" );

    m_destination = destination;

    FUNCTION_EXIT;
}


void TrainStepParameter::setTrainId(unsigned short trainId)
{
    FUNCTION_ENTRY( "setTrainId" );

    m_destination.trainId = trainId;
    m_destination.trainGroup = TA_Base_Core::TG_UNDEFINED;

    FUNCTION_EXIT;
}


void TrainStepParameter::setTrainGroup(TA_Base_Core::ETrainGroup trainGroup)
{
    FUNCTION_ENTRY( "setTrainGroup" );

    m_destination.trainId = TA_Base_Core::TIS_UNDEFINED_TRAIN_ID;
    m_destination.trainGroup = trainGroup;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* TrainStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this, canEdit);
}


#endif // defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

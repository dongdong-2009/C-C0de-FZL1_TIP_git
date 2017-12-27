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
  * <description>
  *
  **/

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "RecordingUnitStepParameter.h"
#include "PlanStepParameterEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

RecordingUnitStepParameter::RecordingUnitStepParameter(PlanStep& parent,std::string name, std::string recordingUnit) :
    PlanStepParameter(parent,name),
    m_recordingUnit(recordingUnit)
{
    FUNCTION_ENTRY( "RecordingUnitStepParameter" );
    FUNCTION_EXIT;
}


RecordingUnitStepParameter::RecordingUnitStepParameter(PlanStep& parent, const RecordingUnitStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_recordingUnit(parameter.getRecordingUnit())
{
    FUNCTION_ENTRY( "RecordingUnitStepParameter" );
    FUNCTION_EXIT;
}


RecordingUnitStepParameter::RecordingUnitStepParameter(const RecordingUnitStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_recordingUnit(parameter.getRecordingUnit())
{
    FUNCTION_ENTRY( "RecordingUnitStepParameter" );
    FUNCTION_EXIT;
}


RecordingUnitStepParameter::~RecordingUnitStepParameter()
{
    FUNCTION_ENTRY( "~RecordingUnitStepParameter" );
    FUNCTION_EXIT;
}


void RecordingUnitStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_recordingUnit.c_str(),this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* RecordingUnitStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}



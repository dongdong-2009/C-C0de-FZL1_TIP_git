/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#include "core/utilities/src/TAAssert.h"
#include "DataPointStepParameter.h"
#include "PlanStepParameterDataPointEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointStepParameter::DataPointStepParameter(PlanStep& parent,std::string name, std::string dataPoint) :
    PlanStepParameter(parent,name),
    m_dataPoint(dataPoint)
{
    FUNCTION_ENTRY( "DataPointStepParameter" );
    FUNCTION_EXIT;
}


DataPointStepParameter::DataPointStepParameter(PlanStep& parent, DataPointStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_dataPoint(parameter.getDataPoint())
{
    FUNCTION_ENTRY( "DataPointStepParameter" );
    FUNCTION_EXIT;
}


DataPointStepParameter::DataPointStepParameter(DataPointStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_dataPoint(parameter.getDataPoint())
{
    FUNCTION_ENTRY( "DataPointStepParameter" );
    FUNCTION_EXIT;
}


DataPointStepParameter::~DataPointStepParameter()
{
    FUNCTION_ENTRY( "~DataPointStepParameter" );
    FUNCTION_EXIT;
}


void DataPointStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_dataPoint.c_str(),this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* DataPointStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterDataPointEditDlg(*this,canEdit);
}


void DataPointStepParameter::setDataPoint(std::string dataPoint)
{
    FUNCTION_ENTRY( "setDataPoint" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_dataPoint = dataPoint;

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

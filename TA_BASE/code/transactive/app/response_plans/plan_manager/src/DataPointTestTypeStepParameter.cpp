/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTestTypeStepParameter.cpp $
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

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "DataPointTestTypeStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointTestTypeStepParameter::DataPointTestTypeStepParameter(PlanStep& parent,std::string name, TA_Base_Core::EDataPointTest testType) :
    PlanStepParameter(parent,name),
    m_testType(testType)
{
    FUNCTION_ENTRY( "DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}


DataPointTestTypeStepParameter::DataPointTestTypeStepParameter(PlanStep& parent, const DataPointTestTypeStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_testType(parameter.getDataPointTestType())
{
    FUNCTION_ENTRY( "DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}


DataPointTestTypeStepParameter::DataPointTestTypeStepParameter(const DataPointTestTypeStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_testType(parameter.getDataPointTestType())
{
    FUNCTION_ENTRY( "DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}


DataPointTestTypeStepParameter::~DataPointTestTypeStepParameter()
{
    FUNCTION_ENTRY( "~DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}


void DataPointTestTypeStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    std::string testTypeStr = (m_testType == TA_Base_Core::UNDEFINED_TEST? "" : Utilities::convertToString(m_testType));

    listCtrl.AddParameter(getName().c_str(), testTypeStr.c_str(), this);

    FUNCTION_EXIT;
}


void DataPointTestTypeStepParameter::setDataPointTestType(TA_Base_Core::EDataPointTest testType)
{
    FUNCTION_ENTRY( "setDataPointTestType" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_testType = testType;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* DataPointTestTypeStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK)

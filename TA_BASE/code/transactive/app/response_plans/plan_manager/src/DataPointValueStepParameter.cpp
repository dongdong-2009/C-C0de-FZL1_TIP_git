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

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "DataPointValueStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "LabelMaker.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointValueStepParameter::DataPointValueStepParameter(PlanStep& parent,std::string name, TA_Base_Core::DataPointValue value) :
    PlanStepParameter(parent,name),
    m_value(value)
{
    FUNCTION_ENTRY( "DataPointValueStepParameter" );
    FUNCTION_EXIT;
}


DataPointValueStepParameter::DataPointValueStepParameter(PlanStep& parent, const DataPointValueStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_value(parameter.getDataPointValue())
{
    FUNCTION_ENTRY( "DataPointValueStepParameter" );
    FUNCTION_EXIT;
}


DataPointValueStepParameter::DataPointValueStepParameter(const DataPointValueStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_value(parameter.getDataPointValue())
{
    FUNCTION_ENTRY( "DataPointValueStepParameter" );
    FUNCTION_EXIT;
}


DataPointValueStepParameter::~DataPointValueStepParameter()
{
    FUNCTION_ENTRY( "~DataPointValueStepParameter" );
    FUNCTION_EXIT;
}


void DataPointValueStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    std::string valueStr = (m_value._d() == TA_Base_Core::UNDEFINED_DATA_POINT? "" : getDataPointValueAsString());

    listCtrl.AddParameter(getName().c_str(), valueStr.c_str(), this);

    FUNCTION_EXIT;
}


std::string DataPointValueStepParameter::getDataPointValueAsString()
{
    FUNCTION_ENTRY( "getDataPointValueAsString" );

    switch (m_value._d())
    {
        case TA_Base_Core::BOOLEAN_DATA_POINT:
            FUNCTION_EXIT;
            return LabelMaker::getTrueFalseLabel(m_value.booleanValue()).operator LPCTSTR();

        case TA_Base_Core::NUMERIC_DATA_POINT:
            FUNCTION_EXIT;
            return TA_Base_Bus::StringUtilities::convertToString(m_value.numericValue().value, m_value.numericValue().precision).operator LPCTSTR();

        case TA_Base_Core::TEXT_DATA_POINT:
            FUNCTION_EXIT;
            return m_value.textValue();

        default:
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid data point value type (" << m_value._d() << ")";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DATA_POINT_SET_STEP));
        }
    }

    FUNCTION_EXIT;
    return "未定义的数据点值";
}


void DataPointValueStepParameter::setDataPointValue(TA_Base_Core::DataPointValue value)
{
    FUNCTION_ENTRY( "setDataPointValue" );

    m_value = value;

    FUNCTION_EXIT;
}


void DataPointValueStepParameter::setDataPointValue(double value, unsigned long precision)
{
    FUNCTION_ENTRY( "setDataPointValue" );

    m_value.numericValue().value = value;
    m_value.numericValue().precision = precision;

    FUNCTION_EXIT;
}


void DataPointValueStepParameter::setDataPointValue(bool value)
{
    FUNCTION_ENTRY( "setDataPointValue" );

    m_value.booleanValue(value);

    FUNCTION_EXIT;
}


void DataPointValueStepParameter::setDataPointValue(const char * value)
{
    FUNCTION_ENTRY( "setDataPointValue" );

    m_value.textValue(CORBA::string_dup(value));

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* DataPointValueStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

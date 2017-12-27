/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointValueStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_)
#define AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointValueStepParameter

    class DataPointValueStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        DataPointValueStepParameter(PlanStep& parent, std::string name, TA_Base_Core::DataPointValue value);
        DataPointValueStepParameter(PlanStep& parent, const DataPointValueStepParameter& parameter);
        DataPointValueStepParameter(const DataPointValueStepParameter& parameter);
        virtual ~DataPointValueStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        TA_Base_Core::DataPointValue getDataPointValue() const { return m_value; }
        std::string getDataPointValueAsString();

        void setDataPointValue(TA_Base_Core::DataPointValue value);
        void setDataPointValue(double value, unsigned long precision);
        void setDataPointValue(bool value);
        void setDataPointValue(const char * value);

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        DataPointValueStepParameter& operator=(const DataPointValueStepParameter& parameter);


    // Attributes
    private:
        TA_Base_Core::DataPointValue m_value;
    };
}

#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET)

#endif // !defined(AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_)

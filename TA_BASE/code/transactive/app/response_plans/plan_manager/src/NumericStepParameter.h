/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/NumericStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_)
#define AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // NumericStepParameter

    class NumericStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        NumericStepParameter(PlanStep& parent, std::string name, unsigned long number, bool isHidden = false);
        NumericStepParameter(PlanStep& parent, const NumericStepParameter& parameter);
        NumericStepParameter(const NumericStepParameter& parameter);
        virtual ~NumericStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        unsigned long getNumericValue() const  { return m_number; }
        bool isHiddenValue()            const  { return m_isHiddenValue; }

        void setValue(unsigned long number);
        void hideValue(bool isHidden);

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        NumericStepParameter& operator=(const NumericStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_number;
        bool m_isHiddenValue;
    };
}

#endif // !defined(AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_)

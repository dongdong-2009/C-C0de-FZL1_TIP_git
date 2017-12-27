/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/YesNoStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_)
#define AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // YesNoStepParameter

    class YesNoStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        YesNoStepParameter(PlanStep& parent, std::string name, bool affirmative);
        YesNoStepParameter(PlanStep& parent, const YesNoStepParameter& parameter);
        YesNoStepParameter(const YesNoStepParameter& parameter);
        virtual ~YesNoStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        bool getAffirmative() const { return m_affirmative; }
        void setAffirmative(bool affirmative);

    protected:

        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        YesNoStepParameter& operator=(const YesNoStepParameter& parameter);


    // Attributes
    private:
        bool m_affirmative;
    };
}

#endif // !defined(AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_)

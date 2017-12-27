/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TextualStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_)
#define AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TextualStepParameter

    class TextualStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        TextualStepParameter(PlanStep& parent, std::string name, std::string text);
        TextualStepParameter(PlanStep& parent, const TextualStepParameter& parameter);
        TextualStepParameter(const TextualStepParameter& parameter);
        virtual ~TextualStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        std::string getMessageText() const { return m_text; }
        void setText(std::string text);


    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        TextualStepParameter& operator=(const TextualStepParameter& parameter);


    // Attributes
    private:
        std::string m_text;
    };
}

#endif // !defined(AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_)

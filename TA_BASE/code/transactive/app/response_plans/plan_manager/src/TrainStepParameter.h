/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the Train parameter of a Station TIS/PA step.
  *
  **/

#if !defined(AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_)
#define AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TrainStepParameter

    class TrainStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        TrainStepParameter(PlanStep& parent, const std::string& name);
        TrainStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TrainDestination& destination);
        TrainStepParameter(PlanStep& parent, const TrainStepParameter& parameter);
        TrainStepParameter(const TrainStepParameter& parameter);
        virtual ~TrainStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        TA_Base_Core::TrainDestination getDestination() const { return m_destination; }

        void setDestination(const TA_Base_Core::TrainDestination& destination);
        void setTrainId(unsigned short trainId);
        void setTrainGroup(TA_Base_Core::ETrainGroup trainGroup);

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        TrainStepParameter& operator=(const TrainStepParameter& parameter);


    // Attributes
    private:
        TA_Base_Core::TrainDestination m_destination;
    };
}

#endif // defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#endif // !defined(AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_)

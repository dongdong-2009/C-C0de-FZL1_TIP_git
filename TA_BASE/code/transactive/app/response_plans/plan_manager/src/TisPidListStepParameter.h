/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPidListStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the PID List parameter of a Station Traveller Info System step.
  *
  **/

#if !defined(AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_)
#define AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include <string>
#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TisPidListStepParameter

    class TisPidListStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        TisPidListStepParameter(PlanStep& parent, const std::string& name);
        TisPidListStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TISPidList& pids);
        TisPidListStepParameter(PlanStep& parent, const TisPidListStepParameter& parameter);
        TisPidListStepParameter(const TisPidListStepParameter& parameter);
        virtual ~TisPidListStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);
        virtual bool editParameter(const std::string& stationName);

        TA_Base_Core::TISPidList getPids() const { return m_pids; }

        void setPids(const TA_Base_Core::TISPidList& pids);
        void setPids(const std::vector<std::string>& pids);
        void resetPids();

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        TisPidListStepParameter& operator=(const TisPidListStepParameter& parameter);


    // Attributes
    private:
        TA_Base_Core::TISPidList m_pids;
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_)

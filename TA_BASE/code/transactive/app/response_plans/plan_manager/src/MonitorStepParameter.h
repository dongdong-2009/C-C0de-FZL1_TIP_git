/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/MonitorStepParameter.h $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_MONITORSTEPPARAMETER_H__F9D9E447_F161_44B5_839D_7E744E32B90C__INCLUDED_)
#define AFX_MONITORSTEPPARAMETER_H__F9D9E447_F161_44B5_839D_7E744E32B90C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // MonitorStepParameter

    class MonitorStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        MonitorStepParameter(PlanStep& parent, std::string name, unsigned long monitorKey);
        MonitorStepParameter(PlanStep& parent, const MonitorStepParameter& parameter);
        MonitorStepParameter(const MonitorStepParameter& parameter);
        virtual ~MonitorStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setMonitorKey(unsigned long monitorKey);
        void setMonitorName(const std::string& monitorName);
        unsigned long getMonitorKey() const { return m_monitorKey; }
        std::string getMonitorName() const { return m_monitorName; }


        // this method relies on the exceptions thrown
        // by VideoSwitchAgentFactory to determine availability of
        // any monitors in the system
        bool isAnyMonitorAvailable ();

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        MonitorStepParameter& operator=(const MonitorStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_monitorKey;
        std::string m_monitorName;
    };
}

#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#endif // !defined(AFX_MONITORSTEPPARAMETER_H__F9D9E447_F161_44B5_839D_7E744E32B90C__INCLUDED_)

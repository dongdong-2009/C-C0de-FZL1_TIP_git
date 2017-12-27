/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StationStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the Station parameter of a Station TIS/PA step.
  *
  **/

#if !defined(AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // StationStepParameter

    class StationStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        StationStepParameter(PlanStep& parent, const std::string& name);
        StationStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationDestination& destination);
        StationStepParameter(PlanStep& parent, const StationStepParameter& parameter);
        StationStepParameter(const StationStepParameter& parameter);
        virtual ~StationStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        TA_Base_Core::StationDestination getDestination() const { return m_destination; }

        void setDestination(const TA_Base_Core::StationDestination& destination);
        void setStation(const std::string& station, unsigned long ulLocationkey );
        void setAllStations(const std::string& station);

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        StationStepParameter& operator=(const StationStepParameter& parameter);


    // Attributes
    private:
        TA_Base_Core::StationDestination m_destination;
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)

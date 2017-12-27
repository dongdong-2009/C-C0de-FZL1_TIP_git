/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PAZonesStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the PaZones parameter of a Station PA step.
  *
  **/

#if !defined(AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PaZonesStepParameter

    class PaZonesStepParameter : public PlanStepParameter
    {
         // Operations
    public:
         PaZonesStepParameter(PlanStep& parent, const std::string& name);
         PaZonesStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationZones& zones);
         PaZonesStepParameter(PlanStep& parent, const PaZonesStepParameter& parameter);
         PaZonesStepParameter(const PaZonesStepParameter& parameter);

         virtual ~PaZonesStepParameter();

         virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

         TA_Base_Core::StationZones getPAZones () const { return m_paZones; }
         void setPaZones(const TA_Base_Core::StationZones& zones);
         void addPaZoneID (const unsigned long zoneId);
         unsigned long getPaZoneID (const int i);
         void resetZones ();
     protected:
         virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
         PaZonesStepParameter& operator=(const PaZonesStepParameter& parameter);

         // Attributes
    private:
         TA_Base_Core::StationZones m_paZones;
    };
}

#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)

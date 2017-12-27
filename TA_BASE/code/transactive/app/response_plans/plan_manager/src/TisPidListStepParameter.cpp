/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPidListStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the PID List parameter of a Station Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include "core/utilities/src/TAAssert.h"
#include "TisPidListStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepParameterTisPidListEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisPidListStepParameter

TisPidListStepParameter::TisPidListStepParameter(PlanStep& parent, const std::string& name) :
    PlanStepParameter(parent, name),
    m_pids()
{
    FUNCTION_ENTRY( "TisPidListStepParameter" );
    FUNCTION_EXIT;
}


TisPidListStepParameter::TisPidListStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TISPidList& pids) :
    PlanStepParameter(parent, name),
    m_pids(pids)
{
    FUNCTION_ENTRY( "TisPidListStepParameter" );
    FUNCTION_EXIT;
}


TisPidListStepParameter::TisPidListStepParameter(PlanStep& parent, const TisPidListStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_pids(parameter.getPids())
{
    FUNCTION_ENTRY( "TisPidListStepParameter" );
    FUNCTION_EXIT;
}


TisPidListStepParameter::TisPidListStepParameter(const TisPidListStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_pids(parameter.getPids())
{
    FUNCTION_ENTRY( "TisPidListStepParameter" );
    FUNCTION_EXIT;
}


TisPidListStepParameter::~TisPidListStepParameter()
{
    FUNCTION_ENTRY( "~TisPidListStepParameter" );
    FUNCTION_EXIT;
}


void TisPidListStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    if (m_pids.length() == 0)
    {
        listCtrl.AddParameter(getName().c_str(), "", this);

        FUNCTION_EXIT;
        return;
    }

    // The list of PIDs may be extensive, so don't insert it as a single row as it's
    // going to be impossible to read. Instead break up the PIDs into multiple rows, with
    // a couple PIDs per each row. Only the first row is labelled with the parameter name.
    const unsigned long PIDS_PER_ROW(3);

    for (unsigned long i(0); i < m_pids.length(); )
    {
        std::string label(i < PIDS_PER_ROW? getName() : "");
        std::string pidStr("");

        for (unsigned long cnt(0); cnt < PIDS_PER_ROW && i < m_pids.length(); cnt++, i++)
        {
            if (!pidStr.empty())
            {
                pidStr += "  "; // separate the PIDs with a white space
            }

            pidStr += m_pids[i].in();
        }

        listCtrl.AddParameter(label.c_str(), pidStr.c_str(), this);
    }

    FUNCTION_EXIT;
}


void TisPidListStepParameter::setPids(const TA_Base_Core::TISPidList& pids)
{
    FUNCTION_ENTRY( "setPids" );

    m_pids = pids;

    FUNCTION_EXIT;
}


void TisPidListStepParameter::setPids(const std::vector<std::string>& pids)
{
    FUNCTION_ENTRY( "setPids" );

    m_pids.length(pids.size());

    for (unsigned long i(0); i < pids.size(); i++)
    {
        m_pids[i] = CORBA::string_dup(pids[i].c_str());
    }

    FUNCTION_EXIT;
}


void TisPidListStepParameter::resetPids()
{
    FUNCTION_ENTRY( "resetPids" );

    m_pids.length(0);

    FUNCTION_EXIT;
}


bool TisPidListStepParameter::editParameter(const std::string& stationName)
{
    FUNCTION_ENTRY( "editParameter" );

    CPlanStepParameterTisPidListEditDlg editDlg(stationName, *this, true);

    if (editDlg.DoModal() == IDOK)
    {
        setPids(editDlg.GetSelectedPids());

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


CPlanStepParameterEditDlg* TisPidListStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );

    TA_ASSERT(false, "Function not supported, use editParameter(std::string stationName) instead");

    FUNCTION_EXIT;
    return NULL;
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

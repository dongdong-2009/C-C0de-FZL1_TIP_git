/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/AutonomousPlanActiveInstanceListCtrl.cpp $
  * @author:  Rob Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Class derived from CPlanActiveInstanceListCtrl that self-manages the initial aquisition
  * and update of state for all currently active plan instances. This is a complete plugin module
  * for any client code wishing to display an MFC report style List Control of currently Active Response
  * Plans. To use, simply assign your List Control resource member to a class of this type.
  */

#include "stdafx.h"
#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/AutonomousPlanActiveInstanceListCtrl.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"

#ifdef _MSC_VER
	#pragma warning(disable : 4786)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CAutonomousPlanActiveListCtrl

// Constructor
CAutonomousPlanActiveInstanceListCtrl::CAutonomousPlanActiveInstanceListCtrl() :
    CPlanActiveInstanceListCtrl(true), m_allOwnersNeeded(true), m_allLocationsNeeded(false)
{
    SetPlanInstanceFilter(this);
    SetActivePlanAccess(this);
}

// Destructor
CAutonomousPlanActiveInstanceListCtrl::~CAutonomousPlanActiveInstanceListCtrl()
{
    Unsubscribe();
}


BEGIN_MESSAGE_MAP(CAutonomousPlanActiveInstanceListCtrl, CPlanActiveInstanceListCtrl)
	//{{AFX_MSG_MAP(CAutonomousPlanActiveInstanceListCtrl)
	//}}AFX_MSG_MAP
    ON_MESSAGE_VOID(WM_ACTIVE_PLAN_LIST_POST_INITIALISE, OnActivePlanListPostInitialise)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAutonomousPlanActiveInstanceListCtrl message handlers

void CAutonomousPlanActiveInstanceListCtrl::OnActivePlanListPostInitialise()
{
    TA_ASSERT(IsInitialised(), "Active plan list has not been initialised");

	try
	{
		// Subscribe to active plan updates
        if (m_allLocationsNeeded)
        {
            SubscribeForAllUpdates();
        }
        else
        {
			SubscribeForLocalUpdates();
        }

        // Populate the list with currently active plans
		Refresh();
	}
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());

        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210089);
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");

        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210089);
    }
}


void CAutonomousPlanActiveInstanceListCtrl::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	// Try extracting to ActivePlanDetail.
    TA_Base_Core::ActivePlanDetail *activePlanDetail = NULL;
    if ( ::IsWindow( m_hWnd ) && (message.messageState >>= activePlanDetail))
	{
        SendMessage(WM_ACTIVE_PLAN_DETAIL_UPDATE, (WPARAM)activePlanDetail);
	}
}


void CAutonomousPlanActiveInstanceListCtrl::showAllOwners(bool allOwnersNeeded)
{
    if (m_allOwnersNeeded != allOwnersNeeded)
    {
        m_allOwnersNeeded = allOwnersNeeded;

        if (allOwnersNeeded)
        {
            Refresh();
        }
        else
        {
            Purge();
        }
    }
}


void CAutonomousPlanActiveInstanceListCtrl::showAllLocations(bool allLocationsNeeded)
{
    if (m_allLocationsNeeded != allLocationsNeeded)
    {
        m_allLocationsNeeded = allLocationsNeeded;

        Unsubscribe();

        if (allLocationsNeeded)
        {
            SubscribeForAllUpdates();
            Refresh();
        }
        else
        {
            SubscribeForLocalUpdates();
            Purge();
        }
    }
}


bool CAutonomousPlanActiveInstanceListCtrl::allOwnersNeeded() const
{
    return m_allOwnersNeeded;   
}


bool CAutonomousPlanActiveInstanceListCtrl::allLocationsNeeded() const
{
    return m_allLocationsNeeded;
}


bool CAutonomousPlanActiveInstanceListCtrl::isRelevant(const TA_Base_Core::ActivePlanId& activePlanId)
{

    std::map<unsigned long, int>::iterator itFound = m_planTypeMap.find( activePlanId.plan );
    if (( itFound != m_planTypeMap.end() ) && ( itFound->second == TA_Base_Core::INVISIBLE_TYPE ))
    {
        return false;
    }

    // If my location only
    if (!allLocationsNeeded())
    {
        if (activePlanId.location != PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation())
        {
            return false;
        }
    }    

    // If my instances only
    if (!allOwnersNeeded())
    {
        try
        {
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

            if (!m_planAgentMap.getPlanAgent(activePlanId.location).isOwner(sessionID.c_str(), activePlanId))
            {
                return false;
            }
        }
        catch (const IPlanAgentCorbaDef::PlanNotFoundError & ex)
        {
            // The instance must have expired as the call to the Plan Agent was being made.
			LOG_EXCEPTION_CATCH(SourceInfo, "IPlanAgentCorbaDef::PlanNotFoundError", ex.what.in());
            return false;
        }
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");
			return false;
		}
    }

    // Satisfies the current filtering rules
    return true;
}


TA_Base_Core::ActivePlanDetails* CAutonomousPlanActiveInstanceListCtrl::getAllActivePlanDetailsAtLocation(unsigned long location)
{
    try
    {
        TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

        m_planAgentMap.getPlanAgent(location).getAllActivePlanDetails(activePlanDetailsVar.out());

        return activePlanDetailsVar._retn();
    }
    catch (const PlanAgentEntityNotFoundException &)
    {
        // No Plan Agent configured for the specified location. Ignore it.
        TA_Base_Core::ActivePlanDetails *empty = new TA_Base_Core::ActivePlanDetails();
        empty->length(0);

        return empty;
    }
}


void CAutonomousPlanActiveInstanceListCtrl::InitialiseActivePlanList()
{

    try
    {
        m_planTypeMap = TA_Base_Bus::PlanDataReader::instance()->getPlanTypeMap();
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to initialise plan type map");
    }

	if (false == IsInitialised())
	{
		// Call base class initialsation to add columns and set extended style
		CPlanActiveInstanceListCtrl::InitialiseActivePlanList();

        // InitialiseActivePlanList() may be called from PreSubclassWindow().
        // Additional initialisation tasks should be carried out after control creation has completed.
        PostMessage(WM_ACTIVE_PLAN_LIST_POST_INITIALISE);
    }
}


void CAutonomousPlanActiveInstanceListCtrl::SubscribeForLocalUpdates()
{
    std::ostringstream profileLocation;
    profileLocation << PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation(); // this can throw an exception
    TA_Base_Core::NameValuePair locationFilter("LocationKey", profileLocation.str());
    TA_Base_Core::FilterData filterData;
    filterData.push_back(&locationFilter);
    
	// Subscribe for ActivePlanUpdates filtered by LocationKey
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, this, &filterData);
}


void CAutonomousPlanActiveInstanceListCtrl::SubscribeForAllUpdates()
{
	// Subscribe for ActivePlanUpdates without any filtering
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, this, NULL);
}


void CAutonomousPlanActiveInstanceListCtrl::Unsubscribe()
{
    // Unsubscribe from active plan detail updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void TA_Base_Bus::CAutonomousPlanActiveInstanceListCtrl::OnDestroy()
{
    Unsubscribe();
}

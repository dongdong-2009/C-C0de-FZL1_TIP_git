/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.cpp $
  * @author:  Bart Golab / Rob Young
  * @version: $Revision: #2 $  *
  * Last modification: $DateTime: 2015/01/22 18:34:29 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Customised CListCtrl to display ActivePlanInstances.
  */

#include "StdAfx.h"

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <vector>
#include "core/utilities/src/TAAssert.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.h"
#include "bus/response_plans/active_plans_display/src/AutonomousPlanActiveInstanceListCtrl.h"
#include "bus/response_plans/active_plans_display/src/IPlanInstanceFilter.h"
#include "bus/response_plans/active_plans_display/src/IActivePlanAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/ComparablePlanInstance.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // Constants used by this class only 

    // Column labels
	std::string CPlanActiveInstanceListCtrl::PLAN_COLUMN_LABEL = "Plan";			    // Column 1 - Plan
    std::string CPlanActiveInstanceListCtrl::CATEGORY_COLUMN_LABEL = "Category";	    // Column 2 - Category
    std::string CPlanActiveInstanceListCtrl::INSTANCE_COLUMN_LABEL = "Instance";	    // Column 3 - Instance
    std::string CPlanActiveInstanceListCtrl::LOCATION_COLUMN_LABEL = "Location";       // Column 4 - Location
    std::string CPlanActiveInstanceListCtrl::OWNER_COLUMN_LABEL = "Owner";			    // Column 5 - Owner
    std::string CPlanActiveInstanceListCtrl::ACTIVATION_COLUMN_LABEL = "Activation";	// Column 6 - Activation time
    std::string CPlanActiveInstanceListCtrl::STATE_COLUMN_LABEL = "State";		    	// Column 7 - State
    std::string CPlanActiveInstanceListCtrl::STEP_COLUMN_LABEL = "Step";		    	// Column 8 - Step
    std::string CPlanActiveInstanceListCtrl::REMARKS_COLUMN_LABEL = "Remarks";		    // Column 9 - Remarks

    // Column Indexes
    const int PLAN_COLUMN_INDEX = 0;		    // Column 1 - Plan
    const int CATEGORY_COLUMN_INDEX = 1;	    // Column 2 - Category
    const int INSTANCE_COLUMN_INDEX = 2;	    // Column 3 - Instance
    const int LOCATION_COLUMN_INDEX = 3;	    // Column 4 - Location
    const int OWNER_COLUMN_INDEX = 4;		    // Column 5 - Owner
    const int ACTIVATION_COLUMN_INDEX = 5;		// Column 6 - Activation time
    const int STATE_COLUMN_INDEX = 6;		    // Column 7 - State
    const int STEP_COLUMN_INDEX = 7;		    // Column 8 - Step
    const int REMARKS_COLUMN_INDEX = 8;         // Column 9 - Remarks



/////////////////////////////////////////////////////////////////////////////
// CPlanActiveInstanceListCtrl

CPlanActiveInstanceListCtrl::CPlanActiveInstanceListCtrl(bool planPathVisible) :
    m_planPathVisible(planPathVisible),
    m_isDynamicallyCreated(false),
    m_initialised(false), 
    m_planInstanceFilter(NULL),
    m_activePlanAccess(NULL)
{
}

CPlanActiveInstanceListCtrl::~CPlanActiveInstanceListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPlanActiveInstanceListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CPlanActiveInstanceListCtrl)
   	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnInsertItem)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnDeleteAllItems)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_ACTIVE_PLAN_DETAIL_UPDATE, OnActivePlanDetailUpdate) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanActiveInstanceListCtrl message handlers

LRESULT CPlanActiveInstanceListCtrl::OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam)
{
	TA_Base_Core::ActivePlanDetail* activePlanDetail = reinterpret_cast<TA_Base_Core::ActivePlanDetail*>(wParam);

    PopulateInstance(*activePlanDetail);

    return 0;
}


void CPlanActiveInstanceListCtrl::OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
    SetItemData(pNMListView->iItem, reinterpret_cast<DWORD>(new TA_Base_Core::ActivePlanDetail));
	
	*pResult = 0;
}


void CPlanActiveInstanceListCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
    TA_Base_Core::ActivePlanDetail *itemData = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(GetItemData(pNMListView->iItem));
    SetItemData(pNMListView->iItem, 0);

    delete itemData;
    itemData = NULL;

	*pResult = 0;
}


void CPlanActiveInstanceListCtrl::OnDeleteAllItems(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	// TODO: Add your message handler code here
    int itemCount = GetItemCount();

    for (int position = 0; position < itemCount; position++)
    {
        TA_Base_Core::ActivePlanDetail *itemData = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(GetItemData(position));
        SetItemData(position, 0);

        delete itemData;
        itemData = NULL;
    }

	*pResult = 0;
}


int CPlanActiveInstanceListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	InitialiseActivePlanList();

	return 0;
}


void CPlanActiveInstanceListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
	
    DeleteAllItems();	
}


BOOL CPlanActiveInstanceListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
    m_isDynamicallyCreated = true;
 
	cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;

	return CListCtrl::PreCreateWindow(cs);
}


void CPlanActiveInstanceListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl::PreSubclassWindow();

    // Can only initialise here during static creation.
	if (!m_isDynamicallyCreated)
    {
        InitialiseActivePlanList();
    }
}


bool CPlanActiveInstanceListCtrl::SetReadOnly(bool readOnly)
{
    DWORD colorRef = ::GetSysColor(readOnly? COLOR_3DFACE : COLOR_WINDOW);
    if (SetBkColor(colorRef) && SetTextBkColor(colorRef))
    {
        return true;
    }
  
    return false;
}


void CPlanActiveInstanceListCtrl::SetPlanInstanceFilter(IPlanInstanceFilter* planInstanceFilter)
{
    m_planInstanceFilter = planInstanceFilter;
}


void CPlanActiveInstanceListCtrl::SetActivePlanAccess(IActivePlanAccess* activePlanAccess)
{
    m_activePlanAccess = activePlanAccess;
}


void CPlanActiveInstanceListCtrl::AutoAdjustColumns()
{
    if (m_initialised)
    {
    	CRect rect;
		GetWindowRect(&rect);
		int listWidth = rect.Width();
		
		int fixedColumnWidth = GetColumnWidth(INSTANCE_COLUMN_INDEX) +
		                       GetColumnWidth(LOCATION_COLUMN_INDEX) +
		                       GetColumnWidth(OWNER_COLUMN_INDEX) +
		                       GetColumnWidth(ACTIVATION_COLUMN_INDEX) +
		                       GetColumnWidth(STATE_COLUMN_INDEX);

		int adjustableColumnWidth = listWidth - fixedColumnWidth - 18; // allow for scrollbar

        // If there is little or no room left for adjustment, then leave the columns as is
		if (adjustableColumnWidth >= 200)
		{
            SetColumnWidth(PLAN_COLUMN_INDEX, static_cast<int>(m_planPathVisible? adjustableColumnWidth * 0.25 : 0));
            SetColumnWidth(CATEGORY_COLUMN_INDEX, static_cast<int>(m_planPathVisible? adjustableColumnWidth * 0.25 : 0));
            SetColumnWidth(STEP_COLUMN_INDEX, static_cast<int>(m_planPathVisible? adjustableColumnWidth * 0.30 : adjustableColumnWidth * 0.55));
            SetColumnWidth(REMARKS_COLUMN_INDEX, adjustableColumnWidth - GetColumnWidth (PLAN_COLUMN_INDEX) - 
									GetColumnWidth (CATEGORY_COLUMN_INDEX) - GetColumnWidth (STEP_COLUMN_INDEX));
        }
    }	
}


void CPlanActiveInstanceListCtrl::Populate(const TA_Base_Core::ActivePlanDetails &activePlanDetails)
{
//    DeleteAllItems();

	for (CORBA::ULong i = 0; i < activePlanDetails.length(); i++)
	{
        PopulateInstance(activePlanDetails[i]);
    }
}


void CPlanActiveInstanceListCtrl::PopulateInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
	// Process instance only if list is ready to accept entries
	if (m_initialised)
	{
        ProcessInstance(activePlanDetail);
	}
}


void CPlanActiveInstanceListCtrl::ProcessInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    // Ignore the first notification sent when the plan starts. It will not have
    // any valid step information in it.
    if (activePlanDetail.activePlanState == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE &&
        activePlanDetail.currentStepDetail.state == TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE)
    {
        return;
    }

	// Determine if addition or existing and set local flag
    int position = FindInstance(activePlanDetail.plan);
	bool newPlanInstance = (position == -1);
    bool hasExpired = (activePlanDetail.activePlanState == TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE);
    bool changedOwner = (activePlanDetail.activePlanState == TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE);

    // The item must be deleted if one of two things has occurred:
    // 1. the instance has reached its expiry period, or
    // 2. the instance changed owner and no longer satisfies the filtering rules.
    if (hasExpired || 
        changedOwner && m_planInstanceFilter && !m_planInstanceFilter->isRelevant(activePlanDetail.plan))
    {
        // Delete the instance if it exists in the list
        if (!newPlanInstance)
        {
            DeleteInstance(position);
        }

		return; // exit method so not to apply details to deprecated index
    }

	// Add if new item
	if (newPlanInstance)
	{ 
        // If filter is enabled, make sure the new plan instance satisfies the filtering rules
        // before adding it to the list
        if (m_planInstanceFilter && !m_planInstanceFilter->isRelevant(activePlanDetail.plan))
        {
            return;
        }

		// Insert new item
		position = InsertItem(GetItemCount(), _T(""));
	}

    // If existing item, ensure active plan detail is more recent than what is currently shown.
    TA_Base_Core::ActivePlanDetail *itemData = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(GetItemData(position));
    TA_ASSERT(itemData, "Item data pointer is NULL");

    if (!newPlanInstance)
    {
        if (ComparablePlanInstance::compareUpdateIds(activePlanDetail.activeUpdateId, itemData->activeUpdateId) < 0)
        {
            return;
        } 
    }

    // Update item data
    bool execStateUpdated = false;
    TA_Base_Core::EActivePlanState previousActivePlanState = itemData->activePlanState;
    *itemData = activePlanDetail;

    if (changedOwner)
    {
        // The owner has changed but the plan's execution state hasn't so restore it.
        itemData->activePlanState = previousActivePlanState;
    }
    else
    {
        execStateUpdated = (itemData->activePlanState != previousActivePlanState);
    }

	// Format step string - if undefined leave blank
	CString currentStep = "";
    if (activePlanDetail.currentStepDetail.state != TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE)
    {
        currentStep = StringUtilities::convertToString(activePlanDetail.currentStepDetail.position);

	    CString stepName = activePlanDetail.currentStepDetail.name;
	    stepName.TrimRight();

	    if (!stepName.IsEmpty())
	    {
		    currentStep += " - " + stepName;
	    }
    }

	// Apply active plan details to the item in the list
	SetItemText(position, OWNER_COLUMN_INDEX, _T(activePlanDetail.owner.in()));
    SetItemText(position, REMARKS_COLUMN_INDEX, _T(activePlanDetail.remark.in()));

    if (!changedOwner)
    {
	    SetItemText(position, PLAN_COLUMN_INDEX, _T(StringUtilities::extractPlanName(activePlanDetail.path.in())));
	    SetItemText(position, CATEGORY_COLUMN_INDEX, _T(StringUtilities::extractCategoryPath(activePlanDetail.path.in())));
	    SetItemText(position, INSTANCE_COLUMN_INDEX, _T(StringUtilities::convertToString(activePlanDetail.plan.instance)));
        SetItemText(position, LOCATION_COLUMN_INDEX, _T(PlanAgentLocationAccess::getInstance().getLocationName(activePlanDetail.plan.location).c_str()));
        SetItemText(position, ACTIVATION_COLUMN_INDEX, _T(StringUtilities::timeToString(activePlanDetail.instantiationTime).c_str()));
	    SetItemText(position, STATE_COLUMN_INDEX, _T(StringUtilities::convertToString(activePlanDetail.activePlanState)));
	    SetItemText(position, STEP_COLUMN_INDEX, _T(currentStep));
    }

	// Re-sort the list if new instance was added or existing instance execution state has changed
	if (newPlanInstance || execStateUpdated)
	{
		SortActivePlanList();
	}
}


void CPlanActiveInstanceListCtrl::Refresh()
{
    if (m_activePlanAccess == NULL)
    {
        return;
    }

    try
    {
        CWaitCursor waitCursor;

        typedef std::vector<unsigned long> LocationList;
        LocationList locations;

        // See if remote plan instances have to be considered
        if (!m_planInstanceFilter || m_planInstanceFilter->allLocationsNeeded())
        {
            locations = PlanAgentLocationAccess::getInstance().getNonDefaultPlanAgentLocations();
        }

        // Need to get local plans always
        locations.push_back(PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation());

        // Contact the plan agent at each of the locations and get its active plan details.
        for (LocationList::iterator iter = locations.begin(); iter != locations.end(); iter++)
        {
            LocationKey location = *iter;

            try
            {
                TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar = 
                    m_activePlanAccess->getAllActivePlanDetailsAtLocation(location);

                Populate(activePlanDetailsVar);

            }
            catch (const TA_Base_Core::TransactiveException &ex) 
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", ex.what());

                std::ostringstream errorMessage;
                errorMessage << "Unable to retrieve active plans data from "
                             << PlanAgentLocationAccess::getInstance().getLocationName(location) << "." 
                             << std::endl << std::endl << ex.what();

				//TD16971 - remove message box when the plan agent cannot be contacted 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

				// TD14164 azenitha++
                /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
				//TA_Base_Bus::TransActiveMessage userMsg;
				//CString temp = "\n\n";
				//CString temp2 = ex.what();
				//std::string temp3 = PlanAgentLocationAccess::getInstance().getLocationName(location);
				//CString locationName = temp + temp2 + temp3.c_str();
				//userMsg << locationName;
				//UINT selectedButton = userMsg.showMsgBox(IDS_UE_210087);
				// TD14164 ++azenitha
				//++TD16971
            }
            catch (const IPlanAgentCorbaDef::PlanAgentError &ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "IPlanAgentCorbaDef::PlanAgentError", ex.what.in());

                std::ostringstream errorMessage;
                errorMessage << "Unable to retrieve active plans data from "
                             << PlanAgentLocationAccess::getInstance().getLocationName(location) << "." 
                             << std::endl << std::endl << ex.what.in();

				//TD16971 - remove message box when the plan agent cannot be contacted 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());
				
				// TD14164 azenitha++
                /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
				//TA_Base_Bus::TransActiveMessage userMsg;
				//CString temp = "\n\n";
				//CString temp2 = ex.what.in();
				//std::string temp3 = PlanAgentLocationAccess::getInstance().getLocationName(location);
				//CString locationName = temp + temp2 + temp3.c_str();
				//userMsg << locationName;
				//UINT selectedButton = userMsg.showMsgBox(IDS_UE_210087);
				// TD14164 ++azenitha
				//++TD16971
            }
            catch (const CORBA::Exception &ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

                std::ostringstream errorMessage;
                errorMessage << "Unable to retrieve active plans data from "
                             << PlanAgentLocationAccess::getInstance().getLocationName(location) << "." 
                             << std::endl << std::endl << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

				//TD16971 - remove message box when the plan agent cannot be contacted 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());
				// TD14164 azenitha++
                /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
				//TA_Base_Bus::TransActiveMessage userMsg;
				//CString temp = "\n\n";
				//std::string temp2 = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
				//std::string temp3 = PlanAgentLocationAccess::getInstance().getLocationName(location);
				//CString locationName = temp + temp2.c_str() + temp3.c_str();
				//userMsg << locationName;
				//UINT selectedButton = userMsg.showMsgBox(IDS_UE_210087);
				// TD14164 ++azenitha
				//++TD16971
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");

                std::ostringstream errorMessage;
                errorMessage << "Unable to retrieve active plans data from "
                             << PlanAgentLocationAccess::getInstance().getLocationName(location) << "."; 

				//TD16971 - remove message box when the plan agent cannot be contacted 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());
				// TD14164 azenitha++
                /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
				//TA_Base_Bus::TransActiveMessage userMsg;
				//std::string locationName = PlanAgentLocationAccess::getInstance().getLocationName(location);
				//userMsg << locationName.c_str();
				//UINT selectedButton = userMsg.showMsgBox(IDS_UE_210087);
				// TD14164 ++azenitha
				//++TD16971
            }
        }        
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Unable to populate the active plan list."
                     << std::endl << std::endl << ex.what();

		// TD14164 azenitha++
        /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString temp = ex.what();
		CString error = "\n\n" + temp;
		userMsg << error;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_210122);
		// TD14164 ++azenitha
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");

		// TD14164 azenitha++
        /*AfxMessageBox(_T("Unable to populate the active plan list."), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString error = "";
		userMsg << error;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_210122);
		// TD14164 ++azenitha
    }
}


void CPlanActiveInstanceListCtrl::Purge()
{
    if (m_planInstanceFilter == NULL)
    {
        return;
    }

    try
    {
        CWaitCursor waitCursor;

        // Delete all active plan instances that do not satisfy the current filtering rules.
        for (int position = GetItemCount() - 1; position >= 0; position--)
        {
            TA_Base_Core::ActivePlanId activePlanId = GetPlanInstanceId(position);

            if (!m_planInstanceFilter->isRelevant(activePlanId))
            {
                DeleteInstance(position);
            }
        }
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Unable to purge the active plan list."
                     << std::endl << std::endl << ex.what();

		// TD14164 azenitha++
        /*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString temp = ex.what();
		CString actionName = "purge the active plan list. \n\n" + temp;
		userMsg << actionName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_632006);
		// TD14164 ++azenitha
    }
    catch (const IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "IPlanAgentCorbaDef::PlanAgentError", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Unable to purge the active plan list."
                     << std::endl << std::endl << ex.what.in();

		// TD14164 azenitha++
		/*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString temp = ex.what.in();
		CString actionName = "purge the active plan list. \n\n" + temp;
		userMsg << actionName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_632006);
		// TD14164 ++azenitha
    }
    catch (const CORBA::Exception &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        std::ostringstream errorMessage;
        errorMessage << "Unable to purge the active plan list."
                     << std::endl << std::endl << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

		// TD14164 azenitha++
		/*AfxMessageBox(_T(errorMessage.str().c_str()), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString temp = "purge the active plan list. \n\n";
		std::string temp2 = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
		CString actionName = temp + temp2.c_str();
		userMsg << actionName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_632006);
		// TD14164 ++azenitha
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");

		// TD14164 azenitha++
		/*AfxMessageBox(_T("Unable to purge the active plan list."), MB_ICONERROR | MB_OK);*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "purge the active plan list";
		userMsg << actionName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_632006);
		// TD14164 ++azenitha
    }
}


TA_Base_Core::ActivePlanId CPlanActiveInstanceListCtrl::GetSelectedPlanInstanceId()
{
    POSITION selectedItemPos = GetFirstSelectedItemPosition();

    if (selectedItemPos)
    {
        int selectedItem = GetNextSelectedItem(selectedItemPos);

        return GetPlanInstanceId(selectedItem);
    }

    TA_Base_Core::ActivePlanId activePlanId;
    activePlanId.plan = (TreeNodeId) -1;
    activePlanId.instance = (PlanInstanceNumber) -1;
    activePlanId.location = (LocationKey) -1;

    return activePlanId;
}


TA_Base_Core::ActivePlanId CPlanActiveInstanceListCtrl::GetPlanInstanceId(int item)
{
    TA_Base_Core::ActivePlanDetail *itemData = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(GetItemData(item));
    TA_ASSERT(itemData, "Item data pointer is NULL");

    return itemData->plan;
}


void CPlanActiveInstanceListCtrl::InitialiseActivePlanList()
{
	if (!m_initialised) // want to do this once only
	{
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

		//
		// insert the columns
		//
		// Plan Column
		InsertColumn(PLAN_COLUMN_INDEX, _T(PLAN_COLUMN_LABEL.c_str()), LVCFMT_LEFT, m_planPathVisible? 180 : 0);
		// Category Column
		InsertColumn(CATEGORY_COLUMN_INDEX, _T(CATEGORY_COLUMN_LABEL.c_str()), LVCFMT_LEFT, m_planPathVisible? 180 : 0);
		// Instance Column
		InsertColumn(INSTANCE_COLUMN_INDEX, _T(INSTANCE_COLUMN_LABEL.c_str()), LVCFMT_RIGHT, 72);
		// Location Column
		InsertColumn(LOCATION_COLUMN_INDEX, _T(LOCATION_COLUMN_LABEL.c_str()), LVCFMT_LEFT, 75);
		// Owner Column 
		InsertColumn(OWNER_COLUMN_INDEX, _T(OWNER_COLUMN_LABEL.c_str()), LVCFMT_LEFT, 150);
		// Activation Column 
		InsertColumn(ACTIVATION_COLUMN_INDEX, _T(ACTIVATION_COLUMN_LABEL.c_str()), LVCFMT_LEFT, 145);
		// State Column
		InsertColumn(STATE_COLUMN_INDEX, _T(STATE_COLUMN_LABEL.c_str()), LVCFMT_LEFT, 85);
		// Step Column
		InsertColumn(STEP_COLUMN_INDEX, _T(STEP_COLUMN_LABEL.c_str()), LVCFMT_LEFT,	m_planPathVisible? 185 : 200);
		// Remarks Column
		// CL-14921++ Item 5
		//InsertColumn(REMARKS_COLUMN_INDEX, _T(REMARKS_COLUMN_LABEL), LVCFMT_LEFT, m_planPathVisible? 170 : 185); 
		InsertColumn(REMARKS_COLUMN_INDEX, _T(REMARKS_COLUMN_LABEL.c_str()), LVCFMT_LEFT, m_planPathVisible? 435 : 450); 
		// ++CL-14921
 

		m_initialised = true;
	}
}


bool CPlanActiveInstanceListCtrl::IsInitialised() const
{
    return m_initialised;
}


void CPlanActiveInstanceListCtrl::SortActivePlanList()
{
	// do nothing if not yet initialised
	if (m_initialised)
	{
		SortItems(ComparePlanInstances, 0);
	}	
}


void CPlanActiveInstanceListCtrl::DeleteInstance(int item)
{
    bool isSelected = (GetItemState(item, LVIS_SELECTED) == LVIS_SELECTED);

    DeleteItem(item);

    if (isSelected)
    {
        SetItemState(__min(item, GetItemCount() - 1), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }
}


int CPlanActiveInstanceListCtrl::FindInstance(const TA_Base_Core::ActivePlanId &activePlanId)
{
    int itemCount = GetItemCount();

    for (int position = 0; position < itemCount; position++)
    {
        TA_Base_Core::ActivePlanId id = GetPlanInstanceId(position);
    
        if (id.plan == activePlanId.plan &&  
            id.location == activePlanId.location &&
            id.instance == activePlanId.instance)
        {
            return position;
        }
    }
          
	return -1;
}


int CALLBACK CPlanActiveInstanceListCtrl::ComparePlanInstances(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    TA_Base_Core::ActivePlanDetail* planInstance1 = reinterpret_cast<TA_Base_Core::ActivePlanDetail*>(lParam1);
    TA_ASSERT(planInstance1, "Plan instance 1 is NULL");

    TA_Base_Core::ActivePlanDetail* planInstance2 = reinterpret_cast<TA_Base_Core::ActivePlanDetail*>(lParam2);
    TA_ASSERT(planInstance2, "Plan instance 2 is NULL");

    ComparablePlanInstance comparablePlanInstance1(*planInstance1);
    ComparablePlanInstance comparablePlanInstance2(*planInstance2);

    if (comparablePlanInstance1 < comparablePlanInstance2)
    {
        return -1;
    }

    if (comparablePlanInstance1 > comparablePlanInstance2)
    {
        return 1;
    }

	return 0;
}

}
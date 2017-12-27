/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/SubsystemDutyPropertyPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Subsystems" tab. It displays the active 
  * subsystem duty state for every physical subsystem in every region.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD15569

// Use boost tokenizer for splitting up RGB values.
#include "boost/tokenizer.hpp"

#include "core/data_access_interface/src/RightsAccessFactory.h" //TD15533
#include "core/data_access_interface/src/SessionAccessFactory.h" //TD15533
#include "core/data_access_interface/src/ProfileAccessFactory.h" //TD15533

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const unsigned int CHECK_SESSION_IN_DB_RETRY_TIMES = 3;
	const CString SUBSYSTEM_COLUMN_HEADING("子系统");
    const int FIXED_COLUMN_COUNT(1);
    const int FIXED_ROW_COUNT(1);

   	const int SUBSYSTEM_ROW(0);
	const int REGION_COLUMN(0);

    const int DEFAULT_ROW_HEIGHT(20);
    const int DEFAULT_COLUMN_WIDTH(70);

    const int ALLOCATED_SUBSYSTEM(1);
    const int UNALLOCATED_SUBSYSTEM(~ALLOCATED_SUBSYSTEM);

    const COLORREF NORMAL_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF DELEGATED_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF DEGRADED_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF UNALLOCATED_SUBSYSTEM_COLOUR( RGB( 225, 225, 225 ) ); // light grey
	
	const unsigned long CENTRAL_PROFILE_TYPE(1); //1 is central profile //TD15533
	const unsigned long NO_ACTION_GROUP(0xFFFFFFFF); //TD15533
	const TA_Base_Bus::LocationKey ALL_LOCATION(0); //0 is all location if central profile //TD15533
}

namespace TA_Base_App
{
	// ++TD19076
	const int OCC_LOCATION = 1;
	const char* TCS_TRAIN_COMMS_STN_SUBSYSTEM = "TCS - Train Comms (Stn)";
	const char* PROFILE_GROUP_TCO = "TCO";
	const char* TCS_SPECIAL_PROJECT_NAME = "4669"; //
	// TD19076++
}

using TA_Base_Bus::SessionCache;
using TA_Base_Bus::SubsystemDutyMatrixElement;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::SubsystemKey;
using TA_Base_Bus::SubsystemKeyList;
using TA_Base_Bus::SubsystemDutyUpdate;
using TA_Base_Bus::SubsystemDutyList;
using TA_Base_Bus::SubsystemStateList;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// SubsystemDutyPropertyPage property page

IMPLEMENT_DYNCREATE(SubsystemDutyPropertyPage, CPropertyPage)

SubsystemDutyPropertyPage::SubsystemDutyPropertyPage() : 
    CPropertyPage(SubsystemDutyPropertyPage::IDD),
    ISessionObserver(DutyDataStore::getSessionCache()),
    ISubsystemDutyObserver(DutyDataStore::getSubsystemDutyMatrix()),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache()),
    m_subsystemDutyMatrix(DutyDataStore::getSubsystemDutyMatrix()),
    m_currentSessionInfo(m_sessionCache.getSessionInfo(m_sessionCache.getCurrentSessionId()))
{ 
    FUNCTION_ENTRY("SubsystemDutyPropertyPage()");

	m_actionGroups.clear(); // yanrong
	//{{AFX_DATA_INIT(SubsystemDutyPropertyPage)
	//}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


SubsystemDutyPropertyPage::~SubsystemDutyPropertyPage()
{
    FUNCTION_ENTRY("~SubsystemDutyPropertyPage()");

	m_actionGroupsDB.clear(); //TD15533

	//DutyDataStore::cleanUp();
    FUNCTION_EXIT;
}


void SubsystemDutyPropertyPage::initialiseSubsystemDutyGrid()
{
    // Initialise the colours to used when rendering the grid.
    m_normalDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_NORMALDUTYCOLOUR ),
                                                     NORMAL_DUTY_COLOUR );
    m_delegatedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DELEGATEDDUTYCOLOUR ),
                                                     DELEGATED_DUTY_COLOUR );
    m_degradedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DEGRADEDDUTYCOLOUR ),
                                                     DEGRADED_DUTY_COLOUR );
    m_unallocatedSubsystemColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR ),
                                                     UNALLOCATED_SUBSYSTEM_COLOUR );

    // By default only those subsystems are shown which the session currently has access to.
    SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));
    RegionKeyList regionKeys(m_subsystemDutyMatrix.getColumnIds());

    m_subsystemDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_subsystemDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_subsystemDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_subsystemDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_subsystemDutyGrid.SetListMode(TRUE);
    m_subsystemDutyGrid.EnableTitleTips(TRUE);
    m_subsystemDutyGrid.EnableSelection(FALSE);
    m_subsystemDutyGrid.EnableDragAndDrop(FALSE);
    m_subsystemDutyGrid.SetEditable(FALSE);
    m_subsystemDutyGrid.SetHeaderSort(FALSE);

    m_subsystemDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_subsystemDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for subsystem names

    //m_subsystemDutyGrid.SetRowCount(m_subsystemDutyGrid.GetFixedRowCount());  // this will grow as subsystems are added to the grid
    //m_subsystemDutyGrid.SetColumnCount(m_subsystemDutyGrid.GetFixedColumnCount() + regionKeys.size());

	m_subsystemDutyGrid.SetRowCount(m_subsystemDutyGrid.GetFixedColumnCount() + regionKeys.size());  
    m_subsystemDutyGrid.SetColumnCount(m_subsystemDutyGrid.GetFixedRowCount()); // this will grow as subsystems are added to the grid
	
    m_subsystemDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
    // Set the fixed column labels.
    m_subsystemDutyGrid.SetItemText(SUBSYSTEM_ROW, REGION_COLUMN, SUBSYSTEM_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int row(REGION_COLUMN + 1);
    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = row;
            gridItem.col = REGION_COLUMN;
            gridItem.szText = m_dataCache.getRegionDisplayName(*regionIt).c_str();
            gridItem.lParam = *regionIt;
			if(0 == gridItem.szText.CompareNoCase(_T("全部车站")))
			{
				m_subsystemDutyGrid.DeleteRow(row);
				continue;
			}
            m_subsystemDutyGrid.SetItem(&gridItem);

            row++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one row
            m_subsystemDutyGrid.DeleteRow(m_subsystemDutyGrid.GetRowCount() - 1);

            //MessageBox::error("Error while adding region to subsystem duty grid.", ex);
			MessageBox::error("",ex,IDS_UW_670014);
        }
    }

	//TD15533++
	TA_Base_Bus::RegionKeyList regionKeysAccessible; 
	unsigned int retryTime = 0;
	while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		try
		{
			++retryTime;
			regionKeysAccessible = getLocations(); 
			break;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
		}
	}
	
	if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "initialiseSubsystemDutyGrid(): After retry %d times, Still no Session persisted in DB",
			CHECK_SESSION_IN_DB_RETRY_TIMES);
		
		TA_Base_Bus::TransActiveMessage userMsg;
		int resp = userMsg.showMsgBox(IDS_UW_670015, "再次尝试失败");
		if (resp == IDOK)
		{
			throw;
		}
	}
	//++TD15533

    // Add individual subsystems to the grid. Subsystem names are used as entries in the column headers	
    for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
    {
        addSubsystem(*subsystemIt, m_currentSessionInfo.ProfileId, regionKeysAccessible); //TD15533
    }

    // Sort subsystems by name.
	m_subsystemDutyGrid.SortTextItems(REGION_COLUMN, TRUE);

    // Size subsystem column width to fit subsystem names.
    //m_subsystemDutyGrid.AutoSizeColumn(REGION_COLUMN);
	m_subsystemDutyGrid.AutoSizeColumns();
}


void SubsystemDutyPropertyPage::enableDutyControlButtons()
{
    bool canTransferSubsystems(m_dutyRights.canTransferSubsystems(aca_DUTY_TRANSFER_SUBSYSTEM));
	bool canTransferAllSubsystems(m_dutyRights.canTransferSubsystems(aca_DUTY_TRANSFER_ALL_SUBSYSTEM));
	//bool canDegradeSubsystem  = m_dutyRights.canDegradeSubsystems();
	

    //m_transferSubsystemDutyBtn.EnableWindow(canTransferSubsystems);
    //m_transferAllSubsystemDutyBtn.EnableWindow(canTransferAllSubsystems);

	m_revokeSubsystemDutyBtn.EnableWindow(canTransferSubsystems);
	m_transferSubsystemDutyBtn.EnableWindow(canTransferSubsystems);

	m_showAllSubsystemsBtn.ShowWindow(SW_HIDE);
}


void SubsystemDutyPropertyPage::addSubsystem(
	TA_Base_Bus::SubsystemKey subsystemKey, 
	TA_Base_Bus::ProfileKeyList profileKeys,
	TA_Base_Bus::RegionKeyList regionKeys) //TD15533
{
    // Append the subsystem to the end of the grid
	
    int col(m_subsystemDutyGrid.GetColumnCount() + 1);
	m_subsystemDutyGrid.SetColumnCount(col);

    //m_subsystemDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    //m_subsystemDutyGrid.SetRowCount(row + 1); // unlike InsertRow(), this will use the default row height
    
    try
    {
        // Set subsystem details.
        GV_ITEM gridItem;
        gridItem.mask = GVIF_TEXT | GVIF_PARAM;
        gridItem.row = 0; //first row
        gridItem.col = col -1;
		
		std::string subsystemDisplayName = m_dataCache.getSubsystemDisplayName(subsystemKey);
		if(subsystemDisplayName.empty() || subsystemDisplayName.length() == 0)
		{
			gridItem.szText = m_dataCache.getSubsystemName(subsystemKey).c_str();
		}		
		else
		{
			gridItem.szText = subsystemDisplayName.c_str();			
		}
		
        gridItem.lParam = subsystemKey;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "subsyste name: %s" , gridItem.szText);
        m_subsystemDutyGrid.SetItem(&gridItem);

        // Where a subsystem is not part of a region, grey out the corresponding grid cell.
        for (gridItem.row = SUBSYSTEM_ROW + 1; gridItem.row < m_subsystemDutyGrid.GetRowCount(); gridItem.row++)
        {
            //RegionKey regionKey(m_subsystemDutyGrid.GetItemData(REGION_ROW, gridItem.col));
			RegionKey regionKey(m_subsystemDutyGrid.GetItemData(gridItem.row, REGION_COLUMN));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, " region key: %d " , regionKey);

			//TD15533++ 

			const SubsystemDutyMatrixElement& sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);

            gridItem.mask = GVIF_PARAM;
            gridItem.lParam = (sdmElement.isSubsystemInRegion()? ALLOCATED_SUBSYSTEM : UNALLOCATED_SUBSYSTEM);

			
			if (!isRegionAccessible(regionKey, regionKeys) ||  //requirement #4
				!isSubsystemAccessible(profileKeys, subsystemKey) || //requirement #2
				!sdmElement.isSubsystemInRegion()) 
			{	
				//force to gray cell if not accessible or not in region
				gridItem.mask |= GVIF_BKCLR;
				gridItem.crBkClr = determineCellBkColour( false, sdmElement.getSubsystemState() );

				gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
				gridItem.iImage = -1;
				gridItem.szText = "";
				gridItem.iImageS = -1;
				gridItem.szTextS = "";
				gridItem.bOverrided = false;

				m_subsystemDutyGrid.SetItem(&gridItem);
				m_subsystemDutyGrid.SetItemS(&gridItem); 

				continue;
			}

            try
            {
				//TD15533++
				//std::string actionGrp(GridAssistant::generateActionGroupString(m_currentSessionInfo, subsystemKey, regionKey));
				//int image(actionGrp.empty()? GridAssistant::GI_CROSS : GridAssistant::GI_TICK);

				//gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
				//gridItem.iImage = image;
				//gridItem.szText = actionGrp.c_str();
				//++TD15533

				TA_Base_App::CellData cellData = generateCellData(subsystemKey, regionKey, sdmElement.getSubsystemState());

				if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					gridItem.mask |= GVIF_BKCLR;
					gridItem.crBkClr = determineCellBkColour( false, sdmElement.getSubsystemState() );

					gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
					gridItem.iImage = -1;
					gridItem.szText = "";
					gridItem.iImageS = -1;
					gridItem.szTextS = "";
					gridItem.bOverrided = false;
				}
				else
				{
					gridItem.mask |= GVIF_BKCLR;
					gridItem.crBkClr = determineCellBkColour( sdmElement.isSubsystemInRegion(), sdmElement.getSubsystemState() );

					gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
					gridItem.iImage = cellData.baseImage;
					gridItem.szText = cellData.baseText.c_str();
					gridItem.iImageS = cellData.overrideImage;
					gridItem.szTextS = cellData.overrideText.c_str();
					gridItem.bOverrided = cellData.isOverrided;
				}

                m_subsystemDutyGrid.SetItem(&gridItem);
				m_subsystemDutyGrid.SetItemS(&gridItem); 
            }
            catch (const TA_Base_Core::MatrixException& ex)
            {
				TA_Base_Bus::TransActiveMessage userMsg;
				const std::string subsystemName = m_dataCache.getSubsystemName(subsystemKey);
				const std::string regionName = m_dataCache.getRegionName(regionKey);
				userMsg<<subsystemName;
				userMsg<<regionName;
				userMsg<<ex.what();
				userMsg.showMsgBox(IDS_UW_670016, "权限管理器");
            }

			//TD15533
        }
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        // Get rid of the newly added row.
        m_subsystemDutyGrid.DeleteColumn(col);

        //MessageBox::error("Error while adding subsystem to subsystem duty grid.", ex);
		MessageBox::error("", ex, IDS_UW_670017);
    }
}


void SubsystemDutyPropertyPage::removeSubsystem(TA_Base_Bus::SubsystemKey subsystemKey)
{
    int col(GridAssistant::findGridColumn(m_subsystemDutyGrid, subsystemKey));

    if (col >= 0)
    {
        m_subsystemDutyGrid.DeleteColumn(col);
        m_subsystemDutyGrid.Invalidate();
    }
}


void SubsystemDutyPropertyPage::updateSubsystems(TA_Base_Bus::SubsystemKeyList oldSubsystemKeys, TA_Base_Bus::SubsystemKeyList newSubsystemKeys, TA_Base_Bus::ProfileKeyList profileKeys) //TD15533
{
    SubsystemKeyList affectedSubsystemKeys;

    std::sort(oldSubsystemKeys.begin(), oldSubsystemKeys.end());
    std::sort(newSubsystemKeys.begin(), newSubsystemKeys.end());

    if (oldSubsystemKeys.size() > newSubsystemKeys.size())
    {
        // The new subsystem list is shorter than the old one - remove from view all subsystems  
        // in the old list not found in the new list.
        std::set_difference(oldSubsystemKeys.begin(), oldSubsystemKeys.end(),
                            newSubsystemKeys.begin(), newSubsystemKeys.end(),
                            std::inserter(affectedSubsystemKeys, affectedSubsystemKeys.begin()));

        for (SubsystemKeyList::const_iterator subKeyIt = affectedSubsystemKeys.begin(); subKeyIt != affectedSubsystemKeys.end(); subKeyIt++)
        {
            removeSubsystem(*subKeyIt);
        }
    }
    else
    {
        // The new subsystem list is longer than the old one - add to view all subsystems  
        // in the new list not found in the old list.
        std::set_difference(newSubsystemKeys.begin(), newSubsystemKeys.end(),
                            oldSubsystemKeys.begin(), oldSubsystemKeys.end(),
                            std::inserter(affectedSubsystemKeys, affectedSubsystemKeys.begin()));

		//TD15533++
		TA_Base_Bus::RegionKeyList regionKeysAccessible; 
		unsigned int retryTime = 0;
		while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
		{
			try
			{
				++retryTime;
				regionKeysAccessible = getLocations(); 
				break;
			}
			catch (...)
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
			}
		}
		//++TD15533
		if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "updateSubsystems(): After retry %d times, Still no Session persisted in DB",
				CHECK_SESSION_IN_DB_RETRY_TIMES);

			TA_Base_Bus::TransActiveMessage userMsg;
			int resp = userMsg.showMsgBox(IDS_UW_670018, "再次尝试失败");
			if (resp == IDOK)
			{
				throw;
			}
		}

        for (SubsystemKeyList::const_iterator subKeyIt = affectedSubsystemKeys.begin(); subKeyIt != affectedSubsystemKeys.end(); subKeyIt++)
        {
            addSubsystem(*subKeyIt, profileKeys, regionKeysAccessible); //TD15533
        }
  
    }
}


// ISessionObserver interface
void SubsystemDutyPropertyPage::sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate)
{
    // Let the thread that created the window process the update.
    HWND wndHandle(GetSafeHwnd());

    if (wndHandle != NULL)
    {
        // Post the message so as not to hold up the notifying thread.
        // Clone the session update and throw onto the heap to make sure 
        // that by the time the message is processed we still have a valid
        // object. The message handler takes care of destroying it once consumed.
        ::PostMessage(wndHandle, WM_USER_RECEIVE_SESSION_UPDATE, 
                      reinterpret_cast<WPARAM>(new SessionUpdate(sessionUpdate)), 0);
    }
}


// ISubsystemDutyObserver interface
void SubsystemDutyPropertyPage::dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate)
{
    // Let the thread that created the window process the update.
    HWND wndHandle(GetSafeHwnd());

    if (wndHandle != NULL)
    {
        // Post the message so as not to hold up the notifying thread.
        // Clone the duty update and throw onto the heap to make sure 
        // that by the time the message is processed we still have a valid
        // object. The message handler takes care of destroying it once consumed.
        ::PostMessage(wndHandle, WM_USER_RECEIVE_SUB_DUTY_UPDATE, 
                      reinterpret_cast<WPARAM>(new SubsystemDutyUpdate(dutyUpdate)), 0);
    }
}


void SubsystemDutyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SubsystemDutyPropertyPage)
	DDX_Control(pDX, IDC_TRANSFER_ALL_SUB_DUTY_BTN, m_transferAllSubsystemDutyBtn);
	DDX_Control(pDX, IDC_SHOW_ALL_SUBS_CHECK, m_showAllSubsystemsBtn);
	DDX_Control(pDX, IDC_TRANSFER_SUB_DUTY_BTN, m_transferSubsystemDutyBtn);
	DDX_Control(pDX, IDC_REVOKE_SUB_DUTY_BTN, m_revokeSubsystemDutyBtn);	
	DDX_GridControl(pDX, IDC_SUB_DUTY_GRID, m_subsystemDutyGrid);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// SubsystemDutyPropertyPage message handlers

BEGIN_MESSAGE_MAP(SubsystemDutyPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(SubsystemDutyPropertyPage)
	ON_WM_SIZE()
    ON_MESSAGE(WM_USER_RECEIVE_SESSION_UPDATE, OnReceiveSessionUpdate)
    ON_MESSAGE(WM_USER_RECEIVE_SUB_DUTY_UPDATE, OnReceiveSubsystemDutyUpdate)
	ON_BN_CLICKED(IDC_SHOW_ALL_SUBS_CHECK, OnShowAllSubsystems)
	ON_BN_CLICKED(IDC_TRANSFER_SUB_DUTY_BTN, OnTransferSubsystemDutyBtn2)
	ON_BN_CLICKED(IDC_TRANSFER_ALL_SUB_DUTY_BTN, OnTransferAllSubsystemDutyBtn)
	ON_BN_CLICKED(IDC_REVOKE_SUB_DUTY_BTN, OnRevokeSubsystemDutyBtn)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL SubsystemDutyPropertyPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
    //TD17082++ - initialization of subsystem dutygrid will be called right after dutydatastore has been initialized
    //initialiseSubsystemDutyGrid();
    enableDutyControlButtons();
  	
    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT SubsystemDutyPropertyPage::OnReceiveSessionUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    // Make sure the session update gets destroyed when we leave here.
    std::auto_ptr<SessionUpdate> sessionUpdate(reinterpret_cast<SessionUpdate*>(wParam));

    TA_Base_Bus::SessionId currentSessionId(m_sessionCache.getCurrentSessionId());

    // If current session undergoes a profile change or a profile override, operator access rights 
    // may be affected so the visible subsystems and the enable state of the duty transfer buttons
    // need to be reevaluated. 

    if (sessionUpdate->updateType == TA_Base_Bus::TA_Security::SessionEnd ||
        sessionUpdate->sessionId != currentSessionId)
    {
        return 0;
    }

    TA_Base_Bus::ProfileKeyList previousProfileId(m_currentSessionInfo.ProfileId);
    m_currentSessionInfo = m_sessionCache.getSessionInfo(currentSessionId);    

    // Get the previously accessible subsystems and the currently accessible subsystems as determined
    // by previous and current profile(s).
    SubsystemKeyList previousSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(previousProfileId));
    SubsystemKeyList currentSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));

	//TD15533++
	//change implementation
//    switch (sessionUpdate->updateType)
//    {
//        case TA_Base_Bus::TA_Security::SessionStart: // profile change (new session ID)
//        {
//            SubsystemKeyList empty;
//
//            if (m_showAllSubsystemsBtn.GetCheck() == BST_UNCHECKED)
//            {
//                // Subsystems are filtered by rights - remove all previously accessible 
//                // subsystems and add the currently accessible ones.
//                updateSubsystems(previousSubsystemKeys, empty);
//                updateSubsystems(empty, currentSubsystemKeys);
//            }
//            else
//            {
//                // All subsystems are visible - we need to refresh duty state display as duty updates for 
//                // the new session were likely to have come before this session start event (in which case 
//                // they would have been ignored as not related to the current session).
//                // Remove both the previously and currently accessible subsystems and re-add them to the 
//                // grid. This will ensure that the correct duties are shown for all affected subsystems.
//                SubsystemKeyList combinedSubsystemKeys(previousSubsystemKeys);
//                combinedSubsystemKeys.insert(combinedSubsystemKeys.end(), currentSubsystemKeys.begin(), currentSubsystemKeys.end());
// 
//                std::sort(combinedSubsystemKeys.begin(), combinedSubsystemKeys.end());
//                SubsystemKeyList::iterator uniqueEndIt(std::unique(combinedSubsystemKeys.begin(), combinedSubsystemKeys.end()));
//                combinedSubsystemKeys.erase(uniqueEndIt, combinedSubsystemKeys.end());
//
//                updateSubsystems(combinedSubsystemKeys, empty);
//                updateSubsystems(empty, combinedSubsystemKeys);
//            }
//
//            break;
//        }
//
//        case TA_Base_Bus::TA_Security::ProfileUpdate: // profile override (same session ID)
//            if (m_showAllSubsystemsBtn.GetCheck() == BST_UNCHECKED)
//            {
//                // For an override begin, previousSubsystemKeys will be a subset of currentSubsystemKeys. 
//                // For an override end, currentSubsystemKeys will be a subset of previousSubsystemKeys. 
//                // updateSubsystems() will add or remove subsystems from the grid as appropriate.
//                updateSubsystems(previousSubsystemKeys, currentSubsystemKeys);
//            }
//
//            break;
//        
//        default:
//            return 0;
//    }
	//++TD15533

	//TD15533++
	//implementation changed
	SubsystemKeyList empty;
	SubsystemKeyList allSubsystemKeys(m_subsystemDutyMatrix.getRowIds());

	if (m_showAllSubsystemsBtn.GetCheck() == BST_UNCHECKED)
	{
		updateSubsystems(previousSubsystemKeys, empty, previousProfileId);
		updateSubsystems(empty, currentSubsystemKeys, m_currentSessionInfo.ProfileId);
	}
	else
	{
		updateSubsystems(allSubsystemKeys, empty, m_currentSessionInfo.ProfileId);
        updateSubsystems(empty, allSubsystemKeys, m_currentSessionInfo.ProfileId);
    }
	//++TD15533

    enableDutyControlButtons();

    return 0;
}


LRESULT SubsystemDutyPropertyPage::OnReceiveSubsystemDutyUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
	USES_CONVERSION;

    // Make sure the duty update gets destroyed when we leave here.
    std::auto_ptr<SubsystemDutyUpdate> dutyUpdate(reinterpret_cast<SubsystemDutyUpdate*>(wParam));

    if ( ( TA_Base_Bus::DUT_ADD == dutyUpdate->updateType ) || ( TA_Base_Bus::DUT_REMOVE == dutyUpdate->updateType ) )
    {
        for (SubsystemDutyList::const_iterator subDutyIt = dutyUpdate->duties.begin(); subDutyIt != dutyUpdate->duties.end(); subDutyIt++)
        {
            try
            {
                // Ignore subsystem duty updates for sessions other than the current one.
                if (m_currentSessionInfo.SessionId != subDutyIt->session.in())
                {
                    continue;
                }

                // Get the row index corresponding to the subsystem affected by duty change.
                // Ignore the update if no such row is found.
                int row(GridAssistant::findGridRow(m_subsystemDutyGrid, subDutyIt->regionKey));

                if (row < 0)
                {
                    continue;
                }
        
                // Get the column index corresponding to the region affected by duty change.
                // Ignore the update if no such column is found.
                int column(GridAssistant::findGridColumn(m_subsystemDutyGrid, subDutyIt->subsystemKey));  
        
                if (column < 0)
                {
                    continue;
                }

				//TD15533++

                // Update the relevant grid cell.
                //std::string actionGrp(GridAssistant::generateActionGroupString(m_currentSessionInfo, subDutyIt->subsystemKey, subDutyIt->regionKey));
                //int image(actionGrp.empty()? GridAssistant::GI_CROSS : GridAssistant::GI_TICK);

				TA_Base_Bus::RegionKeyList regionKeysAccessible; 
				unsigned int retryTime = 0;
				while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					try
					{
						++retryTime;
						regionKeysAccessible = getLocations(); 
						break;
					}
					catch (...)
					{
						LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
					}
				}
				if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnReceiveSubsystemDutyUpdate(): After retry %d times, Still no Session persisted in DB",
						CHECK_SESSION_IN_DB_RETRY_TIMES);
		
					TA_Base_Bus::TransActiveMessage userMsg;
					int resp = userMsg.showMsgBox(IDS_UW_670019, "再次尝试失败");
					if (resp == IDOK)
					{
						throw;
					}
				}
				
				TA_Base_Bus::ProfileKeyList profileKeys = getProfileKeyList(subDutyIt->subsystemKey, subDutyIt->regionKey);

				SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subDutyIt->subsystemKey, subDutyIt->regionKey);
				COLORREF bkColour = determineCellBkColour( true, sdmElement.getSubsystemState() );

				//TD19148---- LiDan 25,Jun,2007
				//2) Duty Manager of Local MFT Subsystem tab populate action group for other locations.
				if (!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible) ||  
					(!isSubsystemAccessible(profileKeys, subDutyIt->subsystemKey) &&
					!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible))|| 
					!sdmElement.isSubsystemInRegion()) 
					continue;
				//----TD19148
				TA_Base_App::CellData cellData = generateCellData(subDutyIt->subsystemKey, subDutyIt->regionKey, sdmElement.getSubsystemState());

				CA2T baseText(cellData.baseText.c_str());
				CA2T overrideText(cellData.overrideText.c_str());

				m_subsystemDutyGrid.SetItemImage(row, column, cellData.baseImage);
				m_subsystemDutyGrid.SetItemText(row, column, baseText);
                m_subsystemDutyGrid.SetItemImageS(row, column, cellData.overrideImage);
				m_subsystemDutyGrid.SetItemTextS(row, column, overrideText); 
				m_subsystemDutyGrid.SetItemOverrided(row, column, cellData.isOverrided); 
				
				/*if (!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible) ||  //requirement #4
					!isSubsystemAccessible(profileKeys, subDutyIt->subsystemKey) || //requirement #2
					!sdmElement.isSubsystemInRegion()) 
				{
					// force to gray if not accessible
					bkColour = determineCellBkColour( false, sdmElement.getSubsystemState() );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, "");
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, ""); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}
				else*/ if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					bkColour = determineCellBkColour( false, sdmElement.getSubsystemState() );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, _T(""));
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, _T("")); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}

				m_subsystemDutyGrid.SetItemBkColour(row, column, bkColour);
                m_subsystemDutyGrid.RedrawCell(row, column);

				//++TD15533
            }
            catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
            }
            catch (const TA_Base_Core::TransactiveException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch (...)
            {
                //MessageBox::error("Error while processing subsystem duty update.");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
            }        
        }
    }
    else if ( TA_Base_Bus::DUT_STATE == dutyUpdate->updateType )
    {
        for (SubsystemStateList::const_iterator subStateIt = dutyUpdate->states.begin(); subStateIt != dutyUpdate->states.end(); subStateIt++)
        {
            try
            {
                // Get the row index corresponding to the subsystem affected by duty change.
                // Ignore the update if no such row is found.
                int row(GridAssistant::findGridRow(m_subsystemDutyGrid, subStateIt->regionKey));

                if (row < 0)
                {
                    continue;
                }
        
                // Get the column index corresponding to the region affected by duty change.
                // Ignore the update if no such column is found.
                int column(GridAssistant::findGridColumn(m_subsystemDutyGrid, subStateIt->subsystemKey));  
        
                if (column < 0)
                {
                    continue;
                }

                // Update the relevant grid cell.

				//TD15533++
				//almost the same now with DUT_ADD and DUT_REMOVE but use the current state from the update

				TA_Base_Bus::RegionKeyList regionKeysAccessible; 
				unsigned int retryTime = 0;
				while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					try
					{
						++retryTime;
						regionKeysAccessible = getLocations(); 
						break;
					}
					catch (...)
					{
						LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
					}
				}
				if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnReceiveSubsystemDutyUpdate(): After retry %d times, Still no Session persisted in DB",
						CHECK_SESSION_IN_DB_RETRY_TIMES);

					TA_Base_Bus::TransActiveMessage userMsg;
					int resp = userMsg.showMsgBox(IDS_UW_670019, "再次尝试失败");
					if (resp == IDOK)
					{
						throw;
					}
				}
				

				TA_Base_Bus::ProfileKeyList profileKeys = getProfileKeyList(subStateIt->subsystemKey, subStateIt->regionKey);

				SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subStateIt->subsystemKey, subStateIt->regionKey);
				COLORREF bkColour = determineCellBkColour( true, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );
				//TD19148---- LiDan 25,Jun,2007
				//2) Duty Manager of Local MFT Subsystem tab populate action group for other locations.

				if (!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible) ||  
					(!isSubsystemAccessible(profileKeys, subStateIt->subsystemKey) && 
					!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible)) || 
					!sdmElement.isSubsystemInRegion()) 
					continue;
				//----TD19148

				TA_Base_App::CellData cellData = generateCellData(subStateIt->subsystemKey, subStateIt->regionKey, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState));

				CA2T baseText(cellData.baseText.c_str());
				CA2T overrideText(cellData.overrideText.c_str());

				m_subsystemDutyGrid.SetItemImage(row, column, cellData.baseImage);
				m_subsystemDutyGrid.SetItemText(row, column, baseText);
                m_subsystemDutyGrid.SetItemImageS(row, column, cellData.overrideImage);
				m_subsystemDutyGrid.SetItemTextS(row, column, overrideText); 
				m_subsystemDutyGrid.SetItemOverrided(row, column, cellData.isOverrided); 
				
				/*if (!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible) ||  //requirement #4
					!isSubsystemAccessible(profileKeys, subStateIt->subsystemKey) || //requirement #2
					!sdmElement.isSubsystemInRegion()) 
				{
					// force to gray if not accessible
					bkColour = determineCellBkColour( false, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, "");
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, ""); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}
				else*/ if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					bkColour = determineCellBkColour( false, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, _T(""));
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, _T("")); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}

				m_subsystemDutyGrid.SetItemBkColour(row, column, bkColour);
                m_subsystemDutyGrid.RedrawCell(row, column);

				//++TD15533
            }
            catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
            }
            catch (const TA_Base_Core::TransactiveException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch (...)
            {
                //MessageBox::error("Error while processing subsystem duty update.");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
            }
        }
    }

    return 0;
}


void SubsystemDutyPropertyPage::OnShowAllSubsystems() 
{
    CWaitCursor waitCursor;

    SubsystemKeyList allSubsystemKeys(m_subsystemDutyMatrix.getRowIds());
    SubsystemKeyList accessibleSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));

    // When the Show All Subsystems button is released we go from the "all subsystems"
    // view to the "subsystems based on access rights" view, and vice versa.
    if (m_showAllSubsystemsBtn.GetCheck() == BST_UNCHECKED)
    {
        updateSubsystems(allSubsystemKeys, accessibleSubsystemKeys, m_currentSessionInfo.ProfileId); //TD15533
    }
    else
    {
        updateSubsystems(accessibleSubsystemKeys, allSubsystemKeys, m_currentSessionInfo.ProfileId); //TD15533
    }
}

void SubsystemDutyPropertyPage::OnTransferSubsystemDutyBtn2() 
{
    try
    {
        // Activate region duty transfer dialog to enable operator to take
        // duties for selected regions + subsystems.
		SubsystemDutyDlg dlg(this,false);
        dlg.DoModal();
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        //MessageBox::error("Error while activating duty transfer dialog.", ex);
		MessageBox::error("", ex, IDS_UW_670011);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		MessageBox::error("", ex, IDS_UW_670011);
    }
    catch (...)
    {
		MessageBox::error("",IDS_UW_670011);
    }	
}

void SubsystemDutyPropertyPage::OnTransferSubsystemDutyBtn() 
{
    try
    {
        // Activate region duty transfer dialog to enable operator to take
        // duties for selected regions + subsystems.
	    SubsystemDutyTransferDlg subDutyXferDlg;
        subDutyXferDlg.DoModal();
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        //MessageBox::error("Error while activating duty request dialog.", ex);
		MessageBox::error("", ex, IDS_UW_670020);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		MessageBox::error("", ex, IDS_UW_670020);
    }
    catch (...)
    {
		MessageBox::error("", IDS_UW_670020);
    }	
}


void SubsystemDutyPropertyPage::OnRevokeSubsystemDutyBtn() 
{
    try
    {
        // Activate region duty transfer dialog to enable operator to take
        // duties for selected regions + subsystems.
		SubsystemDutyDlg dlg(this,true);
        dlg.DoModal();
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        //MessageBox::error("Error while activating duty revoke dialog.", ex);
		MessageBox::error("", ex, IDS_UW_670021);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		MessageBox::error("", ex, IDS_UW_670021);
    }
    catch (...)
    {
		MessageBox::error("", IDS_UW_670021);
    }	
}


void SubsystemDutyPropertyPage::OnTransferAllSubsystemDutyBtn() 
{
	//TD15569++
	TA_Base_Bus::TransActiveMessage userMsg;
	//CString message = "request all subsystem duty";
	CString message = "请求子系统权限";
	userMsg << message;
	if (userMsg.showMsgBox(IDS_UW_010003,"权限管理器") == IDYES) 
	// Are you sure you want to {0}? (where {0} = "request all subsystem duty")
	{
		try
		{
			CWaitCursor waitCursor;

				m_dutyAgent.transferAllSubsystemDuty(m_currentSessionInfo);
		}
		catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
		{
			//MessageBox::error("Error while requesting duty for all subsystems.", ex);
			MessageBox::error("", ex, IDS_UW_670022);
		}	
	}
	//++TD15569
}


void SubsystemDutyPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_subsystemDutyGrid.GetSafeHwnd()))    
    {
        CRect xferAllSubDutyBtnRect;

        m_transferAllSubsystemDutyBtn.GetWindowRect(xferAllSubDutyBtnRect);
        ScreenToClient(xferAllSubDutyBtnRect);
        int buttonWidth(xferAllSubDutyBtnRect.Width());
        int buttonHeight(xferAllSubDutyBtnRect.Height()); 
        xferAllSubDutyBtnRect.right = cx - 8;
        xferAllSubDutyBtnRect.left = xferAllSubDutyBtnRect.right - buttonWidth;
        xferAllSubDutyBtnRect.bottom = cy - 8;
        xferAllSubDutyBtnRect.top = xferAllSubDutyBtnRect.bottom - buttonHeight;

        //m_transferAllSubsystemDutyBtn.MoveWindow(xferAllSubDutyBtnRect);
		m_transferAllSubsystemDutyBtn.ShowWindow(SW_HIDE);

		m_revokeSubsystemDutyBtn.MoveWindow(xferAllSubDutyBtnRect);		

        CRect xferSubDutyBtnRect; 

        m_transferSubsystemDutyBtn.GetWindowRect(xferSubDutyBtnRect);
        ScreenToClient(xferSubDutyBtnRect);
        xferSubDutyBtnRect.right = xferAllSubDutyBtnRect.left - 10;
        xferSubDutyBtnRect.left = xferSubDutyBtnRect.right - buttonWidth;
        xferSubDutyBtnRect.bottom = xferAllSubDutyBtnRect.bottom;
        xferSubDutyBtnRect.top = xferAllSubDutyBtnRect.top;
        m_transferSubsystemDutyBtn.MoveWindow(xferSubDutyBtnRect);
        m_transferSubsystemDutyBtn.Invalidate();

        CRect showAllSubsBtnRect;
 
        m_showAllSubsystemsBtn.GetWindowRect(showAllSubsBtnRect);
        ScreenToClient(showAllSubsBtnRect);
        showAllSubsBtnRect.top = xferSubDutyBtnRect.top;
        showAllSubsBtnRect.bottom = xferSubDutyBtnRect.bottom;
        m_showAllSubsystemsBtn.MoveWindow(showAllSubsBtnRect);
		//m_showAllSubsystemsBtn.ShowWindow(SW_HIDE);

        CRect gridRect;

        m_subsystemDutyGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = xferAllSubDutyBtnRect.top - 8;
        m_subsystemDutyGrid.MoveWindow(gridRect);        
    }	
}


COLORREF SubsystemDutyPropertyPage::convertColourStrToColorref( const std::string& colourStr,
                                                                const COLORREF& defaultColour ) const
{
    FUNCTION_ENTRY("convertColourStrToColorref");

    COLORREF colour = defaultColour;

    if ( !colourStr.empty() )
    {
        // Start the conversion.
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        // Variables required for string->integer conversion.
        std::stringstream tempStream;
        unsigned long tempULong;
        unsigned char colourIndex = 0;
        unsigned char colourChannels[3];

        // Prepare the tokenizer and iterator.
        boost::char_separator<char> lineSep( "," );
        boost::tokenizer< boost::char_separator<char> > channelTokens( colourStr, lineSep );

        // For each channel...
        boost::tokenizer< boost::char_separator<char> >::iterator it;
        for ( it=channelTokens.begin(); ( it!=channelTokens.end() ) && ( 3>colourIndex ); it++ )
        {
            // Prepare the string stream for conversion.
            tempStream.str( *it );
            tempStream.clear();

            // Convert the channel to an integer and add it to the array of colour channel information if
            // the conversion successed.
            if ( !( tempStream >> tempULong ).fail() && ( 256 > tempULong ) )
            {
                colourChannels[colourIndex++] = static_cast<unsigned char>(tempULong);
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while tokenizing RGB value %s, %s is not a valid number for an 8-bit colour channel.",
                    colourStr.c_str(), (*it).c_str() );
                return defaultColour;
            }
        }

        // Create the colour.
        colour = RGB( colourChannels[0], colourChannels[1], colourChannels[2] );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Colour string was empty, using default." );
    }

    FUNCTION_EXIT;
    return colour;
}


COLORREF SubsystemDutyPropertyPage::determineCellBkColour(  const bool inRegion, const TA_Base_Bus::ESubsystemState state  ) const
{
    COLORREF cellColour = m_unallocatedSubsystemColour;

    // See if the subsystem exists in the current region.
    if ( inRegion )
    {
        // It is in the region, so the colour will depend on subsystem state.
        switch ( state )
        {
        case TA_Base_Bus::E_SS_NORMAL:
            cellColour = m_normalDutyColour;
            break;

        case TA_Base_Bus::E_SS_DELEGATED:
            cellColour = m_delegatedDutyColour;
            break;

        case TA_Base_Bus::E_SS_DEGRADED:
            cellColour = m_degradedDutyColour;
            break;

        default:
            TA_ASSERT( false, "Could not determine colour for undefined subsystem state." );
            break;
        };
    }

    return cellColour;
}

//TD15533++
TA_Base_Bus::ActionGroupKey SubsystemDutyPropertyPage::getActionGroupDB(
	const TA_Base_Bus::ProfileKey profileKey,
	const TA_Base_Bus::SubsystemKey subsystemKey,
	const TA_Base_Bus::ESubsystemState subsystemState)
{
	TA_Base_Bus::ActionGroupKey actionGroupKey = NO_ACTION_GROUP;

	try
	{
		// find in cache
		for (int i=0; i<(int)m_actionGroupsDB.size(); i++)
		{
			if ((m_actionGroupsDB[i].profileKey == profileKey) &&
				(m_actionGroupsDB[i].subsystemKey == subsystemKey) &&
				(m_actionGroupsDB[i].subsystemState == subsystemState))
			{
				actionGroupKey = m_actionGroupsDB[i].actionGroupKey;
				break;
			}
		}

		// not yet in cache
		if (actionGroupKey == NO_ACTION_GROUP)
		{
			// find in DB	
			actionGroupKey = TA_Base_Core::RightsAccessFactory::getInstance().getActionGroup(
				profileKey,
				subsystemKey,
				subsystemState);

			// insert in list
			TA_Base_App::PSSA pssa;
			pssa.profileKey = profileKey;
			pssa.subsystemKey = subsystemKey;
			pssa.subsystemState = subsystemState;
			pssa.actionGroupKey = actionGroupKey;
			m_actionGroupsDB.push_back(pssa);
		}
	}
	catch (...)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in getActionGroupDB()");
	}

    return actionGroupKey;
}
//++TD15533

//TD15533++
TA_Base_App::CellData SubsystemDutyPropertyPage::generateCellData(
	const TA_Base_Bus::SubsystemKey subsystemKey, 
	const TA_Base_Bus::RegionKey regionKey,
	const TA_Base_Bus::ESubsystemState subsystemState)
{
	TA_Base_App::CellData cellData;
	cellData.baseImage = -1;
	cellData.baseText = "";
	cellData.overrideImage = -1;
	cellData.overrideText = "";
	cellData.isOverrided = false;

	//OLD
	//case 1: currentActionGroup is empty --> cross / cross (no duty)
	//case 2: currentActionGroup is not empty and has / --> tick / tick (both have duty)
	//case 3: currentActionGroup is not empty but no / --> tick / cross OR cross / tick (one profile has duty)

	//NEW
	//case 1: ProfilesWithDuties is empty --> cross / cross (no duty)
	//case 2: ProfilesWithDuties is 2 --> tick / tick (both have duty)
	//case 3: ProfilesWithDuties is 1 --> tick / cross OR cross / tick (one profile has duty)

	//other cases (handled by CGridCtrlS)
	//case 5: empty / tick (no right, no duty / with rights, with duty)
	//case 6: empty / cross (no right, no duty / with rights, no duty)
	//case 7: tick / empty (with right, with duty / no right, no duty)
	//case 8: cross / empty (with right, no duty / no right, no duty)
	//case 9: empty / empty (no right, no duty / no right, no duty) displayed as gray cell

	//always assume base is 0th element and override is 1st element

	TA_Base_App::ProfilesWithDuties profilesWithDuties = getProfilesWithDuties(subsystemKey, regionKey);

	if (profilesWithDuties.size() == 2)
	{
		cellData.baseImage = GridAssistant::GI_TICK;
		cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
		cellData.overrideImage = GridAssistant::GI_TICK;
		cellData.overrideText = getActionGroupNameWithErrorChecking(profilesWithDuties[1].actionGroupKey);
		cellData.isOverrided = true;
	}
	else if (profilesWithDuties.size() == 0)
	{
		if (m_currentSessionInfo.ProfileId.size() == 2)
		{
			TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
				getActionGroupDB(m_currentSessionInfo.ProfileId[0], subsystemKey, subsystemState);
			TA_Base_Bus::ActionGroupKey overrideActionGroupKey = 
				getActionGroupDB(m_currentSessionInfo.ProfileId[1], subsystemKey, subsystemState);

			cellData.baseImage = GridAssistant::GI_CROSS;
			cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
			cellData.overrideImage = GridAssistant::GI_CROSS;
			cellData.overrideText = getActionGroupNameWithErrorChecking(overrideActionGroupKey);
			cellData.isOverrided = true;
		}
		else if (m_currentSessionInfo.ProfileId.size() == 1)
		{
			TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
				getActionGroupDB(m_currentSessionInfo.ProfileId[0], subsystemKey, subsystemState);

			cellData.baseImage = GridAssistant::GI_CROSS;
			cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
		}
		//else... profile cant be 0
	}
	else if (profilesWithDuties.size() == 1)
	{
		if (m_currentSessionInfo.ProfileId.size() == 2)
		{
			if (profilesWithDuties[0].profileKey == m_currentSessionInfo.ProfileId[0])
			{
				TA_Base_Bus::ActionGroupKey overrideActionGroupKey = 
					getActionGroupDB(m_currentSessionInfo.ProfileId[1], subsystemKey, subsystemState);

				cellData.baseImage = GridAssistant::GI_TICK;
				cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
				cellData.overrideImage = GridAssistant::GI_CROSS;
				cellData.overrideText = getActionGroupNameWithErrorChecking(overrideActionGroupKey);
				cellData.isOverrided = true;
			}
			else if (profilesWithDuties[0].profileKey == m_currentSessionInfo.ProfileId[1])
			{
				TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
					getActionGroupDB(m_currentSessionInfo.ProfileId[0], subsystemKey, subsystemState);

				cellData.baseImage = GridAssistant::GI_CROSS;
				cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
				cellData.overrideImage = GridAssistant::GI_TICK;
				cellData.overrideText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);;
				cellData.isOverrided = true;
			}
		}
		else if (m_currentSessionInfo.ProfileId.size() == 1)
		{
			// okay to assume this profile has duty
			cellData.baseImage = GridAssistant::GI_TICK;
			cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
		}		
		//else... profile cant be 0
	}

	// Yanrong++ TD19076. 
	// Treat with "TCS_Train Comms(stn)" subsystem specially for 4669 project,
	// display yellow tick flag while the central profile has no duty in normal mode
	if( m_dataCache.getSubsystemName(subsystemKey).compare(TCS_TRAIN_COMMS_STN_SUBSYSTEM) == 0 // TCS subsystem
		&& TA_Base_Core::RunParams::getInstance().get( RPARAM_PROJECTNAME ).compare( TCS_SPECIAL_PROJECT_NAME ) == 0 // 4669 project
		&& subsystemState == TA_Base_Bus::E_SS_NORMAL // normal mode
		&& TA_Base_Core::SessionAccessFactory::getInstance().getSession( m_currentSessionInfo.SessionId )->getLocationKey() == OCC_LOCATION // is OCC location
		)
	{
		TA_Base_Bus::SessionInfoEx sessionInfoEx = m_sessionCache.getSessionInfo( m_currentSessionInfo.SessionId );

		// Duty manager will not judge location duty. As some profile will not have location duty when login, such as TCO group profiles.
		// In normal mode, if the profile has duty, show tick; otherwise, show box tick.
		if( profilesWithDuties.size() == 0 ) // has no duty
		{
			if( m_currentSessionInfo.ProfileId.size() == 1 ) // no override
			{
				cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
			}
			else if( m_currentSessionInfo.ProfileId.size() == 2 ) // override
			{
				cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
				cellData.overrideImage = GridAssistant::GI_DELEGATE_TICK;
			}
		}
		else if( profilesWithDuties.size() == 1 && m_currentSessionInfo.ProfileId.size() == 2 ) // override and only one profile has duty
		{
			if( profilesWithDuties[0].profileKey != m_currentSessionInfo.ProfileId[0] ) // base profile has no duty
			{
				cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
			}
			else if( profilesWithDuties[0].profileKey != m_currentSessionInfo.ProfileId[1] ) // override profile has no duty
			{
				cellData.overrideImage = GridAssistant::GI_DELEGATE_TICK;
			}
		}
	}
	// ++Yanrong
	
    return cellData;
}
//++TD15533

//TD15533++
TA_Base_App::ProfilesWithDuties SubsystemDutyPropertyPage::getProfilesWithDuties(
	const TA_Base_Bus::SubsystemKey subsystemKey,
	const TA_Base_Bus::RegionKey regionKey)
{
	TA_Base_App::ProfilesWithDuties profilesWithDuties;

    SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
    SubsystemDutyList subsystemDuties(sdmElement.getSubsystemDuties(m_currentSessionInfo.SessionId));

    if (!subsystemDuties.empty())
    {
        for (SubsystemDutyList::const_iterator dutyIt = subsystemDuties.begin(); dutyIt != subsystemDuties.end(); dutyIt++)
        {
			TA_Base_App::PSSA profileWithDuty;
			profileWithDuty.profileKey = dutyIt->profileKey;
			profileWithDuty.actionGroupKey = dutyIt->actionGroupKey;
			profilesWithDuties.push_back(profileWithDuty);
        }
    }

    return profilesWithDuties;
}
//++TD15533

//TD15533++
TA_Base_Bus::ProfileKeyList SubsystemDutyPropertyPage::getProfileKeyList(
	const TA_Base_Bus::SubsystemKey subsystemKey,
	const TA_Base_Bus::RegionKey regionKey)
{
	TA_Base_Bus::ProfileKeyList profileKeys;

    SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
    SubsystemDutyList subsystemDuties(sdmElement.getSubsystemDuties(m_currentSessionInfo.SessionId));

    if (!subsystemDuties.empty())
    {
        for (SubsystemDutyList::const_iterator dutyIt = subsystemDuties.begin(); dutyIt != subsystemDuties.end(); dutyIt++)
        {
			profileKeys.push_back(dutyIt->profileKey);
        }
    }

    return profileKeys;
}
//++TD15533

//TD15533++
std::string SubsystemDutyPropertyPage::getActionGroupNameWithErrorChecking(
	const TA_Base_Bus::ActionGroupKey actionGroupKey)
{
	std::string actionGroupName = "";

	if (actionGroupKey != NO_ACTION_GROUP)
	{
		// yanrong ++
		//actionGroupName = m_dataCache.getActionGroupName(actionGroupKey);
		actionGroupName = getActionGroupNameWithAGK(actionGroupKey);
		// ++ yanrong
	}	

    return actionGroupName;
}
//++TD15533


// Yanrong ++
std::string SubsystemDutyPropertyPage::getActionGroupNameWithAGK(unsigned long actionGroupKey)
{
	if( m_actionGroups.empty() )
	{
		// Get all action groups from DB.
		m_actionGroups = TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroups();
	}

	std::vector<TA_Base_Core::IActionGroup*>::iterator iter = m_actionGroups.begin();
	for( ; iter != m_actionGroups.end(); ++iter )
	{
		if( (*iter)->getKey() == actionGroupKey )
		{
			//return (*iter)->getName().c_str();
			std::string displayName = (*iter)->getDisplayName();
			if(displayName.empty() || displayName.length() == 0)
			{
				displayName= (*iter)->getName();
			}
			return displayName;
		}
	}

	return "";
}
// ++ Yanrong


//TD15533++
bool SubsystemDutyPropertyPage::isSubsystemAccessible(
	const TA_Base_Bus::ProfileKeyList profileKeys,
	const TA_Base_Bus::SubsystemKey subsystemKey)
{
	bool isAccessible = false;

	SubsystemKeyList accessibleSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(profileKeys));
	for (int i=0; i<(int)accessibleSubsystemKeys.size(); i++)
	{
		if (accessibleSubsystemKeys[i] == subsystemKey)
		{
			isAccessible = true;
			break;
		}
	}

    return isAccessible;
}
//++TD15533

//TD15533++
TA_Base_Bus::RegionKeyList SubsystemDutyPropertyPage::getLocations()
{
	TA_Base_Bus::RegionKeyList regionKeys;

	/* NOTE: for local profile, only current location should be displayed.
	 * for central profile, all locations should be displayed.
	 * if local override with central, then all locations are displayed.
	 * if local override with local, same location is displayed
	 * if central is override with local, there is no such case.
	 * this has nothing to do with rights or duties, only the location.
	 */

	//based on session, get location from DB
	//if 0 then central, if not then local
	//TA_Base_Core::ISession* session;
	//session = TA_Base_Core::SessionAccessFactory::getInstance().getSession( m_currentSessionInfo.SessionId );
	//TA_Base_Bus::AuthenticationLibrary authLib(true);
	//TA_Base_Bus::SessionInfo sesInfo = authLib.getSessionInfo( m_currentSessionInfo.SessionId, m_currentSessionInfo.SessionId );
	//TA_Base_Bus::SessionInfo sesInfo = m_sessionCache.getSessionInfo(m_currentSessionInfo.SessionId);

	//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
	//	"In SubsystemDutyPropertyPage::getLocations()..., sessionId = %s",sesInfo.SessionId.c_str());

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
		"In SubsystemDutyPropertyPage::getLocations()..., sessionId = %s",m_currentSessionInfo.SessionId.c_str());

	// check if it has ended or not
	//unsigned long locationKey = session->getLocationKey();
	unsigned long locationKey = m_currentSessionInfo.LocationId;
	if (locationKey == ALL_LOCATION) 
	{
		//central profile
		regionKeys.push_back(locationKey);

		//no need to check overrided profile's location
	}
	else
	{
		//local profile
		regionKeys.push_back(locationKey);
		
		//need to check overrided profile's location
		//always assume base is 0th element and override is 1st element

		try
		{
			//overrided
			if (m_currentSessionInfo.ProfileId.size() == 2)
			{
				//get profile from DB if local or central for base
				unsigned long baseProfile = m_currentSessionInfo.ProfileId[0];
				TA_Base_Core::IProfile* profileB;
				profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );
				if (profileB->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same

				//get profile from DB if local or central for override
				unsigned long overrideProfile = m_currentSessionInfo.ProfileId[1];
				TA_Base_Core::IProfile* profileO;
				profileO = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( overrideProfile );
				if (profileO->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same
			}
			//single operator
			else if (m_currentSessionInfo.ProfileId.size() == 1)
			{
				//get profile from DB if local or central for base
				unsigned long baseProfile = m_currentSessionInfo.ProfileId[0];
				TA_Base_Core::IProfile* profileB;
				profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );
				if (profileB->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same
			}
			//else... profile cant be 0
		}
		catch (...) 
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in getLocations()");
		}
	}    

	return regionKeys;
}
//++TD15533

//TD15533++
bool SubsystemDutyPropertyPage::isRegionAccessible(
	const TA_Base_Bus::RegionKey regionKey,
	const TA_Base_Bus::RegionKeyList regionKeys)
{
	bool hasLocationDuty = false;

	for (int i=0; i<(int)regionKeys.size(); i++)
	{
		if (regionKeys[i] == ALL_LOCATION)
		{
			//if central location then grant duty for this region
			hasLocationDuty = true;
			break;
		}
		else if (regionKeys[i] == regionKey)
		{
			hasLocationDuty = true;
			break;
		}
	}

    return hasLocationDuty;
}
//++TD15533

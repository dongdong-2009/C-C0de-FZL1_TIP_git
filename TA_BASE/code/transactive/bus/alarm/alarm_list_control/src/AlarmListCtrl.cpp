/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmListCtrl.cpp $
* @author:  Karen Graham
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2015/01/22 17:50:12 $
* Last modified by:  $Author: Noel $
* 
* This is a read-only list of alarms.
*/

#pragma warning(disable:4786)

#include <algorithm>
#include <set>

#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"
#include "bus/alarm/alarm_list_control/src/AlarmStore.h"
#include "bus/alarm/alarm_list_control/src/IAlarmListObserver.h"
#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.h"
#include "bus/alarm/alarm_list_control/src/resource.h"
#include "bus/alarm/alarm_list_control/src/IAlarmColumn.h"
#include "bus/alarm/alarm_list_control/src/ActionThread.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnIdentifiers.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/resources/resource.h"
#include "bus\mfc_extensions\src\coding\Encoder.h"			// utf8 encode/decode

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/alarm/alarm_list_control/src/Se_Exception.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/alarm/alarm_list_control/src/ConstDefine.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmActionException;
using TA_Base_Core::ThreadGuard;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRESSED 0x8000
namespace
{
    // ActionThread::PERFORM_ACTION is WM_USER + 200 so use the next in the sequence.
    const UINT WM_PERFORM_PREFILTER_UPDATE = WM_USER + 201;
	
    // This structure is needed to encapsulate all the information required for a prefilter
    // update.
    typedef struct
    {
        TA_Base_Bus::Filter preFilter;
        std::vector<unsigned long> locations;
        std::string sessionID;
    } PrefilterUpdate;
	
}

namespace TA_Base_Bus
{
	const int AlarmListCtrl::UPDATE_TIMER    = 3; //3
	const int AlarmListCtrl::PREACTION_TIMER = 6;
	
    const COLORREF AlarmListCtrl::BACKGROUND = RGB(255, 255, 255);
    const COLORREF AlarmListCtrl::DEFAULT_COLOUR = RGB(0,0,0);
	
    const int AlarmListCtrl::UPDATE_SLEEP_IN_MILLISECONDS = 400; // This is also the flash time 1500
	
    // Values to show in columns
    const CString AlarmListCtrl::NO_VALUE = "";
	//   const CString AlarmListCtrl::NO_ALARM_STORE_MSG = "Alarms cannot currently be retrieved.";    // UE-040038
	//   const CString AlarmListCtrl::NO_ALARMS_MATCH_FILTER_MSG = "No alarms match current filter.";
	////TD17081++ 
	//   const CString AlarmListCtrl::ALARMS_RETRIEVAL_INPROGRESS_MSG = "Loading of alarms is in progress.";
	////++TD17081
	const CString AlarmListCtrl::NO_ALARM_STORE_MSG = "目前无法获取警报.";    // UE-040038
	const CString AlarmListCtrl::NO_ALARMS_MATCH_FILTER_MSG = "没有匹配当前滤器的警报.";
	//TD17081++ 
	const CString AlarmListCtrl::ALARMS_RETRIEVAL_INPROGRESS_MSG = "正在载入警报.";
	//++TD17081


	const UINT AlarmListCtrl::WM_MENU_BEGIN = 4000;
    const UINT AlarmListCtrl::WM_MENU_END = 4999;
	ACE_Thread_Mutex AlarmListCtrl::m_displayLock;
    AlarmListCtrl::AlarmListCtrl(bool canColumnsBeSorted /*=true*/, bool canColumnsBeReordered /*=true*/)
        : m_alarmStore(NULL),
		m_alarmActionFactory(NULL),
		m_areAlarmsBeingInsertedOrDeleted(false),
		m_observer(NULL),
		m_canColumnsBeSorted(canColumnsBeSorted),
		m_canColumnsBeReordered(canColumnsBeReordered),
		m_isFlashTime(false),
		m_sortColumn(-1),
		m_sortAscending(false), // Set to false so first sort will change this back to true
		m_hasSelectionChanged(false),
		m_isLoadingAlarms(false),
		m_isLastActionSort(false),
		m_isLastActionAckOrClose(false),
		m_bToolTipCtrlCustomizeDone(FALSE),
		m_curSortCol(SORT_BY_DEFAULT),
		//m_needFlash(true),
		m_needUpdateData(false),
		//CL-20972 xinyang++
// 		addnewAlarm (false),
// 		displayNoNull (false),
// 		maxtime(0),
		m_strSelectedAlarmID(""), //++Noel
		m_nCurSelPos(0),
		m_bHasSelected(false),
		m_isRangeSel(false),
		m_nSelDirection(0),
		m_bIgnoreEvent(false),
		m_ulIndexBegin(0),
		m_ulIndexEnd(0),
		m_nFlashCount(0),
		m_loadedSelectedAlarms(0),
		m_ulIdxFrom(0),
		m_ulIdxTo(0),
		m_bIsColResizable(true)
		//++xinyang
    {
        FUNCTION_ENTRY("Constructor");
		m_lockedListCtrl=false;
		_set_se_translator(trans_func);
	/*	if ( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING))
		{
			m_needFlash = false;
		}
		*/
		m_FASAlarmHandle=NULL;
		if (RunParams::getInstance().isSet(Entity_Name_Para))
		{
			std::string entityname=RunParams::getInstance().get(Entity_Name_Para);
			std::transform(entityname.begin(), entityname.end(), entityname.begin(), toupper);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Entity_Name=%s",entityname.c_str());

			if (entityname==Entity_Name_LeftBannerUpper)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "enable FASAlarmHandler");
				m_FASAlarmHandle=new FASAlarmHandler(m_databaseCache);
				m_FASAlarmHandle->start();
			}
			
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "is not isSet Entity_Name");
		}
		//++Noel
		m_pDisplayVector = m_alarmDataSync.getDisplayData();

//		this->GetHeaderCtrl()-&gt;SetDlgCtrlID(this->GetDlgCtrlID());
		FUNCTION_EXIT;
    }
	
	
    AlarmListCtrl::~AlarmListCtrl()
    {
		// See the method OnDestroy(). Most cleanup is performed in there
		if (m_FASAlarmHandle!=NULL)
		{
			m_FASAlarmHandle->terminateAndWait();
			delete m_FASAlarmHandle;
			m_FASAlarmHandle=NULL;
		}
	}

	
	BEGIN_MESSAGE_MAP(AlarmListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(AlarmListCtrl)
		ON_WM_TIMER()
		ON_NOTIFY_REFLECT(LVN_GETDISPINFO, onGetdispinfo)
		ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, onCustomdrawAlarmList )
		ON_WM_ERASEBKGND()
	//	ON_WM_DESTROY()
		ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, onOdCacheHint)
		ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//	ON_NOTIFY_REFLECT(NM_DBLCLK, onDblclk)
		ON_NOTIFY_REFLECT(NM_RCLICK, onRightClick)
		ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, onItemChange)
		ON_NOTIFY_REFLECT(LVN_ODSTATECHANGED, onODItemChange)
		ON_NOTIFY(HDN_ENDTRACKW, 0, OnEndtrack)
		ON_NOTIFY(HDN_ENDTRACKA, 0, OnEndtrack)
// 		ON_NOTIFY(HDN_BEGINTRACKW, 0, OnBegintrack)
// 		ON_NOTIFY(HDN_BEGINTRACKA, 0, OnBegintrack)
		//ON_WM_KEYUP()
		//	ON_WM_PAINT()       //TD15005
	//	ON_WM_LBUTTONDOWN()
	//	ON_WM_MOUSEMOVE()
		ON_MESSAGE(WM_PERFORM_PREFILTER_UPDATE,OnPerformPrefilterUpdate)
		ON_MESSAGE(ALARMSTORE_REFRESH, OnAlarmIndexRefresh)
		ON_MESSAGE(AlARMSTORE_RESTART, OnAlarmStoreRestart)
		ON_MESSAGE(AlARMSTORE_REMAP, OnAlarmStoreRemap)
		ON_MESSAGE(AlARMSTORE_UNMAP, OnAlarmStoreUnmap)
//		ON_MESSAGE(WM_NOTIFY, OnNotify)
		//}}AFX_MSG_MAP
		ON_COMMAND_RANGE(WM_MENU_BEGIN, WM_MENU_END, onContextMenuItemSelected)
		ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
		ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
		END_MESSAGE_MAP()
		
		
		/////////////////////////////////////////////////////////////////////////////
		// Public methods
		/////////////////////////////////////////////////////////////////////////////
		
		void AlarmListCtrl::startAlarmList( IAlarmListObserver& observer,
		AlarmActionFactory& alarmActionFactory,
		const std::vector<IAlarmColumn*> &columns,
		const Filter& preFilter,
		const std::vector<unsigned long>& locations,
		long sortColumn,
		bool isAscendingSort,
		const Filter& filter,
		const std::string& sessionId,
		bool isHighestSeverityNeeded)
    {
        FUNCTION_ENTRY("startAlarmList");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "set new font.");
		//setup font
		m_font.CreatePointFont(110,"宋体");
		SetFont(&m_font,FALSE);
		

		//SetHeaderFont(&m_font,FALSE);


		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "About to store observer.");

		m_observer = &observer;
		m_loadedSelectedAlarms = -1;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting up the alarm retriever and alarm action factory.");
		m_alarmActionFactory = &alarmActionFactory;
		m_alarmRetriever.setAlarmListCtrl(*this);
		m_alarmRetriever.setSelectionRetriever(m_alarmDataSync.getSelectionWrapper());

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting extended styles of list");

		// TD #11289
		// only allow reordering of columns by drag & dropping if the flag has been set

		if(m_canColumnsBeReordered)
		{
			if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_NOGRIDLINES.c_str()) )
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES);
			}
			else
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
			}
			
		}
		else
		{
			if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_NOGRIDLINES.c_str()) )
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES);
			}
			else
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
			}
			
		}
		
//		SetCallbackMask(LVIS_SELECTED);
        SetBkColor(BACKGROUND);
		
        // Add the columns to the list
        Sort sort;
        
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding the columns to the list");
        for(unsigned int column = 0; column < columns.size(); ++column)
        {
            TA_ASSERT(columns[column] != NULL, "One of the columns passed in was NULL");
			
            if (columns[column]->isColumnEnabled())
            {
                // Only insert the column into the list and into the order vector if it is enabled
				//TD14897 azenitha++
				if (columns[column]->getColumnIdentifier() == TA_Base_Bus::COL_SEVERITY)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD14897] Setting severity column to center justified");
					InsertColumn(column, 
						columns[column]->getColumnHeaderText(),
						LVCFMT_CENTER,
						columns[column]->getColumnWidth());
				}
				else
				{
					InsertColumn(column, 
						columns[column]->getColumnHeaderText(),
						LVCFMT_LEFT,
						columns[column]->getColumnWidth());
				}
				//TD14897 ++azenitha
				
                m_alarmColumnOrder.push_back(columns[column]->getColumnIdentifier());
            }
			
            // Store the column in the map. 
            m_alarmColumns.insert( std::map<long, IAlarmColumn*>::value_type(columns[column]->getColumnIdentifier(),
				columns[column]) );
			
			
            // While we're stepping through we should save the sort that we are meant to be sorting on.
            if (columns[column]->getColumnIdentifier() == sortColumn)
            {
                if (isAscendingSort)
                {
                    sort = columns[column]->getColumnAscendingSort();
                }
                else
                {
                    sort = columns[column]->getColumnDescendingSort();
                }
				m_curSortCol=columns[column]->getSortColumn();
            }
        }
		
        addBitmapsToColumnHeaders();
        loadRequiredIcons();
		
        // Create the alarm store so we can get all our alarms to add
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating the alarm store");
        try
        {
            m_alarmStore = new AlarmStore(preFilter, locations, sort, filter, sessionId, isHighestSeverityNeeded);
            
            if (m_alarmStore == NULL)
            {
                TA_THROW( TA_Base_Core::AlarmListCtrlException("Alarm Store could not be constructed. New failed") );
            }
			
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not initialise the alarm store.");
            //UE-040022
            TA_THROW( TA_Base_Core::AlarmListCtrlException("The alarm list could not be displayed because the database configuration is invalid.") );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not initialise the alarm store");
            //UE-040022
            TA_THROW( TA_Base_Core::AlarmListCtrlException("The alarm list could not be displayed because the database configuration is invalid.") );
        }
		
		
        if(  m_pDisplayVector->getSize() == 0 && 
            (m_alarmStore != NULL && !m_alarmStore->isAlarmStoreValid()) )
        {
            // This means we have an invalid alarm store
            // Set the number of items to 1 so we can display an error message in the list control
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Indicating the alarm store is not yet initialised");
        }

 //		m_sortColumn = -1;
 		m_sortAscending = isAscendingSort; // set to the opposite of what we want it will switch in	
		m_alarmDataSync.setDisplaySort(m_curSortCol, m_sortAscending);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Starting timers for flashing and updating");
		
        // Set up a timer so we know when to update the list and make it flash
//		CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
//		thread->PostThreadMessage(ActionThread::PERFORM_PREACTION,
//			reinterpret_cast<WPARAM>(this),NULL);
		SetTimer(UPDATE_TIMER, UPDATE_SLEEP_IN_MILLISECONDS, NULL);
		
		m_alarmDataSync.setStoreService(m_alarmStore);
		m_alarmDataSync.setTarget(m_hWnd);
		m_alarmDataSync.initialize();
	
        FUNCTION_EXIT;
    }
	
	void AlarmListCtrl::OnTimer(UINT nIDEvent) 
	{
		switch(nIDEvent)
		{
		case  AlarmListCtrl::UPDATE_TIMER:
			{
				bool needRefresh=false;
			
				if(m_needUpdateData && !m_alarmDataSync.isUpdating())
				{
//			        ThreadGuard guard(m_updateDisplayLock);		
					needRefresh=true;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace,"alarm Refresh");
				    try
					{
						m_ulIdxFrom = GetTopIndex();
						m_ulIdxTo = m_ulIdxFrom + GetCountPerPage();

						_setListDisplayInfo();
						
						TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo tempInfo = m_alarmDataSync.getIndexInfo();
						bool bWereAlarmAdded = (m_info.dutyAlarmVersion != tempInfo.dutyAlarmVersion);
						m_info.earliestSourceTime.millitm = tempInfo.earliestSourceTime.millitm;
						m_info.earliestSourceTime.time = tempInfo.earliestSourceTime.time;
						m_info.latestSourceTime.millitm = tempInfo.latestSourceTime.millitm;
						m_info.latestSourceTime.time = tempInfo.latestSourceTime.time;
						m_info.exceededMaxDisplay = tempInfo.exceededMaxDisplay;
						m_info.highestSeverity = tempInfo.highestSeverity;
						m_info.isLoadingAlarms = tempInfo.isLoadingAlarms;
						m_info.totalAlarms = tempInfo.totalAlarms;
						m_info.unackAlarms = tempInfo.unackAlarms;
						m_info.dutyAlarmVersion = tempInfo.dutyAlarmVersion;

						updateObserver(true, bWereAlarmAdded, m_info, 0);						
					}
					catch (...)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"Alarm Sound update function is error");
					}
				
					SetItemCountEx(m_pDisplayVector->getSize(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
					m_needUpdateData = false;	
				}
				else
				{
					if (m_alarmDataSync.isUpdating()/*m_alarmSyn.isUpdating()*/) 
					{
					  LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"alarm store is updating");
				  }
				}
				m_nFlashCount += UPDATE_SLEEP_IN_MILLISECONDS;

				if (m_nFlashCount >= 1500)
				{
					m_isFlashTime = !m_isFlashTime;	
					m_nFlashCount = 0;
					needRefresh=true;
				}	

				if (needRefresh==true)	Invalidate();
			
			
				return;  //Must return, otherwise CListCtrl::OnTimer(nIDEvent) will be called and nullifies the SetTimer routine.
			}

		}
		
		CListCtrl::OnTimer(nIDEvent);
	}

	
    void AlarmListCtrl::updatePreFilterLocationsAndSession( const Filter& preFilter,
		const std::vector<unsigned long>& locations,
		const std::string& sessionID)
    {
        FUNCTION_ENTRY("updatePreFilterLocationsAndSession");
		
        // TD12019: Make the alarm store communication occur in a different thread so that communication
        //          issues with the alarm store do not give the appearance that a profile change is
        //          taking a significant amount of time (delays in this method block the control station).
        std::auto_ptr<PrefilterUpdate> prefilterUpdate( new PrefilterUpdate );
        prefilterUpdate->preFilter = preFilter;
        prefilterUpdate->locations = locations;
        prefilterUpdate->sessionID = sessionID;
		
        PostMessage( WM_PERFORM_PREFILTER_UPDATE, reinterpret_cast<WPARAM>( prefilterUpdate.release() ) );
		
        FUNCTION_EXIT;
    }
	
	
    void AlarmListCtrl::enableColumn(long column, bool isEnabled, unsigned int position)
    {
        FUNCTION_ENTRY("enableColumn");
		
        std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(column);
        TA_ASSERT(matching != m_alarmColumns.end(), "The alarm column passed is invalid.");
		
        matching->second->setIsColumnEnabled(isEnabled);
		
        if (isEnabled)
        {
            // Add to order vector if not already in there
            if (m_alarmColumnOrder.end() != std::find(m_alarmColumnOrder.begin(), m_alarmColumnOrder.end(), column))
            {
                // Must add it into the list
                std::vector<long>::iterator insertPos = m_alarmColumnOrder.begin();
                std::advance(insertPos, position);
                m_alarmColumnOrder.insert( insertPos, column );
				
                // Add to the list control
				//TD14897 azenitha++
				if (matching->second->getColumnIdentifier() == TA_Base_Bus::COL_SEVERITY)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD14897] Setting severity column to center justified");
					InsertColumn(position, matching->second->getColumnHeaderText(), LVCFMT_CENTER, matching->second->getColumnWidth());
				}
				else
				{
					InsertColumn(position, matching->second->getColumnHeaderText(), LVCFMT_LEFT, matching->second->getColumnWidth());
				}
				//TD14897 ++azenitha
            }
        }
        else
        {
            // Remove from order vector
            std::vector<long>::iterator iter = std::find(m_alarmColumnOrder.begin(), m_alarmColumnOrder.end(), column);
            if (iter != m_alarmColumnOrder.end())
            {
                int removePos = std::distance(m_alarmColumnOrder.begin(), iter);
                m_alarmColumnOrder.erase(std::remove(m_alarmColumnOrder.begin(), m_alarmColumnOrder.end(), column));
				
                // Remove from the list control
                DeleteColumn(removePos);
            }
        }
		
        if (m_observer != NULL) // This means the list has been initialised
        {
            if (isEnabled)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Enabling column %d", column);
                SetColumnWidth(column,matching->second->getColumnWidth());
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Disabling column %d", column);
                SetColumnWidth(column,0);
            }
        }
		
        FUNCTION_EXIT;
    }
	
	
    void AlarmListCtrl::setFilter(const Filter& filter,bool isDeault)
    {
        FUNCTION_ENTRY("setFilter");
		
        TA_ASSERT(m_alarmStore != NULL, "This method must be called after startAlarmList has successfully completed");
		

		m_alarmDataSync.applyDutyIndexFilter(filter, isDeault);
        //m_alarmStore->setFilter(filter, m_alarmDataSync.getClientPos(),isDeault);
		m_alarmDataSync.unSelectAllAlarmItem();
		m_bIgnoreEvent = false;
		
        FUNCTION_EXIT;
    }


	void AlarmListCtrl::setSort(SORT_COLUMN sortType)
	{
//		ThreadGuard guard(m_updateDisplayLock);
		m_curSortCol = sortType;
		m_alarmDataSync.setDisplaySort(m_curSortCol, m_sortAscending);
	}
	
	
    Filter AlarmListCtrl::getCurrentFilter() const
    {
        FUNCTION_ENTRY("getCurrentFilter");
		
        TA_ASSERT(m_alarmStore != NULL, "This method must be called after startAlarmList has successfully completed");
		
        FUNCTION_EXIT;
		return m_alarmStore->getCurrentFilter();	
    }
	
    Sort AlarmListCtrl::getCurrentSort() const
    {
        FUNCTION_ENTRY("getCurrentSort");
		
        TA_ASSERT(m_alarmStore != NULL, "This method must be called after startAlarmList has successfully completed");
		
        FUNCTION_EXIT;
		return m_alarmStore->getCurrentSort();
		
    }
	
	
    void AlarmListCtrl::executeAction(UINT commandId)
    {
        FUNCTION_ENTRY("executeAction");
		
        TA_ASSERT(m_alarmActionFactory != NULL, "The alarm action factory has not been set. Has startAlarmList() been called");
		
        AlarmActionFactory::AlarmActionList::const_iterator iter = m_alarmActionFactory->getAlarmActions().begin();
        AlarmActionFactory::AlarmActionList::const_iterator iterEnd = m_alarmActionFactory->getAlarmActions().end();
		
        m_alarmRetriever.clearCache();
        while ( iter != iterEnd)
        {
			if ( commandId == INHIBIT_ALARM_BY_TYPE || commandId == INHIBIT_ALARM_BY_DESCRIPTION )
			{
				if ( (*iter)->getActionControlId() == INHIBIT_ALARM )
				{
					ActionSuppressAlarm* suppressAction = dynamic_cast <ActionSuppressAlarm*> (*iter);
					suppressAction->setSubActionControlId( commandId );
					
					// Start a thread to perform the action
					CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
					thread->PostThreadMessage(ActionThread::PERFORM_ACTION,
						reinterpret_cast<WPARAM>(*iter),
						reinterpret_cast<LPARAM>(&m_alarmRetriever) );
					
					// Indicate that the available actions should be reevaluated.
					m_hasSelectionChanged = true;
					break;
				}
			}
            else if( (*iter)->getActionControlId() == commandId )
            {
				// flag the last action if it is a ack or close alarm
				unsigned int actionId = (*iter)->getActionControlId();
				if (actionId == ACKNOWLEDGE_ALARM || actionId == CLOSE_AND_ACK_ALARM || actionId == ACKNOWLEDGE_ALL)
				{
					m_isLastActionAckOrClose = true;
				}
				else if ( actionId == INHIBIT_ALARM )
				{
					// reset the sub action control id to zero ie. perform the main action
					// not the sub action
					ActionSuppressAlarm* suppressAction = dynamic_cast <ActionSuppressAlarm*> (*iter);
					suppressAction->setSubActionControlId( 0 ); // reset the sub control
				}
				
                // Start a thread to perform the action
                CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
                thread->PostThreadMessage(ActionThread::PERFORM_ACTION,
					reinterpret_cast<WPARAM>(*iter),
					reinterpret_cast<LPARAM>(&m_alarmRetriever) );
				
                // Indicate that the available actions should be reevaluated.
                m_hasSelectionChanged = true;
                break;
            }
            ++iter;
        }
		
        FUNCTION_EXIT;
    }
	
    bool AlarmListCtrl::deleteSharedAlarm(std::vector<std::string>& alarmIDs)
    {
        if (m_alarmStore == NULL || !m_alarmStore->isAlarmStoreValid())
        {
            return false;
        }
        return m_alarmStore->deleteSharedAlarm(alarmIDs);
    }

    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods
    /////////////////////////////////////////////////////////////////////////////
	
	
    void AlarmListCtrl::addBitmapsToColumnHeaders()
    {
        FUNCTION_ENTRY("addBitmapsToColumnHeaders");
		
        int column = 0;
        for(std::vector<long>::iterator columnId = m_alarmColumnOrder.begin(); columnId != m_alarmColumnOrder.end(); ++columnId)
        {
            std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(*columnId);
            if (matching != m_alarmColumns.end())
            {
                UINT bitmapID = matching->second->getColumnHeaderBitmapId();
                if (bitmapID != 0) // This means that the column does have a bitmap as a header
                {
					HDITEM headerItem; 
					headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
					
					CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
					headerCtrl->GetItem(column, &headerItem); 
					
					if (headerItem.hbm != 0)
					{ 
						DeleteObject(headerItem.hbm); 
						headerItem.hbm = 0; 
					} 
					headerItem.fmt |= HDF_BITMAP;
					headerItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 
					
                    TA_ASSERT(headerItem.hbm != NULL, "The resource file bus/alarm/alarm_list_control/src/AlarmListCtrl.rc must be included in the application's rc file. Or the bitmap id is invalid.");
					
					headerCtrl->SetItem(column, &headerItem);
                }
            }
            ++column;
        }
		
        FUNCTION_EXIT;
    }
	
	
    void AlarmListCtrl::loadRequiredIcons()
    {
        FUNCTION_ENTRY("loadRequiredIcons");
		
        bool listHasIcons = false; // Does this list control actually have any icons?
		
		m_imageList.Create(16, 16, ILC_MASK, 1, 1);
		
        for(std::map<long, IAlarmColumn*>::iterator iter = m_alarmColumns.begin(); iter != m_alarmColumns.end(); ++iter)
        {
			// Check if this column header has a bitmap
			if(iter->second->getColumnHeaderBitmapId() != 0)
			{
				// This column header has a bitmap, so add it to the list and set a flag indicating
				// there are bitmaps in this list control.
				iter->second->addIconsToImageList(m_imageList);
				listHasIcons = true;
			}
        }
		
		// Only call this function if there are actually icons in the list control.
		// Otherwise the list control has unnecessary space added to the left-hand side.
		if(listHasIcons)
		{
			SetImageList(&m_imageList, LVSIL_SMALL);
		}
		
        FUNCTION_EXIT;
    }

	std::string AlarmListCtrl::getAlarmComment(int item)
	{
		FUNCTION_ENTRY("getAlarmComment");
		std::string comment("");
		if( m_pDisplayVector == NULL)
		{
			FUNCTION_EXIT;
			return (const char*)NO_VALUE;
		}

		if (item < 0 || item >= static_cast<int>(m_pDisplayVector->getSize()))
		{
			// Item not within range
			FUNCTION_EXIT;
			return (const char*)NO_VALUE;
		}

		try
		{
			comment = m_pDisplayVector->getItemString(item, ALARM_COMMENTS);
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Alarm could not be retrieved. Details field will be set to an empty string");
		}

		FUNCTION_EXIT;
		return comment;
	}

    CString AlarmListCtrl::getItemText(int item, int subItem)
    {
		int columnId = m_alarmColumnOrder[subItem];
        std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(columnId);
        if (matching == m_alarmColumns.end())
        {
            return (const char *)NO_VALUE;
        }
        
        CString text = matching->second->getItemText(m_pDisplayVector, item).c_str();
		return text;
    }

    long AlarmListCtrl::getItemIcon(int item, int subItem)
    {
        if(  m_pDisplayVector->getSize() == 0 )
        {
            return -1;
        }
		
        if (item < 0 || item >= static_cast<int>( m_pDisplayVector->getSize()))
        {
            // Item not within range
            return -1;
        }
		
        AlarmDataItem* alarm = NULL;
        try
        {
            //std::string alarmId(m_alarms[item].uuid);
            alarm = m_pDisplayVector->getItemCopyAt(item);
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Alarm could not be retrieved. Details field will be set to no icon");
            return -1;
        }
		
        if (subItem < 0 || subItem >= static_cast<int>(m_alarmColumnOrder.size()))
        {
            if (alarm != NULL)
            {
                delete alarm;
                alarm = NULL;
            }
            return -1;
        }
		
        int columnId = m_alarmColumnOrder[subItem];
        std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(columnId);
		
        if (matching == m_alarmColumns.end())
        {
            if (alarm != NULL)
            {
                delete alarm;
                alarm = NULL;
            }
            return -1;
        }
		
        long iconIndex = matching->second->getItemIcon(alarm);
		
        if (alarm != NULL)
        {
            delete alarm;
            alarm = NULL;
        }
		
        return iconIndex;
	}
	
	
    void AlarmListCtrl::updateObserver(bool wereAlarmsModified, bool wereAlarmsAdded,
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& details,
		unsigned long selectedAlarmCount)
    {
		FUNCTION_ENTRY("AlarmListCtrl::updateObserver");
        // Now update the alarm counts and the highest severity
        if (m_alarmStore != NULL)
        {
            try
            {
                if (m_observer != NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating the observer alarm counts");
                    m_observer->updateAlarmCounts(details.totalAlarms, details.unackAlarms, details.exceededMaxDisplay, details.isLoadingAlarms);
                    
                    if( wereAlarmsModified )
                    {
						//CL-20972 xinyang++
// 						bool noAckAllAlarm = true;
// 						long playHighestSeverity = 4;
// 						std::vector<TA_Base_Core::AlarmItem*>::iterator itr = m_displayVec.begin();
//  					    if (!m_displayVec.empty())
//  					    {
//  							for ( itr; itr != m_displayVec.end(); ++itr)
//  							{
//  								if (strlen((*(*itr)).alarmAcknowledgedBy) == 0)
// 								{
// 									if ( playHighestSeverity > (*(*itr)).alarmSeverity )
// 									{
// 										playHighestSeverity = (*(*itr)).alarmSeverity;
// 									}
// 									noAckAllAlarm = false;
// 								}	
//  							}
							if (details.highestSeverity != 0)
							{
								m_observer->updateAlarmSound(details.highestSeverity, wereAlarmsAdded);
							}
							else 
							{
								m_observer->updateAlarmSound(0, wereAlarmsAdded);
							}		
/*						}*/
						//++xinyang
						//m_observer->updateAlarmSound(details.highestSeverity, wereAlarmsAdded);//CL-20972 xinyang--
                    }
                }
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve alarm details");
            }

        }
		
        updateObserverWithDescription(selectedAlarmCount, (details.totalAlarms > 0));
		FUNCTION_EXIT;
    }
	
    
    void AlarmListCtrl::updateObserverWithActionAvailability()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "updateObserverWithActionAvailability");
        if (m_observer == NULL)
        {
            return;
        }
        if (m_alarmStore == NULL)
        {
            return;
        }
		
        // Now step through all actions and add their availability to the map
        IAlarmListObserver::ItemAvailability itemAvailabilityMap;
		
        AlarmActionFactory::AlarmActionList::const_iterator action = m_alarmActionFactory->getAlarmActions().begin();
        AlarmActionFactory::AlarmActionList::const_iterator actionEnd = m_alarmActionFactory->getAlarmActions().end();
		
        m_alarmRetriever.clearCache();
        while ( action != actionEnd )
        {
            unsigned int controlId = (*action)->getActionControlId();
			
            if (controlId != SEPARATOR) // Don't add any separators obviously
            {
                IAlarmListObserver::Availability state = IAlarmListObserver::DISABLED;
                try
                {
                    if( (*action)->isAvailable(m_alarmRetriever))
                    {
                        state = IAlarmListObserver::ENABLED;
                    }
                }
                catch( const TA_Base_Core::AlarmListCtrlException& ale )
                {
                    std::ostringstream desc;
                    desc << "[AlarmListCtrl::updateObserverWithActionAvailability] Caught AlarmListCtrlException - " << ale.what();
                    LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", desc.str().c_str() );
                }
                catch( ... )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "[AlarmListCtrl::updateObserverWithActionAvailability] Caught unknown exception; Could not determine if action is available" );
                }
				
                itemAvailabilityMap[ controlId ] = state;
            }
            ++action;
        }
		
        m_observer->updateAvailableAlarmActions(itemAvailabilityMap);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "updateObserverWithActionAvailability out");
    }
	
	
    void AlarmListCtrl::updateObserverWithDescription(unsigned long selectedAlarmCount, bool valid)
    {
		FUNCTION_ENTRY("AlarmListCtrl::updateObserverWithDescription");
        if (m_observer == NULL)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No observer");
			FUNCTION_EXIT;
            return;
        }
		
        // If there are no alarms in the list
        if (!valid)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Selected Alarm details is empty");
            m_observer->setCurrentDescriptionText(NO_ALARMS_MATCH_FILTER_MSG);
			FUNCTION_EXIT;
            return;
        }
		
    /*    if (m_alarmStore == NULL || !m_alarmStore->isAlarmStoreValid())
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No alarm store message");
            m_observer->setCurrentDescriptionText(NO_ALARM_STORE_MSG);
        }
        else 
		*/
		if (m_alarmRetriever.isActionAlreadyRunning())
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Getting from alarm retriever");
			//marvin++
			std::string strDesc(m_alarmRetriever.getCurrentAction().c_str());
			//++marvin
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "setting description text");
            m_observer->setCurrentDescriptionText(strDesc.c_str());
        }
        else if (selectedAlarmCount == 0)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Selected alarm count zero");
            m_observer->setCurrentDescriptionText(NO_VALUE);
        }
        else if (selectedAlarmCount == 1)
        {
            // Set with the selected alarm's description
			std::vector<std::string> selectedAlarms = m_alarmDataSync.getSelectionWrapper()->getSelectedAlarms();
			if(selectedAlarms[0].size()==0)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No value selected");
                m_observer->setCurrentDescriptionText(NO_VALUE);
				FUNCTION_EXIT;
                return;
            }
					
            try
			{
				std::string strDesc;
				AlarmDataItem * pdataTemp = m_alarmRetriever.getAlarmItem(selectedAlarms[0]);
				if (pdataTemp != NULL)
				{
					std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarmItem(m_alarmRetriever.getAlarmItem(selectedAlarms[0]));
					strDesc = alarmItem->alarmDescription.c_str();
				}
				
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting current description");
				m_observer->setCurrentDescriptionText(strDesc.c_str());
                
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to get a description from it");
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No alarm store message");
                m_observer->setCurrentDescriptionText(NO_ALARM_STORE_MSG);
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve the alarm from the alarm store to get a description from it");
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No alarm store message");
                m_observer->setCurrentDescriptionText(NO_ALARM_STORE_MSG);
            }
        }
        else
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Alarm selected description");
            // Set with the current selected alarm count
            CString description;
            description.Format("%d alarms selected", selectedAlarmCount);
            m_observer->setCurrentDescriptionText(description);
        }
		FUNCTION_EXIT
    }
	
	
    void AlarmListCtrl::updateHeaderForSort(long column)
	{
		FUNCTION_ENTRY("updateHeaderForSort" );
		
        CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
		
        std::vector<long>::iterator matching = std::find(m_alarmColumnOrder.begin(), m_alarmColumnOrder.end(), column);
        if (matching != m_alarmColumnOrder.end())
        {
            // Only need to update the header if this is a visible column
            int columnIndex = std::distance(m_alarmColumnOrder.begin(), matching);
			
			//First find if the selected column is currently sorted on. If so, toggle 
			//the sorting arrangment. 
			
			HDITEM headerItem; 
			headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
            headerCtrl->GetItem(columnIndex, &headerItem); 
			if (headerItem.hbm != 0) 
			{ 
				DeleteObject(headerItem.hbm); 
				headerItem.hbm = 0; 
			} 
			headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 
			
            // If this is the same column we sorted on last time then now we want to sort in the opposite order
			if(m_sortColumn == column)
			{
                m_sortAscending = !m_sortAscending;
			}
			
            /// Load the bitmap based on whether we are sorting ascending or descending
			headerItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE((m_sortAscending) ? IDB_ASCENDING : IDB_DESCENDING), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 
			
            TA_ASSERT(headerItem.hbm != NULL, "The resource file bus/alarm/alarm_list_control/src/AlarmListCtrl.rc must be included in the application's rc file");
			
			headerCtrl->SetItem(columnIndex, &headerItem); 
        }
		
        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
        if(m_sortColumn != column) 
		{ 
            std::vector<long>::iterator matchingCol = std::find(m_alarmColumnOrder.begin(), m_alarmColumnOrder.end(), m_sortColumn);
            if (matchingCol == m_alarmColumnOrder.end())
            {
                // Column previously sorted on may not be a visible one
                m_sortColumn = column;
                FUNCTION_EXIT;
                return;
            }
			
            int prevSortColumnIndex = std::distance(m_alarmColumnOrder.begin(), matchingCol);
			
            HDITEM headerItem;
			headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
            headerCtrl->GetItem(prevSortColumnIndex, &headerItem); 
			headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
			if(headerItem.hbm != 0) 
			{ 
				DeleteObject(headerItem.hbm); 
				headerItem.hbm = 0; 
			} 
			headerCtrl->SetItem(prevSortColumnIndex, &headerItem); 
			m_sortColumn = column;
		}
		
        FUNCTION_EXIT;
	}
	
    void AlarmListCtrl::determinePrintColours( long index, unsigned long& foregroundColour, unsigned long& backgroundColour )
    {
		std::string strAlarmID;
		try
		{
			TA_Base_Core::IAlarmSeverityData::EColourType foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_FG1;
			TA_Base_Core::IAlarmSeverityData::EColourType backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG1;

			std::string strAlarmID = m_pDisplayVector->getItemString(index, ALARMID); 
			if (strAlarmID.empty() == true)
			{
				foregroundColour = DEFAULT_COLOUR;
				backgroundColour = BACKGROUND;
				return;
			}
			
            std::string ackedBy = m_pDisplayVector->getItemString(index, ALARM_ACKNOWLEDGE_BY);
			TA_Base_Core::AlarmState alarmState = (TA_Base_Core::AlarmState)m_pDisplayVector->getItemNumber(index, STATE);
			unsigned long ulAlarmSeverity = m_pDisplayVector->getItemNumber(index, ALARM_SEVERITY);
            if( ackedBy.empty() )
            {
                // Not Acknowledged
                if (alarmState == TA_Base_Core::AlarmOpen)
                {
                    // Open
					
                    // Use normal colours
                    foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_FG1;
                    backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_BG1;
                }
                else
                {
                    // Closed
					
                    // Use normal colours
                    foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_FG1;
                    backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_BG1;
                }
            }
            else
            {
                // Acknowledged
                if (alarmState == TA_Base_Core::AlarmOpen)
                {
                    // Open
					
                    // Use normal colours
                    foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_FG1;
                    backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG1;
                }
            }
			
            foregroundColour = m_databaseCache.getAlarmSeverityColour(ulAlarmSeverity, foregroundAlarmType);
            backgroundColour = m_databaseCache.getAlarmSeverityColour(ulAlarmSeverity, backgroundAlarmType);   
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not determine alarm severity of alarm to draw it in the correct colour");
			if (m_alarmDataSync.isAlarmItemSelected(strAlarmID)/*m_alarmSyn.isItemselected(pAlarm) */)
            {
				foregroundColour = BACKGROUND;
                backgroundColour = DEFAULT_COLOUR;
            }
            else
            {
				foregroundColour = DEFAULT_COLOUR;
                backgroundColour = BACKGROUND;
            }
        }
    }
	
    DWORD AlarmListCtrl::determineDrawingColours(ULONG ulIndex, unsigned long& foregroundColour, unsigned long& backgroundColour)
    {
		DWORD result = CDRF_SKIPDEFAULT;	
		try
        {
			//foregroundColour=RGB(255,255,255);
			//backgroundColour=foregroundColour;
			foregroundColour = DEFAULT_COLOUR;
            backgroundColour = BACKGROUND;
			TA_Base_Core::IAlarmSeverityData::EColourType foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_FG1;
			TA_Base_Core::IAlarmSeverityData::EColourType backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG1;
			
			std::string strAlarmID = m_pDisplayVector->getItemString(ulIndex, ALARMID);
			if (strAlarmID.size() == 0)
			{
				return result;
			}
       
            if( strcmp(m_pDisplayVector->getItemString(ulIndex, ALARM_ACKNOWLEDGE_BY).c_str(), "") == 0 )
            {
                // Not Acknowledged
                if (m_pDisplayVector->getItemNumber(ulIndex, STATE) == TA_Base_Core::AlarmOpen)
                {
                    // Open
                    if (m_isFlashTime)
                    {
                        // Use flash colour
						if (m_alarmDataSync.isAlarmItemSelected(strAlarmID))
                        {
						// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG2;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG2;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_FG2;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_BG2;
                        }
                    }
                    else
                    {
                        // Use non-flash colour
                       	if (m_alarmDataSync.isAlarmItemSelected(strAlarmID))
                        {
							// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG1;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG1;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_FG1;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_BG1;
                        }
                    }
                    
                }
                else
                {
                    // Closed
                    if (m_isFlashTime)
                    {
                        // Use flash colour
						if (m_alarmDataSync.isAlarmItemSelected(strAlarmID))
                        {
							// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG2;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG2;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_FG2;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_BG2;
                        }
                    }
                    else
                    {
                        // Use non-flash colour
                        if (m_alarmDataSync.isAlarmItemSelected(strAlarmID))
                        {
							// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG1;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG1;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_FG1;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::CLOSED_UNACKED_BG1;
                        }
                    }
                }
            }
            else
            {
                // Acknowledged
                if (m_pDisplayVector->getItemNumber(ulIndex, STATE) == TA_Base_Core::AlarmOpen)
                {
                    // Open
                    if (m_isFlashTime)
                    {
                        // Use flash colour
						if (m_alarmDataSync.isAlarmItemSelected(strAlarmID) )
                        {
						// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_ACKED_FG2;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_ACKED_BG2;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_FG2;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG2;
                        }
                    }
                    else
                    {
                        // Use non-flash colour
                       	if ( m_alarmDataSync.isAlarmItemSelected(strAlarmID) )
                        {
						// Use selection colours
							foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_ACKED_FG1;
							backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::SELECTED_OPEN_ACKED_BG1;
							//result = CDRF_SKIPDEFAULT;
                        }
                        else
                        {
                            // Use normal colours
                            foregroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_FG1;
                            backgroundAlarmType = TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG1;
                        }
                    }
                }
            }
			
            foregroundColour = m_databaseCache.getAlarmSeverityColour(m_pDisplayVector->getItemNumber(ulIndex, ALARM_SEVERITY), foregroundAlarmType);
            backgroundColour = m_databaseCache.getAlarmSeverityColour(m_pDisplayVector->getItemNumber(ulIndex, ALARM_SEVERITY), backgroundAlarmType);
			// delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not determine alarm severity of alarm to draw it in the correct colour");
            /*if (m_alarms[index].isSelected)
            {
				foregroundColour = BACKGROUND;
                backgroundColour = DEFAULT_COLOUR;
            }
            else
            {
				foregroundColour = DEFAULT_COLOUR;
                backgroundColour = BACKGROUND;
            }*/
        }
		return result;
    }
	
	
    unsigned long AlarmListCtrl::getEarliestAlarmTime()
    {
        // Get Alarm count
//         TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo details;
//         details.totalAlarms = 0;
//         details.unackAlarms = 0;
//         details.highestSeverity = 0;
//         details.earliestSourceTime.time = 0;
//         details.earliestSourceTime.millitm = 0;
//         details.latestSourceTime.time = 0;
//         details.latestSourceTime.millitm = 0;
        try
        {
			//chenlei-+
            //details = m_alarmStore->getAlarmDetails();
            //return details.earliestSourceTime.time;
			//++Noel
			//return m_alarmSyn.getIndexinfo().earliestSourceTime.time;
			return m_info.earliestSourceTime.time;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve alarm details");
        }
		
        return 0;
    }
	
    unsigned long AlarmListCtrl::getLatestAlarmTime()
    {
        // Get Alarm count
//         TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo details;
//         details.totalAlarms = 0;
//         details.unackAlarms = 0;
//         details.highestSeverity = 0;
//         details.earliestSourceTime.time = 0;
//         details.earliestSourceTime.millitm = 0;
//         details.latestSourceTime.time = 0;
//         details.latestSourceTime.millitm = 0;
        try
        {
			//chenlei-+
            //details = m_alarmStore->getAlarmDetails();
            //return details.latestSourceTime.time;
//++noel
//			return m_alarmSyn.getIndexinfo().latestSourceTime.time;
			return m_info.latestSourceTime.time;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve alarm details");
        }
		
        return 0;
    }
	
    /////////////////////////////////////////////////////////////////////////////
    // AlarmListCtrl message handlers
    /////////////////////////////////////////////////////////////////////////////
	
	
    void AlarmListCtrl::onGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
    {
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
		//Create a pointer to the item
		
		LV_ITEM* pItem= &(pDispInfo)->item;
		
		// Code block is remove because onCustomDrawAlarmList function will redraw the 
		// the entire list view.

		if (pItem->mask & LVIF_TEXT)
		{
			int item = pDispInfo->item.iItem;
            int subItem = pDispInfo->item.iSubItem;
            
            CString text = getItemText(item, subItem);
			
			int maxLen = pDispInfo->item.cchTextMax-1;
			int	text_length	= text.GetLength();
			if (text_length > maxLen)
			{
				//text = str.substr(0, maxLen);
				text_length = maxLen;
			}
			text_length = std::min(text_length, pDispInfo->item.cchTextMax);
			
			memset(pDispInfo->item.pszText, 0, pDispInfo->item.cchTextMax);
			memcpy(pDispInfo->item.pszText, text, text_length);
		}
		
// 		if( pItem->mask & LVIF_IMAGE) 
// 		{
// 			pItem->mask |= LVIF_STATE; // Enable the state mask
// 			pItem->state = CDIS_DEFAULT; //initial state Unselected
// 			pItem->stateMask = CDIS_DEFAULT;
// 		}
// 		
// 		if (pItem->mask & LVIF_STATE)
// 		{
// 			pItem->state = CDIS_DEFAULT; //initial state Unselected
// 			pItem->stateMask = CDIS_DEFAULT;
// 			
// 			int itemid = pItem->iItem;
// 						
// 			if(m_alarmDataSync.isAlarmItemSelected(m_pDisplayVector->getItemString(itemid, ALARMID)))
// 			{
// 				pItem->state = LVIS_SELECTED; 
// 				pItem->stateMask = LVIS_SELECTED;
// 			}
// 		}

		*pResult = 0;
    }
	
	
    void AlarmListCtrl::onOdCacheHint(NMHDR* pNMHDR, LRESULT* pResult) 
    {
		NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
		
		ULONG ulIdxFrom = GetTopIndex();
		ULONG ulCountPerPage = GetCountPerPage();
		ULONG ulIdxTo = ulIdxFrom + ulCountPerPage;
		
// 		ULONG ulDataCount = ulCountPerPage;
// 		
// 		if ((ulIdxFrom < m_ulIndexBegin) || (ulIdxFrom+ulCountPerPage > m_ulIndexEnd))
// 		{
// 			long lDiff = 0;
// 			m_ulIndexBegin = ulIdxFrom;
// 			if (ulIdxFrom > 0)
// 			{
// 				lDiff = ulIdxFrom - ulCountPerPage;
// 				if (lDiff > 0)
// 				{
// 					m_ulIndexBegin = lDiff;
// 					ulDataCount += ulCountPerPage; // add the previous page count
// 				}
// 				else 
// 				{
// 					// if the difference less than or equal to 0 then the previous page count 
// 					// is equal to the TopIndex value and the beginning index is always 0
// 					m_ulIndexBegin = 0;
// 					ulDataCount += ulIdxFrom; 
// 				}
// 			}
// 			// add the next page buffer //
// 			ulDataCount += ulCountPerPage;
// 			m_ulIndexEnd = m_ulIndexBegin + ulDataCount;
// 			ThreadGuard guard(m_updateDisplayLock);
// 			_setListDisplayInfo();
// 			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "onOdCacheHint: From = %ld -> To = %ld.", pCacheHint->iFrom, pCacheHint->iTo);
// 
// 		}
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "onOdCacheHint: From = %ld -> To = %ld.", pCacheHint->iFrom, pCacheHint->iTo);
		if (ulIdxFrom != m_ulIdxFrom || ulIdxTo != m_ulIdxTo)
		{			
			m_ulIdxFrom = ulIdxFrom;
			m_ulIdxTo = ulIdxTo;

			_setListDisplayInfo();
			Invalidate();
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "onOdCacheHint: From = %ld -> To = %ld.", pCacheHint->iFrom, pCacheHint->iTo);
		}

		*pResult = 0;
    }
	
    void AlarmListCtrl::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onColumnclick");
		
		//	if (m_isLoadingAlarms) return;

		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		
        if (!m_canColumnsBeSorted)
        {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Columns cannot be sorted");
		return;
        }
		SetItemState(-1, 0, LVIS_SELECTED);
		//++Noel 
		m_alarmDataSync.unSelectAllAlarmItem(); // un-select all alarm items to make sure 
		// Reset selection information aside from m_nCurSelPos
		m_isRangeSel = false;
		m_rangeSelInfo.lSelEnd = 0;
		m_rangeSelInfo.lSelStart = 0;
		m_nSelDirection = 0;
		m_strSelectedAlarmID = ""; //reset selected item
		updateObserverWithDescription(0, true);
		updateObserverWithActionAvailability();

        int column = pNMListView->iSubItem;
        
        if (column < 0 || column >= static_cast<int>(m_alarmColumnOrder.size()))
        {
            // Cannot change sort as the column is invalid
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Column clicked was invalid");
            FUNCTION_EXIT;
            return;
        }
		
        int columnId = m_alarmColumnOrder[column];
        std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(columnId);
        if (matching == m_alarmColumns.end())
        {
            // not a relevant column
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Alarm sort column %d selected is not valid", column);
            FUNCTION_EXIT;
            return;
        }
		
		// TD 12710 Stop the store from updating
		//		m_alarmStore->clearQueueChanges();
        updateHeaderForSort(columnId);
		//m_pDisplayVector->setSortAccending(m_sortAscending);
		{
			//ThreadGuard guard(m_updateDisplayLock);
			if(m_curSortCol != matching->second->getSortColumn())
			{	
				m_curSortCol = matching->second->getSortColumn(); 
			}
		}
		
		// Update Sorting 
		m_alarmDataSync.setDisplaySort(m_curSortCol, m_sortAscending);

		m_isLastActionSort = true;
		//	m_alarmStore->startUpdating();
		Invalidate();
		*pResult = 0;
        FUNCTION_EXIT;
    }
	
	
    void AlarmListCtrl::onDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onDblclk");
		
		if (m_isLoadingAlarms)
        {
            return;
        }
		
        if (m_alarmStore == NULL || !m_alarmStore->isAlarmStoreValid() )
        {
            *pResult = 0;
            FUNCTION_EXIT;
            return;
        }
        
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		
        int item = pNMListView->iItem;
        if (item < 0 || item >= static_cast<int>(m_pDisplayVector->getSize()))
        {
            *pResult = 0;
            FUNCTION_EXIT;
            return;
        }
		
		
        int subItem = pNMListView->iSubItem;
        if (subItem < 0 || subItem >= static_cast<int>(m_alarmColumnOrder.size()))
        {
            *pResult = 0;
            FUNCTION_EXIT;
            return;
        }
		
        int columnId = m_alarmColumnOrder[subItem];
        std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(columnId);
        
        if (matching == m_alarmColumns.end())
        {
            *pResult = 0;
            FUNCTION_EXIT;
            return;
        }
		
		
        if (matching->second->getColumnDoubleClickAction() == NULL)
        {
            // No action specified for this column
            *pResult = 0;
            FUNCTION_EXIT;
            return;
        }
		
		
        // Now if this action is available, do it.
        m_alarmRetriever.clearCache();
		
        try
        {
            if (matching->second->getColumnDoubleClickAction()->isAvailable(m_alarmRetriever) )
            {
                CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
                thread->PostThreadMessage(ActionThread::PERFORM_ACTION,
                    reinterpret_cast<WPARAM>(matching->second->getColumnDoubleClickAction()),
                    reinterpret_cast<LPARAM>(&m_alarmRetriever) );
            }
        }
        catch( const TA_Base_Core::AlarmActionException& ex )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString actionName;
            unsigned int errorId;
            
            switch (ex.getFailType())
            {
            case AlarmActionException::CANNOT_FIND_ALARM:
                errorId = IDS_UE_040027;
                break;
				
            case AlarmActionException::CANNOT_UPDATE_ALARM_COMMENTS:
                errorId = IDS_UE_040023;
                actionName.LoadString(IDS_COMMENT);
                break;
				
            case AlarmActionException::CANNOT_ACKNOWLEDGE_ALARMS:
                errorId = IDS_UE_040023;
                actionName.LoadString(IDS_ACKNOWLEDGE);
                break;
				
            case AlarmActionException::CANNOT_ACKNOWLEDGE_ALARM:
                errorId = IDS_UE_040025;
                break;
				
            case AlarmActionException::CANNOT_ACKNOWLEDGE_AND_CLOSE_ALARMS:
                errorId = IDS_UE_040023;
                actionName.LoadString(IDS_ACKNOWLEDGE_AND_CLOSE);
                break;
				
            case AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN:
                errorId = IDS_UE_040008;
                break;
                
            case AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN:
                errorId = IDS_UE_040024;
                actionName.LoadString(IDS_RUN);
                break;
				
            case AlarmActionException::CANNOT_POSTPONE_PLAN:
                errorId = IDS_UE_040024;
                actionName.LoadString(IDS_POSTPONE);
                break;
				
            case AlarmActionException::CANNOT_LAUNCH_SCHEMATIC:
                errorId = IDS_UE_010002;
                actionName.LoadString(IDS_SCHEMATIC);
                break;
				
            case AlarmActionException::CANNOT_LAUNCH_REQUEST_MANAGER:
                errorId = IDS_UE_010002;
                actionName.LoadString(IDS_REQUEST_MANAGER);
                break;
				
            case AlarmActionException::CANNOT_LAUNCH_EVENT_VIEWER:
                errorId = IDS_UE_010002;
                actionName.LoadString(IDS_EVENT_VIEWER);
                break;
				
            case AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL:
                errorId = IDS_UE_010002;
                actionName.LoadString(IDS_INSPECTOR_PANEL);
                break;
				
            case AlarmActionException::CANNOT_LAUNCH_TREND_VIEWER:
                errorId = IDS_UE_010002;
                actionName.LoadString(IDS_TREND_VIEWER);
                break;
				
            default:
				break;
            }
            
            if (!actionName.IsEmpty())
            {
                userMsg << actionName;
            }
            
            userMsg.showMsgBox(errorId);
        }
        catch( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", te.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception in AlarmListCtrl::onDblclk" );
        }
		
        *pResult = 0;
    }
	
	
    void AlarmListCtrl::onRightClick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onRightClick");
		
		if (m_isLoadingAlarms)
        {
            return;
        }
		
		if (m_alarmStore == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Alarm Store was null so context menu cannot be displayed");
            FUNCTION_EXIT;
            return;
        }
		
        // Get the point where the user clicked
        CPoint pt(GetMessagePos());
		
        TA_ASSERT(m_alarmActionFactory != NULL, "The alarm action factory has not been set. Has startAlarmList() been called");
        if (m_alarmActionFactory->getAlarmActions().empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No alarm actions so context menu will not be displayed");
            FUNCTION_EXIT;
            return;
        }
		
        CMenu topMenu;
        topMenu.LoadMenu(IDR_MAIN_MENU);
		if( topMenu == NULL )
		{
			return;
		}
		
		// Find the sub-menu specified by subMenuID
		CMenu* viewMenu = topMenu.GetSubMenu(0);
        if (viewMenu == NULL)
        {
            return;
        }
		
        // Remove the placeholder off the menu
        viewMenu->RemoveMenu(0, MF_BYPOSITION);
		
        m_alarmRetriever.clearCache();
		
        // Get all the actions from the factory and add them in
        for (AlarmActionFactory::AlarmActionList::const_iterator itAlarmActions = m_alarmActionFactory->getAlarmActions().begin();
		itAlarmActions != m_alarmActionFactory->getAlarmActions().end();
		++itAlarmActions )
        {
            try
            {
                UINT controlId = (*itAlarmActions)->getActionControlId();
                if (controlId == SEPARATOR)
                {
                    viewMenu->AppendMenu( MF_SEPARATOR );
                }
				else if ( controlId == INHIBIT_ALARM )
				{
					if ( (*itAlarmActions)->isAvailableInContextMenu() )
					{
						CMenu inhibitMenu;
						VERIFY( inhibitMenu.CreatePopupMenu() );
						ActionSuppressAlarm* suppressAction = dynamic_cast<ActionSuppressAlarm*> (*itAlarmActions);
						std::map<unsigned int, std::string> subControlIds = suppressAction->getSubActionControlIds();
						
						if (subControlIds.size() > 0)
						{
							std::map<unsigned int, std::string>::iterator subItr = subControlIds.begin();
							for (subItr; subItr!=subControlIds.end(); subItr++)
							{
								inhibitMenu.AppendMenu( MF_ENABLED, subItr->first, subItr->second.c_str() );
							}
						}
						viewMenu->AppendMenu(MF_POPUP, (UINT)inhibitMenu.m_hMenu, (*itAlarmActions)->getMenuEntry().c_str() );
						
						if( !(*itAlarmActions)->isAvailable(m_alarmRetriever) )
						{
							viewMenu->EnableMenuItem((UINT)inhibitMenu.m_hMenu, MF_BYCOMMAND|MF_GRAYED );
							std::map<unsigned int, std::string>::iterator subItr = subControlIds.begin();
							for (subItr; subItr!=subControlIds.end(); subItr++)
							{
								viewMenu->EnableMenuItem(subItr->first, MF_BYCOMMAND|MF_GRAYED);
							}
						}
					}
				}
                else if( (*itAlarmActions)->isAvailableInContextMenu() )
                {
                    viewMenu->AppendMenu( MF_ENABLED, controlId, (*itAlarmActions)->getMenuEntry().c_str());
                    if( !(*itAlarmActions)->isAvailable(m_alarmRetriever) )
                    {
                        viewMenu->EnableMenuItem((*itAlarmActions)->getActionControlId(), MF_BYCOMMAND|MF_GRAYED );
                    }
                }
            }
            catch( const TA_Base_Core::AlarmListCtrlException& ale )
            {
                std::ostringstream desc;
                desc << "[AlarmListCtrl::onRightClick] Caught AlarmListCtrlException - " << ale.what();
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", desc.str().c_str() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "[AlarmListCtrl::onRightClick] Caught unknown exception" );
            }
        }
		
		viewMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x, pt.y, this);
		
		*pResult = 0;
    }
	
	
    void AlarmListCtrl::onContextMenuItemSelected(UINT nID)
    {
        FUNCTION_ENTRY("onContextMenuItemSelected");
		
        try
        {
            executeAction(nID);
        }
        catch(const TA_Base_Core::TransactiveException& te)
        {
            std::ostringstream desc;
            desc << "Caught exception executing action " << nID << "; Reason: " << te.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str() );
        }
        catch(...)
        {
            std::ostringstream desc;
            desc << "[AlarmListCtrl::onContextMenuItemSelected] Caught unknown exception; nID: " << nID;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str().c_str() );
        }
		
        FUNCTION_EXIT;
    }
	
	
    void AlarmListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
    {
		if (m_isLoadingAlarms) return;
		
        if (nChar == VK_ESCAPE)
        {
            // This means we want to unselect any items that are currently selected
            POSITION pos = GetFirstSelectedItemPosition();
            while (pos != NULL)
            {
                int selected = GetNextSelectedItem(pos); // This increments pos
                SetItemState(selected,0, LVIS_SELECTED); // Unselect the item
            }
        }
		
		CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
    }
	
	
    BOOL AlarmListCtrl::OnEraseBkgnd(CDC* pDC) 
    {
		CRect rc;
        CRect hiddenHdRect, rcH;
		GetClientRect(&rc);
        
		CHeaderCtrl* pHC = GetHeaderCtrl();
        if(pHC != NULL)
        {
            pHC->GetItemRect(0, &rcH);
			// Reset the rect of the hidden column 0
			hiddenHdRect.top = rcH.top-3;
			hiddenHdRect.bottom = rcH.bottom+3;
			hiddenHdRect.right = rcH.right+5;
			hiddenHdRect.left=rcH.left-3;
		}
		bool blnDisplayIsEmpty=false;

		{	
			//fix alarm store hang issue, m_displayVec is handled by mul-threads, so it should be locked.
			//ThreadGuard guard(m_updateDisplayLock);
			blnDisplayIsEmpty= m_pDisplayVector->getSize() == 0? true:false;
		}
        
		if (blnDisplayIsEmpty)
        {
            // No items so draw the entire region
            pDC->FillSolidRect(&rc, BACKGROUND);  // or whatever colour the background needs to be
			
            // If the alarm store is invalid we want to display some text indicating this.
            if ( m_alarmStore == NULL || !m_alarmStore->isAlarmStoreValid() )
            {
                int nSavedDC = pDC->SaveDC();  //save the current DC state      
				
                //If there is a header, we need to account for the space it occupies
                if(pHC != NULL)
                {
                    rc.top += rcH.bottom;
                }
				
                //Now we actually display the text        
                pDC->SetTextColor(DEFAULT_COLOUR);  //set the text color        
                pDC->SetBkColor(BACKGROUND);        //set the background color        
				
                CFont newFont;                          // Select a font  
				newFont.CreatePointFont(110, "宋体");
				pDC->SelectObject(newFont);
				
               	//TD17081++ - when alarms are not yet loaded, display appropriate messsage to the user           					
				CString alarmMsg = NO_ALARM_STORE_MSG;
				if (NULL == m_alarmStore) 
				{ 
					alarmMsg = ALARMS_RETRIEVAL_INPROGRESS_MSG; 
				}
				else if (m_info.totalAlarms == 0)
				{
					alarmMsg = NO_ALARMS_MATCH_FILTER_MSG;
				}
				
                //pDC->DrawText(NO_ALARM_STORE_MSG,
				pDC->DrawText(alarmMsg,
					&rc,
					DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP); //and draw the text        
               	//++TD17081
                // Restore dc
                pDC->RestoreDC(nSavedDC);
                ReleaseDC(pDC);
				
                newFont.DeleteObject();
            }
        }
        else
        {
            // Draw around the items for maximum smoothness
            RECT firstItemPos;
            GetItemRect(GetTopIndex(),&firstItemPos,LVIR_BOUNDS );
			
            // Fill above the items
            pDC->FillSolidRect(rc.left, rc.top, rc.Width(), firstItemPos.top - rc.top, BACKGROUND);
			
            // TD 3354: Removed the black line drawn to the left of the left-most column.
            // Fill to the left of the items
            // pDC->FillSolidRect(rc.left,rc.top, firstItemPos.left - rc.left, rc.Height(), BACKGROUND);
			
            // Fill to the right of the items
            RECT fullRowPos;
            GetItemRect(GetTopIndex(), &fullRowPos, LVIR_BOUNDS);
            pDC->FillSolidRect(fullRowPos.right, rc.top, rc.right - fullRowPos.right, rc.Height(), BACKGROUND);
			
            // Fill below the last item
            int bottomItem = GetTopIndex() + GetCountPerPage();
            if (bottomItem >= GetItemCount())
            {
                bottomItem = GetItemCount() - 1;
            }
            RECT lastItemPos;
            GetItemRect(bottomItem,&lastItemPos,LVIR_BOUNDS);
			
            pDC->FillSolidRect(rc.left, lastItemPos.bottom, rc.Width(), rc.bottom - lastItemPos.bottom, BACKGROUND);
			
			// Reset the rect of the hidden Column 0 below header
			m_hiddenColRect.top = hiddenHdRect.bottom;
			m_hiddenColRect.bottom = lastItemPos.bottom;
			m_hiddenColRect.left = hiddenHdRect.left;
			m_hiddenColRect.right = hiddenHdRect.right;
        }
		
		return TRUE;
    }
	
	
	void AlarmListCtrl::onCustomdrawAlarmList(NMHDR* pNMHDR, LRESULT* pResult)
	{
		*pResult = 0;
		
		LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
		//int iRow = lplvcd->nmcd.dwItemSpec;
	
		switch(lplvcd->nmcd.dwDrawStage)
		{
		case CDDS_PREPAINT :
			{
//				_setListDisplayInfo();

				*pResult = CDRF_NOTIFYITEMDRAW;
				return;
			}
			
			// Modify item text and or background
		case CDDS_ITEMPREPAINT:
			{
 				DWORD row=lplvcd->nmcd.dwItemSpec;
				
				// Get the drawing color for the entire row
				determineDrawingColours(row, lplvcd->clrText, lplvcd->clrTextBk);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Determine drawing colors of index %ld", row);
				
				//++Noel
				// Notify custom drawing per sub item
				*pResult = CDRF_NOTIFYSUBITEMDRAW; //CDRF_NEWFONT; //

				//lplvcd->clrText = RGB(0,0,0);
				// If you want the sub items the same as the item,
				// set *pResult to CDRF_NEWFONT
								
				return;
			}
		case CDDS_ITEMPREERASE:
			{
				*pResult = CDRF_NOTIFYSUBITEMDRAW;
				return;
			}
			// Modify sub item text and/or background
		case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
			{
				DWORD row=lplvcd->nmcd.dwItemSpec;
				// no need to query the drawing color because we already got the drawing color
				// in the CDDS_ITEMPREPAINT stage.
				//determineDrawingColours(row, lplvcd->clrText, lplvcd->clrTextBk);

				*pResult = CDRF_SKIPDEFAULT;

				DWORD col=lplvcd->iSubItem;
				if(col == 0)
					return;
				CString str=getItemText(row,col);
				CDC* pDC;
				CRect rect;
				pDC = CDC::FromHandle(lplvcd->nmcd.hdc);
				CBrush brush(lplvcd->clrTextBk);
				GetSubItemRect(row,col,LVIR_BOUNDS,rect);
				
				if (col != 1)
					rect.left=rect.left+1;
				rect.bottom=rect.bottom-1;
				pDC->FillRect(&rect,&brush);

					 
					CFont font;
					font.CreateFont(16, 0, 0, 0, FW_THIN, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,"宋体");
					HGDIOBJ  objF= pDC->SelectObject(&font);

				int columnId = m_alarmColumnOrder[col];
				std::map<long, IAlarmColumn*>::iterator matching = m_alarmColumns.find(columnId);
				if(matching->second->getColumnIdentifier() == TA_Base_Bus::COL_SEVERITY)
				{
					pDC->DrawText(str, &rect,DT_CENTER|DT_END_ELLIPSIS);
				}
				else
				{	
					//str.Insert(0, " ");
					rect.OffsetRect(6,0);
					rect.right -= 6;
					//[CL-20828] display "&" wrong, LuoHuiRong 03-09-2009
					pDC->DrawText(str,&rect,DT_END_ELLIPSIS|DT_NOPREFIX);
				}
				//++Noel
				if (m_alarmDataSync.isAlarmItemSelected(m_pDisplayVector->getItemString(row, ALARMID)))
				{
					COLORREF polor = RGB(0,0,255);
					GetItemRect(row,rect,LVIR_BOUNDS);
					rect.bottom -= 1;
					CBrush brush1(polor);
					pDC->FrameRect(&rect,&brush1);
					}
					pDC->SelectObject(&objF);
					font.DeleteObject();
				return;
			}
		}

	}
	
	
    void AlarmListCtrl::OnDestroy() 
    {
        FUNCTION_ENTRY("OnDestroy");
		
		CListCtrl::OnDestroy();
		
		//TD15765--
        //m_ToolTip.Detach();
		//--TD15765
		
        KillTimer(UPDATE_TIMER);
		
        // We do this here rather than the destructor as we want to stop updates from the alarm store.
        if (m_alarmStore != NULL)
        {
            delete m_alarmStore;
            m_alarmStore = NULL;
        }
		
		// delete the selected alarm if not empty
// 		if (m_selectedAlarms.empty() == false)
// 		{
// 			for (std::vector<std::string*>::iterator itr=m_selectedAlarms.begin(); itr!=m_selectedAlarms.end(); /*itr++*/)
// 			{
// 				std::string* selAlarm = *itr;
// 				itr = m_selectedAlarms.erase(itr);	// must get return iterator
// 				delete selAlarm;
// 				selAlarm = NULL;
// 			}
// 		}
// 		m_selectedAlarms.clear();
		
        FUNCTION_EXIT;
    }
	
    void AlarmListCtrl::PreSubclassWindow()
    {
		EnableToolTips();
		CListCtrl::PreSubclassWindow();

		//TD15765++
        //HWND toolTipHandle = reinterpret_cast<HWND>(SendMessage(LVM_GETTOOLTIPS,0,0));
        //m_ToolTip.Attach(toolTipHandle);
        //m_ToolTip.SetDelayTime(TTDT_AUTOPOP,86400000);
        //m_ToolTip.SetMaxTipWidth(100);
		//++TD15765
    }
	
	//TD15765++
	int AlarmListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		// Determine the coordinates of the cell.
		int row, col;
		RECT cellrect;
		row = CellRectFromPoint(point, &cellrect, &col );
		
		if ( row == -1 )
		{
			return -1;
		}
		
		// Encode the cell coordinates the ID.
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = cellrect;
		
		return pTI->uId;
	}
	
	int AlarmListCtrl::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		int colnum;
		
		// Make sure that the ListView is in LVS_REPORT
		if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		{
			return -1;
		}
		
		// Get the top and bottom row visible
		int row = GetTopIndex();
		int bottom = row + GetCountPerPage();
		if ( bottom > GetItemCount() )
		{
			bottom = GetItemCount();
		}
		
		// Get the number of columns
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColumnCount = pHeader->GetItemCount();
		
		// Loop through the visible rows
		for ( ; row<=bottom; row++ )
		{
			// Get bounding rect of item and check whether point falls in it.
			CRect rect;
			GetItemRect( row, &rect, LVIR_BOUNDS );
			if ( rect.PtInRect(point) )
			{
				// Now find the column
				for ( colnum = 0; colnum < nColumnCount; colnum++ )
				{
					int colwidth = GetColumnWidth(colnum);
					if ( ( point.x >= rect.left ) && ( point.x <= (rect.left + colwidth ) ) )
					{
						RECT rectClient;
						GetClientRect( &rectClient );
						if ( col ) *col = colnum;
						rect.right = rect.left + colwidth;
						
						// Make sure that the right extent does not exceed the client area
						if( rect.right > rectClient.right )
						{
							rect.right = rectClient.right;
						}
						*cellrect = rect;
						return row;
					}
					rect.left += colwidth;
				}
			}
		}
		return -1;
	}
	
	BOOL AlarmListCtrl::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		UINT nID = pNMHDR->idFrom;
		
		*pResult = 0;
		
		// We want to disable automatic tooltips completely.
		if( 0 == nID )
		{
			return TRUE;
		}
		
		// I want to implement this in PreSubclassWindow(), but it crashes.
		if(!m_bToolTipCtrlCustomizeDone)
		{
#if _MFC_VER >= 0x0700 // MFC Version 7 and above
			AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
#else
			_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#endif
			CToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
			
			// Set max tip width in pixel.
			// you can change delay time, tip text or background color also. enjoy yourself!
			pToolTip->SetMaxTipWidth(500);
			pToolTip->SetDelayTime(TTDT_AUTOPOP,86400000);//TD16138
			m_bToolTipCtrlCustomizeDone = TRUE;
		}
		
		// need to handle both ANSI and UNICODE versions of the message
		TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
		TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
		
		// Extract the row and column numbers.
		int row = ((nID-1) >> 10) & 0x3fffff ;
		int col = (nID-1) & 0x3ff;
		CString strTip;
		strTip = GetItemText( row, col );
		
		//TD16138++ wangyn
		int columnLength = GetColumnWidth(col);
		
		int textWidth = GetStringWidth(strTip.GetBuffer(0)) + 12;
		
		if(columnLength >= textWidth)
		{
			return FALSE;
		}
		//++TD16138
		// Convert the text as required.
		if(pNMHDR->code == TTN_NEEDTEXTA)
		{
			m_pchTip = std::auto_ptr<TCHAR>( new TCHAR[strTip.GetLength()+1] );
			lstrcpyn( m_pchTip.get(), strTip, strTip.GetLength() );
			m_pchTip.get()[strTip.GetLength()] = NULL;
			pTTTW->lpszText = (WCHAR*)m_pchTip.get();
		}
		else
		{
			m_pwchTip = std::auto_ptr<WCHAR>( new WCHAR[strTip.GetLength()+1] );
			_mbstowcsz( m_pwchTip.get(), strTip, strTip.GetLength() );
			m_pwchTip.get()[strTip.GetLength()] = NULL;
			pTTTW->lpszText = (WCHAR*)m_pwchTip.get();
		}
		
		return TRUE;
	}
	//TD15765++
	
	void AlarmListCtrl::OnMouseMove( UINT nFlags, CPoint point )
	{
		// Set the cursor to wait ie. hour glass if the alarms are still loading
		if (m_isLoadingAlarms) 
		{ 
			BeginWaitCursor();
		}
		else 
		{
			EndWaitCursor();
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		
		m_hiddenColRect.NormalizeRect();
		if ( m_hiddenColRect.IsRectEmpty() == 0 && m_hiddenColRect.PtInRect(point) ) return;
		
		CWnd::OnMouseMove(nFlags, point);
	}
	
	void AlarmListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		// TODO: Add your message handler code here and/or call default
		if (m_isLoadingAlarms) 
		{
			BeginWaitCursor();
			return;
		}
		else
		{
			EndWaitCursor();
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		
		CWnd::OnLButtonDown(nFlags, point);
	}
	
	
    LRESULT AlarmListCtrl::OnPerformPrefilterUpdate( WPARAM wParam, LPARAM lParam )
    {
        if (m_alarmStore != NULL)
        {
            // TD12019: Extract the prefilter update information from the first parameter and
            //          trigger the alarm store to update its prefilter using the provided info.
            std::auto_ptr<PrefilterUpdate> prefilterUpdate( reinterpret_cast<PrefilterUpdate*>(wParam) );
            if ( NULL != prefilterUpdate.get() )
            {
                m_alarmStore->updatePreFilterLocationsAndSession( prefilterUpdate->preFilter,
					prefilterUpdate->locations,
					prefilterUpdate->sessionID );
            }
        }
		
        return 0;
    }
	
	//TD17081++
	void AlarmListCtrl::startAlarmList( AlarmActionFactory& alarmActionFactory,
		const Filter& preFilter,
		const std::vector<unsigned long>& locations,
		const Filter& filter,
		const std::string& sessionId,
		bool isHighestSeverityNeeded)
    {
        FUNCTION_ENTRY("startAlarmList");
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Setting up the alarm retriever and alarm action factory.");
		
		m_loadedSelectedAlarms = -1;
        m_alarmActionFactory = &alarmActionFactory;
		
        // Create the alarm store so we can get all our alarms to add
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating the alarm store");
        try
        {
            m_alarmStore = new AlarmStore(preFilter, locations, *m_defaultSort, filter, sessionId, isHighestSeverityNeeded);
            
            if (m_alarmStore == NULL)
            {
                TA_THROW( TA_Base_Core::AlarmListCtrlException("Alarm Store could not be constructed. New failed") );
            }
			
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not initialise the alarm store.");
            //UE-040022
            TA_THROW( TA_Base_Core::AlarmListCtrlException("The alarm list could not be displayed because the database configuration is invalid.") );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not initialise the alarm store");
            //UE-040022
            TA_THROW( TA_Base_Core::AlarmListCtrlException("The alarm list could not be displayed because the database configuration is invalid.") );
        }
		
		
        if( m_pDisplayVector->getSize() == 0 && 
            (m_alarmStore != NULL && !m_alarmStore->isAlarmStoreValid()) )
        {
            // This means we have an invalid alarm store
            // Set the number of items to 1 so we can display an error message in the list control
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Indicating the alarm store is not yet initialised");
        }
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Starting timers for flashing and updating");
		
        // Set up a timer so we know when to update the list and make it flash
//		CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
//		thread->PostThreadMessage(ActionThread::PERFORM_PREACTION,
//			reinterpret_cast<WPARAM>(this),NULL);
		SetTimer(UPDATE_TIMER, UPDATE_SLEEP_IN_MILLISECONDS, NULL);

// 		m_alarmDataSync.setDisplaySort(m_curSortCol);
// 		m_pDisplayVector->setSortAccending(m_sortAscending);
// 		m_observer->setCurrentDescriptionText(ALARMS_RETRIEVAL_INPROGRESS_MSG);
		
		m_alarmDataSync.setStoreService(m_alarmStore);
		m_alarmDataSync.setTarget(m_hWnd);
		m_alarmDataSync.initialize();
        FUNCTION_EXIT;
    }
	
	
	void AlarmListCtrl::setupAlarmList(  IAlarmListObserver& observer, 
		const std::vector<IAlarmColumn*> &columns, 
		long sortColumn,
		bool isAscendingSort)
	{
		FUNCTION_ENTRY("setupAlarmList");

		m_font.CreatePointFont(110,"宋体");
		SetFont(&m_font,FALSE);

		m_observer = &observer;
		m_alarmRetriever.setAlarmListCtrl(*this); 
		m_alarmRetriever.setSelectionRetriever(m_alarmDataSync.getSelectionWrapper());
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting extended styles of list");
		// TD #11289
		// only allow reordering of columns by drag & dropping if the flag has been set
		
		if(m_canColumnsBeReordered)
		{
			if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_NOGRIDLINES.c_str()) )
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES);
			}
			else
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
			}
			
		}
		else
		{
			if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_NOGRIDLINES.c_str()) )
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES);
			}
			else
			{
				SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
			}
			
		}
		
//		SetCallbackMask(LVIS_SELECTED);
		
        SetBkColor(BACKGROUND);
		
        // Add the columns to the list
        Sort sort;
        
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding the columns to the list");
        for(unsigned int column = 0; column < columns.size(); ++column)
        {
            TA_ASSERT(columns[column] != NULL, "One of the columns passed in was NULL");
			
            if (columns[column]->isColumnEnabled())
            {
                // Only insert the column into the list and into the order vector if it is enabled
				//TD14897 azenitha++
				if (columns[column]->getColumnIdentifier() == TA_Base_Bus::COL_SEVERITY)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD14897] Setting severity column to center justified");
					InsertColumn(column, 
						columns[column]->getColumnHeaderText(),
						LVCFMT_CENTER,
						columns[column]->getColumnWidth());
				}
				else
				{
					InsertColumn(column, 
						columns[column]->getColumnHeaderText(),
						LVCFMT_LEFT,
						columns[column]->getColumnWidth());
				}
				//TD14897 ++azenitha
				
                m_alarmColumnOrder.push_back(columns[column]->getColumnIdentifier());
            }
			
            // Store the column in the map. 
            m_alarmColumns.insert( std::map<long, IAlarmColumn*>::value_type(columns[column]->getColumnIdentifier(),
				columns[column]) );
			
			
            // While we're stepping through we should save the sort that we are meant to be sorting on.
        if (columns[column]->getColumnIdentifier() == sortColumn)
            {
                if (isAscendingSort)
                {
                    sort = columns[column]->getColumnAscendingSort();
                }
                else
                {
                    sort = columns[column]->getColumnDescendingSort();
                }
				m_curSortCol=columns[column]->getSortColumn();  //GF-84, alarm sorting
            }
        }
		m_defaultSort = new Sort(sort);
		
        addBitmapsToColumnHeaders();
        loadRequiredIcons();
		
		m_sortColumn = -1;
		m_sortAscending = isAscendingSort; // set to the opposite of what we want it will switch in
	 
		updateHeaderForSort(sortColumn);
		m_sortAscending = isAscendingSort;
		//m_sortColumn = sortColumn;
		m_alarmDataSync.setDisplaySort(m_curSortCol, m_sortAscending);
		m_observer->setCurrentDescriptionText(ALARMS_RETRIEVAL_INPROGRESS_MSG);
		
        FUNCTION_EXIT;
    }
	//++TD17081
	
	//Begin----   TD15005
	void AlarmListCtrl::OnPaint() 
	{
		CPaintDC dc(this); // device context for painting
		
		DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
		OnDraw(&dc);
	}

	LRESULT AlarmListCtrl::OnAlarmIndexRefresh(WPARAM wParam, LPARAM lParam)
	{
	 
		if (m_lockedListCtrl==true)
		{
			this->UnlockWindowUpdate();
			m_lockedListCtrl=false;
		}
		m_needUpdateData = true;
		return 0;
	}

	LRESULT AlarmListCtrl::OnAlarmStoreRestart(WPARAM wParam, LPARAM lParam)
	{
		//FUNCTION_ENTRY("OnAlarmStoreRestart");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OnAlarmStoreRestart come in ");
		m_alarmStore->updatePreFilterLocationsAndSession( m_alarmStore->m_preFilter,
			m_alarmStore->m_locations,m_alarmStore->m_sessionId );
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OnAlarmStoreRestart come out");
        //FUNCTION_EXIT;
		return 0;
	}
	LRESULT AlarmListCtrl::OnAlarmStoreRemap(WPARAM wParam, LPARAM lParam)
	{	 
		m_lockedListCtrl=true;
		this->LockWindowUpdate();    
		//m_displayVec.clear();
		//m_mapSelectedAlarms.clear();
		
		//SetItemCountEx(m_displayVec.size(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
		//updateObserverWithDescription(m_mapSelectedAlarms.size()/*GetSelectedCount()*/, true);
// 		SetItemCountEx(0, LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
// 		updateObserverWithActionAvailability();

		return 0;
	}
	
	// This is much more accurate than waiting for the store remap because it might take some time for
	// the shared memory to be initialize
	LRESULT  AlarmListCtrl::OnAlarmStoreUnmap(WPARAM wParam, LPARAM lParam)
	{
		m_lockedListCtrl=true;
		this->LockWindowUpdate();    
		
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo details;
        details.totalAlarms = 0;
        details.unackAlarms = 0;
        details.highestSeverity = 0;
        details.earliestSourceTime.time = 0;
        details.earliestSourceTime.millitm = 0;
        details.latestSourceTime.time = 0;
        details.latestSourceTime.millitm = 0;
		
		m_alarmDataSync.unSelectAllAlarmItem();
		SetItemCountEx(0, LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
		updateObserver(true, false, details, 0);
		updateObserverWithActionAvailability();
	
		// Update observer to reset its filters if any.
		m_observer->notifyFilterReset();
		
		Invalidate(); // call to refresh the current display
		return 0;
	}

	void AlarmListCtrl::defaultSort()
	{
	
		if ( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING))
		{
			//GF-84, alarm sorting
			if (m_sortColumn==COL_SOURCETIMESTAMP && m_sortAscending==false)
			{
				return;
			}			
			m_curSortCol = SORT_BY_DATETIME;
			if (m_sortColumn==COL_SOURCETIMESTAMP)
			{
				m_sortAscending = true; // set to the opposite of what we want it will switch in		
			}
			else
			{
			   m_sortAscending = false; // set to the opposite of what we want it will switch in		
			}			
			updateHeaderForSort(COL_SOURCETIMESTAMP);
			m_sortAscending = false;
		}
		else
		{
			m_curSortCol = SORT_BY_DEFAULT;
			m_sortAscending = true;
		}
//		ThreadGuard guard(m_updateDisplayLock);

		//++Noel
		m_alarmDataSync.setDisplaySort(m_curSortCol, m_sortAscending);
	}
	
	void AlarmListCtrl::doPreAction()
	{
		for (AlarmActionFactory::AlarmActionList::const_iterator itAlarmActions = m_alarmActionFactory->getAlarmActions().begin();
		itAlarmActions != m_alarmActionFactory->getAlarmActions().end();
		++itAlarmActions ) //this will take about 15 secs when db is not busy
        {
            try
            {
			//	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "!!!test name %s begin", (*itAlarmActions)->getMenuEntry().c_str());
                (*itAlarmActions)->preformPreAction();
			//	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "!!!test name %s end", (*itAlarmActions)->getMenuEntry().c_str());
            }
            catch( const TA_Base_Core::AlarmListCtrlException& ale )
            {
                std::ostringstream desc;
                desc << "[AlarmListCtrl::doPreAction] Caught AlarmListCtrlException - " << ale.what();
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", desc.str().c_str() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "[AlarmListCtrl::doPreAction] Caught unknown exception" );
            }
        }
		
	}
	
	void AlarmListCtrl::onODItemChange(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMLVODSTATECHANGE lpLV = reinterpret_cast<LPNMLVODSTATECHANGE>(pNMHDR);
		
		try
		{
			// This condition is met when selecting the same set of items 
			if ((lpLV->uNewState & LVIS_SELECTED) && /*(GetKeyState(VK_LBUTTON) & PRESSED) && */(m_rangePrevious.lSelStart == lpLV->iFrom) && (m_rangePrevious.lSelEnd == lpLV->iTo))
			{
				// retrieve the selected item
				int nFrom = lpLV->iFrom;
				int nTo = lpLV->iTo;
				long lIdx = m_pDisplayVector->findItemPos(m_strSelectedAlarmID);
				
				if (lIdx != -1)
				{
					m_nCurSelPos = lIdx;
				}

				if (m_nSelDirection == -1) // selection is going up
				{
					nTo = m_nCurSelPos;
				}
				else if (m_nSelDirection == 1) // selection is going down
				{
					nFrom = m_nCurSelPos;
				}
				
				m_alarmDataSync.selectRangeAlarms(nFrom, nTo);

// 				for(long nIndex = nFrom; nIndex <= nTo; nIndex++)
// 				{
// 					m_alarmDataSync.selectAlarmItem(nIndex);
// 				}
				m_isRangeSel = true;
				// No need to udpate observers because selected Items are the same.
				updateObserverWithDescription(m_alarmDataSync.getSelectedAlarmsCount()/*GetSelectedCount()*/, true);
				updateObserverWithActionAvailability();
			}
			// remember the range selection to solve the problem with moving items and selecting the same range		
			m_rangePrevious.lSelStart = lpLV->iFrom;
			m_rangePrevious.lSelEnd = lpLV->iTo;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred.");
		}
		*pResult = 0;
		return;
	}
	void AlarmListCtrl::onItemChange(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMLISTVIEW lpLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		*pResult = 0;
	
		// Block right click event
		if(GetKeyState(VK_RBUTTON) & PRESSED)
		{
			return;
		}

		// Clear all selected alarms
		if( -1  == lpLV->iItem && 0 == lpLV->uNewState)
		{
			m_alarmDataSync.unSelectAllAlarmItem();
			m_bIgnoreEvent = false;
			return;
		}

		try 
		{
			if(lpLV->uNewState & LVIS_SELECTED)
			{
				int nVirtKey;
				nVirtKey = GetKeyState(VK_SHIFT); 
				
				// if shift key is not pressed or shift key is pressed and has already selected atleast 1 item.
				if (!(nVirtKey & PRESSED) || ((nVirtKey & PRESSED) && !m_bHasSelected))
				{
					if ((!m_isRangeSel) && ((GetKeyState(VK_UP) & PRESSED) || (GetKeyState(VK_DOWN) & PRESSED)))
					{
						m_nCurSelPos = m_pDisplayVector->findItemPos(m_strSelectedAlarmID);
						if ((GetKeyState(VK_UP) & PRESSED))
						{
							lpLV->iItem = ((m_nCurSelPos-1)<0)? 0 : m_nCurSelPos-1;
						}
						else if ((GetKeyState(VK_DOWN) & PRESSED))
						{
							lpLV->iItem = ((m_nCurSelPos+1)>m_pDisplayVector->getSize())? m_pDisplayVector->getSize() : m_nCurSelPos+1;
						}
						
						/*	SetItemState(lpLV->iItem, LVIS_FOCUSED, LVIS_FOCUSED);*/
					}
					
					m_strSelectedAlarmID =  m_alarmDataSync.getAlarmID(lpLV->iItem); //m_pDisplayVector->getItemString(lpLV->iItem, ALARMID); /*getItem(lpLV->iItem)->alarmID;*/
					// Assign the current position because alarm might be remove and getCurrentSeletedPos will return -1
					// This is neccessary so that we will have an idea on the position of the first selection.
					m_nCurSelPos = lpLV->iItem;
					
				}
				
				// This will process multiple selection if shift key is pressed and mouse left click
				if ((nVirtKey & PRESSED) && m_bHasSelected && (GetKeyState(VK_LBUTTON) & PRESSED)) 
				{
					multiselectAlarmItem(lpLV->iItem);
				}
				// This will process single selection
				else if (!(nVirtKey & PRESSED)) // shift key is not pressed
				{
					m_alarmDataSync.selectAlarmItem(lpLV->iItem);
					
					updateObserverWithDescription(m_alarmDataSync.getSelectedAlarmsCount(), true);
					updateObserverWithActionAvailability();
					
					m_bHasSelected = true; // Has already selected at least once
					m_isRangeSel = false; // selection is not ranged
					
					m_nSelDirection = 0; // Neutral direction

					// Reset the old range selection
					m_rangePrevious.lSelStart = 0;
					m_rangePrevious.lSelEnd = 0;
				}
			}
			// We only allow this if from keyboard input and the new state is focused //
			else if((GetKeyState(VK_SHIFT) & PRESSED) && !(GetKeyState(VK_LBUTTON) & PRESSED) && (lpLV->uNewState & LVIS_FOCUSED))// /*((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState(VK_UP) & 0x80))*/)
			{			
				if (m_bIgnoreEvent)
				{
					m_bIgnoreEvent = false;
					return;
				}
				int nVirtualKey;
				if ((GetKeyState(VK_DOWN) & PRESSED))
				{
					nVirtualKey = VK_DOWN;
				}
				else if ((GetKeyState(VK_UP) & PRESSED))
				{
					nVirtualKey = VK_UP;
				}
				else if ((GetKeyState(VK_NEXT) & PRESSED))
				{
					nVirtualKey = VK_NEXT;
				}
				else if ((GetKeyState(VK_END) & PRESSED))
				{
					nVirtualKey = VK_END;
				}
				else if ((GetKeyState(VK_PRIOR) & PRESSED))
				{
					nVirtualKey = VK_PRIOR;
				}
				else if ((GetKeyState(VK_HOME) & PRESSED))
				{
					nVirtualKey = VK_HOME;
				}
				
				multiselectAlarmItem(lpLV->iItem, nVirtualKey);
				
			}
			
			//tianxiang fix can not deselect alarm
			if(lpLV->uOldState & LVIS_SELECTED && !(GetKeyState(VK_SHIFT) & PRESSED))
			{
				m_alarmDataSync.unselectAlarmItem(lpLV->iItem);
				
				updateObserverWithDescription(m_alarmDataSync.getSelectedAlarmsCount(), true);
				updateObserverWithActionAvailability();
				
				// Reset the old range selection
				m_rangePrevious.lSelStart = 0;
				m_rangePrevious.lSelEnd = 0;

			}
			//tianxiang fix can not deselect alarm

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown exception occurred.");
		}
	}
	void  AlarmListCtrl::OnDraw(CDC* pDC)
	{
	/*	if (GetItemCount() <= 0)
	return;
	
	  CPen pen;
	  TRY {
	  pen.CreatePen(PS_SOLID, 0, RGB(192,192,192));
	  }
	  CATCH (CResourceException, e)
	  {
	  e->Delete();
	  return;
	  }
	  END_CATCH
	  CPen *pOldPen;
	  pOldPen = pDC->SelectObject(&pen);
	  
		CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
		if (pHeaderCtrl == NULL)
		return;
		int  nColumnCount = pHeaderCtrl->GetItemCount();
		int	 nTotalItemWidth(0);
		for (int i=0;i<nColumnCount;i++)
		{
		nTotalItemWidth+=GetColumnWidth(i);
		}
		
		  int lTopIndex,lLastIndex;
		  lTopIndex = GetTopIndex();
		  lLastIndex = GetCountPerPage() + lTopIndex;
		  if (lLastIndex > GetItemCount() - 1)
		  lLastIndex = GetItemCount() -1;
		  
			
			  CRect rect;
			  GetClientRect(&rect);
			  
				int nWidth;
				if (rect.right < nTotalItemWidth)
				nWidth = rect.right;
				else
				nWidth = nTotalItemWidth;
				CRect itemRect;
				long lcoutrow=0;
				GetItemRect(lTopIndex,itemRect,LVIR_BOUNDS);
				pDC->MoveTo(0,itemRect.top - 1);
				pDC->LineTo(nWidth,itemRect.top - 1);
				for (int lrow = lTopIndex;lrow <= lLastIndex;lrow++)
				{		
				
				  GetItemRect(lrow,itemRect,LVIR_BOUNDS);
				  pDC->MoveTo(0,itemRect.bottom-1);
				  pDC->LineTo(nWidth,itemRect.bottom-1);
				  }
				  
					int iMinCol;
					int nColumn = 0, nRight = 0;
					int nHorzScroll = GetScrollPos(SB_HORZ);
					long lx(0);
					if (nHorzScroll == 0)
					{
					iMinCol = 0;
					}
					else
					{
					while (nRight < nHorzScroll && nColumn < (nColumnCount-1))
					nRight += GetColumnWidth(nColumn++);
					iMinCol = nColumn;
					lx = nRight - nHorzScroll;
					}
					nColumn = 0;
					nRight = 0;
					while (nRight < nHorzScroll + rect.right && nColumn < (nColumnCount-1))
					nRight += GetColumnWidth(nColumn++);
					int iMaxCol = (nColumn == nColumnCount - 1)?nColumn:nColumn - 1;
					long lHeight;
					lHeight = itemRect.Height() * (lLastIndex - lTopIndex + 1);
					GetItemRect(lTopIndex,itemRect,LVIR_BOUNDS);
					if (nHorzScroll == 0)
					{
					pDC->MoveTo(0,itemRect.top);
					pDC->LineTo(0,lHeight + itemRect.top);
					}
					else
					{
					pDC->MoveTo(lx - 1,itemRect.top);
					pDC->LineTo(lx - 1,lHeight + itemRect.top);
					}
					for (int iColumn = iMinCol;iColumn <= iMaxCol;iColumn++)
					{
					lx+= GetColumnWidth(iColumn);
					pDC->MoveTo(lx - 1,itemRect.top);
					pDC->LineTo(lx - 1,lHeight + itemRect.top);
					}
					
					  pDC->SelectObject(pOldPen);
		*/
	}
	//----End  TD15005

	bool AlarmListCtrl::isDeterminateColumn(const long columnID, const CString& strColumnCaption)
	{
		FUNCTION_ENTRY("isDeterminateColumn");
		bool bFindColumn = false;
        for(std::map<long, IAlarmColumn*>::iterator iter = m_alarmColumns.begin(); iter != m_alarmColumns.end(); ++iter)
        {
			LOG2(SourceInfo,DebugUtil::DebugInfo, "========%d , %s",iter->second->getColumnIdentifier(),iter->second->getColumnHeaderText().GetBuffer(0));
			if (iter->second->getColumnIdentifier() == columnID)
			{
				if(iter->second->getColumnHeaderText() == strColumnCaption)
				{
					bFindColumn = true;
					break;
				}
			}
		}

		FUNCTION_EXIT;
		return bFindColumn;
	}

	//////////////////////////////////////////////////////////
	// The following three functions is used for stress test
	/*
	* newAlarm
	*
	* @param alarmDetail - indicate the new alarm detailed data
	*/
	void AlarmListCtrl::newAlarm(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail)
	{
		m_alarmStore->newAlarm(alarmDetail);
	}
	
	/*
	* updateAlarm
	*
	* @param alarmUpdate - indicate the updated alarm detailed data
	*/
	void AlarmListCtrl::updateAlarm(TA_Base_Core::AlarmUpdateCorbaDef alarmUpdate)
	{
		m_alarmStore->updateAlarm(alarmUpdate);
	}
	
	/*
	* getASTimeStamp
	*
	* this is used to check if alarmStore work normally
	*/
	long AlarmListCtrl::getASTimeStamp()
	{
		return m_alarmStore->getASTimeStamp();
	}
	//////////////////////////////////////////////////////////


	// Process the multiple selection using mouse event
	void AlarmListCtrl::multiselectAlarmItem(int nTargetIndex)
	{
		rangeSelInfo rangeInfo;
		// get updated position of our selected alarm Item

		try
		{
			long nTempPos = m_pDisplayVector->findItemPos(m_strSelectedAlarmID);
			if (nTempPos != -1)
			{
				m_nCurSelPos = nTempPos;
			}
			
			if (m_nCurSelPos > nTargetIndex)
			{
				rangeInfo.lSelStart = nTargetIndex;
				rangeInfo.lSelEnd = m_nCurSelPos;
				
				m_nSelDirection = -1; // Selection is going up
			}
			else
			{
				rangeInfo.lSelStart = m_nCurSelPos;
				rangeInfo.lSelEnd = nTargetIndex;
				
				m_nSelDirection = 1; // Selection is going down
			}

// 			m_alarmDataSync.selectRangeAlarms(rangeInfo.lSelStart, rangeInfo.lSelEnd);
			// retrieve the selected item
			if (rangeInfo.lSelStart == rangeInfo.lSelEnd)
				m_alarmDataSync.selectAlarmItem(rangeInfo.lSelStart);
			else
				m_alarmDataSync.selectRangeAlarms(rangeInfo.lSelStart, rangeInfo.lSelEnd);


			updateObserverWithDescription(m_alarmDataSync.getSelectedAlarmsCount(), true);
			updateObserverWithActionAvailability();
			
			m_rangeSelInfo = rangeInfo; // reset range information
			m_isRangeSel = true; // selection is not ranged

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred.");
		}
	}


	// Process the multiple selection using keyboard events
	void AlarmListCtrl::multiselectAlarmItem(int nTargetIndex, int nKeyPressed)
	{
		rangeSelInfo rangeInfo;
		
		try
		{
			// get updated position of our selected alarm Item
			int nTempPos = m_pDisplayVector->findItemPos(m_strSelectedAlarmID);
			if (nTempPos != -1)
			{
				m_nCurSelPos = nTempPos;
			}
			
			switch (nKeyPressed)
			{
			case VK_DOWN:
				{
					// Selection should start at first selected item to end item.
					int nFocusedIndex = 0; 
					rangeInfo.lSelStart = m_nCurSelPos;

					if (m_isRangeSel) // from range selection and arrow down is pressed
					{
						// re-calculate selected alarm position
						long nRange = 0;
						nRange = m_rangeSelInfo.lSelEnd - m_rangeSelInfo.lSelStart;
						//nRange--;
						if (nRange == 0)
						{
							m_nSelDirection = 1; // change the selection direction
						}
						
						if (m_nSelDirection == 1) // direction is going down
						{
							rangeInfo.lSelEnd = (m_nCurSelPos + (nRange+1) >= (long)m_pDisplayVector->getSize())? (long)m_pDisplayVector->getSize() - 1 : m_nCurSelPos + (nRange+1); 						
							nFocusedIndex = rangeInfo.lSelEnd;
						}
						else if( m_nSelDirection == -1) // direction is going up
						{
							rangeInfo.lSelStart = (m_nCurSelPos - (nRange-1) < 0)? 0:(m_nCurSelPos - (nRange-1));
							rangeInfo.lSelEnd = m_nCurSelPos;
							nFocusedIndex = rangeInfo.lSelStart;
						}
						
					}
					// arrow down is pressed and previous operation is not range selection
					else /*if(!m_isRangeSel) */
					{
						rangeInfo.lSelEnd = (m_nCurSelPos + 1 >= (long)m_pDisplayVector->getSize())? m_pDisplayVector->getSize() -1:(m_nCurSelPos + 1); 
						nFocusedIndex = rangeInfo.lSelEnd;
						m_nSelDirection = 1;
					}

					m_bIgnoreEvent = true;
					SetItemState(nFocusedIndex, LVIS_FOCUSED, LVIS_FOCUSED);
				}	
				break;
			case VK_UP:
				{
					int nFocusedIndex = 0; 
					rangeInfo.lSelEnd = m_nCurSelPos;
					
					if (m_isRangeSel) // from range selection
					{
						// re-calculate selected alarm position
						long nRange = 0;
						nRange =  m_rangeSelInfo.lSelEnd - m_rangeSelInfo.lSelStart; 
						//nRange--;
						// Todo need to check with the current nRange with m_mapSelectedAlarms.size();
						
						if (nRange == 0)
						{
							m_nSelDirection = -1; // change the selection direction
						}
						
						// direction is going up
						if (m_nSelDirection == -1) 
						{
							rangeInfo.lSelStart = (m_nCurSelPos - (nRange+1) <= 0)? 0:m_nCurSelPos - (nRange+1);
							nFocusedIndex = rangeInfo.lSelStart; 
						}
						// direction is going down
						else if( m_nSelDirection == 1)
						{
							rangeInfo.lSelStart = m_nCurSelPos;
							rangeInfo.lSelEnd = (m_nCurSelPos + (nRange - 1) >= (long)m_pDisplayVector->getSize())? (long)m_pDisplayVector->getSize() - 1:m_nCurSelPos + (nRange - 1);
							nFocusedIndex = rangeInfo.lSelEnd; 
						}
						
					}
					// arrow up is pressed and previous operation is not range selection
					else /*if(!m_isRangeSel)*/
					{
						rangeInfo.lSelStart = (m_nCurSelPos - 1 < 0)?0:m_nCurSelPos - 1;
						nFocusedIndex = rangeInfo.lSelStart; 
						m_nSelDirection = -1;
					}

					m_bIgnoreEvent = true;
					SetItemState(nFocusedIndex, LVIS_FOCUSED, LVIS_FOCUSED);
				}
				break;
			case VK_NEXT:
			case VK_END:			
				if (m_nSelDirection == -1 && m_nCurSelPos > nTargetIndex)
				{
					rangeInfo.lSelStart = nTargetIndex;
					rangeInfo.lSelEnd = m_nCurSelPos;
				}
				else
				{
				rangeInfo.lSelStart = m_nCurSelPos;
				rangeInfo.lSelEnd = nTargetIndex;
				m_nSelDirection = 1;
		
				}
				break;
			case VK_PRIOR:
			case VK_HOME:			
				if (m_nSelDirection == 1 && m_nCurSelPos < nTargetIndex)
				{
					rangeInfo.lSelStart = m_nCurSelPos;
					rangeInfo.lSelEnd = nTargetIndex;
				}
				else
				{
				rangeInfo.lSelEnd = m_nCurSelPos;				
				// Selection should start at beginning to the first selected item.
				rangeInfo.lSelStart = nTargetIndex;
				m_nSelDirection = -1;

				}
				break;
			}

			// retrieve the selected item
			if (rangeInfo.lSelStart == rangeInfo.lSelEnd)
				m_alarmDataSync.selectAlarmItem(rangeInfo.lSelStart);
			else
				m_alarmDataSync.selectRangeAlarms(rangeInfo.lSelStart, rangeInfo.lSelEnd);

			m_rangeSelInfo = rangeInfo;
			m_isRangeSel = true;
			updateObserverWithDescription(m_alarmDataSync.getSelectedAlarmsCount(), true);
			updateObserverWithActionAvailability();

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred.");
		}
	}

	void AlarmListCtrl::OnEndtrack(NMHDR *pNMHDR, LRESULT *pResult) 
	{ 
		HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

		// Intentionally empty. This function is added to disable the refresh of the column
		// on resize so that the display will not be distorted..
		m_alarmDataSync.setFreezeCacheRefresh(false);
		Invalidate();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnEndTrack called...");
		*pResult = 0;
	}

// 	void AlarmListCtrl::OnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
// 	{
// 		m_alarmDataSync.setFreezeCacheRefresh(true);
// 		this->Invalidate(TRUE); // to refresh the whole alarmlist control
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnBeginTrack called...");
// 
// 		pResult = 0;
// 	}

	BOOL AlarmListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
		LPNMHDR pnmh = (LPNMHDR) lParam; 
		
		if (true == m_bIsColResizable && (pnmh->code == (UINT)HDN_BEGINTRACKA || pnmh->code == (UINT)HDN_BEGINTRACKW)) //begin track
		{
			m_alarmDataSync.setFreezeCacheRefresh(true);
			this->Invalidate(TRUE); // to refresh the whole alarmlist control
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnBeginTrack called...");
		}
			
		return CListCtrl::OnNotify(wParam, lParam, pResult);
	}
	
	//++ Noel
	void AlarmListCtrl::_setListDisplayInfo()
	{
		ListDispInfo listInfo;
		int nCountPerPage = GetCountPerPage();

		listInfo.ulTopIndex = GetTopIndex();
		listInfo.ulDisplayCount = nCountPerPage;
		listInfo.ulNextPageBuffer = nCountPerPage;
		listInfo.ulPrevPageBuffer = nCountPerPage;

		m_alarmDataSync.setListDisplayInfo(listInfo);

	}

	void AlarmListCtrl::setColumnResizable(bool bSizable)
	{
		m_bIsColResizable = bSizable;
	}
}

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/alarmlistctrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This is a read-only list of alarms.
  */

#if !defined(AFX_ALARMLISTCTRL_H__CC35262F_8DDA_4C64_9CAB_E6CF217A2343__INCLUDED_)
#define AFX_ALARMLISTCTRL_H__CC35262F_8DDA_4C64_9CAB_E6CF217A2343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/alarm/alarm_list_control/src/CommonStructures.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "ace/Task.h"
#include "bus/alarm/alarm_list_control/src/WinMsgDef.h"
#include "core/exceptions/src/AlarmListCtrlException.h"

//++Noel
#include "bus/alarm/alarm_list_control/src/AlarmDataSync.h"


#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "bus/alarm/alarm_list_control/src/FASAlarmHandler.h"


namespace TA_Base_Bus
{
    class AlarmStore;
    class Filter;
    class Sort;
    class IAlarmListObserver;
    class AlarmListContextMenu;
    class AlarmActionFactory;
    class IAlarmAction;
    class IAlarmColumn;

    class AlarmListCtrl : public CListCtrl
    {
        friend class AlarmRetriever;

    public:

        /**
         * Constructor
         *
         * @param EViewMode - This is a quick way to set up which columns should be displayed and their sizes.
         * @param canColumnsBeSorted - This indicates if the columns can be sorted by clicking on the column headers.
         *                             If this is set to false then no column sorting will be allowed.
         * @param canColumnsBeReordered - This indicates if the columns can be reordered by dragging the column headers.
         *                                If this is set to false then no column reordering will be allowed.
         */
        AlarmListCtrl(bool canColumnsBeSorted = true, bool canColumnsBeReordered = true);
		//TD15765++
		int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
		BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
		//++TD15765
        /**
         * Destructor
         */
        virtual ~AlarmListCtrl();


        /**
         * startAlarmList
         *
         * This must be called after the alarm list has been constructed to tell it what to show
         * etc. This will tell the list what it should be showing and it will begin the list's updates
         *
         * @param IAlarmListObserver& - This will be used by the list to let an application know when
         *                              to enable/disable options and to tell it the current status
         *                              of the list etc.
         * @param AlarmActionFactory& - A factory holding all the valid alarm actions to appear in the list
         *                              context menu. The caller must tidy this object up.
         * @param std::vector<IAlarmColumn*> - All the columns to insert in the order they should be inserted. The
         *                                     caller is responsible for deleting the column objects. If a column
         *                                     is disabled it can be added into any position in the list as it
         *                                     will be ignored until it is enabled.
         * @param Filter - This is the prefilter to apply. This must not be changed regularly and should just
         *                 filter out alarms that this list does not need to see. Eg filter on a particular
         *                 profile. This would only change when the user changed their login.
         * @param vector<unsigned long> - A vector of location keys. This lets the list know which alarms it
         *                                should be displaying. For all locations the vector should be empty.
         * @param long - The column that the list should first be sorted on
         * @param bool - Indicates whether the sort should be ascending (true) or descending (false)
         * @param Filter - This is any active filters the user wants applied. This filter can be changed regularly.
         * @param sessionId - If set, the session ID the alarm should use to determine if an alarm is
         *                    acknowledgeable (and therefore requires an alarm sound).
         * @param isHighestSeverityNeeded - if true, the alarm store will return the highest un-acknowledged alarm severity
		 *									to the alarm list ctrl when the alarm list ctrl retrieve the alarm index details
         *
         * @exception AlarmListCtrlException - This is thrown if an unrecoverable error occurred that means
         *                                     the alarm list will not be able to show any alarms.
         */
        virtual void startAlarmList(IAlarmListObserver& observer,
                                    AlarmActionFactory& alarmActionFactory,
                                    const std::vector<IAlarmColumn*> &columns,
                                    const Filter& preFilter,
                                    const std::vector<unsigned long>& locations,
                                    long sortColumn,
                                    bool isAscendingSort,
                                    const Filter& filter,
                                    const std::string& sessionId = "",
									bool isHighestSeverityNeeded = false);

       /**
         * startAlarmList (TD17081)
         *
         * This must be called after the alarm list has been constructed and initialized
         * and start updating the list with what it should be showing 
         *
         * @param AlarmActionFactory& - A factory holding all the valid alarm actions to appear in the list
         *                              context menu. The caller must tidy this object up.
         * @param Filter - This is the prefilter to apply. This must not be changed regularly and should just
         *                 filter out alarms that this list does not need to see. Eg filter on a particular
         *                 profile. This would only change when the user changed their login.
         * @param vector<unsigned long> - A vector of location keys. This lets the list know which alarms it
         *                                should be displaying. For all locations the vector should be empty.
         * @param Filter - This is any active filters the user wants applied. This filter can be changed regularly.
         * @param sessionId - If set, the session ID the alarm should use to determine if an alarm is
         *                    acknowledgeable (and therefore requires an alarm sound).
         *
		 * @param isHighestSeverityNeeded - if true, the alarm store will return the highest un-acknowledged alarm severity
		 *									to the alarm list ctrl when the alarm list ctrl retrieve the alarm index details
         * @exception AlarmListCtrlException - This is thrown if an unrecoverable error occurred that means
         *                                     the alarm list will not be able to show any alarms.
         */
	   virtual void startAlarmList( AlarmActionFactory& alarmActionFactory,
									 const Filter& preFilter,
                                    const std::vector<unsigned long>& locations,
									 const Filter& filter,
                                    const std::string& sessionId = "",
									bool isHighestSeverityNeeded = false);


      /**
         * setupAlarmList (TD17081)
         *
         * This initializes the alarm list after it has been constructed, and before the alarms are loaded
		 * to the list. This sets the alarm columns, the default sort column, and sort type 
         *
         * @param IAlarmListObserver& - This will be used by the list to let an application know when
         *                              to enable/disable options and to tell it the current status
         *                              of the list etc.
         * @param std::vector<IAlarmColumn*> - All the columns to insert in the order they should be inserted. The
         *                                     caller is responsible for deleting the column objects. If a column
         *                                     is disabled it can be added into any position in the list as it
         *                                     will be ignored until it is enabled.
         * @param long - The column that the list should first be sorted on
         * @param bool - Indicates whether the sort should be ascending (true) or descending (false)
         * @param Filter - This is any active filters the user wants applied. This filter can be changed regularly.
         *
         */
	   virtual void setupAlarmList( IAlarmListObserver& observer, 
									const std::vector<IAlarmColumn*> &columns,                                   
                                    long sortColumn,
                                    bool isAscendingSort);

        /**
         * updatePreFilterLocationsAndSession
         *
         * This will update the prefilter, locations and sessionID the alarm store contains. Note that setting
         * this affects all alarm store users on the same machine.
         *
         * @param Filter - This is the prefilter to apply. This must not be changed regularly and should just
         *                 filter out alarms that this list does not need to see. Eg filter on a particular
         *                 profile. This would only change when the user changed their login.
         * @param vector<unsigned long> - A vector of location keys. This lets the list know which alarms it
         *                                should be displaying. For all locations the vector should be empty.
         * @param sessionID - The session ID the alarm store should use to determine the ackability of alarms.
         */
        virtual void updatePreFilterLocationsAndSession( const Filter& preFilter,
                                                  const std::vector<unsigned long>& locations,
                                                  const std::string& sessionID);
     

        /**
         * enableColumn
         *
         * This will display or hide the specified column. It can be called before or after startAlarmList().
         *
         * @param long - The column to show or hide
         * @param bool - True to view the column, false to hide it
         * @param int - The position the column should be inserted in in the list control. This only needs to be
         *              set if the column is being enabled.
         */
        virtual void enableColumn(long column, bool isEnabled, unsigned int position = 0);


        /**
         * getCurrentFilter
         *
         * This will retrieve the current filter the list is using. This must be called after a successful
         * call to startAlarmList().
         *
         * @return Filter - The current filter being used
         */
        virtual Filter getCurrentFilter() const;


        /**
         * setFilter
         *
         * This sets a new filter on the alarm store. This must be called after a successful
         * call to startAlarmList().
         *
         * @param Filter - The new filter to set
         *
         * @exception AlarmListCtrlException - Thrown if the filter is invalid
         */
        virtual void setFilter(const Filter& filter,bool isDefault);

		/**
		 * setSort
		 * 
		 * This sets a new sort for the list. Only for stress test
		 */

		virtual void setSort(SORT_COLUMN sortType);
        
        /**
         * getCurrentSort
         *
         * This will retrieve the current sort the list is using. This must be called after a successful
         * call to startAlarmList().
         *
         * @return Sort - The current sort being used
         */
        virtual Sort getCurrentSort() const;


        /**
         * setSort
         *
         * This sets a new sort on the alarm store. This must be called after a successful
         * call to startAlarmList().
         *
         * @param long - The column's sort to use
         * @param bool - Whether the sort is ascending(true) or descending(false)
         *
         * @exception AlarmListCtrlException - Thrown if the current filter is invalid
         */
        //virtual void setSort(long column, bool isAscending);


        /**
         * executeAction
         *
         * This will perform the action associated with the command id passed in. This method
         * will detect any errors and display an error message to the user.
         *
         * @param UINT - The command id of the action to perform 
         */
        virtual void executeAction(UINT commandId);

        /**
         * determinePrintColours
         *
         * Determines the colours that should be used when printing the specified row.
         *
         * @param index The row number to determine the colours for.
         * @param foreground The variable to store the foreground colour in.
         * @param background The variable to store the background colour in.
         */
        void determinePrintColours( long index, unsigned long& foregroundColour, unsigned long& backgroundColour ); 

		void doPreAction();

        unsigned long getEarliestAlarmTime();

        unsigned long getLatestAlarmTime();
		
		std::string getAlarmComment(int item);

		void defaultSort();


        /**
         * isDeterminateColumn
         *
         * Determines the column name and column ID is paired 
         */
		bool isDeterminateColumn(const long columnID, const CString& strColumnCaption);

		//////////////////////////////////////////////////////////
		// The following three functions is used for stress test
		/*
		* newAlarm
		*
		* @param alarmDetail - indicate the new alarm detailed data
		*/
		void newAlarm(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail);
		
		/*
		* updateAlarm
		*
		* @param alarmUpdate - indicate the updated alarm detailed data
		*/
		void updateAlarm(TA_Base_Core::AlarmUpdateCorbaDef alarmUpdate);
		
		/*
		* getASTimeStamp
		*
		* this is used to check if alarmStore work normally
		*/
		long getASTimeStamp();
		//////////////////////////////////////////////////////////

		/*
		* setColumnResizable
		*
		* this is used to set flag whether columns can be resize
		*/
		void setColumnResizable(bool bSizable);

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AlarmListCtrl)
	protected:
	//}}AFX_VIRTUAL


	    // Generated message map functions
    protected:
	    //{{AFX_MSG(AlarmListCtrl)
    afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void onGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onCustomdrawAlarmList ( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void onOdCacheHint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onRightClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void onItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onODItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   	afx_msg LRESULT  OnAlarmIndexRefresh(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnAlarmStoreRestart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnPerformPrefilterUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnAlarmStoreRemap(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnAlarmStoreUnmap(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();    //TD15005
	//}}AFX_MSG
    afx_msg void onContextMenuItemSelected(UINT nId);
        DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getRightsChecker
         *
         * This will return the object that allows rights to be checked
         *
         * @return RightsChecker& - The object to use to determine if the user can perform a certain action
         */
        virtual RightsChecker& getRightsChecker()
        {
            return m_rightsChecker;
        }


        /**
         * getDatabaseCache
         *
         * This will return the object that caches database information for faster performance
         *
         * @return DatabaseCache& - The object to use to retrieve database information
         */
        virtual DatabaseCache& getDatabaseCache()
        {
            return m_databaseCache;
        }

        virtual void PreSubclassWindow();

		virtual bool deleteSharedAlarm(std::vector<std::string>& alarmIDs);
    private:
		AlarmListCtrl(const AlarmListCtrl&);
		inline void EnableHighlighting(HWND hWnd, int row, bool bHighlight)
		{
			ListView_SetItemState(hWnd, row, bHighlight? 0xff: 0, LVIS_SELECTED);
		}
		
		inline bool IsRowHighlighted(HWND hWnd, int row)
		{
			return IsRowSelected(hWnd, row) /*&& (::GetFocus() == hWnd)*/;
		}
		
		
		inline bool IsRowSelected(HWND hWnd, int row)
		{
			return ListView_GetItemState(hWnd, row, LVIS_SELECTED) != 0;
		}
        /**
         * addBitmapsToColumnHeaders
         *
         * This will add any bitmaps to the alarm column headers
         */
        void addBitmapsToColumnHeaders();


        /**
         * loadRequiredIcons
         *
         * This will let each of the columns load any icons they may need and add them to the image list
         */
        void loadRequiredIcons();


        /**
         * getItemText
         *
         * This determines what text is at the specified item and subitem
         *
         * @param int - The row we are looking at
         * @param subItem - The column we are looking at
         *
         * @return string - The text to display in the specified position
         */
        CString getItemText(int item, int subItem);
		
        /**
         * getItemIcon
         *
         * This determines what icon is at the specified item and subitem
         *
         * @param int - The row we are looking at
         * @param subItem - The column we are looking at
         *
         * @return long - The icon number to display. -1 if no icon should be displayed
         */
        long getItemIcon(int item, int subItem);

        
        /**
         * updateDisplay
         *
         * The alarm display is updated periodically. This method is called every UPDATE_SLEEP_IN_MILLISECONDS
         * milliseconds to update the display
         */
        void updateDisplay();

        /**
         * updateObserver
         *
         * This updates the observer with the number of new alarms that have come in and the highest
         * severity so it is able to play a sound if necessary
         *
         * @param bool - This indicates if any alarms were modified. If alarms were modified then the
         *               observer needs to be told the new alarm severity to play a sound for.
         * @param bool - This indicates if any alarms were added. If new alarms came in, then the observer
         *               may need to unsilence the alarm sound.
         * @param IAlarmStoreCorbaDef::IndexInfo - The details retrieved from the alarm store on the number
         *                                         of alarms etc.
         * @param long - The number of alarms currently selected.
         * @param bool - The number of alarms stored is more than the max allowable for display i.e. display
		 *               has been trimmed to the max_display_alarms
         */
        void updateObserver(bool wereAlarmsModified, bool wereAlarmsAdded,
                            TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& details,
                            unsigned long selectedAlarmCount);

        /**
         * updateObserverWithActionAvailability
         *
         * This updates the observer with information about each action and whether it is currently 
         * available or not.
         */
        void updateObserverWithActionAvailability();


        /**
         * updateObserverWithDescription
         *
         * This updates the observer with an appropriate description to show in a status bar or the like.
         *
         * @param long - The number of alarms currently selected.
         */
        void updateObserverWithDescription(unsigned long selectedAlarmCount, bool valid);


        /**
         * updateHeaderForSort
         *
         * This method updates the column header bitmaps to show which direction the sort is.
         *
         * @param long - The column being sorted. This should be the column identifier and not
         *               the column position.
         */
    	void updateHeaderForSort(long column);


        /**
         * determineDrawingColours
         *
         * Given an alarm this method determines the foreground and background drawing colours. This is based
         * on the state of the alarm, whether it is selected or not and also whether the list is flashing or not.
         *
         * @param long - The index into the alarm list for the alarm.
         * @param unsigned long& - This is an out parameter and will be set with the COLORREF for the foreground
         * @param unsigned long& - This is an out parameter and will be set with the COLORREF for the background
         */
        DWORD determineDrawingColours(ULONG ulIndex, unsigned long& foregroundColour, unsigned long& backgroundColour);
		
		//Begin----  TD15005 

		//Make the grid line to be gray
		void  OnDraw(CDC* pDC);
		//----End  TD15005

		//TD15765++
		/**
		 * CellRectFromPoint
		 *
		 * Determines the row, column, and bounding rect of a cell.
		 *
		 * @param point The point to be tested.
		 * @param collrect To hold the bounding rect.
		 * @param col To hold the column index.
		 *
		 * @return int The row index, or -1 on failure.
		 */
		int CellRectFromPoint( CPoint & point, RECT * cellrect, int * col ) const;
		//++TD15765
		
        //
        // Constants
        //
        

        // The timer id for updates and flashing
        static const int UPDATE_TIMER;
		
		static const int PREACTION_TIMER;

   
        // The sleep times for the timers.
        static const int UPDATE_SLEEP_IN_MILLISECONDS;

        // The background colour of the list control
        static const COLORREF BACKGROUND;

        // This is the default colour that will be used if no appropriate colour can be determined
        static const COLORREF DEFAULT_COLOUR;

        // Values to show in columns
        //static const CString DATE_TIME_FORMAT; //TD12474 
        static const CString NO_VALUE;

        // This is the message that will be displayed to the user when no alarms can be retrieved.
        static const CString NO_ALARM_STORE_MSG;

        // This is the status bar message that will be displayed when no alarms match the selected filter
        // (but the system is otherwise working)
        static const CString NO_ALARMS_MATCH_FILTER_MSG;

        //TD17081++
		// This is the status bar message that will be displayed when the retrieving alarms from the
		// alarm store is still in progress
		static const CString ALARMS_RETRIEVAL_INPROGRESS_MSG;
		
        // This specifies the range of command id's that the context menu items can fall between
        static const UINT WM_MENU_BEGIN;
        static const UINT WM_MENU_END;

        //
        // Member variables
        //

        // This class interfaces to the alarm store for us
        AlarmStore* m_alarmStore;

        // The list of all alarms (in order) that we are currently displaying
        //AlarmList m_alarms;

        // These are all the alarm actions available to the user such as acking, closing, running plans etc
        AlarmActionFactory* m_alarmActionFactory;

        // This object is used to retrieve database information. It caches the information for faster lookups.
        DatabaseCache m_databaseCache;

        // This object is used to determine whether the current operator can perform certain actions.
        RightsChecker m_rightsChecker;

        // This is used by alarm actions to retrieve alarm details and to perform their tasks
        AlarmRetriever m_alarmRetriever;

        // This image list is used for any icons that need to be displayed in the list control
        CImageList m_imageList;

        // This indicates when new items are being added to the list so we stop all
        // selection updates during this time.
        bool m_areAlarmsBeingInsertedOrDeleted;

        // This contains the information for all columns in the list control. The map is keyed on the
        // column's unique identifiers.
        std::map<long, IAlarmColumn*> m_alarmColumns;

        // This vector stores the order of the columns. Each entry in the vector is an index into the
        // alarm column map (m_alarmColumns).
        std::vector<long> m_alarmColumnOrder;

        // This is the alarm list observer we can callback to tell when things change etc
        IAlarmListObserver* m_observer;

        // This indicates whether the alarm should draw in the bright colour or light colour when it
        // is redrawn. This is what simulates the flash.
        bool m_isFlashTime;

	//	bool m_needFlash;

		bool m_needUpdateData;

        // This indicates if the user can change the sort order of column. It is set at construction time
        bool m_canColumnsBeSorted;

        // This indicates if the user can reorder columns by dragging and dropping them. It is set at construction time
        bool m_canColumnsBeReordered;

        // This is the column the list is currently sorted on
        int m_sortColumn;

        // This indicates the order in which the list is currently sorted.
        bool m_sortAscending;

        // We only want to update which options are available every time the selection updates
        // So we keep track of when this changes.
        bool m_hasSelectionChanged;

		//TD15765++
        // The tooltip control so we can set the display time of the tooltip
        //CToolTipCtrl m_ToolTip;
		//++TD15765

		// The alarm details maintained after trimming
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo m_details;

		// Hidden first column rect (both header and item column)
		CRect m_hiddenColRect;

		// Indicator that the alarm store is still loading alarms
		bool m_isLoadingAlarms;

		//display lock
		static ACE_Thread_Mutex m_displayLock;
		
		//sort column
		SORT_COLUMN m_curSortCol;

		//TD13250 This will be true if the last action is SORT or ColumnClick
		bool m_isLastActionSort;

		//TD13250 This will be true if the last action is an Ack or Close alarm
		bool m_isLastActionAckOrClose;

		//TD13562 This will cache the selected alarms while list is still loading
		// It will be cleared after loading is completed
//		std::vector<std::string*> m_selectedAlarms;
		int m_loadedSelectedAlarms;
		//TD15765++
		// Variables for multiline tooltips.
		std::auto_ptr<TCHAR> m_pchTip;
		std::auto_ptr<WCHAR> m_pwchTip;
		BOOL m_bToolTipCtrlCustomizeDone;
		//++TD15765
        //TD17081++
		Sort* m_defaultSort; 
		//CL-20972 xinyang++ Sound alarm update.
// 		bool addnewAlarm;
// 		bool displayNoNull;
// 		long maxtime;
		//++xinyang
//		TA_Base_Core::ReEntrantThreadLockable m_updateDisplayLock;	
		//++noel
		AlarmDataSync m_alarmDataSync;
		AlarmVectorWrap * m_pDisplayVector;
		// This flag will serve as a guard to determine if its the first time to use shift key
		bool m_bHasSelected;

		// This variable holds the current position
		long m_nCurSelPos;
				
		// This variable will hold the currently selected alarm Item
		std::string m_strSelectedAlarmID;
		
		// Process the multiple selection using mouse event
		void multiselectAlarmItem(int nTargetIndex);
		// Process the multiple selection using keyboard events
		void multiselectAlarmItem(int nTargetIndex, int nKeyPressed);

		struct rangeSelInfo{
			long lSelStart;
			long lSelEnd;
			
			rangeSelInfo() : lSelStart(0), lSelEnd(0) {}
		};

		// Holds the record of the current selection index
		// This is used to check the if the previous range selection is the same as the current selection
		rangeSelInfo m_rangeSelInfo;

		// This variable is a flag to check if the last operation was range selection.
		bool m_isRangeSel;

		// This variable will be used the current selection direction for arrow up and down to functionality
		int m_nSelDirection; //  -1 = UP | 1 = DOWN  | 0 = NEUTRAL

		rangeSelInfo m_rangePrevious;

        bool m_bIgnoreEvent;
		bool m_lockedListCtrl;

		//++Noel
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo m_info;
		
		int m_nFlashCount;

		void _setListDisplayInfo();

		ULONG m_ulIdxFrom;
		ULONG m_ulIdxTo;

		ULONG m_ulIndexBegin;
		ULONG m_ulIndexEnd;
		// font
		CFont m_font;

		//handle FAS alarm
		FASAlarmHandler* m_FASAlarmHandle;

		// Flag to determine if column is resizable
		bool m_bIsColResizable;

    };
    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ALARMLISTCTRL_H__CC35262F_8DDA_4C64_9CAB_E6CF217A2343__INCLUDED_)

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/Filter.h $
* @author:  Nick Jardine
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2014/07/18 15:34:16 $
* Last modified by:  $Author: huirong.luo $
*
* Filter is a class that is used as the base class for each type of data filter that the list needs to show. It provides
* standard filtering methods, and a number of variables to be used by specialised filter classes, as well as a uniform
* filter interface.
*/

#if !defined(AFX_FILTER_H__01C4BE31_261D_402B_A20F_E740984A79E3__INCLUDED_)
#define AFX_FILTER_H__01C4BE31_261D_402B_A20F_E740984A79E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)  // identifier truncated to 255 characters

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <set>

#include "app/event/event_viewer/EventListComponent/src/DisplayItem.h"
#include "app/event/event_viewer/EventListComponent/src/DutyAdapter.h"
#include "app/event/event_viewer/EventListComponent/src/IEventAction.h"

#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/IApplicationObserver.h"

namespace TA_Base_Core
{
	class NamedFilter;
}
namespace EventConstants
{
	static const std::string RPARAM_NO_VISIBILITY_RULE = "NoVisibilityRule";
};

class Filter
{
public:
	// Constants to hold the masks for status information.
	static const unsigned long MASK_PAGEUP;
	static const unsigned long MASK_PAGEDOWN;
	static const unsigned long MASK_REFRESH;

	// Constant to hold the programmatic value for a filter based on "current time" (which is 0).
	static const __int64 CURRENT_TIME;
	static const __int64 PREVIOUS_DAY;
	static const __int64 UNDEFINED_TIME;

	// All filters will contain time constraints so specify the related constants here.
	static const std::string FILTER_TOTIME;
	static const std::string FILTER_FROMTIME;

	// Structure for storing column information.
	struct Column
	{
		Column( const std::string& name, unsigned long width, unsigned long action );
		Column( const Column& rhs );
		~Column();
		void operator=( const Column& rhs );

		std::string   m_name;
		unsigned long m_width;
		unsigned long m_action;
	};

	/**
	* constructor
	*/
	Filter();

	/**
	* destructor
	*/
	virtual ~Filter();

	///////////////////////////////////
	// Named Filter related methods. //
	///////////////////////////////////


	//virtual void populateTimeAndSortingToFilterCriteria (FilterCriteria & filterCri)=0;


	/**
	* setNamedFilter
	*
	* Sets the current named filter used to limit the events retrieved from the database.
	*
	* @param namedFilter The new named filter.
	*/
	void setNamedFilter( std::auto_ptr<TA_Base_Core::NamedFilter> namedFilter );

	/**
	* getNamedFilter
	*
	* Returns the current named filter.
	*
	* NOTE: This object is owned by the Filter and should not be deleted!
	*
	* @return NamedFilter* The current named filter in use.
	*/
	TA_Base_Core::NamedFilter* getNamedFilter();

	/**
	* getCurrentFilterString
	*
	* Constructs and returns a string representing the item categories that are included in the current filter.
	*
	* @return The list of items categories included in the current filter. They are presented in the following format:
	* [ItemName1]( [ItemName2]... [ItemNameN])
	*/
	virtual std::string getCurrentFilterString() =0;

	/**
	* setToTime
	*
	* Sets the upper bound of the time window.
	*
	* @param toTime The upper bound.
	*/
	virtual void setToTime( __int64 toTime );

	/**
	* getToTime
	*
	* Returns the upper bound of the time window.
	*
	* @return time_t The upper bound.
	*/
	virtual __int64 getToTime();


	/**
	* setFromTime
	*
	* Sets the lower bound of the time window.
	*
	* @param fromTime The lower bound.
	*/
	virtual void setFromTime( __int64 fromTime );

	/**
	* getFromTime
	*
	* Returns the lower bound of the time window.
	*
	* @return time_t The lower bound.
	*/
	virtual __int64 getFromTime();




	/////////////////////////////////
	// Event List related methods. //
	/////////////////////////////////

	//     /**
	//      * clearList
	//      *
	//      * Deletes all the display items currently held in memory and resets the list to 0.
	//      */
	//     virtual void clearList();

	//     /**
	//      * loadList
	//      *
	//      * Loads list data from the database and is responsible for sending all messages required
	//      * to make use of this data.
	//      */
	//     virtual void loadList();


	//     /**
	//      * refreshList
	//      *
	//      * Refreshes the list data and then sends an update message so the new data will be used.
	//      */
	//     virtual void refreshList();



	////////////////////////////////////////////
	// General Configuration related methods. //
	////////////////////////////////////////////

	/**
	* setFilterID
	*
	* Sets the filter ID for this filter. This enables an application using a pointer to the base filter to determine
	* what type of filter in use.
	*
	* @param filterID The numerical filter ID to associated with this filter object.
	*/
	virtual void setFilterID(const int filterID);

	/**
	* getFilterID
	*
	* Retrieves the filter ID associated with this filter object, allowing identification of the type of filter.
	*
	* @return The numerical filter ID associated with this filter object.
	*/
	virtual int getFilterID();


	/**
	* setEarliestTime
	*
	* Sets the earliest time taken from the table from which the events are retrieved.
	*
	* @param earliestTime The earliest time in the table.
	*/
	void setEarliestTime(time_t earliestTime);

	/**
	* getEarliestTime
	*
	* Returns the time of the earliest event present for this filter.
	*
	* @return time_t The earliest time in the table.
	*/
	time_t getEarliestTime() const
	{
		return m_earliestTime;
	}

	/**
	* getColumns
	*
	* Retrieves the list of column names that will be displayed for data retrieved by the filter object.
	*
	* @return A vector of strings, where each string is a different column name.
	*/
	virtual const std::vector<Column>& getColumns() const =0;


	/**
	* getFilters
	*
	* Retrieves the list filter IDs. This may be the same as the list of columns, or it may be (additional filters
	* that are not columns, or columns that cannot be filtered on).
	*
	* @return A vector of strings, where each string is a different filter name.
	*/
	virtual const std::vector<std::string> getFilters() const =0;

	/////////////////////////////////////////////
	// Sorting and comparison related methods. //
	/////////////////////////////////////////////

	/**
	* sort
	*
	* Initiates a sort of the data list held by the filter.
	*
	* @param columnName The name of the column by which to sort the data list.
	* @param sortAscending Flag to determine if sort order is ascending or decending. True if ascending
	* and false if descending.
	*/
	virtual void sort(const std::string& columnName,const bool sortAscending) = 0;

	/**
	* reverse
	*
	* Reverse the data list.
	*
	* @param columnName The name of the column by which to sort the data list.
	* @param sortAscending Flag to determine if sort order is ascending or decending. True if ascending
	* and false if descending.
	*/
	virtual void reverse(const std::string& columnName,const bool sortAscending) = 0;



	/**
	* getDefaultSortColumn
	*
	* Retrieves the name of the column that is to be sorted by default
	*/
	virtual const std::string getDefaultSortColumn() =0;

	//////////////////////////////////////
	// Loading control related methods. //
	//////////////////////////////////////

	virtual void stopCurrentListDataLoad();

	virtual void pauseCurrentListDataLoad();

	virtual void resumeCurrentListDataLoad();

	virtual void resetListLoadFlag() =0;


	virtual void pauseRefresh();
	virtual void resumeRefresh();


	/////////////////////////////
	// Action related methods. //
	/////////////////////////////

	/**
	* getActionWithId
	*
	* Returns the action with the specified ID.
	*
	* @param actionId The ID of the action to execute.
	*/
	virtual IEventAction* getActionWithId( const unsigned int actionId );

	//////////////////////////////////
	// Event Level related methods. //
	//////////////////////////////////

	/**
	* setEventLevel
	*
	* Sets the event level.
	*
	* @param eventLevel The new event level.
	*/
	virtual void setEventLevel( const unsigned long eventLevel );

	/**
	* getEventLevel
	*
	* Returns the current event level.
	*
	* @return unsigned long The current event level.
	*/
	virtual unsigned long getEventLevel();

	///////////////////////////
	// Duty related methods. //
	///////////////////////////

	/**
	* setDutyEnabled
	*
	* Specifies whether duty should be used when filtering for events.
	*
	* @param enabled True if duty is to be used.
	*/
	virtual void setDutyEnabled( const bool enabled );

	/**
	* isDutyEnabled
	*
	* Returns whether duty is being used to filter events.
	*
	* @return bool True if duty is being used.
	*/
	virtual bool isDutyEnabled();

	/**
	* reverseListItems
	*
	* Reverse all items in current filter list.
	*
	*/
	virtual void reverseListItems() =0;


	/**
	* sessionChanged
	*
	* called when session details changed
	*
	*/
	virtual void sessionChanged() =0;

	/**
	* sortListItems
	*
	* Sort all items in current filter list with current sorting column.
	*
	*/
	virtual void sortListItems() =0;

	/////////////////////////////
	// Colour related methods. //
	/////////////////////////////


	/**
	* getBackgroundColour
	*
	* Returns the background colour that should be used when drawing filter information.
	*
	* @return COLORREF The background colour. This will return the default colour
	*/
	virtual COLORREF getBackgroundColour();

	/**
	* getForegroundColour
	*
	* Returns m_forgroundColor
	*
	* @return COLORREF The foreground colour. This will return the default foreground colour
	*/
	virtual COLORREF getForegroundColour();

	/**
	* getRowColour
	*
	* Returns the colour that should be used when drawing the specified row.
	*
	* @return COLORREF The foreground colour. This will return the default foreground colour
	*/
	virtual COLORREF getRowColour( const unsigned long row )=0;


	/**
	* getRowColour
	*
	* Returns the foreground and background colours that should be used when drawing the specified row.
	*
	* @return COLORREF The row colours.
	*/
	virtual void getRowColour( const unsigned long row, COLORREF& foreground, COLORREF& background )=0;

	/**
	* getItemColour
	*
	* Retrieves the background colour for the given row and column.
	*
	* @param index The list entry (i.e. row) for which to retrieve the colour.
	* @param columnName The name of the column within the row for which to retrieve the colour.
	*
	* @return The RGB colour reference value of the colour to use as the background for the cell identified
	* by the index and columnName.
	*/
	virtual const COLORREF getItemColour(const unsigned long index,const std::string& columnName) ;

	/**
	* getColourReference
	*
	* Provide the colour reference based on the specified row and event colour type
	* The colour is obtained from the event colour map member
	*
	* @param row The specified row in the display list.
	* @param colourType The event colour type to be provided.
	*
	* @return COLORREF Returns the colour reference RGB of the row
	*/
	virtual const COLORREF getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType)=0;

	/////////////////////////////
	// Paging related methods. //
	/////////////////////////////

	/**
	* firstPage
	*
	* Resets back to the first page.
	*/
	virtual void firstPage()
	{
		m_currentPage = 0;
	}

	/**
	* nextPage
	*
	* Loads the next page from the database.
	*/
	virtual void nextPage()=0;

	/**
	* previousPage
	*
	* Loads the previous page from the database.
	*/
	virtual void previousPage()
	{
		// Only change to the previous page if we aren't on page 0.
		if ( m_currentPage > 0 )
		{
			m_currentPage--;
		}
	}

	/**
	* currentPage
	*
	* Returns the number of the current page.
	*
	* @return unsigned int The current page number.
	*/
	virtual unsigned int getCurrentPage()
	{
		return m_currentPage;
	}

	/**
	* insertPageTimeMap
	*
	* Stores the page and timeStamp  information.
	*/
	void insertPageTimeMap(unsigned int currentpage, std::string time)
	{
		m_pageTimeMap.insert(PageTimeMap::value_type(currentpage, time));
	}

	void clearPageTimeMap()
	{
		m_pageTimeMap.clear();
	}

	/**
	* setPageStatus
	*
	* Sets the pageStatus.
	*/
	void setPageStatus(LPARAM pageStatus )
	{
		m_pageStatus = pageStatus;
	}

	/**
	* setPageStatus
	*
	* Return the current pageStatus.
	*
	*@return LPARAM The current Page status
	*/
	LPARAM getPageStatus( )
	{
		return m_pageStatus;
	}

	/////////////////////////////////////
	// FilterCriteria related methods. //
	/////////////////////////////////////
	/**
	* populateFilterCriteria
	*
	* Populates the provided FilterCriteria object with the current filter information
	* in order to let EventHandlerManager use in setParameter(FukterCriteria, EevntDispayMode).
	* Then, later in the IEventWorker, this structure(FilterCirtia) will be transformed
	* into EventFilter to allow the DAI use.
	* NOTE: the StartTimePeriodDefined and EndTimePeriodDefined for filterCri maybe not correct after this function.
	* So need to check again.
	*
	* @param filterCri The structure to populate with filter information.
	*/
	//virtual void populateFilterCriteria (FilterCriteria & filterCri) = 0;

	//////////////////////////////////////
	// Event Selection related methods. //
	//////////////////////////////////////


	/**
	* getSelectedItems
	*
	* Returns the list of currently selected items.
	*
	* @return std::vector<int> The list of items.
	*/
	virtual const std::vector<unsigned long>& getSelectedItems() const;

    virtual void setSelectedItemIDs(std::vector<unsigned long>& selectedItems ) =0;
	virtual bool isRowSeleted(unsigned long index) = 0;

	/**
	* getItemData
	*
	* Retrieves the string data that is to be displayed in the list for the item at the specified index, in the
	* specified column.
	*
	* @param index The list entry (i.e. row) for which to retrieve the data.
	* @param columnName The name of the column within the row for which to retrieve the data.
	* @param summarise Specifies whether the column data should be summarised.
	*
	* @return The string data for display in the list box.
	*/
	virtual const std::string getItemData(const unsigned long index,const std::string& columnName, const bool summarise=true) =0;



	// The list of selected rows. This will be updated by decendant classes whenever
	// a refresh occurs.
	std::vector<unsigned long> m_selectedItems;

	std::set<std::string>m_selectedEventIDs;

	HWND getHWndDlg()
	{
		return m_hwndDlgWnd;
	}

	/////////////////////////////
	// Event related methods.  //
	/////////////////////////////
	/**
	* setParameter
	*
	* If the FilterCriteria and EventDisplayMode is just the same as last one, do nothing and return.
	* Else, update UI as empty result first, and let m_historicalHandler (or m_realtimeHandler,
	* depending on the dispaly mode) stop current task and start the new task.
	*
	* @param searchfilter, the filter cirteria for the new task.
	* @param mode, the display mode of the new task.
	*/
	//virtual void startNewTask(FilterCriteria searchfilter, EventDisplayMode mode) = 0;

	// to save the current FilterCriteria
	//void setFilterCriteria(FilterCriteria filter);

	// to get the current FilterCriteria
	//FilterCriteria getFilterCriteria();

	// Pure Abstract method
	virtual void reloadEvents()=0;

	//virtual void initEventsCount(int maxEvents, int refreshTime) = 0;
	//virtual void initialHandler() = 0;
	//virtual std::string getStatusMsg() = 0;
	//virtual bool currentListWasCutted()= 0;
	//virtual	time_t getEarliestTimeFromList() = 0 ;

	//virtual time_t getLatestTimeFromList()=0 ;

	//virtual void setBothPkeyRightBoundary(FilterCriteria & filter)=0 ;

	//virtual void setBothPkeyLeftBoundary(FilterCriteria & filter)=0 ;


protected:
	// 	   /**
	//      * getListData
	//      *
	//      * Retrieves all entries from the database matching the current filter.
	//      */
	//     virtual void getListData(bool clearPage = true) =0;

	//     /**
	//      * refreshListdata
	//      *
	//      * Retrieves all entries from the database matching the current filter, with createtimes after the latest time
	//      * currently displayed in the viewer.
	//      *
	//      * @return bool Returns true if any of the display items were modified.
	//      */
	//     virtual bool refreshListData() =0;

	/////////////////////////////
	// Colour related methods. //
	/////////////////////////////

	/**
	* determineRowColours
	*
	* Returns the foreground and background colours that should be used when drawing the specified row.
	*
	* @param in row The index of row for colouring
	* @param out unsigned long foregroundColour The return row foreground colours.
	* @param out unsigned long backgroundColour The return row background colours.
	*/
	virtual void determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour ) = 0;

	/**
	* getRowColourDefault
	*
	* Returns the default foreground colour for the specified row (if there are errors encountered, to avoid infinite loop)
	*
	* @param colourType The event colour type to be provided.
	*
	* @return COLORREF The colour to draw the row in.
	*/
	virtual COLORREF getRowColourDefault( TA_Base_Core::IEventColourData::EEventColourType colourType );



protected:


	// The latest time currently contained in the m_displayItems vector
	time_t m_latestTime;

	// The earliest time in the Event table
	time_t m_earliestTime;

	// The list of actions associated with the filter.
	std::vector<IEventAction*> m_actions;

	// Variables for controlling event loading.
	bool m_stopEventLoading;
	bool m_pauseEventLoading;
	bool m_pauseRefresh;  //changxing
	// The named filter.
	std::auto_ptr<TA_Base_Core::NamedFilter> m_namedFilter;

	// The event level setting is cannot be saved and it is therefore not suitable to store it
	// in the named filter.
	unsigned long m_eventLevel;

	// The adapter that is responsible for processing duty state.
	DutyAdapter m_dutyAdapter;

	// The variable controlling whether duty is used.
	bool m_rightEnabled;

	// The variable used to keep track of the page.
	unsigned int m_currentPage;

	// The default background colour to use when drawing filter information.
	COLORREF m_backgroundColour;

	// The default foreground colour to use when drawing filter information.
	COLORREF m_foregroundColour;

	// Map of the event colours for display
	std::map<std::string, TA_Base_Core::IEventColourData*> m_eventColoursMap;

private:
	// Define operator= and copy constructor privately so it cannot be used without a compile error.
	Filter& operator=(Filter& rhs);
	Filter(const Filter& theFilter);

	int m_filterID;
	HWND m_hwndDlgWnd;

	// The variable used to keep track of the page.
	typedef std::map<unsigned int, std::string> PageTimeMap;
	PageTimeMap m_pageTimeMap;
	LPARAM m_pageStatus;

	//FilterCriteria m_filterCriteria;


};

#endif // !defined(AFX_FILTER_H__01C4BE31_261D_402B_A20F_E740984A79E3__INCLUDED_)

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/Filter.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
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

#include "app/soe/soe_event_viewer/EventListComponent/src/DisplayItem.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/DutyAdapter.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/IEventAction.h"

#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/IApplicationObserver.h"

namespace TA_Base_Core
{
    class NamedFilter;
}

class Filter
{
public:
    // Constants to hold the masks for status information.
    static const unsigned long MASK_PAGEUP;
    static const unsigned long MASK_PAGEDOWN;
    static const unsigned long MASK_REFRESH;
	static const unsigned long MASK_LOADING;
	static const unsigned long MASK_UPDATE_LIST;
	static const unsigned long MASK_SORTING;

    // Constant to hold the programmatic value for a filter based on "current time" (which is 0).
    static const time_t CURRENT_TIME;
    static const time_t PREVIOUS_DAY;

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

	/**
	 * setfirstrPKey
	 *
	 * Sets the m_firstrPKey in the Filter, it is only used in source time sorting
	 *
	 * @param firstrPKey The new firstrPKey
	 */
	void setfirstrPKey(double firstrPKey);

	/**
	* getfirstrPKey
	*
	* Gets the firstrPKey in the Filter, it is only used in source time sorting
	*/
	double getfirstrPKey();
	
	/**
	 * setlastrPKey
	 * 
	 * Sets the m_lastrPKey in the Filter, it is only used in source time sorting
	 * 
	 * @param lastrPKey The new firstrPKey
	 */
	void setlastrPKey(double lastrPKey);

	/**
	 * getlastrPKey
	 *
	 * Gets the m_lastrPKey  in the Filter, it is only used in source time sorting
	 */
	double getlastrPKey();

    /**
     * setNamedFilter
     *
     * Sets the current named filter used to limit the events retrieved from the database.
     *
     * @param namedFilter The new named filter.
     */
    virtual void setNamedFilter( std::auto_ptr<TA_Base_Core::NamedFilter> namedFilter );

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
    virtual void setToTime( time_t toTime );

    /**
     * getToTime
     *
     * Returns the upper bound of the time window.
     *
     * @return time_t The upper bound.
     */
    virtual time_t getToTime();

    virtual void terminateThread() = 0;

    /**
     * setFromTime
     *
     * Sets the lower bound of the time window.
     *
     * @param fromTime The lower bound.
     */
    virtual void setFromTime( time_t fromTime );

    /**
     * getFromTime
     *
     * Returns the lower bound of the time window.
     *
     * @return time_t The lower bound.
     */
    virtual time_t getFromTime();

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
     * getRowColour
     *
     * Returns the colour that should be used when drawing the specified row.
     *
     * @return COLORREF The foreground colour. This will return the default foreground colour
     */
    virtual COLORREF getRowColour( const unsigned long row );

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
    virtual const COLORREF getItemColour(const unsigned long index,const std::string& columnName) =0;

    /**
     * getRowColour
     *
     * Returns the foreground and background colours that should be used when drawing the specified row.
     *
     * @return COLORREF The row colours.
     */
    virtual void getRowColour( const unsigned long row, COLORREF& foreground, COLORREF& background );


    /////////////////////////////////
    // Event List related methods. //
    /////////////////////////////////

    /**
     * clearList
     *
     * Deletes all the display items currently held in memory and resets the list to 0.
     */
    virtual void clearList();

    /**
     * loadList
     *
     * Loads list data from the database and is responsible for sending all messages required
     * to make use of this data.
     */
    virtual void loadList();

    /**
     * refreshList
     *
     * Refreshes the list data and then sends an update message so the new data will be used.
     */
    virtual void refreshList();

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
    virtual const std::string& getItemData(const unsigned long index,const std::string& columnName, const bool summarise=true) =0;

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
     * setMaxEvents
     *
     * Sets the eariles time from which this filter will retrieve database entries. All filters must have a time concept.
     *
     * @param fromTime The from time for this filter.
     */
    virtual void setMaxEvents(const unsigned long maxEvents);

    /**
     * getMaxEvents
     *
     * Returns the maximum number of events that will be placed on a page.
     *
     * @return unsigned long The maximum number of events per page.
     */
    virtual unsigned long getMaxEvents();

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
    virtual void sort(const std::string& columnName,const bool sortAscending);

	/**
     * reverse
     *
     * Reverse the data list.
     *
     * @param columnName The name of the column by which to sort the data list.
     * @param sortAscending Flag to determine if sort order is ascending or decending. True if ascending
     * and false if descending.
     */
	virtual void reverse(const std::string& columnName,const bool sortAscending);

    /**
     * compare_events
     *
     * This struct implements the std::binary_function template. It defines a method for comparing two DisplayItem pointers
     * to deterimine correct sort or positioning order.
     *
     * By using the std::binary_function template, this function can be used in conjunction with standard template functors
     * such as bind_1st and bind_2nd, which makes it quite powerful.
     */
    struct compare_events : public std::binary_function<DisplayItem*,DisplayItem*,bool>
    {    
        /**
         * operator()
         *
         * Determines the sort order of two DisplayItem pointers. By default, standard template functions will only do
         * pointer comparrisons (not useful), so this method de-references the pointers.
         *
         * @param arg1 The "left hand side" of the function
         * @param arg2 The "right hand side" of the function
         *
         * @return True if arg1 should come before arg2 under current sorting conditions.
         *
         * NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
         */
        bool operator()(DisplayItem* arg1,DisplayItem* arg2) const
        {
            return *arg1 < *arg2;
        }
    };

    /**
     * getDefaultSortColumn
     *
     * Retrieves the name of the column that is to be sorted by default
     */
    virtual const std::string& getDefaultSortColumn() =0;

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
    virtual void nextPage()
    {
        // Only change to the next page if the current is full.
        if ( m_displayItems.size() >= m_maxEvents )
        {
            m_currentPage++;
        }
    }

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

    //////////////////////////////////////
    // Event Selection related methods. //
    //////////////////////////////////////

    /**
     * setSelectedItems
     *
     * Sets the internal list of selected items.
     *
     * @param selectedItems The new set of selected items.
     */
    virtual void setSelectedItems( std::vector<unsigned long>& selectedItems );

    /**
     * getSelectedItems
     *
     * Returns the list of currently selected items.
     *
     * @return std::vector<int> The list of items.
     */
    virtual const std::vector<unsigned long>& getSelectedItems() const;

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


	// check whether loading thread is running
	virtual bool isLoading() = 0;


protected:
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
     * getListData
     *
     * Retrieves all entries from the database matching the current filter.
     */
    virtual void getListData(bool clearPage = true) =0;

    /**
     * refreshListdata
     *
     * Retrieves all entries from the database matching the current filter, with createtimes after the latest time
     * currently displayed in the viewer.
     *
     * @return bool Returns true if any of the display items were modified.
     */
    virtual bool refreshListData() =0;

	/**
     * sortListItems
     *
     * Sort all items in current filter list with current sorting column.
     *
     */
	virtual void sortListItems() =0;
    /**
     * sendStatusBarUpdate
     *
     * Sends an update message so the state of the status bar can change.
     *
     * @param eventCount The number of events to count up to in the status bar.
     * @param text The text to set if the event count is zero.
     *
     * NOTE: The text specified will be reference from another thread... do NOT use a
     *       text that will be destroyed before it is used!
     */
    virtual void sendStatusBarUpdate( const unsigned long eventCount, const char* text );

    /**
     * sendListUpdate
     *
     * Sends an update message so events can be displayed as they are loaded.
     *
     * @param numElements The number of elements now in the list. This parameter is used over
     *                    the current size of the display list to ensure an element isnt requested
     *                    when the display list is emptied.
     */
    virtual void sendListUpdate( const unsigned long numElements );

	virtual void sendListViewUpdate( const unsigned long numElements, const unsigned long pageStatus);

    // The event list. This vector stores all items that are to be displayed by the Event List.
    std::vector<DisplayItem*> m_displayItems;
    // The event list. This vector stores all items that are not excluded by current filter.
	std::vector<DisplayItem*> m_eventStoreListItems;

    // The latest time currently contained in the m_displayItems vector
    time_t m_latestTime;

    // The earliest time in the Event table
    time_t m_earliestTime;

    // Configurable maximum event count limit
    static unsigned long m_maxEvents;

    // The variable used to keep track of the page.
    unsigned int m_currentPage;

    // The list of selected rows. This will be updated by decendant classes whenever
    // a refresh occurs.
    std::vector<unsigned long> m_selectedItems;

    // The default background colour to use when drawing filter information.
    COLORREF m_backgroundColour;

    // The default foreground colour to use when drawing filter information.
    COLORREF m_foregroundColour;
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
    bool m_dutyEnabled;

	//for filter in the page changing
	static double m_firstrPKey;
	static double m_lastrPKey;

private:
    // Define operator= and copy constructor privately so it cannot be used without a compile error.
    Filter& operator=(Filter& rhs);
    Filter(const Filter& theFilter);

    int m_filterID;
	HWND  m_hwndDlgWnd;
};

#endif // !defined(AFX_FILTER_H__01C4BE31_261D_402B_A20F_E740984A79E3__INCLUDED_)

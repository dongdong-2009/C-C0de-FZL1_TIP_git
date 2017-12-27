/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventFilter.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/12/03 15:46:08 $
  * Last modified by:  $Author: huirong.luo $
  *
  * CombinedEventFilter is an implementation of the abstract class Filter, from the
  * EventListComponent. It specifies the details of events from both the EVENT table
  * and the LO_DATAPT_STATE_UPDATE table, and includes the column names, and
  * filterable data items, in constant static variables.
  *
  */

#if !defined(_COMBINED_EVENT_FILTER_H_)
#define _COMBINED_EVENT_FILTER_H_

#include "app/soe/soe_event_viewer/EventListComponent/src/Filter.h"
#include "app/soe/soe_event_viewer/src/ActionIds.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/soe/src/SOEFilter.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
//#include "app/soe/event_viewer/src/CombinedEventItem.h"





class CombinedEventItem;
class CCreateDefaultFilterThread; //zhiqiang++

#include <vector>
#include <map>

//*zhiqiang++begin
struct SrtActionPermittedInfo
{
	std::string sessionId;
	unsigned long resourceId;
	unsigned long locationKey;
	unsigned long subsystemKey;
};
//*/ //zhiqiang++end

class CombinedEventFilter : public Filter
{
public:
    // Static constants for identifying columns.
	 
	static const std::string COLUMN_TRANSACTIVE_TIME;
	static const std::string COLUMN_SOURCE_DATETIME;		// source_time 
	static const std::string COLUMN_ASSET;
	static const std::string COLUMN_DESCRIPTION; 
	static const std::string COLUMN_DISPLAY_VALUE;
	static const std::string COLUMN_LOCATION_NAME;

	// Assign the required values to the hidden column constants.

	static const std::string FILTER_TRANSACTIVE_TIME;
	static const std::string FILTER_SOURCE_DATETIME;	
	static const std::string FILTER_ASSET;
	static const std::string FILTER_DESCRIPTION;
	static const std::string FILTER_DISPLAY_VALUE;
	static const std::string FILTER_LOCATION; 
 
	

	static const unsigned long  REFRESH_PRECOUNT;

    // Static constants for storing static filter names.
    static const std::string NAMEDFILTER_DEFAULT;

	static const unsigned long MIN_DISPLAY_ITEM_NUM;
	
	static const unsigned long FIRST_EVENT_COUNT;
	static const unsigned long MAX_EVENT_COUNT;

	class LoadingThread : public TA_Base_Core::Thread
	{
	public:
		LoadingThread(CombinedEventFilter* filter);
		~LoadingThread();

		virtual void run();
		virtual void terminate();
		bool getStopFlag();
		void setColumnRange(std::string startTime, unsigned long totalRows);

	private:
		LoadingThread();
		std::string m_startTimeStr;
		unsigned long m_totalRows;
		bool m_isStopped;
		CombinedEventFilter* m_filter;

		
	};


    /**
     * Constructor
     *
     * Initialises all columns, prepares initial timestamps, and retrieves the 
     * alarm severity colour information.
     */
    CombinedEventFilter();

    /**
     * Destructor
     *
     * Cleans up any items that are currently loaded in the filter.
     */
    virtual ~CombinedEventFilter();

    //-------- Methods inherited from Filter --------/

    /**
     * getColumns
     *
     * Retrieves the column names that can be displayed for this type of filter.
     * 
     * @return A vector containing names of columns that will be displayed by the list.
     */
    virtual const std::vector<Filter::Column>& getColumns() const;

    /**
     * getFilters
     *
     * Retrieves the list filter IDs. This may be the same as the list of columns, or it may be (additional filters
     * that are not columns, or columns that cannot be filtered on).
     *
     * @return A vector of strings, where each string is a different filter name.
     */
    virtual const std::vector<std::string> getFilters() const;

    /**
     * getItemData
     *
     * Retrieves the string data that is to be displayed in the list for the item at the specified index, in the
     * specified column.
     * 
     * @param index The list entry (i.e. row) for which to retrieve the data.
     * @param columnName The name of the column within the row for which to retrieve the data.
     *
     * @return The string data for display in the list box.
     */
    virtual const std::string& getItemData(const unsigned long index,const std::string& columnName,const bool summarise=true);

    /**
     * getRowColour
     *
     * Returns the default foreground colour for the specified row.
     *
     * @param row The row to retrieve the colour for.
     *
     * @return COLORREF The colour to draw the row in.
     */
    virtual COLORREF getRowColour( const unsigned long row );

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
    virtual const COLORREF getItemColour(const unsigned long index,const std::string& columnName);

    /**
     * getCurrentFilterString
     *
     * Constructs and returns a string representing the item categories that are included in the current filter.
     *
     * @return The list of items categories included in the current filter. They are presented in the following format:
     * [ItemName1]( [ItemName2]... [ItemNameN])
     */
    virtual std::string getCurrentFilterString();

    /**
     * getDefaultSortColumn
     *
     * Retrieves the name of the column that is to be sorted by default
     */
    virtual const std::string& getDefaultSortColumn();

    /**
     * resetListLoadFlag
     *
     * Resets the stop flag in the CombinedEventAccessFactory.
     */
    virtual void resetListLoadFlag();

    /**
     * nextPage
     *
     * Loads the next page from the database.
     */
    virtual void nextPage();

	/**
     * previousPage
     *
     * Loads the previous page from the database.
     */
    virtual void previousPage();

	/**
     * sortListItems
     *
     * Sort all items in current filter list with current sorting column.
     *
     */
	virtual void sortListItems();

	/**
     * reverseListItems
     *
     * Reverse all items in current filter list.
     *
     */
	virtual void reverseListItems();

	/**
     * sessionChanged
     *
     * called when session details changed
     *
     */
	virtual void sessionChanged();
	virtual unsigned long getStoredListItemsSize();

	virtual void pushIntoStoredListItems(CombinedEventItem* eventItem);
	
	virtual void reserveStoredListItemsSize(unsigned long newSize);

	virtual bool getStopLoadingFlag()
	{
		return m_stopEventLoading;
	};
	
	bool CombinedEventFilter::getPauseLoadingFlag()
	{
		return m_pauseEventLoading;
	};

	void insertBinaryDisplayItem(DisplayItem* displayitem, bool bSortAscend);

	virtual void refreshListView();

	virtual void preloadListView( unsigned long nStatus = 0 );

	virtual void populatePublicFilter(TA_Base_Core::CombinedEventAccessFactory::EventFilter	&filter, bool refresh = false);

	virtual void setPauseRefreshFlag(bool refreshFlag)
	{
		m_pauseRefresh = refreshFlag;
	};

	virtual void assignDisplayListItems();
	virtual void terminateThread();
	virtual void CreateDefaultFilter(); //zhiqiang++


	virtual void sendListViewUpdate(const unsigned long numElements, const unsigned long pageStatus);

	// to set SOE filter
	virtual void setNamedFilter( std::auto_ptr<TA_Base_Core::NamedFilter> namedFilter );


	HWND m_hwndDlgWnd;




	// for SOE
	// audit pkey
	unsigned long getAuditPKey() const{ return m_nAuditPKey; }
	void setAuditPKey( unsigned long pkey ){ m_nAuditPKey = pkey; }
	
	void populateSOEFilter( TA_Base_Core::SOEEventFilter* filter );

	// check is the loading thread is running and loading data
	virtual bool isLoading();



	virtual void sort(const std::string& columnName,const bool sortAscending);

	void initPageMap();


	void adjustNextPageTime();

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
	virtual void determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour );

	void internalGetListData(std::string startTime, unsigned long totalRows);

    /**
     * getListData
     *
     * Retrieves all entries from the database matching the current filter.
     *
     * @return The number of database entries retrieved.
     */
    virtual void getListData(bool clearPage = true);

    /**
     * refreshListdata
     *
     * Retrieves all entries from the database matching the current filter, with createtimes after the latest time
     * currently displayed in the viewer.
     *
     * @return bool Returns true if any of the display items were modified.
     */
    virtual bool refreshListData();


private:
    
    /**
     * populateFilter
     *
     * Populates the provided EventFilter object with the current filter information
     * in order to allow the DAI to retrieve the required events.
     *
     * @param filter The structure to populate with filter information.
     * @param refresh Determines whether pkey information should be added to the filter.
     */
    void populateFilter( TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter, bool refresh = false );

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
	const COLORREF getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType);


    // A vector containing the columns
    std::vector<Filter::Column> m_columns;

    // The latest pkey stored in the display list.
    unsigned int m_latestPkey;

	unsigned long m_nAuditPKey;

	// Map of the event colours for display
	std::map<std::string, TA_Base_Core::IEventColourData*> m_eventColoursMap;

	TA_Base_Bus::AuthenticationLibrary*	m_authenticationLib;

	//LoadingThread* m_loadingThread;

	LPARAM m_pageStatus;

	typedef std::map<unsigned int, std::string> PageTimeMap;

	//li zhongyan add
	typedef std::map<unsigned int, double> PagebottomEventIDnMap;

	PageTimeMap m_pageTimeMap;

	//li zhongyan add for page
	PagebottomEventIDnMap m_pagebottomEventIDMap;

	CCreateDefaultFilterThread *m_pCreateDefaultFilterThread;//zhiqiang++
	// yanrong++ CL-15055
	std::string privGetCurrentFilterString();
	void saveAsDefaultFilter(const TA_Base_Core::NamedFilter& filter);
	std::string m_defaultFilterString;
    //fix crash problem;add lock for display vector and store vector --luohuirong  
	TA_Base_Core::ReEntrantThreadLockable m_displayItemsLock;
	TA_Base_Core::ReEntrantThreadLockable m_storedItemsLock;
	TA_Base_Core::ReEntrantThreadLockable m_sessionMapLock;

     typedef  std::map<std::string, TA_Base_Bus::SessionInfo> SESSION_MAP;
     SESSION_MAP m_SessionMap;
	// ++yanrong CL-15055
};

#endif // !defined(_COMBINED_EVENT_FILTER_H_)
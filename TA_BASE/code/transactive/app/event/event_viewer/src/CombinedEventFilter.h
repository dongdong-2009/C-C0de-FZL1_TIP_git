/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventFilter.h $
* @author:  Bradley Cleaver
* @version: $Revision: #3 $
*
* Last modification: $DateTime: 2014/07/18 15:34:16 $
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

#include "app/event/event_viewer/EventListComponent/src/Filter.h"
#include "app/event/event_viewer/src/ActionIds.h"
//#include "app/event/event_viewer/src/GUIObserver.h"
#include "app/event/event_viewer/src/EventHandlerManager.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

//#include "app/event/event_viewer/src/CombinedEventItem.h"

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
	static const std::string COLUMN_DATETIME;
	static const std::string COLUMN_SEVERITY;
	static const std::string COLUMN_ASSET;
	static const std::string COLUMN_ALARM_STATUS; // added
	static const std::string COLUMN_DESCRIPTION;
	static const std::string COLUMN_VALUE;
	static const std::string COLUMN_T;  // added
	static const std::string COLUMN_MMS;
	static const std::string COLUMN_DSS;
	static const std::string COLUMN_AVL;
	static const std::string COLUMN_OPERATOR;
	static const std::string COLUMN_COMMENT;

	// Static constants for identifying hidden columns.
	static const std::string HIDDEN_COLUMN_EVENTID;
	static const std::string HIDDEN_COLUMN_ALARMID;
	static const std::string HIDDEN_COLUMN_ACKED;
	static const std::string HIDDEN_COLUMN_STATUS;

	//hiden columns for saving redundant info : upper case value for column (equipment, description, val, operator)
	static const std::string COLUMN_DESCRIPTION_UPPER;
	static const std::string COLUMN_VALUE_UPPER;
	static const std::string COLUMN_OPERATOR_UPPER;
	static const std::string COLUMN_ASSET_UPPER;


	//static const std::string HIDDEN_COLUMN_SELECTED;

	// Static constants for identifying filters.
	static const std::string FILTER_APPLICATION_SUBSYSTEM;
	static const std::string FILTER_PHYSICAL_SUBSYSTEM;
	static const std::string FILTER_SYSTEM_STR;
	static const std::string FILTER_SUBSYSTEM_STR;
	static const std::string FILTER_LOCATION;
	static const std::string FILTER_OPERATOR;
	static const std::string FILTER_OPERATOR_STR;
	static const std::string FILTER_LOCATION_STR;
	static const std::string FILTER_EVENTTYPE; 
	static const std::string FILTER_ISCS_SYSTEM;     // add for 955
	//static const std::string FILTER_ATS_SYSTEM;     // add for 955
	static const std::string FILTER_TMTYPE;     // add for 955
	static const std::string FILTER_ALARMSTATE; // add for 955
	static const std::string FILTER_ASSET;
	static const std::string FILTER_DESCRIPTION;
	static const std::string FILTER_SEVERITY;
	static const std::string FILTER_ALARMTYPE;
	static const std::string FILTER_ALARMID;

	// Static constants for storing static filter names.
	static const std::string NAMEDFILTER_DEFAULT;

	static const unsigned long MIN_DISPLAY_ITEM_NUM;

	static const unsigned long FIRST_EVENT_COUNT;
	static const unsigned long MAX_EVENT_COUNT;


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

	virtual bool isRowSeleted(unsigned long index);

	virtual void setSelectedItemIDs(std::vector<unsigned long>& selectedItems ) ;

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
	virtual const std::string getDefaultSortColumn();

	/**
	* resetListLoadFlag
	*
	* Resets the stop flag in the CombinedEventAccessFactory.
	*/
	virtual void resetListLoadFlag();


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


	virtual void CreateDefaultFilter(); //zhiqiang++



public:

	/**
	* populateFilterCriteria
	*
	* Populates the provided FilterCriteria object with the current filter information
	* in order to let EventHandlerManager use in setParameter(FukterCriteria, EevntDispayMode).
	* Then, later in the IEventWorker, this structure(FilterCirtia) will be transformed 
	* into EventFilter to allow the DAI use.
	*
	* @param filterCri The structure to populate with filter information.
	*/
	virtual void populateFilterCriteria (FilterCriteria & filterCri);

	// to save the current FilterCriteria
	void setFilterCriteria(FilterCriteria filter);

	// to get the current FilterCriteria
	FilterCriteria getFilterCriteria();

	virtual void reloadEvents();

	virtual void populateTimeAndSortingToFilterCriteria (FilterCriteria & filterCri);

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

	virtual COLORREF getRowColour( const unsigned long row );

	/**
	* getRowColour
	*
	* Returns the foreground and background colours that should be used when drawing the specified row.
	*
	* @return COLORREF The row colours.
	*/
	virtual void getRowColour( const unsigned long row, COLORREF& foreground, COLORREF& background );

	/**
	* determineRowColours
	*
	* Returns the foreground and background colours that should be used when drawing the specified row.
	*
	* @param in row The index of row for colouring
	* @param out unsigned long foregroundColour The return row foreground colours.
	* @param out unsigned long backgroundColour The return row background colours.
	*/
	virtual void determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour ) ;

	void determineRowColoursByHandler( EventHandler* eventHandler,const unsigned long row,unsigned long& foregroundColour, unsigned long& backgroundColour );

	virtual const COLORREF getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType);
	
	/////////////////////////////
	// Paging related methods. //
	/////////////////////////////
	/**
	* nextPage
	*
	* Loads the next page from the database.
	*/
	virtual void nextPage();



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
	virtual const std::string getItemData(const unsigned long index,const std::string& columnName,const bool summarise=true);

	virtual void startNewTask(FilterCriteria searchfilter, EventDisplayMode mode);

	/**
	* initEventsCount
	*
	* Sets the MAX number of Events and Refresh Period of the events from Database.
	* 
	* @param maxEvents The max number of events
	* @param refreshTime The refresh period.
	*
	*/
	virtual void initEventsCount(int maxEvents, int refreshTime, int refreshTimeAgent);

	void setSortingPriority();
	
	
	/**
	* initialHandler
	*
	* Initial Event Handlers.
	* 
	*
	*/
	virtual void initialHandler();

	/**
	* initialHandler
	*
	* Initial Event Handlers.
	* 
	*
	*/
	virtual std::string getStatusMsg()
	{
		return m_eventHanlderManager->m_msg;
	}

	virtual bool currentListWasCutted();

	virtual time_t getEarliestTimeFromList() ;

	virtual time_t getLatestTimeFromList() ;

	virtual void setBothPkeyRightBoundary(FilterCriteria & filter) ;

	virtual void setBothPkeyLeftBoundary(FilterCriteria & filter) ;


private:

	// A vector containing the columns
	std::vector<Filter::Column> m_columns;

	// The latest pkey stored in the display list.
	unsigned int m_latestPkey;

	TA_Base_Bus::AuthenticationLibrary*	m_authenticationLib;

	CCreateDefaultFilterThread *m_pCreateDefaultFilterThread;//zhiqiang++
	// yanrong++ CL-15055
	std::string privGetCurrentFilterString();
	void saveAsDefaultFilter(const TA_Base_Core::NamedFilter& filter);

	std::string m_defaultFilterString;

	TA_Base_Core::ReEntrantThreadLockable m_sessionMapLock;

	typedef  std::map<std::string, TA_Base_Bus::SessionInfo> SESSION_MAP;
	SESSION_MAP m_SessionMap;
	// ++yanrong CL-15055

	EventHandlerManager * m_eventHanlderManager;

	FilterCriteria m_filterCriteria;

};

#endif // !defined(_COMBINED_EVENT_FILTER_H_)
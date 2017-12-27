/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/EventListComponent.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * EventListComponent is the main list class. It is a subclass of CListCtrl, and is responsible for controlling the display
 * of list data, and providing functions to the main application such as print, sort, and export.
 */

#if !defined(AFX_EVENTLISTCOMPONENT_H__CEA29538_337D_45EF_BA62_C01BB75DD54D__INCLUDED_)
#define AFX_EVENTLISTCOMPONENT_H__CEA29538_337D_45EF_BA62_C01BB75DD54D__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventListComponent.h : header file
//

#include <algorithm>
#include <string>

#include "app/soe/soe_event_viewer/EventListComponent/src/EventListComponent_rc.h"

// Define User Messages sent by the list
#define WM_USER_LIST_UPDATE             WM_USER + 0x500
#define WM_USER_LOAD_COUNT              WM_USER + 0x501
#define WM_USER_LOAD_UPDATE             WM_USER + 0x502
#define WM_USER_LIST_SELECTION_UPDATE   WM_USER + 0x503
#define WM_USER_LISTVIEW_UPDATE         WM_USER + 0x504

//zhiqiang++begin
#define WM_USER_POPULATE_EVENT_LEVELS   WM_USER + 0x506 
#define WM_USER_APPLY_RIGHTS            WM_USER + 0x507
//zhiqiang++end

/////////////////////////////////////////////////////////////////////////////
// EventListComponent window

class EventLoader;  // forward declaration
class Filter;       // forward declartion

class EventListComponent : public CListCtrl
{
// Construction
public:

    /**
     * constructor
     */
	EventListComponent();

    int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
    BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventListComponent)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(EventListComponent)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnCustomdrawEventList ( NMHDR* pNMHDR, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()

public:

    /**
     * destructor
     */
	virtual ~EventListComponent();

    /**
     * clearList
     *
     * Wraps the DeleteAllItems method of CListCtrl to remove the active sort
     */
    virtual BOOL clearList();

    /**
     * setFilter
     *
     * Set the filter object that will be used by the list to load and sort data when required.
     * 
     * @param filter The filter object to use for data operations.
     */
    void setFilter(Filter* filter);

    /**
     * loadEvents
     *
     * Notifies the event loading thread to begin loading a new list of events in its next loop
     */
    void loadEvents(bool enableRefresh);


	void reloadEvents();

    /**
     * refreshEvents
     *
     * Notifies the event loading thread to begin refreshing the list of events in its next loop
     */
    void refreshEvents();

    /**
     * setRefreshPeriod
     *
     * Sets the time period (in miliseconds) between automatic list updates
     *
     * @param refreshTimeMs The time (in miliseconds) between each list refresh. 
     * NOTE: The default value (0), is the "off" value, which will disable automatic refresihing.
     */
    void setRefreshPeriod(const unsigned long refreshTimeMs = 0);

    /**
     * shutdownList
     *
     * Called when the list is about to be removed. It will block until the list is no longer active, and it has been
     * prepared for deletion.
     *
     * N.B.: This method will "pump" the main application's message queue while waiting to shutdown, to prevent the main 
     * application from appearing to be hung during the shutdown process. This, however, leades to a prerequisite:
     * 
     * pre: This method MUST be called from the main GUI thread. It cannot be called from a TransActive thread, nor from a
     * CWinThread
     */
    void shutdownList();

    /**
     * print
     *
     * This method prints the current contents of the event list. It will print only those columns that are currently visible,
     * but it will print ALL events in the list (not just those that are visible).
     *
     * NOTE: while this method is active, the main GUI is not accissible, although automatic refresh updates will continue 
     * if enabled.
     */
    void print();

    /**
     * export
     *
     * This method writes the ENTIRE contents of the event list to a file - INCLUDING columns that are currently hidden.
     *
     * NOTE: while this method is active, the main GUI is not accissible, although automatic refresh updates will continue 
     * if enabled.
     *
     * @param exportLocation The default export location. To not set a default location, ensure the property 
     * exportLocation.IsEmpty() == TRUE
     */
    void export(const CString& exportLocation);

    /**
     * setColourCoding
     *
     * Flag determines if colour coding information is requested from the filter object. If enabled, the filter is queried
     * for colour coding information based on row ID and column position for every entry in the list.
     *
     * @param isEnabled True if colour coding is to be enabled, false otherwise.
     */
    void setColourCoding(const bool isEnabled);

    /**
     * changeDisplay
     *
     * Tells the EventListComponent to discard its current filter object, and reset itself to use the new filter object
     * provided to this method.
     *
     * @param filter The filter object to replace the existing one with.
     */
    void changeDisplay(Filter* filter);

    /**
     * isActive
     *
     * Determines if the EventListComponent is still actively carrying out database interaction. Can be used to determine when
     * it is safe to carry out certain operations (exit, export, etc).
     *
     * @return True if the EventListComponent is currently "active".
     */
    bool isActive();

    /**
     * sort
     *
     * Sorts the contents of the list view based on the "Default" filter, as retrieved from the filter object. Sets the 
     * sort icon on the correct column header.
     */
    void sort();

    /**
     * sort
     *
     * Sorts the contents of the list view based on the contents of the specified column name, and in the specified order
     * Sets the sort icon on the correct column header.
     *
     * @param columnName The name of the column to sort on
     * @param sortAscending True if the sort order is ascending, false for descending.
     */
    void sort(const std::string& columnName,const bool sortAscending);

    /**
     * reapplyCurrentSort
     *
     * Reapplies the last sort. This should be called after the list has been reloaded from the database if the last
     * sort needs to be reapplied.
     */
    void reapplyCurrentSort();

    /**
     * previousPage
     *
     * Moves back to the previous page.
     */
    void previousPage();

    /**
     * nextPage
     *
     * Moves on to the next page.
     */
    void nextPage();

    /**
     * SetItemCountEx
     *
     * Override this method so that we can reapply selections every time the list
     * is modified.
     *
     * @param size The number of elements in the list.
     * @param flags Identifies the behaviour of the list when inserting.
     */
    BOOL SetItemCountEx( int size, DWORD flags = LVSICF_NOINVALIDATEALL );

    /**
     * executeAction
     *
     * Executes the action with the specified action ID.
     *
     * @param actionId The ID of the action.
     */
    void executeAction( const unsigned int actionId );

private:
    /**
     * convertText
     *
     * Used by export() to escape commas in text fields before writing them to the file. As the file format is "comma-delimited"
     * a comma on its own will break the formmating, and therefore needs to be "escaped".
     *
     * @param text A reference to the text that is to be searched for commas. After the method has completed, the string contained
     * by text will be correctly delimited.
     */
    void convertText(std::string& text);

    /**
     * sort
     *
     * Sorts the contents of the list view. This is the function that actually carries out the sort processing. The others just
     * determine how to fill out these parameters.
     *
     * @param columnName The name of the column to sort on
     * @param columnId The "position" of the column in the list. Note that this is NOT the physical position of the column,
     * rather the index to an internal array of columns heald by the list control.
     * @param sortAscending True if the sort order is ascending, false for descending.
     */
    void sort(const std::string& columnName, const int columnId, const bool sortAscending);

    /**
     * removeNonPrintableCharacters
     *
     * For TD7760. Removes non-printable characters from a string.
	 * passed in, by replacing them with a space character.
     *
     * @param inputString The string from which non-printable
	 * characters should be replaced.
     * @return string The input string without non-printable characters.
     */
	std::string replaceNonPrintableCharactersWithSpace(const std::string& inputString);

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


    Filter* m_filter;
    EventLoader* m_eventLoader;

    unsigned long m_refreshPeriodMs;
    unsigned long m_elapsedTimeMs;

    int m_currentSortColumn;
    bool m_sortAscending;
    bool m_isColourCoded;

    static const unsigned long ONE_SECOND;

    bool m_reapplyingSelection;

    // Variables for multiline tooltips.
    std::auto_ptr<TCHAR> m_pchTip;
    std::auto_ptr<WCHAR> m_pwchTip;
    BOOL m_bToolTipCtrlCustomizeDone;
	// TD 12763 Default column width arrays for appropriate printout
	int * m_printoutColWidths;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTLISTCOMPONENT_H__CEA29538_337D_45EF_BA62_C01BB75DD54D__INCLUDED_)

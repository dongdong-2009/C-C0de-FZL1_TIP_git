/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/EventLoader.h $
 * @author:  Nick Jardine
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * EventLoader implements the Thread. It is a thread that loops for the life of the Event List, and is responsible
 * for all database loading.
 */

#if !defined(AFX_EVENTLOADER_H__57CD9BEC_6F3A_4DF1_80AA_0AC7510E596C__INCLUDED_)
#define AFX_EVENTLOADER_H__57CD9BEC_6F3A_4DF1_80AA_0AC7510E596C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/soe/soe_event_viewer/src/StatusProgressBar.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/Filter.h"

#include "core/data_access_interface/src/IDatabaseCallbackUser.h"

#include "core/threads/src/Thread.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class EventLoader  :  public TA_Base_Core::Thread
{
public:
    /**
     * constructor
     *
     * @param filter The filter object the class will use to retrieve events from the database.
     */
	EventLoader(Filter* filter);

    /**
     * destructor
     */
	virtual ~EventLoader();

//
// Thread 
//

    /**
     * run
     *
     * Called by the threading framework when this thread is started (by calling start())
     */
    void run();

    /**
     * terminate
     *
     * Called by the threading framework when this thread is to be stopped (by calling terminateAndWait())
     */
    void terminate();

//
// Event Loader specific
//

    /**
     * setRefreshPeriod
     *
     * Sets the time period between automatic refresh updates.
     *
     * @param refreshTimeMs The time to wait between refresh updates.
     *
     * @return The time period that has been overwritten by this new setting.
     */
    unsigned long setRefreshPeriod(const unsigned long refreshTimeMs);

    /**
     * setRefresh
     *
     * Forces a refresh on the next loop through the thread.
     */
    void setRefresh();

    /**
     * setLoad
     *
     * Informs the class to perform a load on the next loop through the thread.
     */
    void setLoad(bool enableRefresh);

	/**
	* reLoad
	*
	* using the new filter criteria to reload data, whether auto-refresh or not determine by m_refreshaction .
	*/
	void reLoad();

    /**
     * changeFilter
     *
     * Changes the filter used by the event loader when loading events from the databse.
     *
     * @param filter The new filter object to use for database access.
     */
    void changeFilter(Filter* filter);

    /**
     * isCurrentlyActive
     *
     * Determines if the thread is currently accessing the database.
     *
     * @return True if the thread is accessing the database.
     */
    bool isCurrentlyActive();

    /**
     * isCurrentlyLoading
     *
     * Determines if the thread is currently loading from the database.
     *
     * @return True if the thread is loading.
     */
    bool isCurrentlyLoading();

    /**
     * isCurrentlyRefreshing
     *
     * Determines if the thread is currently refreshing from the database.
     *
     * @return True if the thread is refreshing.
     */
    bool isCurrentlyRefreshing();

    /**
     * stopCurrentLoad
     *
     * This method will stop the current load midway so that shutdown can be executed immediately
     * without waiting for the loading to finish.
     */
    void stopCurrentLoad();

private:
    // This type is used to track the actions the loader is required to perform.
    typedef enum
    {
        E_NOACTION,
        E_REFRESH,
        E_LOAD
    } ELoaderAction;

    /**
     * refreshList
     *
     * Initiates a refresh from the database, and notifies the main application via a message when the refresh is complete.
     */
    void refreshList();

    /**
     * loadList
     *
     * Initiates a load from the database, and notifies the main application via a message when the load is complete.
     */
    void loadList();

    /**
     * setActive
     *
     * Sets the active flag, so that any call to isCurrentlyRefreshing will return true
     */
    void setCurrentAction( ELoaderAction action );


    // Filter object used for database interaction
    Filter* m_filter;

    // The number of loops the main thread should wait between refresh updates
    unsigned long m_restLoops;

	unsigned long m_refreshsec;     //changxing TD14760
    // Flags to control application behaviour
    bool m_isTerminated;
    bool m_firstLoadComplete;
    bool m_actionLoad;
    bool m_actionRefresh;
    ELoaderAction m_currentAction;

    // The time the applicaiton will sleep for at each loop (not the total wait time - wait time = SLEEP_TIME*m_restLoops)
    static const int SLEEP_TIME;

    // Guard to control concurrent access to important variables.
    TA_Base_Core::ReEntrantThreadLockable m_actionGuard;
};

#endif // !defined(AFX_EVENTLOADER_H__57CD9BEC_6F3A_4DF1_80AA_0AC7510E596C__INCLUDED_)

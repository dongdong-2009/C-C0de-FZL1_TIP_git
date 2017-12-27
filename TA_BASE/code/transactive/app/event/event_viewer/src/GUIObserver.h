/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/GUIObserver.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * GUIObserver is ...
  *
  */

#if !defined(GUI_OBSERVER_)
#define GUI_OBSERVER_

#include <windows.h>
#include <vector>
#include <string>
#include <map>

#include "app/event/event_viewer/src/FilterCriteria.h "
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/data_access_interface/src/IEventColourData.h"

#define WM_USER_LIST_UPDATE             WM_USER + 0x500
#define WM_USER_LOAD_COUNT              WM_USER + 0x501
#define WM_USER_LIST_SELECTION_UPDATE   WM_USER + 0x503
#define WM_USER_LISTVIEW_UPDATE         WM_USER + 0x504


namespace TA_Base_App{
	class GUIObserver
	{
	public:
		GUIObserver(void);
		~GUIObserver(void);
		void sendListViewUpdateByObserver( const unsigned long numElements, const unsigned long pageStatus) ;
		void sethwndDlg ( HWND dlg );
		/*virtual void updateAfterGetList()=0;*/
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
	 * update status, when ready to start a new query. "Query" or "Disabled" will show.
	 */
	virtual void statusBarUpdateWhenStartQuery()=0;

	virtual void statusBarUpdate (const unsigned long eventCount,TA_Base_Core::EventCategory eventCategory , const std::string status )=0;

	virtual void reloadEvents()=0;
	/**
     * sendListUpdate
     *
     * To update the status bar(current status, e.g. Quering/Ready..)
     */
    virtual void sendListUpdate( const unsigned long numElements );

	/** 
	 * sendListViewUpdate
	 *
	 * To update the list content, and the total size on the status bar
	 */
	virtual void sendListViewUpdate( const unsigned long numElements, const unsigned long pageStatus);
	
	/*virtual void preloadListView()=0;*/

   virtual void refreshListView()=0;


	private: 
	protected:
		HWND m_hwndDlgWndEventViewer;   // Filter has this member too!  should be modified.

	};

};


#endif // !defined(GUI_OBSERVER_)

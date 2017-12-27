/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/IEventWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * IEventWorker is ...
  *
  */

#include "app/event/event_viewer/src/GUIObserver.h"



namespace TA_Base_App 
{

	GUIObserver::GUIObserver(void):m_hwndDlgWndEventViewer(NULL)
	{
		//m_hwndDlgWndEventViewer = AfxGetMainWnd()->GetSafeHwnd();
	}
	GUIObserver::~GUIObserver(void)
	{

	}
	void GUIObserver::sendListViewUpdateByObserver( const unsigned long numElements, const unsigned long pageStatus )
	{
		// Send the message.
		PostMessage( m_hwndDlgWndEventViewer, WM_USER_LIST_UPDATE, numElements, 0 );
		PostMessage( m_hwndDlgWndEventViewer, WM_USER_LISTVIEW_UPDATE, numElements, 0 );
	}

	void GUIObserver::sethwndDlg ( HWND dlg )
	{
		m_hwndDlgWndEventViewer = dlg;
	}

	void GUIObserver::sendStatusBarUpdate( const unsigned long eventCount, const char* text )
	{
		// Send the message.
		PostMessage( m_hwndDlgWndEventViewer, WM_USER_LOAD_COUNT, eventCount, reinterpret_cast<unsigned long>(text) );
	}

	void GUIObserver::sendListUpdate( const unsigned long numElements )
	{
		// Send the message.
		PostMessage( m_hwndDlgWndEventViewer, WM_USER_LIST_UPDATE, numElements, 0 );
	}

	void GUIObserver::sendListViewUpdate( const unsigned long numElements, const unsigned long pageStatus )
	{
		// Send the message.
		PostMessage( m_hwndDlgWndEventViewer, WM_USER_LISTVIEW_UPDATE, numElements, pageStatus );
	
	}

};
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/MainFrm.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The main frame.  This class is responsible for updating the toolbar base on rights and job selection.
  */

#include <afxpriv.h>
#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManager.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerDoc.h"
#include "app/scheduling/scheduling_manager/src/MainFrm.h"
#include <algorithm>
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/resources/resource.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IResource.h"

//TD16358
#include "bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h"
//TD16358
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveFrame;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{

    CMainFrame::ButtonInfo CMainFrame::BUTTON_INFO[] = 
    {
		{TA_Base_App::SchedulingManager::Add, _T("增加"), IDI_TOOLBAR_ADD, ID_JOB_ADD, false},
		{TA_Base_App::SchedulingManager::Delete, _T("删除"), IDI_TOOLBAR_DELETE, ID_JOB_DELETE, false},
		{TA_Base_App::SchedulingManager::View, _T("查看"), IDI_TOOLBAR_VIEW, ID_VIEW_JOBDETAILS, false},
		{TA_Base_App::SchedulingManager::Filter, _T("全部工作"), IDI_TOOLBAR_FILTER, ID_VIEW_ALLJOBS, true},
		{TA_Base_App::SchedulingManager::Edit, _T("编辑"), IDI_TOOLBAR_EDIT, ID_JOB_EDIT, false},
		{TA_Base_App::SchedulingManager::Enable, _T("生效"), IDI_TOOLBAR_ENABLE, ID_JOB_ENABLE, false},
		{TA_Base_App::SchedulingManager::Disable, _T("失效"), IDI_TOOLBAR_DISABLE, ID_JOB_DISABLE, false},
		{TA_Base_App::SchedulingManager::Print, _T("打印"), IDI_TOOLBAR_PRINT, ID_FILE_PRINT, true}
    };

    const int CMainFrame::FIRST_BUTTON_INDEX = 0;
    const int CMainFrame::NUM_BUTTONS = sizeof CMainFrame::BUTTON_INFO / sizeof CMainFrame::BUTTON_INFO[FIRST_BUTTON_INDEX];
    const CSize CMainFrame::BUTTON_SIZE = CSize(100, 44);
    const CSize CMainFrame::IMAGE_SIZE = CSize(24,24);


    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame

    IMPLEMENT_DYNCREATE(CMainFrame, TransActiveFrame)

    BEGIN_MESSAGE_MAP(CMainFrame, TransActiveFrame)
	    //{{AFX_MSG_MAP(CMainFrame)
	    ON_WM_CREATE()
	    ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, onUpdateFilePrint)
	    ON_UPDATE_COMMAND_UI(ID_VIEW_JOBDETAILS, onUpdateViewJobdetails)
	    ON_UPDATE_COMMAND_UI(ID_JOB_ADD, onUpdateJobAdd)
	    ON_UPDATE_COMMAND_UI(ID_JOB_DELETE, onUpdateJobDelete)
	    ON_UPDATE_COMMAND_UI(ID_JOB_DISABLE, onUpdateJobDisable)
	    ON_UPDATE_COMMAND_UI(ID_JOB_EDIT, onUpdateJobEdit)
	    ON_UPDATE_COMMAND_UI(ID_JOB_ENABLE, onUpdateJobEnable)
		ON_MESSAGE(WM_SETMESSAGESTRING, onSetMessageString)
	    //}}AFX_MSG_MAP
        ON_MESSAGE(WM_JOBSELECTION_CHANGE, onJobSelectionChange)
        ON_MESSAGE(WM_RIGHTS_AGENT_DIED, onRightsAgentDied)
        ON_MESSAGE(WM_STATUSTEXT_CHANGE, onStatusTextChange)
		//TD16358++
		ON_MESSAGE(WM_SYSTEM_INNORMALMODE, onSystemInNormalMode)
		ON_MESSAGE(WM_SYSTEM_INDEGRADEDMODE, onSystemInDegradedMode)
		//++TD16358
        ON_MESSAGE(WM_CLEAN_UP, cleanUp)
    END_MESSAGE_MAP()

    static UINT indicators[] =
    {
	    ID_SEPARATOR,           // status line indicator
	    ID_INDICATOR_CAPS,
	    ID_INDICATOR_NUM,
	    ID_INDICATOR_SCRL,
    };

    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame construction/destruction

    CMainFrame::CMainFrame()
        : m_lastSelectedJobs (NULL),
		  //TD16358++
		  bGroupOffline(false),
		  //++TD16358
		  m_groupUpdateSubscriber(NULL)	// TD16358
    {
	    FUNCTION_ENTRY( "Constructor" );
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1; 
        properties.maxWidth = -1;
        properties.minWidth = 477;
        properties.minHeight = 400;
        setResizingProperties(properties);	

		m_statusBarString = "";

		// TD16358++
		m_groupUpdateSubscriber = new TA_Base_Bus::SystemControllerGroupUpdateSubscriber(*this);
		//TD16358

		FUNCTION_EXIT;
	}


    CMainFrame::~CMainFrame()
    {
	    FUNCTION_ENTRY( "Destructor" );
        if (m_lastSelectedJobs != NULL)
        {
            // delete the old selections cache
            delete m_lastSelectedJobs;
            m_lastSelectedJobs = NULL;
        }

			// TD16358++
		if(m_groupUpdateSubscriber != NULL)
        {
            delete m_groupUpdateSubscriber;
            m_groupUpdateSubscriber = NULL;
        }
		// ++TD16358
	    FUNCTION_EXIT;
    }


    LRESULT CMainFrame::cleanUp(WPARAM, LPARAM)
    {
        CSchedulingManagerDoc* doc = dynamic_cast<CSchedulingManagerDoc*>(GetActiveDocument());
        if (doc != NULL)
        {
            doc->cleanUp();
        }
        return 0;
    }

	//++TD16358
    void CMainFrame::notifySystemStateDegraded()
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateDegraded" );

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD16358]Occ group now offline!");

        CWnd* pWnd = AfxGetApp()->GetMainWnd();
		if( (NULL != pWnd) && IsWindow(pWnd->GetSafeHwnd()))
		{
            // To disable the "Add/Delete/Edit/Enable/Disable" item
            pWnd->PostMessage(WM_SYSTEM_INDEGRADEDMODE);
		}
		else
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The Main dialog has been closed or it is not a window, so exit");
		}

	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateDegraded" );
    }


    void CMainFrame::notifySystemStateNormal()
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateNormal" );

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD16358]Occ group now online!");

		CWnd* pWnd = AfxGetApp()->GetMainWnd();
		if( (NULL != pWnd) && IsWindow(pWnd->GetSafeHwnd()))
		{
		    // To enable the "Add/Delete/Edit/Enable/Disable" item
            pWnd->PostMessage(WM_SYSTEM_INNORMALMODE);
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The Main dialog has been closed or it is not a window, so exit");
		}

	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateNormal" );
    }
	//TD16358++

    int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
	    FUNCTION_ENTRY( "OnCreate" );
	    if (TransActiveFrame::OnCreate(lpCreateStruct) == -1)
        {
		    return -1;
        }

	    if (!m_wndStatusBar.Create(this) ||
		    !m_wndStatusBar.SetIndicators(indicators,
		      sizeof(indicators)/sizeof(UINT)))
	    {
		    TRACE0("Failed to create status bar\n");
		    return -1;      // fail to create
	    }

	    FUNCTION_EXIT;
		return 0;
		//TD18095,jianghp, to configure the toolbar after start up the frame window
        //return initialiseToolbar();
    }

    BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
    {
	    FUNCTION_ENTRY( "PreCreateWindow" );
	    if( !TransActiveFrame::PreCreateWindow(cs) )
		    return FALSE;
	    // TODO: Modify the Window class or styles here by modifying
	    //  the CREATESTRUCT cs

		// turn off the FWS_ADDTOTITLE flag to stop the document title being prefixed to the window title
		// (i.e. instead of "Untitled - SchedulingManager", we just want "SchedulingManager")
		cs.style &= ~FWS_ADDTOTITLE;

	    FUNCTION_EXIT;
	    return TRUE;
    }


    void CMainFrame::setCallbackObject( TA_Base_Bus::IGUIAccess& gui)
    {
	    FUNCTION_ENTRY( "setCallbackObject" );

        TransActiveFrame::setCallbackObject(gui);
/*for jianghp
                CSchedulingManagerDoc* doc = dynamic_cast<CSchedulingManagerDoc*>(GetActiveDocument());
                TA_ASSERT(doc != NULL, "Unable to get SchedulingManagerDoc");
                doc->initialise();*/
        

        CString appName = RunParams::getInstance().get(RPARAM_APPNAME).c_str();
        SetWindowText(appName);
        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame diagnostics

    #ifdef _DEBUG
    void CMainFrame::AssertValid() const
    {
	    TransActiveFrame::AssertValid();
    }

    void CMainFrame::Dump(CDumpContext& dc) const
    {
	    TransActiveFrame::Dump(dc);
    }
    #endif //_DEBUG


    int CMainFrame::initialiseToolbar()
    {
        using namespace TA_Base_App::SchedulingManager;

		AfxGetMainWnd()->SetWindowText(_T("系统时间表"));

	    FUNCTION_ENTRY( "initialiseToolbar" );
        DWORD style1 = TBSTYLE_FLAT | TBSTYLE_WRAPABLE;
        DWORD style2 = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
        // Have turned off tooltips as there isn't much point. They just say the same as the button name.

        // Create and load the toolbar
	    if (!m_wndToolBar.CreateEx(this, style1, style2))
        {
            // if can't create it, then just return
            return -1;
        }
        m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
        m_wndToolBar.SetSizes(BUTTON_SIZE, IMAGE_SIZE); 

        CToolBarCtrl& toolbarCtrl = m_wndToolBar.GetToolBarCtrl();

        // Create the image lists for the icons
        // Start with an image list of size 10 and grow by 10 each time
        m_toolbarImageListHot.Create(ICON_SIZE,ICON_SIZE,ILC_COLOR32,NUM_BUTTONS,NUM_BUTTONS);
        m_toolbarImageListHot.SetBkColor( GetSysColor(COLOR_3DFACE) );
        toolbarCtrl.SetHotImageList( &m_toolbarImageListHot );

        m_toolbarImageListEnabled.Create(ICON_SIZE,ICON_SIZE,ILC_COLOR32,NUM_BUTTONS,NUM_BUTTONS);
        m_toolbarImageListEnabled.SetBkColor( GetSysColor(COLOR_3DFACE) );
        toolbarCtrl.SetImageList( &m_toolbarImageListEnabled );
        toolbarCtrl.SetDisabledImageList( &m_toolbarImageListEnabled );

        RecalcLayout(FALSE);

        // Delete the empty button off the static toolbar.  Can't delete this on the
        // resource 'cos it'll crash if you try to compile it that way
        toolbarCtrl.DeleteButton(FIRST_BUTTON_INDEX);

        // add buttons
        for (int i = FIRST_BUTTON_INDEX; i < NUM_BUTTONS; i++)
        {
            addButton(BUTTON_INFO[i].name, BUTTON_INFO[i].iconId, BUTTON_INFO[i].command);
        }

		// TD16538: Start receiving message informing GUI when system is degraded/recovered
		LOG_GENERIC(
				SourceInfo, DebugUtil::DebugInfo,
				"[TD16068]Starting online update subscriptions");
		m_groupUpdateSubscriber->startNotifications();
		// ++TD16358

	    FUNCTION_EXIT;
        return 0;
    }


    void CMainFrame::addButton(const CString& buttonName, const unsigned long iconId, const int command )
    {
	    FUNCTION_ENTRY( "addButton" );
        CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();

        TBBUTTON tbButton;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_BUTTON;
        tbButton.dwData = 0;
        tbButton.iString = toolbar.AddStrings(buttonName);

        HICON buttonIcon = AfxGetApp()->LoadIcon(iconId);

        // We add the coloured icon first so we make sure we're converting the icon of the correct
        // size into grey.
        tbButton.iBitmap = m_toolbarImageListHot.Add( buttonIcon );
    
        // Convert the coloured icon to grey for the enabled icon
        HICON greyIcon = createGreyscaleIcon( m_toolbarImageListHot.ExtractIcon(tbButton.iBitmap) );

        if (greyIcon == NULL)
        {
            // If we didn't create a grey icon then just use the coloured one.
            greyIcon = buttonIcon;
        }

        m_toolbarImageListEnabled.Add( greyIcon );

        tbButton.idCommand = command;
        TA_VERIFY(toolbar.AddButtons(1, &tbButton),"Could not add button to the toolbar control");

        // Now associate the button name with the button so we can retrieve it later. Just adding
        // the name to the string table will ensure the text displays on the button but it is
        // then difficult to retrieve the text later on
        m_wndToolBar.SetButtonText( m_wndToolBar.CommandToIndex(command), buttonName );
	    FUNCTION_EXIT;
    }
    

    HICON CMainFrame::createGreyscaleIcon(HICON colourIcon)
    {
        FUNCTION_ENTRY("createGreyscaleIcon");

	    ICONINFO	csII;
	    BOOL bRetValue = ::GetIconInfo(colourIcon, &csII);
	    if (bRetValue == FALSE)
        {
            FUNCTION_EXIT;
		    return NULL;
        }

        HDC hMainDC = ::GetDC(m_hWnd);
	    HDC hMemDC1 = ::CreateCompatibleDC(hMainDC);
	    HDC hMemDC2 = ::CreateCompatibleDC(hMainDC);
	    if (hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL)
        {
            FUNCTION_EXIT;
		    return NULL;
        }

        ICONINFO    csGrayII;
	    BITMAP		bmp;
        HICON       coldIcon = NULL;
	    if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
	    {
		    csGrayII.hbmColor = ::CreateBitmap(csII.xHotspot*2, csII.yHotspot*2, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
		    if (csGrayII.hbmColor)
		    {
			    HBITMAP hOldBmp1 = reinterpret_cast<HBITMAP>(::SelectObject(hMemDC1, csII.hbmColor));
			    HBITMAP hOldBmp2 = reinterpret_cast<HBITMAP>(::SelectObject(hMemDC2, csGrayII.hbmColor));

			    ::BitBlt(hMemDC2, 0, 0, csII.xHotspot*2, csII.yHotspot*2, hMemDC1, 0, 0, SRCCOPY);

			    for (DWORD dwLoopY = 0; dwLoopY < csII.yHotspot*2; dwLoopY++)
			    {
				    for (DWORD dwLoopX = 0; dwLoopX < csII.xHotspot*2; dwLoopX++)
				    {
                        COLORREF crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);

                        BYTE byNewPixel = static_cast<BYTE>(((GetRValue(crPixel) * 0.299) + (GetGValue(crPixel) * 0.587) + (GetBValue(crPixel) * 0.114)));
					    if (crPixel)
                        {
                            ::SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
                        }
				    } // for
			    } // for
			    ::SelectObject(hMemDC1, hOldBmp1);
			    ::SelectObject(hMemDC2, hOldBmp2);

			    csGrayII.hbmMask = csII.hbmMask;

			    csGrayII.fIcon = TRUE;
			    coldIcon = ::CreateIconIndirect(&csGrayII);
		    } // if

		    ::DeleteObject(csGrayII.hbmColor);
	    } // if

	    ::DeleteObject(csII.hbmColor);
	    ::DeleteObject(csII.hbmMask);
	    ::DeleteDC(hMemDC1);
	    ::DeleteDC(hMemDC2);
	    ::ReleaseDC(m_hWnd, hMainDC);

        FUNCTION_EXIT;
        return coldIcon;
    }


    LRESULT CMainFrame::onJobSelectionChange(WPARAM theJob, LPARAM forceCheckRights)
    {
        JobPtrVector* jobs = reinterpret_cast<JobPtrVector*>(theJob);

        CToolBarCtrl& toolbarCtrl = m_wndToolBar.GetToolBarCtrl();
        CSchedulingManagerDoc* doc = dynamic_cast<CSchedulingManagerDoc*>(GetActiveDocument());
        TA_ASSERT(doc != NULL, "Unable to get the SchedulingManagerDoc");

        // we definitely need to check rights when it is forced or when we are doing it for the first time
        bool needToCheckRights = (0 != forceCheckRights) || (m_lastSelectedJobs == NULL) || (jobs == NULL);

        // if it doesn't force rights checking and it is not the first time that it's performed, then figure
        // out whether the old and new lists are the same
        if (!needToCheckRights && (jobs != NULL) && (m_lastSelectedJobs != NULL))
        {
            // check to see if the size is the same.  If it is different, then we definitely
            // need to check rights.  Otherwise, we have to iterate through the list to 
            // figure out if they are identical
            if (jobs->size() == m_lastSelectedJobs->size())
            {
                for (unsigned int i = 0; i < m_lastSelectedJobs->size(); i++)
                {
                    if (std::find(jobs->begin(), jobs->end(), (*m_lastSelectedJobs)[i]) == jobs->end())
                    {
                        // if we can't find a particular job, then the lists must be different
                        needToCheckRights = true;
                        break;
                    }
                }
            }
            else
            {
                // size is different, so we definitely have to check rights again
                needToCheckRights = true;
            }
        }

        if (!needToCheckRights)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No need to check rights again as the job selection is still the same");
            return 0;
        }

        if (m_lastSelectedJobs != NULL)
        {
            // delete the old selections cache
            delete m_lastSelectedJobs;
            m_lastSelectedJobs = jobs;
        }

		//TD16358++
		// get the new states for the toolbar buttons
		for (int i = 0; i < NUM_BUTTONS; i++)
		{
			if(bGroupOffline==false)
			{	
				BUTTON_INFO[i].enable = doc->isActionOnJobPermitted(BUTTON_INFO[i].action, jobs);
			}
			else
			{
				BUTTON_INFO[i].enable = false;
			}
		}
		//++TD16358
        return 0;
    }

	//TD16358++
    LRESULT CMainFrame::onSystemInNormalMode(WPARAM, LPARAM)
    {
		BUTTON_INFO[TA_Base_App::SchedulingManager::Add].enable = true;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Delete].enable = true;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Edit].enable = true;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Enable].enable = true;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Disable].enable = true;
		bGroupOffline = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD16358]Occ group now online!");
        return 0;
    }


    LRESULT CMainFrame::onSystemInDegradedMode(WPARAM, LPARAM)
    {
		BUTTON_INFO[TA_Base_App::SchedulingManager::Add].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Delete].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Edit].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Enable].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Disable].enable = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD16358]Occ group now offline!");
		bGroupOffline = true;
        return 0;
    }
	//++TD16358

    LRESULT CMainFrame::onStatusTextChange(WPARAM text, LPARAM)
    {
        // just return if text is NULL
        if (text == NULL)
        {
            return 0;
        }

		m_statusBarString = *(reinterpret_cast<CString*>(text));
        m_wndStatusBar.SetWindowText(m_statusBarString);

        /*CString* status = reinterpret_cast<CString*>(text);
        TA_ASSERT(status != NULL, "The WPARAM must be a CString");

        m_wndStatusBar.SetWindowText(*status);

        delete status;
        status = NULL;*/
        return 0;
    }
    

    void CMainFrame::onUpdateJobAdd(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Add].enable);
    }


    void CMainFrame::onUpdateJobDelete(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Delete].enable);
    }


    void CMainFrame::onUpdateViewJobdetails(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::View].enable);
    }


    void CMainFrame::onUpdateJobEdit(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Edit].enable);
    }


    void CMainFrame::onUpdateJobEnable(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Enable].enable);
    }


    void CMainFrame::onUpdateJobDisable(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Disable].enable);
    }


    void CMainFrame::onUpdateFilePrint(CCmdUI* pCmdUI) 
    {
        TA_ASSERT(pCmdUI != NULL, "CCmdUI Pointer is NULL");
        pCmdUI->Enable(BUTTON_INFO[TA_Base_App::SchedulingManager::Print].enable);
    }

    LRESULT CMainFrame::onRightsAgentDied(WPARAM, LPARAM)
    {
		// TD #13519
		// If the Rights Agent has died, disable all the toolbar buttons
		// that require rights permission to use
		BUTTON_INFO[TA_Base_App::SchedulingManager::Add].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Delete].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::View].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Edit].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Enable].enable = false;
		BUTTON_INFO[TA_Base_App::SchedulingManager::Disable].enable = false;
		return 0;
	}

	LRESULT CMainFrame::onSetMessageString(WPARAM wParam, LPARAM lParam)
	{
		UINT nIDLast = m_nIDLastMessage;

		CString sMsg;
		CString strMessage;

		if (wParam != 0)
		{
            if (wParam == AFX_IDS_IDLEMESSAGE)
			{
                sMsg = m_statusBarString;
			}
			else
			{
				GetMessageString(wParam, strMessage);
				sMsg = strMessage;
			}
		}

		m_wndStatusBar.SetWindowText(sMsg);

		m_nIDLastMessage = (UINT)wParam;
		m_nIDTracking = (UINT)wParam;

		return nIDLast;
	}

	void CMainFrame::initJob()
	{
		initialiseToolbar();
        CSchedulingManagerDoc* doc = dynamic_cast<CSchedulingManagerDoc*>(GetActiveDocument());
        TA_ASSERT(doc != NULL, "Unable to get SchedulingManagerDoc");
        doc->initialise();
	}
};

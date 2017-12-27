/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/saverwnd.cpp $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "stdafx.h"

#include <string>
#include <regstr.h>
#include <comdef.h>
#include <Objbase.h>

//lizettejl++
// TD12495 - The RippleControlStation COM object will not
// be used when verifying the user login for this application.
// ControlStation2 will be used instead
#import "ControlStation.tlb"

#include "app/system_control/transact_screensaver/src/Saver.h"
#include "app/system_control/transact_screensaver/src/saverwnd.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveMessage;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const CString CSaverWnd::NAME_SEPARATOR  = " / ";

    CSaverWnd::CSaverWnd()  
    {
    
    }

    
    CSaverWnd::~CSaverWnd()
    {
    }

    
    BOOL CSaverWnd::Create(HWND hwndParent)
    {
	    FUNCTION_ENTRY("Create");

	    CRect rect;
	    
	    m_dwPasswordDelay = 20;

	    const char* pszClassName 
            = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS|CS_DBLCLKS,NULL);

	    if (::GetSystemMetrics(SM_CMONITORS) == 0)
	    {
		    // Multiple display monitors not supported.
		    rect.SetRect(0, 0,
					     ::GetSystemMetrics(SM_CXSCREEN),
					     ::GetSystemMetrics(SM_CYSCREEN));
	    }
	    else
	    {
		    // Multiple display monitors supported.
		    const int nLeft = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		    const int nTop = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
		    const int nRight = nLeft + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		    const int nBottom = nTop + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		    rect.SetRect(nLeft, nTop, nRight, nBottom);
	    }
  
	    FUNCTION_EXIT;
        return CWnd::CreateEx( WS_EX_TOPMOST, pszClassName, "", WS_POPUP | WS_VISIBLE,
		    rect.left, rect.top, rect.Width(), rect.Height(), hwndParent, NULL );
    }

    
    BEGIN_MESSAGE_MAP(CSaverWnd, CWnd)
	    //{{AFX_MSG_MAP(CSaverWnd)
	    ON_WM_TIMER()
	    ON_WM_PAINT()
	    ON_WM_CREATE()
	    ON_WM_DESTROY()
	    ON_WM_CLOSE()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()
 

    int CSaverWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
	    FUNCTION_ENTRY("OnCreate");

	    m_bIsDialogActive = FALSE;
	    m_bReallyClose = FALSE;
 
	    for ( int nOp=0; nOp<=10; ++nOp )
        {
		    m_anRequestQueue.Add( nOp );
        }

	    SetTimer( 1, 20, NULL );
	    m_hBrushBackground = CreateSolidBrush(RGB(0, 0, 0));
	    
	    GetClientRect( &m_clientRect );
	    m_animationEngine.SetScreen( m_clientRect );	

  	    FUNCTION_EXIT;
	    return 0;
    }

    
    void CSaverWnd::OnDestroy()
    {
	    KillTimer(1);
	    if ( m_hBrushBackground )
        {
		    DeleteObject(m_hBrushBackground);
        }

	    m_animationEngine.DestroyGDIs();

 	    PostQuitMessage( 0 );
	    CWnd::OnDestroy();
    } 

    
    void CSaverWnd::OnClose()
    {
	    if (false == m_bReallyClose)
        {
		    return;
        }
	    
	    if ( FALSE == m_bIsDialogActive)
	    {            
 		    m_bIsDialogActive = TRUE;
 		    bool bCanClose = VerifyPassword(GetSafeHwnd());	
		    m_bIsDialogActive = FALSE;

		    if (false == bCanClose)
            {
 			    return;        
            }
	    }
 
	    CWnd::OnClose();
    }

    
    void CSaverWnd::CloseSaverWindow()
    {
 	    m_bReallyClose = TRUE;
 	    PostMessage(WM_CLOSE,0,0);
    }


    bool CSaverWnd::VerifyPassword(HWND hWndParent) 
    {
        if( false == isControlStationActivated() )
        {
            return true;
        }

        CWnd * pParent = CWnd::FromHandle(hWndParent);
        ASSERT_VALID(pParent);
	    
        bool bPasswdOK = false;
        if (m_loginDlg.DoModal() == IDOK)
        {
            if ( isPasswordValid(m_loginDlg.getPassword()) )
            {
                bPasswdOK = true;
            }
            else
            {   
                CString action =  "Please input the correct password.";
				TransActiveMessage userMsg;
                userMsg << action;
				userMsg.showMsgBox(IDS_UI_050063);
            }
        }
    
        return bPasswdOK;
    }


    bool CSaverWnd::isControlStationActivated() 
    {
   	    FUNCTION_ENTRY("isControlStationActivated");
       
        CMutex mutex( false, TA_Base_App::TA_ControlStation::APPLICATION_NAME );
        if ( TRUE == mutex.Lock( 100 ) )
        {
            // Control Station is not running...
            mutex.Unlock();
            return false;
        }

        bool bActivated = false;

	    try
	    {
		    // TD17837
		    //ControlStationLib::IControlStation2Ptr pAccount(_T("ControlStation.ControlStation2.1"));
            ControlStationLib::IControlStation2Ptr pAccount;
		    pAccount.CreateInstance("ControlStation.ControlStation2");
		    // TD17837
	        if (pAccount != NULL)
            {        
			    CString strOperator;
                CString strProfile;
                CString strLocation;
			    
                strOperator = _com_util::ConvertBSTRToString(pAccount->getOperator());

			    m_strOperator = (strOperator.Find(NAME_SEPARATOR) > 0) ? 
				    strOperator.Left(strOperator.Find(NAME_SEPARATOR)) : strOperator;
			     
                strLocation = _com_util::ConvertBSTRToString(pAccount->getLocation());

			    m_loginDlg.setLocation(strLocation.Find(NAME_SEPARATOR) > 0 ? 
                    strLocation.Left(strLocation.Find(NAME_SEPARATOR)) : strLocation);

                strProfile = _com_util::ConvertBSTRToString(pAccount->getProfile());

			    m_loginDlg.setProfile(strProfile.Find(NAME_SEPARATOR) > 0 ? 
                    strProfile.Left(strProfile.Find(NAME_SEPARATOR)) : strProfile);

			    m_loginDlg.setOperator(m_strOperator);
            }
		    // TD17837
	    }	 
	    catch( _com_error& )
	    {
		    LOG_EXCEPTION_CATCH( SourceInfo, "COM_Error", "Failed in creating instance of the COM object" );

	    }
	    catch(COleDispatchException& )
	    {
		    LOG_EXCEPTION_CATCH( SourceInfo, "COleDispatchException", "Failed in OLE Dispatch Interface" );
	    } 
	    catch(...)
	    {
		    //TD12495 - must report a log 
		    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "failed in CSaverWnd::isControlStationActivated()" );
	    }    
    
	    // check if all necessary info needed for an operator to relogin 
	    // has been successfully retrieved
	    if( true == m_loginDlg.isDialogInitialized() )
	    {
 		    bActivated = true;
	    }

 	    FUNCTION_EXIT;
        return bActivated;
    }


    bool CSaverWnd::isPasswordValid(const CString &strPWD)
    {
        bool bPassed = false;
	    
	    try
	    {
		    // TD17837
		    // ControlStationLib::IControlStation2Ptr pAccount(_T("ControlStation.ControlStation2.1"));
		    ControlStationLib::IControlStation2Ptr pAccount;
		    pAccount.CreateInstance("ControlStation.ControlStation2");
		    // TD17837
		    if (pAccount != NULL)
		    {			 
			    // TD17837
			    // bPassed = pAccount->isLoginAllowed(_com_util::ConvertStringToBSTR(m_strOperator), _com_util::ConvertStringToBSTR(strPWD));;                        
			    BSTR bstrOper = _com_util::ConvertStringToBSTR(m_strOperator.operator LPCTSTR() );
			    BSTR bstrPwd  = _com_util::ConvertStringToBSTR(strPWD.operator LPCTSTR());
			    bPassed = (FALSE != pAccount->isLoginAllowed(bstrOper, bstrPwd));
			    SysFreeString(bstrOper);
			    SysFreeString(bstrPwd);
			    pAccount.Release();         
			    // TD17837
		    }
	    }
	    catch(_com_error&)
	    {
		    LOG_EXCEPTION_CATCH( SourceInfo, "COM_Error", "Failed in creating instance of the COM object" );
	    }
	    catch(COleDispatchException&)
	    {
		    LOG_EXCEPTION_CATCH( SourceInfo, "COleDispatchException", "Failed in OLE Dispatch Interface" );
	    }
	    catch(...)
	    {
		    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "failed in CSaverWnd::isAllowedLogin()" );
	    }
	    
	    return bPassed;
    }


    void CSaverWnd::PostNcDestroy() 
    {
	    CWnd::PostNcDestroy();
	    delete this;
    }


    LRESULT CSaverWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
    {
        static BOOL bHere = FALSE;
        static POINT ptLast;
        POINT ptCursor, ptCheck;
	    CRect rectWnd;

        switch ( message )
	    {
        case WM_SYSCOMMAND:
            if ( (wParam == SC_SCREENSAVE) || (wParam == SC_CLOSE) )
            {
                return FALSE;
            }
            break;

        case WM_ERASEBKGND:
		    GetClientRect(&rectWnd);
		    FillRect( (HDC)wParam, &rectWnd, m_hBrushBackground );
		    break;

		    // not needed for this screen saver
        case WM_APP:
            break;

        case WM_SETCURSOR:
		    SetCursor(m_bIsDialogActive
 				    ? ::AfxGetApp()->LoadStandardCursor(IDC_ARROW) : NULL);
		    
            break;

        case WM_NCACTIVATE:
		    if ( FALSE == m_bIsDialogActive )
		    {
			    if (wParam == FALSE)
                {
				    return FALSE;			 
                }
		    }
		    break;

        case WM_ACTIVATE:
		    if ( FALSE == m_bIsDialogActive )
		    {
			    if (wParam == WA_INACTIVE)
			    {
				    CloseSaverWindow();
			    }
		    }
		    break;

        case WM_ACTIVATEAPP:
		    if ( FALSE == m_bIsDialogActive )
		    {
			    if (wParam == FALSE)
			    {
				    CloseSaverWindow(); //break;
			    }
		    }
 
        case WM_MOUSEMOVE:
		    if ( FALSE == m_bIsDialogActive )
		    {			
			    if( false == bHere )
			    {
				    GetCursorPos( &ptLast );
				    bHere = TRUE;
			    }
			    else
			    {
				    GetCursorPos( &ptCheck );
				    ptCursor.x = ptCheck.x - ptLast.x;
				    ptCursor.y = ptCheck.y - ptLast.y;
				    if ( ptCursor.x < 0 )
                    {
					    ptCursor.x *= -1;
                    }
				    
                    if ( ptCursor.y < 0 )
                    {
					    ptCursor.y *= -1;
                    }

				    if ( (ptCursor.x + ptCursor.y) > 5 )
				    {
					    CloseSaverWindow();
				    }
			    }
		    }
            break;

        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
 		    if (TRUE == m_bIsDialogActive)
            {
			    break;
            }

            GetCursorPos( &ptCursor );
            ptCursor.x ++;
            ptCursor.y ++;
            SetCursorPos( ptCursor.x, ptCursor.y );
            GetCursorPos( &ptCheck );
            if ( ptCheck.x != ptCursor.x && ptCheck.y != ptCursor.y )
            {
	            ptCursor.x -= 2;
            }

            ptCursor.y -= 2;
            SetCursorPos( ptCursor.x, ptCursor.y );

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
		    if ( TRUE == m_bIsDialogActive )
            {
			    break;
            }
            
            CloseSaverWindow();
            break;
        }

        return CWnd::WindowProc(message, wParam, lParam);
    }


    void CSaverWnd::LayoutText(CDC &dc, CRect rect)
    {
	    int nItems = m_animationEngine.RunFrame( dc );

	    if ( nItems < 1 )
	    {
            CPoint ptIntro = m_animationEngine.RandPt( 100 );
#ifdef KRTC_BUILD
            POSITION posImg = m_animationEngine.CreateItem( "" ).SetFrames( 100 ).SetBitmap( IDB_KRTC_TRANSACTIVE )
			.AlignCenter().SetStart( ptIntro.x, ptIntro.y - 70 )
			.SetFinishRel( 0, 26 ).FadeIn().GetPos();
#else
            POSITION posImg = m_animationEngine.CreateItem( "" ).SetFrames( 100 ).SetBitmap( IDB_C830_TRANSACTIVE )
			.AlignCenter().SetStart( ptIntro.x, ptIntro.y - 70 )
			.SetFinishRel( 0, 26 ).FadeIn().GetPos();
#endif

		    POSITION posImg2 = posImg;
		    if ( ! m_animationEngine.Rand(3) ) // move it to center once in 3 times
            {
			    posImg2 = m_animationEngine.CreateItemAfter( posImg )
				    .SetFinish( m_animationEngine.HCenter(), m_animationEngine.VCenter() )
				    .SetFrames( 20 )
				    .FadeOut()
				    .GetPos();
            }
	    }
    }


    void CSaverWnd::OnTimer(UINT nIDEvent) 
    {
	    if ( nIDEvent == 1 )
	    {	
		    CClientDC dc(this);
		    LayoutText( dc, m_clientRect );
	    }     
	    
	    CWnd::OnTimer(nIDEvent);
    }


    void CSaverWnd::OnPaint() 
    {
	    CPaintDC dc(this); // device context for painting
	    LayoutText( dc, m_clientRect );
	    // Do not call CDrawWnd::OnPaint() for painting messages
    }
}

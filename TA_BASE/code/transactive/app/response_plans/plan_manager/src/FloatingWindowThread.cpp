//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// A self-deleting, user interface thread used as the base for the
// Plan Controller and Plan Editor threads.

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "FloatingWindowThread.h"
#include "FloatingWindowThreadRegister.h"
#include "PlanBrowserDoc.h"
#include "TreeNodeFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CFloatingWindowThread

IMPLEMENT_DYNAMIC(CFloatingWindowThread, CWinThread)

CFloatingWindowThread::CFloatingWindowThread(TreeNodeId nodeId) : m_nodeId(nodeId), m_initEvent(FALSE, FALSE)
{
    FUNCTION_ENTRY( "CFloatingWindowThread" );

    m_bAutoDelete = true;

    FUNCTION_EXIT;
}


CFloatingWindowThread::~CFloatingWindowThread()
{
    FUNCTION_ENTRY( "~CFloatingWindowThread" );

    m_initEvent.Unlock();

    FUNCTION_EXIT;
}


BOOL CFloatingWindowThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
    FUNCTION_ENTRY( "CreateThread" );

    if (CWinThread::CreateThread(dwCreateFlags, nStackSize, lpSecurityAttrs))
    {
        // Wait until the thread has initialised its main window in order to enable messages to be
        // sent to it as soon as this method returns.
        CSingleLock initEventLock(&m_initEvent);
        initEventLock.Lock();

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


void CFloatingWindowThread::AdjustPosition()
{
	// Position the created frame centrally over the main frame, but make sure
	// the main frame is not covered entirely.
	CWnd *appMainWnd = AfxGetApp()->m_pMainWnd;
	if (NULL != appMainWnd)
	{
		CRect wndRect;
		appMainWnd->GetWindowRect(wndRect);
		
		wndRect.top += 30;
		wndRect.left += 30;
		wndRect.bottom -= 30;
		wndRect.right -= 30;
		
        if ( NULL != m_pMainWnd )
        {
            m_pMainWnd->MoveWindow(wndRect);
        }
	}
}


void CFloatingWindowThread::ShowMyWindow()
{
    FUNCTION_ENTRY( "ShowMyWindow" );

    if (m_pMainWnd && ::IsWindow(m_pMainWnd->GetSafeHwnd()))
    {
		
        if (!m_pMainWnd->IsWindowVisible())
        {
            m_pMainWnd->ShowWindow(SW_SHOW);
        }
        else if (m_pMainWnd->IsIconic())
        {
            m_pMainWnd->ShowWindow(SW_RESTORE);
        }
		
        m_pMainWnd->SetForegroundWindow();
        m_pMainWnd->BringWindowToTop();
    }
	
    FUNCTION_EXIT;
}


BOOL CFloatingWindowThread::InitInstance()
{
    FUNCTION_ENTRY( "InitInstance" );

    ::SetThreadLocale( MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED) );

    BOOL initResult = FALSE;

    CDocTemplate *documentTemplate = GetDocumentTemplate();
    TA_ASSERT(documentTemplate, "Document template pointer is null");

    m_docManager.AddDocTemplate(documentTemplate);

    // Allow the created frame to be assigned as the thread's main window.
    m_pMainWnd = NULL;

    // Now create the frame.
    m_docManager.OnFileNew();

    if (m_pMainWnd)
    {
		AdjustPosition();

        // The frame traps its initial activation when created to allow us to reposition
        // it transparently. Once moved the window needs to be made visible.
        ShowMyWindow();

        initResult = TRUE;
    }

    // Release the lock in CreateThread()
    m_initEvent.SetEvent();

    FUNCTION_EXIT;
    return initResult;
}


int CFloatingWindowThread::ExitInstance()
{
    FUNCTION_ENTRY( "ExitInstance" );

    // TODO:  perform any per-thread cleanup here
    if (m_pMainWnd)
    {
        m_pMainWnd->DestroyWindow();
    }

    m_docManager.CloseAllDocuments(TRUE);

    FloatingWindowThreadRegister::getInstance().deregisterThread(this);

    FUNCTION_EXIT;
    return CWinThread::ExitInstance();
}


BOOL CFloatingWindowThread::OnIdle(LONG lCount)
{
    FUNCTION_ENTRY( "OnIdle" );

    // Imitate what CWinApp does on idle
    if (lCount <= 0)
    {
        CWinThread::OnIdle(lCount);

        POSITION pos = m_docManager.GetFirstDocTemplatePosition();
        while (pos != NULL)
        {
            CDocTemplate *docTemplate = m_docManager.GetNextDocTemplate(pos);

            if (docTemplate)
            {
                docTemplate->OnIdle();
            }
        }
    }
    else if (lCount == 1)
    {
        CWinThread::OnIdle(lCount);
    }

    FUNCTION_EXIT;
    return lCount < 1;  // more to do if lCount < 1
}


BEGIN_MESSAGE_MAP(CFloatingWindowThread, CWinThread)
    // {{AFX_MSG_MAP(CFloatingWindowThread)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloatingWindowThread message handlers

void CFloatingWindowThread::SetNodeId(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "SetNodeId" );

    m_nodeId = nodeId;

    FUNCTION_EXIT;
}


TreeNodeId CFloatingWindowThread::GetNodeId() const
{
    FUNCTION_ENTRY( "GetNodeId" );
    FUNCTION_EXIT;
    return m_nodeId;
}


void CFloatingWindowThread::ActivateWindow()
{
    FUNCTION_ENTRY( "ActivateWindow" );

	// Show Window
    ShowMyWindow();
	// Adjust Window
	AdjustPosition();
	// Show Window(restore current position).
	ShowMyWindow();

    FUNCTION_EXIT;
}


void CFloatingWindowThread::Terminate()
{
    FUNCTION_ENTRY( "Terminate" );

    if ( NULL != m_pMainWnd )
    {
        m_pMainWnd->SendMessage(WM_CLOSE);
    }

    FUNCTION_EXIT;
}



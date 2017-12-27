// HostsTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"

#include "Resource.h"
#include "ProcessListCtrl.h"
#include "AdminManager.h"
#include "HostsTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HostsTabCtrl
namespace TA_Base_App
{

HostsTabCtrl::HostsTabCtrl() : CTabCtrl(),
m_processLists(),
m_pCurActiveProcessList( NULL ),
m_pCurActiveAdminManager( NULL )
{
}

HostsTabCtrl::~HostsTabCtrl()
{
}

BEGIN_MESSAGE_MAP(HostsTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(HostsTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HostsTabCtrl message handlers

void HostsTabCtrl::initTabs( const std::vector<std::string>& vecHostNames )
{
    int nID = 10000;
    CFont* pFont = GetFont();
    for ( std::vector<std::string>::const_iterator itLoop = vecHostNames.begin();
          vecHostNames.end() != itLoop; ++itLoop )
    {
        int nFirstPos = itLoop->find( '|' );
        int nSecondPos = itLoop->find( '|', nFirstPos + 1 );
        std::string strServerName = itLoop->substr( 0, nFirstPos );
        std::string strHostName = itLoop->substr( nFirstPos + 1, nSecondPos - nFirstPos - 1 );;
        std::string strEntityKey = itLoop->substr( nSecondPos + 1, itLoop->length() - nSecondPos - 1 );
        ++nID;
        ProcessListCtrl* pNewProcessListCtrl = new ProcessListCtrl();
        TA_ASSERT( NULL != pNewProcessListCtrl, "Can't create instance of ProcessListCtrl" );

        RECT rtListRect;
        GetClientRect( &rtListRect );
        rtListRect.top += 25;
        pNewProcessListCtrl->Create( WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SORTASCENDING | LVS_ALIGNTOP | LVS_SHOWSELALWAYS, rtListRect, this, IDC_MAIN_PROCESSLIST );
        pNewProcessListCtrl->init( strHostName );
        pNewProcessListCtrl->SetFont( pFont, FALSE );
        pNewProcessListCtrl->EnableWindow( FALSE );
        pNewProcessListCtrl->ShowWindow( SW_HIDE );

        int nInsertTabIndex = GetItemCount();
        std::stringstream stmConvertor(strEntityKey);
        unsigned long ulEntityKey = 0;
        stmConvertor >> ulEntityKey;
        AdminManager* pNewAdminManager = new AdminManager( strServerName, ulEntityKey );
        TA_ASSERT( NULL != pNewAdminManager, "Can't create instance of AdminManager" );
        ProcessListAdmin* pNewProcessListAdmin = new ProcessListAdmin();
        TA_ASSERT( NULL != pNewProcessListAdmin, "Can't create instance of ProcessListAdmin" );

        pNewProcessListAdmin->pAdminManager = pNewAdminManager;
        pNewProcessListAdmin->pProcessListCtrl = pNewProcessListCtrl;
        m_processLists[nInsertTabIndex] = pNewProcessListAdmin;

        InsertItem( nInsertTabIndex, strServerName.c_str() );

//         pNewProcessListCtrl->InsertItem( 0, "You are so good" );
//         pNewProcessListCtrl->InsertItem( 1, "You are so good or not" );
//         pNewProcessListCtrl->SetItemText( 0, 3, "Running Control" );
//         pNewProcessListCtrl->SetItemText( 1, 3, "Running Monitor" );
//         if ( nID > 10003 )
//         {
//             break;
//         }
    }
    
    int nCurSel = GetCurSel();
    m_pCurActiveProcessList = m_processLists[nCurSel]->pProcessListCtrl;
    m_pCurActiveAdminManager = m_processLists[nCurSel]->pAdminManager;
    m_pCurActiveProcessList->ShowWindow( SW_SHOW );
    m_pCurActiveProcessList->EnableWindow( TRUE );
}

ProcessListCtrl* HostsTabCtrl::getActiveProcessListCtrl()
{
    return m_pCurActiveProcessList;
}

AdminManager* HostsTabCtrl::getActiveAdminManager()
{
    return m_pCurActiveAdminManager;
}

ProcessListCtrl* HostsTabCtrl::getAdminBindProcessList( const AdminManager* const pAdminManaer )
{
    if ( NULL == pAdminManaer )
    {
        return NULL;
    }

    for ( std::map<int, ProcessListAdmin*>::iterator itFound = m_processLists.begin();
          m_processLists.end() != itFound; ++itFound )
    {
        if ( pAdminManaer == itFound->second->pAdminManager )
        {
            return itFound->second->pProcessListCtrl;
        }
    }

    return NULL;
}

void HostsTabCtrl::highlightAdminBindTab( const AdminManager* const pAdminManaer, bool bHightlight )
{
    if ( NULL == pAdminManaer )
    {
        return;
    }

    for ( std::map<int, ProcessListAdmin*>::iterator itFound = m_processLists.begin();
    m_processLists.end() != itFound; ++itFound )
    {
        if ( pAdminManaer == itFound->second->pAdminManager )
        {
            HighlightItem( itFound->first, bHightlight ? TRUE : FALSE );
            break;
        }
    }
}

void HostsTabCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void HostsTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if ( NULL != m_pCurActiveProcessList )
    {
//         m_pCurActiveProcessList->freeze();
        m_pCurActiveProcessList->EnableWindow( FALSE );
        m_pCurActiveProcessList->ShowWindow( SW_HIDE );
    }

    int nCurSel = GetCurSel();
    m_pCurActiveProcessList = m_processLists[nCurSel]->pProcessListCtrl;
    m_pCurActiveAdminManager = m_processLists[nCurSel]->pAdminManager;
    if ( NULL != m_pCurActiveProcessList )
    {
        m_pCurActiveProcessList->ShowWindow( SW_SHOW );
        m_pCurActiveProcessList->EnableWindow( TRUE );
    }
//     m_pCurActiveProcessList->unfreeze();

    AdminManager::getMainWnd()->PostMessage( WM_TAB_SEL_CHANGED, reinterpret_cast<WPARAM>(m_pCurActiveAdminManager), reinterpret_cast<LPARAM>(m_pCurActiveProcessList) );
	*pResult = 0;
}

void HostsTabCtrl::clearProcessList()
{
    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pProcessListCtrl->DeleteAllItems();
    }
}

int HostsTabCtrl::login( const std::string& portNumber, const std::string& sessionId, const std::string& operatorName )
{
    int nResult = AdminManager::loginSetup( portNumber, sessionId, operatorName );
    if ( ERR_AUTH_NO_ERROR != nResult )
    {
        return nResult;
    }

    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pAdminManager->start();
    }

    return nResult;
}

void HostsTabCtrl::logout()
{
    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pAdminManager->terminate();
    }
    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pAdminManager->logout();
    }
}

void HostsTabCtrl::retryLoginAllFailed()
{
    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pAdminManager->retryLogin();
    }
}

void HostsTabCtrl::checkPermissions()
{
    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pAdminManager->checkPermissions();
    }
}

void HostsTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CTabCtrl::OnSize(nType, cx, cy);

    RECT rtListRect;
    GetClientRect( &rtListRect );
    rtListRect.top += 25;

    for ( std::map<int, ProcessListAdmin*>::iterator itLoop = m_processLists.begin(); 
          m_processLists.end() != itLoop; ++itLoop )
    {
        itLoop->second->pProcessListCtrl->MoveWindow( &rtListRect );
    }
}

void HostsTabCtrl::unHighlightAllTabs()
{
    int nItemCount = GetItemCount();
    for ( int nLoop = 0; nLoop < nItemCount; ++nLoop )
    {
        HighlightItem( nLoop, FALSE );
    }
}

}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the main Region Manager dialog window.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int RGN_MGR_DEFAULT_WIDTH(900);
    const int RGN_MGR_DEFAULT_HEIGHT(600);
    const int RGN_MGR_MINIMUM_WIDTH(400);
    const int RGN_MGR_MINIMUM_HEIGHT(300);
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::TransActiveDialog;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// RegionManagerDlg dialog

RegionManagerDlg::RegionManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback) :
    TransActiveDialog(genericGUICallback, RegionManagerDlg::IDD),
    IConfigDataObserver(DutyDataStore::getDataCache())
{
    FUNCTION_ENTRY("RegionManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback)");

	//{{AFX_DATA_INIT(RegionManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_REGIONMANAGER_ICON);

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxWidth = -1;
    properties.maxHeight = -1;
    properties.minWidth = RGN_MGR_MINIMUM_WIDTH;
    properties.minHeight = RGN_MGR_MINIMUM_HEIGHT;

    setResizingProperties(properties);

    FUNCTION_EXIT;
}


RegionManagerDlg::~RegionManagerDlg()
{
    FUNCTION_ENTRY("~RegionManagerDlg()");
    FUNCTION_EXIT;
}


void RegionManagerDlg::dataChanged(const TA_Base_Bus::DataUpdate& dataUpdate)
{
    typedef std::map<TA_Base_Core::EModificationType, std::string> ActionMap;
    typedef std::map<TA_Base_Core::EObjectType, std::string> ItemMap;

    static ActionMap actionMap;
    static ItemMap itemMap;

    if (actionMap.empty())
    {
        actionMap.insert(std::make_pair(TA_Base_Core::Create, std::string("added")));
        actionMap.insert(std::make_pair(TA_Base_Core::Update, std::string("modified")));
        actionMap.insert(std::make_pair(TA_Base_Core::Delete, std::string("deleted")));
    }

    if (itemMap.empty())
    {
        itemMap.insert(std::make_pair(TA_Base_Core::OPERATOR, std::string("Operator")));
        itemMap.insert(std::make_pair(TA_Base_Core::PROFILE, std::string("Profile")));
        itemMap.insert(std::make_pair(TA_Base_Core::LOCATION, std::string("Location")));
        itemMap.insert(std::make_pair(TA_Base_Core::REGION, std::string("Region")));
        itemMap.insert(std::make_pair(TA_Base_Core::SUBSYSTEM, std::string("Subsystem")));
    }

    ActionMap::const_iterator actionIt(actionMap.find(dataUpdate.updateType));
    ItemMap::const_iterator itemIt(itemMap.find(dataUpdate.itemType));

    if (actionIt == actionMap.end() || itemIt == itemMap.end())
    {
        return;
    }

    std::ostringstream msg;
    msg << itemIt->second << " configuration data has been " << actionIt->second << "." << std::endl
        << TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME) << " should be restarted for any" << std::endl
        << "changes to become visible.";

    MessageBox::warning(msg.str());
}


void RegionManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RegionManagerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// RegionManagerDlg message handlers

BEGIN_MESSAGE_MAP(RegionManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(RegionManagerDlg)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL RegionManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog()");

	TransActiveDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialise the tabs.
    m_regionPropertySheet.AddPage(&m_activeOpsPropertyPage);
    m_regionPropertySheet.AddPage(&m_subsystemsPropertyPage);
    m_regionPropertySheet.AddPage(&m_allOpsPropertyPage);
	m_regionPropertySheet.Create(this, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

    // Set the initial dimensions of the dialog. This will also resize the tabs.
    SetWindowPos(NULL, 0, 0, RGN_MGR_DEFAULT_WIDTH, RGN_MGR_DEFAULT_HEIGHT, SWP_NOZORDER | SWP_NOMOVE);

    FUNCTION_EXIT;	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void RegionManagerDlg::OnAppAbout()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void RegionManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		TransActiveDialog::OnPaint();
	}
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR RegionManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void RegionManagerDlg::OnSize(UINT nType, int cx, int cy) 
{
	TransActiveDialog::OnSize(nType, cx, cy);
	
	// Resize the tabs and their contents.
	if (::IsWindow(m_regionPropertySheet.GetSafeHwnd()))
    {
        CRect propSheetRect;

        m_regionPropertySheet.GetWindowRect(propSheetRect);
        ScreenToClient(propSheetRect);
        propSheetRect.left = propSheetRect.top = 0;
        propSheetRect.right = cx - 5;
        propSheetRect.bottom = cy - 5;
        m_regionPropertySheet.MoveWindow(propSheetRect);  // The prop sheet will size the pages
    }
}

void RegionManagerDlg::OnAppExit() 
{
	// TODO: Add your command handler code here
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();
}

void RegionManagerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();
}

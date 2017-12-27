/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/EditShortcutDialog.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the dialog for user to edit the shortcuts associated with the session
  * 
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/ControlStation.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "app/system_control/control_station/src/EditShortcutDialog.h"
#include "bus\generic_gui\src\HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/ScadaDisplay.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_App::TA_ControlStation::APPLICATION_NAME;
using TA_Base_App::TA_ControlStation::MAX_SHORTCUT;
using TA_Base_App::TA_ControlStation::DEFAULT_SHORTCUT_PREFIX;

const int EditShortcutDialog::INDEX_COLUMN = 0;
const int EditShortcutDialog::NAME_COLUMN = 1;
const int EditShortcutDialog::DISPLAY_COLUMN = 2;

/////////////////////////////////////////////////////////////////////////////
// EditShortcutDialog dialog


EditShortcutDialog::EditShortcutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(EditShortcutDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(EditShortcutDialog)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void EditShortcutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditShortcutDialog)
	DDX_Control(pDX, IDC_REMOVE_SHORTCUT, m_removeShortcutCtrl);
	DDX_Control(pDX, IDC_MOVE_UP, m_moveUpCtrl);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_moveDownCtrl);
	DDX_Control(pDX, IDC_SET_SHORTCUT, m_setShortcutCtrl);
	DDX_Control(pDX, IDC_DISPLAY_COMBO, m_display);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_SHORTCUT_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditShortcutDialog, CDialog)
	//{{AFX_MSG_MAP(EditShortcutDialog)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_ADD_SHORTCUT, OnAddShortcut)
	ON_BN_CLICKED(IDC_REMOVE_SHORTCUT, OnRemoveShortcut)
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SHORTCUT_LIST, OnGetdispinfoShortcutList)
	ON_NOTIFY(NM_CLICK, IDC_SHORTCUT_LIST, OnClickShortcutList)
	ON_BN_CLICKED(IDC_SET_SHORTCUT, OnSetShortcut)
    ON_MESSAGE(WM_SCADA_DISPLAY_CHANGE, onScadaDisplayChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void EditShortcutDialog::initialise(ShortcutVector& shortcuts,
                                    std::vector<TA_Base_Core::IScadaDisplay*>& allDisplays,
                                    const int selection)
{
    FUNCTION_ENTRY("initialise");
    cleanUp();
    for (unsigned int i = 0; i < shortcuts.size(); i++)
    {
        ShortcutInformation* item = new struct ShortcutInformation;
        item->name = shortcuts[i]->name;
        item->display = shortcuts[i]->display;
        m_shortcuts.push_back(item);
    }
    m_allDisplays = &allDisplays;   // references to the scada display vector
    m_selection = selection;
    FUNCTION_EXIT;
}


void EditShortcutDialog::cleanUp()
{
    FUNCTION_ENTRY("cleanUp");
    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
        delete m_shortcuts[i];
        m_shortcuts[i] = NULL;
    }
    m_shortcuts.clear();
    FUNCTION_EXIT;
}


BOOL EditShortcutDialog::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog");
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Add "About..." menu item to system menu.
    // Remove the "Close" menu item from the system menu
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
	    TA_VERIFY(strAboutMenu.LoadString(IDS_ABOUTBOX)!=0, "Unable to load IDS_ABOUTBOX");
	    if (!strAboutMenu.IsEmpty())
	    {
		    pSysMenu->AppendMenu(MF_SEPARATOR);
		    pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	    }
    }

    initialiseDisplayCombo();

    // initialise the shortcuts list control
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_INFOTIP);
	m_listCtrl.InsertColumn (INDEX_COLUMN, " ", LVCFMT_LEFT);
	m_listCtrl.InsertColumn (NAME_COLUMN, "Name", LVCFMT_LEFT);
	m_listCtrl.InsertColumn (DISPLAY_COLUMN, "Display", LVCFMT_LEFT);

    // Add the item to the list view.
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_PARAM; 
    lvi.iSubItem = 0; 
    lvi.iImage = 0;
    lvi.pszText = LPSTR_TEXTCALLBACK; 

    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
		lvi.iItem = i;
		lvi.lParam = (LPARAM)m_shortcuts[i];
		m_listCtrl.InsertItem(&lvi);
	}

    m_listCtrl.SetColumnWidth(INDEX_COLUMN, LVSCW_AUTOSIZE_USEHEADER );

    // If there's no item, then size the columns based on the size of the headers.  Otherwise
    // size the columns to fit the contents
    if (m_shortcuts.size() == 0)
    {
        m_listCtrl.SetColumnWidth(NAME_COLUMN, LVSCW_AUTOSIZE_USEHEADER);
        m_listCtrl.SetColumnWidth(DISPLAY_COLUMN, LVSCW_AUTOSIZE_USEHEADER );
    }
    else
    {
        m_listCtrl.SetColumnWidth(NAME_COLUMN, 200);  
        m_listCtrl.SetColumnWidth(DISPLAY_COLUMN, 400);
    }

	m_listCtrl.SetItemState(m_selection, LVIS_SELECTED, LVIS_SELECTED);
    m_name.SetLimitText(TA_Base_App::TA_ControlStation::SHORTCUT_NAME_MAX_LENGTH);
    populateShortcutDetails();

    // Make the Window always on top.
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// EditShortcutDialog message handlers

void EditShortcutDialog::OnMoveUp() 
{
    FUNCTION_ENTRY("OnMoveUp");
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
    {
		return;
    }
    int index = m_listCtrl.GetNextSelectedItem(pos);
    swapItems(index, false);
    FUNCTION_EXIT;
}


void EditShortcutDialog::OnMoveDown() 
{
    FUNCTION_ENTRY("OnMoveDown");
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
    {
		return;
    }
    int index = m_listCtrl.GetNextSelectedItem(pos);
    swapItems(index, true);
    FUNCTION_EXIT;
}


void EditShortcutDialog::OnAddShortcut() 
{
    FUNCTION_ENTRY("OnAddShortcut");
    unsigned int index = m_shortcuts.size();
    if (index >= MAX_SHORTCUT)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << MAX_SHORTCUT;
        userMsg.showMsgBox(IDS_UE_020065);
        return;
    }

    ShortcutInformation* newShortcut = new struct ShortcutInformation;
    newShortcut->name = getDefaultShortcutName();
    newShortcut->display = NULL;
    m_shortcuts.push_back(newShortcut);

    // Add the item to the list view.
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_PARAM; 
    lvi.iSubItem = 0; 
    lvi.iImage = 0;
    lvi.pszText = LPSTR_TEXTCALLBACK; 
	lvi.iItem = index;
	lvi.lParam = (LPARAM)m_shortcuts[index];
	m_listCtrl.InsertItem(&lvi);

	m_listCtrl.SetItemState( index , LVIS_SELECTED, LVIS_SELECTED);
	m_listCtrl.EnsureVisible(index, FALSE);
    m_listCtrl.Invalidate();
    populateShortcutDetails();

    FUNCTION_EXIT;
}


void EditShortcutDialog::OnRemoveShortcut() 
{
    FUNCTION_ENTRY("OnRemoveShortcut");
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
    {
		return;
    }
    int shortcutIndex = m_listCtrl.GetNextSelectedItem(pos);

    ShortcutInformation* item = reinterpret_cast<ShortcutInformation*>(m_listCtrl.GetItemData(shortcutIndex));

    m_listCtrl.Invalidate();
    // delete shortcut from the internal shortcut list
    for (ShortcutVector::iterator iter = m_shortcuts.begin();
         iter != m_shortcuts.end();
         iter++)
    {
        if ((*iter) == item)
        {
            delete item;
            item = NULL;
            m_shortcuts.erase(iter);
            break;
        }
    }            

     // delete the shortcut from the list control
	m_listCtrl.DeleteItem(shortcutIndex);

    m_listCtrl.Invalidate();
    if (static_cast<unsigned int>(shortcutIndex) >= m_shortcuts.size())  // if it was the last item, then we need to set focus to the next item
    {
        shortcutIndex = m_shortcuts.size() - 1;
    }
	m_listCtrl.SetItemState(shortcutIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_listCtrl.EnsureVisible(shortcutIndex, FALSE);
    populateShortcutDetails();

    FUNCTION_EXIT;
}


void EditShortcutDialog::OnHelpButton() 
{
    FUNCTION_ENTRY("OnHelpButton");
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}


void EditShortcutDialog::OnGetdispinfoShortcutList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    LV_ITEM* pItem = &(pDispInfo)->item;

    int iItemIndx = pItem->iItem;

    if (pItem->mask & LVIF_TEXT) //valid text buffer?
    {
        switch(pItem->iSubItem)
        {
        case INDEX_COLUMN: //fill in the name of shortcut
            {
                CString str;
                str.Format("%d", ++iItemIndx);
                lstrcpy(pItem->pszText, str);
            }
            break;
        case NAME_COLUMN: //fill in the name of shortcut
            lstrcpy(pItem->pszText, m_shortcuts[iItemIndx]->name.c_str());
            break;
        case DISPLAY_COLUMN: //fill in the path to shortcut
            try
            {
                if (m_shortcuts[iItemIndx]->display != NULL)
                {
                    lstrcpy(pItem->pszText, m_shortcuts[iItemIndx]->display->getName().c_str());
                }
            }
            catch (...)
            {
                // some error has occurred, just ignore
            }
            break;
        default:
            break;
        }
    }
	
	*pResult = 0;
}


void EditShortcutDialog::OnClickShortcutList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnClickShortcutList");

    // put the details of the selected shorcut to the Details pane
    populateShortcutDetails();
	*pResult = 0;
    FUNCTION_EXIT;
}


void EditShortcutDialog::populateShortcutDetails()
{
    FUNCTION_ENTRY("populateShortcutDetails");

    // if no shortcut is selected, then disable all controls that require a shortcut to be selected
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
    {
        activateShortcutControls(false);
		return;
    }
    int index = m_listCtrl.GetNextSelectedItem(pos);

    // enable all controls so that the shortcut could be modified
    activateShortcutControls(true);

    m_name.SetWindowText(m_shortcuts[index]->name.c_str());

    if (m_shortcuts[index]->display != NULL)
    {
        try
        {
            m_display.SetCurSel(m_display.FindString(0, m_shortcuts[index]->display->getName().c_str()));
        }
        catch (...)
        {
            m_display.SetCurSel(CB_ERR);
        }
    }
    else
    {
        m_display.SetCurSel(CB_ERR);
    }


    // set focus to the name text box
    m_name.SetFocus();

    FUNCTION_EXIT;
}


void EditShortcutDialog::activateShortcutControls(bool enable)
{
    m_name.EnableWindow(enable);
    m_display.EnableWindow(enable);
    m_setShortcutCtrl.EnableWindow(enable);
    m_removeShortcutCtrl.EnableWindow(enable);
    m_moveUpCtrl.EnableWindow(enable);
    m_moveDownCtrl.EnableWindow(enable);
}


void EditShortcutDialog::OnSetShortcut() 
{
    FUNCTION_ENTRY("OnSetShortcut");
    POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
    {
        // just ignore if nothing is selected - we shouldn't be able to get here
		return;
    }
    int shortcutIndex = m_listCtrl.GetNextSelectedItem(pos);

    // check that both name and display are filled in
    CString name;
    m_name.GetWindowText(name);
    if (name.IsEmpty())
    {
        CString itemName;
        TA_VERIFY(itemName.LoadString(IDS_SHORTCUT_NAME)!=0, "Unable to load IDS_SHORTCUT_NAME");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << itemName;
        userMsg.showMsgBox(IDS_UE_020066);
        return;
    }

    int displayIndex = m_display.GetCurSel();
    if (displayIndex == CB_ERR)
    {
        CString itemName;
        TA_VERIFY(itemName.LoadString(IDS_DISPLAY)!=0, "Unable to load IDS_DISPLAY");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << itemName;
        userMsg.showMsgBox(IDS_UE_020066);
        return;
    }

    // set data
    CString text;
    m_name.GetWindowText(text);
    m_shortcuts[shortcutIndex]->name = text;

    m_shortcuts[shortcutIndex]->display = reinterpret_cast<TA_Base_Core::IScadaDisplay*>(m_display.GetItemData(displayIndex));

    m_listCtrl.Invalidate();
    FUNCTION_EXIT;
}


void EditShortcutDialog::swapItems(int nItem1, bool moveDown)
{
    FUNCTION_ENTRY("swapItems");
    int nItem2;

    if (moveDown)
    {
        nItem2 = nItem1 + 1;    
        // If we're supposed to move down but it's the last item on the list, then just return
        if (static_cast<unsigned int>(nItem2) >= m_shortcuts.size())
        {
            return;
        }
    }
    else // it's moving up
    {
        nItem2 = nItem1 - 1;    
        // If we're supposed to move up but it's the first item on the list, then just return
        if (nItem2 < 0)
        {
            return;
        }
    }

    ShortcutInformation* temp = m_shortcuts[nItem1];
    m_shortcuts[nItem1] = m_shortcuts[nItem2];
    m_shortcuts[nItem2] = temp;

    m_listCtrl.SetItemState(nItem2, LVIS_SELECTED, LVIS_SELECTED);
	m_listCtrl.EnsureVisible(nItem2, FALSE);
    m_listCtrl.Invalidate();
    FUNCTION_EXIT;
}


std::string EditShortcutDialog::getDefaultShortcutName() 
{
    FUNCTION_ENTRY("getDefaultShortcutName");
    // New Shortcut 1, New Shortcut 2, ...
    std::string retval = DEFAULT_SHORTCUT_PREFIX;
    int length = retval.size();
    int index = 0;
    int posfix;

    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
        if (m_shortcuts[i]->name.find(DEFAULT_SHORTCUT_PREFIX) == 0)
        {
            // see if we can convert the end of the string to number, if we could, then 
            // we've created the string ourselves, otherwise, leave it alone.  Update the
            // current index if the posfix has higher value
            posfix = atol(m_shortcuts[i]->name.substr(length, m_shortcuts[i]->name.size()).c_str());
            if ((0 != posfix) && (posfix > index))
            {
                index = posfix;
            }
        }
    }

    // append the new index to the prefix
    char temp[10];
    _ltoa(++index, temp, 10);
    retval += temp;

    FUNCTION_EXIT;
    return retval;
}


void EditShortcutDialog::OnSysCommand(UINT nID, LPARAM lParam) 
{
    // In WM_SYSCOMMAND messages, the four low-order bits of the nID parameter 
    // are used internally by Windows CE. When an application tests the value of 
    // nID, it must combine the value 0xFFF0 with the nID value by using the 
    // bitwise-AND operator to obtain the correct result.
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void EditShortcutDialog::OnOK() 
{
    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
        // make sure all displays are specified
        if (m_shortcuts[i]->display == NULL)
        {
            CString itemName;
            TA_VERIFY(itemName.LoadString(IDS_DISPLAY)!=0, "Unable to load IDS_DISPLAY");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            userMsg.showMsgBox(IDS_UE_020066);
            return;
        }
    }

    CDialog::OnOK();
}


void EditShortcutDialog::initialiseDisplayCombo()
{
    // initialise the display list
    for (unsigned int i = 0; i < m_allDisplays->size(); i++)
    {
        int index = m_display.AddString((*m_allDisplays)[i]->getName().c_str());
        m_display.SetItemData(index, reinterpret_cast<DWORD>((*m_allDisplays)[i]));
    }
}


LRESULT EditShortcutDialog::onScadaDisplayChange(WPARAM, LPARAM)
{
    m_display.ResetContent();
    initialiseDisplayCombo();
    m_listCtrl.Invalidate();
    populateShortcutDetails();
    TA_Base_Bus::TransActiveMessage userMsg;
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020067);
	return S_OK;
}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/ActiveOperatorsPropertyPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Active Operators" tab. It displays the active 
  * region duty state for logged in operators.
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
    const CString OPERATOR_COLUMN_HEADING("Operator");
    const CString LOGIN_COLUMN_HEADING("Login");
    const int DEFAULT_ROW_HEIGHT(20);
    const int FIXED_COLUMN_COUNT(2);
    const int FIXED_ROW_COUNT(1);
    const int OPERATOR_COLUMN(0);
    const int LOGIN_COLUMN(1);
    const int REGION_ROW(0);

    const COLORREF CURRENT_SESSION_COLOUR(RGB(0, 0, 255)); // blue

    const TA_Base_Bus::SessionKey INVALID_SESSION_KEY(-1);

    const int WM_USER_ENABLE_ASSIGN_RGN_BTN(WM_USER + 1000);
    const int WM_USER_RECEIVE_SESSION_UPDATE(WM_USER + 1001);
    const int WM_USER_RECEIVE_RGN_DUTY_UPDATE(WM_USER + 1002);
}

using TA_Base_Bus::DutyRights;
using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::RegionDutyMatrix;
using TA_Base_Bus::RegionDutyMatrixElement;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionKey;
using TA_Base_Bus::SessionId;
using TA_Base_Bus::SessionIdList;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::ProfileKey;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::RegionDutyList;
using TA_Base_Bus::RegionDutyUpdate;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// ActiveOperatorsPropertyPage property page

IMPLEMENT_DYNCREATE(ActiveOperatorsPropertyPage, CPropertyPage)

ActiveOperatorsPropertyPage::ActiveOperatorsPropertyPage() : 
    CPropertyPage(ActiveOperatorsPropertyPage::IDD),
    ISessionObserver(DutyDataStore::getSessionCache()),
    IRegionDutyObserver(DutyDataStore::getRegionDutyMatrix()),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache()),
    m_regionDutyMatrix(DutyDataStore::getRegionDutyMatrix())
{
    FUNCTION_ENTRY("ActiveOperatorsPropertyPage()");

	//{{AFX_DATA_INIT(ActiveOperatorsPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


ActiveOperatorsPropertyPage::~ActiveOperatorsPropertyPage()
{
    FUNCTION_ENTRY("~ActiveOperatorsPropertyPage()");
    FUNCTION_EXIT;
}


void ActiveOperatorsPropertyPage::addOperator(const TA_Base_Bus::SessionInfoEx& sessionInfoEx)
{
    CCellRange selectedCells(m_regionDutyGrid.GetSelectedCellRange());
    TA_ASSERT(selectedCells.GetRowSpan() <= 1, "Multiple rows selected");

    // Expanding the grid causes it to reset the current selection, so we have to remember
    // which operator was selected before adding a new row.
    SessionKey selectedSessionKey(INVALID_SESSION_KEY);

    if (selectedCells.GetRowSpan() > 0)
    {
        selectedSessionKey = m_regionDutyGrid.GetItemData(selectedCells.GetMinRow(), OPERATOR_COLUMN);
    }

    // Append the new operator to the end of the grid and re-order rows by operator name.
    int row(m_regionDutyGrid.GetRowCount());

    m_regionDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_regionDutyGrid.SetRowCount(row + 1); // unlike InsertRow(), this will use the default row height
    updateOperator(row, sessionInfoEx);
    m_regionDutyGrid.SortTextItems(OPERATOR_COLUMN, TRUE);

    // Locate the new operator following row reshuffle.
    row = GridAssistant::findGridRow(m_regionDutyGrid, sessionInfoEx.sessionKey);
    TA_ASSERT(row != GridAssistant::INVALID_GRID_ROW, "Row not found");

    // Initialise grid cells for the new operator based on his region duties.
    // Where the session has duty for a region, set the corresponding grid cell 
    // to show the green tick symbol.
    for (int column = LOGIN_COLUMN + 1; column < m_regionDutyGrid.GetColumnCount(); column++)
    {
        RegionKey regionKey(m_regionDutyGrid.GetItemData(REGION_ROW, column));

        try
        {
            const RegionDutyMatrixElement& rdmElement = m_regionDutyMatrix.getElement(sessionInfoEx.SessionId, regionKey);
            
            if (rdmElement.dutiesExist())
            {
                m_regionDutyGrid.SetItemImage(row, column, GridAssistant::GI_TICK);
            }
        }
        catch (const TA_Base_Core::MatrixException& ex)
        {
            std::ostringstream msg;
            msg << "Unable to initialise Active Operators grid cell for " << std::endl
                << "operator " << m_dataCache.getOperatorName(sessionInfoEx.UserId)
                << " and region " << m_dataCache.getRegionName(regionKey);

            MessageBox::error(msg.str(), ex);
        }
    }

    // Restore the previous selection (if any).
    if (selectedSessionKey != INVALID_SESSION_KEY)
    {
        int selectedRow(GridAssistant::findGridRow(m_regionDutyGrid, selectedSessionKey));
        TA_ASSERT(selectedRow != GridAssistant::INVALID_GRID_ROW, "Selected operator not found");

        selectedCells.SetMinRow(selectedRow);
        selectedCells.SetMaxRow(selectedRow);

        m_regionDutyGrid.SetSelectedRange(selectedCells, FALSE);
    }

    // Force a redraw of the grid for the changes to become visible.
    m_regionDutyGrid.Invalidate();
}


void ActiveOperatorsPropertyPage::addOperator(const TA_Base_Bus::SessionId& sessionId)
{
    addOperator(m_sessionCache.getSessionInfo(sessionId));
}


void ActiveOperatorsPropertyPage::updateOperator(int row, const TA_Base_Bus::SessionInfoEx& sessionInfoEx, bool redrawRow)
{
    TA_ASSERT(row > REGION_ROW && row < m_regionDutyGrid.GetRowCount(), "Row index out of bounds");

    GV_ITEM operatorGridItem, loginGridItem;

    operatorGridItem.mask = loginGridItem.mask = GVIF_TEXT | GVIF_PARAM;
    operatorGridItem.row = loginGridItem.row = row;
    operatorGridItem.lParam = loginGridItem.lParam = sessionInfoEx.sessionKey;

    // Highlight the current session in a different colour.
    if (m_sessionCache.getCurrentSessionId() == sessionInfoEx.SessionId)
    {
        operatorGridItem.mask = loginGridItem.mask |= GVIF_FGCLR;
        operatorGridItem.crFgClr = loginGridItem.crFgClr = CURRENT_SESSION_COLOUR; 
    }
   
    operatorGridItem.col = OPERATOR_COLUMN;
    operatorGridItem.szText = m_dataCache.getOperatorDescription(sessionInfoEx.UserId).c_str();

    loginGridItem.col = LOGIN_COLUMN;
    loginGridItem.szText = GridAssistant::generateOperatorLoginString(sessionInfoEx).c_str();

    m_regionDutyGrid.SetItem(&operatorGridItem);
    m_regionDutyGrid.SetItem(&loginGridItem);
    
    if (redrawRow)
    {
        m_regionDutyGrid.RedrawRow(row);
    }
}


void ActiveOperatorsPropertyPage::updateOperator(int row, const TA_Base_Bus::SessionId& sessionId, bool redrawRow)
{
    updateOperator(row, m_sessionCache.getSessionInfo(sessionId), redrawRow);
}


void ActiveOperatorsPropertyPage::removeOperator(int row)
{
    TA_ASSERT(row > REGION_ROW && row < m_regionDutyGrid.GetRowCount(), "Row index out of bounds");

    CCellRange selectedCells(m_regionDutyGrid.GetSelectedCellRange());
    TA_ASSERT(selectedCells.GetRowSpan() <= 1, "Multiple rows selected");

    // Shrinking the grid causes it to reset the current selection, so we have to remember
    // which operator was selected before removing an existing row.
    SessionKey selectedSessionKey(INVALID_SESSION_KEY);

    if (selectedCells.GetRowSpan() > 0)
    {
        selectedSessionKey = m_regionDutyGrid.GetItemData(selectedCells.GetMinRow(), OPERATOR_COLUMN);
    }

    m_regionDutyGrid.DeleteRow(row);

    // Restore the previous selection (if any).
    if (selectedSessionKey != INVALID_SESSION_KEY)
    {
        int selectedOperatorRow(GridAssistant::findGridRow(m_regionDutyGrid, selectedSessionKey));

        // If the previously selected operator has been removed, reset the assign/unassign regions buttons, 
        // otherwise re-select the operator.
        if (selectedOperatorRow == GridAssistant::INVALID_GRID_ROW) 
        {
            enableDutyControlBtns();
        }
        else
        {
            selectedCells.SetMinRow(selectedOperatorRow);
            selectedCells.SetMaxRow(selectedOperatorRow);
        
            m_regionDutyGrid.SetSelectedRange(selectedCells, FALSE);
        }
    }

    // Force a redraw of the grid for the changes to become visible.
    m_regionDutyGrid.Invalidate();
}


TA_Base_Bus::SessionInfoEx ActiveOperatorsPropertyPage::getSessionInfo(int row)
{
    SessionKey sessionKey(m_regionDutyGrid.GetItemData(row, OPERATOR_COLUMN));

    return m_sessionCache.getSessionInfo(sessionKey);
}


void ActiveOperatorsPropertyPage::enableDutyControlBtns()
{
    // The following conditions must be satisfied for the buttons to become enabled:
    // 1. A single operator must be selected in the grid
    // 2. Must have region duty transfer rights

    CCellRange selectedCells(m_regionDutyGrid.GetSelectedCellRange());

    bool singleOperatorSelected(selectedCells.GetRowSpan() == 1);
    bool canTransferRegions(m_dutyRights.canTransferRegions());
    
    m_assignRegionsBtn.EnableWindow(singleOperatorSelected && canTransferRegions);
    m_unassignRegionsBtn.EnableWindow(singleOperatorSelected && canTransferRegions);
}


// ISessionObserver interface
void ActiveOperatorsPropertyPage::sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate)
{
    // Let the thread that created the window process the update.
    SendMessage(WM_USER_RECEIVE_SESSION_UPDATE, reinterpret_cast<WPARAM>(&sessionUpdate));
}


// IRegionDutyObserver interface
void ActiveOperatorsPropertyPage::dutyChanged(const TA_Base_Bus::RegionDutyUpdate& dutyUpdate)
{
    // Let the thread that created the window process the update.
    SendMessage(WM_USER_RECEIVE_RGN_DUTY_UPDATE, reinterpret_cast<WPARAM>(&dutyUpdate));
}


void ActiveOperatorsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(ActiveOperatorsPropertyPage)
	DDX_Control(pDX, IDC_ASSIGN_REGIONS_BTN, m_assignRegionsBtn);
	DDX_Control(pDX, IDC_UNASSIGN_REGIONS_BTN, m_unassignRegionsBtn);
	DDX_GridControl(pDX, IDC_RGN_DUTY_GRID, m_regionDutyGrid);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// ActiveOperatorsPropertyPage message handlers

BEGIN_MESSAGE_MAP(ActiveOperatorsPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(ActiveOperatorsPropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(GVN_SELCHANGED, IDC_RGN_DUTY_GRID, OnGridSelChanged)
	ON_BN_CLICKED(IDC_ASSIGN_REGIONS_BTN, OnAssignRegionsBtn)
	ON_BN_CLICKED(IDC_UNASSIGN_REGIONS_BTN, OnUnassignRegionsBtn)
    ON_MESSAGE(WM_USER_RECEIVE_SESSION_UPDATE, OnReceiveSessionUpdate)
    ON_MESSAGE(WM_USER_RECEIVE_RGN_DUTY_UPDATE, OnReceiveRegionDutyUpdate)
	ON_MESSAGE_VOID(WM_USER_ENABLE_ASSIGN_RGN_BTN, enableDutyControlBtns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL ActiveOperatorsPropertyPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
    SessionIdList sessionIds(m_regionDutyMatrix.getRowIds());
    RegionKeyList regionKeys(m_regionDutyMatrix.getColumnIds());

    m_regionDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_regionDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_regionDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_regionDutyGrid.SetListMode(TRUE);
    m_regionDutyGrid.EnableTitleTips(TRUE);
    m_regionDutyGrid.EnableSelection(TRUE);
    m_regionDutyGrid.SetSingleRowSelection(TRUE);
    m_regionDutyGrid.EnableDragAndDrop(FALSE);
    m_regionDutyGrid.SetEditable(FALSE);
    m_regionDutyGrid.SetHeaderSort(FALSE);

    m_regionDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_regionDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for operator names and login details
    m_regionDutyGrid.SetRowCount(m_regionDutyGrid.GetFixedRowCount());  // this will grow as operators are added to the grid
    m_regionDutyGrid.SetColumnCount(m_regionDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
    // Set the fixed column labels.
    m_regionDutyGrid.SetItemText(REGION_ROW, OPERATOR_COLUMN, OPERATOR_COLUMN_HEADING);
    m_regionDutyGrid.SetItemText(REGION_ROW, LOGIN_COLUMN, LOGIN_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(LOGIN_COLUMN + 1);
    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = REGION_ROW;
            gridItem.col = column;
            gridItem.szText = m_dataCache.getRegionName(*regionIt).c_str();
            gridItem.lParam = *regionIt;

            m_regionDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_regionDutyGrid.DeleteColumn(m_regionDutyGrid.GetColumnCount() - 1);

            MessageBox::error("Error while adding region to Active Operators grid.", ex);
        }
    }

    // Add active operators to the grid. Operator names are used as entries in the first column.
    for (SessionIdList::const_iterator sessionIt = sessionIds.begin(); sessionIt != sessionIds.end(); sessionIt++)
    {
        try
        {
            addOperator(*sessionIt);
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            MessageBox::error("Error while adding operator to Active Operators grid.", ex);
        }
    }

    // Size column widths to fit column headings.
    m_regionDutyGrid.AutoSizeColumns();

    // Set the enable state of the assign/unassign regions buttons.
    enableDutyControlBtns();
  
    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void ActiveOperatorsPropertyPage::OnAssignRegionsBtn() 
{
    CCellRange selectedCells = m_regionDutyGrid.GetSelectedCellRange();
    TA_ASSERT(selectedCells.GetRowSpan() == 1, "Multiple or no rows are selected");

    try
    {
        CWaitCursor waitCursor;

        // Get the session associated with selected row.
        SessionInfoEx sessionInfoEx(getSessionInfo(selectedCells.GetMinRow()));

        // Activate region duty transfer dialog for the selected session.
	    RegionDutyTransferDlg rgnDutyXferDlg(sessionInfoEx.SessionId);
        rgnDutyXferDlg.DoModal();
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty transfer dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str(), ex);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty transfer dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str(), ex);
    }
    catch (...)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty transfer dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str());
    }
}


void ActiveOperatorsPropertyPage::OnUnassignRegionsBtn() 
{
    CCellRange selectedCells = m_regionDutyGrid.GetSelectedCellRange();
    TA_ASSERT(selectedCells.GetRowSpan() == 1, "Multiple or no rows are selected");

    try
    {
        CWaitCursor waitCursor;

        // Get the session associated with selected row.
        SessionInfoEx sessionInfoEx(getSessionInfo(selectedCells.GetMinRow()));

        // Activate region duty transfer dialog for the selected session.
	    RegionDutyRemovalDlg rgnDutyRemovalDlg(sessionInfoEx.SessionId);
        rgnDutyRemovalDlg.DoModal();
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty removal dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str(), ex);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty removal dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str(), ex);
    }
    catch (...)
    {
        std::ostringstream msg;
        msg << "Unable to activate region duty removal dialog for operator " 
            << static_cast<LPCTSTR>(m_regionDutyGrid.GetItemText(selectedCells.GetMinRow(), OPERATOR_COLUMN));

        MessageBox::error(msg.str());
    }	
}


LRESULT ActiveOperatorsPropertyPage::OnReceiveSessionUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    const SessionUpdate& sessionUpdate = *reinterpret_cast<const SessionUpdate*>(wParam);

    try
    {
        int row(GridAssistant::findGridRow(m_regionDutyGrid, sessionUpdate.sessionKey));

        switch (sessionUpdate.updateType)
        {
            case TA_Base_Bus::TA_Security::SessionStart:
                if (row == GridAssistant::INVALID_GRID_ROW)
                {
                    addOperator(sessionUpdate.sessionId);
                }
                break;
                               
            case TA_Base_Bus::TA_Security::ProfileUpdate:
                if (row != GridAssistant::INVALID_GRID_ROW)
                {
                    updateOperator(row, sessionUpdate.sessionId, true);                    
                }
                break;

            case TA_Base_Bus::TA_Security::SessionEnd:
                if (row != GridAssistant::INVALID_GRID_ROW)
                {
                    removeOperator(row);
                }
                break;

            default:
                return 0;
        }

        // Session changes involving the current session may affect the current operators 
        // access rights, so the assign/unassign buttons' enable state needs to be reevaluated.
        if (sessionUpdate.sessionId == m_sessionCache.getCurrentSessionId())
        {
            enableDutyControlBtns();
        }
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        MessageBox::error("Error while processing session update.", ex);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        MessageBox::error("Error while processing session update.", ex);
    }
    catch (...)
    {
        MessageBox::error("Error while processing session update.");
    }

    return 0;
}


LRESULT ActiveOperatorsPropertyPage::OnReceiveRegionDutyUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    const RegionDutyUpdate& dutyUpdate = *reinterpret_cast<const RegionDutyUpdate*>(wParam);

    for (RegionDutyList::const_iterator rgnDutyIt = dutyUpdate.duties.begin(); rgnDutyIt != dutyUpdate.duties.end(); rgnDutyIt++)
    {
        try
        {
            // When region duties are removed check to see if the given session has any additional
            // duties for the region (under a different profile). If this is the case, do not
            // remove the green tick from the grid. 
            if (dutyUpdate.updateType == TA_Base_Bus::DUT_REMOVE)
            {
                const RegionDutyMatrixElement& rdmElement = m_regionDutyMatrix.getElement(rgnDutyIt->session.in(), rgnDutyIt->regionKey);

                if (rdmElement.dutiesExist())
                {
                    continue;
                }
            }

            SessionInfoEx sessionInfoEx(m_sessionCache.getSessionInfo(rgnDutyIt->session.in()));

            // Get the row index corresponding to the session affected by duty change.
            // Add the row if necessary.
            int row(GridAssistant::findGridRow(m_regionDutyGrid, sessionInfoEx.sessionKey));

            if (row == GridAssistant::INVALID_GRID_ROW)
            {
                addOperator(sessionInfoEx);
                continue;
            }
        
            // Get the column index corresponding to the region affected by duty change.
            // Ignore the update if no such column is found.
            int column(GridAssistant::findGridColumn(m_regionDutyGrid, rgnDutyIt->regionKey));  
        
            if (column == GridAssistant::INVALID_GRID_COLUMN)
            {
                continue;
            }

            // Update the relevant grid cell. 
            int image(dutyUpdate.updateType == TA_Base_Bus::DUT_ADD? GridAssistant::GI_TICK : GridAssistant::GI_BLANK);

            m_regionDutyGrid.SetItemImage(row, column, image);
            m_regionDutyGrid.RedrawCell(row, column);
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            MessageBox::error("Error while processing region duty update.", ex);
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            MessageBox::error("Error while processing region duty update.", ex);
        }
        catch (...)
        {
            MessageBox::error("Error while processing region duty update.");
        }        
    }

    return 0;
}


void ActiveOperatorsPropertyPage::OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // pNMHDR points to a NM_GRIDVIEW structure, but that does not contain
    // any accurate information about the grid's selection state. 
    // The grid has to be asked for the selected cells, however, even though it
    // has sent us a GVN_SELCHANGED message, the selection map hasn't yet been
    // updated, so we need to delay enabling the button until the grid has had
    // the chance to do this. 
    PostMessage(WM_USER_ENABLE_ASSIGN_RGN_BTN);

	*pResult = 0;
}


void ActiveOperatorsPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_regionDutyGrid.GetSafeHwnd()))    
    {
        CRect assignBtnRect;
        
        m_assignRegionsBtn.GetWindowRect(assignBtnRect);
        ScreenToClient(assignBtnRect);
        int buttonHeight(assignBtnRect.Height());
        assignBtnRect.bottom = cy - 8;
        assignBtnRect.top = assignBtnRect.bottom - buttonHeight;
        m_assignRegionsBtn.MoveWindow(assignBtnRect);

        CRect unassignBtnRect;
        
        m_unassignRegionsBtn.GetWindowRect(unassignBtnRect);
        ScreenToClient(unassignBtnRect);
        unassignBtnRect.top = assignBtnRect.top;
        unassignBtnRect.bottom = assignBtnRect.bottom;
        m_unassignRegionsBtn.MoveWindow(unassignBtnRect);

        CRect gridRect;

        m_regionDutyGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = assignBtnRect.top - 8;
        m_regionDutyGrid.MoveWindow(gridRect);        
    }
}


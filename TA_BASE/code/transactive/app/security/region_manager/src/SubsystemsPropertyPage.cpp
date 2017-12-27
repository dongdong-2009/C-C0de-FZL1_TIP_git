/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/SubsystemsPropertyPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Subsystems" tab. It displays the active 
  * subsystem duty state for every physical subsystem in every region.
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
    const CString SUBSYSTEM_COLUMN_HEADING("Subsystem");
    const int DEFAULT_ROW_HEIGHT(20);
    const int FIXED_COLUMN_COUNT(1);
    const int FIXED_ROW_COUNT(1);
    const int SUBSYSTEM_COLUMN(0);
    const int REGION_ROW(0);

    const COLORREF UNALLOCATED_SUBSYSTEM_COLOUR(RGB(225, 225, 225)); // light grey
    const COLORREF UNCONTROLLED_REGION_COLOUR(RGB(255, 0, 0)); // red

    const int WM_USER_RECEIVE_SUB_DUTY_UPDATE(WM_USER + 1000);
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::SubsystemDutyMatrix;
using TA_Base_Bus::SubsystemDutyMatrixElement;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::SubsystemKey;
using TA_Base_Bus::SubsystemKeyList;
using TA_Base_Bus::SubsystemDutyUpdate;
using TA_Base_Bus::SubsystemDutyList;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// SubsystemsPropertyPage property page

IMPLEMENT_DYNCREATE(SubsystemsPropertyPage, CPropertyPage)

SubsystemsPropertyPage::SubsystemsPropertyPage() : 
    CPropertyPage(SubsystemsPropertyPage::IDD),
    ISubsystemDutyObserver(DutyDataStore::getSubsystemDutyMatrix()),
    m_dataCache(DutyDataStore::getDataCache()),
    m_subsystemDutyMatrix(DutyDataStore::getSubsystemDutyMatrix())
{
    FUNCTION_ENTRY("SubsystemsPropertyPage()");

	//{{AFX_DATA_INIT(SubsystemsPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


SubsystemsPropertyPage::~SubsystemsPropertyPage()
{
    FUNCTION_ENTRY("~SubsystemsPropertyPage()");
    FUNCTION_EXIT;
}


void SubsystemsPropertyPage::setUncontrolledRegionIndicator(int column)
{
    // Where a region is assigned subsystem(s) and none of those subsystems is being manned, 
    // the region is highlighted by placing its heading on a red background. 

    TA_ASSERT(column >= m_subsystemDutyGrid.GetFixedColumnCount() && column < m_subsystemDutyGrid.GetColumnCount(), "Column index out of range");

    RegionKey regionKey(m_subsystemDutyGrid.GetItemData(REGION_ROW, column));    
    bool isIndicatorOn(m_subsystemDutyGrid.GetItemBkColour(REGION_ROW, column) == UNCONTROLLED_REGION_COLOUR);
    bool isRegionUncontrolled(true);
    bool isRegionEmpty(true);

    for (int row = REGION_ROW + 1; row < m_subsystemDutyGrid.GetRowCount(); row++)
    {
        SubsystemKey subsystemKey(m_subsystemDutyGrid.GetItemData(row, SUBSYSTEM_COLUMN));
        const SubsystemDutyMatrixElement& sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
        
        if (sdmElement.isSubsystemInRegion())
        {
            isRegionEmpty = false;
  
            if (sdmElement.dutiesExist())
            {
                isRegionUncontrolled = false;
                break;
            }
        }
    }

    if (!isRegionEmpty && isRegionUncontrolled != isIndicatorOn)
    {
        COLORREF colour(isRegionUncontrolled? UNCONTROLLED_REGION_COLOUR : m_subsystemDutyGrid.GetFixedBkColor());
        
        m_subsystemDutyGrid.SetItemBkColour(REGION_ROW, column, colour);
        m_subsystemDutyGrid.RedrawCell(REGION_ROW, column);
    }
}


// ISubsystemDutyObserver interface
void SubsystemsPropertyPage::dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate)
{
    // Let the thread that created the window process the update.
    SendMessage(WM_USER_RECEIVE_SUB_DUTY_UPDATE, reinterpret_cast<WPARAM>(&dutyUpdate));
}


void SubsystemsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SubsystemsPropertyPage)
	DDX_GridControl(pDX, IDC_SUB_DUTY_GRID, m_subsystemDutyGrid);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// SubsystemsPropertyPage message handlers

BEGIN_MESSAGE_MAP(SubsystemsPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(SubsystemsPropertyPage)
	ON_WM_SIZE()
    ON_MESSAGE(WM_USER_RECEIVE_SUB_DUTY_UPDATE, OnReceiveSubsystemDutyUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL SubsystemsPropertyPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
    SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIds());
    RegionKeyList regionKeys(m_subsystemDutyMatrix.getColumnIds());

    m_subsystemDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_subsystemDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_subsystemDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_subsystemDutyGrid.SetListMode(TRUE);
    m_subsystemDutyGrid.EnableTitleTips(TRUE);
    m_subsystemDutyGrid.EnableSelection(FALSE);
    m_subsystemDutyGrid.EnableDragAndDrop(FALSE);
    m_subsystemDutyGrid.SetEditable(FALSE);
    m_subsystemDutyGrid.SetHeaderSort(FALSE);

    m_subsystemDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_subsystemDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for subsystem names
    m_subsystemDutyGrid.SetRowCount(m_subsystemDutyGrid.GetFixedRowCount() + subsystemKeys.size());
    m_subsystemDutyGrid.SetColumnCount(m_subsystemDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
    // Set the fixed column labels.
    m_subsystemDutyGrid.SetItemText(REGION_ROW, SUBSYSTEM_COLUMN, SUBSYSTEM_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(SUBSYSTEM_COLUMN + 1);
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

            m_subsystemDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_subsystemDutyGrid.DeleteColumn(m_subsystemDutyGrid.GetColumnCount() - 1);

            MessageBox::error("Error while adding region to Subsystems grid.", ex);
        }
    }

    // Set the cells in the first column with the names of the subsystems.
    // Use cell item data to store the corresponding subsystem keys so that the subsystem
    // representing each row can be easily identified.
    int row(REGION_ROW + 1);
    for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
    {
        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = row;
            gridItem.col = SUBSYSTEM_COLUMN;
            gridItem.szText = m_dataCache.getSubsystemName(*subsystemIt).c_str();
            gridItem.lParam = *subsystemIt;

            m_subsystemDutyGrid.SetItem(&gridItem);

            row++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one row
            m_subsystemDutyGrid.DeleteRow(m_subsystemDutyGrid.GetRowCount() - 1);

            MessageBox::error("Error while adding subsystem to Subsystems grid.", ex);
        }
    }
 
    // Where a subsystem is not part of a region, grey out the corresponding grid cell.
    // Where a subsystem IS part of a region and has duty for that region, set cell to
    // show the green tick symbol. Otherwise show the red cross symbol.
    for (column = SUBSYSTEM_COLUMN + 1; column < m_subsystemDutyGrid.GetColumnCount(); column++)
    {
        RegionKey regionKey(m_subsystemDutyGrid.GetItemData(REGION_ROW, column));

        for (row = REGION_ROW + 1; row < m_subsystemDutyGrid.GetRowCount(); row++)
        {
            SubsystemKey subsystemKey(m_subsystemDutyGrid.GetItemData(row, SUBSYSTEM_COLUMN));

            try
            {
                const SubsystemDutyMatrixElement& sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);

                if (!sdmElement.isSubsystemInRegion())
                {
                    m_subsystemDutyGrid.SetItemBkColour(row, column, UNALLOCATED_SUBSYSTEM_COLOUR);
                    continue;
                }

                int image(sdmElement.dutiesExist()? GridAssistant::GI_TICK : GridAssistant::GI_CROSS);

                m_subsystemDutyGrid.SetItemImage(row, column, image);
            }
            catch (const TA_Base_Core::MatrixException& ex)
            {
                std::ostringstream msg;
                msg << "Error while setting Subsystems grid cell for" << std::endl
                    << "subsystem " << m_dataCache.getSubsystemName(subsystemKey)
                    << " and region " << m_dataCache.getRegionName(regionKey);

                MessageBox::error(msg.str(), ex);
            }
        }

        // Update the background colour of the column heading depending on whether
        // the region is uncontrolled or not.
        setUncontrolledRegionIndicator(column);
    }
  	
    // Sort rows by subsystem name.
    m_subsystemDutyGrid.SortTextItems(SUBSYSTEM_COLUMN, TRUE);

    // Size column widths to fit column headings.
    m_subsystemDutyGrid.AutoSizeColumns();

    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT SubsystemsPropertyPage::OnReceiveSubsystemDutyUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    const SubsystemDutyUpdate& dutyUpdate = *reinterpret_cast<const SubsystemDutyUpdate*>(wParam);

    for (SubsystemDutyList::const_iterator subDutyIt = dutyUpdate.duties.begin(); subDutyIt != dutyUpdate.duties.end(); subDutyIt++)
    {
        try
        {
            // When subsystem duties are removed check to see if the given subsystem has any additional
            // duties for the region (under a different session or profile). If this is the case, do not
            // remove the green tick from the grid. 
            if (dutyUpdate.updateType == TA_Base_Bus::DUT_REMOVE)
            {
                const SubsystemDutyMatrixElement& sdmElement = m_subsystemDutyMatrix.getElement(subDutyIt->subsystemKey, subDutyIt->regionKey);

                if (sdmElement.dutiesExist())
                {
                    continue;
                }
            }

            // Get the row index corresponding to the subsystem affected by duty change.
            // Ignore the update if no such row is found.
            int row(GridAssistant::findGridRow(m_subsystemDutyGrid, subDutyIt->subsystemKey));

            if (row == GridAssistant::INVALID_GRID_ROW)
            {
                continue;
            }
        
            // Get the column index corresponding to the region affected by duty change.
            // Ignore the update if no such column is found.
            int column(GridAssistant::findGridColumn(m_subsystemDutyGrid, subDutyIt->regionKey));  
        
            if (column == GridAssistant::INVALID_GRID_COLUMN)
            {
                continue;
            }

            // Update the relevant grid cell. 
            int image(dutyUpdate.updateType == TA_Base_Bus::DUT_ADD? GridAssistant::GI_TICK : GridAssistant::GI_CROSS);

            m_subsystemDutyGrid.SetItemImage(row, column, image);
            m_subsystemDutyGrid.RedrawCell(row, column);

            // Update the background colour of the column heading depending on whether
            // the region is uncontrolled or not.
            setUncontrolledRegionIndicator(column);
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            MessageBox::error("Error while processing subsystem duty update.", ex);
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            MessageBox::error("Error while processing subsystem duty update.", ex);
        }
        catch (...)
        {
            MessageBox::error("Error while processing subsystem duty update.");
        }        
    }

    return 0;
}


void SubsystemsPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_subsystemDutyGrid.GetSafeHwnd()))    
    {
        CRect gridRect;

        m_subsystemDutyGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = cy - 8;
        m_subsystemDutyGrid.MoveWindow(gridRect);        
    }	
}

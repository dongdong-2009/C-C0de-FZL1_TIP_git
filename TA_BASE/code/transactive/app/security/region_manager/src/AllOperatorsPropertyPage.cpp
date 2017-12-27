/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/AllOperatorsPropertyPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "All Operators" tab. It displays the default 
  * operator-region assignments for all configured operators.
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
    const int DEFAULT_ROW_HEIGHT(20);
    const int FIXED_COLUMN_COUNT(1);
    const int FIXED_ROW_COUNT(1);
    const int OPERATOR_COLUMN(0);
    const int REGION_ROW(0);
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::DefaultRegionAssignmentMatrix;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::OperatorKey;
using TA_Base_Bus::OperatorKeyList;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// AllOperatorsPropertyPage property page

IMPLEMENT_DYNCREATE(AllOperatorsPropertyPage, CPropertyPage)

AllOperatorsPropertyPage::AllOperatorsPropertyPage() : 
    CPropertyPage(AllOperatorsPropertyPage::IDD),
    m_dataCache(DutyDataStore::getDataCache()),
    m_defRgnAssignMatrix(DutyDataStore::getDefRegionAssignmentMatrix())
{
    FUNCTION_ENTRY("AllOperatorsPropertyPage()");

	//{{AFX_DATA_INIT(AllOperatorsPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


AllOperatorsPropertyPage::~AllOperatorsPropertyPage()
{
    FUNCTION_ENTRY("~AllOperatorsPropertyPage()");
    FUNCTION_EXIT;
}


void AllOperatorsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(AllOperatorsPropertyPage)
	DDX_GridControl(pDX, IDC_DEF_RGN_ASSIGN_GRID, m_defRgnAssignGrid);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// AllOperatorsPropertyPage message handlers

BEGIN_MESSAGE_MAP(AllOperatorsPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(AllOperatorsPropertyPage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL AllOperatorsPropertyPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
    OperatorKeyList operatorKeys(m_defRgnAssignMatrix.getRowIds());
    RegionKeyList regionKeys(m_defRgnAssignMatrix.getColumnIds());

    m_defRgnAssignGrid.SetImageList(&GridAssistant::getGridImages());
    m_defRgnAssignGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_defRgnAssignGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_defRgnAssignGrid.SetListMode(TRUE);
    m_defRgnAssignGrid.EnableTitleTips(TRUE);
    m_defRgnAssignGrid.EnableSelection(FALSE);
    m_defRgnAssignGrid.EnableDragAndDrop(FALSE);
    m_defRgnAssignGrid.SetEditable(FALSE);
    m_defRgnAssignGrid.SetHeaderSort(FALSE);

    m_defRgnAssignGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_defRgnAssignGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for operator names
    m_defRgnAssignGrid.SetRowCount(m_defRgnAssignGrid.GetFixedRowCount() + operatorKeys.size());
    m_defRgnAssignGrid.SetColumnCount(m_defRgnAssignGrid.GetFixedColumnCount() + regionKeys.size());

    // Set the fixed column labels.
    m_defRgnAssignGrid.SetItemText(REGION_ROW, OPERATOR_COLUMN, OPERATOR_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(OPERATOR_COLUMN + 1);
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

            m_defRgnAssignGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_defRgnAssignGrid.DeleteColumn(m_defRgnAssignGrid.GetColumnCount() - 1);

            MessageBox::error("Error while adding region to All Operators grid.", ex);
        }
    }

    // Set the cells in the first column with the names of the operators.
    // Use cell item data to store the corresponding operator keys so that the operator
    // representing each row can be easily identified.
    int row(REGION_ROW + 1);
    for (OperatorKeyList::const_iterator operatorIt = operatorKeys.begin(); operatorIt != operatorKeys.end(); operatorIt++)
    {
        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = row;
            gridItem.col = OPERATOR_COLUMN;
            gridItem.szText = m_dataCache.getOperatorDescription(*operatorIt).c_str();
            gridItem.lParam = *operatorIt;

            m_defRgnAssignGrid.SetItem(&gridItem);

            row++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one row
            m_defRgnAssignGrid.DeleteRow(m_defRgnAssignGrid.GetRowCount() - 1);

            MessageBox::error("Error while adding operator to All Operators grid.", ex);
        }
    }
 
    // Where a region is allocated to an operator, set the corresponding grid cell 
    // to show the green tick symbol.
    for (row = REGION_ROW + 1; row < m_defRgnAssignGrid.GetRowCount(); row++)
    {
        OperatorKey operatorKey(m_defRgnAssignGrid.GetItemData(row, OPERATOR_COLUMN));

        for (column = OPERATOR_COLUMN + 1; column < m_defRgnAssignGrid.GetColumnCount(); column++)
        {
            RegionKey regionKey(m_defRgnAssignGrid.GetItemData(REGION_ROW, column));

            try
            {
                bool isAssigned(m_defRgnAssignMatrix.getElement(operatorKey, regionKey));

                if (isAssigned)
                {
                    m_defRgnAssignGrid.SetItemImage(row, column, GridAssistant::GI_TICK);
                }
            }
            catch (const TA_Base_Core::MatrixException& ex)
            {
                std::ostringstream msg;
                msg << "Error while setting All Operators grid cell for" << std::endl
                    << "operator " << m_dataCache.getOperatorName(operatorKey)
                    << " and region " << m_dataCache.getRegionName(regionKey);

                MessageBox::error(msg.str(), ex);
            }
        }
    }

    // Sort rows by operator name.
    m_defRgnAssignGrid.SortTextItems(OPERATOR_COLUMN, TRUE);

    // Size column widths to fit column headings.
    m_defRgnAssignGrid.AutoSizeColumns();

    FUNCTION_EXIT; 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AllOperatorsPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_defRgnAssignGrid.GetSafeHwnd()))    
    {
        CRect gridRect;

        m_defRgnAssignGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = cy - 8;
        m_defRgnAssignGrid.MoveWindow(gridRect);        
    }
}

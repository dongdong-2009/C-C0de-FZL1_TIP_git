/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/FilterDialog.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class to perform filtering of the equipment status listCtrl based on provided filters
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "FilterDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

// NOTE : Report related
#include "core/data_access_interface/src/IEquipReport.h"
#include "core/data_access_interface/src/IConfigEquipReport.h"
#include "core/data_access_interface/src/EquipReportAccessFactory.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

	CFilterDialog::CFilterDialog(bool aCanConfigure,
        FilterFlagsAndStrings& sourceFilters, 
        std::string& reportId,
        CString const& reportName,
        CString const& locationName,
        EquipmentStatusListItemMap const& reportItems,
        CWnd* pParent /*=NULL*/)
		:
        CDialog(CFilterDialog::IDD, pParent), 
		m_canConfigure(aCanConfigure),
        m_ReportId( reportId),
        m_ReportName( reportName),
        m_LocationName( locationName),
        m_reportItems( reportItems),
        m_sourceFilters( sourceFilters),
        m_currentFilters( sourceFilters)
	{
		//{{AFX_DATA_INIT(CFilterDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT

        //  Make sure the working copy of the filters is big enough
        m_currentFilters.resize( ESL_FILTER_OFFSET_MAX);
	}


	void CFilterDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CFilterDialog)
	    DDX_Control(pDX, IDC_MMS_INHIBIT, m_Mms_Inhibit_Check);
	    DDX_Control(pDX, IDC_TAGGED_PTW, m_Tagged_PTW_Check);
	    DDX_Control(pDX, IDC_TAGGED, m_Tagged_Check);
	    DDX_Control(pDX, IDC_OVERRIDDEN, m_Overridden_Check);
	    DDX_Control(pDX, IDC_ALARM_INHIBIT, m_Alarm_Inhibit_Check);
	    DDX_Control(pDX, IDC_SCAN_INHIBIT, m_Scan_Inhibit_Check);
	    DDX_Control(pDX, IDC_CONTROL_INHIBIT, m_Control_Inhibit_Check);
	    DDX_Control(pDX, IDC_ITEMTYPE_FILTER, m_ItemTypeFilterCombo);
		DDX_Control(pDX, IDC_TIMESTAMP_FILTER_ENABLED, m_TimestampFilterCheck);
		DDX_Control(pDX, IDC_STATUSCHANGEBY_FILTER_ENABLED, m_StatusChangeByFilterCheck);
		DDX_Control(pDX, IDC_STATUS_FILTER_ENABLED, m_StatusFilterCheck);
		DDX_Control(pDX, IDC_DPVALUE_FILTER_ENABLED, m_DPValueFilterCheck);
		DDX_Control(pDX, IDC_DPTYPE_FILTER_ENABLED, m_DPTypeFilterCheck);
		DDX_Control(pDX, IDC_DPNAME_FILTER_ENABLED, m_DPNameFilterCheck);
		DDX_Control(pDX, IDC_TIMESTAMP_FILTER, m_TimestampFilterCombo);
		DDX_Control(pDX, IDC_STATUSCHANGEBY_FILTER, m_StatusChangeByFilterCombo);
		DDX_Control(pDX, IDC_DPVALUE_FILTER, m_DPValueFilterCombo);
		DDX_Control(pDX, IDC_DPTYPE_FILTER, m_DPTypeFilterCombo);
		DDX_Control(pDX, IDC_DPNAME_FILTER, m_DPNameFilterCombo);
		DDX_Control(pDX, IDC_DESC_FILTER_ENABLED, m_DescriptionFilterCheck);
		DDX_Control(pDX, IDC_DESCRIPTION_FILTER, m_DescriptionFilterCombo);
		DDX_Control(pDX, IDC_ASSET_FILTER_ENABLED, m_AssetFilterCheck);
		DDX_Control(pDX, IDC_ASSET_FILTER, m_AssetFilterCombo);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CFilterDialog, CDialog)
		//{{AFX_MSG_MAP(CFilterDialog)
        ON_BN_CLICKED(IDB_CLEAR, OnClearButton)
		ON_BN_CLICKED(IDC_ASSET_FILTER_ENABLED, OnAssetFilterEnabled)
		ON_BN_CLICKED(IDC_DESC_FILTER_ENABLED, OnDescFilterEnabled)
		ON_BN_CLICKED(IDC_DPNAME_FILTER_ENABLED, OnDpnameFilterEnabled)
		ON_BN_CLICKED(IDC_DPTYPE_FILTER_ENABLED, OnDptypeFilterEnabled)
		ON_BN_CLICKED(IDC_DPVALUE_FILTER_ENABLED, OnDpvalueFilterEnabled)
		ON_BN_CLICKED(IDC_STATUS_FILTER_ENABLED, OnStatusFilterEnabled)
		ON_BN_CLICKED(IDC_STATUSCHANGEBY_FILTER_ENABLED, OnStatuschangebyFilterEnabled)
		ON_BN_CLICKED(IDC_TIMESTAMP_FILTER_ENABLED, OnTimestampFilterEnabled)
		ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_ITEMTYPE_FILTER, OnSelchangeItemtypeFilter)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()



	BOOL CFilterDialog::OnInitDialog() 
	{
        CWaitCursor waitCursor;
		CDialog::OnInitDialog();

		if (m_canConfigure)
			SetWindowText("Configure Filters");
		else
			SetWindowText("Current Filters");

		// Set the Group Box display text
		CWnd *window = GetDlgItem(IDC_FILTER_GROUP_BOX);
		CString title = "Report:  " + m_ReportName + " at " + m_LocationName;
		int MAX_LENGTH = 70;
		if (title.GetLength() > MAX_LENGTH)
		{
			title.Delete(MAX_LENGTH - 1, title.GetLength() - MAX_LENGTH + 1);
			title.Insert(title.GetLength(), "...");
		}
		window->SetWindowText(title);

		if( (m_ReportId == ACTIVE_PROFILE_ALL_REPORT) || (m_ReportId == ACTIVE_PROFILE_MMS_REPORT) )
		{
			m_ItemTypeFilterCombo.EnableWindow(FALSE);
		}

		// populate the item type combo box
		m_ItemTypeFilterCombo.AddString(REPORT_TYPE_ALL);
		m_ItemTypeFilterCombo.AddString(REPORT_TYPE_EQUIPMENT);
		m_ItemTypeFilterCombo.AddString(REPORT_TYPE_DATAPOINT);

        // populate the other combo boxes with the data from the report items (including those not displayed)
        FilterStringSet ssAsset;
        FilterStringSet ssDescription;
        FilterStringSet ssDatapoint;
        FilterStringSet ssType;
        FilterStringSet ssValue;
        FilterStringSet ssModifiedBy;
        FilterStringSet ssTimestamp;

		FilterStringSet ssDatapointAsset;
		FilterStringSet ssEquipmentAsset;
        FilterStringSet ssDatapointDescription;
		FilterStringSet ssEquipmentDescription;

        //  Iterate through the map ( first=entityKey, second=EquipmentStatusListItemSharedPtr)
        //  to build lists of unique values for each column
        EquipmentStatusListItemMap::const_iterator item = m_reportItems.begin();
        for ( ; item != m_reportItems.end(); item++)
        {
            if ((m_ReportId == ACTIVE_PROFILE_ALL_REPORT) || (m_ReportId == ACTIVE_PROFILE_MMS_REPORT))
            {
                if (!item->second->isFiltered())
                {
                    AddToFilterStringSet(ssAsset,       item->second->getColumnText( ASSET_COLUMN));
                    AddToFilterStringSet(ssDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );
                    AddToFilterStringSet(ssDatapoint,   item->second->getColumnText( DATAPOINT_COLUMN) );
                    AddToFilterStringSet(ssType,        item->second->getColumnText( TYPE_COLUMN) );
                    AddToFilterStringSet(ssValue,       item->second->getColumnText( VALUE_COLUMN) );
                    AddToFilterStringSet(ssModifiedBy,  item->second->getColumnText( MODIFIED_BY_COLUMN) );
                    AddToFilterStringSet(ssTimestamp,   item->second->getColumnText( TIMESTAMP_COLUMN) );

					// TD16914:Wenching
					if (true == TA_Base_Core::EntityAccessFactory::getInstance().isEquipmentDataNode(item->first))
					{
						AddToFilterStringSet(ssEquipmentAsset,       item->second->getColumnText( ASSET_COLUMN));
						AddToFilterStringSet(ssEquipmentDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );		
					}
					else
					{
						AddToFilterStringSet(ssDatapointAsset,       item->second->getColumnText( ASSET_COLUMN));
						AddToFilterStringSet(ssDatapointDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );		
					}
					//++Wenching
                }
            }
            else
            {
                AddToFilterStringSet(ssAsset,       item->second->getColumnText( ASSET_COLUMN));
                AddToFilterStringSet(ssDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );
                AddToFilterStringSet(ssDatapoint,   item->second->getColumnText( DATAPOINT_COLUMN) );
                AddToFilterStringSet(ssType,        item->second->getColumnText( TYPE_COLUMN) );
                AddToFilterStringSet(ssValue,       item->second->getColumnText( VALUE_COLUMN) );
                AddToFilterStringSet(ssModifiedBy,  item->second->getColumnText( MODIFIED_BY_COLUMN) );
                AddToFilterStringSet(ssTimestamp,   item->second->getColumnText( TIMESTAMP_COLUMN) );

				// TD16914:Wenching++
				if (true == TA_Base_Core::EntityAccessFactory::getInstance().isEquipmentDataNode(item->first))
				{
					AddToFilterStringSet(ssEquipmentAsset,       item->second->getColumnText( ASSET_COLUMN));
					AddToFilterStringSet(ssEquipmentDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );		
				}
				else
				{
					AddToFilterStringSet(ssDatapointAsset,       item->second->getColumnText( ASSET_COLUMN));
					AddToFilterStringSet(ssDatapointDescription, item->second->getColumnText( DESCRIPTION_COLUMN) );		
				}
				//++Wenching
            }
        }

		m_equipmentAssetComboMap[REPORT_TYPE_ALL] = ssAsset;
		m_equipmentAssetComboMap[REPORT_TYPE_EQUIPMENT] = ssEquipmentAsset;
		m_equipmentAssetComboMap[REPORT_TYPE_DATAPOINT] = ssDatapointAsset;
		m_equipmentDescriptionComboMap[REPORT_TYPE_ALL] = ssDescription;
		m_equipmentDescriptionComboMap[REPORT_TYPE_EQUIPMENT] = ssEquipmentDescription;
		m_equipmentDescriptionComboMap[REPORT_TYPE_DATAPOINT] = ssDatapointDescription;

        //  Now have sets of unique values for each column
    	//PopulateComboBoxWithStringSet( m_AssetFilterCombo, ssAsset);
    	//PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, ssDescription);
    	PopulateComboBoxWithStringSet( m_DPNameFilterCombo, ssDatapoint);
    	PopulateComboBoxWithStringSet( m_DPTypeFilterCombo, ssType);
    	PopulateComboBoxWithStringSet( m_DPValueFilterCombo, ssValue);
    	PopulateComboBoxWithStringSet( m_StatusChangeByFilterCombo, ssModifiedBy);
    	PopulateComboBoxWithStringSet( m_TimestampFilterCombo, ssTimestamp);

        // now set the right values as whats in the actual filters
		std::string filterString;
		int pos = 0;
		int index = 0;

		if (m_currentFilters[REPORT_TYPE_FILTER_OFFSET].filterInUse)
		{
			pos = m_ItemTypeFilterCombo.FindStringExact(-1, m_currentFilters[REPORT_TYPE_FILTER_OFFSET].filterString.c_str());
			if (pos == CB_ERR)
			{
				pos = 0;
			}
			m_ItemTypeFilterCombo.SetCurSel(pos);
		} 
		else
		{
			m_ItemTypeFilterCombo.SetCurSel(0);
		}

		// TD16914:Wenching++
		CString szItemText;
		m_ItemTypeFilterCombo.GetLBText(m_ItemTypeFilterCombo.GetCurSel(), szItemText);
			
		if (szItemText==REPORT_TYPE_EQUIPMENT)
		{
			PopulateComboBoxWithStringSet( m_AssetFilterCombo, ssEquipmentAsset);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, ssEquipmentDescription);
		}
		else if (szItemText==REPORT_TYPE_DATAPOINT)
		{
			PopulateComboBoxWithStringSet( m_AssetFilterCombo, ssDatapointAsset);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, ssDatapointDescription);
		}
		else
		{
			PopulateComboBoxWithStringSet( m_AssetFilterCombo, ssAsset);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, ssDescription);
		}
		//++Wenching

        SetFilterFlagAndString( m_currentFilters[ASSET_FILTER_OFFSET].filterInUse,           m_AssetFilterCheck,          m_AssetFilterCombo,          ASSET_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[DESCRIPTION_FILTER_OFFSET].filterInUse,     m_DescriptionFilterCheck,    m_DescriptionFilterCombo,    DESCRIPTION_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[DATAPOINT_NAME_FILTER_OFFSET].filterInUse,  m_DPNameFilterCheck,         m_DPNameFilterCombo,         DATAPOINT_NAME_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[DATAPOINT_TYPE_FILTER_OFFSET].filterInUse,  m_DPTypeFilterCheck,         m_DPTypeFilterCombo,         DATAPOINT_TYPE_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[DATAPOINT_VALUE_FILTER_OFFSET].filterInUse, m_DPValueFilterCheck,        m_DPValueFilterCombo,        DATAPOINT_VALUE_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[MODIFIED_BY_FILTER_OFFSET].filterInUse,     m_StatusChangeByFilterCheck, m_StatusChangeByFilterCombo, MODIFIED_BY_FILTER_OFFSET);
        SetFilterFlagAndString( m_currentFilters[TIMESTAMP_FILTER_OFFSET].filterInUse,       m_TimestampFilterCheck,      m_TimestampFilterCombo,      TIMESTAMP_FILTER_OFFSET);

        // read the status general flag
		if (m_currentFilters[STATUS_FILTER_OFFSET].filterInUse)
		{
			m_StatusFilterCheck.SetCheck(BST_CHECKED);
    		if (m_currentFilters[STATUS_SCAN_INHIBIT_FILTER_OFFSET].filterInUse)
				m_Scan_Inhibit_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_ALARM_INHIBIT_FILTER_OFFSET].filterInUse)
				m_Alarm_Inhibit_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_CONTROL_INHIBIT_FILTER_OFFSET].filterInUse)
				m_Control_Inhibit_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_MMS_INHIBIT_FILTER_OFFSET].filterInUse)
				m_Mms_Inhibit_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_TAGGED_FILTER_OFFSET].filterInUse)
				m_Tagged_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_TAGGED_PTW_FILTER_OFFSET].filterInUse)
				m_Tagged_PTW_Check.SetCheck(BST_CHECKED);

    		if (m_currentFilters[STATUS_OVERRIDDEN_FILTER_OFFSET].filterInUse)
				m_Overridden_Check.SetCheck(BST_CHECKED);
		}

		OnSelchangeItemtypeFilter();

		return TRUE;
	}

    void CFilterDialog::AddToFilterStringSet(FilterStringSet &fss, CString newString)
    {
        //  Add the string to the set if it isn't blank
        //  The std::set will ignore duplicates on insertion.
        newString.TrimLeft();
        newString.TrimRight();
        if (newString.IsEmpty())
            return;
        fss.insert( newString );
    }


    void CFilterDialog::PopulateComboBoxWithStringSet(CComboBox &comboBox, const FilterStringSet &filterStrings)
    {
        //  Fill the combobox in bulk by disabling redraw and pre-allocating some space
        comboBox.SetRedraw( false);
        comboBox.ResetContent();
        comboBox.InitStorage( filterStrings.size(), 10); // average 10 characters per string

        //  Make sure the drop listbox is wide enough to display every string
        CDC* pDC = comboBox.GetDC();
		if ( NULL == pDC)
		{
			TA_ASSERT ( FALSE, "Unable to get handle to Device Context of the Combo Control." );
		}
		int maxTextSize = 0;
        CSize textSize;

        int nIndex=0;
        FilterStringSet::const_iterator iter = filterStrings.begin();
        for ( ; iter != filterStrings.end(); iter++)
        {
            textSize = pDC->GetTextExtent( *iter );
            if ( textSize.cx > maxTextSize)
            {
                maxTextSize = textSize.cx;
            }
            comboBox.InsertString( nIndex, *iter);
            nIndex++;
        }
        comboBox.ReleaseDC( pDC);

        //  Add some horizontal space for the scrollbars and two edges
        maxTextSize += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);
		comboBox.SetDroppedWidth( maxTextSize);

        comboBox.SetRedraw( true);
    }

	void CFilterDialog::OnAssetFilterEnabled() 
	{
        SetFilterFlagAndString( (m_AssetFilterCheck.GetCheck() == BST_CHECKED), m_AssetFilterCheck, m_AssetFilterCombo, ASSET_FILTER_OFFSET);
	}

	void CFilterDialog::OnDescFilterEnabled() 
	{
        SetFilterFlagAndString( (m_DescriptionFilterCheck.GetCheck() == BST_CHECKED), m_DescriptionFilterCheck, m_DescriptionFilterCombo, DESCRIPTION_FILTER_OFFSET);
	}

	void CFilterDialog::OnDpnameFilterEnabled() 
	{
        SetFilterFlagAndString( (m_DPNameFilterCheck.GetCheck() == BST_CHECKED), m_DPNameFilterCheck, m_DPNameFilterCombo, DATAPOINT_NAME_FILTER_OFFSET);
	}

	void CFilterDialog::OnDptypeFilterEnabled() 
	{
        SetFilterFlagAndString( (m_DPTypeFilterCheck.GetCheck() == BST_CHECKED), m_DPTypeFilterCheck, m_DPTypeFilterCombo, DATAPOINT_TYPE_FILTER_OFFSET);
	}

	void CFilterDialog::OnDpvalueFilterEnabled() 
	{
        SetFilterFlagAndString( (m_DPValueFilterCheck.GetCheck() == BST_CHECKED), m_DPValueFilterCheck, m_DPValueFilterCombo, DATAPOINT_VALUE_FILTER_OFFSET);
	}

	void CFilterDialog::OnStatusFilterEnabled() 
	{
		bool isEnabled;
		CString szItemText;
		m_ItemTypeFilterCombo.GetLBText(m_ItemTypeFilterCombo.GetCurSel(), szItemText);


		if (m_StatusFilterCheck.GetCheck() == BST_CHECKED)
		{
			isEnabled = true;
		}
		else
		{
			isEnabled = false;
		}

		if (szItemText == REPORT_TYPE_EQUIPMENT)
		{
			m_Tagged_Check.EnableWindow(isEnabled);
			m_Tagged_PTW_Check.EnableWindow(isEnabled);
			if (m_ReportId == ACTIVE_PROFILE_ALL_REPORT)
			{
				m_Alarm_Inhibit_Check.EnableWindow(isEnabled);
				m_Control_Inhibit_Check.EnableWindow(isEnabled);
				m_Scan_Inhibit_Check.EnableWindow(isEnabled);
				m_Mms_Inhibit_Check.EnableWindow(isEnabled);
				m_Overridden_Check.EnableWindow(isEnabled);
			} else if (m_ReportId == ACTIVE_PROFILE_MMS_REPORT)
			{
				m_Alarm_Inhibit_Check.EnableWindow(FALSE);
				m_Control_Inhibit_Check.EnableWindow(FALSE);
				m_Scan_Inhibit_Check.EnableWindow(FALSE);
				m_Mms_Inhibit_Check.EnableWindow(FALSE);
				m_Overridden_Check.EnableWindow(FALSE);
				m_Tagged_PTW_Check.EnableWindow(FALSE);
			} else
			{
				m_Alarm_Inhibit_Check.EnableWindow(FALSE);
				m_Control_Inhibit_Check.EnableWindow(FALSE);
				m_Scan_Inhibit_Check.EnableWindow(FALSE);
				m_Mms_Inhibit_Check.EnableWindow(FALSE);
				m_Overridden_Check.EnableWindow(FALSE);
			}
		} 
		else if (szItemText == REPORT_TYPE_DATAPOINT)
		{
			m_Alarm_Inhibit_Check.EnableWindow(isEnabled);
			m_Control_Inhibit_Check.EnableWindow(isEnabled);
			m_Scan_Inhibit_Check.EnableWindow(isEnabled);
			m_Mms_Inhibit_Check.EnableWindow(isEnabled);

			m_Overridden_Check.EnableWindow(isEnabled);
			m_Tagged_Check.EnableWindow(FALSE);
			m_Tagged_PTW_Check.EnableWindow(FALSE);
		} else
		{
			m_Alarm_Inhibit_Check.EnableWindow(isEnabled);
			m_Control_Inhibit_Check.EnableWindow(isEnabled);
			m_Scan_Inhibit_Check.EnableWindow(isEnabled);
			m_Mms_Inhibit_Check.EnableWindow(isEnabled);

			m_Overridden_Check.EnableWindow(isEnabled);
			m_Tagged_Check.EnableWindow(isEnabled);
			m_Tagged_PTW_Check.EnableWindow(isEnabled);
		}
	}

	void CFilterDialog::OnStatuschangebyFilterEnabled() 
	{
        SetFilterFlagAndString( (m_StatusChangeByFilterCheck.GetCheck() == BST_CHECKED), m_StatusChangeByFilterCheck, m_StatusChangeByFilterCombo, MODIFIED_BY_FILTER_OFFSET);
	}

	void CFilterDialog::OnTimestampFilterEnabled() 
	{
        SetFilterFlagAndString( (m_TimestampFilterCheck.GetCheck() == BST_CHECKED), m_TimestampFilterCheck, m_TimestampFilterCombo, TIMESTAMP_FILTER_OFFSET);
	}


	void CFilterDialog::OnOK() 
	{
		// current filters will not be updated in the parent
		if (GetCurrentFilters() == false)
		{
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_083007);
			return;
		}

        // copy the changes to the active filters in the parent
        m_sourceFilters.swap( m_currentFilters);

		if (m_canConfigure == false) // do not save to DB
		{
			CDialog::OnOK();
			return;
		}

		// save the filters back into the database for future reference
		// get an instance of the report to save information into
		using TA_Base_Core::IConfigEquipReport;
		IConfigEquipReport *aReport = NULL;

		try
		{
			aReport = dynamic_cast<IConfigEquipReport*>(TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(m_ReportId, true));
		}
		catch (...)
		{
			CDialog::OnOK();
			return;
		}

		if (m_sourceFilters[REPORT_TYPE_FILTER_OFFSET].filterString == REPORT_TYPE_ALL)
			aReport->setReportTypeFilterFlag(false);
		else
			aReport->setReportTypeFilterFlag(true);
		aReport->setReportTypeFilter(m_sourceFilters[REPORT_TYPE_FILTER_OFFSET].filterString);

        aReport->setAssetFilterFlag(m_sourceFilters[ASSET_FILTER_OFFSET].filterInUse);
		aReport->setAssetFilter(m_sourceFilters[ASSET_FILTER_OFFSET].filterString);

		aReport->setDescFilterFlag(m_sourceFilters[DESCRIPTION_FILTER_OFFSET].filterInUse);
		aReport->setDescFilter(m_sourceFilters[DESCRIPTION_FILTER_OFFSET].filterString);

        if ( (m_sourceFilters[REPORT_TYPE_FILTER_OFFSET].filterString == REPORT_TYPE_ALL) ||
             (m_sourceFilters[REPORT_TYPE_FILTER_OFFSET].filterString == REPORT_TYPE_DATAPOINT) )
		{
			aReport->setDPNameFilterFlag(m_sourceFilters[DATAPOINT_NAME_FILTER_OFFSET].filterInUse);
			aReport->setDPNameFilter(m_sourceFilters[DATAPOINT_NAME_FILTER_OFFSET].filterString);

			aReport->setDPTypeFilterFlag(m_sourceFilters[DATAPOINT_TYPE_FILTER_OFFSET].filterInUse);
			aReport->setDPTypeFilter(m_sourceFilters[DATAPOINT_TYPE_FILTER_OFFSET].filterString);

			aReport->setDPValueFilterFlag(m_sourceFilters[DATAPOINT_VALUE_FILTER_OFFSET].filterInUse);
			aReport->setDPValueFilter(m_sourceFilters[DATAPOINT_VALUE_FILTER_OFFSET].filterString);
		}
        else
		{
			aReport->setDPNameFilterFlag(false);
			aReport->setDPNameFilter("");

			aReport->setDPTypeFilterFlag(false);
			aReport->setDPTypeFilter("");

			aReport->setDPValueFilterFlag(false);
			aReport->setDPValueFilter("");
		}

        // write the status flag
		aReport->setStatusFilterFlag(m_sourceFilters[STATUS_FILTER_OFFSET].filterInUse);

		// write the sub status flags
		aReport->setScanInhibitFlag(m_sourceFilters[STATUS_SCAN_INHIBIT_FILTER_OFFSET].filterInUse);
		aReport->setAlarmInhibitFlag(m_sourceFilters[STATUS_ALARM_INHIBIT_FILTER_OFFSET].filterInUse);
		aReport->setControlInhibitFlag(m_sourceFilters[STATUS_CONTROL_INHIBIT_FILTER_OFFSET].filterInUse);
		aReport->setMmsInhibitFlag(m_sourceFilters[STATUS_MMS_INHIBIT_FILTER_OFFSET].filterInUse);
		aReport->setTaggedFlag(m_sourceFilters[STATUS_TAGGED_FILTER_OFFSET].filterInUse);
		aReport->setTaggedPTWFlag(m_sourceFilters[STATUS_TAGGED_PTW_FILTER_OFFSET].filterInUse);
		aReport->setOverriddenFlag(m_sourceFilters[STATUS_OVERRIDDEN_FILTER_OFFSET].filterInUse);

		aReport->setStatusChangeByFilterFlag(m_sourceFilters[MODIFIED_BY_FILTER_OFFSET].filterInUse);
		aReport->setStatusChangeByFilter(m_sourceFilters[MODIFIED_BY_FILTER_OFFSET].filterString);

		aReport->setTimestampFilterFlag(m_sourceFilters[TIMESTAMP_FILTER_OFFSET].filterInUse);
		aReport->setTimestampFilter(m_sourceFilters[TIMESTAMP_FILTER_OFFSET].filterString);

		// finally commit all the changes to the database
		aReport->applyChanges();

		CDialog::OnOK();
	}


	void CFilterDialog::SetFilterFlagAndString( bool inUse, CButton& checkBox, CComboBox& comboBox, int filterIndex)
	{
		checkBox.EnableWindow(true);
		checkBox.SetCheck( inUse);
		comboBox.EnableWindow( inUse);

		if (inUse)
		{
            std::string sourceString = m_currentFilters.at(filterIndex).filterString;
            comboBox.SetWindowText( sourceString.c_str());
		}
	}


	// Gets the filter string if its related checkbox is ticked
    // Returns true if the input is valid
	bool CFilterDialog::GetFilterFlagAndString( CButton const& checkBox, CComboBox const& comboBox, int filterIndex )
	{
		if (checkBox.GetCheck() == BST_CHECKED)
		{
            CString s;
    		comboBox.GetWindowText( s);
            if (s.IsEmpty())
            {
                return false;
            }

            m_currentFilters.at(filterIndex).filterInUse = true;
            m_currentFilters.at(filterIndex).filterString = (LPCTSTR) s;
        }
        else
        {
            m_currentFilters.at(filterIndex).filterInUse = false;
            m_currentFilters.at(filterIndex).filterString = "";
        }
        return true;
	}


    void CFilterDialog::GetFilterFlagAndPredefinedString( CButton const& checkBox, int filterIndex, PredefinedFilterStrings pfsIndex )
	{
		if (checkBox.GetCheck() == BST_CHECKED)
		{
            m_currentFilters.at(filterIndex).filterInUse = true;
            m_currentFilters.at(filterIndex).filterString = FilterString[pfsIndex];
		}
		else
		{
            m_currentFilters.at(filterIndex).filterInUse = false;
            m_currentFilters.at(filterIndex).filterString = "";
		}
	}


	// Gets the filters which are enabled
	// they are the only ones setable for this session

	bool CFilterDialog::GetCurrentFilters()
	{
		if (m_currentFilters.size() != ESL_FILTER_OFFSET_MAX)
		{
			return false;
		}


        FilterFlagsAndStrings::iterator ffsIter = m_currentFilters.begin();
        for ( ; ffsIter != m_currentFilters.end(); ffsIter++)
        {
            ffsIter->filterInUse = false;
            ffsIter->filterString = "";
        }

		// get Item Type
		if (m_ItemTypeFilterCombo.GetCurSel() != 0) 
		{
			CString s("");

            m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterInUse = true;
			m_ItemTypeFilterCombo.GetLBText(m_ItemTypeFilterCombo.GetCurSel(), s);
            m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString = (LPCTSTR) s;
		}
		else
		{
            m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterInUse = false;
            m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString = REPORT_TYPE_ALL;
		}

		bool isInputOk = true;

        isInputOk &= GetFilterFlagAndString( m_AssetFilterCheck,       m_AssetFilterCombo,       ASSET_FILTER_OFFSET );
        isInputOk &= GetFilterFlagAndString( m_DescriptionFilterCheck, m_DescriptionFilterCombo, DESCRIPTION_FILTER_OFFSET );

		if ((REPORT_TYPE_ALL == m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString)
            ||
            (REPORT_TYPE_DATAPOINT == m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString) )
		{
            isInputOk &= GetFilterFlagAndString( m_DPNameFilterCheck,  m_DPNameFilterCombo,  DATAPOINT_NAME_FILTER_OFFSET );
            isInputOk &= GetFilterFlagAndString( m_DPTypeFilterCheck,  m_DPTypeFilterCombo,  DATAPOINT_TYPE_FILTER_OFFSET );
            isInputOk &= GetFilterFlagAndString( m_DPValueFilterCheck, m_DPValueFilterCombo, DATAPOINT_VALUE_FILTER_OFFSET );
		}

		if (m_StatusFilterCheck.GetCheck() == BST_CHECKED)
		{
            m_currentFilters.at(STATUS_FILTER_OFFSET).filterInUse = true;

			// get all the sub status filter flags
    		if (REPORT_TYPE_EQUIPMENT == m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString)
			{
                // Equipment rows will only have inhibit text if the report
                // is a summary report (ACTIVE_PROFILE_..._DATA)
				if (m_ReportId == ACTIVE_PROFILE_ALL_REPORT)
				{
                	GetFilterFlagAndPredefinedString( m_Scan_Inhibit_Check,    STATUS_SCAN_INHIBIT_FILTER_OFFSET,    SCAN_FILTERSTRING);
                	GetFilterFlagAndPredefinedString( m_Alarm_Inhibit_Check,   STATUS_ALARM_INHIBIT_FILTER_OFFSET,	 ALARM_FILTERSTRING);
                	GetFilterFlagAndPredefinedString( m_Control_Inhibit_Check, STATUS_CONTROL_INHIBIT_FILTER_OFFSET, CONTROL_FILTERSTRING);
                	GetFilterFlagAndPredefinedString( m_Mms_Inhibit_Check,     STATUS_MMS_INHIBIT_FILTER_OFFSET,	 MMS_FILTERSTRING);
				}

                GetFilterFlagAndPredefinedString( m_Tagged_Check,     STATUS_TAGGED_FILTER_OFFSET,	   TAGGED_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Tagged_PTW_Check, STATUS_TAGGED_PTW_FILTER_OFFSET, TAGGED_PTW_FILTERSTRING);

				if (m_ReportId == ACTIVE_PROFILE_ALL_REPORT)
				{

                    GetFilterFlagAndPredefinedString( m_Overridden_Check, STATUS_OVERRIDDEN_FILTER_OFFSET, OVERRIDDEN_FILTERSTRING);
				}

				if (m_ReportId == ACTIVE_PROFILE_ALL_REPORT)
				{
                    if (! (m_currentFilters.at(STATUS_SCAN_INHIBIT_FILTER_OFFSET).filterInUse    ||
                           m_currentFilters.at(STATUS_ALARM_INHIBIT_FILTER_OFFSET).filterInUse   ||
                           m_currentFilters.at(STATUS_CONTROL_INHIBIT_FILTER_OFFSET).filterInUse ||
                           m_currentFilters.at(STATUS_MMS_INHIBIT_FILTER_OFFSET).filterInUse     ||
                           m_currentFilters.at(STATUS_TAGGED_FILTER_OFFSET).filterInUse          ||
                           m_currentFilters.at(STATUS_TAGGED_PTW_FILTER_OFFSET).filterInUse      ||
                           m_currentFilters.at(STATUS_OVERRIDDEN_FILTER_OFFSET).filterInUse ) )
					{
                        isInputOk = false;
					}
				}
				else
				{
                    if (! (m_currentFilters.at(STATUS_TAGGED_FILTER_OFFSET).filterInUse          ||
                           m_currentFilters.at(STATUS_TAGGED_PTW_FILTER_OFFSET).filterInUse ) )
					{
						isInputOk = false;
					}
				}

			}
			else if (REPORT_TYPE_DATAPOINT == m_currentFilters.at(REPORT_TYPE_FILTER_OFFSET).filterString)
			{
                GetFilterFlagAndPredefinedString( m_Scan_Inhibit_Check,    STATUS_SCAN_INHIBIT_FILTER_OFFSET,    SCAN_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Alarm_Inhibit_Check,   STATUS_ALARM_INHIBIT_FILTER_OFFSET,	 ALARM_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Control_Inhibit_Check, STATUS_CONTROL_INHIBIT_FILTER_OFFSET, CONTROL_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Mms_Inhibit_Check,     STATUS_MMS_INHIBIT_FILTER_OFFSET,	 MMS_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Overridden_Check,      STATUS_OVERRIDDEN_FILTER_OFFSET,      OVERRIDDEN_FILTERSTRING);

                if (! (m_currentFilters.at(STATUS_SCAN_INHIBIT_FILTER_OFFSET).filterInUse    ||
                       m_currentFilters.at(STATUS_ALARM_INHIBIT_FILTER_OFFSET).filterInUse   ||
                       m_currentFilters.at(STATUS_CONTROL_INHIBIT_FILTER_OFFSET).filterInUse ||
                       m_currentFilters.at(STATUS_MMS_INHIBIT_FILTER_OFFSET).filterInUse     ||
                       m_currentFilters.at(STATUS_OVERRIDDEN_FILTER_OFFSET).filterInUse ) )
				{
					isInputOk = false;
				}
			}
			else
			{   //  Equipment AND datapoints

                GetFilterFlagAndPredefinedString( m_Scan_Inhibit_Check,    STATUS_SCAN_INHIBIT_FILTER_OFFSET,    SCAN_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Alarm_Inhibit_Check,   STATUS_ALARM_INHIBIT_FILTER_OFFSET,	 ALARM_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Control_Inhibit_Check, STATUS_CONTROL_INHIBIT_FILTER_OFFSET, CONTROL_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Mms_Inhibit_Check,     STATUS_MMS_INHIBIT_FILTER_OFFSET,	 MMS_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Tagged_Check,          STATUS_TAGGED_FILTER_OFFSET,	         TAGGED_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Tagged_PTW_Check,      STATUS_TAGGED_PTW_FILTER_OFFSET,      TAGGED_PTW_FILTERSTRING);
                GetFilterFlagAndPredefinedString( m_Overridden_Check,      STATUS_OVERRIDDEN_FILTER_OFFSET,      OVERRIDDEN_FILTERSTRING);

                if (! (m_currentFilters.at(STATUS_SCAN_INHIBIT_FILTER_OFFSET).filterInUse    ||
                       m_currentFilters.at(STATUS_ALARM_INHIBIT_FILTER_OFFSET).filterInUse   ||
                       m_currentFilters.at(STATUS_CONTROL_INHIBIT_FILTER_OFFSET).filterInUse ||
                       m_currentFilters.at(STATUS_MMS_INHIBIT_FILTER_OFFSET).filterInUse     ||
                       m_currentFilters.at(STATUS_TAGGED_FILTER_OFFSET).filterInUse          ||
                       m_currentFilters.at(STATUS_TAGGED_PTW_FILTER_OFFSET).filterInUse      ||
                       m_currentFilters.at(STATUS_OVERRIDDEN_FILTER_OFFSET).filterInUse ) )
				{
					isInputOk = false;
				}
			}
		}

        isInputOk &= GetFilterFlagAndString( m_StatusChangeByFilterCheck, m_StatusChangeByFilterCombo, MODIFIED_BY_FILTER_OFFSET );
        isInputOk &= GetFilterFlagAndString( m_TimestampFilterCheck, m_TimestampFilterCombo, TIMESTAMP_FILTER_OFFSET );

		return isInputOk;
	}


	void CFilterDialog::OnSelchangeItemtypeFilter() 
	{
		CString szItemText;

		m_ItemTypeFilterCombo.GetLBText(m_ItemTypeFilterCombo.GetCurSel(), szItemText);

		if (szItemText == REPORT_TYPE_EQUIPMENT)
		{
			m_DPNameFilterCheck.EnableWindow(FALSE);
			m_DPNameFilterCombo.EnableWindow(FALSE);
			m_DPTypeFilterCheck.EnableWindow(FALSE);
			m_DPTypeFilterCombo.EnableWindow(FALSE);
			m_DPValueFilterCheck.EnableWindow(FALSE);
			m_DPValueFilterCombo.EnableWindow(FALSE);

			PopulateComboBoxWithStringSet( m_AssetFilterCombo, m_equipmentAssetComboMap[REPORT_TYPE_EQUIPMENT]);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, m_equipmentDescriptionComboMap[REPORT_TYPE_EQUIPMENT]);

			if (m_StatusFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_Tagged_Check.EnableWindow(TRUE);
				m_Tagged_PTW_Check.EnableWindow(TRUE);
				if (m_ReportId == ACTIVE_PROFILE_ALL_REPORT)
				{
					m_Alarm_Inhibit_Check.EnableWindow(TRUE);
					m_Scan_Inhibit_Check.EnableWindow(TRUE);
					m_Control_Inhibit_Check.EnableWindow(TRUE);
					m_Mms_Inhibit_Check.EnableWindow(TRUE);
					m_Overridden_Check.EnableWindow(TRUE);
				} else if (m_ReportId == ACTIVE_PROFILE_MMS_REPORT)
				{
					m_Alarm_Inhibit_Check.EnableWindow(FALSE);
					m_Scan_Inhibit_Check.EnableWindow(FALSE);
					m_Control_Inhibit_Check.EnableWindow(FALSE);
					m_Mms_Inhibit_Check.EnableWindow(FALSE);
					m_Overridden_Check.EnableWindow(FALSE);
					m_Tagged_PTW_Check.EnableWindow(FALSE);
				} else
				{
					m_Alarm_Inhibit_Check.EnableWindow(FALSE);
					m_Scan_Inhibit_Check.EnableWindow(FALSE);
					m_Control_Inhibit_Check.EnableWindow(FALSE);
					m_Mms_Inhibit_Check.EnableWindow(FALSE);
					m_Overridden_Check.EnableWindow(FALSE);
				}
			}
			else
			{	m_Tagged_Check.EnableWindow(FALSE);
				m_Tagged_PTW_Check.EnableWindow(FALSE);
				m_Alarm_Inhibit_Check.EnableWindow(FALSE);
				m_Scan_Inhibit_Check.EnableWindow(FALSE);
				m_Control_Inhibit_Check.EnableWindow(FALSE);
				m_Mms_Inhibit_Check.EnableWindow(FALSE);
				m_Overridden_Check.EnableWindow(FALSE);
			}

		}
		else if (szItemText == REPORT_TYPE_DATAPOINT)
		{
			PopulateComboBoxWithStringSet( m_AssetFilterCombo, m_equipmentAssetComboMap[REPORT_TYPE_DATAPOINT]);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, m_equipmentDescriptionComboMap[REPORT_TYPE_DATAPOINT]);

			m_DPNameFilterCheck.EnableWindow(TRUE);

			if (m_DPNameFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPNameFilterCombo.EnableWindow(TRUE);
			}

			m_DPTypeFilterCheck.EnableWindow(TRUE);

			if (m_DPTypeFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPTypeFilterCombo.EnableWindow(TRUE);
			}

			m_DPValueFilterCheck.EnableWindow(TRUE);

			if (m_DPValueFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPValueFilterCombo.EnableWindow(TRUE);
			}

			if (m_StatusFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_Alarm_Inhibit_Check.EnableWindow(TRUE);
				m_Scan_Inhibit_Check.EnableWindow(TRUE);
				m_Control_Inhibit_Check.EnableWindow(TRUE);
				m_Mms_Inhibit_Check.EnableWindow(TRUE);
				m_Overridden_Check.EnableWindow(TRUE);
			}
			else
			{
				m_Alarm_Inhibit_Check.EnableWindow(FALSE);
				m_Scan_Inhibit_Check.EnableWindow(FALSE);
				m_Control_Inhibit_Check.EnableWindow(FALSE);
				m_Mms_Inhibit_Check.EnableWindow(FALSE);
				m_Overridden_Check.EnableWindow(FALSE);
			}

			m_Tagged_Check.EnableWindow(FALSE);
			m_Tagged_PTW_Check.EnableWindow(FALSE);
		}
		else if (szItemText == REPORT_TYPE_ALL)
		{
			PopulateComboBoxWithStringSet( m_AssetFilterCombo, m_equipmentAssetComboMap[REPORT_TYPE_ALL]);
			PopulateComboBoxWithStringSet( m_DescriptionFilterCombo, m_equipmentDescriptionComboMap[REPORT_TYPE_ALL]);

			if (m_DPNameFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPNameFilterCheck.EnableWindow();
			}
			else 
			{
				m_DPNameFilterCheck.EnableWindow(FALSE);
			}

			m_DPNameFilterCheck.EnableWindow();


			if (m_DPTypeFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPTypeFilterCombo.EnableWindow();
			}
			else
			{
				m_DPTypeFilterCombo.EnableWindow(FALSE);
			}

			m_DPTypeFilterCheck.EnableWindow();

			if (m_DPValueFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_DPValueFilterCombo.EnableWindow();
			}
			else
			{
				m_DPValueFilterCombo.EnableWindow(FALSE);
			}

			m_DPValueFilterCheck.EnableWindow();

			if (m_StatusFilterCheck.GetCheck() == BST_CHECKED)
			{
				m_Alarm_Inhibit_Check.EnableWindow();
				m_Scan_Inhibit_Check.EnableWindow();
				m_Control_Inhibit_Check.EnableWindow();
				m_Mms_Inhibit_Check.EnableWindow();
				m_Overridden_Check.EnableWindow();
				m_Tagged_Check.EnableWindow();
				m_Tagged_PTW_Check.EnableWindow();
			}
		}

	}

    void CFilterDialog::OnClearButton()
    {
        // Clear all combo boxes to have null selection (except for item type), and
        // uncheck all checkboxes
        CWnd* pWndChild = GetWindow(GW_CHILD);

        // Iterate through all controls on page
        while (0 != pWndChild)
        {
    		if (pWndChild == &m_ItemTypeFilterCombo)
    		{
                // Special case, set item type to default setting
                m_ItemTypeFilterCombo.SetCurSel(0);
                // Manually invoke sel changed notification
                OnSelchangeItemtypeFilter();
    		}
            else
            {
                // Determine if checkbox, or combo
                if (0 != pWndChild->m_hWnd)
                {
                    enum { BUF_LEN  = 64 };
                    char buf[BUF_LEN];

                    GetClassName(pWndChild->m_hWnd, buf, BUF_LEN);

                    if (0 == strcmp(buf, "Button"))
                    {
                        // Check if a checkbox button
                        if (pWndChild->GetStyle() & BS_CHECKBOX)
                        {
                            // Clear all checkboxes
                            pWndChild->SendMessage(BM_SETCHECK, BST_UNCHECKED, 0);
                        }
                    }
                    else if (0 == strcmp(buf, "ComboBox"))
                    {
                        // Clear all combo box selections
                        pWndChild->SendMessage(CB_SETCURSEL, -1, 0);
                        // Disable all combo boxes (since we know associated checkbox
                        // is going to be cleared) - this also deals with a qwerk in the way
                        // the checkbox check/uncheck notifications are handled (it doesn't
                        // check the checkbox state..)
                        pWndChild->EnableWindow(FALSE);
                    }
                }
            }

            // When changing the checked state of the status checkbox, must change
            // the enabled states of the associated grouping of checkboxes..
            if (pWndChild == &m_StatusFilterCheck)
            {
                OnStatusFilterEnabled();
            }
    	    pWndChild = pWndChild->GetNextWindow(GW_HWNDNEXT);
        }
    }

} // TA_Base_App

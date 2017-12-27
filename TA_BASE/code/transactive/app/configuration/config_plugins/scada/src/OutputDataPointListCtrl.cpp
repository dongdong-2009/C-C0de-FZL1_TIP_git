/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/OutputDataPointListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the output association data points.
  */


#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/OutputDataPointListCtrl.h"
#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedOutputAssociation.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These are the set up details for the attribute list control
    const CString OutputDataPointListCtrl::STATE_COLUMN_NAME = "State";
    const int OutputDataPointListCtrl::STATE_COLUMN_WIDTH    = 100;
    const int OutputDataPointListCtrl::STATE_COLUMN     = 0;

    const CString OutputDataPointListCtrl::DATAPOINT_COLUMN_NAME = "DataPoint";
    const int OutputDataPointListCtrl::DATAPOINT_COLUMN_WIDTH    = 300;
    const int OutputDataPointListCtrl::DATAPOINT_COLUMN     = 1;

    const CString OutputDataPointListCtrl::OUTPUT_VALUE_COLUMN_NAME = "Output Value";
    const int OutputDataPointListCtrl::OUTPUT_VALUE_COLUMN_WIDTH    = 200;
    const int OutputDataPointListCtrl::OUTPUT_VALUE_COLUMN          = 2;

    const int OutputDataPointListCtrl::PARAMETER_VALUE_NUMBER_LIMIT  = 9;

    OutputDataPointListCtrl::OutputDataPointListCtrl() : m_derivedDataPoint(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    OutputDataPointListCtrl::~OutputDataPointListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(OutputDataPointListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(OutputDataPointListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void OutputDataPointListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void OutputDataPointListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(STATE_COLUMN,        STATE_COLUMN_NAME,        LVCFMT_LEFT, STATE_COLUMN_WIDTH);
        InsertColumn(DATAPOINT_COLUMN,    DATAPOINT_COLUMN_NAME,    LVCFMT_LEFT, DATAPOINT_COLUMN_WIDTH);
        InsertColumn(OUTPUT_VALUE_COLUMN, OUTPUT_VALUE_COLUMN_NAME, LVCFMT_LEFT, OUTPUT_VALUE_COLUMN_WIDTH);


        FUNCTION_EXIT;
    }


    void OutputDataPointListCtrl::populateControl(TA_Base_Core::IConfigDerivedDataPoint& derivedDataPoint)
    {
        FUNCTION_ENTRY("populateControl");

        m_derivedDataPoint = &derivedDataPoint;

        // Remove all old associations and add the new ones
        DeleteAllItems();

        try
        {
			typedef TA_Base_Core::IConfigDerivedDataPoint::OutputAssociationMap OutputAssociationMap;
            const OutputAssociationMap& outputs = m_derivedDataPoint->getOutputAssociations();

            for(OutputAssociationMap::const_iterator iter = outputs.begin(); iter != outputs.end(); ++iter)
            {
                CString state;
                state.Format("%d", iter->first);
                int position = InsertItem(-1, state, 0);

                SetItemText(position, DATAPOINT_COLUMN, iter->second->getEntityName().c_str());

                CString outputValue;
                outputValue.Format("%d",iter->second->getOutputValue());
                SetItemText(position, OUTPUT_VALUE_COLUMN, outputValue);

                SetItemData(position, reinterpret_cast<unsigned long>(iter->second));
            }

        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database Exception", "Could not generate a list of output associations for this derived datapoint");
        }

        FUNCTION_EXIT;
    }


    void OutputDataPointListCtrl::addNewItem()
    {
        FUNCTION_ENTRY("addNewItem");

        try
        {
            // Generate a list of datapoints we already have as input associations so we don't add another
            // the same.
            std::vector<unsigned long> dataPointsToIgnore;

            typedef TA_Base_Core::IConfigDerivedDataPoint::OutputAssociationMap OutputAssociationMap;
            const OutputAssociationMap& outputs = m_derivedDataPoint->getOutputAssociations();

            for(OutputAssociationMap::const_iterator iter = outputs.begin(); iter != outputs.end(); ++iter)
            {
                dataPointsToIgnore.push_back( iter->second->getOutputEntityKey() );
            }

            // If this is not a new derived data point then we should ignore this data point as well
            if( !m_derivedDataPoint->isNew() )
            {
                dataPointsToIgnore.push_back(m_derivedDataPoint->getKey()); 
            }
        
            SelectAssociationEntityFactory factory(dataPointsToIgnore, false, true);

            // Pass in the number of states we have
	        AddAssociationDialog dlg(m_derivedDataPoint->getDerivedStates().size(), factory);
            if (dlg.DoModal() == IDCANCEL)
            {
                FUNCTION_EXIT;
                return;
            }

            unsigned long key = dlg.getSelectedKey();
            int state = dlg.getSelectedState();

            m_derivedDataPoint->addOutputAssociation( state, key );

            populateControl(*m_derivedDataPoint);
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Database Exception", "Cannot add a new output association");
            
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        FUNCTION_EXIT;
    }


    void OutputDataPointListCtrl::removeSelectedItem()
    {
        FUNCTION_ENTRY("removeSelectedItem");

        try
        {
            POSITION pos = GetFirstSelectedItemPosition();
            if (pos == NULL)
            {
                FUNCTION_EXIT;
                return;
            }

            int selected = GetNextSelectedItem(pos);

	        TA_ASSERT (m_derivedDataPoint != NULL,"We do not yet have an IConfigDerivedDataPoint* item set");

            // Retrieve the state from the list and convert it into an integer
            std::istringstream stateString;
            stateString.str( std::string(GetItemText(selected, STATE_COLUMN)) );
            int state;
            stateString >> state;

            using TA_Base_Core::ConfigDerivedOutputAssociation;
            ConfigDerivedOutputAssociation* outputAssoc;
            outputAssoc = reinterpret_cast<ConfigDerivedOutputAssociation*>(GetItemData(selected));

            m_derivedDataPoint->removeOutputAssociation( state, outputAssoc->getOutputEntityKey() );

            populateControl(*m_derivedDataPoint);
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Database Exception", "Cannot remove an output association");
            
            CString actionName, reason;
            actionName.LoadString(IDS_DELETE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        

        FUNCTION_EXIT;
    }

            
    ICellManager::CellType OutputDataPointListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

        if (subItem == OUTPUT_VALUE_COLUMN)
        {
            return CT_NUMBER;
        }

        return CT_NOTEDITABLE;
    }


    unsigned long OutputDataPointListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        if (subItem == OUTPUT_VALUE_COLUMN)
        {
            return PARAMETER_VALUE_NUMBER_LIMIT;
        }
        return 0;
    }



    ICellManager::CellData OutputDataPointListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        /// No CT_SELECTION cells so nothing is required here
	    ICellManager::CellData data;
        return data;
    }


    CString OutputDataPointListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char OutputDataPointListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool OutputDataPointListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
        using TA_Base_Core::ConfigDerivedOutputAssociation;

        ConfigDerivedOutputAssociation* outputAssoc = reinterpret_cast<ConfigDerivedOutputAssociation*>(listCtrl->GetItemData(item));

        try
        {
            if (outputAssoc != NULL)
            {
                if (subItem == OUTPUT_VALUE_COLUMN)
                {
                    // First check if this is a valid real number
                    FieldValidityChecker checker;
                    if (!checker.isValidInteger(std::string(value)) )
                    {
                        return false;
                    }

                    // Retrieve the double out of the string
                    std::istringstream theValue;
                    theValue.str(std::string(value));
                    int outputValue;
                    theValue >> outputValue;

                    // Now set the value
                    outputAssoc->setOutputValue(outputValue);
                }

                // First GetParent() gives  us the property pages, then the next gives us the property sheet and
                // finally we get the detail view.
                AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent()->GetParent());
                if (detailView != NULL)
                {
                    detailView->refreshButtonAvailability();
                }

                return true;
            }
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not change the output association details");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not change the output association details");
        }
        
        return false;
    }

}


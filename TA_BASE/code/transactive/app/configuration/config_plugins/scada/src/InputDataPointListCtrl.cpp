/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/InputDataPointListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the input association data points.
  */


#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/InputDataPointListCtrl.h"
#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.h"

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
    const CString InputDataPointListCtrl::DATAPOINT_COLUMN_NAME = "DataPoint";
    const int InputDataPointListCtrl::DATAPOINT_COLUMN_WIDTH    = 300;
    const int InputDataPointListCtrl::DATAPOINT_COLUMN     = 0;

    const CString InputDataPointListCtrl::CALC_ORDER_COLUMN_NAME = "Calculation Order";
    const int InputDataPointListCtrl::CALC_ORDER_COLUMN_WIDTH    = 130;
    const int InputDataPointListCtrl::CALC_ORDER_COLUMN          = 1;

    const CString InputDataPointListCtrl::PREDEFINED_VALUE_COLUMN_NAME = "Predefined Input Value";
    const int InputDataPointListCtrl::PREDEFINED_VALUE_COLUMN_WIDTH    = 180;
    const int InputDataPointListCtrl::PREDEFINED_VALUE_COLUMN          = 2;

    const int InputDataPointListCtrl::PARAMETER_VALUE_NUMBER_LIMIT  = 9;

    InputDataPointListCtrl::InputDataPointListCtrl() : m_derivedDataPoint(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    InputDataPointListCtrl::~InputDataPointListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(InputDataPointListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(InputDataPointListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void InputDataPointListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void InputDataPointListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(DATAPOINT_COLUMN,        DATAPOINT_COLUMN_NAME,        LVCFMT_LEFT, DATAPOINT_COLUMN_WIDTH);
        InsertColumn(CALC_ORDER_COLUMN,       CALC_ORDER_COLUMN_NAME,       LVCFMT_LEFT, CALC_ORDER_COLUMN_WIDTH);
        InsertColumn(PREDEFINED_VALUE_COLUMN, PREDEFINED_VALUE_COLUMN_NAME, LVCFMT_LEFT, PREDEFINED_VALUE_COLUMN_WIDTH);

        FUNCTION_EXIT;
    }


    void InputDataPointListCtrl::populateControl(TA_Base_Core::IConfigDerivedDataPoint& derivedDataPoint)
    {
        FUNCTION_ENTRY("populateControl");

        m_derivedDataPoint = &derivedDataPoint;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {
			typedef TA_Base_Core::IConfigDerivedDataPoint::InputAssociationMap InputAssociationMap;
            const InputAssociationMap& inputs = m_derivedDataPoint->getInputAssociations();

            for(InputAssociationMap::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter)
            {
                int position = InsertItem(-1,iter->second->getEntityName().c_str(), 0);

                CString calculationOrder;
                calculationOrder.Format("%d", iter->second->getInputCalculationOrder());
                SetItemText(position, CALC_ORDER_COLUMN, calculationOrder);

                CString predefinedValue;
                predefinedValue.Format("%f",iter->second->getPredefinedInputValue());
                SetItemText(position, PREDEFINED_VALUE_COLUMN, predefinedValue);
                
                SetItemData(position, reinterpret_cast<unsigned long>(iter->second));
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database Exception", "Could not generate a list of input associations for this Derived DataPoint");
        }

        FUNCTION_EXIT;
    }


    void InputDataPointListCtrl::addNewItem()
    {
        FUNCTION_ENTRY("addNewItem");

        try
        {
            // Generate a list of datapoints we already have as input associations so we don't add another
            // the same.
            std::vector<unsigned long> dataPointsToIgnore;

            typedef TA_Base_Core::IConfigDerivedDataPoint::InputAssociationMap InputAssociationMap;
            const InputAssociationMap& inputs = m_derivedDataPoint->getInputAssociations();

            for(InputAssociationMap::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter)
            {
                dataPointsToIgnore.push_back( iter->second->getInputEntityKey() );
            }

            // If this is not a new derived data point then we should ignore this data point as well
            if( !m_derivedDataPoint->isNew() )
            {
                dataPointsToIgnore.push_back(m_derivedDataPoint->getKey()); 
            }
        
            SelectAssociationEntityFactory factory(dataPointsToIgnore, true, false);

	        AddAssociationDialog dlg(0, factory); // pass in that there are zero states
            if (dlg.DoModal() == IDCANCEL)
            {
                FUNCTION_EXIT;
                return;
            }

            unsigned long key = dlg.getSelectedKey();
            CString name = dlg.getSelectedName();

            m_derivedDataPoint->addInputAssociation( key );

            populateControl(*m_derivedDataPoint);
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Database Exception", "Cannot add a new input association");
            
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        FUNCTION_EXIT;
    }


    void InputDataPointListCtrl::removeSelectedItem()
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

            using TA_Base_Core::ConfigConfiguredInputAssociation;
            ConfigConfiguredInputAssociation* inputAssoc;
            inputAssoc = reinterpret_cast<ConfigConfiguredInputAssociation*>(GetItemData(selected));

            m_derivedDataPoint->removeInputAssociation( inputAssoc->getInputEntityKey() );

            populateControl(*m_derivedDataPoint);
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Database Exception", "Cannot remove an input association");
            
            CString actionName, reason;
            actionName.LoadString(IDS_DELETE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }


        FUNCTION_EXIT;
    }

            
    ICellManager::CellType InputDataPointListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

        if (subItem == CALC_ORDER_COLUMN)
        {
            return CT_SELECTION;
        }

        if (subItem == PREDEFINED_VALUE_COLUMN)
        {
            return CT_NUMBER;
        }

        return CT_NOTEDITABLE;
    }


    unsigned long InputDataPointListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        if (subItem == PREDEFINED_VALUE_COLUMN)
        {
            return PARAMETER_VALUE_NUMBER_LIMIT;
        }
        return 0;
    }


    ICellManager::CellData InputDataPointListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    ICellManager::CellData data;

        for (int i = 0; i < listCtrl->GetItemCount(); ++i)
        {
            CString number;
            number.Format("%d",i);
            data.push_back( std::string(number) );
        }

	    return data;
    }


    CString InputDataPointListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char InputDataPointListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool InputDataPointListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        using TA_Base_Core::ConfigConfiguredInputAssociation;

        ConfigConfiguredInputAssociation* inputAssoc = reinterpret_cast<ConfigConfiguredInputAssociation*>(listCtrl->GetItemData(item));

        try
        {
            if (inputAssoc != NULL)
            {
                if (subItem == CALC_ORDER_COLUMN)
                {
                    // Retrieve the integer out of the string
                    std::istringstream theValue;
                    theValue.str(std::string(value));
                    int calculationOrder;
                    theValue >> calculationOrder;

                    // Now set the value
                    inputAssoc->setInputCalculationOrder(calculationOrder);
                }
                else if (subItem == PREDEFINED_VALUE_COLUMN)
                {
                    // First check if this is a valid real number
                    FieldValidityChecker checker;
                    if (!checker.isValidRealNumber(std::string(value)) )
                    {
                        return false;
                    }

                    // Retrieve the double out of the string
                    std::istringstream theValue;
                    theValue.str(std::string(value));
                    double predefinedValue;
                    theValue >> predefinedValue;

                    // Now set the value
                    inputAssoc->setPredefinedInputValue(predefinedValue);
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not change the input association details");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not change the input association details");
        }

        return false;
    }

}


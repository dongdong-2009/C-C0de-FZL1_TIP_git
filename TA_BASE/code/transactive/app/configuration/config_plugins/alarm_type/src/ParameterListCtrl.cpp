/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/ParameterListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the entity parameters in the EntityView class
  */

#include "app/configuration/config_plugins/alarm_type/src/stdafx.h"
#include "app/configuration/config_plugins/alarm_type/src/ParameterListCtrl.h"
#include "app/configuration/config_plugins/alarm_type/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigAlarmType.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "app/configuration/config_plugins/alarm_type/src/ChineseSupport.h"
using namespace alarmType_chinese;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These are the set up details for the attribute list control

    const int     ParameterListCtrl::ATTRIBUTE_NAME_COLUMN_WIDTH  = 190;
    const int     ParameterListCtrl::ATTRIBUTE_NAME_COLUMN        = 0;
   
    const int     ParameterListCtrl::ATTRIBUTE_VALUE_COLUMN_WIDTH = 210;
    const int     ParameterListCtrl::ATTRIBUTE_VALUE_COLUMN       = 1;
    
    const int     ParameterListCtrl::ATTRIBUTE_UPDATE_COLUMN_WIDTH= 70;
    const int     ParameterListCtrl::ATTRIBUTE_UPDATE_COLUMN      = 2;

    const int ParameterListCtrl::PARAMETER_VALUE_TEXT_LIMIT    = 256;
    const int ParameterListCtrl::PARAMETER_VALUE_NUMBER_LIMIT  = 9;

    ParameterListCtrl::ParameterListCtrl() : m_currentAlarmType(NULL),
                                             m_maximumCharactersInField(0),
                                             CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    ParameterListCtrl::~ParameterListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(ParameterListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(ParameterListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ParameterListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void ParameterListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(ATTRIBUTE_NAME_COLUMN,   ATTRIBUTE_NAME_COLUMN_NAME,   LVCFMT_LEFT, ATTRIBUTE_NAME_COLUMN_WIDTH);
        InsertColumn(ATTRIBUTE_VALUE_COLUMN,  ATTRIBUTE_VALUE_COLUMN_NAME,  LVCFMT_LEFT ,ATTRIBUTE_VALUE_COLUMN_WIDTH);
        InsertColumn(ATTRIBUTE_UPDATE_COLUMN, ATTRIBUTE_UPDATE_COLUMN_NAME, LVCFMT_LEFT ,ATTRIBUTE_UPDATE_COLUMN_WIDTH);

        m_imageList.Create(IDB_BITMAP_MANDATORY,10,10,ILC_COLOR);
        SetImageList(&m_imageList,LVSIL_SMALL);


        FUNCTION_EXIT;
    }


    void ParameterListCtrl::populateControl(TA_Base_Core::IConfigAlarmType* currentAlarmType)
    {
        FUNCTION_ENTRY("populateControl");

        TA_ASSERT(currentAlarmType != NULL, "IConfigAlarmType passed was NULL");
        m_currentAlarmType = currentAlarmType;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        std::vector<std::string> parameterNames = m_currentAlarmType->getParameterNames();
        for( std::vector<std::string>::iterator iter = parameterNames.begin();
             iter != parameterNames.end();
             ++iter)
        {
			// All parameters are mandatory for current implementation
            int imageNumber = 0; // This indicates the image should be used

            // Insert the parameter name in the first column
            int position = InsertItem(-1,iter->c_str(), imageNumber);
        
            std::string parameterValue = m_currentAlarmType->getParameterValue(*iter);

            // Insert the parameter value in the second column
            SetItemText(position, ATTRIBUTE_VALUE_COLUMN, parameterValue.c_str());

            CString updateText = BOOLEAN_TRUE.c_str(); // Need restart ie Yes
			/* // All current parameters are not online updatable
            if( !m_currentAlarmType->isParameterOnlineUpdatable(*iter) )
            {
                updateText = BOOLEAN_TRUE.c_str();
            }*/
            SetItemText(position, ATTRIBUTE_UPDATE_COLUMN, updateText);
        }

        FUNCTION_EXIT;
    }

	void ParameterListCtrl::resetParameters()
	{
        FUNCTION_ENTRY("resetParameters");

        CString parameterName;
        try
        {
			std::vector<std::string> parameterNames = m_currentAlarmType->getParameterNames();
			for( std::vector<std::string>::iterator iter = parameterNames.begin();
				 iter != parameterNames.end();
				 ++iter)
			{
				parameterName=(*iter).c_str();
				// Get the current value
				std::string parameterValue = m_currentAlarmType->getParameterValue(*iter);
				if (parameterValue.empty() == false)
				{
					// Set the value to empty string
					m_currentAlarmType->setParameterValue( "", *iter );
				}
			}

			DeleteAllItems();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set the parameter");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Parameter entered was of the wrong type");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << parameterName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030062);
        }

        FUNCTION_EXIT;
	}

	bool ParameterListCtrl::validateEmptyValues()
	{
        FUNCTION_ENTRY("validateEmptyValues");
		std::string invalidParam;

        std::vector<std::string> parameterNames = m_currentAlarmType->getParameterNames();
        for( std::vector<std::string>::iterator iter = parameterNames.begin();
             iter != parameterNames.end();
             ++iter)
		{
			// Get the current value
            std::string parameterValue = m_currentAlarmType->getParameterValue(*iter);
			if (parameterValue.empty() == true)
			{
				if (invalidParam.empty()) 
				{
					invalidParam = *iter;
				}
				else 
				{
					invalidParam += " + " + *iter;
				}
			}
		}

		if (invalidParam.empty() == false)
		{
			// display a msg box
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << invalidParam;
            userMsg.showMsgBox(IDS_UE_030062);

			return true;
		}

		return false;

        FUNCTION_EXIT;
	}

    ICellManager::CellType ParameterListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentAlarmType != NULL,"We do not yet have an IConfigAlarmType* item set");

        m_maximumCharactersInField = 0;

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is editable
	    if (subItem != ATTRIBUTE_VALUE_COLUMN)
        {
            return CT_NOTEDITABLE;
        }
		
		// Current fields are all text
        m_maximumCharactersInField = PARAMETER_VALUE_TEXT_LIMIT;
        return CT_TEXT;
    }


    unsigned long ParameterListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return m_maximumCharactersInField;
    }


    ICellManager::CellData ParameterListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT (m_currentAlarmType != NULL,"We do not yet have an IConfigAlarmType* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

	    ICellManager::CellData data;

	    if (subItem != ATTRIBUTE_VALUE_COLUMN)
	    {
            return data;
        }

		// Since all parameters are currently text, combox values are not necessary
	    return data;
    }


    CString ParameterListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char ParameterListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool ParameterListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentAlarmType != NULL,"We do not yet have an IConfigAlarmType* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        CString parameterName = listCtrl->GetItemText(item, ATTRIBUTE_NAME_COLUMN);

        try
        {
            m_currentAlarmType->setParameterValue( std::string(value), std::string(parameterName) );
            
            AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
            if (detailView != NULL)
            {
                detailView->refreshButtonAvailability();
            }

            return true;
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set the parameter");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Parameter entered was of the wrong type");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << parameterName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030062);
        }

        return false;
    }

}


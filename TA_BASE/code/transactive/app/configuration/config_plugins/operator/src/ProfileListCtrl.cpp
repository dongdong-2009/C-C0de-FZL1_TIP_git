/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/ProfileListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available profiles for the operator
  * in the OperatorDetailView class
  */

#include "app/configuration/config_plugins/operator/src/stdafx.h"
#include "app/configuration/config_plugins/operator/src/ProfileListCtrl.h"
#include "app/configuration/config_plugins/operator/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    // These are the set up details for the attribute list control
    const CString ProfileListCtrl::PROFILE_NAME_COLUMN_NAME   = "½ÇÉ«";
    const int     ProfileListCtrl::PROFILE_NAME_COLUMN_WIDTH  = 200;
    const int     ProfileListCtrl::PROFILE_NAME_COLUMN        = 0;
    const CString ProfileListCtrl::PROFILE_CHECK_COLUMN_NAME  = "ÆôÓÃ";
    const int     ProfileListCtrl::PROFILE_CHECK_COLUMN_WIDTH = 100;
    const int     ProfileListCtrl::PROFILE_CHECK_COLUMN       = 1;

    const std::string ProfileListCtrl::BOOLEAN_TRUE = "Yes";
    const std::string ProfileListCtrl::BOOLEAN_FALSE = "No";

    ProfileListCtrl::ProfileListCtrl() : m_currentOperator(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    ProfileListCtrl::~ProfileListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(ProfileListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(ProfileListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ProfileListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void ProfileListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");		

		m_font.CreatePointFont(90, "ËÎÌå");
		SetFont(&m_font,FALSE);

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(PROFILE_NAME_COLUMN,  PROFILE_NAME_COLUMN_NAME,  LVCFMT_LEFT, PROFILE_NAME_COLUMN_WIDTH);
        InsertColumn(PROFILE_CHECK_COLUMN, PROFILE_CHECK_COLUMN_NAME, LVCFMT_LEFT ,PROFILE_CHECK_COLUMN_WIDTH);

        FUNCTION_EXIT;
    }


    void ProfileListCtrl::populateControl(TA_Base_Core::IConfigOperator* currentOperator, bool isEnabled)
    {
        FUNCTION_ENTRY("populateControl");

        TA_ASSERT(currentOperator != NULL, "IConfigOperator passed was NULL");

        m_currentOperator = currentOperator;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {
            // First retrieve all profiles and add them into the list
            using TA_Base_Core::IProfile;
            std::vector<IProfile*> profiles;
            if (m_currentOperator->isSystemOperator())
            {
                profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllSystemProfiles();
            }
            else
            {
                profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllNonSystemProfiles();
            }

            std::vector<unsigned long> associatedProfiles = m_currentOperator->getAssociatedProfileKeys();

            for (std::vector<IProfile*>::iterator iter = profiles.begin(); iter != profiles.end(); ++iter)
            {
                // Insert the profile name in the first column. Use -1 to ensure we get no icon.
                int position = InsertItem(0,(*iter)->getName().c_str(),-1);
                
                // Set the item data to be the key
                SetItemData(position, (*iter)->getKey());

                // Now see if this profile is associated with the operator
                std::vector<unsigned long>::iterator matching = std::find(associatedProfiles.begin(),associatedProfiles.end(),(*iter)->getKey());
                bool isChecked = false;
                if (matching != associatedProfiles.end() )
                {
                    // This is a match so we can check the box
                    isChecked = true;
                }

                // Now ensure the check box gets displayed and is checked correctly and enabled
                showCheckIcon(position, PROFILE_CHECK_COLUMN, isChecked, isEnabled);

                // We're done with the profile now so delete it
                delete *iter;
                *iter = NULL;
            }

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }


        FUNCTION_EXIT;
    }


    ICellManager::CellType ProfileListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentOperator != NULL,"We do not yet have an IConfigEntity* item set");

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is editable
	    if (subItem != PROFILE_CHECK_COLUMN)
        {
            return CT_NOTEDITABLE;
        }

        if (!IsWindowEnabled())
        {
            return CT_CHECKBOX_DISABLED;
        }
       
	    return CT_CHECKBOX;
    }


    unsigned long ProfileListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


    ICellManager::CellData ProfileListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT (m_currentOperator != NULL,"We do not yet have an IConfigEntity* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

	    ICellManager::CellData data;

	    return data;
    }


    CString ProfileListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char ProfileListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool ProfileListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentOperator != NULL,"We do not yet have an IConfigEntity* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        bool isTicked = true;
        if ( 0 == value.CompareNoCase("no") )
        {
            isTicked = false;
        }

        unsigned long profileKey = listCtrl->GetItemData(item);
        
        try
        {
            m_currentOperator->setAssociatedProfile(profileKey,isTicked);

            AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
            if (detailView != NULL)
            {
                detailView->refreshButtonAvailability();
            }

            return true;
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }

        return false;
    }

}


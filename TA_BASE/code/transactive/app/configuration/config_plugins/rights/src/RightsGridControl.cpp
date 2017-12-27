/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsGridControl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a wrapper around the CGridCtrl (from mfc_extensions) to modify it to display and allow
  * configuration of rights.
  */


#pragma warning(disable:4786)

#include <vector>

#include "app/configuration/config_plugins/rights/src/stdafx.h"
#include "app/configuration/config_plugins/rights/src/RightsGridControl.h"
#include "app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h"
#include "app/configuration/config_plugins/rights/src/RightsComponent.h"
#include "app/configuration/config_plugins/rights/src/RightsView.h"
#include "app/configuration/config_plugins/rights/src/resource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/src/IConfigRights.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/ConfigRights.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;
using TA_Base_Core::RunParams;
using TA_Base_Core::ConfigRights;

namespace TA_Base_App
{
    const int RightsGridControl::LOCAL_PROFILE = 0;
    const int RightsGridControl::CENTRAL_PROFILE = 1;


    RightsGridControl::RightsGridControl() : m_databaseAccessor(NULL), m_subsystemStateKey(0)
    {
    }


    RightsGridControl::~RightsGridControl()
    {
    }


BEGIN_MESSAGE_MAP(RightsGridControl, CGridCtrl)
	//{{AFX_MSG_MAP(RightsGridControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void RightsGridControl::PreSubclassWindow() 
    {
	    SetEditable(TRUE);
        SetDoubleBuffering(TRUE);
	    EnableDragAndDrop(FALSE);

        SetFixedRowCount(1);    // This is the row for subsystems
		SetFixedColumnCount(1); // This is the column of profiles

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control
        SetBkColor( GetSysColor(COLOR_3DFACE) );

        m_profileIcons.Create(SMALL_ICON_SIZE,SMALL_ICON_SIZE,ILC_COLOR32,2,2);
        m_profileIcons.Add( LoadIcon(GetModuleHandle(RightsComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_CENTRAL_PROFILE)) );
        m_profileIcons.Add( LoadIcon(GetModuleHandle(RightsComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_LOCAL_PROFILE)) );

        SetImageList( &m_profileIcons );

        CGridCtrl::PreSubclassWindow();
    }


    void RightsGridControl::loadItems(RightsDatabaseAccessor* dbAccessor,
                                      RightsDatabaseAccessor::RightsType gridType,
                                      CProgressCtrl& progress, unsigned long subsystemStateKey)
    {
        FUNCTION_ENTRY("loadItems");

        TA_ASSERT(dbAccessor != NULL, "Database accessor passed was NULL")
        m_databaseAccessor = dbAccessor;
        m_gridType = gridType;

        setUpHeaders();

        progress.OffsetPos(10);
        m_actionGroups.clear();
        m_actionGroups = m_databaseAccessor->getAllDuties();

        progress.OffsetPos(30);
        populateWithData(subsystemStateKey);
        
        progress.OffsetPos(10);

        // Now that we've set up we have to set the font on the control
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
		SetFont(&arialBold);

        // Now all the data is inserted we should auto-size the control to the right size for the text
	    AutoSize();

        m_subsystemStateKey = subsystemStateKey;

        FUNCTION_EXIT;
    }


    void RightsGridControl::setUpHeaders()
    {
        FUNCTION_ENTRY("setUpHeaders");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");

        // Set up the standard item fields so we don't have to do it each time we insert an item
		GV_ITEM item;
        item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.col = 0;
		item.nFormat = DT_LEFT|DT_WORDBREAK;

        //
        // Set up the profile headings
        //
        std::map<unsigned long, RightsDatabaseAccessor::ProfileDetails> profiles = m_databaseAccessor->getAllProfiles();

        SetRowCount(profiles.size()+1);

        // We ignore the first row (0) because it is the corner cell
        int row = 1;

        for (std::map<unsigned long, RightsDatabaseAccessor::ProfileDetails>::iterator profileIter = profiles.begin();
             profileIter != profiles.end(); ++profileIter)
        {
			item.row = row;
            item.szText = profileIter->second.name.c_str();

			SetItem(&item);

            SetItemData(item.row,item.col,profileIter->first);
            SetItemImage(row,0,profileIter->second.typeKey);

            ++row;
        }

        // Sort the first column so that all the profiles are in alphabetical order.
        SortTextItems(0,TRUE);

        //
        // Set up the subsystem headings
        //
        std::map<std::string, unsigned long> subsystems = m_databaseAccessor->getAllSubsystems(m_gridType);

        SetColumnCount(subsystems.size()+1);

        // We ignore the first column (0) because it is the corner cell
        int column = 1;
        item.row = 0;

        for (std::map<std::string, unsigned long>::iterator subsystemIter = subsystems.begin();
             subsystemIter != subsystems.end(); ++subsystemIter)
        {
			item.col = column;
            item.szText = subsystemIter->first.c_str();

			SetItem(&item);

            SetItemData(item.row,item.col,subsystemIter->second);

            ++column;
        }
        
        FUNCTION_EXIT;             
    }


    void RightsGridControl::populateWithData(unsigned long subsystemStateKey)
    {
        FUNCTION_ENTRY("populateWithData");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");

        static bool first = true;

        resetGrid();

        try
        {
            m_databaseAccessor->loadItems(subsystemStateKey, m_gridType);

            const RightsDatabaseAccessor::LoadedRights& rights = m_databaseAccessor->getRights(m_gridType);

            // Set up the standard item fields so we don't have to do it each time we insert an item
		    GV_ITEM item;
            item.mask = GVIF_TEXT|GVIF_FORMAT;
		    item.nFormat = DT_LEFT|DT_WORDBREAK;

            for ( RightsDatabaseAccessor::LoadedRights::const_iterator iter = rights.begin(); iter != rights.end(); ++iter)
            {
                int matchingRow = 0;
                int matchingCol = 0;

                // First find the matching row (profile) for this right
                unsigned long profileKey = iter->second->getProfileKey();
                for ( int row = 1; row < GetRowCount(); ++row)
                {
                    unsigned long data = GetItemData(row,0);
                    if (data == profileKey)
                    {
                        matchingRow = row;
                        break;
                    }
                }

                // Now find the matching column (subsystem) for this right
                unsigned long subsystemKey = iter->second->getSubsystemKey();
                for ( int col = 1; col < GetColumnCount(); ++col)
                {
                    unsigned long data = GetItemData(0,col);
                    if (data == subsystemKey)
                    {
                        matchingCol = col;
                        break;
                    }
                }

                if (matchingRow != 0 && matchingCol != 0)
                {
                    // We now have a matching cell where we can insert the data
                    item.col = matchingCol;
                    item.row = matchingRow;
                    
                    std::map<unsigned long, std::string>::iterator matching = m_actionGroups.find( iter->second->getActionGroupKey() );
                    if (matching != m_actionGroups.end())
                    {
                        item.szText = matching->second.c_str();
                        SetItem(&item);
                        //SetItemData( matchingRow,matchingCol,iter->first );

                        TA_Base_Core::IConfigRights* castRights = dynamic_cast<TA_Base_Core::IConfigRights*>(iter->second);
                        if( castRights != NULL )
                        {
                            SetItemData( matchingRow,matchingCol,castRights->getUniqueIdentifier() );
                            castRights->setActionGroupKey(matching->first);
                        }
                    }
                }
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


    void RightsGridControl::resetGrid()
    {
        GV_ITEM item;
        item.mask = GVIF_TEXT|GVIF_FORMAT;
        item.nFormat = DT_LEFT|DT_WORDBREAK;

        for (int row = 1; row < GetRowCount(); ++row)
        {
            for ( int col = 1; col < GetColumnCount(); ++col)
            {
                item.szText = "";
                SetItem(&item);
                SetItemData( row,col,0 );
                OnEndEditCell(row, col, "");
            }
        }
    }


    bool RightsGridControl::isConfigurationValid(std::vector<unsigned long>& subsystems)
    {
        FUNCTION_ENTRY("isConfigurationValid");

        // We only need to check configuration for subsystems (not applications)
        if (m_gridType == RightsDatabaseAccessor::APPLICATION)
        {
            return true;
        }

        // For each column (ie subsystem) we must determine if more than one central profile has
        // a value set for it. If more than one does then the configuration is invalid.
        for(int col = 1; col < GetColumnCount(); ++col)
        {
            bool hasSubsystemGotProfile = false;

            for(int row = 1; row < GetRowCount(); ++row)
            {
                // If the cell is not empty and it is for a central profile then we cannot change this one
                if( (!GetItemText(row, col).IsEmpty()) && (GetItemImage(row, 0) == CENTRAL_PROFILE) )
                {
                    if (!hasSubsystemGotProfile)
                    {
                        hasSubsystemGotProfile = true;
                    }
                    else
                    {
                        // This means we've already got a profile for this subsystem so this is an error
                        subsystems.push_back( GetItemData(0, col) );
                        break;
                    }
                }
            }
        }

        if (!subsystems.empty())
        {
            FUNCTION_EXIT;
            return false;
        }

        return true;
        FUNCTION_EXIT;
    }


    void RightsGridControl::OnEndEditCell(int nRow, int nCol, CString str)
    {
        FUNCTION_ENTRY("OnEndEditCell");

        try
        {
            unsigned long uniqueId = GetItemData(nRow,nCol);

            TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");

            TA_Base_Core::IConfigRights* item = m_databaseAccessor->getItem( uniqueId );
            if (item == NULL)
            {
                if (str.IsEmpty())
                {
                    // This means there was not existing item but the user re-set it to blank so we don't need to
                    // do anything here
                    CGridCtrl::OnEndEditCell(nRow, nCol, str);

                    FUNCTION_EXIT;
                    return;
                }

                // We have no matching item so we need to create a new one
                unsigned long profileKey = GetItemData(nRow,0);
                unsigned long subsystemKey = GetItemData(0,nCol);
				unsigned long subsystemStateKey = m_subsystemStateKey;


                item = m_databaseAccessor->newItem(profileKey, subsystemKey, subsystemStateKey, m_gridType);

                // Now set the data for this new unique identifier we've been given
                SetItemData(nRow,nCol, item->getUniqueIdentifier());
            }


            long key = TA_Base_Core::ConfigRights::INVALID_ACTIONGROUP;

            // If the value entered is blank then this means the user does not want a duty type for this entry
            // So if it is not empty/blank then we need to determine the key to use
            if (!str.IsEmpty())
            {
                // We must find the key from the name we have got
                for( std::map<unsigned long, std::string>::iterator iter = m_actionGroups.begin(); iter != m_actionGroups.end(); ++iter)
                {
                    if ( str == iter->second.c_str() )
                    {
                        key = iter->first;
                        break;
                    }
                }
            }

            // Now set the key
            item->setActionGroupKey( key );

            if (item->hasChanged())
            {
                RightsView* mainView = dynamic_cast<RightsView*>(GetParent());
                if (mainView != NULL)
                {
                    // We must now enable the reset and apply button since the user has made a change
                    mainView->refreshButtonAvailability(true);
                }
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

        CGridCtrl::OnEndEditCell(nRow, nCol, str);

        FUNCTION_EXIT;
    }


    void RightsGridControl::CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar)
    {
        CStringArray duties;
/* This comment is removing the code that enforces that each subsystem only has a duty set for one central
   profile. If this is reversed this should be added back in. The removal relates to PW2621.

        // If the grid type is SUBSYSTEM we must check that we don't allow a subsystem to have rights
        // set twice for a central profile
        if (m_gridType == RightsDatabaseAccessor::SUBSYSTEM)
        {
            // The image stored for each row will tell us which are the central profiles
            if( GetItemImage(nRow, 0) == CENTRAL_PROFILE)
            {
                for(int row = 0; row < GetRowCount(); ++row)
                {
                    if (row == nRow)
                    {
                        // If the row we are looking at is this row then ignore
                        continue;
                    }

                    // If the cell is not empty and it is for a central profile then we cannot change this one
                    if( (!GetItemText(row, nCol).IsEmpty()) && (GetItemImage(row, 0) == CENTRAL_PROFILE) )
                    {
                        // Don't add any duties to the array as the user cannot select anything

	                    // InPlaceList and auto-deletes itself
	                    new CInPlaceList(this, rect,
                                         CBS_DROPDOWNLIST,
                                         nID,                   // ID of control being created
                                         nRow, nCol, 
                                         duties, szText, nChar);

                        return;
                    }
                }
            }
        }
*/
        // All duties are available so add them all
        // Add a blank entry as the user does not have to specify a duty
        duties.Add(""); 

        for( std::map<unsigned long, std::string>::iterator iter = m_actionGroups.begin(); iter != m_actionGroups.end(); ++iter)
        {
            duties.Add(iter->second.c_str());
        }

	    // InPlaceList and auto-deletes itself
	    new CInPlaceList(this, rect,
                         CBS_DROPDOWNLIST,
                         nID,                   // ID of control being created
                         nRow, nCol, 
                         duties, szText, nChar);
    }

}

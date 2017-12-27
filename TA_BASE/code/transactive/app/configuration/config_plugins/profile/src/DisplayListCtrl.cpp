/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/DisplayListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available displays for the profile
  * in the ProfileDetailView class
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile/src/stdafx.h"
#include "app/configuration/config_plugins/profile/src/DisplayListCtrl.h"
#include "app/configuration/config_plugins/profile/src/ProfileComponent.h"
#include "app/configuration/config_plugins/profile/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/profile/src/ProfileDatabaseAccessor.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigProfile.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConfigEntity.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include <algorithm>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

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
    const CString DisplayListCtrl::LOCATION_AVAILABLE_COLUMN_NAME = "ÆôÓÃ";
    const int     DisplayListCtrl::LOCATION_AVAILABLE_COLUMN_WIDTH = 80;
    const int     DisplayListCtrl::LOCATION_AVAILABLE_COLUMN = 0;
    const CString DisplayListCtrl::LOCATION_NAME_COLUMN_NAME = "Î»ÖÃ";
    const int     DisplayListCtrl::LOCATION_NAME_COLUMN_WIDTH = 120; // If this is changed then the mandatory \ on the display 1 column must be moved manually in the rc file.
    const int     DisplayListCtrl::LOCATION_NAME_COLUMN = 1;

    const CString     DisplayListCtrl::DISPLAY1_COLUMN_NAME = "ÏÔÊ¾Æ÷ 1";
    const CString     DisplayListCtrl::DISPLAY2_COLUMN_NAME = "ÏÔÊ¾Æ÷ 2";
    const CString     DisplayListCtrl::DISPLAY3_COLUMN_NAME = "ÏÔÊ¾Æ÷ 3";

    const std::string DisplayListCtrl::BOOLEAN_TRUE = "Yes";
    const std::string DisplayListCtrl::BOOLEAN_FALSE = "No";

    const int         DisplayListCtrl::DISPLAY_COLUMN_WIDTH = 120;

    const CString     DisplayListCtrl::LEFT_BANNER_COLUMN_NAME = "×ó²àºá·ù"; 
    const CString     DisplayListCtrl::CENTRE_BANNER_COLUMN_NAME = "ÖÐ¼äºá·ù";
    const CString     DisplayListCtrl::RIGHT_BANNER_COLUMN_NAME = "ÓÒ²àºá·ù";

    DisplayListCtrl::DisplayListCtrl() 
        : m_currentProfile(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    DisplayListCtrl::~DisplayListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(DisplayListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(DisplayListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void DisplayListCtrl::PreSubclassWindow() 
    {
	    setupList();
		addBitmapToHeader();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void DisplayListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor has not been set");

        m_font.CreatePointFont(90,"ËÎÌå");
        SetFont(&m_font,FALSE);

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(LOCATION_AVAILABLE_COLUMN,LOCATION_AVAILABLE_COLUMN_NAME,LVCFMT_LEFT,LOCATION_AVAILABLE_COLUMN_WIDTH);
        InsertColumn(LOCATION_NAME_COLUMN,  LOCATION_NAME_COLUMN_NAME,  LVCFMT_LEFT, LOCATION_NAME_COLUMN_WIDTH);

        // There are certain parameters that we want displayed in a specific order. These are:
        //
        // Display 1
        // Display 2
        // Display 3
        // LeftBannerDefaultPage
        // CentreBannerDefaultPage
        // RightBanerDefaultPage
        //
        // So hardcode this order and then the rest (if there are any) can be displayed in alphabetic order.
        std::vector<std::string> tempNames;
        try
        {
            tempNames = m_databaseAccessor->getAllDistinctParameterNames();
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve parameter names");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
            return;
        }

        // The following lines add the parameter to the list and remove it from the temp store (tempNames)
        setUpKnownParameter(tempNames, LPCSTR("Display 1"));
        setUpKnownParameter(tempNames, LPCSTR("Display 2"));
        setUpKnownParameter(tempNames, LPCSTR("Display 3"));
        setUpKnownParameter(tempNames, LPCSTR("LeftBannerDefaultPage"));
        setUpKnownParameter(tempNames, LPCSTR("CentreBannerDefaultPage"));
        setUpKnownParameter(tempNames, LPCSTR("RightBannerDefaultPage"));

        // tempNames will now contain the rest of the parameters (if there are any). As it does not matter
        // which order these are displayed, just append this vector.
        m_parameterNames.insert(m_parameterNames.end(), tempNames.begin(), tempNames.end());

        std::vector<std::string> columnNames = toChinese(m_parameterNames);

        // Now the parameters are in the order we want them. Add them to the list.
        for (size_t i = 0, count = LOCATION_NAME_COLUMN + 1; i < columnNames.size(); ++i, count++)
        {
            InsertColumn(count, columnNames[i].c_str(),  LVCFMT_LEFT, DISPLAY_COLUMN_WIDTH);
        }

        FUNCTION_EXIT;
    }


    void DisplayListCtrl::populateControl(TA_Base_Core::IConfigProfile* currentProfile)
    {
        FUNCTION_ENTRY("populateControl");

        TA_ASSERT(currentProfile != NULL, "IConfigProfile passed was NULL");

        m_currentProfile = currentProfile;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {
            // First retrieve all locations and add them into the list
            using TA_Base_Core::ILocation;
            std::vector<ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

            for (std::vector<ILocation*>::iterator iter = locations.begin(); iter != locations.end(); ++iter)
            {
                if ( !m_currentProfile->requiresLocationToBeSelected() && (*iter)->getKey() != 0)
                {
                    // Do not add this to the list as we only want the unallocated location
                    // We only use the unallocated location when the profile is a central one.
                    delete *iter;
                    *iter = NULL;
                    continue;
                }
                else if (m_currentProfile->requiresLocationToBeSelected() && (*iter)->getKey() == 0)
                {
                    // Do not add this to the list as we don't want the unallocated location
                    // to be seen for local profiles.
                    delete *iter;
                    *iter = NULL;
                    continue;
                }

                // Insert the item
                int position = InsertItem(-1,(*iter)->getName().c_str(), -1);

                // Put the check-box in the first column
                showCheckIcon(position,LOCATION_AVAILABLE_COLUMN,
                    m_currentProfile->isLocationAssociated((*iter)->getKey()),true);

                // Put the name into the second position in the table
                SetItemText(position,LOCATION_NAME_COLUMN,(*iter)->getName().c_str());
                
                // Set the item data to be the key
                SetItemData(position, (*iter)->getKey());

                // Retrieve all the displays
                int count = LOCATION_NAME_COLUMN + 1;
                std::vector<std::string>::iterator namesIter = m_parameterNames.begin();
                for (; namesIter != m_parameterNames.end(); ++namesIter)
                {
                    std::string displayName = "";

                    if(*namesIter == LPCSTR(LEFT_BANNER_COLUMN_NAME) || *namesIter == LPCSTR(CENTRE_BANNER_COLUMN_NAME) ||
                        *namesIter == LPCSTR(RIGHT_BANNER_COLUMN_NAME))
                    {
                        displayName = m_currentProfile->getParameterActualValue((*iter)->getKey(), *namesIter);
                    }
                    else
                    {
                        try
                        {
                            unsigned long displayKey = strtoul(m_currentProfile->getParameterActualValue((*iter)->getKey(), *namesIter).c_str(), NULL, 10);

                            TA_Base_Core::IScadaDisplay* display = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getScadaDisplay(displayKey);
                            displayName = display->getName();
                            delete display;
                            display = NULL;
                        }
                        catch ( const TA_Base_Core::DataException& exception )
                        {
                            if (exception.getFailType() != TA_Base_Core::DataException::NO_VALUE)
                            {
                                // If this is not a NO_VALUE exception then this is actually an error
                                throw;
                            }
                            // else the displayName will be set to an empty string which is correct
                        }
                    }

                    SetItemText(position,count,displayName.c_str());
                    ++count;
                }
               
                // We're done with the location now so delete it
                delete *iter;
                *iter = NULL;
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot load and retrieve and full list of locations");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot load and retrieve and full list of locations");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        

        FUNCTION_EXIT;
    }


    ICellManager::CellType DisplayListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentProfile != NULL,"We do not yet have an IConfigEntity* item set");

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is non-editable. This is the second column
	    if (subItem == LOCATION_NAME_COLUMN)
        {
            return CT_NOTEDITABLE;
        } // While the available column is a checkbox.
        else if (subItem == LOCATION_AVAILABLE_COLUMN)
        {
            return CT_CHECKBOX;
        }

	    return CT_SELECTION;
    }


    unsigned long DisplayListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


    ICellManager::CellData DisplayListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT (m_currentProfile != NULL,"We do not yet have an IConfigEntity* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        CWaitCursor waitCursor;     //  TD 10960
	    ICellManager::CellData data;

        HDITEM hdi;
        TCHAR  lpBuffer[256];
        
        hdi.mask = HDI_TEXT;
        hdi.pszText = lpBuffer;
        hdi.cchTextMax = 256;

        GetHeaderCtrl()->GetItem(subItem, &hdi);

        // Add a blank entry so the user can remove a display setting. This is only available for the display 2
        // and 3 columns. Display 1 is mandatory.
        
        // Display the SCADA displays if it is a Display column
        if(hdi.pszText == DISPLAY1_COLUMN_NAME || hdi.pszText == DISPLAY2_COLUMN_NAME || hdi.pszText == DISPLAY3_COLUMN_NAME)
        {
            using TA_Base_Core::IScadaDisplay;

            if(hdi.pszText != DISPLAY1_COLUMN_NAME)
            {
                data.push_back("");
            }

            // Now load all the possible displays
            std::vector<IScadaDisplay*> displays = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getAllScadaDisplays();

            for ( std::vector<IScadaDisplay*>::iterator iter = displays.begin(); iter != displays.end(); ++iter)
            {
                try
                {
                    // Add each of these to the list. If an error occurs and one cannot be retrieved then it will
                    // not be available in the list.
                    data.push_back( (*iter)->getName() );

                    // Now we're done with this so delete it
                    delete (*iter);
                    *iter = NULL;
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the Scada Display name");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the Scada Display name");
                }
            }
        }
        else if(hdi.pszText == LEFT_BANNER_COLUMN_NAME || hdi.pszText == CENTRE_BANNER_COLUMN_NAME || hdi.pszText == RIGHT_BANNER_COLUMN_NAME)
        {
			// TD #12987
			// need to be able to set centre/right banners back to blank, since they aren't mandatory
            if(hdi.pszText != LEFT_BANNER_COLUMN_NAME)
            {
                data.push_back("");
            }

            TA_Base_Core::ConfigEntity configEntity("Banner");
            std::map<std::string,std::string> parameterValues = configEntity.getPossibleEnumerationValues("StartTab");
            std::map<std::string,std::string>::iterator iter = parameterValues.begin();
            for(; iter != parameterValues.end(); ++iter)
            {
                data.push_back(iter->first);
            }
        }

 
	    return data;
    }


    CString DisplayListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char DisplayListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool DisplayListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentProfile != NULL,"We do not yet have an IConfigProfile* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        HDITEM hdi;
        TCHAR  lpBuffer[256];

        hdi.mask = HDI_TEXT;
        hdi.pszText = lpBuffer;
        hdi.cchTextMax = 256;

        GetHeaderCtrl()->GetItem(subItem, &hdi);

        unsigned long locationKey = listCtrl->GetItemData(item);

        // If an available checkbox has been checked, then that is always okay, so just update and return
        if (subItem == LOCATION_AVAILABLE_COLUMN)
        {
            try
            {
                // At this point the status of the check-box has yet to be updated, so need to invert the result
                m_currentProfile->setLocationAssociationStatus(locationKey,!isItemChecked(item,subItem));

                AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
                if (detailView != NULL)
                {
                    detailView->refreshButtonAvailability();
                }
                return true;
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set the association status");
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not set the association status");
            }

            return false;
        }

		// TD #12987
		// the removeParameter function has been made redundant with the profile parameter table being restructured.
		// instead, the parameter will just be reset to 0/blank

        // If the value is empty then this display needs to be removed
        /*if (value.IsEmpty())
        {
            try
            {
                m_currentProfile->removeParameter(locationKey,hdi.pszText);

                AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
                if (detailView != NULL)
                {
                    detailView->refreshButtonAvailability();
                }

                return true;
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set the location display");
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not set the location display");
            }

            return false;
        }*/

        using TA_Base_Core::IScadaDisplay;
        unsigned long displayKey = 0;

        try
        {
            if(hdi.pszText == DISPLAY1_COLUMN_NAME || hdi.pszText == DISPLAY2_COLUMN_NAME || hdi.pszText == DISPLAY3_COLUMN_NAME)
            {
				// TD #12987
				// if value is empty, the blank list item was chosen, so leave displayKey as 0
				if(!value.IsEmpty())
				{
					// Get the display key of the new value
					IScadaDisplay* display = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getScadaDisplay(std::string(value));
					displayKey = display->getKey();
				}

				// Convert the key to a string
				std::ostringstream str;
				str << displayKey;
                m_currentProfile->setParameterValue(locationKey, toEnglish(hdi.pszText), str.str());
            }
            else
            {
                m_currentProfile->setParameterValue(locationKey, toEnglish(hdi.pszText), std::string(value));
            }

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

	
	void DisplayListCtrl::addBitmapToHeader()
    {
		// Bitmap for "DISPLAY 1" column
        HDITEM HeaderItem; 
	    HeaderItem.mask = HDI_FORMAT | HDI_BITMAP; 
	    CHeaderCtrl* HeaderCtrl = GetHeaderCtrl(); 
	    HeaderCtrl->GetItem(LOCATION_NAME_COLUMN + 1, &HeaderItem); 
	    if (HeaderItem.hbm != 0) 
	    { 
		    DeleteObject(HeaderItem.hbm); 
		    HeaderItem.hbm = 0; 
	    } 
	    
	    HeaderItem.fmt |= HDF_BITMAP; 

	    // Load the required image
		HeaderItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(ProfileComponent::DLL_NAME), 
			MAKEINTRESOURCE(IDB_BITMAP_MANDATORY), IMAGE_BITMAP, 0, 0, 
			LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS); 

	    // Setup the header with the image
	    HeaderCtrl->SetItem(LOCATION_NAME_COLUMN + 1, &HeaderItem); 


		// TD #12987
		// Left Banner is mandatory too, so add the bitmap to its column

		// Bitmap for "LEFT BANNER" column
	    HeaderCtrl->GetItem(LOCATION_NAME_COLUMN + 4, &HeaderItem); 
	    if (HeaderItem.hbm != 0) 
	    { 
		    DeleteObject(HeaderItem.hbm); 
		    HeaderItem.hbm = 0; 
	    } 
	    
	    HeaderItem.fmt |= HDF_BITMAP; 

	    // Load the required image
		HeaderItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(ProfileComponent::DLL_NAME), 
			MAKEINTRESOURCE(IDB_BITMAP_MANDATORY), IMAGE_BITMAP, 0, 0, 
			LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS); 

	    // Setup the header with the image
	    HeaderCtrl->SetItem(LOCATION_NAME_COLUMN + 4, &HeaderItem); 
    }


    void DisplayListCtrl::setUpKnownParameter(std::vector<std::string>& parameterList, const std::string& parameterName)
    {
        std::vector<std::string>::iterator iter = std::find(parameterList.begin(), parameterList.end(), parameterName);
        if ( iter != parameterList.end() )
        {
            m_parameterNames.push_back(*iter);
            parameterList.erase(iter);
        }
    }

    std::string DisplayListCtrl::toChinese(const std::string& s)
    {
        static std::map<std::string, std::string> map = boost::assign::map_list_of
            ("Display 1", DISPLAY1_COLUMN_NAME)
            ("Display 2", DISPLAY2_COLUMN_NAME)
            ("Display 3", DISPLAY3_COLUMN_NAME)
            ("LeftBannerDefaultPage", LEFT_BANNER_COLUMN_NAME)
            ("CentreBannerDefaultPage", CENTRE_BANNER_COLUMN_NAME)
            ("RightBannerDefaultPage", RIGHT_BANNER_COLUMN_NAME)
            ;

        return translate(s, map);
    }

    std::string DisplayListCtrl::toEnglish(const std::string& s)
    {
        static std::map<std::string, std::string> map = boost::assign::map_list_of
            (DISPLAY1_COLUMN_NAME, "Display 1")
            (DISPLAY2_COLUMN_NAME, "Display 2")
            (DISPLAY3_COLUMN_NAME, "Display 3")
            (LEFT_BANNER_COLUMN_NAME, "LeftBannerDefaultPage")
            (CENTRE_BANNER_COLUMN_NAME, "CentreBannerDefaultPage")
            (RIGHT_BANNER_COLUMN_NAME, "RightBannerDefaultPage")
            ;

        return translate(s, map);
    }

    std::string DisplayListCtrl::translate(const std::string& s, const std::map<std::string, std::string>& map)
    {
        std::map<std::string, std::string>::const_iterator it = map.find(s);

        if (it != map.end())
        {
            return it->second;
        }

        return s;
    }

    std::vector<std::string> DisplayListCtrl::toChinese(const std::vector<std::string>& vs)
    {
        std::vector<std::string> result(vs);

        for (size_t i = 0; i < vs.size(); ++i)
        {
            result[i] = toChinese(vs[i]);
        }

        return result;
    }

    std::vector<std::string> DisplayListCtrl::toEnglish(const std::vector<std::string>& vs)
    {
        std::vector<std::string> result(vs);

        for (size_t i = 0; i < vs.size(); ++i)
        {
            result[i] = toEnglish(vs[i]);
        }

        return result;
    }

}

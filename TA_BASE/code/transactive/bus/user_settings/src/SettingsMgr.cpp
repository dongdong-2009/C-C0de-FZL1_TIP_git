/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/user_settings/src/SettingsMgr.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * <description>
  *
  */
#pragma warning(disable:4786)

#include "bus\user_settings\src\SettingsMgr.h"
#include "core\data_access_interface\user_settings\src\IUserSettings.h"
#include "core\data_access_interface\user_settings\src\UserSettingsAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\exceptions\src\DataUserSettingException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataUserSettingException.h"
#include "core\exceptions\src\UserSettingsException.h"

#include <string>
#include <sstream>
#include <vector>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::UserSettingsAccessFactory;
using TA_Base_Core::DataUserSettingException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::UserSettingsException;

namespace TA_Base_Bus
{
    const std::string SettingsMgr::PLACEMENT_SETTING = "WINDOWPLACEMENT_SETTING";

    SettingsMgr::SettingsMgr(std::string sessionId, const unsigned long applicationType)
         : m_settings(NULL)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "Constructor");
        //Get all the settings from the database for this user and application
        m_settings = UserSettingsAccessFactory::getInstance().getUserSettings(sessionId,applicationType);
        LOG( SourceInfo, DebugUtil::FunctionExit, "Constructor");
    }


    SettingsMgr::~SettingsMgr()
    {
        try
        {
            if(m_settings != NULL)
            {
	            delete m_settings;
                m_settings = NULL;
            }
        }
        catch( ... )
        {
        }
    }


    unsigned long SettingsMgr::getSettingAsLong(const std::string& settingName)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingAsLong");

        unsigned long value;
        try
        {
            value = m_settings->getSettingAsUnsignedLong(settingName);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingAsLong");
        return value;
    }


    std::string SettingsMgr::getSettingAsString(const std::string& settingName)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingAsString");

        std::string value;
        try
        {
            value = m_settings->getSettingAsString(settingName);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingAsString");
        return value;
    }


    std::vector<std::string> SettingsMgr::getSettingsLike(const std::string& settingName)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingsLike");

        std::vector<std::string> value;
        try
        {
            value = m_settings->getSettingsLike(settingName);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingsLike");
        return value;
    }


    void SettingsMgr::saveSetting(const std::string& settingName, const unsigned long value) 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "saveSetting");

        try
        {
            m_settings->setSetting(settingName,value);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& ex )
        {
            if ( ex.getFailType() != DataUserSettingException::SETTING_NOT_FOUND )
            {
                // This means that an error did actually occur so we will deal with it here.
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
            // otherwise if the exception failure was SETTING_NOT_FOUND then we want to try 
            // adding the setting instead of updating it.
            try
            {
                m_settings->addSetting(settingName, value);
            }
            catch ( DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Error with the database somehow") );
            }
            catch ( DataUserSettingException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "saveSetting");
    }


    void SettingsMgr::saveSetting(const std::string& settingName, std::string& value) 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "saveSetting");

        try
        {
            m_settings->setSetting(settingName,value);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& ex )
        {
            if ( ex.getFailType() != DataUserSettingException::SETTING_NOT_FOUND )
            {
                // This means that an error did actually occur so we will deal with it here.
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
            // otherwise if the exception failure was SETTING_NOT_FOUND then we want to try 
            // adding the setting instead of updating it.
            try
            {
                m_settings->addSetting(settingName, value);
            }
            catch ( DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Error with the database somehow") );
            }
            catch ( DataUserSettingException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "saveSetting");
    }


    void SettingsMgr::deleteSetting(const std::string& settingName) 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "deleteSetting");

        try
        {
            m_settings->deleteSetting(settingName);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "deleteSetting");
    }


    void SettingsMgr::restoreWindowPlacement(CWnd* const windowPtr)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "restoreWindowPlacement");

        if( windowPtr == NULL )
        {
            TA_THROW( UserSettingsException("The window pointer passed was NULL") );            
        }
        if( windowPtr->m_hWnd == NULL )
        {
            TA_THROW( UserSettingsException("The window handle passed was NULL") );
        }

        std::string placementStr;
        try
        {
            placementStr = m_settings->getSettingAsString(PLACEMENT_SETTING);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
        }

        // Convert the string into a Windows placement
        WINDOWPLACEMENT placement = str2Placement(placementStr);

        // Now pass this placement to the Window so it can resize/move.
        if( !windowPtr->SetWindowPlacement(&placement) )
        {
            TA_THROW( UserSettingsException("Placement of the window failed and returned false") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "restoreWindowPlacement");
    }


    void SettingsMgr::saveWindowPlacement(const CWnd* const windowPtr) 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "saveWindowPlacement");

        if( windowPtr == NULL )
        {
            TA_THROW( UserSettingsException("The window pointer passed was NULL") );            
        }
        if( windowPtr->m_hWnd == NULL )
        {
            TA_THROW( UserSettingsException("The window handle passed was NULL") );
        }

        //Get the current placement of the window passed
        WINDOWPLACEMENT placement;
        if( ! (windowPtr->GetWindowPlacement(&placement)))
        {
            TA_THROW( UserSettingsException("Retrieving of the window placement returned false") );
        }

        //Serialize a WINDOWPLACEMENT struct into a string
        std::string placementStr = placement2Str(placement);

        try
        {
            //Save the WINDOWPLACEMENT_SETTING
            m_settings->setSetting(PLACEMENT_SETTING,placementStr);
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
            TA_THROW( UserSettingsException("Error with the database somehow") );
        }
        catch ( DataUserSettingException& ex )
        {
            if ( ex.getFailType() != DataUserSettingException::SETTING_NOT_FOUND )
            {
                // This means that an error did actually occur so we will deal with it here.
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
            // otherwise if the exception failure was SETTING_NOT_FOUND then we want to try 
            // adding the setting instead of updating it.
            try
            {
                m_settings->addSetting(PLACEMENT_SETTING, placementStr);
            }
            catch ( DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Error with the database somehow") );
            }
            catch ( DataUserSettingException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
                TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "saveWindowPlacement");
    }


    std::string SettingsMgr::placement2Str(const WINDOWPLACEMENT& placement)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "placement2Str");

        //Get the data from the placement structure
        UINT  length = placement.length;
        UINT  flags = placement.flags;
        UINT  showCmd = placement.showCmd;
        //POINT ptMinPosition
        LONG  ptMinPositionX = placement.ptMinPosition.x;
        LONG  ptMinPositionY = placement.ptMinPosition.y;
        //POINT ptMaxPosition
        LONG  ptMaxPositionX = placement.ptMaxPosition.x;
        LONG  ptMaxPositionY = placement.ptMaxPosition.y;
        //RECT  rcNormalPosition	
        LONG left = placement.rcNormalPosition.left;
        LONG top = placement.rcNormalPosition.top;
        LONG right = placement.rcNormalPosition.right;
        LONG bottom = placement.rcNormalPosition.bottom;

        std::ostringstream oss;
        oss << length << "\\";
        oss << flags << "\\";
        oss << showCmd << "\\";
        oss << ptMinPositionX << "\\";
        oss << ptMinPositionY << "\\";
        oss << ptMaxPositionX << "\\";
        oss << ptMaxPositionY << "\\";
        oss << left << "\\";
        oss << top << "\\";
        oss << right << "\\";
        oss << bottom << "\\";
        std::string placementStr = oss.str();

        //verify that the conversion is OK
        WINDOWPLACEMENT placement2 = str2Placement(placementStr);

        if( !(
	        (length == placement2.length) &&
	        (flags == placement2.flags) &&
	        (showCmd == placement2.showCmd) &&
	        (ptMinPositionX == placement2.ptMinPosition.x) &&
	        (ptMinPositionY == placement2.ptMinPosition.y) &&
	        (ptMaxPositionX == placement2.ptMaxPosition.x) &&
	        (ptMaxPositionY == placement2.ptMaxPosition.y) &&
	        (left == placement2.rcNormalPosition.left) &&
	        (top == placement2.rcNormalPosition.top) &&
	        (right == placement2.rcNormalPosition.right) &&
	        (bottom == placement2.rcNormalPosition.bottom)
        ) )
        {
            TA_THROW( UserSettingsException("Error during WINDOWPLACEMENT conversion to a string") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "placement2Str");
        return placementStr;	
    }


    WINDOWPLACEMENT SettingsMgr::str2Placement(const std::string& placeStr)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "str2Placement");

        //Break the string up into a vector of strings
        std::vector<std::string> valueStrings;

        std::string::size_type start = 0;
        std::string::size_type end;
        do
        {
	        end = placeStr.find('\\', start);
	        if(end != std::string::npos)
	        {
		        std::string thisValue(&placeStr[start], &placeStr[end]);
		        valueStrings.push_back(thisValue);
	        }
	        start = end + 1;
        }while(end != std::string::npos);

        //Verify that you have a value for each member (and member's member's)
        if(11 != valueStrings.size())
        {
                TA_THROW( UserSettingsException("Could not convert std::string to WINDOWPLACEMENT struct") );
        }

        //Write the values into the WINDOWPLACEMENT structure
        int index = 0;
        WINDOWPLACEMENT placement;
        placement.length = atoi((valueStrings[index++]).c_str());
        placement.flags = atoi((valueStrings[index++]).c_str());
        placement.showCmd = atoi((valueStrings[index++]).c_str());
        placement.ptMinPosition.x = atol((valueStrings[index++]).c_str());
        placement.ptMinPosition.y = atol((valueStrings[index++]).c_str());
        placement.ptMaxPosition.x = atol((valueStrings[index++]).c_str());
        placement.ptMaxPosition.y = atol((valueStrings[index++]).c_str());
        placement.rcNormalPosition.left = atol((valueStrings[index++]).c_str());
        placement.rcNormalPosition.top = atol((valueStrings[index++]).c_str());
        placement.rcNormalPosition.right = atol((valueStrings[index++]).c_str());
        placement.rcNormalPosition.bottom = atol((valueStrings[index++]).c_str());

        LOG( SourceInfo, DebugUtil::FunctionExit, "str2Placement");
        return placement;
    }
} // end of namespace TA_Base_Bus

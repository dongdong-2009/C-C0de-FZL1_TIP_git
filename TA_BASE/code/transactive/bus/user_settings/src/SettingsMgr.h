/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/user_settings/src/SettingsMgr.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class provides methods to create, set, and get User settings so that user
  * settings can be restored from one instance of the managed process to the next. Methods are
  * also provided to restore a CWnd's placement (size and position) from one instance to the next.
  * 
  * NOTE: An object of this class can only be instantiated in Gen Gui based application
  * 
  * The settings that are set and retrieved by this class are for a given user / app type pair.
  * The user is defined as the user logged on to the Control Station when this class is instantiated.
  * 
  * Settings for a given user/application should NOT be configured in the database prior to execution.
  * Instead, this class should be used to create user settings by calling one of the overloaded 
  * SaveSetting() methods. 
  * Further calls to the SaveSetting() methods will update the value of the setting if the setting
  * all ready exists.
  * 
  */

#if !defined(AFX_SETTINGSMGR_H__7F6204C5_192D_11D6_B1B3_0050BAB0C99D__INCLUDED_)
#define AFX_SETTINGSMGR_H__7F6204C5_192D_11D6_B1B3_0050BAB0C99D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\user_settings\src\stdafx.h"
#include <string>
#include <map>
#include <vector>

namespace TA_Base_Core
{
    class IUserSettings;
}

namespace TA_Base_Bus
{
    class SettingsMgr  
    {

    public:

        /*
         * Constructor 
         *
         * @param string - The current sessionId of the user
         * @param unsigned long - The applicationType key of the application.
         */
        SettingsMgr(const std::string sessionId, const unsigned long applicationType);

        /*
         * Destructor 
         */
        virtual ~SettingsMgr();

        /* getSettingsAsLong
         *
         * Use this method to return the value of a setting if the setting value is type unsigned long.
         *
         * @param string settingName - The name of the setting to retrieve
         *
         * @return unsigned long - The value of the setting as an unsigned long
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        unsigned long getSettingAsLong(const std::string& settingName);

        /* getSettingAsString
         *
         * Use this method to return the value of a setting if the setting value is type std::string.
         *
         * @param string settingName - The name of the setting to retrieve
         *
         * @return string - The value of the setting as a string
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        std::string getSettingAsString(const std::string& settingName);

        /** 
          * getSettingsLike
          *
          * Gets all settings prefixed with the specified string in the lexical order.
          * (Make sure strings like 'Setting2' are named 'Setting02' instead if it is to
          * come before 'Setting11'.
          *
          * @param prefix The prefix of the setting name
          *
          * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
          */
        std::vector<std::string> getSettingsLike(const std::string& prefix);

        /*
         * saveSetting
         *
         * Save the setting named SettingName that has a value of type long.
         * If the setting can not be found this method will create a new setting with the name and
         * value specified. 
         * NOTE: The settingName "WINDOWPLACEMENT_SETTING" should not be used, it is used internally.
         *
         * @param string settingName - The setting to save
         * @param unsigned long value - The value to save for the setting
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        void saveSetting(const std::string& settingName, const unsigned long value);

        /*
         * saveSetting
         *
         * Save the setting named SettingName that has a value of type string.
         * If the setting can not be found this method will create a new setting with the name and
         * value specified. 
         * NOTE: The settingName "WINDOWPLACEMENT_SETTING" should not be used, it is used internally.
         *
         * @param string settingName - The setting to save
         * @param string value - The value to save for the setting
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        void saveSetting(const std::string& settingName, std::string& value);

        /** 
          * deleteSetting
          *
          * Deletes the setting with the specified name
          *
          * @param name The name of the setting to delete
          *
          * @exception DatabaseException - This is thrown if there is some error with the database
          *                                connection or an SQL statement failed to execute for
          *                                some reason.
          * @exception DataUserSettingsException - This is thrown if the user setting data was
          *                                        invalid in some way.
          */
        void deleteSetting(const std::string& name);

        /*
         * restoreWindowPlacement
         *
         * This method will restore the placement of windowPtr (via CWnd::SetWindowPlacement()) 
         * to the placement last saved with the SaveWindowPlacement method.
         *
         * @param CWnd* windowPtr - This is the pointer to the window to be restored.
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        void restoreWindowPlacement(CWnd* const windowPtr);

        /*
         * saveWindowPlacement
         *
         * This method will save the placement of windowPtr (retrieved via CWnd::GetWindowPlacement()) 
         * to the database. See RestoreWindowPlacement() for why you might want to do this.
         * Note that if you call this on more than one CWnd only the last call will be saved. In other
         * words, this method is only useful for saving the placement of only one window per application
         *
         * @param CWnd* windowPtr - This is the pointer to the window with the settings to be saved.
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        void saveWindowPlacement(const CWnd* const windowPtr);

        /*
         * Str2Placement
         *
         * Convert a string into a WINDOWPLACEMENT struct. The string contains each value as a string
         * separated by a back slash - ie 423\537...
         *
         * This has been made a public member as some applications require parts of the WINDOWPLACEMENT
         * structure and not all of it. This allows the applications to use the generic method in this
         * class rather than writing their own. Applications needing this are more probably document/view
         * applications.
         *
         * @param string - This takes a string in the appropriate format to be converted to a
         *                 WINDOWPLACEMENT.
         *
         * @return WINDOWPLACEMENT - A WINDOWPLACEMENT structure which can be used to position a
         *                           window
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        WINDOWPLACEMENT str2Placement(const std::string& placeStr);

        //
        // This holds the string representing the window placement setting name.
        static const std::string PLACEMENT_SETTING;

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        SettingsMgr& operator=(const SettingsMgr &) { };
        SettingsMgr( const SettingsMgr& ) {};

        /**
         * placement2Str
         * Convert a WINDOWPLACEMENT struct into a string by writing each value as a string
         * separated by a back slash - ie 423\537...
         *
         * @param WINDOWPLACEMENT - A WINDOWPLACEMENT structure which holds position settings
         *                          for a window
         *
         * @param string - This returns a formatted string holding the same details as the
         *                 WINDOWPLACEMENT structure.
         *
         * @exception UserSettingsException - If any error occurs and the setting cannot be retrieved.
         */
        std::string placement2Str(const WINDOWPLACEMENT& placement);


        //
        // This is the database object of user settings. This is where all the settings are
        // held and can be retrieved or set.
        //
            TA_Base_Core::IUserSettings* m_settings;
    };

} //end namespace TA_Base_Bus

#endif // !defined(AFX_SETTINGSMGR_H__7F6204C5_192D_11D6_B1B3_0050BAB0C99D__INCLUDED_)


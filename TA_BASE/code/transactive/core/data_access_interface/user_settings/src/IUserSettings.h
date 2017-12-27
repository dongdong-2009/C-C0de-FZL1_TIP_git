/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/user_settings/src/IUserSettings.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This interface contains the database retrieval and write methods for the UserSettings library.
  */

#if !defined(IUserSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
#define IUserSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IUserSettings
    {

    public:

        virtual ~IUserSettings() { };
        /**
         * getSettingAsString
         *
         * This retrieves a setting from the database or internal cache and returns it
         * to the caller.
         *
         * @param string - This is the name of the setting to retrieve
         *
         * @return string - This is the string value for the requested setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way.
         */
        virtual std::string getSettingAsString(const std::string& settingName) =0;

        /**
         * getSettingAsUnsignedLong
         *
         * This retrieves a setting from the database or internal cache and returns it
         * to the caller as an unsigned long.
         *
         * @param string - This is the name of the setting to retrieve
         *
         * @return unsigned long - This is the value for the requested setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way.
         */
        virtual unsigned long getSettingAsUnsignedLong(const std::string& settingName) =0;

        /** 
          * getSettingsLike
          *
          * Gets all settings prefixed with the string specified.  
          *
          * @param prefix The prefix of the setting name
          *
          * @exception DatabaseException - This is thrown if there is some error with the database
          *                                connection or an SQL statement failed to execute for
          *                                some reason.
          * @exception DataUserSettingsException - This is thrown if the user setting data was
          *                                        invalid in some way.
          */
        virtual std::vector<std::string> getSettingsLike(const std::string& prefix) = 0;

        /**
         * setSetting
         *
         * This updates a setting in the database and then updates the local cache.
         *
         * @param string - This is the name of the setting to update
         * @param string - This is the string value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way. If this exception has the
         *                                        type SETTING_NOT_FOUND then this means that the
         *                                        caller should try addSetting() instead.
         */
        virtual void setSetting(const std::string& name, const std::string& value) =0;

        /**
         * setSetting
         *
         * This updates a setting in the database and then updates the local cache.
         *
         * @param string - This is the name of the setting to update
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way. If this exception has the
         *                                        type SETTING_NOT_FOUND then this means that the
         *                                        caller should try addSetting() instead.
         */
        virtual void setSetting(const std::string& name, const unsigned long value) =0;

        /**
         * addSetting
         *
         * This adds a new setting into the database and then adds it to the local cache.
         *
         * @param string - This is the name of the setting to add
         * @param string - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way.
         */
        virtual void addSetting(const std::string& name, const std::string& value) = 0;

        /**
         * addSetting
         *
         * This adds a new setting into the database and then adds it to the local cache.
         *
         * @param string - This is the name of the setting to add
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way.
         */
        virtual void addSetting(const std::string& name, const unsigned long value) = 0;

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
        virtual void deleteSetting(const std::string& name) = 0;
    };
} //end namespace TA_Base_Core

#endif // !defined(IUserSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)

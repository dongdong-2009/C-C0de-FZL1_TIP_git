/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/user_settings/src/UserSettingsData.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class contains the database retrievals and writes for the UserSettings library.
  * It implements the IUserSettings interface.
  */


#if !defined(UserSettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define UserSettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

#include <map>
#include "core\data_access_interface\user_settings\src\IUserSettings.h"

namespace TA_Base_Core
{
    class UserSettingsData : public IUserSettings
    {

    public:

        /**
         * Constructor
         *
         * @param string - This is the current user's session Id. This will be stored
         *                        so the settings are saved for the same user as when they were
         *                        loaded.
         * @param unsigned long - This is the application type. This is needed to determine
         *                        which user settings to load/save.
         */
        UserSettingsData(std::string sessionId, unsigned long appType);

        /**
         * Destructor
         */
        virtual ~UserSettingsData();

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
        virtual std::string getSettingAsString(const std::string& name);


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
        virtual unsigned long getSettingAsUnsignedLong(const std::string& name);

        /** 
          * getSettingsLike
          *
          * Gets all settings prefixed with the specified string in the lexical order.
          * (Make sure strings like 'Setting2' are named 'Setting02' instead if it is to
          * come before 'Setting11'.
          *
          * @param prefix The prefix of the setting name
          *
          * @exception DatabaseException - This is thrown if there is some error with the database
          *                                connection or an SQL statement failed to execute for
          *                                some reason.
          * @exception DataUserSettingsException - This is thrown if the user setting data was
          *                                        invalid in some way.
          */
        virtual std::vector<std::string> getSettingsLike(const std::string& prefix);

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
        virtual void setSetting(const std::string& name, const std::string& value);

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
        virtual void setSetting(const std::string& name, const unsigned long value);

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
        virtual void addSetting(const std::string& name, const std::string& value);

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
        virtual void addSetting(const std::string& name, const unsigned long value);

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
        virtual void deleteSetting(const std::string& name);

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        UserSettingsData& operator=(const UserSettingsData &) { };
        UserSettingsData( const UserSettingsData& ) {};

        /**
         * reload
         *
         * This loads all settings from the database.
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingException - This is thrown if duplicate settings appear in
         *                                       the database.
         */
        void reload();

        /**
         * updateSettingInDatabase
         *
         * This updates a setting in the database.
         *
         * @param string - This is the name of the setting to update
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or the SQL failed to execute for
         *                                some reason.
         */
        void updateSettingInDatabase(const std::string& name, const std::string& value);

        /**
         * insertSettingIntoDatabase
         *
         * This inserts a new setting into the database.
         *
         * @param string - This is the name of the setting to insert
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or the SQL failed to execute for
         *                                some reason.
         */
        void insertSettingIntoDatabase(const std::string& name, const std::string& value);

        /**
         * unsignedLongToString
         *
         * This converts an unsigned long into a string.
         *
         * @param unsigned long - The unsigned long to convert.
         *
         * @return string - The unsigned long converted to a string.
         */
        std::string unsignedLongToString(unsigned long x);

        /**
         * stringToUnsignedLong
         *
         * This converts a string into an unsigneed long
         *
         * @param string - The string to convert.
         *
         * @return unsigned long - The string converted to an unsigned long.
         *
         * @exception DataUserSettingException - This indicates that the string could not be 
         *                                       converted into an unsigned long because it was
         *                                       the wrong format.
         */
        unsigned long stringToUnsignedLong(const std::string& str);

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        //
        // These constants are used for retrieving data from the IData object.
        //
        static const std::string NAME_COLUMN;
        static const std::string VALUE_COLUMN;

        //
        // We will use this type of map to store all our settings
        // as (name, value).
        //
        typedef std::map<std::string, std::string> Settings;

        //
        // This holds all the current, configured settings retrieved from the
        // database
        //
        Settings m_settings;

        //
        // We want to store the sesionID and application type so that when we go to
        // save the settings later we use the same operator and application type.
        //
		std::string m_sessionId;
        unsigned long m_applicationType;

        //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //
        bool m_isValidData;

        std::string m_localDatabase;

    };
} //end namespace TA_Base_Core

#endif // !defined(UserSettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)

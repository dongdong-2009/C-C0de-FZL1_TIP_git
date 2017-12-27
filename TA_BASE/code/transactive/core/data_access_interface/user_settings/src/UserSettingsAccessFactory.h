/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/user_settings/src/UserSettingsAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * UserSettingsAccessFactory is a singleton that is used to generate and retrieve 
  * IUserSettings objects.
  */


#if !defined(UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

#include <string>

namespace TA_Base_Core
{
    class IUserSettings;

    class UserSettingsAccessFactory
    {

    public:
        virtual ~UserSettingsAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a UserSettingsAccessFactory object.
         */
        static UserSettingsAccessFactory& getInstance()
        {
            static UserSettingsAccessFactory fact;
            return fact;
        }

        /**
         * getUserSettings
         *
         * Retrieves the user settings for the specified operator and application type.
         *
         * @param string - The sessionId which we will use to retrieve the operator
         *                        key from.
         * @param unsigned long - The application key which will be used to retrieve the settings
         *
         * @return A pointer to an object conforming to the IUserSettings interface.
         *
         */
        IUserSettings* getUserSettings(std::string sessionId, unsigned long appType);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        UserSettingsAccessFactory() { };
        UserSettingsAccessFactory& operator=(const UserSettingsAccessFactory &) { };
        UserSettingsAccessFactory( const UserSettingsAccessFactory& ) {};

    };
} //end namespace TA_Base_Core

#endif // !defined(UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)

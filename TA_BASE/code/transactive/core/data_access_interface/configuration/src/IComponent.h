 /*
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/configuration/src/IComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This interface contains the database retrieval and write methods for retrieving component
  * information for the Configuration Editor.
  */

#if !defined(IComponent_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
#define IComponent_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_

#include <string>

namespace TA_Base_Core
{
    class IComponent
    {

    public:

        virtual ~IComponent() { };


        /**
         * getCategory
         *
         * This returns the category this componet falls into
         *
         * @return string - This is the name of the category
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getCategory() =0;


        /**
         * getComponent
         *
         * This returns the name of the component
         *
         * @return string - This is the name of the component
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getComponent() =0;


        /**
         * getLibrary
         *
         * This returns the name of the dynamic library to load for this component
         *
         * @return string - This is the name of the DLL
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getLibrary() =0;


        /**
         * getHelpPath
         *
         * This returns the help path and file for this component
         *
         * @return string - This is the help file or path to pass to the Help Launcher
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the component data was
         *                                        invalid in some way.
         */
        virtual std::string getHelpPath() =0;

        
        /**
         * getEntityTypeName
         *
         * This returns the name of the entity type this component is for. If this component is not
         * for an entity then this will return "none"
         *
         * @return string - This is the name of the entity type or "none"
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way. If this exception has the
         *                                        type SETTING_NOT_FOUND then this means that the
         *                                        caller should try addSetting() instead.
         */
        virtual std::string getEntityTypeName() =0;

    };
} //end namespace TA_Base_Core

#endif // !defined(IComponent_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)

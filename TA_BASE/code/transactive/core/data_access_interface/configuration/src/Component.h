/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/configuration/src/Component.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class implements the IComponent interface and will retrieve the data
  * for a single entry from the CO_COMPONENT table for the Configuration Editor.
  * This is used in the Configuration Editor so does not need to have an invalidate
  * method. This is because the Configuration Editor does not perform online udpates.
  * If it needs to in the future it would need to be added in.
  */


#if !defined(Component_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define Component_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

#include "core\data_access_interface\configuration\src\IComponent.h"

namespace TA_Base_Core
{
    class Component : public IComponent
    {

    public:

        /**
         * Constructor
         *
         * @param unsigned long - The key of this component
         * @param string - The name of the category for this component
         * @param string - The name of the component for this component
         * @param string - The dynamic library (DLL) name for this component
         * @param string - The help path for this component
         * @param string - The entity type this component represents. This will default to "" if
         *                 this component does not represent an entity
         */
        Component(unsigned long key,
                  const std::string& category,
                  const std::string& component,
                  const std::string& library,
                  const std::string& help,
                  const std::string& entityType = "");


        /**
         * Destructor
         */
        virtual ~Component();


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
        virtual std::string getCategory()
        {
            return m_category;
        }


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
        virtual std::string getComponent()
        {
            return m_component;
        }


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
        virtual std::string getLibrary()
        {
            return m_library;
        }


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
        virtual std::string getHelpPath()
        {
            return m_helpPath;
        }

        
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
        virtual std::string getEntityTypeName()
        {
            return m_entityType;
        }


    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        Component& operator=(const Component &) { };
        Component( const Component& ) {};


        std::string m_category;   // The category name for this component
        std::string m_component;  // The name of this component
        std::string m_library;    // The name of the library (DLL) for this component
        std::string m_helpPath;   // The help file for this component
        std::string m_entityType; // The entity type this component. This string will be empty if
                                  // this component is not for an entitytype.

    };
} //end namespace TA_Base_Core

#endif // !defined(Component_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigGui.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigGui is an interface to a gui object that can be read and written to. It allows the gui object
  * implementation to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IConfigGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IGui.h"

namespace TA_Base_Core
{
    class IConfigEntity;


    class IConfigGui : public virtual IConfigItem, public IGui
    {

    public:
        virtual ~IConfigGui() { };



        /**
         * getDateCreated
         *
         * Returns the date created for this location.
         *
         * @return The date created for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this location.
         *
         * @return The date modified for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateModified() =0;

        
        /**
         * getRelatedEntity
         *
         * Most GUI applications have an associated entity (specified in the --entity-name command line
         * parameter). This will retrieve that entity and return it.
         *
         * @return IConfigEntity* - The entity related to this Gui application. If the Gui does not have
         * an entity this will return NULL.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the key is invalid 
         */
        virtual IConfigEntity* getRelatedEntity() =0;


        /**
         * setFullName
         *
         * Sets the full name of this Gui.
         *
         * @param The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setFullName(const std::string& fullName) =0;


        /**
         * setExecutable
         *
         * Sets the executable for this Gui.
         *
         * @param The executable for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual void setExecutable(const std::string executable) =0;


        /**
         * setHelpFilePath
         *
         * Sets the help file path for the Gui
         *
         * @param The help file path for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual void setHelpFilePath(const std::string& helpFilePath) =0;


        /**
         * setIsIconDisplayed
         *
         * Sets whether the icon should be displayed for this Gui
         *
         * @param true if the GUI should be displayed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual void setIsIconDisplayed(bool isIconDisplayed) =0;


        /**
         * setAreInstancesLimited
         *
         * Sets whether the application should be limited to one instance
         *
         * @param true if the GUI should only have one instance running
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual void setAreInstancesLimited(bool areInstancesLimited) =0;


        /**
         * setShouldPassParameters
         *
         * Sets whether the application should be re-passed the parameters if it 
         * should not be relaunched
         *
         * @param true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual void setShouldPassParameters(bool shouldPassParams) =0;


        /**
         * setShouldReposition
         *
         * Sets whether the application should be repositioned when launched.
         *
         * @param true If the GUI should be repositioned when it is launched.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual void setShouldReposition(bool shouldReposition) =0;


        /**
         * setShouldTerminateOnLogout
         *
         * Sets whether the application should be terminated when the user logs out.
         *
         * @param true if the GUI should be terminated on system logout
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual void setShouldTerminateOnLogout(bool shouldTerminateOnLogout) =0;

        
        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete() =0;


        /**
         * deleteThisGui
         *
         * Removes this gui from the database. 
         * The calling application MUST then delete this gui object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This gui has not been deleted
         */
        virtual void deleteThisGui() =0;



    };
} //close namespace TA_Base_Core

#endif // !defined(IConfigGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

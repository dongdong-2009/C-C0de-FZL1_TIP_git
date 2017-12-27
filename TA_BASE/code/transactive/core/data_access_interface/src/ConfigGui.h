/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigGui.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigGui is an implementation of IConfigGui. It holds the data specific to a gui entry
  * in the database, and allows read and write access to that data.
  */

#if !defined(ConfigGui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigGui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IConfigGui.h"

namespace TA_Base_Core
{
	class IData;
    class GuiHelper;

    class ConfigGui : public IConfigGui
    {

    public:

        /**
         * Constructor
         *
         * Construct a Gui class for a new GUi
         *
         * @param string - An entity type. This will be used to create a related entity
         *                 for the Gui.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        ConfigGui(const std::string& type);

        
        /**
         * Constructor
         *
         * Construct an Gui class based around a key.
         *
         * @param key The key of this Gui in the database
         */
        ConfigGui(const unsigned long key);

		/**
         * Constructor
         *
         * Construct a ConfigGui class based around dataset
         */
		ConfigGui(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a Gui class based around an existing gui. This will
		 * copy all the paramters of the existing gui
         *
         * @param theGui The gui to copy
         */
        ConfigGui( const ConfigGui& theGui);            


        /**
         * Destructor
         */
        virtual ~ConfigGui();


        /**
         * getKey
         *
         * Returns the key for this Gui.
         *
         * @return The key for this gui as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


        /**
         * getFullName
         *
         * Returns the full name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getFullName();


        /**
         * getExecutable
         *
         * Returns the executable for this Gui.
         *
         * @return The executable for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getExecutable();


        /**
         * getHelpFilePath
         *
         * Returns the help file path for the Gui
         *
         * @return The help file path for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getHelpFilePath();


        /**
         * isIconDisplayed
         *
         * Returns whether the icon should be displayed for this Gui
         *
         * @return true if the GUI should be displayed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual bool isIconDisplayed();


        /**
         * areInstancesLimited
         *
         * Returns whether the application should be limited to one instance
         *
         * @return true if the GUI should only have one instance running
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual bool areInstancesLimited();


        /**
         * shouldReposition
         *
         * Returns whether the application should be repositioned if it should not be relaunched 
         * (only meaningful if application is limited to 1 running instance).
         *
         * @return true if the GUI should be repositioned 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual bool shouldReposition();

        /**
         * shouldPassParameters
         *
         * Returns whether the application should be re-passed the parameters if it 
         * should not be relaunched
         *
         * @return true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual bool shouldPassParameters();


        /**
         * shouldTerminateOnLogout
         *
         * Returns whether the application should be closed when the user logs out of the system 
         *
         * @return true if the GUI should terminate on a logout
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual bool shouldTerminateOnLogout();
        

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
        virtual void setShouldTerminateOnLogout(bool shouldTerminateOnLogout);

        
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
        virtual time_t getDateCreated();


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
        virtual time_t getDateModified();

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

        
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
        virtual void deleteThisGui();

        
        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete();

        
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
        virtual void setFullName(const std::string& fullName);


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
        virtual void setExecutable(const std::string executable);


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
        virtual void setHelpFilePath(const std::string& helpFilePath);

        
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
        virtual void setIsIconDisplayed(bool isIconDisplayed);


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
        virtual void setAreInstancesLimited(bool areInstancesLimited);


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
        virtual void setShouldPassParameters(bool shouldPassParams);

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
        virtual void setShouldReposition(bool shouldReposition);
        
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
        virtual IConfigEntity* getRelatedEntity();

        
        /**
         * invalidate
         *
         * Make the data contained by this gui as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        /**
         * updateGuiChanges
         *
         * This updates the map recording the changes to the gui. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateGuiChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

        /**
         * getEntityNameFromExecutable
         *
         * This parses the executable string and retrieves the entity name out of it
         *
         * @param string - The executable path to process
         *
         * @return string - The name of the entity or an empty string if one is not found
         */
        std::string getEntityNameFromExecutable(const std::string& executable);

		
        // Assignment operator not used so it is made private
		ConfigGui& operator=(const ConfigGui&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string SHORTNAME;
        static const std::string FULLNAME;
        static const std::string EXECUTABLE;
        static const std::string HELPFILE;
        static const std::string IS_ICON_DISPLAYED;
        static const std::string ARE_INSTANCES_LIMITED;
        static const std::string SHOULD_PASS_PARAMS;
        static const std::string SHOULD_TERMINATE_ON_LOGOUT;
        static const std::string SHOULD_REPOSITION;

        static const std::string NO;
        static const std::string YES;

        static const std::string ENTITYNAME;

        static const int MAX_ENTITY_NAME_LENGTH;
        static const int MAX_EXECUTABLE_LENGTH;


        // Some Gui applications are reserved and cannot be deleted. All of these will have a key below
        // this constant
        static const unsigned long MAXIMUM_RESERVED_KEY;

        TA_Base_Core::GuiHelper * m_guiHelper;

        IConfigEntity* m_relatedEntity;   // The entity linked to this GUI application. This can be NULL

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_guiChanges; // This is a map of all changes made to the gui so that they are
                                  // recorded and can be sent in audit or online update messages.

		// TD #2899
		// This flag will be true if this application was created with a type of "None".
		// (i.e. has no related entity)
		bool m_noType;

    };
} // closes TA_Base_Core

#endif // !defined(ConfigGui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

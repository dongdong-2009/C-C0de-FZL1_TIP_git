/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigActionGroup.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * ConfigActionGroup is an implementation of IConfigActionGroup. It holds the data specific to a ActionGroup entry
  * in the database, and allows read-write access to that data.
  *
  */

#if !defined(ConfigActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>
#include <vector>

#include "core/data_access_interface/src/IConfigActionGroup.h"

namespace TA_Base_Core
{
	// forward declarations
	class IData;
    class ActionGroupHelper;


    class ConfigActionGroup : public IConfigActionGroup
    {

    public:

        /**
         * Constructor
         *
         * Construct a new ConfigActionGroup class
         */
        ConfigActionGroup();

        
        /**
         * Constructor
         *
         * Construct a ConfigActionGroup class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this ActionGroup in the database
         * @param name The name of this ActionGroup in the database
         */
        ConfigActionGroup( const unsigned long key, const std::string& name );

        /**
         * Constructor
         *
         * Construct a ConfigActionGroup class based around action group dataset
         */
		ConfigActionGroup(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions);

        /**
         * Copy Constructor
         *
         * This copies the ActionGroup passed in and makes a new config object
         */
    	ConfigActionGroup( const ConfigActionGroup& theActionGroup);


        /**
         * Destructor
         */
        virtual ~ConfigActionGroup();


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
        virtual bool hasChanged()
        {
            // If the ActionGroup changes is empty then nothing has changed
            return !m_actionGroupChanges.empty();
        }


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
         * getKey
         *
         * Returns the key for this ActionGroup.
         *
         * @return The key for this ActionGroup as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this ActionGroup. If this is the first time data for this ActionGroup
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this ActionGroup as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();		

		
		virtual std::string getDisplayName();
		
        /**
         * isControlType
         *
         * Returns whether or not the isControlType flag is set in the database.
         *
         * @return boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isControlType();

        
        /**
         * isActionIncluded
         *
         * This checks to see if the specified actionkey is currently included in the list of actions
         * for this ActionGroup.
         *
         * @param unsigned long - The action key to check for
         *
         * @return bool - This will return true if the actionkey is associated with this ActionGroup and false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isActionIncluded(unsigned long actionKey);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this ActionGroup
         *
         * @return The date created for this ActionGroup as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this ActionGroup.
         *
         * @return The date modified for this ActionGroup as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();


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
         * setDisplayName
         *
         * Sets the display name of this item locally.
         *
         * @param name The display name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setDisplayName(const std::string& displayName);

        /**
         * setIsControlType
         *
         * Sets whether or not the isControlType flag is set in the database.
         *
         * @param boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setIsControlType(bool isControlType);

        
        /**
         * addAction
         *
         * Adds a new action to the ActionGroup type
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void addAction(unsigned long actionKey);


        /**
         * removeAction
         *
         * Removes an action from the ActionGroup type
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeAction(unsigned long actionKey);



        /**
         * deleteThisActionGroup
         *
         * Removes this ActionGroup from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ActionGroup was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This ActionGroup has not been deleted
         */
        virtual void deleteThisActionGroup();

        
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
         * invalidate
         *
         * Make the data contained by this ActionGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		ConfigActionGroup& operator=(const ConfigActionGroup&);

           
	    /**
         * updateChanges
         *
         * This updates the map recording the changes to the ActionGroup. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

        /**
         * getActionNames
         * 
         * This uses the m_actionGroupChanges member to generate a list of changes using names instead of keys
         *
         * @return ItemChanges - All action changes.
         */
        ItemChanges getActionNames();

		
       // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string IS_CONTROL_TYPE;

        static const std::string YES;
        static const std::string NO;

        static const std::string ACTION_SELECTED;
        static const std::string ACTION_UNSELECTED;


        ActionGroupHelper* m_actionGroupHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of actions in the
                                          // Configuration Editor because new actions do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_actionGroupChanges;     // This is a map of all changes made to the ActionGroup so that they are
                                       // recorded and can be sent in audit or online update messages.

		
    };
} // closes TA_Base_Core

#endif // !defined(ConfigActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

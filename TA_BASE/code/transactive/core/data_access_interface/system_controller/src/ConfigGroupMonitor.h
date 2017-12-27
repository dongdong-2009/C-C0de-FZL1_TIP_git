/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ConfigGroupMonitor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigGroupMonitor is an implementation of IConfigGroupMonitor. It holds the data specific to a monitor group entry
  * in the database, and allows read-write access to that data.
  */

#if !defined(ConfigGroupMonitor_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigGroupMonitor_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/system_controller/src/IConfigGroupMonitor.h"

namespace TA_Base_Core
{
    class ConfigGroupMonitor : public IConfigGroupMonitor
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigGroupMonitor class ready to be populated.
         */
        ConfigGroupMonitor( );


      
        /**
         * Constructor
         *
         * Construct a complete monitor group, this will set isValidData to true
         *
         * @param key The key of this monitor group in the database
         * @param name The name of this monitor group in the database
         * @param description The description of this monitor group in the database
         * @param dateCreated The date and time the monitor group was created
         * @param dateModified The date and time the monitor group was last modified
         */
        ConfigGroupMonitor( const unsigned long key, const std::string& name, const std::string& description,
                            const time_t dateCreated, const time_t dateModified);


        /**
         * Constructor
         *
         * Construct a monitor group class based around an existing monitor group. This will
		 * copy all the paramters of the existing monitor group
         *
         * @param theGroup The monitor group to copy
         */
		ConfigGroupMonitor( const ConfigGroupMonitor& theGroup);            	
		

        /**
         * Destructor
         */
        virtual ~ConfigGroupMonitor();


        /**
         * getKey
         *
         * Returns the key for this monitor group.
         *
         * @return The key for this monitor group as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges()
        {
            return m_groupChanges;
        }

        
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
            // If the monitor group changes is empty then nothing has changed
            return !m_groupChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew()
        {
            return m_isNew;
        }

        
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
        };


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
         * deleteThisGroup
         *
         * Removes this monitor group from the database. 
         * The calling application MUST then delete this monitor group object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this monitor group was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This monitor group has not been deleted
         */
        virtual void deleteThisGroup();


        /**
         * getName
         *
         * Returns the name of this monitor group. If this is the first time data for this monitor group
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
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
         * getDescription
         *
         * Returns the description of this monitor group. If this is the first time data for this monitor group
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this monitor group as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();
		

        /**
         * getDateCreated
         *
         * Returns the date created for this monitor group
         *
         * @return The date created for this monitor group as a time_t.
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
         * Returns the date modified for this monitor group.
         *
         * @return The date modified for this monitor group as a time_t.
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
         * Sets the name of this monitor group locally.
         *
         * @param name The name to give this monitor group.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the monitor group key is invalid (and this is not a new monitor group).
         */
        virtual void setName(const std::string& name);


        /**
         * setDescription
         *
         * Sets the description of this monitor group locally.
         *
         * @param name The description to give this monitor group.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the monitor group key is invalid (and this is not a new monitor group).
         */
        virtual void setDescription(const std::string& description);


        /**
         * invalidate
         *
         * Make the data contained by this monitor group as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


private:
	
        /**
         * updateGroupChanges
         *
         * This updates the map recording the changes to the monitor group. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateGroupChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
		
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();


        /**
         * modifyExistingGroup
         *
         * This will update an existing SC group in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the SC group object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingGroup();

        
        /**
         * addNewGroup
         *
         * This will add a new SC group in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the SC group object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewGroup();


        /**
         * checkForLinkedSystemControllers
         *
         * This will check to see if any System Controllers are currently assigned to this group
         *
         * @exception DataExeption - This is thrown with the type CANNOT_DELETE if it is linked to
         *                           system controllers
         * @exception DatabaseException
         */
        void checkForLinkedSystemControllers();


        // Assignment operator not used so it is made private
		ConfigGroupMonitor& operator=(const ConfigGroupMonitor&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string DESCRIPTION;

        static long s_nextAvailableIdentifier; // The next unique identifier available

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of monitor groups in the
                                          // Configuration Editor because new monitor groups do not have keys.
                                          // Therefore we use this method for identification.

        unsigned long m_key;
        std::string m_name;            
        std::string m_description;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;

        ItemChanges m_groupChanges; // This is a map of all changes made to the monitor group so that they are
                                    // recorded and can be sent in audit or online update messages.
        
    };
} // closes TA_Base_Core

#endif // !defined(ConfigGroupMonitor_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

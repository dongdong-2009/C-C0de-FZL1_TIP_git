/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigGlobalParameter.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigGlobalParameter is an implementation of IConfigGlobalParameter. It holds the data specific to a GlobalParameter entry
  * in the database, and allows read-write access to that data.
  */

#if !defined(ConfigGlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigGlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>
#include <vector>

#include "core/data_access_interface/src/IConfigGlobalParameter.h"

namespace TA_Base_Core
{
	class IData;
    class GlobalParameterHelper;

    class ConfigGlobalParameter : public IConfigGlobalParameter
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigGlobalParameter class ready to be populated.
         */
        ConfigGlobalParameter( );


        /**
         * Constructor
         *
         * Construct a GlobalParameter class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this GlobalParameter in the database
         */
        ConfigGlobalParameter( const unsigned long key );


		/**
         * Constructor
         *
         * Construct a ConfigGlobalParameter class based around dataset
         */
		ConfigGlobalParameter(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a GlobalParameter class based around an existing GlobalParameter. This will
		 * copy all the paramters of the existing GlobalParameter
         *
         * @param theGlobalParameter The GlobalParameter to copy
         */
		ConfigGlobalParameter( const ConfigGlobalParameter& theGlobalParameter);
		

        /**
         * Destructor
         */
        virtual ~ConfigGlobalParameter();


        /**
         * getKey
         *
         * Returns the key for this GlobalParameter.
         *
         * @return The key for this GlobalParameter as an unsigned long.
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
            return m_GlobalParameterChanges;
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
            // If the GlobalParameter changes is empty then nothing has changed
            return !m_GlobalParameterChanges.empty();
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
         * deleteThisGlobalParameter
         *
         * Removes this GlobalParameter from the database. 
         * The calling application MUST then delete this GlobalParameter object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this GlobalParameter was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This GlobalParameter has not been deleted
         */
        virtual void deleteThisGlobalParameter();


        /**
         * getName
         *
         * Returns the name of this GlobalParameter. If this is the first time data for this GlobalParameter
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
		

        virtual std::string getValue();
			

        virtual unsigned long getSubsystemKey();	

        /**
         * getDateCreated
         *
         * Returns the date created for this GlobalParameter
         *
         * @return The date created for this GlobalParameter as a time_t.
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
         * Returns the date modified for this GlobalParameter.
         *
         * @return The date modified for this GlobalParameter as a time_t.
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
         * Sets the name of this GlobalParameter locally.
         *
         * @param name The name to give this GlobalParameter.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the GlobalParameter key is invalid (and this is not a new GlobalParameter).
         */
        virtual void setName(const std::string& name);


        virtual void setValue(const std::string& value);


        virtual void setSubsystemKey(unsigned long subsystemKey);


        /**
         * invalidate
         *
         * Make the data contained by this GlobalParameter as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


private:
	
        /**
         * updateGlobalParameterChanges
         *
         * This updates the map recording the changes to the GlobalParameter. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateGlobalParameterChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
		
        // Assignment operator not used so it is made private
		ConfigGlobalParameter& operator=(const ConfigGlobalParameter&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string VALUE;
        static const std::string SUBSYSTEMKEY;


        GlobalParameterHelper* m_GlobalParameterHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of GlobalParameters in the
                                          // Configuration Editor because new GlobalParameters do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_GlobalParameterChanges; // This is a map of all changes made to the GlobalParameter so that they are
                                       // recorded and can be sent in audit or online update messages.
        
    };
} // closes TA_Base_Core

#endif // !defined(ConfigGlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

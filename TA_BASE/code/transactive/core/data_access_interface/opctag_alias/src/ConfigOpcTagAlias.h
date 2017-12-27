/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/ConfigOpcTagAlias.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigOpcTagAlias is an implementation of IConfigOpcTagAlias. It holds the data specific to a OpcTagAlias entry
  * in the database, and allows read-write access to that data.
  *
  */

#if !defined(ConfigOpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigOpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/opctag_alias/src/IConfigOpcTagAlias.h"

namespace TA_Base_Core
{
    class OpcTagAliasHelper;


    class ConfigOpcTagAlias : public IConfigOpcTagAlias
    {

    public:

        /**
         * Constructor
         *
         * Construct a new ConfigOpcTagAlias class
         */
        ConfigOpcTagAlias();

        
        /**
         * Constructor
         *
         * Construct a ConfigOpcTagAlias class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        ConfigOpcTagAlias( const unsigned long entitykey, const std::string& name );

        /**
         * Constructor
         *
         * Construct a OpcTagAlias class based around all the necessary values, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        ConfigOpcTagAlias( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string& attribute);


        /**
         * Copy Constructor
         *
         * This copies the OpcTagAlias passed in and makes a new config object
         */
    	ConfigOpcTagAlias( const ConfigOpcTagAlias& theOpcTagAlias);


        /**
         * Destructor
         */
        virtual ~ConfigOpcTagAlias();


        /**
         * getUniqueIdentifier
         *
         * This retrieves the unique identifier for this item. We cannot use keys for identification
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
            // If the OpcTagAlias changes is empty then nothing has changed
            return !m_itemChanges.empty();
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
         * Returns the key for this OpcTagAlias.
         *
         * @return The key for this OpcTagAlias as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this OpcTagAlias. If this is the first time data for this OpcTagAlias
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this OpcTagAlias as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getName();		

	    /**
	      * getOpcTagAlias
	      * 
	      * get the OPC Alias
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getOpcTagAlias(); 

        /**
	      * getAttributeName
	      * 
	      * Returns the the attribute
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getAttributeName();

        /**
	      * getEntityName
	      * 
	      * get the datapoint name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getEntityName();

	    /**
	      * long getEntityKey
	      * 
	      * get the datapoint key
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getEntityKey() ;
        
        /**
         * getDateCreated
         *
         * Returns the date created for this OpcTagAlias
         *
         * @return The date created for this OpcTagAlias as a time_t.
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
         * Returns the date modified for this OpcTagAlias.
         *
         * @return The date modified for this OpcTagAlias as a time_t.
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
	      * setLessAlias
	      * 
	      * set the OPC Alias
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setOpcTagAlias(std::string& aliasname); 

	    /**
	      * setAttributeName
	      * 
	      * set the attribute
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setAttributeName(std::string& attribute); 

        /**
	      * setEntityName
	      * 
	      * set the entity name
	      * 
	      * @return void
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityName(std::string& entityname);

	    /**
	      * setEntityKey
	      * 
	      * set the entity key
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityKey(unsigned long entitykey);

        /**
         * deleteThisOpcTagAlias
         *
         * Removes this OpcTagAlias from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this OpcTagAlias was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This OpcTagAlias has not been deleted
         */
        virtual void deleteThisOpcTagAlias();

        
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
         * Make the data contained by this OpcTagAlias as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		ConfigOpcTagAlias& operator=(const ConfigOpcTagAlias&);

           
	    /**
         * updateChanges
         *
         * This updates the map recording the changes to the OpcTagAlias. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
        OpcTagAliasHelper* m_opcTagAliasHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of actions in the
                                          // Configuration Editor because new actions do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        TA_Base_Core::ItemChanges   m_itemChanges;     // This is a map of all changes made to the OpcTagAlias so that they are
                                       // recorded and can be sent in audit or online update messages.

    private:
        static const std::string OPC_ALIAS;
        static const std::string ENTITY_KEY;
        static const std::string ATTRIBUTE;
		
    };
} // closes TA_Base_Core

#endif // !defined(ConfigOpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

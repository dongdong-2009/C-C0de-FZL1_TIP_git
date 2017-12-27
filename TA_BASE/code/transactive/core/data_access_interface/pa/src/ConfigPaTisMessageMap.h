/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaTisMessageMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaTisMessageMap is a concrete implementation of IConfigPaTisMessageMap, which is in turn an implementation
  * of IPaTisMessageMap. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaTisMessageMaps.
  */


#if !defined(ConfigPaTisMessageMap_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaTisMessageMap_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaTisMessageMap.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaTisMessageMapHelper;

    class ConfigPaTisMessageMap : public IConfigPaTisMessageMap
    {

    public:
        /**
         * ConfigPaTisMessageMap (constructor)
         *
         * Constructs a new instance of the ConfigPaTisMessageMap with no id. This is used when
         * creating a *new* PaTisMessageMap - that is, one that does not yet exist in the database.
         */
        ConfigPaTisMessageMap();

        /**
         * ConfigPaTisMessageMap (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaTisMessageMapHelper::PaTisMessageMapHelper(IData*)
         *
         */
        ConfigPaTisMessageMap(unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigPaTisMessageMap (constructor)
         *
         * Constructs a new instance of ConfigPaTisMessageMap with the specified id. This is used 
         * when creating a ConfigPaTisMessageMap based around an existing PaTisMessageMap in the database.
         *
         * @param key The key to this PaTisMessageMap in the database.
         */
        ConfigPaTisMessageMap(const unsigned long idKey);


        /**
         * ConfigPaTisMessageMap (copy constructor)
         *
         * @param ConfigPaTisMessageMap& - The PaTisMessageMap to make a copy of.
         */
        ConfigPaTisMessageMap( const ConfigPaTisMessageMap& theConfigPaTisMessageMap);

        
        virtual ~ConfigPaTisMessageMap();


    public:

        /**
         * deleteThisObject
         *
         * Removes this PaTisMessageMap from the database. 
         * The calling application MUST then delete this PaTisMessageMap object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaTisMessageMap was initially loaded from the database
         *      OR     - writePaTisMessageMapData() has already been called
         *      This PaTisMessageMap has not been deleted
         */
        void deleteThisObject(bool cascade=false);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PaTisMessageMap object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaTisMessageMap has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this PaTisMessageMap.
         *
         * @return The key for this PaTisMessageMap as an unsigned long.
         *
         * pre: Either - this PaTisMessageMap was initially loaded from the database
         *      OR     - writePaTisMessageMapData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getPaDvaMessageKey();
        virtual unsigned long getTisMessageTag();
        virtual unsigned long getTisLibrarySection();
        virtual unsigned long getTisLibraryVersion();

        virtual void setPaDvaMessageKey(const unsigned long id);
        virtual void setTisMessageTag(const unsigned long tisMessageTag);
        virtual void setTisLibrarySection(const unsigned long tisLibrarySection);
        virtual void setTisLibraryVersion(const unsigned long version);


         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA DVA Message this is 
         * just the key in a string format e.g. "PA Dva Message 1".
         *
         * @return The key description for this item as a std::string
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
         * setName
         *
         * Not applicable for PA DVA Messages.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to this zone. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_paTisMessageMapChanges;
        }

       /**
         * getUniqueIdentifier
         *
         * This retrieves the unique identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this operator
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };

        /**
         * getDateCreated
         *
         * Returns the date created for this PaTisMessageMap
         *
         * @return The date created for this PaTisMessageMap as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaTisMessageMap.
         *
         * @return The date modified for this PaTisMessageMap as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the PaTisMessageMap has been changed by the user.
         *
         * @return bool - This will return true if the PaTisMessageMap does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaTisMessageMap changes is empty then nothing has changed
            return !m_paTisMessageMapChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this PaTisMessageMap is new. A new PaTisMessageMap is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaTisMessageMap has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this PaTisMessageMap as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updatePaTisMessageMapChanges
         *
         * This updates the map recording the changes to the PaTisMessageMap. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaTisMessageMapChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaTisMessageMapChanges
         *
         * This updates the map recording the changes to the PaTisMessageMap. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePaTisMessageMapChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PaTisMessageMap not used so it is made private
		ConfigPaTisMessageMap& operator=(const ConfigPaTisMessageMap&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaTisMessageMaps in the
                                          // Configuration Editor because new PaTisMessageMaps do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PaTisMessageMapHelper* m_paTisMessageMapHelper;


        TA_Base_Core::ItemChanges m_paTisMessageMapChanges; // This is a map of all changes made to the PaTisMessageMap so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Base_Core

#endif // !defined(ConfigPaTisMessageMap_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

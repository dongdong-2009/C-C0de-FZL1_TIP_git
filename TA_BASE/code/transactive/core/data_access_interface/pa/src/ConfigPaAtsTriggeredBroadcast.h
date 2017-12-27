/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaAtsTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaAtsTriggeredBroadcast is a concrete implementation of IConfigPaAtsTriggeredBroadcast, 
  * which is in turn an implementation
  * of IPaAtsTriggeredBroadcast. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaAtsTriggeredBroadcasts.
  *
  */


#if !defined(ConfigPaAtsTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaAtsTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaAtsTriggeredBroadcast.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaAtsTriggeredBroadcastHelper;

    class ConfigPaAtsTriggeredBroadcast : public IConfigPaAtsTriggeredBroadcast
    {

    public:
        /**
         * ConfigPaAtsTriggeredBroadcast (constructor)
         *
         * Constructs a new instance of the ConfigPaAtsTriggeredBroadcast with no id. This is used when
         * creating a *new* PaAtsTriggeredBroadcast - that is, one that does not yet exist in the database.
         */
        ConfigPaAtsTriggeredBroadcast();

        /**
         * ConfigPaAtsTriggeredBroadcast (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper(IData*)
         *
         */
        ConfigPaAtsTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigPaAtsTriggeredBroadcast (constructor)
         *
         * Constructs a new instance of ConfigPaAtsTriggeredBroadcast with the specified id. This is used 
         * when creating a ConfigPaAtsTriggeredBroadcast based around an existing PaAtsTriggeredBroadcast in the database.
         *
         * @param key The key to this PaAtsTriggeredBroadcast in the database.
         */
        ConfigPaAtsTriggeredBroadcast(const unsigned long idKey);


        /**
         * ConfigPaAtsTriggeredBroadcast (copy constructor)
         *
         * @param ConfigPaAtsTriggeredBroadcast& - The PaAtsTriggeredBroadcast to make a copy of.
         */
        ConfigPaAtsTriggeredBroadcast( const ConfigPaAtsTriggeredBroadcast& theConfigPaAtsTriggeredBroadcast);

        
        virtual ~ConfigPaAtsTriggeredBroadcast();


    public:

        /**
         * deleteThisObject
         *
         * Removes this PaAtsTriggeredBroadcast from the database. 
         * The calling application MUST then delete this PaAtsTriggeredBroadcast object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaAtsTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaAtsTriggeredBroadcastData() has already been called
         *      This PaAtsTriggeredBroadcast has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PaAtsTriggeredBroadcast object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaAtsTriggeredBroadcast has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this PaAtsTriggeredBroadcast.
         *
         * @return The key for this PaAtsTriggeredBroadcast as an unsigned long.
         *
         * pre: Either - this PaAtsTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaAtsTriggeredBroadcastData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getAtsEventType();
        virtual unsigned long getLocationKey();
        virtual unsigned long getDestinationLocationKey();
        virtual unsigned long getPlatformId();
        virtual bool          getIsLastTrain();
        virtual unsigned long getPaZoneGroupKey();
        virtual unsigned long getPaDvaMessageKey();
        virtual bool          getIsEnabledParameter();
        virtual std::string   getDvaMessageVersion();

        virtual void setAtsEventType(unsigned long eventType);
        virtual void setLocationKey(unsigned long locationKey);
        virtual void setDestinationLocationKey(unsigned long key);
        virtual void setPlatformId(unsigned long platformId);
        virtual void setIsLastTrain(bool isEnabled);
        virtual void setPaZoneGroupKey(unsigned long key);
        virtual void setPaDvaMessageKey(unsigned long key);
        virtual void setIsEnabledParameter(bool isEnabled);
        virtual void setDvaMessageVersion(const std::string& version);

         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA ATS Triggered Broadcast this is 
         * just the key in a string format e.g. "PA ATS Triggered Broadcast 1".
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
         * Not applicable for PA ATS Triggered Broadcasts.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to this ATS Triggered Broadcast. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_paAtsTriggeredBroadcastChanges;
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
        }

        /**
         * getDateCreated
         *
         * Returns the date created for this PaAtsTriggeredBroadcast
         *
         * @return The date created for this PaAtsTriggeredBroadcast as a time_t.
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
         * Returns the date modified for this PaAtsTriggeredBroadcast.
         *
         * @return The date modified for this PaAtsTriggeredBroadcast as a time_t.
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
         * hasChanged
         *
         * This should be called to determine if any part of the PaAtsTriggeredBroadcast has been changed by the user.
         *
         * @return bool - This will return true if the PaAtsTriggeredBroadcast does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaAtsTriggeredBroadcast changes is empty then nothing has changed
            return !m_paAtsTriggeredBroadcastChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this PaAtsTriggeredBroadcast is new. A new PaAtsTriggeredBroadcast is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaAtsTriggeredBroadcast has not been applied to the database.
         */
        virtual bool isNew();



        /**
         * invalidate
         *
         * Make the data contained by this PaAtsTriggeredBroadcast as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updatePaAtsTriggeredBroadcastChanges
         *
         * This updates the map recording the changes to the PaAtsTriggeredBroadcast. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaAtsTriggeredBroadcastChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaAtsTriggeredBroadcastChanges
         *
         * This updates the map recording the changes to the PaAtsTriggeredBroadcast. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePaAtsTriggeredBroadcastChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PaAtsTriggeredBroadcast not used so it is made private
		ConfigPaAtsTriggeredBroadcast& operator=(const ConfigPaAtsTriggeredBroadcast&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaAtsTriggeredBroadcasts in the
                                          // Configuration Editor because new PaAtsTriggeredBroadcasts do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PaAtsTriggeredBroadcastHelper* m_paAtsTriggeredBroadcastHelper;


        TA_Base_Core::ItemChanges m_paAtsTriggeredBroadcastChanges; // This is a map of all changes made to the PaAtsTriggeredBroadcast so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Base_Core

#endif // !defined(ConfigPaAtsTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/ConfigPaFasTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * ConfigPaFasTriggeredBroadcast is a concrete implementation of IConfigPaFasTriggeredBroadcast, 
  * which is in turn an implementation
  * of IPaFasTriggeredBroadcast. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaFasTriggeredBroadcasts.
  *
  */


#if !defined(ConfigPaFasTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaFasTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaFasTriggeredBroadcast.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaFasTriggeredBroadcastHelper;

    class ConfigPaFasTriggeredBroadcast : public IConfigPaFasTriggeredBroadcast
    {

    public:
        /**
         * ConfigPaFasTriggeredBroadcast (constructor)
         *
         * Constructs a new instance of the ConfigPaFasTriggeredBroadcast with no id. This is used when
         * creating a *new* PaFasTriggeredBroadcast - that is, one that does not yet exist in the database.
         */
        ConfigPaFasTriggeredBroadcast();

        /**
         * ConfigPaFasTriggeredBroadcast (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper(IData*)
         *
         */
        ConfigPaFasTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigPaFasTriggeredBroadcast (constructor)
         *
         * Constructs a new instance of ConfigPaFasTriggeredBroadcast with the specified id. This is used 
         * when creating a ConfigPaFasTriggeredBroadcast based around an existing PaFasTriggeredBroadcast in the database.
         *
         * @param key The key to this PaFasTriggeredBroadcast in the database.
         */
        ConfigPaFasTriggeredBroadcast(const unsigned long idKey);


        /**
         * ConfigPaFasTriggeredBroadcast (copy constructor)
         *
         * @param ConfigPaFasTriggeredBroadcast& - The PaFasTriggeredBroadcast to make a copy of.
         */
        ConfigPaFasTriggeredBroadcast( const ConfigPaFasTriggeredBroadcast& theConfigPaFasTriggeredBroadcast);

        
        virtual ~ConfigPaFasTriggeredBroadcast();


    public:

        /**
         * deleteThisObject
         *
         * Removes this PaFasTriggeredBroadcast from the database. 
         * The calling application MUST then delete this PaFasTriggeredBroadcast object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaFasTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaFasTriggeredBroadcastData() has already been called
         *      This PaFasTriggeredBroadcast has not been deleted
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
         * @exception DataConfigurationException If the data contained in the PaFasTriggeredBroadcast object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaFasTriggeredBroadcast has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this PaFasTriggeredBroadcast.
         *
         * @return The key for this PaFasTriggeredBroadcast as an unsigned long.
         *
         * pre: Either - this PaFasTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaFasTriggeredBroadcastData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getFasAlarmEntityKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getPaCyclicTimes();
        virtual unsigned long getPaZoneGroupKey();
        virtual unsigned long getPaDvaMessageKey();
        virtual bool          getIsEnabledParameter();

        virtual void setFasAlarmEntityKey(unsigned long ulEntityKey);
        virtual void setLocationKey(unsigned long locationKey);
        virtual void setPaCyclicTimes(unsigned long platformId);
        virtual void setPaZoneGroupKey(unsigned long key);
        virtual void setPaDvaMessageKey(unsigned long key);
        virtual void setIsEnabledParameter(bool isEnabled);

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
            return m_PaFasTriggeredBroadcastChanges;
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
         * Returns the date created for this PaFasTriggeredBroadcast
         *
         * @return The date created for this PaFasTriggeredBroadcast as a time_t.
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
         * Returns the date modified for this PaFasTriggeredBroadcast.
         *
         * @return The date modified for this PaFasTriggeredBroadcast as a time_t.
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
         * This should be called to determine if any part of the PaFasTriggeredBroadcast has been changed by the user.
         *
         * @return bool - This will return true if the PaFasTriggeredBroadcast does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaFasTriggeredBroadcast changes is empty then nothing has changed
            return !m_PaFasTriggeredBroadcastChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this PaFasTriggeredBroadcast is new. A new PaFasTriggeredBroadcast is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaFasTriggeredBroadcast has not been applied to the database.
         */
        virtual bool isNew();



        /**
         * invalidate
         *
         * Make the data contained by this PaFasTriggeredBroadcast as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updatePaFasTriggeredBroadcastChanges
         *
         * This updates the map recording the changes to the PaFasTriggeredBroadcast. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaFasTriggeredBroadcastChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaFasTriggeredBroadcastChanges
         *
         * This updates the map recording the changes to the PaFasTriggeredBroadcast. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePaFasTriggeredBroadcastChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PaFasTriggeredBroadcast not used so it is made private
		ConfigPaFasTriggeredBroadcast& operator=(const ConfigPaFasTriggeredBroadcast&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaFasTriggeredBroadcasts in the
                                          // Configuration Editor because new PaFasTriggeredBroadcasts do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PaFasTriggeredBroadcastHelper* m_PaFasTriggeredBroadcastHelper;


        TA_Base_Core::ItemChanges m_PaFasTriggeredBroadcastChanges; // This is a map of all changes made to the PaFasTriggeredBroadcast so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Base_Core

#endif // !defined(ConfigPaFasTriggeredBroadcast_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaZone.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaZone is a concrete implementation of IConfigPaZone, which is in turn an implementation
  * of IPaZone. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaZones.
  */


#if !defined(ConfigPaZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaZone.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{
    class PaZoneHelper;

    class ConfigPaZone : public IConfigPaZone
    {

    public:
        /**
         * ConfigPaZone (constructor)
         *
         * Constructs a new instance of the ConfigPaZone with no id. This is used when
         * creating a *new* PaZone - that is, one that does not yet exist in the database.
         */
        ConfigPaZone();


        /**
         * ConfigPaZone (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaZoneHelper::PaZoneHelper(IData*)
         *
         */
        ConfigPaZone(unsigned long row, TA_Base_Core::IData& data);

                
        /**
         * ConfigPaZone (constructor)
         *
         * Constructs a new instance of ConfigPaZone with the specified id. This is used 
         * when creating a ConfigPaZone based around an existing PaZone in the database.
         *
         * @param key The key to this PaZone in the database.
         */
        ConfigPaZone(const unsigned long idKey);


        /**
         * ConfigPaZone (copy constructor)
         *
         * @param ConfigPaZone& - The PaZone to make a copy of.
         */
        ConfigPaZone( const ConfigPaZone& theConfigPaZone);

        
        virtual ~ConfigPaZone();


    public:

        /**
         * deleteThisObject
         *
         * Removes this PaZone from the database. 
         * The calling application MUST then delete this PaZone object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaZone was initially loaded from the database
         *      OR     - writePaZoneData() has already been called
         *      This PaZone has not been deleted
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
         * @exception DataConfigurationException If the data contained in the PaZone object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaZone has not been deleted
         */
        void applyChanges();
        
        /**
         * haveStationModesChanged
         *
         * We have to have some way of maintaining whether changes have been made to the list of associated
         * StationModes or not. It is too difficult to try and manage StationMode changes in strings in 
         * m_ECSMasterModeChanges.  Instead we use the m_originalStationModes and m_newStationModees. This 
         * methods adds and removes new and old StationModes from m_newStationModes and compares the sets 
         * to determine if changes have been made.
         *
         * @param unsigned long - This is the key of the StationMode to add or remove from the associations
         * @param bool - This indicates whether this profile is to be added or removed. True if the
         *               StationMode is to be added to the associations, false if the StationMode is to be
         *               removed.
         */
        //bool haveEquipmentEntitiesChanged(const std::string& entityName, bool isAssociated);

        /**
         * getKey
         *
         * Returns the key for this PaZone.
         *
         * @return The key for this PaZone as an unsigned long.
         *
         * pre: Either - this PaZone was initially loaded from the database
         *      OR     - writePaZoneData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getId();
        virtual std::string getLabel();
        virtual std::string getStnEntityName();
        virtual std::string getOccEntityName();
        //virtual unsigned long getStatusEntityKey();
        //virtual std::list<std::string> getAssociatedEquipmentEntities();


        virtual void setLocationKey(const unsigned long locationKey);
        virtual void setId(const unsigned long id);
        virtual void setLabel(const std::string& label);
        //virtual void setStatusEntityKey(const unsigned long statusEntityKey);
        virtual void setStnEntityName(const std::string& name);
        virtual void setOccEntityName(const std::string& name);       
        //virtual void setAssociatedEquipmentEntity( const std::string& entityName, bool isAssociated);

         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Zone this is 
         * just the key in a string format e.g. "PA Zone 1".
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
         * Not applicable for PA Zones.  Will just assert.
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
            return m_paZoneChanges;
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
         * Returns the date created for this PaZone
         *
         * @return The date created for this PaZone as a time_t.
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
         * Returns the date modified for this PaZone.
         *
         * @return The date modified for this PaZone as a time_t.
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
         * This should be called to determine if any part of the PaZone has been changed by the user.
         *
         * @return bool - This will return true if the PaZone does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaZone changes is empty then nothing has changed
            return !m_paZoneChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this PaZone is new. A new PaZone is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaZone has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this PaZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();

        /**
         * createPaZoneHelperRecords (interface implementation)
         *
         * @see IConfigPaZone::createPaZoneGroupHelperRecords for more details
         */     
        //virtual void createPaZoneHelperRecords(const std::vector<std::string>& entityEquipmentKeys);        

    private:
            
	    /**
         * updatePaZoneChanges
         *
         * This updates the map recording the changes to the PaZone. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaZoneChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaZoneChanges
         *
         * This updates the map recording the changes to the PaZone. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePaZoneChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PaZone not used so it is made private
		ConfigPaZone& operator=(const ConfigPaZone&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaZones in the
                                          // Configuration Editor because new PaZones do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PaZoneHelper* m_paZoneHelper;


        TA_Base_Core::ItemChanges m_paZoneChanges; // This is a map of all changes made to the PaZone so that they are
                                       // recorded and can be sent in audit or online update messages.

        // This is a set of all the original EquipmentEntities
        // associated with this PaZone. This is used to determine
        // whether changes have been made and allows us to report
        // those changes.
        TA_Base_Core::PrimitiveWrapper<std::set<std::string> > m_originalEquipmentEntities; 

        // This is a set of all the EquipmentEntity changes. This allows us
        // to compare the sets to see if any changes have been made.
        std::set<std::string> m_modifiedEquipmentEntities; 

    };
} // closes TA_Base_Core

#endif // !defined(ConfigPaZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

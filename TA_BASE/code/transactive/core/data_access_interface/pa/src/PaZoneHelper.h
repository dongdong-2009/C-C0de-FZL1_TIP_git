/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaZoneHelper is an object that is held by PaZone and ConfigPaZone objects. 
  * It contains all data used by PaZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaZone and ConfigPaZone.
  */

#if !defined(PaZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <list>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class TA_Base_Core::IData;

    class PaZoneHelper
        : public GenericDaiHelper<PaZoneHelper>
    {
    public:
        static const std::string PA_ZONE_TABLE_NAME;
        static const std::string KEY_COL;
        static const std::string PA_ZONE_SEQUENCE_COUNTER;

        static const std::string PA_ZONE_HELPER_TABLE_NAME;
        static const std::string PA_ZONE_HELPER_KEY_COL;
        static const std::string PA_ZONE_HELPER_ENTITY_NAME_COL;
        static const std::string PA_ZONE_HELPER_ZONE_KEY;
        static const std::string PA_ZONE_HELPER_SEQUENCE_COUNTER;
    
        static const std::string LOCATIONKEY_COL;
        static const std::string ID_COL;
        static const std::string LABEL_COL;
        // static const std::string EQUIPMENTENTITYKEY_COL;
        static const std::string STN_EQUIPMENTENTITYNAME_COL;
        static const std::string OCC_EQUIPMENTENTITYNAME_COL;
        static const std::string DATEMODIFIED_COL;
        static const std::string DATECREATED_COL;

        /*
        static const std::string FOREIGN_ENTITY_TABLE_NAME;
        static const std::string FOREIGN_ENTITY_NAME_COL;
        static const std::string FOREIGN_ENTITY_KEY_COL;
        */


        /** 
         * PaZoneHelper
         *
         * This constructor creates a new PaZoneHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaZoneHelper(const unsigned long key);

        /** 
         * PaZoneHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        PaZoneHelper(unsigned long row, TA_Base_Core::IData& data);
        

        /** 
         * PaZoneHelper
         *
         * Copy constructor.
         *
         * @param   thePaZoneHelper the original PaZoneHelper to copy.
         *       
         */
        PaZoneHelper( const PaZoneHelper& thePaZoneHelper);


        /** 
         * PaZoneHelper
         *
         * This constructor creates a new PaZoneHelper for configuration, initiailising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePaZoneData() is called for the first time.
         *
         */
        PaZoneHelper();


        /** 
         * ~PaZoneHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaZoneHelper();    

        /**
         * isNew
         *
         * This returns whether this is a new PaZone.  A new PaZone is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaZone
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaZone.
         *
         * pre:    the data exists in the database
         *
         * @return the primary key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Primary Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Location Key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * getId
         *
         * Returns the ID of this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Location Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the ID should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getId();


        /**
         * getLabel
         *
         * Returns the Label of this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Operation description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Label should be a string)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getLabel();

        
        /**
         * getStnEntityName
         *
         * Returns the Station Entity Name of this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Track description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Area should be a string)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getStnEntityName();
        /// OCC implementation of getStnEntityName
        virtual std::string getOccEntityName();

        /**
         * getStatusEntityKey
         *
         * Returns the Status Entity Key of this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Location Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        //virtual unsigned long getStatusEntityKey();

        /**
         * getAssociatedEquipmentEntities
         *
         * Returns a list of PID zone equipment datanode entity names associated with 
         * this Pa Zone.
         *
         * @return list of associated PID equipment entities
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        //virtual std::list<std::string> getAssociatedEquipmentEntities();

        /**
         * setAssociatedEquipmentEntity
         *
         * Changes an EquipmentEntity association for this PaZone
         *
         * @param  entityName     the unique entity name of the PID zone equipment datanode to add or 
         *                        remove from the associations
         * @param  isAssociated   indicates whether this StationMode is to be added or removed. True if 
         *                        the EquipmentEntity is to be added to the associations, false if the 
         *                        EquipmentEntity is to be removed.
         *
         * pre: This PaZone has not been deleted
         */
        //virtual void setAssociatedEquipmentEntity( const std::string& entityName, bool isAssociated);


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PaZone. The set values are not written to the database
         * until writePaZoneData() is called.  This method should only be used by the 
         * ConfigPaZone class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the Location Key for this PaZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);


        /**
         * setId
         *
         * Sets the ID of this PaZone. The set values are not written to the database
         * until writePaZoneData() is called.  This method should only be used by the 
         * ConfigPaZone class.
         *
         * @param  id the id for this PaZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setId(const unsigned long id);


        /**
         * setLabel
         *
         * Sets the Label of this PaZone. The set values are not written to the database
         * until writePaZoneData() is called.  This method should only be used by the 
         * ConfigPaZone class.
         *
         * @param  operation the Operation description for this PaZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. label should be a string)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLabel(const std::string& label);


        /**
         * setStnEntityName
         *
         * Sets the Status Entity Name of this PaZone. The set values are not written to the database
         * until writePaZoneData() is called.  This method should only be used by the 
         * ConfigPaZone class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  statusEntityKey the Status Entity Key for this PaZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setStnEntityName(const std::string& name);
        /// OCC implementation of setStnEntityName
        virtual void setOccEntityName(const std::string& name);
        // virtual void setStatusEntityKey(const unsigned long statusEntityKey);
        

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaZone as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaZone.  If this PaZone record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZone has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaZone as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZone record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this PaZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaZoneData() has been called prior to calling
         * this method on a new PaZone, as it does not make any sense to invalidate an
         * PaZone that has not yet been written to the database.
         *
         * pre:    This PaZone was initially loaded from the database
         *         or writePaZoneData() has already been called
         */
        void invalidate();


        /**
         * writePaZoneData
         * 
         * Write this PaZone to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the PaZone record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaZone returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaZone object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaZoneData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaZone
         *
         * Remove this PaZone from the database. Once this method has been executed, the
         * PaZoneHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaZone was initially loaded from the database
         *         or writePaZoneData() has already been called
         *
         * @param  cascade
         *          set to true to force the removal of this record by removing all 
         *          references to this record
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *         - CANNOT_DELETE if references to this record exists in linked tables
         *           table.  These can be removed by specifying the cascade option.
         *
         */
        void deletePaZone(bool cascade = false);

        /**
         * associateZoneWithEquipmentEntity
         *
         * Creates a new entry in the PA Zone Helper table matching
         *      (associating) the input zone and entity names
         *
         * @param zoneKey the zone to associate with
         *
         * @param entityName the entity to associate with
         *
         * @exception DataException if there is a matching
         *          entity name/zone key pair already in the helper table table
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        //static void associateZoneWithEquipmentEntity(unsigned long zoneKey,
        //                                                const std::string& entityName);

        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         * Allows additional joinedTable parameter
         *
         * @param joinedTable (optional) the statement will perform a join
         *          with this table (if string non empty)
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static const DatabaseQueryHelper::QueryData getBasicQueryData(const std::string& joinedTable = "");

    protected:

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

    private:

        enum ELocationType
        {
            OCC,
            STN
        };

        // Made private as it is not used
		PaZoneHelper& operator=(const PaZoneHelper &);
        
        /**
         * getDbStatusEntityKeyString
         *
         * @return the status entity name, as an appropriate string value for use
         *          in an SQL update statement (that is, null if defined as unset, 
         *          otherwise the actual value)
         */
        std::string getDbStnEntityNameString() const;
        std::string getDbOccEntityNameString() const;        
        //std::string getDbStatusEntityKeyString() const;

        /**
         * loadAssociatedEquipmentEntityData
         *
         * Fills out the m_associatedEquipmentEntity array with all the
         *  entity equipment data
         *
         */
        //void loadAssociatedEquipmentEntityData();

        /**
         * validateNewZoneEntityNameUniqueness
         *
         * Checks the database to confirm the input zone key / entity name
         *  form a unique pair within the PA_ZONE_HELPER table
         *
         * @param entityName the entity to associate with
         *
         * @param zoneKey the zone to associate with
         *
         * @exception DataException if there is a matching
         *          entity name/zone key pair in our table
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        static void validateNewZoneEntityNameUniqueness(unsigned long zoneKey,
                                                        const std::string& entityName);
        
        /**
         * modifyExistingPaZone
         *
         * This will update an existing PaZone in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingPaZone(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaZone
         *
         * This will add a new PaZone in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         *
         */
        void addNewPaZone(); //throw(TA_Core::TransactiveException);

        /**
         * updateEquipmentEntityAssociations
         *
         * This removes and adds EquipmentEntities from/to the database to get 
         // it up-to-date with the user's changes.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.
         */
        //void updateEquipmentEntityAssociations();
        
        /**
         * validateIdLocationPairUniqueness
         *
         * Checks the database to confirm the ID and Location parameters
         * of this Pa Zone do not match any existing item in the database
         *
         * @exception DataConfigurationException if there is a matching
         *          id/location pair in the Pa Zone table
         *
         * @throws DataException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        void validateIdLocationPairUniqueness();

        /**
         * validateEquipmentEntityNameUniqueness
         *
         * Checks the database to confirm the  Equipment Entity Name parameters
         * of this Pa Zone do not match any existing item in the database
         *
         * @param locationType defines the type of location we're checking uniqueness for
         *
         * @exception DataConfigurationException if there is a matching
         *          id/Equipment Entity Key pair in the Pa Zone table
         *
         * @throws DataException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        void validateEquipmentEntityNameUniqueness(ELocationType locationType);
      
        /**
         * validateLocationKeyExistence
         *
         * @throws  DataException
         *          - If some of the required internal values for this zone have not been set
         *
         */
        void validateLocationKeyExistence(); //throw(TA_Core::TransactiveException);

        /**
         * validateEquipmentEntityExistence
         *
         * @throws  DataException
         *          - If some of the required internal values for this zone have not been set
         *
         */
        void validateEquipmentEntityExistence(const std::string& equipmentEntity);
            //throw(TA_Core::TransactiveException);
        
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key; }

        unsigned long m_key;
        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;        // Mustn't be 'unset', all table columns non null
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_id;                 // Mustn't be 'unset', all table columns non null
  		std::string                     m_label;
        //PrimitiveWrapper<unsigned long> m_statusEntityKey;    // Mustn't be 'unset', all table columns non null
        
        // Foreign data extracted from ENTITY table
        std::string                     m_stnEquipmentEntityName;
        std::string                     m_occEquipmentEntityName;
        
        time_t                          m_dateCreated;
        time_t                          m_dateModified;
        bool                            m_isValidData;
        bool                            m_isNew;

        //std::list<std::string>          m_associatedEquipmentEntity;
        //bool                            m_associatedEquipmentEntitySet;
    };
} // closes TA_Base_Core

#endif // !defined(PaZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

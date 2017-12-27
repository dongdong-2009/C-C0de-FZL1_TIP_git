/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneGroupHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaZoneGroupHelper is an object that is held by PaZoneGroup and ConfigPaZoneGroup objects. 
  * It contains all data used by PaZoneGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaZoneGroup and ConfigPaZoneGroup.
  * Additionally, it can be used to query and update the PA_ZONE_GROUP_HELPER database table,
  * as there is no need to have a separate object based database interface for this table
  */

#if !defined(PaZoneGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaZoneGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/IPaZone.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaZoneGroupHelper
        : public GenericDaiHelper<PaZoneGroupHelper>
    {

    public:
        // Defined here so other classes may access column names
        static const std::string KEY_COL;
        static const std::string LOCATIONKEY_COL;
        static const std::string LABEL_COL;
        static const std::string ISEVENTGROUP_COL;
        static const std::string DATEMODIFIED_COL;
        static const std::string DATECREATED_COL;


        /** 
         * PaZoneGroupHelper
         *
         * This constructor creates a new PaZoneGroupHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaZoneGroupHelper(const unsigned long key);

        
        /** 
         * PaZoneGroupHelper
         *
         * Copy constructor.
         *
         * @param   thePaZoneGroupHelper the original PaZoneGroupHelper to copy.
         *       
         */
        PaZoneGroupHelper( const PaZoneGroupHelper& thePaZoneGroupHelper);

        /** 
         * PaZoneGroupHelper
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
        PaZoneGroupHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaZoneGroupHelper
         *
         * This constructor creates a new PaZoneGroupHelper for configuration, initialising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePaZoneGroupData() is called for the first time.
         *
         */
        PaZoneGroupHelper();


        /** 
         * ~PaZoneGroupHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaZoneGroupHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaZoneGroup.  A new PaZoneGroup is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaZoneGroup
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaZoneGroup.
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
         *           (e.g. the key should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this PaZoneGroup.  If this PaZoneGroup record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZoneGroup has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * getLabel
         *
         * Returns the Label of this PaZoneGroup.  If this PaZoneGroup record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZoneGroup has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getLabel();
        

        /**
         * getIsEventGroup
         *
         * Returns the IsEventGroup state of this PaZoneGroup.  If this PaZoneGroup record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZoneGroup has been requested, all the data is loaded from the database.
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
         *           (e.g. the IsEventGroup property should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        virtual unsigned long getIsEventGroup();

        
        /**
         * setLocationKey
         *
         * Sets the Location Key of this PaZoneGroup. The set values are not written to the database
         * until writePaZoneGroupData() is called.  This method should only be used by the 
         * ConfigPaZoneGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PaZoneGroup
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);


        /**
         * setLabel
         *
         * Sets the Label of this PaZoneGroup. The set values are not written to the database
         * until writePaZoneGroupData() is called.  This method should only be used by the 
         * ConfigPaZoneGroup class.
         *
         * @param  operation the Operation description for this PaZoneGroup
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. label should be a string)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLabel(const std::string& label);


        /**
         * setIsEventGroup
         *
         * Sets the IsEventGroup status of this PaZoneGroup. The set values are not written to the database
         * until writePaZoneGroupData() is called.  This method should only be used by the 
         * ConfigPaZoneGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  condition the Condition description for this PaZoneGroup
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. IsEventGroup variable should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setIsEventGroup(const unsigned long isEventGroup);        

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaZoneGroup.  If this PaZoneGroup record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZoneGroup has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaZoneGroup as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaZoneGroup.  If this PaZoneGroup record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaZoneGroup has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaZoneGroup as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        /**
         * getAssociatedPaZones
         *
         * Returns the PaZones associated with this Pa Zone Group - none of them
         *      will have the entity equipment association data loaded for them
         *
         * @return The PaZones in this group
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual IPaZones getAssociatedPaZones();

        /**
         * invalidate
         *
         * Make the data contained by this PaZoneGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaZoneGroupData() has been called prior to calling
         * this method on a new PaZoneGroup, as it does not make any sense to invalidate an
         * PaZoneGroup that has not yet been written to the database.
         *
         * pre:    This PaZoneGroup was initially loaded from the database
         *         or writePaZoneGroupData() has already been called
         */
        void invalidate();


        /**
         * writePaZoneGroupData
         * 
         * Write this PaZoneGroup to the database.
         * 
         * @param recreateModified - if the zone already exists (isNew() is false), and
         *          there is an attempt to update the existing record, but it is not found
         *          (because an external source has deleted it), then we automatically
         *          regenerate it using our key value
         * 
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - NO_VALUE if the PaZoneGroup record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaZoneGroup returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaZoneGroup object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaZoneGroupData(bool recreateModified = false); //throw(TA_Core::TransactiveException);


        /**
         * associateGroupWithZones
         *
         * Creates a new entry in the PA Zone Group helper table matching
         *      (associating) the input group and zone records
         *
         * @param groupKey the group to associate with
         *
         * @param zoneKeys the zones to associate with
         *
         * @exception DataException if there is a matching
         *          group ID/zone key pair in the helper table table
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         * @see removeGroupZoneAssociations for inverse procedure
         *
         */
        static void associateGroupWithZones(unsigned long groupKey,
                                                const std::list<unsigned long>& zoneKeys);

        /**
         * removeGroupZoneAssociation
         *
         * Removes a set of entries in the PA Zone Group helper table matching
         *     the input group and zone identifiers
         *
         * @param groupKey the group to remove associations from
         *
         * @param zoneKeys the zones to remove associations
         *
         * @exception DataException if there is a matching
         *          group ID/zone key pair in the helper table table
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         * @see associateGroupWithZones for inverse procedure
         *
         */
        static void removeGroupZoneAssociations(unsigned long groupKey,
                                                const std::list<unsigned long>& zoneKeys);


        /**
         * deletePaZoneGroup
         *
         * Remove this PaZoneGroup from the database. Once this method has been executed, the
         * PaZoneGroupHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaZoneGroup was initially loaded from the database
         *         or writePaZoneGroupData() has already been called
         *
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *
         */
        void deletePaZoneGroup();
	
        /**
         * destroyGroupZoneAssociations
         *
         * Destroys all PA_ZONE_GROUP_HELPER objects associated
         * with the PA_ZONE_GROUP identified by the input key
         *
         * @param groupKey the primary key identifying the record
         *      in the PA_ZONE_GROUP table that we want to remove
         *      zone associations from
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException if there is an error while modifying the data
         *
         */
        void destroyGroupZoneAssociations(unsigned long groupKey);
        
        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static DatabaseQueryHelper::QueryData getBasicQueryData(const std::string& joinedTable = "");

    protected:

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

    private:

        // Made private as it is not used
		PaZoneGroupHelper& operator=(const PaZoneGroupHelper &);

        /**
         * validateNewGroupIdZoneKeyUniqueness
         *
         * Checks the database to confirm the input group Id and zone key
         *  form a unique pair within the PA_ZONE_GROUP_HELPER table
         *
         * @param groupKey the group to associate with
         *
         * @param zoneKey the zone to associate with
         *
         * @exception DataException if there is a matching
         *          group ID/zone key pair in our table
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        static void validateNewGroupIdZoneKeyUniqueness(unsigned long groupId,
                                                        unsigned long zoneKey);

        /**
         * validateLabelLocationPairUniqueness
         *
         * Checks the database to confirm the label and Location parameters
         * of this record do not match any existing (similar) item in the database
         *
         * @exception DataException if there is a matching
         *          label/location pair in our table
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        void validateLabelLocationPairUniqueness();


        /**
         * validateLabelUniqueness
         *
         * Checks the database to confirm the label
         * of this record do not match any existing (similar) item in the database
         *
         * @exception DataException if there is a matching
         *          label pair in our table
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        void validateLabelUniqueness();

        
        /**
         * modifyExistingPaZoneGroup
         *
         * This will update an existing PaZoneGroup in the database with the internals currently 
         * set in this instance.
         *
         * @param recreateModified - @see writePaZoneGroupData for definition of parameter
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaZoneGroup object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingPaZoneGroup(bool recreateModified); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaZoneGroup
         *
         * This will add a new PaZoneGroup in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaZoneGroup object is not sufficent to create an
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
        void addNewPaZoneGroup(); //throw(TA_Core::TransactiveException);

        
        /**
         * validateLocationKeyExistence
         *
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         *
         */
        void validateLocationKeyExistence(); //throw(TA_Core::TransactiveException);
        
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key; }

        /**
         * addNewPaZoneGroupUsingKey
         *
         * Adds a new PA Zone group record using the key provided
         *
         * @param databaseConnection
         * @param keyToUse the primary key of the zone group to be used when inserting the new record
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaZoneGroup object is not sufficent to create an
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
        void addNewPaZoneGroupUsingKey(TA_Base_Core::IDatabase* databaseConnection, unsigned long keyToUse);

        // For DDB to get the location DB name
        static std::string getLocalDatabaseName();
        
        // The name of the local database to send updates to
        static std::string m_localDatabase;

        unsigned long m_key;

		// marvin++ TD8110
		// Implementation of copy functionality
		unsigned long m_copiedKey;
		bool          m_isCopiedKeySet;
		// ++marvin TD8110
        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;     // Mustn't be 'unset', all table columns non null
        std::string m_label;
        // Mustn't be 'unset', all table columns non null (but this does default to 0)
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_isEventGroup;    
        
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Base_Core

#endif // !defined(PaZoneGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

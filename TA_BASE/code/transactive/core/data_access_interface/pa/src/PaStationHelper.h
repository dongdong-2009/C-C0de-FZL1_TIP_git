/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaStationHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaStationHelper is an object that is held by PaStation and ConfigPaStation objects. 
  * It contains all data used by PaStations, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaStation and ConfigPaStation.
  */

#if !defined(PaStationHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaStationHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaStationHelper
        : public GenericDaiHelper<PaStationHelper>
    {

    public:


        /** 
         * PaStationHelper
         *
         * This constructor creates a new PaStationHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaStationHelper(const unsigned long key);

        
        /** 
         * PaStationHelper
         *
         * Copy constructor.
         *
         * @param   thePaStationHelper the original PaStationHelper to copy.
         *       
         */
        PaStationHelper( const PaStationHelper& thePaStationHelper);

        /** 
         * PaStationHelper
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
        PaStationHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaStationHelper
         *
         * This constructor creates a new PaStationHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writePaStationData() is called for the first time.
         *
         */
        PaStationHelper();


        /** 
         * ~PaStationHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaStationHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaStation.  A new PaStation is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaStation
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        /**
         * getKey
         *
         * Returns the primary key of this PaStation.
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
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();

        /**
         * getLocationKey
         *
         * Returns the Location Key of this PaStation.  If this PaStation record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaStation has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PaStation. The set values are not written to the database
         * until writePaStationData() is called.  This method should only be used by the 
         * ConfigPaStation class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PaStation
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);


        /**
         * setKey
         *
         * Sets the ID of this PaStation. The set values are not written to the database
         * until writePaStationData() is called.  This method should only be used by the 
         * ConfigPaStation class.
         *
         * @param  id the id for this PaStation
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setKey(const unsigned long id);


        /**
         * setKeyToUse
         *
         * Sets the key to be used when writing new objects to the database
         *
         * @param keyToUse the key to use when updating / inserting into database
         *
         */
        virtual void setKeyToUse(unsigned long keyToUse);


        /**
         * getDateCreated
         *
         * Returns the date created for this PaStation.  If this PaStation record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaStation has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaStation as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaStation.  If this PaStation record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaStation has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaStation as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this PaStation as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaStationData() has been called prior to calling
         * this method on a new PaStation, as it does not make any sense to invalidate an
         * PaStation that has not yet been written to the database.
         *
         * pre:    This PaStation was initially loaded from the database
         *         or writePaStationData() has already been called
         */
        void invalidate();


        /**
         * writePaStationData
         * 
         * Write this PaStation to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the PaStation record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaStation returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaStation object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaStationData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaStation
         *
         * Remove this PaStation from the database. Once this method has been executed, the
         * PaStationHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaStation was initially loaded from the database
         *         or writePaStationData() has already been called
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
        void deletePaStation(bool cascade = false);
	
        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static DatabaseQueryHelper::QueryData getBasicQueryData();

        /**
         * getModifiedKey
         *
         * @return the modified (if applicable) primary key, but if the
         *          key hasn't been modified, returns the actual primary key
         *
         */
        unsigned long getModifiedKey();

        /** 
         * isKeyDuplicate
         * 
         * Checks the database to confirm the user defined key value
         *  doesn't already exist in the database
         *
         * @return true if this key already present in database, false otherwise
         *
         * @param keyToUse the key to perform the search through table with
         *
         */       
        static bool isKeyDuplicate(unsigned long keyToUse);

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
		PaStationHelper& operator=(const PaStationHelper &);

        
        /**
         * validateIdLocationPairUniqueness
         *
         * Checks the database to confirm the ID and Location parameters
         * of this record do not match any existing (similar) item in the database
         *
         * @exception DataConfigurationException if there is a matching
         *          id/location pair in our table
         *
         * @throws DataException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        void validateLocationUniqueness();

        
        /**
         * modifyExistingPaStation
         *
         * This will update an existing PaStation in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaStation object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingPaStation(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaStation
         *
         * This will add a new PaStation in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaStation object is not sufficent to create an
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
        void addNewPaStation(); //throw(TA_Core::TransactiveException);
        
        /**
         * validateLocationKeyExistence
         *
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         *
         */
        void validateLocationKeyExistence(); //throw(TA_Core::TransactiveException);
                
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }

        void validateKeyExistence(); //throw(TA_Core::TransactiveException)
        void validateKeyAbsence(); //throw(TA_Core::TransactiveException)






        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;
        // The key we want to change to (if at all)
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_modifiedKey;

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;        // Mustn't be 'unset', all table columns non null

        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(PaStationHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisZoneMapHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaTisZoneMapHelper is an object that is held by PaTisZoneMap and ConfigPaTisZoneMap objects. 
  * It contains all data used by PaTisZoneMaps, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaTisZoneMap and ConfigPaTisZoneMap.
  */

#ifndef PATISZONEMAPHELPER_INCLUDED_
#define PATISZONEMAPHELPER_INCLUDED_

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
    class PaTisZoneMapHelper
    {

    public:


        /** 
         * PaTisZoneMapHelper
         *
         * This constructor creates a new PaTisZoneMapHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaTisZoneMapHelper(const unsigned long key);

        
        /** 
         * PaTisZoneMapHelper
         *
         * Copy constructor.
         *
         * @param   thePaTisZoneMapHelper the original PaTisZoneMapHelper to copy.
         *       
         */
        PaTisZoneMapHelper( const PaTisZoneMapHelper& thePaTisZoneMapHelper);

        /** 
         * PaTisZoneMapHelper
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
        PaTisZoneMapHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaTisZoneMapHelper
         *
         * This constructor creates a new PaTisZoneMapHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writePaTisZoneMapData() is called for the first time.
         *
         */
        PaTisZoneMapHelper();


        /** 
         * ~PaTisZoneMapHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaTisZoneMapHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaTisZoneMap.  A new PaTisZoneMap is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaTisZoneMap
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaTisZoneMap.
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
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getId
         *
         * Returns the ID of this PaTisZoneMap.  If this PaTisZoneMap record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTisZoneMap has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getPaZoneKey();


        /**
         * getTisMessageTag
         *
         * Returns the TIS Message Tag of this PaTisZoneMap.  If this PaTisZoneMap record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTisZoneMap has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        virtual std::string getTisZoneEntity();


        /**
         * setId
         *
         * Sets the ID of this PaTisZoneMap. The set values are not written to the database
         * until writePaTisZoneMapData() is called.  This method should only be used by the 
         * ConfigPaTisZoneMap class.
         *
         * @param  id the id for this PaTisZoneMap
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setPaZoneKey(const unsigned long id);


        /**
         * setTisMessageTag
         *
         * Sets the TIS Message Tag of this PaTisZoneMap. The set values are not written to the database
         * until writePaTisZoneMapData() is called.  This method should only be used by the 
         * ConfigPaTisZoneMap class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  tisMessageTag the new value for the tisMessageTag property of this PaTisZoneMap
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setTisZoneEntity(const std::string& strTisZoneEntity);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaTisZoneMap.  If this PaTisZoneMap record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTisZoneMap has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaTisZoneMap as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaTisZoneMap.  If this PaTisZoneMap record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTisZoneMap has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaTisZoneMap as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this PaTisZoneMap as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaTisZoneMapData() has been called prior to calling
         * this method on a new PaTisZoneMap, as it does not make any sense to invalidate an
         * PaTisZoneMap that has not yet been written to the database.
         *
         * pre:    This PaTisZoneMap was initially loaded from the database
         *         or writePaTisZoneMapData() has already been called
         */
        void invalidate();


        /**
         * writePaTisZoneMapData
         * 
         * Write this PaTisZoneMap to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the PaTisZoneMap record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaTisZoneMap returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaTisZoneMap object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaTisZoneMapData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaTisZoneMap
         *
         * Remove this PaTisZoneMap from the database. Once this method has been executed, the
         * PaTisZoneMapHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaTisZoneMap was initially loaded from the database
         *         or writePaTisZoneMapData() has already been called
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
        void deletePaTisZoneMap(bool cascade = false);
	

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
		PaTisZoneMapHelper& operator=(const PaTisZoneMapHelper &);

        

        
        /**
         * modifyExistingPaTisZoneMap
         *
         * This will update an existing PaTisZoneMap in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaTisZoneMap object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingPaTisZoneMap(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaTisZoneMap
         *
         * This will add a new PaTisZoneMap in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaTisZoneMap object is not sufficent to create an
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
        void addNewPaTisZoneMap(); //throw(TA_Core::TransactiveException);


        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key; }


        unsigned long m_key;

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulPaZoneKey;                 // Mustn't be 'unset', all table columns non null
        std::string m_strTisZoneEntity;    // Allowed to be null

        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    private:

		void reload();

    };
} // closes TA_Base_Core

#endif // !defined(PaTisZoneMapHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

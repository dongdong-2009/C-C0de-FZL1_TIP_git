/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaDvaMessageHelper is an object that is held by PaDvaMessage and ConfigPaDvaMessage objects. 
  * It contains all data used by PaDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaDvaMessage.
  */

#if !defined(PaDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class PaDvaMessageHelper
    {

    public:


        /** 
         * PaDvaMessageHelper
         *
         * This constructor creates a new PaDvaMessageHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaDvaMessageHelper(const unsigned long key);

        
        /** 
         * PaDvaMessageHelper
         *
         * Copy constructor.
         *
         * @param   thePaDvaMessageHelper the original PaDvaMessageHelper to copy.
         *       
         */
        PaDvaMessageHelper( const PaDvaMessageHelper& thePaDvaMessageHelper);

        /** 
         * PaDvaMessageHelper
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
        PaDvaMessageHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaDvaMessageHelper
         *
         * This constructor creates a new PaDvaMessageHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writePaDvaMessageData() is called for the first time.
         *
         */
        PaDvaMessageHelper();


        /** 
         * ~PaDvaMessageHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessageHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaDvaMessage.  A new PaDvaMessage is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaDvaMessage
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaDvaMessage.
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
         * Returns the Location Key of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * getId
         *
         * Returns the ID of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getId();


        /**
         * getLabel
         *
         * Returns the Label of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getLabel();


        /**
         * getDescription
         *
         * Returns the Description of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getDescription();

        
        /**
         * getType
         *
         * Returns the Type of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned short getType();

        /**
         * getTisMessageTag
         *
         * Returns the TIS Message Tag of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        //virtual unsigned long getTisMessageTag();


        /**
         * getTisLibrarySection
         *
         * Returns the TIS Library Section of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        //virtual unsigned long getTisLibrarySection();


        /**
         * getTisLibraryVersion
         *
         * Returns the TIS Library version of this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *         
         */
        //virtual unsigned long getTisLibraryVersion();

        
        /**
         * setLocationKey
         *
         * Sets the Location Key of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);


        /**
         * setId
         *
         * Sets the ID of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.
         *
         * @param  id the id for this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setId(const unsigned long id);


        /**
         * setLabel
         *
         * Sets the Label of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.
         *
         * @param  operation the Operation description for this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. label should be a string)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLabel(const std::string& label);


        /**
         * setDescription
         *
         * Sets the Description of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.
         *
         * @param  operation the Operation description for this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. label should be a string)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setDescription(const std::string& description);


        /**
         * setType
         *
         * Sets the Area of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.  
         *
         * @param  type the new Type for this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be a string)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setType(const unsigned short type);


        /**
         * setTisMessageTag
         *
         * Sets the TIS Message Tag of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  tisMessageTag the new value for the tisMessageTag property of this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        //virtual void setTisMessageTag(const unsigned long tisMessageTag);
        
        /**
         * setTisLibrarySection
         *
         * Sets the TIS Library Section of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  tisLibrarySection the new value for the tisLibrarySection property of this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        //virtual void setTisLibrarySection(const unsigned long tisLibrarySection);

        /**
         * setTisLibraryVersion
         *
         * Sets the TIS Library version of this PaDvaMessage. The set values are not written to the database
         * until writePaDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaDvaMessage class.  For data to be correctly written to the database, the referenced 
         * PKEY in the ENTITY table must exist.
         *
         * @param  version the new value for the TIS Library version of this PaDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Status Entity Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        //virtual void setTisLibraryVersion(const unsigned long version);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaDvaMessage as a time_t.
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
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessage.  If this PaDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessage has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaDvaMessage as a time_t.
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
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this PaDvaMessage as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaDvaMessageData() has been called prior to calling
         * this method on a new PaDvaMessage, as it does not make any sense to invalidate an
         * PaDvaMessage that has not yet been written to the database.
         *
         * pre:    This PaDvaMessage was initially loaded from the database
         *         or writePaDvaMessageData() has already been called
         */
        void invalidate();


        /**
         * writePaDvaMessageData
         * 
         * Write this PaDvaMessage to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaDvaMessage returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaDvaMessage object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaDvaMessageData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaDvaMessage
         *
         * Remove this PaDvaMessage from the database. Once this method has been executed, the
         * PaDvaMessageHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaDvaMessage was initially loaded from the database
         *         or writePaDvaMessageData() has already been called
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
        void deletePaDvaMessage(bool cascade = false);

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
		PaDvaMessageHelper& operator=(const PaDvaMessageHelper &);

        
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
        void validateIdLocationPairUniqueness();

        
        /**
         * modifyExistingPaDvaMessage
         *
         * This will update an existing PaDvaMessage in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessage object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingPaDvaMessage(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaDvaMessage
         *
         * This will add a new PaDvaMessage in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessage object is not sufficent to create an
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
        void addNewPaDvaMessage(); //throw(TA_Core::TransactiveException);
        
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
        virtual unsigned long getKeyDirect() const { return m_key; }


        unsigned long m_key;
        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;        // Mustn't be 'unset', all table columns non null
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_id;                 // Mustn't be 'unset', all table columns non null
  		std::string m_label;
        std::string m_description;
        TA_Base_Core::PrimitiveWrapper<unsigned short> m_type;
        //TA_Base_Core::PrimitiveWrapper<unsigned long> m_tisMessageTag;    // Allowed to be null
        //TA_Base_Core::PrimitiveWrapper<unsigned long> m_tisLibrarySection;    // Allowed to be null
        //TA_Base_Core::PrimitiveWrapper<unsigned long> m_tisLibraryVersion;    // Null allowed

        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

		std::string m_localDatabase;

    private:

		void reload();

		std::string getLocalDatabaseName();

        ///**
        // * getTisMessageTagDatabaseRepresentation
        // *
        // * This function will return a suitable database representation of the m_tisMessageTag value
        // *
        // * @return null if the internal value is unset, otherwise the actual value
        // *
        // */
        //std::string getTisMessageTagDatabaseRepresentation();

        ///**
        // * getTisLibrarySectionDatabaseRepresentation
        // *
        // * This function will return a suitable database representation of the m_tisLibrarySection value
        // *
        // * @return null if the internal value is unset, otherwise the actual value
        // *
        // */
        //std::string getTisLibrarySectionDatabaseRepresentation();

        ///**
        // * getTisLibraryVersionDatabaseRepresentation
        // *
        // * This function will return a suitable database representation of the 
        // *  m_tisLibraryVersion value
        // *
        // * @return null if the internal value is unset, otherwise the actual value
        // *
        // */
        //std::string getTisLibraryVersionDatabaseRepresentation();
    };
} // closes TA_Base_Core

#endif // !defined(PaDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

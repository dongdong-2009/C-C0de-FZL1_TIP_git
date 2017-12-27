/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageVersionHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaDvaMessageVersionHelper is an object that is held by PaDvaMessageVersion and ConfigPaDvaMessageVersion objects. 
  * It contains all data used by PaDvaMessageVersions, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessageVersion and ConfigPaDvaMessageVersion.
  *
  */

#if !defined(PaDvaMessageVersionHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaDvaMessageVersionHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/IPaDvaMessageVersion.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{

    class PaDvaMessageVersionHelper
        : public GenericDaiHelper<PaDvaMessageVersionHelper>
    {

    public:

        /** 
         * PaDvaMessageVersionHelper
         *
         * This constructor creates a new PaDvaMessageVersionHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaDvaMessageVersionHelper(const unsigned long key);

        
        /** 
         * PaDvaMessageVersionHelper
         *
         * Copy constructor.
         *
         * @param   thePaDvaMessageVersionHelper the original PaDvaMessageVersionHelper to copy.
         *       
         */
        PaDvaMessageVersionHelper( const PaDvaMessageVersionHelper& thePaDvaMessageVersionHelper);

        /** 
         * PaDvaMessageVersionHelper
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
        PaDvaMessageVersionHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaDvaMessageVersionHelper
         *
         * This constructor creates a new PaDvaMessageVersionHelper for configuration, initialising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePaDvaMessageVersionData() is called for the first time.
         *
         */
        PaDvaMessageVersionHelper();


        /** 
         * ~PaDvaMessageVersionHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessageVersionHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaDvaMessageVersion.  A new PaDvaMessageVersion is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaDvaMessageVersion
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaDvaMessageVersion.
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
         *         - NO_VALUE if the PaDvaMessageVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this PA DVA Message Version.  If this 
         * PA DVA Message Version record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PA DVA Message Version has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();

        /**
         * getVersionData
         *
         * Determines what version is associated with the input message Id
         *      
         * @param location the location the query originated from
         *
         * @param stationDvaMessageId the id of the station DVA Message
         *
         * @return a string representation of the version
         *
         */
        std::string getVersionData();


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PA DVA Message Version. The set values are not written to the database
         * until writePaDvaMessageVersionGroupData() is called.  This method should only be used by the 
         * ConfigPaDvaMessageVersionGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PA DVA Message Version
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);

        /**
         * setVersionData
         *
         * Sets new version data for this PA DVA Message Version set. The values are not written to the database
         * until writePaDvaMessageVersionGroupData() is called.  This method should only be used by the 
         * ConfigPaDvaMessageVersionGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param newVersion the new Location Key for this PA DVA Message Version
         *
         * @param column the specific version column we want to update
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setVersionData(const std::string& newVersion);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessageVersion.  If this PaDvaMessageVersion record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessageVersion has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaDvaMessageVersion as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessageVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessageVersion.  If this PaDvaMessageVersion record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessageVersion has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaDvaMessageVersion as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessageVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


        /**
         * invalidate
         *
         * Make the data contained by this PaDvaMessageVersion as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaDvaMessageVersionData() has been called prior to calling
         * this method on a new PaDvaMessageVersion, as it does not make any sense to invalidate an
         * PaDvaMessageVersion that has not yet been written to the database.
         *
         * pre:    This PaDvaMessageVersion was initially loaded from the database
         *         or writePaDvaMessageVersionData() has already been called
         */
        void invalidate();


        /**
         * writePaDvaMessageVersionData
         * 
         * Write this PaDvaMessageVersion to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - For new messages, the primary key hasn't been set or is non unique
         *          - NO_VALUE if the PaDvaMessageVersion record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaDvaMessageVersion returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaDvaMessageVersion object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaDvaMessageVersionData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaDvaMessageVersion
         *
         * Remove this PaDvaMessageVersion from the database. Once this method has been executed, the
         * PaDvaMessageVersionHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaDvaMessageVersion was initially loaded from the database
         *         or writePaDvaMessageVersionData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *
         */
        void deletePaDvaMessageVersion();

        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static DatabaseQueryHelper::QueryData getBasicQueryData();

    protected:

		std::string getLocalDatabaseName();

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
	
    private:

        // Made private as it is not used
		PaDvaMessageVersionHelper& operator=(const PaDvaMessageVersionHelper &);
        
        /**
         * modifyExistingPaDvaMessageVersion
         *
         * This will update an existing PaDvaMessageVersion in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessageVersion object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         */
        void modifyExistingPaDvaMessageVersion(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaDvaMessageVersion
         *
         * This will add a new PaDvaMessageVersion in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessageVersion object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         *
         */
        void addNewPaDvaMessageVersion(); //throw(TA_Core::TransactiveException);
      

        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }
        
        // The actual key (as returned from database - or set by user)        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;     

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;

        std::string m_strVersion;
        std::string m_localDatabase;

        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(PaDvaMessageVersionHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

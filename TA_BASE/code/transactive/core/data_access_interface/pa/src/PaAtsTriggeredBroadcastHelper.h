/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcastHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaAtsTriggeredBroadcastHelper is an object that is held by PaAtsTriggeredBroadcast and ConfigPaAtsTriggeredBroadcast objects. 
  * It contains all data used by PaAtsTriggeredBroadcasts, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaAtsTriggeredBroadcast and ConfigPaAtsTriggeredBroadcast.
  * Additionally, it can be used to query and update the PA_ZONE_GROUP_HELPER database table,
  * as there is no need to have a separate object based database interface for this table
  *
  */

#if !defined(PaAtsTriggeredBroadcastHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaAtsTriggeredBroadcastHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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

    class PaAtsTriggeredBroadcastHelper
        : public GenericDaiHelper<PaAtsTriggeredBroadcastHelper>
    {

    public:


        /** 
         * PaAtsTriggeredBroadcastHelper
         *
         * This constructor creates a new PaAtsTriggeredBroadcastHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaAtsTriggeredBroadcastHelper(const unsigned long key);

        
        /** 
         * PaAtsTriggeredBroadcastHelper
         *
         * Copy constructor.
         *
         * @param   thePaAtsTriggeredBroadcastHelper the original PaAtsTriggeredBroadcastHelper to copy.
         *       
         */
        PaAtsTriggeredBroadcastHelper( const PaAtsTriggeredBroadcastHelper& thePaAtsTriggeredBroadcastHelper);

        /** 
         * PaAtsTriggeredBroadcastHelper
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
        PaAtsTriggeredBroadcastHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaAtsTriggeredBroadcastHelper
         *
         * This constructor creates a new PaAtsTriggeredBroadcastHelper for configuration, initialising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writePaAtsTriggeredBroadcastData() is called for the first time.
         *
         */
        PaAtsTriggeredBroadcastHelper();


        /** 
         * ~PaAtsTriggeredBroadcastHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaAtsTriggeredBroadcastHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaAtsTriggeredBroadcast.  A new PaAtsTriggeredBroadcast is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaAtsTriggeredBroadcast
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaAtsTriggeredBroadcast.
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
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();

        /**
         * getAtsEventType
         *
         * Returns the Ats Event Type of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the ATS Event Type
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ATS Event Type should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getAtsEventType();

        /**
         * getLocationKey
         *
         * Returns the Location Key of this PaAtsTriggeredBroadcast.  If this PaAtsTriggeredBroadcast record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * getDestinationLocationKey
         *
         * Returns the Destination Location Key of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Destination Location Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Destination Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getDestinationLocationKey();

        /**
         * getPlatformId
         *
         * Returns the Platform ID of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Platform ID
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Platform ID should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getPlatformId();

        /**
         * getIsLastTrain
         *
         * Returns the Is Last Train State of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Is Last Train State
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Is Last Train State should be a boolean)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual bool getIsLastTrain();

        /**
         * getPaZoneGroupKey
         *
         * Returns the PA Zone Group foreign key of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the PA Zone Group foreign key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PA Zone Group foreign key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getPaZoneGroupKey();

        /**
         * getPaDvaMessageKey
         *
         * Returns the PA DVA Message foreign key of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the PA DVA Message foreign key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PA DVA Message foreign key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getPaDvaMessageKey();

        /**
         * getIsEnabledParameter
         *
         * Returns the Is Enabled Parameter Value of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Is Enabled Parameter Value
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Is Enabled Parameter Value should be a boolean)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual bool getIsEnabledParameter();


        /**
         * getDvaMessageVersion
         *
         * Returns the DVA Message Version of this PaAtsTriggeredBroadcast.  
         * If this PaAtsTriggeredBroadcast record does not exist in the
         * database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is 
         * loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the DVA Message Version
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. DVA Message Version should be a string)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getDvaMessageVersion();


        /**
         * setAtsEventType
         *
         * Sets the ATS Event Type of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  eventType the new ATS Event Type for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ATS Event Type should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setAtsEventType(const unsigned long eventType);


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);

        
        /**
         * setDestinationLocationKey
         *
         * Sets the Destination Location Key of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  key the new Destination Location Key for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Destination Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setDestinationLocationKey(const unsigned long key);

        /**
         * setPlatformId
         *
         * Sets the Platform ID of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  platformId the new Platform ID for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Platform ID should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setPlatformId(const unsigned long platformId);

        /**
         * setIsLastTrain
         *
         * Sets the Is Last Train State of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  isEnabled the new Is Last Train State for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Is Last Train State should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setIsLastTrain(bool isEnabled);

        /**
         * setPaZoneGroupKey
         *
         * Sets the PA Zone Group Foreign Key of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  key the new PA Zone Group Foreign Key for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PA Zone Group Foreign Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setPaZoneGroupKey(const unsigned long key);

        /**
         * setPaDvaMessageKey
         *
         * Sets the PA DV Message Foreign Key of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  key the new PA DV Message Foreign Key for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PA DVA Message Foreign Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setPaDvaMessageKey(const unsigned long key);

        /**
         * setIsEnabledParameter
         *
         * Sets the Is Enabled Parameter State of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  isEnabled the new Is Enabled Parameter State for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Is Enabled Parameter State should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setIsEnabledParameter(bool isEnabled);

        /**
         * setDvaMessageVersion
         *
         * Sets the DVA Message Version of this PaAtsTriggeredBroadcast. The set values are not written to the database
         * until writePaAtsTriggeredBroadcastData() is called.  This method should only be used by the 
         * ConfigPaAtsTriggeredBroadcast class.  For data to be correctly written to the database, the referenced 
         * PKEY in the relevant table must exist.
         *
         * @param  version the new DVA Message Version for this PaAtsTriggeredBroadcast
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. DVA Message Version should be a string)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setDvaMessageVersion(const std::string& version);
        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaAtsTriggeredBroadcast.  If this PaAtsTriggeredBroadcast record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaAtsTriggeredBroadcast as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaAtsTriggeredBroadcast.  If this PaAtsTriggeredBroadcast record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaAtsTriggeredBroadcast has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaAtsTriggeredBroadcast as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


        /**
         * invalidate
         *
         * Make the data contained by this PaAtsTriggeredBroadcast as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaAtsTriggeredBroadcastData() has been called prior to calling
         * this method on a new PaAtsTriggeredBroadcast, as it does not make any sense to invalidate an
         * PaAtsTriggeredBroadcast that has not yet been written to the database.
         *
         * pre:    This PaAtsTriggeredBroadcast was initially loaded from the database
         *         or writePaAtsTriggeredBroadcastData() has already been called
         */
        void invalidate();


        /**
         * writePaAtsTriggeredBroadcastData
         * 
         * Write this PaAtsTriggeredBroadcast to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaAtsTriggeredBroadcast returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaAtsTriggeredBroadcast object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaAtsTriggeredBroadcastData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaAtsTriggeredBroadcast
         *
         * Remove this PaAtsTriggeredBroadcast from the database. Once this method has been executed, the
         * PaAtsTriggeredBroadcastHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaAtsTriggeredBroadcast was initially loaded from the database
         *         or writePaAtsTriggeredBroadcastData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         *
         */
        void deletePaAtsTriggeredBroadcast();
	
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

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

    private:

        // Made private as it is not used
		PaAtsTriggeredBroadcastHelper& operator=(const PaAtsTriggeredBroadcastHelper &);

        /**
         * modifyExistingPaAtsTriggeredBroadcast
         *
         * This will update an existing PaAtsTriggeredBroadcast in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaAtsTriggeredBroadcast object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         */
        void modifyExistingPaAtsTriggeredBroadcast(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaAtsTriggeredBroadcast
         *
         * This will add a new PaAtsTriggeredBroadcast in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaAtsTriggeredBroadcast object is not sufficent to create an
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
        void addNewPaAtsTriggeredBroadcast(); //throw(TA_Core::TransactiveException);

        /**
         * validateLocationKeyExistence
         *
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         *
         */
        void validateLocationKeyExistence(); //throw(TA_Core::TransactiveException);

        void validateLabelLocationPairUniqueness();
    
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key; }


        unsigned long m_key;
        
        // none of the values can be null
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_atsEventType;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;    
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_destinationLocationKey;  
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_platformId;

        TA_Base_Core::PrimitiveWrapper<bool> m_isLastTrain;    
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_paZoneGroupKey;    
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_paDvaMessageKey;    
        TA_Base_Core::PrimitiveWrapper<bool> m_isEnabledParameter;          
        TA_Base_Core::PrimitiveWrapper<std::string> m_dvaMessageVersion;

        /**
         * fetchValueFrom
         *
         * Helper function to retrieve internal parameters values - database
         *  reloaded as required (which will update the input reference)
         *
         * @param inputRef a reference to one of our contained variables
         *          - assumed this will be updated when reload is called
         *
         * @return Type the value contained by inputRef after a reload (if required)
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaAtsTriggeredBroadcast record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        template <class Type>
        Type fetchValueFrom(const TA_Base_Core::PrimitiveWrapper<Type>& inputRef);
      
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(PaAtsTriggeredBroadcastHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

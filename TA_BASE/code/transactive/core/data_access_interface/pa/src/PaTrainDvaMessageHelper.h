/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTrainDvaMessageHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaTrainDvaMessageHelper is an object that is held by PaTrainDvaMessage and ConfigPaTrainDvaMessage objects. 
  * It contains all data used by PaTrainDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaTrainDvaMessage and ConfigPaTrainDvaMessage.
  *
  */

#if !defined(PaTrainDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PaTrainDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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

    class PaTrainDvaMessageHelper
        : public GenericDaiHelper<PaTrainDvaMessageHelper>
    {

    public:


        /** 
         * PaTrainDvaMessageHelper
         *
         * This constructor creates a new PaTrainDvaMessageHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaTrainDvaMessageHelper(const unsigned long key);

        
        /** 
         * PaTrainDvaMessageHelper
         *
         * Copy constructor.
         *
         * @param   thePaTrainDvaMessageHelper the original PaTrainDvaMessageHelper to copy.
         *       
         */
        PaTrainDvaMessageHelper( const PaTrainDvaMessageHelper& thePaTrainDvaMessageHelper);

        /** 
         * PaTrainDvaMessageHelper
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
        PaTrainDvaMessageHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaTrainDvaMessageHelper
         *
         * This constructor creates a new PaTrainDvaMessageHelper for configuration, initialising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePaTrainDvaMessageData() is called for the first time.
         *
         */
        PaTrainDvaMessageHelper();


        /** 
         * ~PaTrainDvaMessageHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaTrainDvaMessageHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaTrainDvaMessage.  A new PaTrainDvaMessage is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaTrainDvaMessage
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaTrainDvaMessage.
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
         *         - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLabel
         *
         * Returns the Label of this PaTrainDvaMessage.  If this PaTrainDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTrainDvaMessage has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getLabel();       


        /**
         * setLabel
         *
         * Sets the Label of this PaTrainDvaMessage. The set values are not written to the database
         * until writePaTrainDvaMessageData() is called.  This method should only be used by the 
         * ConfigPaTrainDvaMessage class.
         *
         * @param  operation the Operation description for this PaTrainDvaMessage
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. label should be a string)
         *         - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLabel(const std::string& label);


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
         * Returns the date created for this PaTrainDvaMessage.  If this PaTrainDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTrainDvaMessage has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaTrainDvaMessage as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaTrainDvaMessage.  If this PaTrainDvaMessage record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaTrainDvaMessage has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaTrainDvaMessage as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


        /**
         * invalidate
         *
         * Make the data contained by this PaTrainDvaMessage as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaTrainDvaMessageData() has been called prior to calling
         * this method on a new PaTrainDvaMessage, as it does not make any sense to invalidate an
         * PaTrainDvaMessage that has not yet been written to the database.
         *
         * pre:    This PaTrainDvaMessage was initially loaded from the database
         *         or writePaTrainDvaMessageData() has already been called
         */
        void invalidate();


        /**
         * writePaTrainDvaMessageData
         * 
         * Write this PaTrainDvaMessage to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - For new messages, the primary key hasn't been set or is non unique
         *          - NO_VALUE if the PaTrainDvaMessage record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaTrainDvaMessage returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaTrainDvaMessage object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaTrainDvaMessageData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaTrainDvaMessage
         *
         * Remove this PaTrainDvaMessage from the database. Once this method has been executed, the
         * PaTrainDvaMessageHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaTrainDvaMessage was initially loaded from the database
         *         or writePaTrainDvaMessageData() has already been called
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
        void deletePaTrainDvaMessage(bool cascade = false);
	
        /**
         * validateKeyUniqueness
         *
         * Checks the database to confirm the user defined key value
         *  doesn't already exist in the database
         *
         * @pre the user defined key has been set internally
         *
         * @param keyToTest the primary key to check for presence in database
         *
         * @exception DataException if there is a matching
         *         primary key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        static void validateKeyUniqueness(unsigned long keyToTest); 

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
		PaTrainDvaMessageHelper& operator=(const PaTrainDvaMessageHelper &);

        
        /**
         * modifyExistingPaTrainDvaMessage
         *
         * This will update an existing PaTrainDvaMessage in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaTrainDvaMessage object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         */
        void modifyExistingPaTrainDvaMessage(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaTrainDvaMessage
         *
         * This will add a new PaTrainDvaMessage in the database with the internals currently set in this 
         * instance.
         *
         * @param keyToUse the key value to use for insertion (up to client code
         *          to determine a suitable key)
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaTrainDvaMessage object is not sufficent to create an
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
        void addNewPaTrainDvaMessage(unsigned long keyToUse); //throw(TA_Core::TransactiveException);
        
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }
       
        // The actual key (as returned from database - or set by user)        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;     
        // The key we want to change to (if at all)
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_modifiedKey;

        std::string m_label;
        
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Base_Core

#endif // !defined(PaTrainDvaMessageHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

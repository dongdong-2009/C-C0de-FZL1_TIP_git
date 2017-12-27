/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaBroadcastShortcutHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  *
  * PaDvaMessageHelper is an object that is held by PaDvaMessage and ConfigPaDvaMessage objects. 
  * It contains all data used by PaDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaDvaMessage.
  */

#ifndef PABROADCASTSHORTCUTHELPER__INCLUDED_
#define PABROADCASTSHORTCUTHELPER__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;

    class PaBroadcastShortcutHelper : public GenericDaiHelper<PaBroadcastShortcutHelper>
    {

    public:
        /** 
         * PaBroadcastShortcutHelper
         *
         * This constructor creates a new PaBroadcastShortcutHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaBroadcastShortcutHelper(const unsigned long key);

        
        /** 
         * PaBroadcastShortcutHelper
         *
         * Copy constructor.
         *
         * @param   thePaBroadcastShortcutHelper the original PaBroadcastShortcutHelper to copy.
         *       
         */
        PaBroadcastShortcutHelper( const PaBroadcastShortcutHelper& thePaBroadcastShortcutHelper);

        /** 
         * PaBroadcastShortcutHelper
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
        PaBroadcastShortcutHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaBroadcastShortcutHelper
         *
         * This constructor creates a new PaBroadcastShortcutHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writePaDvaMessageData() is called for the first time.
         *
         */
        PaBroadcastShortcutHelper();


        /** 
         * ~PaBroadcastShortcutHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaBroadcastShortcutHelper();


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

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getShortcutNo();
        virtual unsigned long getDVAMsgKey();

        virtual void setLocationKey( unsigned long ulLocationKey );
        virtual void setShortcutNo( unsigned long ulShortcutNo );
        virtual void setDVAMsgKey( unsigned long ulDVAMsgKey );

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
        void writePaBroadcastShortcut(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaDvaMessage
         *
         * Remove this PaDvaMessage from the database. Once this method has been executed, the
         * PaBroadcastShortcutHelper object should be destroyed.  Note that the track field can be NULL 
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
        void deletePaBroadcastShortcut();
	
        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static const DatabaseQueryHelper::QueryData getBasicQueryData();

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
		PaBroadcastShortcutHelper& operator=(const PaBroadcastShortcutHelper &);

        
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
        void modifyExistingPaBroadcastShortcut(); //throw(TA_Core::TransactiveException);

        
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
        void addNewPaBroadcastShortcut(); //throw(TA_Core::TransactiveException);

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

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulLocationKey;        // Mustn't be 'unset', all table columns non null
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulShortcutNo;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulDVAMsgKey;

        bool m_isValidData;
        bool m_isNew;
    };
} // closes TA_IRS_Core

#endif // !defined(PaBroadcastShortcutHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

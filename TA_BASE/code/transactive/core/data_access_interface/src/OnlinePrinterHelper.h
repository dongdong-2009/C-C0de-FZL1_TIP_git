/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinterHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * OnlinePrinterHelper is an object that is held by OnlinePrinter and ConfigOnlinePrinter objects. 
  * It contains all data used by OnlinePrinters, and manipulation
  * methods for the data. It helps avoid re-writing code for both OnlinePrinter and ConfigOnlinePrinter.
  */

#if !defined(OnlinePrinterHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define OnlinePrinterHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // _MSC_VER

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>
#include <climits>


namespace TA_Base_Core
{
    class IData;

    class OnlinePrinterHelper
    {

    public:


        /** 
         * OnlinePrinterHelper
         *
         * This constructor creates a new OnlinePrinterHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        OnlinePrinterHelper(const unsigned long idKey);
		
        /** 
         * OnlinePrinterHelper
         *
         * Copy constructor.
         *
         * @param   theOnlinePrinterHelper the original OnlinePrinterHelper to copy.
         *
         */
        OnlinePrinterHelper( const OnlinePrinterHelper& theOnlinePrinterHelper);

        /** 
         * OnlinePrinterHelper
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
        OnlinePrinterHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * OnlinePrinterHelper
         *
         * This constructor creates a new OnlinePrinterHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setOnlinePrinterId() before writing 
         * to the database for the first time.
         *
         */
        OnlinePrinterHelper();

        
        /** 
         * ~OnlinePrinterHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~OnlinePrinterHelper();

        /**
         * isNew
         *
         * This returns whether this is a new OnlinePrinter.  A new OnlinePrinter is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new OnlinePrinter
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getKey
         *
         */
		unsigned long getKey();


		std::string getPrinter();
        unsigned long getSubsystem();
        unsigned long getLocation();
        bool getIsAlarm();

        void setPrinter(const std::string& pPrinter);
        void setSubsystem(unsigned long pSubsystem);
        void setLocation(unsigned long pLocation);
        void setIsAlarm(bool pIsAlarm);

    
        /**
         * invalidate
         *
         * Make the data contained by this OnlinePrinter as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeOnlinePrinterData() has been called prior to calling
         * this method on a new OnlinePrinter, as it does not make any sense to invalidate an
         * OnlinePrinter that has not yet been written to the database.
         *
         * pre:    This OnlinePrinter was initially loaded from the database
         *         or writeOnlinePrinterData() has already been called
         */
        void invalidate();


        /**
         * writeOnlinePrinterData
         * 
         * Write this OnlinePrinter to the database.
         */
        void writeOnlinePrinterData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteOnlinePrinter
         *
         * Remove this OnlinePrinter from the database. Once this method has been executed, the
         * OnlinePrinterHelper object should be destroyed.
         *
         * pre:    This OnlinePrinter was initially loaded from the database
         *         or writeOnlinePrinterData() has already been called
         *
         * @param  cascade set to true to force the removal of this record by removing all 
         *         references to this record in other tables (AT_FIXED_BLOCK_MAP).  This should 
         *         be set to true to prevent the DatabaseException from being thrown.
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           AT_FIXED_BLOCK_MAP.  These can be removed by specifying the cascade option.
         *
         */
        void deleteOnlinePrinter(bool cascade = false);
		

    private:
        // Made private as it is not used
		OnlinePrinterHelper& operator=(const OnlinePrinterHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This OnlinePrinter was initially loaded from the database
         *         or writeOnlinePrinterData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. OnlinePrinterId should be an unsigned long)
         *         - NO_VALUE if the OnlinePrinter record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void reload();

        /**
         * reloadUsing
         *
         * Fills out the local members with data contained in the input data object
         *
         * @param row the row to query in the data object
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
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
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        
        /**
         * modifyExistingOnlinePrinter
         *
         * This will update an existing OnlinePrinter in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the OnlinePrinter object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingOnlinePrinter() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewOnlinePrinter
         *
         * This will add a new OnlinePrinter in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the OnlinePrinter object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewOnlinePrinter() throw(TA_Base_Core::TransactiveException);


        int m_idKey;
        std::string m_printer;
        unsigned long m_subsystem;
        unsigned long m_location;
        bool m_isAlarm;
        
        bool m_isValidData;
        bool m_isNew; 

    };
} // closes TA_IRS_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(OnlinePrinterHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrintCfgHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by:  $Author: huirong.luo $
  *
  * OnlinePrintCfgHelper is an object that is held by OnlinePrintCfg and ConfigOnlinePrintCfg objects. 
  * It contains all data used by OnlinePrintCfgs, and manipulation
  * methods for the data. It helps avoid re-writing code for both OnlinePrintCfg and ConfigOnlinePrintCfg.
  */

#if !defined(OnlinePrintCfgHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define OnlinePrintCfgHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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

    class OnlinePrintCfgHelper
    {

    public:


        /** 
         * OnlinePrintCfgHelper
         *
         * This constructor creates a new OnlinePrintCfgHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        OnlinePrintCfgHelper(const unsigned long idKey);

        /** 
         * OnlinePrintCfgHelper
         *
         * Copy constructor.
         *
         * @param   theOnlinePrintCfgHelper the original OnlinePrintCfgHelper to copy.
         *
         */
        OnlinePrintCfgHelper( const OnlinePrintCfgHelper& theOnlinePrintCfgHelper);

        /** 
         * OnlinePrintCfgHelper
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
        OnlinePrintCfgHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * OnlinePrintCfgHelper
         *
         * This constructor creates a new OnlinePrintCfgHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setOnlinePrintCfgId() before writing 
         * to the database for the first time.
         *
         */
        OnlinePrintCfgHelper();

        
        /** 
         * ~OnlinePrintCfgHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~OnlinePrintCfgHelper();

        /**
         * isNew
         *
         * This returns whether this is a new OnlinePrintCfg.  A new OnlinePrintCfg is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new OnlinePrintCfg
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		std::string getDefaultPrinter();
        unsigned long getLocation();
        std::string getLocationName();
		std::string getLocationDisplayName();

        void setDefaultPrinter(const std::string& pPrinter);
        void setLocation(unsigned long pLocation);
        void setLocationName(const std::string& pLocationName);

    
        /**
         * invalidate
         *
         * Make the data contained by this OnlinePrintCfg as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeOnlinePrintCfgData() has been called prior to calling
         * this method on a new OnlinePrintCfg, as it does not make any sense to invalidate an
         * OnlinePrintCfg that has not yet been written to the database.
         *
         * pre:    This OnlinePrintCfg was initially loaded from the database
         *         or writeOnlinePrintCfgData() has already been called
         */
        void invalidate();


        /**
         * writeOnlinePrintCfgData
         * 
         * Write this OnlinePrintCfg to the database.
         */
        void writeOnlinePrintCfgData() throw(TA_Base_Core::TransactiveException);


    private:
        // Made private as it is not used
		OnlinePrintCfgHelper& operator=(const OnlinePrintCfgHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This OnlinePrintCfg was initially loaded from the database
         *         or writeOnlinePrintCfgData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. OnlinePrintCfgId should be an unsigned long)
         *         - NO_VALUE if the OnlinePrintCfg record cannot be found for the helper 
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
         * modifyExistingOnlinePrintCfg
         *
         * This will update an existing OnlinePrintCfg in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the OnlinePrintCfg object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingOnlinePrintCfg() throw(TA_Base_Core::TransactiveException);
        void addNewOnlinePrintCfg() throw(TA_Base_Core::TransactiveException);


        std::string m_defaultPrinter;
        unsigned long m_location;
        std::string m_locationName;
		std::string m_locationDisplayName;
        
        bool m_isValidData;
        bool m_isNew; 
    };
} // closes TA_IRS_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(OnlinePrintCfgHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

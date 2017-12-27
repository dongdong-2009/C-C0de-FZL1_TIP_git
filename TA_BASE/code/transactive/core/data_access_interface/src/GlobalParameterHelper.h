/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GlobalParameterHelper.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * GlobalParameterHelper is an object that is held by all type-specific GlobalParameter objects, as well as by
  * the ConfigGlobalParameter. It contains all data common across GlobalParameters, and manipulation
  * methods for the data. It helps avoid re-writing code for each GlobalParameter object.
  *
  */


#if !defined(GlobalParameterHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define GlobalParameterHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <map>
#include <ctime>

#include "core/data_access_interface/src/IData.h"


namespace TA_Base_Core
{

    class GlobalParameterHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigGlobalParameter class ready to be populated.
         */
        GlobalParameterHelper( );


        /**
         * Constructor
         *
         * Construct a GlobalParameter class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this GlobalParameter in the database
         */
        GlobalParameterHelper( const unsigned long key );

        
	    /**
	      * GlobalParameterHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */		
		GlobalParameterHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a GlobalParameterHelper class based around an existing GlobalParameter. This will
		 * copy all the paramters of the existing GlobalParameter
         *
         * @param theGlobalParameter The GlobalParameter to copy
         */
		GlobalParameterHelper( const GlobalParameterHelper& theGlobalParameter);            	
		

        /**
         * Destructor
         */
        virtual ~GlobalParameterHelper();


        /**
         * isNew
         *
         * This returns whether this is a new GlobalParameter or not
         *
         * @return bool - True if this is a new entity
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the key for this GlobalParameter.
         *
         * @return The key for this GlobalParameter as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the GlobalParameter object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This entity has not been deleted
         */
        virtual void applyChanges();


        /**
         * deleteThisGlobalParameter
         *
         * Removes this GlobalParameter from the database. 
         * The calling application MUST then delete this GlobalParameter object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this GlobalParameter was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This GlobalParameter has not been deleted
         */
        virtual void deleteThisGlobalParameter();


        /**
         * getName
         *
         * Returns the name of this GlobalParameter. If this is the first time data for this GlobalParameter
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

        virtual std::string getValue();
			

        virtual unsigned long getSubsystemKey();	
		

        /**
         * getDateCreated
         *
         * Returns the date created for this GlobalParameter.
         *
         * @return The date created for this GlobalParameter as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the GlobalParameter key is invalid (and this is not a new GlobalParameter).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this GlobalParameter.
         *
         * @return The date modified for this GlobalParameter as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the GlobalParameter key is invalid (and this is not a new GlobalParameter).
         */
        time_t getDateModified();

        
        /**
         * setName
         *
         * Sets the name of this GlobalParameter locally.
         *
         * @param name The name to give this GlobalParameter.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the GlobalParameter key is invalid (and this is not a new GlobalParameter).
         */
        virtual void setName(const std::string& name);


        virtual void setValue(const std::string& value);


        virtual void setSubsystemKey(unsigned long subsystemKey);


        /**
         * invalidate
         *
         * Make the data contained by this GlobalParameter as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();
    
    private:

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * modifyExistingGlobalParameter
         *
         * This will update an existing GlobalParameter in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the GlobalParameter object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingGlobalParameter();

        
        /**
         * addNewGlobalParameter
         *
         * This will add a new GlobalParameter in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the GlobalParameter object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewGlobalParameter();

        void validateKeyUniqueness();

        void validateNameSubsystemUniqueness();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();


        // Operator = is not used so this method is hidden.
        GlobalParameterHelper& operator=(const GlobalParameterHelper &);


        unsigned long   m_key;
        std::string     m_name;            
        std::string     m_value;
        unsigned long   m_subsystemKey;
        time_t          m_dateCreated;
        time_t          m_dateModified;
        std::string     m_localDatabase;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(GlobalParameterHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)

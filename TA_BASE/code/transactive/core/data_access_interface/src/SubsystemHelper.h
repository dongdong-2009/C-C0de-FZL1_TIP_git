/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/SubsystemHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 09:59:59 $
  * Last modified by:  $Author: Ouyang $
  * 
  * SubsystemHelper is used by ConfigSubsystem and Subsystem to implement
  * the actual database code that those classes provide the interface to.
  *
  */


#ifndef SUBSYSTEM_HELPER_H
#define SUBSYSTEM_HELPER_H


#include <string>
#include <vector>
#include <ctime>

#include "core/data_access_interface/src/IData.h"


namespace TA_Base_Core
{

    class SubsystemHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty SubsystemHelper class ready to be populated.
         */
        SubsystemHelper();


        /**
         * Constructor
         *
         * Construct a Subsystem class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Subsystem in the database
         */
        SubsystemHelper(const unsigned long key);
		
	    /**
	      * SubsystemHelper
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
		SubsystemHelper(const unsigned long row, TA_Base_Core::IData& data);        

        /**
         * Constructor
         *
         * Construct a SubsystemHelper class based around an existing subsystem. This will
		 * copy all the paramters of the existing subsystem
         *
         * @param theSubsystem The subsystem to copy
         */
		SubsystemHelper(const SubsystemHelper& theSubsystem);            	
		

        /**
         * Destructor
         */
        virtual ~SubsystemHelper();


        /**
         * isNew
         *
         * This returns whether this is a new subsystem or not
         *
         * @return bool - True if this is a new entity
         */
        virtual bool isNew() const;


        /**
         * getKey
         *
         * Returns the key for this Subsystem.
         *
         * @return The key for this Subsystem as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getSystemKey
         *
         * Returns the key for this system.
         *
         * @return The key for this system as an unsigned long.
         */
        virtual unsigned long getSystemKey();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Subsystem object
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
         * deleteThisSubsystem
         *
         * Removes this subsystem from the database. 
         * The calling application MUST then delete this subsystem object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this subsystem was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This subsystem has not been deleted
         */
        virtual void deleteThisSubsystem();


        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete();


        /**
         * getName
         *
         * Returns the name of this Subsystem. If this is the first time data for this Subsystem
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
		/**
		* getDisplayName
		*
		* Returns the display name of this Subsystem. If this is the first time data for this Subsystem
		* has been requested, all the data is loaded from the database.
		*
		* @return The  display name for this operator as a std::string
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. ACCESSLEVEL should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved.
		*/
		virtual std::string getDisplayName();
		

        /** 
         * isPhysical
         *
         * This returns whether this is a physical subsystem or not.
         *
         * @return bool - True if this is a physical subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isPhysical();

		/**
		  * See ISubsystem.h for a description of this method.
		  */
		virtual bool isExclusive();


		/**
		  * See ISubsystem.h for a description of this method.
		  */
		virtual bool isLocationExclusive();
        /**
         * setExclusiveControlStatus
         * 
         * Sets whether the subsystem has exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setExclusiveControlStatus(bool isExclusive);



        /**
         * setLocationExclusiveControlStatus
         * 
         * Sets whether the subsystem has locaction exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setLocationExclusiveControlStatus(bool isExclusive);		

        /**
         * getDateCreated
         *
         * Returns the date created for this subsystem.
         *
         * @return The date created for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this subsystem.
         *
         * @return The date modified for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateModified();

        
        /**
         * setName
         *
         * Sets the name of this subsystem locally.
         *
         * @param name The name to give this subsystem.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setName(const std::string& name);
		/**
		* setDisplayName
		*
		* Sets the display name of this subsystem locally.
		*
		* @param name The display name to give this subsystem.
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. TYPEKEY should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved, or if the
		*            the subsystem key is invalid (and this is not a new subsystem).
		*/
		virtual void setDisplayName(const std::string& name);


        /**
         * invalidate
         *
         * Make the data contained by this Subsystem as invalid. The next call to get...() 
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
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this action was initially loaded from the database
         *      OR     - writeProfile() has already been called
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
         * modifyExistingSubsystem
         *
         * This will update an existing subsystem in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the subsystem object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingSubsystem();

        
        /**
         * addNewSubsystem
         *
         * This will add a new subsystem in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Subsystem object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewSubsystem();


        /**
         * checkForReferencesFromEntityTable
         *
         * Checks that there are no foreign keys in the entity table
         * referencing this row of the Subsystem table.
         *
         * @exception DataException thrown if there is a foreign key reference to this row.
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem communicating with the database.
         */
        void SubsystemHelper::checkForReferencesFromEntityTable();

        /**
         * checkForReferencesFromEntityTypeTable
         *
         * Checks that there are no foreign keys in the entity type table
         * referencing this row of the Subsystem table.
         *
         * @exception DataException thrown if there is a foreign key reference to this row.
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem communicating with the database.
         */
        void SubsystemHelper::checkForReferencesFromEntityTypeTable();

        /**
         * checkForReferencesFromProfileTables
         *
         * Checks that there are no foreign keys in the profile access or
         * profile delegate tables referencing this row of the Subsystem table.
         *
         * @exception DataException thrown if there is a foreign key reference to this row.
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem communicating with the database.
         */
        void SubsystemHelper::checkForReferencesFromProfileTables();


        /**
         * getCommaSeparatedList
         *
         * Gets the values in columnName from the given data object as a comma
         * separated list.
         * @exception DataException thrown if the values are not string data.
         */
        std::string SubsystemHelper::getCommaSeparatedList(
            IData* data, const std::string&  columnName);



        // Operator = is not used so this method is hidden.
        SubsystemHelper& operator=(const SubsystemHelper &);


        unsigned long m_key;
		unsigned long m_systemKey;
        std::string m_name;
		std::string m_displayName;
        bool m_isExclusive;
		bool m_isLocationExclusive;
        bool m_isPhysical;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // SUBSYSTEM_HELPER_H

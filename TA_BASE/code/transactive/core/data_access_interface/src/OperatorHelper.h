/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OperatorHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * OperatorHelper is an object that is held by Operator and ConfigOperator objects. 
  * It contains all data used by operators, and manipulation
  * methods for the data. It helps avoid re-writing code for both Operator and ConfigOperator.
  */

#if !defined(OperatorHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define OperatorHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <climits>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class IProfile;
	class IRegion;
	class IData;

    class OperatorHelper
    {

    public:
        /**
         * OperatorHelper
         * 
         * This constructor creates a new OperatorHelper for the specified key.
         *
         * @param key The key into the OPERATOR table for this Operator
         */
        OperatorHelper(const unsigned long key);

        /**
         * OperatorHelper
         * 
         * This constructor creates a new OperatorHelper for the specified parameters.
         *
         * @param key The key into the OPERATOR table for this Operator
         * @param name The name into the OPERATOR table for this Operator
         * @param description The description into the OPERATOR table for this Operator
         * @param password The password into the OPERATOR table for this Operator
         * @param isSystemOperator The is_system into the OPERATOR table for this Operator
         * @param dateCreated The date_created into the OPERATOR table for this Operator
         * @param dateModified The date_modified into the OPERATOR table for this Operator
         * @param loadProfiles If true, the associated profiles will be loaded
         * @param loadRegions If true, the associated regions will be loaded
         */
        OperatorHelper(const unsigned long key,
                       const std::string& name,
                       const std::string& description,
                       const std::string& password,
                       bool isSystemOperator,
                       time_t dateCreated,
                       time_t dateModified,
                       bool loadProfiles = false,
                       bool loadRegions = false);
       
		/**
         * OperatorHelper
         * 
         * This constructor creates a new OperatorHelper for the specified parameters.
		 *  the constructor creates a opertor for display only.
         */
		OperatorHelper(const unsigned long key,
                       const std::string& name,bool isSystemOperator);

        /** 
         * OperatorHelper
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
        OperatorHelper(unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions);
		

        /**
         * OperatorHelper
         * 
         * Copy Constructor
         */
        OperatorHelper( const OperatorHelper& theOperatorHelper);


        /**
         * OperatorHelper
         * 
         * This constructor creates a new OperatorHelper for configuration, initiailising
         * the key to 0 - it will be specified once this operator is written to the database
         * for the first time.
         */
        OperatorHelper();


        virtual ~OperatorHelper();

    public:

        /**
         * isNew
         *
         * This returns whether this is a new operator or not
         *
         * @return bool - True if this is a new operator
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        
        /**
         * getKey
         *
         * Returns the key for this Operator.
         *
         * @return The key for this operator as an unsigned long.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this Operator. If this is the first time data for this operator
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
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this operator. This should only be used by the ConfigOperator class
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * @param name The name to give this operator.
         */
        void setName(const std::string& name);


        /**
         * getDescription
         *
         * Returns the description for this Operator.
         *
         * @return The description for this Operator as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - setDescription() has already been called
         */
        std::string getDescription();


        /**
         * setDescription
         *
         * Sets the description of this operator. This should only be used by the ConfigOperator class
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * @param description The description to give this Operator
         */
        void setDescription(const std::string& description);


        /**
         * getPassword
         *
         * Returns the password for this Operator
         *
         * @return The password for the Operator as a string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        std::string getPassword();
		

        /**
         * setPassword
         *
         * Sets the password for this Operator. This should only be used by the ConfigOperator class.
         *
         * @param password The password to give this Operator.
         *
         * pre: This operator has not been deleted
         */
        void setPassword( const std::string password );
		

		/**
         * isSystemOperator
         *
         * This indicates if this operator is a system operator or not
         *
         * @return bool - True if this operator is a system operator, false otherwise
         */
        bool isSystemOperator();

     
        /**
         * getAssociatedProfiles
         *
         * Returns the profiles that this operator can use
         *
         * @return The profiles accessible by this operator, as a vector of IProfile*'s. It is the caller's
         *         responsibility to delete these.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		std::vector<IProfile*> getAssociatedProfiles();


		/**
         * getAssociatedRegions
         *
         * Returns the regions that this operator can use
         *
         * @return The regions accessible by this operator, as a vector of IRegion*'s. It is the caller's
         *         responsibility to delete these.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		std::vector<IRegion*> getAssociatedRegions();

        /**
         * getAssociatedProfileKeys
         *
         * Returns the profile keys that are accessible by this operator
         *
         * @return The profiles accessible by this operator, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		std::vector<unsigned long> getAssociatedProfileKeys();

        
        /**
         * setAssociatedProfile
         *
         * Changes a profile association for this operator
         *
         * @param unsigned long - This is the key of the profile to add or remove from the associations
         * @param bool - This indicates whether this profile is to be added or removed. True if the
         *               profile is to be added to the associations, false if the profile is to be
         *               removed.
         *
         * pre: This operator has not been deleted
         */
        void setAssociatedProfile( unsigned long profileKey, bool isAssociated);

        /**
         * getAssociatedRegionKeys
         *
         * Returns the region keys that are accessible by this operator
         *
         * @return The regions accessible by this operator, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		std::vector<unsigned long> getAssociatedRegionKeys();

        /**
         * getDateCreated
         *
         * Returns the date created for this operator
         *
         * @return The date created for this operator as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this operator.
         *
         * @return The date modified for this operator as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this operator as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database. This includes associated profiles and regions if required.
         * It is a precondition that writeOperatorData() has been called prior to calling
         * this method on a new operator, as it does not make any sense to invalidate an
         * operator that has not yet been written to the database.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         */
        void invalidate();


        /**
         * writeOperatorData
         * 
         * Write this operator to the database. If this is a new operator, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeOperatorData();


        /**
         * deleteOperator
         *
         * Remove this operator from the database. Once this method has been executed, the
         * OperatorHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         */
        void deleteOperator();
		

    private:
        // Made private as it is not used
		OperatorHelper& operator=(const OperatorHelper &);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
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
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions);

        /**
         * reloadProfiles()
         *
         * This method reloads the associated profiles from the database. It is called from reload()
         * or when getAssociatedProfileKeys() is called and the data state is not valid.
         */
        void reloadProfiles();

        /**
         * reloadRegions()
         *
         * This method reloads the associated regions from the database. It is called from reload()
         * or when getAssociatedRegionKeys() is called and the data state is not valid.
         */
        void reloadRegions();
        
        /**
         * modifyExistingOperator
         *
         * This will update an existing operator in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingOperator();

		/**
		* getLocalDatabaseName
		*
		* returns the name of the local database
		*/
		std::string getLocalDatabaseName();

        /**
         * addNewOperator
         *
         * This will add a new operator in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Location object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewOperator();


        /**
         * updateProfileAssociations
         *
         * This removes and adds profiles from/to the database to get it up-to-date with the user's changes.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         */
        void updateProfileAssociations();


        /**
         * retrieveDeletedNameForOperator
         *
         * When an operator is deleted certain fields must be appended with #number. The number is incremented for
         * each operator deleted with the same name or description. This determines what the next number will be
         * for this operator when it is deleted.
         *
         * @param string - This is the current value for the field we are checking. So this is the current name or
         *                 description. (ie m_name or m_description)
         * @param string - This is the name of the column in the database that stores this field.
         * @param unsigned long - This is the maximum size of the field that the database will allow.
         *
         * @return string - The new name for the field when it is deleted
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        std::string retrieveDeletedField(const std::string& currentField, const std::string& columnName, unsigned long maxFieldSize);


        /**
         * initialiseMaxFieldLengths
         *
         * This retrieves the maximum field lengths for the name and description fields from the database.
         *
         * @exception DataException A DataException will be thrown if there is a 
         *            problem retrieving the data from the database.
         */
        void initialiseMaxFieldLengths();


        /** 
          * getProfileKeys
          *
          * Returns the profile keys associated with this operator
          *
          *
          * @return the profile keys associated with this operator
          */
        std::vector<unsigned long> getProfileKeys();


        // This is the string to append to a name when it is being deleted
        static const std::string DELETED_FLAG;

        static const unsigned long DEFAULT_NAME_MAX_LENGTH;
        static const unsigned long DEFAULT_DESCRIPTION_MAX_LENGTH;
		
		std::string m_localDatabase;
        unsigned long m_key;
        std::string m_name;
        std::string m_description;
		std::string m_password;
        bool m_isSystemOperator;
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_loadProfiles;
        bool m_loadRegions;

        std::vector<unsigned long> m_associatedProfiles;
		std::vector<unsigned long> m_associatedRegions;
        bool m_isValidData;
        bool m_isValidProfileData;
        bool m_isValidRegionData;
        bool m_isNew;

	    static TA_Base_Core::NonReEntrantThreadLockable m_nameMaxLengthLock;
        static volatile unsigned long m_nameMaxLength;

        unsigned long m_descriptionMaxLength;

    };
} // closes TA_Base_Core

#endif // !defined(OperatorHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

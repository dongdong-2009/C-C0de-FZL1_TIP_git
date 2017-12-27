/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigOperator.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigOperator is a concrete implementation of IConfigOperator, which is in turn an implementation
  * of IOperator. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for operators.
  */


#if !defined(ConfigOperator_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigOperator_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/src/IConfigOperator.h"

namespace TA_Base_Core
{
	class IData;
    class OperatorHelper;

    class ConfigOperator : public IConfigOperator
    {

    public:
        /**
         * ConfigOperator (constructor)
         *
         * Constructs a new instance of the ConfigOperator with no key. This is used when
         * creating a *new* Operator - that is, one that does not yet exist in the database.
         */
        ConfigOperator();


        /**
         * ConfigOperator (constructor)
         *
         * Constructs a new instance of ConfigOperator with the specified key. This is used 
         * when creating a ConfigOperator based around an existing operator in the database.
         *
         * @param key The key to this operator in the database.
         */
        ConfigOperator(const unsigned long key);


        /**
         * ConfigOperator
         * 
         * Construct an ConfigOperator class based around given parameters.
         *
         * @param key The key into the OPERATOR table for this Operator
         * @param name The name into the OPERATOR table for this Operator
         * @param description The description into the OPERATOR table for this Operator
         * @param password The password into the OPERATOR table for this Operator
         * @param isSystemOperator The is_system into the OPERATOR table for this Operator
         * @param dateCreated The date_created into the OPERATOR table for this Operator
         * @param dateModified The date_modified into the OPERATOR table for this Operator
         * @param reloadProfile By default the associated profiles are not loaded, but if 
         *                      this is set to true, then the associated profile will be
         *                      loaded 
         */
        ConfigOperator(const unsigned long key, 
                 const std::string& name, 
                 const std::string& description,
                 const std::string& password,
                 bool isSystemOperator,
                 time_t dateCreated,
                 time_t dateModified,
                 bool reloadProfile = false);                 

		/**
         * Constructor
         *
         * Construct a ConfigOperator class based around dataset
         */
		ConfigOperator(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions);


        /**
         * ConfigOperator (copy constructor)
         *
         * @param ConfigOperator& - The operator to make a copy of.
         */
        ConfigOperator( const ConfigOperator& theConfigOperator);

        
        virtual ~ConfigOperator();


    public:

        /**
         * deleteThisObject
         *
         * Removes this operator from the database. 
         * The calling application MUST then delete this operator object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      This operator has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This operator has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this Operator.
         *
         * @return The key for this operator as an unsigned long.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      AND    - deleteThisObject() has NOT been called
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
         *      AND    - deleteThisObject() has NOT been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this operator. This should only be used by the ConfigOperator class
         *
         * @param name The name to give this operator.
         *
         * pre: deleteThisObject() has NOT been called
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
         *      AND    - deleteThisObject() has NOT been called
         */
        std::string getDescription();


        /**
         * setDescription
         *
         * Sets the description of this operator. This should only be used by the ConfigOperator class
         *
         * @param description The description to give this Operator
         *
         * pre: deleteThisObject() has NOT been called
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
        virtual std::string getPassword();

        
        /**
         * setPassword
         *
         * Sets the password for this Operator. This updates the database straight away.
         *
         * @param password The password to give this Operator.
         *
         * pre: This operator has not been deleted
         */
        virtual void setPassword( const std::string password );
		

		/**
         * isSystemOperator
         *
         * This indicates if this operator is a system operator or not
         *
         * @return bool - True if this operator is a system operator, false otherwise
         */
        virtual bool isSystemOperator();


        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this operator
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };


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
        virtual time_t getDateCreated();


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
        virtual time_t getDateModified();

        
        /**
         * getAssociatedProfiles
         *
         * Returns the profiles that this operator can use
         *
         * @return The profiles accessible by this operator, as a vector of IProfile*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<IProfile*> getAssociatedProfiles();


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
        virtual void setAssociatedProfile( unsigned long profileKey, bool isAssociated);

        
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
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the operator. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all operator changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the operator has been changed by the user.
         *
         * @return bool - This will return true if the operator does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the operator changes is empty then nothing has changed
            return !m_operatorChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this operator is new. A new operator is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the operator has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this operator as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateOperatorChanges
         *
         * This updates the map recording the changes to the operator. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateOperatorChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

        /**
         * haveProfilesChanged
         *
         * We have to have some way of maintaining whether changes have been made to the list of associated
         * profiles or not. It is too difficult to try and manage profile changes in strings in m_operatorChanges.
         * Instead we use the m_originalProfiles and m_newProfiles. This methods adds and removes new and old
         * profiles from m_newProfiles and compares the sets to determine if changes have been made.
         *
         * @param unsigned long - This is the key of the profile to add or remove from the associations
         * @param bool - This indicates whether this profile is to be added or removed. True if the
         *               profile is to be added to the associations, false if the profile is to be
         *               removed.
         */
        bool haveProfilesChanged(unsigned long profileKey, bool isAssociated);


        /**
         * getProfileNames
         * 
         * This takes a set of profile keys and converts it into a comma separated list of
         * strings. This can be used for audit messages.
         *
         * @param set<unsigned long> - A set of profile keys
         * 
         * @return string - A comma separated list of profile names. This is suitable to be displayed to the user.
         */
        std::string getProfileNames(std::set<unsigned long>& profileKeys);

		
        // Assignment operator not used so it is made private
		ConfigOperator& operator=(const ConfigOperator&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string DESCRIPTION;
        static const std::string PASSWORD;
        static const std::string PROFILES;


        OperatorHelper* m_operatorHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of operators in the
                                          // Configuration Editor because new operators do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_operatorChanges; // This is a map of all changes made to the operator so that they are
                                       // recorded and can be sent in audit or online update messages.

        std::set<unsigned long> m_originalProfiles; // This is a set of all the original profiles
                                                    // associated with this operator. This is used to determine
                                                    // whether changes have been made and allows us to report
                                                    // those changes.
        std::set<unsigned long> m_newProfiles;      // This is a set of all the profile changes. This allows us
                                                    // to compare the sets to see if any changes have been made.

    };
} // closes TA_Base_Core

#endif // !defined(ConfigOperator_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

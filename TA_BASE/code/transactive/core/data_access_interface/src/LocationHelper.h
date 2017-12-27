/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/LocationHelper.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * LocationHelper is an object that is held by all type-specific location objects, as well as by
  * the ConfigLocation. It contains all data common across locations, and manipulation
  * methods for the data. It helps avoid re-writing code for each location object.
  *
  */


#if !defined(LocationHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define LocationHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <map>
#include <ctime>
#include <vector>

#include "core/data_access_interface/src/IData.h"


namespace TA_Base_Core
{

    class LocationHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigLocation class ready to be populated.
         */
        LocationHelper( );


        /**
         * Constructor
         *
         * Construct a Location class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Location in the database
         */
        LocationHelper( const unsigned long key );

        
        /**
         * Constructor
         *
         * Construct a complete location, this will set isValidData to true
         *
         * @param key The key of this Location in the database
         * @param name The name of this Location in the database
         * @param description The description of this Location in the database
         * @param dateCreated The date and time the location was created
         * @param dateModified The date and time the location was last modified
         */
        LocationHelper( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,
                        const std::string displayname, const time_t dateCreated, const time_t dateModified, bool displayOnly = false, const std::string& typeName = "" );

	    /**
	      * LocationHelper
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
		LocationHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a LocationHelper class based around an existing location. This will
		 * copy all the paramters of the existing location
         *
         * @param theLocation The location to copy
         */
		LocationHelper( const LocationHelper& theLocation);            	
		

        /**
         * Destructor
         */
        virtual ~LocationHelper();


        /**
         * isNew
         *
         * This returns whether this is a new location or not
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
         * Returns the key for this Location.
         *
         * @return The key for this Location as an unsigned long.
         */
        virtual unsigned long getKey();

		std::string getDisplayName();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Location object
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
         * deleteThisLocation
         *
         * Removes this location from the database. 
         * The calling application MUST then delete this location object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this location was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This location has not been deleted
         */
        virtual void deleteThisLocation();


        /**
         * getName
         *
         * Returns the name of this Location. If this is the first time data for this Location
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
         * getOrderId
         *
         * Returns the oderId Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The physical order of this location
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getOrderId();
		


        /**
         * getDescription
         *
         * Returns the description of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this location as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();
		

        /**
         * getDateCreated
         *
         * Returns the date created for this location.
         *
         * @return The date created for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this location.
         *
         * @return The date modified for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateModified();

        
        /**
         * setName
         *
         * Sets the name of this location locally.
         *
         * @param name The name to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setName(const std::string& name);


        /**
         * setDescription
         *
         * Sets the description of this location locally.
         *
         * @param name The description to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setDescription(const std::string& description);


        
        /**
         * setOrderId
         *
         * Sets the orderId of this location locally.
         *
         * @param name The physical order of this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setOrderId(const unsigned long orderId);

		/**
         * setDisplayName
         *
         * Sets the display name of this location locally.
         *
         * @param dispalyname The name to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
		void setDisplayName(std::string dispalyname);


		/**
         * setDisplayOnly
         *
         * Sets display only or not of this location locally.
         *
         * @param displayOnly The value of display only to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
		virtual void setDisplayOnly(bool displayOnly);


        /**
         * getDefaultDisplay
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long profileKey - the profileKey for the display
         *
         * @return The display for this profile as a std::string. This string should be a path.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDefaultDisplay(unsigned long profileKey, int displayNumber);

        
        /**
         * isProfileAssociated
         *
         * Determines if the specified profile is associated with this location. This can be used to determine if a 
         * profile is valid at the given location.
         *
         * @return true if the profile is associated with the location, otherwise false.
         *
         * @param profileId The key to the profile for which to check the association.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        bool isProfileAssociated(unsigned long profileId);

        bool isDisplayOnly();
        /**
         * invalidate
         *
         * Make the data contained by this Location as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        std::string getTypeName();
        void setTypeName( const std::string& typeName ) ;

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
         * reloadProfileData
         *
         * This method reloads the list of profiles associated with this location.
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the location object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void reloadProfileData();


        /**
         * modifyExistingLocation
         *
         * This will update an existing location in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the location object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingLocation();

        
        /**
         * addNewLocation
         *
         * This will add a new location in the database with the settings currently saved here
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
        void addNewLocation();


        // Operator = is not used so this method is hidden.
        LocationHelper& operator=(const LocationHelper &);


        unsigned long m_key;
		unsigned long m_orderId;
        std::string m_name;            
        std::string m_description;
		std::string m_displayName;
        time_t m_dateCreated;
        time_t m_dateModified;
		
		bool m_displayOnly;
        std::string m_typeName;

        bool m_isNew;
        bool m_isValidData;
        bool m_isValidProfileData;

        std::vector<unsigned long> m_associatedProfileKeys;
    };
} // closes TA_Base_Core

#endif // !defined(LocationHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)

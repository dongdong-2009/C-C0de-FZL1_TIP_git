/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * AlarmPlanAssocHelper is an object that is held by all type-specific alarm plan association objects.
  * It contains all data common across alarm plan associations, and manipulation
  * methods for the data. It helps avoid re-writing code for each object.
  */


#if !defined(AlarmPlanAssocHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define AlarmPlanAssocHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <ctime>

namespace TA_Base_Core
{

    class AlarmPlanAssocHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty helper class ready to be populated.
         */
        AlarmPlanAssocHelper( );


		/**
		 * AlarmPlanAssocData (constructor)
		 *
		 * Construct a helper class for the entry represented by the specified data.
		 * The helper will be constructed with valid data.
		 *
		 * @param key The key of this Alarm Plan Association in the database
		 * @param unsigned long entityKey
		 * @param unsigned long entityTypeKey
		 * @param const std::string& alarmType
		 * @param const std::string& planPath
         * @param time_t The time the entry was created in the database
         * @param time_t The time the entry was last modified in the database
		 */
		AlarmPlanAssocHelper( unsigned long key, 
			                  unsigned long entityKey,
		                      unsigned long entityTypeKey,
			                  unsigned long alarmType,
			                  const std::string& planPath,
                              time_t dateCreated,
                              time_t dateModified);



        /**
         * Constructor
         *
         * Constructs a AlarmPlanAssocHelper class for the entry specified by the key. the first call
         * to the object will populate the rest of the fields
         *
		 * @param key The key of this Alarm Plan Association in the database
         */
        AlarmPlanAssocHelper( unsigned long key );


        /**
         * Constructor
         *
         * Construct a AlarmPlanAssocHelper class based around an existing association. This will
		 * copy all the paramters of the existing alarm plan association.
         *
         * @param rhs The alarm plan association to copy
         */
		AlarmPlanAssocHelper( const AlarmPlanAssocHelper& rhs);
		

        /**
         * Destructor
         */
        virtual ~AlarmPlanAssocHelper();


        /**
         * isNew
         *
         * This returns whether this is a new alarm plan association or not
         *
         * @return bool - True if this is a new alarm plan association
         */
        bool isNew() const
        {
            return m_isNew;
        };


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the alarm plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This entity has not been deleted
         */
        void applyChanges();


        /**
         * deleteThisAssociation
         *
         * Removes this alarm plan association from the database. 
         * The calling application MUST then delete this alarm plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this alarm plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This alarm plan association has not been deleted
         */
        void deleteThisAssociation();


		/**
         * getKey
         *
         * Returns the key for this AlarmPlanAssocData
         *
         * @return The key for this AlarmPlanAssocData as an unsigned long.
         */
		unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        std::string getName();
		
        
        /** 
		  * getAlarmType
		  *
		  * Returns the alarm type for this association
		  *
		  * @return std::string The alarm type
		  */
		unsigned long getAlarmType();


		/** 
		  * getEntityKey
		  *
		  * Returns the entity key for this association
		  *
		  * @return unsigned long the entitykey
		  */
		unsigned long getEntityKey();


		/** 
		  * getEntityTypeKey
		  *
		  * Returns the entity type key for this association
		  *
		  * @return unsigned long the entity type key
		  */
		unsigned long getEntityTypeKey();


        /**
         * getPlanPath
         *
         * Returns the full path of the plan for this association
         *
         * @return std::string plan path in the format "[/<category1>/<categoryN>]/<planName>"
         */
        std::string getPlanPath();


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm plan association.
         *
         * @return The date created for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm plan association.
         *
         * @return The date modified for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
         */
        time_t getDateModified();

        
        /** 
		  * setAlarmType
		  *
		  * Sets the alarm type for this association
		  *
		  * @param std::string The alarm type
		  */
        void setAlarmType(unsigned long alarmType);


		/** 
		  * setEntityKey
		  *
		  * Sets the entity key for this association. Set to 0 if the entity key is not used.
		  *
		  * @param unsigned long the entitykey
		  */
		void setEntityKey(unsigned long entityKey);


		/** 
		  * setEntityTypeKey
		  *
		  * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
		  *
		  * @param unsigned long the entity type key
		  */
		void setEntityTypeKey(unsigned long entityTypeKey);


		/** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association. The plan path is validated to ensure the plan exists.
		  *
		  * @param std::string plan path in the format "[/<category1>/<categoryN>]/<planName>"
          *
          * @exception DataException - This is thrown if the path is empty or if it denotes a category
          *                            rather than a plan.
          * @exception DatabaseException - This is thrown if there is some error with the database
          *                                connection or an SQL statement failed to execute for
          *                                some reason.
		  */
		void setPlanPath(const std::string& planPath);


        /**
         * invalidate
         *
         * Make the data contained by this alarm plan association as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();
    

        /**
         * retrieveAlarmTypeName
         *
         * @param string - The key of the alarm type to look up
         *
         * @return This gets the display name for the alarm type passed in and returns it
         */
        std::string retrieveAlarmTypeName(unsigned long alarmTypeKey);

		/**
         * retrieveAlarmTypeDispalyName
         *
         * @param string - The key of the alarm type to look up
         *
         * @return This gets the display name for the alarm type passed in and returns it
         */
        std::string retrieveAlarmTypeDispalyName(unsigned long alarmTypeKey);
        /**
         * retrieveEntityName
         *
         * @param unsigned long - The key of the entity to look up
         *
         * @return This gets the display name for the entity passed in and returns it. If the entity is not set
         *         this returns ""
         */
        std::string retrieveEntityName(unsigned long entityKey);

        
        /**
         * retrieveEntityTypeName
         *
         * @param unsigned long - The key of the entity type to look up
         *
         * @return This gets the display name for the entity type passed in and returns it. If the entity type
         *         is not set this returns ""
         */
        std::string retrieveEntityTypeName(unsigned long entityTypeKey);


    private:
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
         * modifyExisting
         *
         * This will update an existing alarm plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the alarm plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

        
        /**
         * addNew
         *
         * This will add a new alarm plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the alarm plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();


        // Operator = is not used so this method is hidden.
        AlarmPlanAssocHelper& operator=(const AlarmPlanAssocHelper &);


		unsigned long m_key;
		unsigned long m_entityKey;
		unsigned long m_entityTypeKey;
		unsigned long m_alarmType;
		std::string m_planPath;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(AlarmPlanAssocHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocHelper.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * EventPlanAssocHelper is an object that is held by all type-specific event plan association objects.
  * It contains all data common across event plan associations, and manipulation methods for the data.
  */


#if !defined(_EVENT_PLAN_ASSOC_HELPER_H_)
#define _EVENT_PLAN_ASSOC_HELPER_H_


#include <string>
#include <ctime>

namespace TA_Base_Core
{

    class EventPlanAssocHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty helper class ready to be populated.
         */
        EventPlanAssocHelper();


        /**
         * Constructor
         *
         * Construct a helper class for the entry represented by the specified data.
         * The helper will be constructed with valid data.
         *
         * @param key The key of this Event Plan Association in the database.
         * @param enabled True if the association is enabled.
         * @param eventTypeKey The key of the event type.
         * @param entityKey The key of the entity.
         * @param entityTypeKey The key of the entity type.
         * @param planPath The full path of the plan to execute.
         * @param dateCreated The date at which the association was created.
         * @param dateModified The date at which the association was last modified.
         */
        EventPlanAssocHelper( unsigned long key,
                              bool enabled,
                              unsigned long eventTypeKey,
                              unsigned long entityKey,
                              unsigned long entityTypeKey,
                              const std::string& planPath,
                              time_t dateCreated,
                              time_t dateModified );


        /**
         * Constructor
         *
         * Constructs a EventPlanAssocHelper class for the entry specified by the key. The first call
         * to the object will populate the rest of the fields
         *
         * @param key The key of this Event Plan Association in the database
         */
        EventPlanAssocHelper( unsigned long key );


        /**
         * Constructor
         *
         * Construct a EventPlanAssocHelper class based around an existing association. This will
         * copy all the paramters of the existing event plan association.
         *
         * @param rhs The event plan association to copy
         */
        EventPlanAssocHelper( const EventPlanAssocHelper& rhs);


        /**
         * Destructor
         */
        virtual ~EventPlanAssocHelper();


        /**
         * isNew
         *
         * This returns whether this is a new event plan association or not
         *
         * @return bool - True if this is a new event plan association
         */
        bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the key for this EventPlanAssocData
         *
         * @return The key for this EventPlanAssocData as an unsigned long.
         */
        unsigned long getKey();


        /**
         * getEventTypeKey
         *
         * Returns the event type for this association.
         *
         * @return unsigned long The event type key.
         */
        unsigned long getEventTypeKey();


        /**
         * isEnabled
         *
         * Returns whether the association is enabled.
         *
         * @return bool True if the association is enabled.
         */
        bool isEnabled();


        /** 
          * getEntityKey
          *
          * Returns the entity key for this association.
          *
          * @return unsigned long The entity key.
          */
        unsigned long getEntityKey();


        /** 
         * getEntityTypeKey
         *
         * Returns the entity type key for this association.
         *
         * @return unsigned long the entity type key.
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
         * Returns the date created for this event plan association.
         *
         * @return The date created for this event plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the event plan association key is invalid (and this is not a new event plan association).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this event plan association.
         *
         * @return The date modified for this event plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the event plan association key is invalid (and this is not a new event plan association).
         */
        time_t getDateModified();


        /**
         * setEventTypeKey
         *
         * Sets the event type key for this association.
         *
         * @param eventTypeKey The event type key.
         */
        void setEventTypeKey( unsigned long eventTypeKey );


        /**
         * setEnabled
         *
         * Sets whether this association is enabled.
         *
         * @param enabled True if the association is enabled.
         */
        void setEnabled( bool enabled );


        /**
         * setEntityKey
         *
         * Sets the entity key for this association. Set to 0 if the entity key is not used.
         *
         * @param unsigned long the entitykey
         */
        void setEntityKey( unsigned long entityKey );


        /**
         * setEntityTypeKey
         *
         * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
         *
         * @param unsigned long the entity type key
         */
        void setEntityTypeKey( unsigned long entityTypeKey );


		/** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association
		  *
		  * @param std::string plan path
		  */
		void setPlanPath(const std::string& planPath);


        /**
         * retrieveEventTypeKey
         *
         * Retrieves the key of the event type with the specified name.
         *
         * @param eventTypeName The name of the event type.
         *
         * @return unsigned long The event type key.
         */
        unsigned long retrieveEventTypeKey( const std::string& eventTypeName );

        /**
         * retrieveEventTypeName
         *
         * Retrieves the name of the event type with the specified key.
         *
         * @param eventTypeKey The key of the event type.
         *
         * @return std::string The event type name.
         */
        std::string retrieveEventTypeName( unsigned long eventTypeKey );

        /**
         * retrieveEventTypeDisplayName
         *
         * Retrieves the name of the event type with the specified key.
         *
         * @param eventTypeKey The key of the event type.
         *
         * @return std::string The event type name.
         */
        std::string retrieveEventTypeDisplayName( unsigned long eventTypeKey );

        /**
         * retrieveEntityTypeKey
         *
         * Retrieves the key of the entity type with the specified name.
         *
         * @param entityTypeName The name of the entity type.
         *
         * @return unsigned long The entity type key.
         */
        unsigned long retrieveEntityTypeKey( const std::string& entityTypeName );


        /**
         * retrieveEntityTypeName
         *
         * Retrieves the name of the entity type with the specified key.
         *
         * @param entityTypeKey The key of the entity type.
         *
         * @return std::string The entity type name.
         */
        std::string retrieveEntityTypeName( unsigned long entityTypeKey );


        /**
         * retrieveEntityKey
         *
         * Retrieves the key of the entity with the specified name.
         *
         * @param entityName The name of the entity.
         *
         * @return unsigned long The entity key.
         */
        unsigned long retrieveEntityKey( const std::string& entityName );


        /**
         * retrieveEntityName
         *
         * Retrieves the name of the entity with the specified key.
         *
         * @param entityKey The key of the entity.
         *
         * @return std::string The entity name.
         */
        std::string retrieveEntityName( unsigned long entityKey );


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the event plan association object
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
         * Removes this event plan association from the database. 
         * The calling application MUST then delete this event plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this event plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This event plan association has not been deleted
         */
        void deleteThisAssociation();


        /**
         * invalidate
         *
         * Make the data contained by this event plan association as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();


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
         * This will update an existing event plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the event plan association object
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
         * This will add a new event plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the event plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();


        // Operator = is not used so this method is hidden.
        EventPlanAssocHelper& operator=( const EventPlanAssocHelper& );


        unsigned long m_key;
        bool m_enabled;
        unsigned long m_eventTypeKey;
        unsigned long m_entityKey;
        unsigned long m_entityTypeKey;
        std::string m_planPath;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(_EVENT_PLAN_ASSOC_HELPER_H_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionGroupHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 09:59:59 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ActionGroupHelper is an object that is held by all type-specific action group objects, as well as by
  * the ConfigActionGroup. It contains all data common across action groups, and manipulation
  * methods for the data. It helps avoid re-writing code for each action group object.
  *
  */
  

#if !defined(ActionGroupHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define ActionGroupHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <set>
#include <iterator>
#include <vector>

namespace TA_Base_Core
{
	// forward declarations
	class IData;

    class ActionGroupHelper
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigActionGroup class ready to be populated.
         */
        ActionGroupHelper( );


        /**
         * Constructor
         *
         * Construct an ActionGroup class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this ActionGroup in the database
         * @param name The name of this ActionGroup in the database
         */
        ActionGroupHelper( const unsigned long key, const std::string& name );


        /**
         * Constructor
         *
         * Construct a ActionGroupHelper class based around an existing ActionGroup. This will
		 * copy all the parameters of the existing ActionGroup
         *
         * @param theActionGroup The ActionGroup to copy
         */
		ActionGroupHelper( const ActionGroupHelper& theActionGroup);            	

	    /**
	      * ActionGroupHelper
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
        ActionGroupHelper(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions);

		
        /**
         * Destructor
         */
        virtual ~ActionGroupHelper();


        /**
         * isNew
         *
         * This returns whether this is a new ActionGroup or not
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
         * Returns the key for this ActionGroup.
         *
         * @return The key for this ActionGroup as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this ActionGroup. If this is the first time data for this ActionGroup
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
         * Returns the display name of this ActionGroup. If this is the first time data for this ActionGroup
         * has been requested, all the data is loaded from the database.
         *
         * @return The display name for this operator as a std::string
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
         * isControlType
         *
         * Returns whether or not the isControlType flag is set in the database.
         *
         * @return boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isControlType();

        /**
         * isActionIncluded
         *
         * This checks to see if the specified actionkey is currently included in the list of actions
         * for this ActionGroup.
         *
         * @param unsigned long - The action key to check for
         *
         * @return bool - This will return true if the actionkey is associated with this ActionGroup and false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        bool isActionIncluded(unsigned long actionKey);


        /**
         * getDateCreated
         *
         * Returns the date created for this ActionGroup
         *
         * @return The date created for this ActionGroup as a time_t.
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
         * Returns the date modified for this ActionGroup.
         *
         * @return The date modified for this ActionGroup as a time_t.
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
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);

	   /**
         * setDisplayName
         *
         * Sets the display name of this item locally.
         *
         * @param name The display name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setDisplayName(const std::string& displayName);

        /**
         * setIsControlType
         *
         * Sets whether or not the isControlType flag is set in the database.
         *
         * @param boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setIsControlType(bool isControlType);

        /**
         * addAction
         *
         * Adds a new action to the ActionGroup type
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void addAction(unsigned long actionKey);


        /**
         * removeAction
         *
         * Removes an action from the ActionGroup
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeAction(unsigned long actionKey);

        
        /**
         * invalidate
         *
         * Make the data contained by this ActionGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();
    

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the ActionGroup object
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
         * deleteThisActionGroup
         *
         * Removes this ActionGroup from the database. 
         * The calling application MUST then delete this ActionGroup object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ActionGroup was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This ActionGroup has not been deleted
         */
        virtual void deleteThisActionGroup();


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
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions);

        /**
         * modifyExisting
         *
         * This will update an existing ActionGroup in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the ActionGroup object
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
         * This will add a new ActionGroup in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the ActionGroup object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();


        /**
         * updateActions
         *
         * This will update the actions in the database
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the ActionGroup object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void updateActions();


        /**
         * checkForLinkedProfiles
         *
         * This checks to see if this ActionGroup is being used by any profiles. If it has profiles linked to it
         * it will throw an exception. It is checking these to determine if a delete can be 
         * performed. So discovering linked profiles is an error.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException - The type should be set to CANNOT_DELETE and it will contain
         *                            the list of linked profiles (maximum of 10)
         */
        void checkForLinkedProfiles();

        
        // Operator = is not used so this method is hidden.
        ActionGroupHelper& operator=(const ActionGroupHelper &);


        unsigned long m_key;
        std::string m_name;
		std::string m_displayName;
        bool m_isControlType;
        time_t m_dateCreated;
        time_t m_dateModified;

        std::set<unsigned long> m_actions;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(ActionGroupHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)

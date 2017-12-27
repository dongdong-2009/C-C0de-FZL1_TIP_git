/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GuiHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GuiHelper is an object that is held by Gui and ConfigGui objects. 
  * It contains all data used by guis, and manipulation
  * methods for the data. It helps avoid re-writing code for both Gui and ConfigGui.
  */

#if !defined(GuiHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define GuiHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>

#include <climits>

namespace TA_Base_Core
{
	// forward declarations
	class IData;

    class GuiHelper
    {

    public:
        /**
         * Constructor
         * 
         * This constructor creates a new GuiHelper for the specified key.
         *
         * @param key The key into the GUIAPPLICATION table for this Gui
         */
        GuiHelper(const unsigned long key);

	    /**
	      * GuiHelper
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
		GuiHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         * 
         * This constructor creates a new GuiHelper for configuration, initiailising
         * the key to 0 - it will be specified once this gui is written to the database
         * for the first time.
         */
        GuiHelper();


        /**
         * Copy Constructor
         */
        GuiHelper( const GuiHelper& theGuiHelper);


        virtual ~GuiHelper() {};

    public:
        /**
         * getKey
         *
         * Returns the key for this Gui.
         *
         * @return The key for this gui as an unsigned long.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getName();


        /**
         * getFullName
         *
         * Returns the full name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The full name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getFullName();


        /**
         * getExecutable
         *
         * Returns the executable for this Gui.
         *
         * @return The executable for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setDescription() has already been called
         */
        std::string getExecutable();


        /**
         * getHelpFilePath
         *
         * Returns the help file path for the Gui
         *
         * @return The help file path for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        std::string getHelpFilePath();


        /**
         * isIconDisplayed
         *
         * Returns whether or not this Gui should be offered as an application for the user to launch
         *
         * @return true if the application can be offered as an icon for the user to launch.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setAccessLevel() has already been called
         */             
        bool isIconDisplayed();


        /**
         * areInstancesLimited
         *
         * Returns whether or not this Gui should only have limited instances running.
         *
         * @return true if only one instance of the GUI should be running at a time.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setAccessLevel() has already been called
         */             
        bool areInstancesLimited();


        /**
         * shouldReposition
         *
         * Returns whether the application should be repositioned if it should not be relaunched 
         * (only meaningful if application is limited to 1 running instance).
         *
         * @return true if the GUI should be repositioned 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setAccessLevel() has already been called
         */ 
        bool shouldReposition();


        /**
         * shouldPassParameters
         *
         * Returns whether the application should be re-passed the parameters if it 
         * should not be relaunched
         *
         * @return true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - setAccessLevel() has already been called
         */             
        bool shouldPassParameters();


        /**
         * shouldTerminateOnLogout
         *
         * Returns whether the application should be closed when the user logs out of the system 
         *
         * @return true if the GUI should terminate on a logout
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        bool shouldTerminateOnLogout();
        
        
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
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool isNew()
        {
            return m_isNew;
        }

        
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
        void setName(const std::string& name);

        
        /**
         * setFullName
         *
         * Sets the full name of this Gui.
         *
         * @param The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void setFullName(const std::string& fullName);


        /**
         * setExecutable
         *
         * Sets the executable for this Gui.
         *
         * @param The executable for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        void setExecutable(const std::string& executable);


        /**
         * setHelpFilePath
         *
         * Sets the help file path for the Gui
         *
         * @param The help file path for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        void setHelpFilePath(const std::string& helpFilePath);

        
        /**
         * setIsIconDisplayed
         *
         * Sets whether the icon should be displayed for this Gui
         *
         * @param true if the GUI should be displayed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        void setIsIconDisplayed(bool isIconDisplayed);


        /**
         * setAreInstancesLimited
         *
         * Sets whether the application should be limited to one instance
         *
         * @param true if the GUI should only have one instance running
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        void setAreInstancesLimited(bool areInstancesLimited);


        /**
         * setShouldReposition
         *
         * Sets whether the application should be repositioned if it 
         * should not be relaunched
         *
         * @param true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        void setShouldReposition(bool shouldReposition);


        /**
         * setShouldPassParameters
         *
         * Sets whether the application should be re-passed the parameters if it 
         * should not be relaunched
         *
         * @param true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        void setShouldPassParameters(bool shouldPassParams);


        /**
         * setShouldTerminateOnLogout
         *
         * Sets whether the application should be terminated when the user logs out.
         *
         * @param true if the GUI should be terminated on system logout
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        void setShouldTerminateOnLogout(bool shouldTerminateOnLogout);

    
        /**
         * invalidate
         *
         * Make the data contained by this gui as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeGuiData() has been called prior to calling
         * this method on a new gui, as it does not make any sense to invalidate an
         * gui that has not yet been written to the database.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        void invalidate();


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
         * pre: This gui has not been deleted
         */
        void applyChanges();


        /**
         * deleteThisGui
         *
         * Removes this gui from the database. 
         * The calling gui MUST then delete this gui object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This gui has not been deleted
         */
        void deleteThisGui();

        /**
         * resetToNew
         *
         * Resets the "isNew" flag to be true. This can be used after calling "deleteThisGui" if the intention
         * is to undo a previous database write where a new object was written but then has to be removed.
         */
        void resetToNew()
        {
            m_isNew = true;
        };


        /** 
          * checkUniqueConstraints
          *
          * Ensures that m_name and m_fullname are unique.
          *
          * @exception DatabaseException  Thrown if the fields are not unique in the database.
          */
        void checkUniqueConstraints();


    private:

		GuiHelper& operator=(const GuiHelper &);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
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
         * modifyExisting
         *
         * This will update an existing gui in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the gui object
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
         * This will add a new gui in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the gui object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();

        
        bool m_isValidData;
        bool m_isNew;

        unsigned long m_key;
        std::string m_name;
        std::string m_fullName;
        std::string m_executable;
        std::string m_helpFilePath;
        bool m_isIconDisplayed;
        bool m_areInstancesLimited;
        bool m_shouldReposition;
        bool m_shouldPassParameters;
        bool m_shouldTerminateOnLogout;
        time_t m_dateCreated;
        time_t m_dateModified;

    };
} // closes TA_Base_Core

#endif // !defined(GuiHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

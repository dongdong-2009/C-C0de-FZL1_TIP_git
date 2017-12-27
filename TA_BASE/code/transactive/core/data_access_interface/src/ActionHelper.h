#if !defined(ACTIONHELPER_H_INCLUDED)
#define ACTIONHELPER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionHelper.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  * This is the base Action class that supports all get and set operaions on actions.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)
#include <string>

namespace TA_Base_Core
{
	// forward declarations
	class IData;

    class ActionHelper
    {

    public:
		
		virtual ~ActionHelper() {};

        /**
         * Constructor
         * 
         * This constructor creates a new ActionHelper for the specified key.
         *
         * @param key The key into the SE_ACTION table for this Gui
         */
        ActionHelper(unsigned long key);


        /**
         * Constructor
         *
         * Construct an ActionHelper class based around all data (except the duties).
         *
         * @param key The key of this Action in the database
         * @param name The name of this Action in the database
         * @param description The description of this Action in the database
         */
        ActionHelper( const unsigned long key, const std::string& name, const std::string& description );

		/**
         * Constructor
         *
         * Construct an ActionHelper class based around all data (except the duties).
         *
         * @param key The key of this Action in the database
         * @param name The name of this Action in the database
         * @param description The description of this Action in the database
		 * @param isDisplayOnly the IS_DISPLAY_ONLY of thsi Action in the database
         */
        ActionHelper( const unsigned long key, const std::string& name, const std::string& description,const bool isDisplayOnly );
	    /**
	      * ActionHelper
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
		ActionHelper(const unsigned long row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups);

        /**
         * Refer to IAction.h for a description of this method.
         */
        unsigned long getKey();

        /**
         * Refer to IAction.h for a description of this method.
         */
        std::string getName();

        /**
         * Refer to IAction.h for a description of this method.
         */
        std::string getDescription();
        
		/**
		  * Refer to IAction.h for a description of this method.
		  */
		std::vector<std::string> getActionGroups();

		/**
		  * Refer to IAction.h for a description of this method.
		  */
        void invalidate();

		bool isDisplayOnly();
    private:
		
		ActionHelper() {};
        ActionHelper( const ActionHelper& );
		ActionHelper& operator=( const ActionHelper& );

        bool m_isValidData;

        unsigned long m_key;
        std::string m_name;
        std::string m_description;
		bool m_isDisplayOnly;

		std::vector<std::string> m_actionGroups;

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
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups);
    };

} // closes TA_Base_Core

#endif // !defined(ACTIONHELPER_H_INCLUDED)

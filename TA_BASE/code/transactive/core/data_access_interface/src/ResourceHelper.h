/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ResourceHelper.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if !defined(RESOURCE_HELPER_H_INCLUDED)
#define RESOURCE_HELPER_H_INCLUDED

#include <vector>

namespace TA_Base_Core
{
	class IData;

    class ResourceHelper
    {

    public:
        /**
         * Constructor
         * 
         * This constructor creates a new ResourceHelper for the specified key.
         *
         * @param key The key into the SE_PROFILE table for this Gui
         */
        ResourceHelper(const unsigned long key);

        /**
         * Constructor
         * 
         * This constructor creates a new ResourceHelper for configuration, initiailising
         * the key to 0 - it will be specified once this Profile is written to the database
         * for the first time.
         */
        ResourceHelper();

	    /**
	      * ResourceHelper
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
		ResourceHelper(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Copy Constructor
         */
        ResourceHelper( const ResourceHelper& theHelper);


        /**
         * Destructor
         */
        virtual ~ResourceHelper();


        /**
         * getKey
         *
         * Returns the key for this Profile.
         *
         * @return The key for this Profile as an unsigned long.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        unsigned long getKey();

		/**
         * getSubsystem
         *
         * Returns the subsystem associated with the Resource. If this is the first time data for this
		 * resource has been requested, all the data is loaded from the database.
         *
         * @return The subsystem as an unsigned long
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: This resource was initially loaded from the database.
         */
        unsigned long getSubsystem();

    private:

        // Made private as it is not needed
		ResourceHelper& operator=(const ResourceHelper &);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
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

        bool m_isValidData;
		unsigned long m_key;
		unsigned long m_subsystem;
    };

} // TA_Base_Core

#endif // !defined(RESOURCE_HELPER_H_INCLUDED)

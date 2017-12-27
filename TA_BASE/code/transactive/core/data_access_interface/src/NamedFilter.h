/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/NamedFilter.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * NamedFilter is a container class used to store filterable information under a
  * specific name. It contains no database access code, instead it relies on the
  * NamedFilterAccessFactory for all loading, saving, and deleting.
  */

#if !defined(_NAMED_FILTER_H_)
#define _NAMED_FILTER_H_

#include <map>
#include <string>
#include <vector>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class NamedFilter
    {
    public:
        // Type definitions required for storing filter data.
        typedef std::vector<std::string> StringList;
        typedef std::vector<unsigned long> NumericList;
        typedef std::map<std::string,StringList> StringListMap;
        typedef std::map<std::string,NumericList> NumericListMap;

        // This static is the name that should be given to 'unnamed' filters.
        static const std::string UNNAMED;

        // Constants for encapsulating the list type.
        static const unsigned long LIST_TYPE_STRING;
        static const unsigned long LIST_TYPE_NUMERIC;

        /**
         * Constructor
         */
        NamedFilter();


        /**
         * Copy Constructor
         */
        NamedFilter( const NamedFilter& param );


        /**
         * Destructor
         */
        virtual ~NamedFilter();


        /**
         * Assignment Operator
         */
        virtual NamedFilter& operator=( const NamedFilter& rhs );


        /**
         * clone
         *
         * This method is used by static filters to duplicate themselves. It is
         * the responsibility of the caller to delete the returned object.
         *
         * @return NamedFilter* The new filter.
         */
        virtual NamedFilter* clone();


        /**
         * clear
         *
         * Removes all filter data.
         */
        virtual void clear();


        /**
         * getName
         *
         * Returns the name of this filter.
         *
         * @return const std::string& The current filter name.
         */
        virtual const std::string& getName() const;


        /**
         * setName
         *
         * Sets the name of this filter.
         *
         * @param filterName The new name of the filter.
         */
        virtual void setName( const std::string& filterName );


        /**
         * getStringFilterList
         *
         * Returns a list of all the configured string filters.
         *
         * @return StringList A list of the string filters.
         */
        virtual StringList getStringFilterList() const;


        /**
         * getStringFilter
         *
         * Returns the value for the specified string filter.
         *
         * @param listName The name of the list to return.
         *
         * @return StringList All values in the specified list.
         */
        virtual StringList getStringFilter( const std::string& listName ) const;


        /**
         * setStringFilter
         *
         * Allows a list of string filters to be set in one go.
         *
         * @param listName The name of the list to set.
         * @param stringList The list of strings to add.
         */
        virtual void setStringFilter( const std::string& listName, const StringList& stringList );


        /**
         * addStringFilter
         *
         * Adds the specified data to the specified string list.
         *
         * @param listName The name of the list to add the value to.
         * @param value The value to add to the list.
         */
        virtual void addStringFilter( const std::string& listName, const std::string& value );


        /**
         * removeStringFilter
         *
         * Removes the specified string filter.
         *
         * @param listName The name of the list to remove.
         */
        virtual void removeStringFilter( const std::string& listName );


        /**
         * getNumericFilterList
         *
         * Returns a list of all the configured numeric filters.
         *
         * @return StringList A list of the numeric filters.
         */
        virtual StringList getNumericFilterList() const;


        /**
         * getNumericFilter
         *
         * Returns the value for the specified numeric filter.
         *
         * @param listName The name of the list to return.
         *
         * @return NumericList All values in the specified list.
         */
        virtual NumericList getNumericFilter( const std::string& listName ) const;


        /**
         * setNumericFilter
         *
         * Allows a list of numeric filters to be set in one go.
         *
         * @param listName The name of the list to set.
         * @param numericList The list of numbers to add.
         */
        virtual void setNumericFilter( const std::string& listName, const NumericList& numericList );


        /**
         * addNumericFilter
         *
         * Adds the specified data to the specified numeric list.
         *
         * @param listName The name of the list to add the value to.
         * @param value The value to add to the list.
         */
        virtual void addNumericFilter( const std::string& listName, const unsigned long value );


        /**
         * removeNumericFilter
         *
         * Removes the specified numeric filter.
         *
         * @param listName The name of the list to remove.
         */
        virtual void removeNumericFilter( const std::string& listName );


        /**
         * saveNamedFilter
         *
         * Saves the given named filter.
         *
         * @exception DataException When the filter could not be saved.
         */
        virtual void saveNamedFilter();


        /**
         * deleteNamedFilter
         *
         * Deletes the given named filter. Once the filter has been deleted from the
         * database the filter is marked as unnamed and its contents left alone.
         *
         * @exception DataException When the filter could not be deleted.
         */
        virtual void deleteNamedFilter();

    private:
        /**
         * getApplicationKey
         *
         * Returns the GUI application key of the executing application.
         *
         * @return unsigned long The GUI application key.
         */
        unsigned long getApplicationKey();

        /**
         * getSessionId
         *
         * Returns the ID of the current session.
         *
         * @return std::string The session ID.
         */
        std::string getSessionId();

        /**
         * saveFilterItem
         *
         * Saves the specified filter item.
         *
         * @param filterName The name of the filter being saved.
         * @param listName The name of the list.
         * @param listType The type of the list.
         * @param listItem The item to add to the list.
         */
        void saveFilterItem( const std::string& listName, const unsigned long listType, const std::string& listItem );

        // The name of the current filter.
        std::string m_filterName;

        // The maps used to store the actual filter information.
        StringListMap  m_stringFilters;
        NumericListMap m_numericFilters;

        // The guard for stopping concurrent access to the map.
        mutable TA_Base_Core::NonReEntrantThreadLockable m_filterLock;
    };
};

#endif // !defined(_NAMED_FILTER_H_)


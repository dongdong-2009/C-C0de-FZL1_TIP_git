/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/NamedFilterAccessFactory.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * NamedFilterAccessFactory is a singleton that is used to access named filters saved by an operator
  * for a specified application. It also provides an interface to allow static filters to be registered
  * which apply for all users. These static filters can be made to change their state depending on the
  * current state of the running application.
  */

#if !defined(_NAMED_FILTER_ACCESS_FACTORY_H_)
#define _NAMED_FILTER_ACCESS_FACTORY_H_

#include <map>
#include <vector>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class NamedFilter;
    
    class NamedFilterAccessFactory
    {
    public:
        // Filter Name - ReadWrite pair.
        typedef std::pair<std::string,bool> FilterName;
        typedef std::vector<FilterName> FilterNameList;

        /**
         * getInstance
         *
         * Returns a reference to the only instance of NamedFilterAccessFactory.
         */
        static NamedFilterAccessFactory& getInstance();

        /**
         * registerStaticNamedFilter
         *
         * Allows a static named filter to be registered. These filters will apply to all
         * users for the current application.
         *
         * NOTE: The NamedFilterAccessFactory takes ownership of the filter once it is registered!
         *
         * @param namedFilter The NamedFilter to register.
         */
        void registerStaticNamedFilter( std::auto_ptr<NamedFilter> namedFilter );

        /**
         * getAllFilterNames
         *
         * Returns a list of all filter names available for the specified session.
         *
         * @return FilterNameList The list of filter names.
         *
         * @exception DataException When the filter names could not be retrieved.
         */
        FilterNameList getAllFilterNames();

        /**
         * getNamedFilter
         *
         * Returns the requested named filter.
         *
         * @param filterName The name of the filter to retrieve.
         *
         * @return NamedFilter* The filter.
         *
         * @exception DataException When the data could not be retrieved or converted.
         */
        std::auto_ptr<NamedFilter> getNamedFilter( const std::string& filterName );

    private:
        // Disabled methods.
        NamedFilterAccessFactory();
        virtual ~NamedFilterAccessFactory();
        NamedFilterAccessFactory( const NamedFilterAccessFactory& );
        NamedFilterAccessFactory& operator=( const NamedFilterAccessFactory& );

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
         * Simply returns the current session ID.
         *
         * @return std::string The current session ID.
         */
        std::string getSessionId();

        // Singleton object.
        static NamedFilterAccessFactory* m_instance;

        // The map for storing static filters.
        typedef std::map<std::string,NamedFilter*> StaticFilterMap;
        StaticFilterMap m_staticFilters;

        // The guard for stopping concurrent access to the map.
        TA_Base_Core::NonReEntrantThreadLockable m_staticFiltersLock;
    };
};

#endif // !defined(_NAMED_FILTER_ACCESS_FACTORY_H_)

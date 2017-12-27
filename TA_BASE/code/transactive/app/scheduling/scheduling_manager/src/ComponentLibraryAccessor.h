/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/ComponentLibraryAccessor.h $
  * @author:  San Teo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/03 17:02:56 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  *
  * Adapted from Configuration Editor.
  */


#if !defined(ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include "app/scheduling/scheduling_manager/src/Component.h"
#include <vector>
#include <map>

namespace TA_Base_App
{
    class ComponentLibraryAccessor
    {

    public:
        /**
         * Constructor
         *
         * @param jobtypes the list of jobtypes recognised by the Scheduling Agent
         */
        ComponentLibraryAccessor(std::vector<std::string>& jobtypes);


        /**
         * Destructor
         */
        virtual ~ComponentLibraryAccessor();


        /**
         * loadComponents
         *
         * This will load the corresponding component if it has not already done so and return the 
         * pointer to the IJobDetailsPlugin object.  
         *
         * @param name the name of the component as known by the Scheduling Agent
         *
         * @return pointer to the IJobDetailsPlugin object.  
         */
        IJobDetailsPlugin* getComponent(std::string& name);

        std::string dispToComName( const std::string& strName );

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ComponentLibraryAccessor( const ComponentLibraryAccessor& theComponentLibraryAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ComponentLibraryAccessor& operator=(const ComponentLibraryAccessor& pm);

        void initDispToComName();

        typedef std::map<std::string, Component> ComponentMap;

        // The map of all components recognised by the Scheduling Agent
        ComponentMap m_componentMap;

        std::map<std::string, std::string> m_mapDispToComName;
    };
};
#endif // !defined(ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)

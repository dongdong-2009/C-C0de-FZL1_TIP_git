/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/ComponentLibraryAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  */


#if !defined(ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <vector>

#include "app/system_control/banner/src/Component.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Bus
{
    class BannerFrameworkDialog;
    class IBannerPage;
}

namespace TA_Base_App
{
    class RightsChecker;


    // Function object to sort entities by pkey.
    class EntitySort
    {
    
    public:

        bool operator()(TA_Base_Core::IEntityData* a, TA_Base_Core::IEntityData* b) const
        {
            return (a->getKey() < b->getKey());
        }
    };


    class ComponentLibraryAccessor
    {

    public:
        /**
         * Constructor
         */
	    ComponentLibraryAccessor();


        /**
         * Destructor
         */
        virtual ~ComponentLibraryAccessor();


        /**
         * loadComponents
         *
         * This will load all component libraries and set up the m_allComponents vector. It will also
         * keep track of any component libraries that could not be loaded and display an error
         * message to the user.
         */
        std::vector<Component> getComponents(int xPosition, int yPosition, 
            int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog); 


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


        std::vector<Component> m_components;    // This is the vector of all loaded components
    };
}

#endif // !defined(ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)

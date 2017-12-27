/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ComponentLibraryAccessor.h $
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

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/configuration/configuration_editor/src/Component.h"
#include <vector>

namespace TA_Base_Core
{
    class IComponent;
    class SingleThreadBarrier;
}

namespace TA_Base_App
{
    class RightsChecker;

    class IComponentLibraryAccessorCallback
    {
    public:
        /**
          * updateLoadProgress
          *
          * Is called whenever this is an update in the percentage of components
          * loaded.
          *
          * @param percentage The percentage of components that have been loaded.
          */
        virtual void updateLoadProgress(unsigned char percentage) = 0;
        virtual void setTotalComponentNumber(size_t totalComponentNumber) = 0;
        virtual void setLoadedComponentNumber(size_t loadedComponentNumber) = 0;
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
          *
          * @param callback The object that will receive progress updates.
          */
        void loadComponents(IComponentLibraryAccessorCallback* callback = NULL);
        void loadComponents2(IComponentLibraryAccessorCallback* callback = NULL);

        /**
          * setUpSingletonsInComponents
          *
          * This will set up the singletons in all the component interfaces retrieved from the DLLs.
          */
        void setUpSingletonsInComponents();

        /**
          * determineAvailableComponents
          *
          * This will determine which components have access rights be used by this user
          * It will then put all components that can be used into the m_components vector
          *
          * @param RightsChecker& - This is used to check the rights of each of the components
          */
        void determineAvailableComponents(RightsChecker& checker);

        /**
          * getComponents
          *
          * This will return a reference to the m_components member variable. This should be
          * checked to see if it is empty. It it is empty then either loadComponents()
          * has not been called (programming error) or no libraries were available to be
          * loaded (runtime error).
          *
          * @return std::vector<Component>& - This is the list of components loaded.
          *                                   The ComponentLibraryAccessor object cannot go out of scope
          *                                   while the caller is using this reference. Care must be taken.
          */
        std::vector<Component>& getComponents()
        {
            return m_components;
        }

        void createComponentThread(const std::string& category,
                                           const std::string& component,
                                           const std::string& library,
                                           const std::string& helpFile,
                                           const std::vector<std::string>& entityTypes,
                                           CString& unavailableComponents,
                                           TA_Base_Core::SingleThreadBarrier& barrier);

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ComponentLibraryAccessor(const ComponentLibraryAccessor& theComponentLibraryAccessor) { };

        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ComponentLibraryAccessor& operator=(const ComponentLibraryAccessor& pm);

        /**
          * findOtherTypes
          *
          * This takes all of the loaded components and searches for ones with matching details.
          * If it finds other components with matching category name, component name and DLL then it
          * adds the entity type to the list of strings. IT then removes that item from the list of
          * components so it is not displayed twice.
          *
          * @param vector<IComponent*>::iterator - The currently component we are processing
          * @param vector<IComponent*> - All the components we can search through to find the other matches
          *                              This vector can be modified as items can be removed
          *
          * @return vector<string> - All the entity types for this componet and category.
          */
        std::vector<std::string> findOtherTypes(const std::vector<TA_Base_Core::IComponent*>::iterator& currentComp,
                                                std::vector<TA_Base_Core::IComponent*>& components);

        /**
          * isCategorySetInRunPara
          *
          * Support run parameter to set what categories are accessible for the
          * current operator.
          * --
          */
        bool isCategorySetInRunPara(const std::string& category);

    private:

        std::vector<Component> m_components;    // This is the vector of all loaded components
        TA_Base_Core::NonReEntrantThreadLockable m_componentsLock;
    };
}

#endif // !defined(ComponentLibraryAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)

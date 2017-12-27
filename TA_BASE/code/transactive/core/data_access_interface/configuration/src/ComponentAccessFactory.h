/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/configuration/src/ComponentAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ComponentAccessFactory is a singleton that is used to retrieve 
  * IComponent objects.
  */


#if !defined(ComponentAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define ComponentAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

#include <vector>
#include <string>

namespace TA_Base_Core
{
    class IComponent;

    class ComponentAccessFactory
    {

    public:
        virtual ~ComponentAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static ComponentAccessFactory& getInstance()
        {
            static ComponentAccessFactory fact;
            return fact;
        }


        /**
         * getAllComponents
         *
         * Retrieves the configuration components from the database
         *
         * @return A vector of pointers to IComponent objects. The caller must delete
         *         these objects.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the data retrieved
         *            is of the wrong type.
         */
        std::vector<IComponent*> getAllComponents();


    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        ComponentAccessFactory() { };
        ComponentAccessFactory& operator=(const ComponentAccessFactory &) { };
        ComponentAccessFactory( const ComponentAccessFactory& ) {};


        // This is used to indicate if an entry has no entity type associated with it.
        static const std::string NO_ENTITY_TYPE;

    };
} //end namespace TA_Base_Core

#endif // !defined(ComponentAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PlanAccessFactory is a singleton that is used to retrieve IPlan objects.
  */


#if !defined(PlanAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define PlanAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

#include <string>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IPlan;

    class PlanAccessFactory
    {

    public:
        virtual ~PlanAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static PlanAccessFactory& getInstance();


        /**
         * getPlanByPath
         *
         * Retrieves a plan based on a plan path.
         * The path must be in the format "[/<category1>/<categoryN>]/<planName>"
         *
         * @return An IPlan* object. It is the caller's responsibility to delete it.
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
        IPlan* getPlanByPath(const std::string& planPath) const;


        /**
         * getPlanPathByNodeId
         *
         * Retrieves a plan path based on a plan id.
         *
         * @return a string.
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
        std::string getPlanPathByNodeId(const unsigned long ulNodeId) const;


        /**
         * getPlanName
         *
         * Extract the plan's name from the specified plan path.
         * The path must be in the format "[/<category1>/<categoryN>]/<planName>"
         *
         * @return std::string - name of the plan identified by an absolute path.
         */      
        std::string getPlanName(const std::string& planPath) const;


    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        PlanAccessFactory() { };
        PlanAccessFactory& operator=(const PlanAccessFactory &) { };
        PlanAccessFactory( const PlanAccessFactory& ) {};

        static PlanAccessFactory* s_instance;
        static ReEntrantThreadLockable m_singletonLock;

    };
} //end namespace TA_Base_Core

#endif // !defined(PlanAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * DerivedDataPointAccessFactory is a singleton that is used to retrieve derived datapoint objects
  */


#if !defined(DerivedDataPointAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define ComponentAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

#include <vector>
#include <set>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IConfigDerivedDataPoint;

    class DerivedDataPointAccessFactory
    {

    public:
        virtual ~DerivedDataPointAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static DerivedDataPointAccessFactory& getInstance();


        /**
         * getDerivedDataPoint
         *
         * This retrieves all configuration for a single derived datapoint
         *
         * @param unsigned long - The entity pkey of the derived data point
         * @param bool - Indicates whether a read and write object should be returned or just a read-only
         *               object. Default is false (a read-only object).
         *
         * @return IConfigDerivedDataPoint* - The object containing all details. This must be
         *                                    deleted by the calling class. This will return NULL if the 
         *                                    key passed does not belong to a derived datapoint.
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
        IConfigDerivedDataPoint* getDerivedDataPoint(unsigned long pkey, bool readAndWrite = false);


        /**
         * newDerivedDataPoint
         *
         * This creates a new derived data point. No key will be set on the new datapoint and this
         * must be set as soon as the datapoint is first applied to the database.
         *
         * @return IConfigDerivedDataPoint* - The new derived data point object
         */
        IConfigDerivedDataPoint* newDerivedDataPoint();


        /**
         * copyDerivedDataPoint
         *
         * This will copy the derived data point passed in. No key will be set on the new datapoint and
         * this must be set as soon as the datapoint is first applied to the database.
         *
         * @param IConfigDerivedDataPoint* - The derived data point to copy
         *
         * @return IConfigDerivedDataPoint* - The new derived data point object populated with the same data
         *                                    as the derived data point that was passed in.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        IConfigDerivedDataPoint* copyDerivedDataPoint(IConfigDerivedDataPoint* dpToCopy);

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		void bulkLoadByAgentKey(unsigned long agentKey);

        void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey);

    private:
		std::set<unsigned long>		m_loadEntityKeyList;
	    bool isValidEntityKey(unsigned long entityKey);
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        DerivedDataPointAccessFactory() { };
        DerivedDataPointAccessFactory& operator=(const DerivedDataPointAccessFactory &) { };
        DerivedDataPointAccessFactory( const DerivedDataPointAccessFactory& ) {};


        static TA_Base_Core::ReEntrantThreadLockable s_singletonLock;
        static DerivedDataPointAccessFactory* s_instance;
    };
} //end namespace TA_Base_Core

#endif // !defined(DerivedDataPointAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)

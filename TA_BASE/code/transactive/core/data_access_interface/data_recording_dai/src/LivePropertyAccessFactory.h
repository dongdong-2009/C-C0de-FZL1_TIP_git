#ifndef LIVE_PROPERTY_ACCESS_FACTORY_H
#define LIVE_PROPERTY_ACCESS_FACTORY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <vector>
#include <map>

#include "core/data_access_interface/data_recording_dai/src/ILivePropertyData.h"
#include "core/data_access_interface/data_recording_dai/src/IConfigLivePropertyData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class LivePropertyAccessFactory
    {

    public:

        /**
         * getInstance
         *
         * Static access to the singleton
         *
         * @return singleton instance of LivePropertyAccessFactory
         *
         */
        static LivePropertyAccessFactory& getInstance();


        /**
         * getLivePropertyData
         *
         * @param   typeKey     the type key for the Live Property to retrieve
         * @param   name        the name of the Live Property to retrieve
         * @param   writable    whether or not the Live Property Data shall be editable
         *
         * @return  pointer to the ILivePropertyData object
         *
         * @exception   DataException       An error occurred with the data used in the 
         *                                  database query.
         *
         * @exception   DatabaseException   An error occurred trying to connect to the 
         *                                  database.
         */
        ILivePropertyDataPtr getLivePropertyData( unsigned long typeKey, const std::string& name, bool writable = false );


        /**
         * getLivePropertiesByType
         *
         * @param   typeKey     the type key for the Live Properties to retrieve
         * @return  pointer(s) to the ILivePropertyData object(s)
         *
         * @exception   DataException       An error occurred with the data used in the 
         *                                  database query.
         *
         * @exception   DatabaseException   An error occurred trying to connect to the 
         *                                  database.
         */
        std::vector<ILivePropertyDataPtr> getLivePropertiesByType( unsigned long typeKey, bool writable = false );


        /**
         * createLiveProperty
         *
         * @return  pointer to a new Live Property Data object
         *
         */
        IConfigLivePropertyDataPtr createLiveProperty();
        

    private:

        /**
         * Private constructor
         *
         */
        LivePropertyAccessFactory();


        /**
         * Private Destructor
         *
         */
        ~LivePropertyAccessFactory();


        /**
         * Prevent copying
         *
         */
        LivePropertyAccessFactory( const LivePropertyAccessFactory& copyMe );
        LivePropertyAccessFactory& operator=( const LivePropertyAccessFactory& copyMe );


        static LivePropertyAccessFactory*   s_singleton;
        static NonReEntrantThreadLockable   s_singletonLock;

        ReEntrantThreadLockable             m_lock;
        std::map<unsigned long, std::vector<ILivePropertyDataPtr> >                     m_LivePropertyMap;
        typedef std::map<unsigned long, std::vector<ILivePropertyDataPtr> >             LivePropertyMap;
        typedef std::map<unsigned long, std::vector<ILivePropertyDataPtr> >::iterator   LivePropertyMapPtr;
    };
}
#endif // LIVE_PROPERTY_ACCESS_FACTORY_H

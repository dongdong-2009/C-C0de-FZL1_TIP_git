#if !defined(_DBCACHELOOKUPEX_H__05A736E3_0A86_4181_A46D_61BF5C359D1C__INCLUDED_)
#define _DBCACHELOOKUPEX_H__05A736E3_0A86_4181_A46D_61BF5C359D1C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DbCacheLookupEx.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is an extension of DbCacheLookup. It enables the user to provide a pointer to a funtion
  * used to extract the complete list of database objects. DbCacheLookup only supports a reader 
  * function for a single object with a specific key.
  *
  */

namespace TA_Base_Bus
{
    template <typename Key, typename Value, typename AccessFactory> 
    class DbCacheLookupEx : public DbCacheLookup<Key, Value, AccessFactory>
    {
    public:
        typedef Value* (AccessFactory::*GetValueFn)(const Key, const bool);
		typedef std::vector<Value*> (AccessFactory::*GetValueListFn)(const bool);


    public:
        DbCacheLookupEx(std::map<Key,Value*>& dbMap, AccessFactory& factory, GetValueFn getValueFn, GetValueListFn getValueListFn);
        virtual ~DbCacheLookupEx();

        /*
         * Sets the contents of the value map using the value list getter function
         * provided at instantiation.
         *
         */
        void populate();

    private:
        DbCacheLookupEx(const DbCacheLookupEx<Key, Value, AccessFactory>& dbCacheLookupEx);
        DbCacheLookupEx<Key, Value, AccessFactory>& operator=(const DbCacheLookupEx<Key, Value, AccessFactory>& dbCacheLookupEx);


    private:
		AccessFactory& m_dataAccessFactory;
        GetValueListFn m_getValueListFn;
    };
}


#include "bus/security/duty_monitor_framework/src/DbCacheLookupEx.inl"


#endif // !defined(_DBCACHELOOKUPEX_H__05A736E3_0A86_4181_A46D_61BF5C359D1C__INCLUDED_)

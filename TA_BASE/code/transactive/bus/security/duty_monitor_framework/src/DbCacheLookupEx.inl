/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DbCacheLookupEx.inl $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/03 14:40:45 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This is an extension of DbCacheLookup. It enables the user to provide a pointer to a funtion
  * used to extract the complete list of database objects. DbCacheLookup only supports a reader 
  * function for a single object with a specific key.
  *
  */

namespace TA_Base_Bus
{
    template <typename Key, typename Value, typename AccessFactory>
    DbCacheLookupEx<Key, Value, AccessFactory>::DbCacheLookupEx(std::map<Key,Value*>& dbMap, AccessFactory& factory, 
                                                                GetValueFn getValueFn, GetValueListFn getValueListFn) :

        DbCacheLookup<Key, Value, AccessFactory>(dbMap, factory, getValueFn),
        m_dataAccessFactory(factory),
        m_getValueListFn(getValueListFn)
    {
        FUNCTION_ENTRY("DbCacheLookupEx(std::map<Key,Value*>& dbMap, AccessFactory& factory, "
                                        "GetValueFn getValueFn, GetValueListFn getValueListFn)");
        FUNCTION_EXIT;
    }


    template <typename Key, typename Value, typename AccessFactory>
    DbCacheLookupEx<Key, Value, AccessFactory>::~DbCacheLookupEx()
    {
        FUNCTION_ENTRY("~DbCacheLookupEx()");

        m_getValueListFn = NULL;

        FUNCTION_EXIT;
    }


    template <typename Key, typename Value, typename AccessFactory>
    void DbCacheLookupEx<Key, Value, AccessFactory>::populate()
    {
        FUNCTION_ENTRY("populate()");

        TA_ASSERT(m_getValueListFn != NULL, "Get value list function pointer is NULL");

        DbCacheLookup<Key, Value, AccessFactory>::set((m_dataAccessFactory.*m_getValueListFn)(false));

        FUNCTION_EXIT;
    }
}


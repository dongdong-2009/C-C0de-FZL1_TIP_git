/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StlMapUtilities.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Function objects for doing various things with maps.
  */ 

#include <set>

namespace //unnamed
{
    //
    // MapEquals
    //
    // This funtion object is used to do compare the value of a map entry
    // with a given value. 
    //
    // Example: It can be used in conjunction with std::find_if()
    // to search a map for a particular value. Note that doing this is not 
    // very time efficient but can still sometimes be useful.
    // 
    template <class K, class V>
    class MapEquals
    {
    public:
        MapEquals(const V& value)
            :m_value(value)
        {}

        bool operator() (std::pair<K, V> elem)
        {
            return elem.second == m_value;
        }
    private:
        V m_value;
    };


    //
    // KeyInserter
    //
    // The funtion object is used to insert the key of a map entry into
    // a given set.
    //
    // Example: It can be used in conjunction with std::for_each() 
    // to retrieve all the keys in a map.
    //
    template <class K, class V>
    class KeyInserter
    {
    public:
        KeyInserter(std::set<K>& keySet)
            :m_keySet(keySet)
        {}

        void operator() (std::pair<K, V> elem)
        {
            m_keySet.insert(elem.first);
        }
    private:
        std::set<K>& m_keySet;
    };
}


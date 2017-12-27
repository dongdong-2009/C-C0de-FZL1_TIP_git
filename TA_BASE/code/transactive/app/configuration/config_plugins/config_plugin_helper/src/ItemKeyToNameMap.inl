template <class T>
void ItemKeyToNameMap<T>::refreshUsing(std::vector<T*>& dataArray)
{
    m_keyToNameMap.clear();

    typename T;
    std::vector<T*>::iterator it;
    
    // Now step through each one and:
    // 1) Add it's name and key to the map we want ot return
    // 2) Delete the ILocation object when we're done
    for (it = dataArray.begin(); it != dataArray.end(); ++it)
    {
        if (*it != NULL)
        {
            try
            {
                m_keyToNameMap.insert( KeyToNameMap::value_type( (*it)->getKey(), (*it)->getName() ) );
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This Location will not be added to the map");
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This Location will not be added to the map");
            }
    
            // Now we're finished with this iterator so we might as well delete what it contains so we
            // don't have to do it later.
            delete *it;
            *it = NULL;
        }
    }
    dataArray.clear();
}


template <class T>
std::string ItemKeyToNameMap<T>::getName(unsigned long key) const
{
    KeyToNameMap::const_iterator it = m_keyToNameMap.find(key);
    if (it != m_keyToNameMap.end())
    {
        return it->second;
    }
    
    // Can't find it
    return "";
}


template <class T>
unsigned long ItemKeyToNameMap<T>::getKey(const std::string& name)
{
    KeyToNameMap::iterator it = m_keyToNameMap.begin();
    for (; it != m_keyToNameMap.end(); ++it)
    {
        if (it->second == name)
        {
            return it->first;
        }
    }
    
    // Can't find it
    return -1;
}

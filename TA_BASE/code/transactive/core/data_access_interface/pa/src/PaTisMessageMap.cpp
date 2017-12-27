 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisMessageMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTisMessageMap is an implementation of IPaTisMessageMap. It holds the data specific to an PaTisMessageMap entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaTisMessageMap.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaTisMessageMap::PaTisMessageMap(const unsigned long key)
     : m_paTisMessageMapHelper ( new PaTisMessageMapHelper(key))
    {}


    PaTisMessageMap::PaTisMessageMap(unsigned long row, TA_Base_Core::IData& data)
     : m_paTisMessageMapHelper(new PaTisMessageMapHelper(row, data))
    {
    }


    PaTisMessageMap::~PaTisMessageMap() 
	{
        delete m_paTisMessageMapHelper;
        m_paTisMessageMapHelper=NULL;
    }

    unsigned long PaTisMessageMap::getKey()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getKey();
    }

    unsigned long PaTisMessageMap::getPaDvaMessageKey()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getPaDvaMessageKey();
    }

    
    unsigned long PaTisMessageMap::getTisMessageTag()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisMessageTag();
    }


    unsigned long PaTisMessageMap::getTisLibrarySection()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisLibrarySection();
    }


    unsigned long PaTisMessageMap::getTisLibraryVersion()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisLibraryVersion();
    }

    void PaTisMessageMap::invalidate()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        m_paTisMessageMapHelper->invalidate();
    }

    std::string PaTisMessageMap::getName()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The m_paTisMessageMapHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_paTisMessageMapHelper->getKey();

        return name.str();
    }


} // closes TA_Base_Core

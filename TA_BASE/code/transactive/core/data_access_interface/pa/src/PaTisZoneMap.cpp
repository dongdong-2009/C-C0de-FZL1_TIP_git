 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisZoneMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTisZoneMap is an implementation of IPaTisZoneMap. It holds the data specific to an PaTisZoneMap entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaTisZoneMap.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaTisZoneMap::PaTisZoneMap(const unsigned long key)
     : m_PaTisZoneMapHelper ( new PaTisZoneMapHelper(key))
    {}


    PaTisZoneMap::PaTisZoneMap(unsigned long row, TA_Base_Core::IData& data)
     : m_PaTisZoneMapHelper(new PaTisZoneMapHelper(row, data))
    {
    }


    PaTisZoneMap::~PaTisZoneMap() 
	{
        delete m_PaTisZoneMapHelper;
        m_PaTisZoneMapHelper=NULL;
    }

    unsigned long PaTisZoneMap::getKey()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getKey();
    }

    unsigned long PaTisZoneMap::getPaZoneKey()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getPaZoneKey();
    }

    
    std::string PaTisZoneMap::getTisZoneEntity()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getTisZoneEntity();
    }


    void PaTisZoneMap::invalidate()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        m_PaTisZoneMapHelper->invalidate();
    }

    std::string PaTisZoneMap::getName()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The m_PaTisZoneMapHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_PaTisZoneMapHelper->getKey();

        return name.str();
    }


} // closes TA_Base_Core

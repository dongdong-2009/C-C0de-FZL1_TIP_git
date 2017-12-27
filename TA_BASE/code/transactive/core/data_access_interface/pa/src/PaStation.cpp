 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaStation.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaStation is an implementation of IPaStation. It holds the data specific to an PaStation entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaStation.h"
#include "core/data_access_interface/src/Location.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaStation::PaStation(const unsigned long key)
     : m_paStationHelper ( new PaStationHelper(key))
    {}


    PaStation::PaStation(unsigned long row, TA_Base_Core::IData& data)
     : m_paStationHelper(new PaStationHelper(row, data))
    {
    }


    PaStation::~PaStation() 
	{
        delete m_paStationHelper;
        m_paStationHelper=NULL;
    }

    unsigned long PaStation::getKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getKey();
    }  
    
    unsigned long PaStation::getLocationKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getLocationKey();
    }


    void PaStation::invalidate()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        m_paStationHelper->invalidate();
    }

    std::string PaStation::getName()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The m_paStationHelper pointer is null.");

		Location location(m_paStationHelper->getLocationKey());
		
		return location.getName();
    }


} // closes TA_Base_Core

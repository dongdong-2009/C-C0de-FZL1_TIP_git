 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaFasTriggeredBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * PaFasTriggeredBroadcast is an implementation of IPaFasTriggeredBroadcast. It holds the data specific to an PaFasTriggeredBroadcast entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcast.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaFasTriggeredBroadcast::PaFasTriggeredBroadcast(const unsigned long key)
     : m_PaFasTriggeredBroadcastHelper ( new PaFasTriggeredBroadcastHelper(key))
    {}


    PaFasTriggeredBroadcast::PaFasTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data)
     : m_PaFasTriggeredBroadcastHelper(new PaFasTriggeredBroadcastHelper(row, data))
    {
    }


    PaFasTriggeredBroadcast::~PaFasTriggeredBroadcast() 
	{
        delete m_PaFasTriggeredBroadcastHelper;
        m_PaFasTriggeredBroadcastHelper=NULL;
    }

    unsigned long PaFasTriggeredBroadcast::getKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getKey();
    }

    unsigned long PaFasTriggeredBroadcast::getFasAlarmEntityKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getFasAlarmEntityKey();
    }


    unsigned long PaFasTriggeredBroadcast::getLocationKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getLocationKey();
    }
    
    unsigned long PaFasTriggeredBroadcast::getPaCyclicTimes()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaCyclicTimes();
    }


    unsigned long PaFasTriggeredBroadcast::getPaZoneGroupKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaZoneGroupKey();
    }


    unsigned long PaFasTriggeredBroadcast::getPaDvaMessageKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaDvaMessageKey();
    }


    bool          PaFasTriggeredBroadcast::getIsEnabledParameter()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getIsEnabledParameter();
    }


    void PaFasTriggeredBroadcast::invalidate()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        m_PaFasTriggeredBroadcastHelper->invalidate();
    }

    std::string PaFasTriggeredBroadcast::getName()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The m_PaFasTriggeredBroadcastHelper pointer is null.");

        std::ostringstream name;
		name << "PA ATS Triggered Broadcast " << m_PaFasTriggeredBroadcastHelper->getKey();

        return name.str();
    }

} // closes TA_Base_Core

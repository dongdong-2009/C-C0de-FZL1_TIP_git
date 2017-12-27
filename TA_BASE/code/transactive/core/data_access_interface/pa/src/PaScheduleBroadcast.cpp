 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * PaScheduleBroadcast is an implementation of IPaScheduleBroadcast. It holds the data specific to an PaScheduleBroadcast entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaScheduleBroadcastHelper.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcast.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

PaScheduleBroadcast::PaScheduleBroadcast( const unsigned long key ) : 
m_pPaScheduleBroadcastHelper( new PaScheduleBroadcastHelper( key ))
{
}

PaScheduleBroadcast::PaScheduleBroadcast( unsigned long row, TA_Base_Core::IData& data ) : 
m_pPaScheduleBroadcastHelper( new PaScheduleBroadcastHelper( row, data ))
{
}

PaScheduleBroadcast::~PaScheduleBroadcast() 
{
    delete m_pPaScheduleBroadcastHelper;
    m_pPaScheduleBroadcastHelper=NULL;
}

unsigned long PaScheduleBroadcast::getKey()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getKey();
}

unsigned long PaScheduleBroadcast::getLocationKey()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getLocationKey();
}

unsigned long PaScheduleBroadcast::getDvaMsgKey()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getDvaMsgKey();
}

unsigned long PaScheduleBroadcast::getCyclicTimes()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getCyclicTimes();
}

unsigned long PaScheduleBroadcast::getPriorityGroup()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getPriorityGroup();
}

time_t PaScheduleBroadcast::getStartTime()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getStartTime();
}

time_t PaScheduleBroadcast::getEndTime()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getEndTime();
}

std::string PaScheduleBroadcast::getScheduleName()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getScheduleName();
}

std::string PaScheduleBroadcast::getIntervalType()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getIntervalType();
}

std::string PaScheduleBroadcast::getSourceType()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getSourceType();
}

std::string PaScheduleBroadcast::getStatus()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getStatus();
}

std::vector<unsigned long> PaScheduleBroadcast::getZoneKeys()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getZoneKeys();
}

bool PaScheduleBroadcast::getIsEnable()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getIsEnable();
}

bool PaScheduleBroadcast::getAutoDel()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    return m_pPaScheduleBroadcastHelper->getAutoDel();
}

void PaScheduleBroadcast::invalidate()
{
    TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
    m_pPaScheduleBroadcastHelper->invalidate();
}

} // closes TA_IRS_Core

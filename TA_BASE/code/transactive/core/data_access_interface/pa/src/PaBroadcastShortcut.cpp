 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaBroadcastShortcut.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * PaBroadcastShortcut is an implementation of IPaBroadcastShortcut. It holds the data specific to an PaBroadcastShortcut entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaBroadcastShortcutHelper.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcut.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

PaBroadcastShortcut::PaBroadcastShortcut( const unsigned long key ) : 
m_pPaBroadcastShortcutHelper( new PaBroadcastShortcutHelper( key ))
{
}

PaBroadcastShortcut::PaBroadcastShortcut( unsigned long row, TA_Base_Core::IData& data ) : 
m_pPaBroadcastShortcutHelper( new PaBroadcastShortcutHelper( row, data ))
{
}

PaBroadcastShortcut::~PaBroadcastShortcut() 
{
    delete m_pPaBroadcastShortcutHelper;
    m_pPaBroadcastShortcutHelper=NULL;
}

unsigned long PaBroadcastShortcut::getKey()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
    return m_pPaBroadcastShortcutHelper->getKey();
}

unsigned long PaBroadcastShortcut::getLocationKey()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
    return m_pPaBroadcastShortcutHelper->getLocationKey();
}

unsigned long PaBroadcastShortcut::getShortcutNo()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
    return m_pPaBroadcastShortcutHelper->getShortcutNo();
}

unsigned long PaBroadcastShortcut::getDVAMsgKey()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
    return m_pPaBroadcastShortcutHelper->getDVAMsgKey();
}

void PaBroadcastShortcut::invalidate()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
    m_pPaBroadcastShortcutHelper->invalidate();
}

std::string PaBroadcastShortcut::getName()
{
    TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The m_pPaBroadcastShortcutHelper pointer is null.");

    std::ostringstream name;
	name << "PA Broadcast Shortcut " << m_pPaBroadcastShortcutHelper->getKey();

    return name.str();
}

} // closes TA_IRS_Core

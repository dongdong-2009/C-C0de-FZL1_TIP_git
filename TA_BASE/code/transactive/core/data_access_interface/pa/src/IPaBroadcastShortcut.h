/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/IPaDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * IPaDvaMessage is an interface to a PaDvaMessage object.  It allows the PA DVA Message object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#ifndef IPABROADCASTSHORTCUT__INCLUDED_
#define IPABROADCASTSHORTCUT__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

class IPaBroadcastShortcut : public virtual TA_Base_Core::IItem
{
public:
    virtual unsigned long getKey() = 0;
    virtual unsigned long getLocationKey() = 0;
    virtual unsigned long getShortcutNo() = 0;
    virtual unsigned long getDVAMsgKey() = 0;

    /**
     * invalidate
     *
     * Make the data contained by this item as invalid. The next call to get...() 
     * following a call to invalidate() will cause all the data to be reloaded from
     * the database.
     */
    virtual void invalidate() =0;
};

typedef std::vector<IPaBroadcastShortcut*> IPaBroadcastShortcuts;
typedef IPaBroadcastShortcuts::iterator IPaBroadcastShortcutsIt;

} //close namespace TA_IRS_Core


#endif // !defined(IPaDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

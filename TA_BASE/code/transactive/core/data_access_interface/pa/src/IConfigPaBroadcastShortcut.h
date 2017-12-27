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

#ifndef ICONFIGPABROADCASTSHORTCUT__INCLUDED_
#define ICONFIGPABROADCASTSHORTCUT__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaBroadcastShortcut.h"

namespace TA_Base_Core
{

class IConfigPaBroadcastShortcut : public virtual TA_Base_Core::IConfigItem, public virtual IPaBroadcastShortcut
{
public:
    virtual void setLocationKey( const unsigned long ulLocationKey ) = 0;
    virtual void setShortcutNo( const unsigned long ulShortcutNo ) = 0;
    virtual void setDVAMsgKey( const unsigned long ulDVAMsgKey ) = 0;
    
    /**
     * deleteThisObject
     *
     * Removes this PaDvaMessage from the database. 
     * The calling application MUST then delete this PaDvaMessage object, as it makes no sense
     * to keep it any longer.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a 
     *            problem writing the data to the database.
     *
     * pre: Either - this PaDvaMessage was initially loaded from the database
     *      OR     - writePaDvaMessageData() has already been called
     *      This PaDvaMessage has not been deleted
     */
    virtual void deleteThisObject() = 0;
};

} //close namespace TA_IRS_Core


#endif // !defined(IPaDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

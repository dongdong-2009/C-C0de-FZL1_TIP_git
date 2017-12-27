 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessage is an implementation of IPaDvaMessage. It holds the data specific to an PaDvaMessage entry
  * in the database, and allows read-only access to that data.
  */


#ifndef PABROADCASTSHORTCUT__INCLUDED_
#define PABROADCASTSHORTCUT__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/IPaBroadcastShortcut.h"

namespace TA_Base_Core
{

class IData;
class PaBroadcastShortcutHelper;

class PaBroadcastShortcut : public IPaBroadcastShortcut
{
public:
    /**
     * PaDvaMessage (constructor)
     *
     * Construct an PaDvaMessage class based around a key.
     *
     * @param key The key of this PaDvaMessage in the database
     */
    PaBroadcastShortcut(const unsigned long key);

    /**
     * PaDvaMessage (constructor)
     *
     * Constructs a new instance using the information provided in IData interface
     *
     * @see PaDvaMessageHelper::PaDvaMessageHelper(IData*)
     *
     */
    PaBroadcastShortcut(unsigned long row, TA_Base_Core::IData& data);

    virtual ~PaBroadcastShortcut();

    virtual unsigned long getKey();
    virtual unsigned long getLocationKey();
    virtual unsigned long getShortcutNo();
    virtual unsigned long getDVAMsgKey();

    /**
     * getName
     *
     * Returns the name of this item for the IItem interface.  For the PA DVA Messages this is 
     * just the key in a string format e.g. "PA Dva Message 1".
     *
     * @return The key description for this item as a std::string
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be 
     *            converted to the required format (e.g. ACCESSLEVEL should be an 
     *            unsigned long), or if the wrong ammount of data is retrieved.
     */
    virtual std::string getName();

    /**
     * invalidate
     *
     * Make the data contained by this PaDvaMessage as invalid. The next call to get...() 
     * following a call to invalidate() will cause all the data to be reloaded from
     * the database.
     */
    virtual void invalidate();

private:

    // Assignment PaDvaMessage not used so it is made private
	PaBroadcastShortcut& operator=( const PaBroadcastShortcut & );

    // Copy constructor should not be used and has therefore been made private. If a copy
    // constructor is required care should be taken. The copy constructor fo the PaDvaMessageHelper
    // has only been written for ConfigLocation objects and will not copy the PaDvaMessage helper
    // for a read-only object (ie this one).
    PaBroadcastShortcut( const PaBroadcastShortcut& thePaDvaMessage );  

    PaBroadcastShortcutHelper* m_pPaBroadcastShortcutHelper;
};

} // closes TA_IRS_Core

#endif // !defined(PaDvaMessage_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

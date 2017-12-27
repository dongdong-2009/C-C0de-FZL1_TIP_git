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


#ifndef PASCHEDULEBROADCAST__INCLUDED_
#define PASCHEDULEBROADCAST__INCLUDED_

#include "core/data_access_interface/pa/src/IPaScheduleBroadcast.h"

namespace TA_Base_Core
{

class IData;
class PaScheduleBroadcastHelper;

class PaScheduleBroadcast : public IPaScheduleBroadcast
{
public:
    /**
     * PaDvaMessage (constructor)
     *
     * Construct an PaDvaMessage class based around a key.
     *
     * @param key The key of this PaDvaMessage in the database
     */
    PaScheduleBroadcast(const unsigned long key);

    /**
     * PaDvaMessage (constructor)
     *
     * Constructs a new instance using the information provided in IData interface
     *
     * @see PaDvaMessageHelper::PaDvaMessageHelper(IData*)
     *
     */
    PaScheduleBroadcast(unsigned long row, TA_Base_Core::IData& data);

    virtual ~PaScheduleBroadcast();

    virtual unsigned long getKey();
    virtual unsigned long getLocationKey();
    virtual unsigned long getDvaMsgKey();
    virtual unsigned long getCyclicTimes();
    virtual unsigned long getPriorityGroup();
    virtual time_t getStartTime();
    virtual time_t getEndTime();
    virtual std::string getScheduleName();
    virtual std::string getIntervalType();
    virtual std::string getSourceType();
    virtual std::string getStatus();
    virtual bool getIsEnable();
    virtual bool getAutoDel();
    virtual std::vector<unsigned long> getZoneKeys();

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
	PaScheduleBroadcast& operator=( const PaScheduleBroadcast & );

    // Copy constructor should not be used and has therefore been made private. If a copy
    // constructor is required care should be taken. The copy constructor fo the PaDvaMessageHelper
    // has only been written for ConfigLocation objects and will not copy the PaDvaMessage helper
    // for a read-only object (ie this one).
    PaScheduleBroadcast( const PaScheduleBroadcast& thePaDvaMessage );  

    PaScheduleBroadcastHelper* m_pPaScheduleBroadcastHelper;
};

} // closes TA_IRS_Core

#endif // !defined(PASCHEDULEBROADCAST__INCLUDED_)

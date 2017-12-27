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

#ifndef IPASCHEDULEBROADCAST__INCLUDED_
#define IPASCHEDULEBROADCAST__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Core
{

class IPaScheduleBroadcast
{
public:
    virtual unsigned long getKey() = 0;
    virtual unsigned long getLocationKey() = 0;
    virtual unsigned long getDvaMsgKey() = 0;
    virtual unsigned long getCyclicTimes() = 0;
    virtual unsigned long getPriorityGroup() = 0;
    virtual time_t getStartTime() = 0;
    virtual time_t getEndTime() = 0;
    virtual std::string getScheduleName() = 0;
    virtual std::string getIntervalType() = 0;
    virtual std::string getSourceType() = 0;
    virtual std::string getStatus() = 0;
    virtual std::vector<unsigned long> getZoneKeys() = 0;
    virtual bool getIsEnable() = 0;
    virtual bool getAutoDel() = 0;

    virtual void invalidate() =0;
};

typedef std::vector<IPaScheduleBroadcast*> IPaScheduleBroadcasts;
typedef IPaScheduleBroadcasts::iterator IPaScheduleBroadcastsIt;

} //close namespace TA_IRS_Core


#endif // !defined(IPASCHEDULEBROADCAST__INCLUDED_)

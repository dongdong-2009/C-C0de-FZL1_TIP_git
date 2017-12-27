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

#ifndef ICONFIGPASCHEDULEBROADCAST__INCLUDED_
#define ICONFIGPASCHEDULEBROADCAST__INCLUDED_

#include "core/data_access_interface/pa/src/IPaScheduleBroadcast.h"

namespace TA_Base_Core
{

class IConfigPaScheduleBroadcast : public virtual IPaScheduleBroadcast
{
public:
    virtual void setLocationKey( unsigned long ulLocationKey ) = 0;
    virtual void setDvaMsgKey( unsigned long ulDvaMsgKey ) = 0;
    virtual void setCyclicTimes( unsigned long ulCyclicTimes ) = 0;
    virtual void setPriorityGroup( unsigned long ulPriorityGroup ) = 0;
    virtual void setStartTime( time_t tmStartTime ) = 0;
    virtual void setEndTime( time_t tmEndTime ) = 0;
    virtual void setScheduleName( const std::string& strScheduleName ) = 0;
    virtual void setIntervalType( const std::string& strIntervalType ) = 0;
    virtual void setSourceType( const std::string& strSourceType ) = 0;
    virtual void setStatus( const std::string& strStatus ) = 0;
    virtual void setZoneKeys( const std::vector<unsigned long>& vecZoneKeys ) = 0;
    virtual void setIsEnable( bool bEnable ) = 0;
    virtual void setAutoDel( bool bAutoDel ) = 0;

    virtual void applyChanges() = 0;
    virtual void deleteThisObject() = 0;
};

} //close namespace TA_IRS_Core


#endif // !defined(ICONFIGPASCHEDULEBROADCAST__INCLUDED_)

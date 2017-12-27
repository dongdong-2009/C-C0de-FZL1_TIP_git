#if !defined(_ICONFIGDATAOBSERVER_H__B63809FB_769E_4907_8CA1_C6B4F8A2E692__INCLUDED_)
#define _ICONFIGDATAOBSERVER_H__B63809FB_769E_4907_8CA1_C6B4F8A2E692__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/IConfigDataObserver.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class should be implemented by clients who want to be notified when any
  * of the config data held by DataCache is changed in the database.
  * Note that DataCache does not automatically update its contents when config
  * changes occur, it merely passes the notification onwards via this interface.
  *
  */

namespace TA_Base_Bus
{
    class IConfigDataObserver : public SeObserver<DataCache>
    {
    public:
        IConfigDataObserver(DataCache& dataCache);
        virtual ~IConfigDataObserver();

        /**
          * Implements SeObserver::update(). Gets the update from
          * DataCache and calls dataCached() with it.
          *
          */
        virtual void update(DataCache& dataCache);

        /** 
          * This is called when a config change occurs. 
          * Observers should implement this method.
          *
          */
        virtual void dataChanged(const DataUpdate& dataUpdate) = 0;
    };
}

#endif // !defined(_ICONFIGDATAOBSERVER_H__B63809FB_769E_4907_8CA1_C6B4F8A2E692__INCLUDED_)

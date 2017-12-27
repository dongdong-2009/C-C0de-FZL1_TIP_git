#if !defined(_ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_)
#define _ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/ISessionObserver.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class should be implemented by clients who want to be notified when  
  * SessionCache contents change due to operator login, logout and override events.
  *
  */

namespace TA_Base_Bus
{
    class ISessionObserver : public SeObserver<SessionCache>
    {
    public:
        ISessionObserver(SessionCache& sessionCache);
        virtual ~ISessionObserver();

        /**
          * Implements SeObserver::update(). Gets the update from
          * SessionCache and calls sessionChanged() with it.
          *
          */
        virtual void update(SessionCache& sessionCache);

        /**
          * This is called when a session change occurs. 
          * Observers should implement this method.
          *
          */
        virtual void sessionChanged(const SessionUpdate& sessionUpdate) = 0;
    };
}

#endif // !defined(_ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_)

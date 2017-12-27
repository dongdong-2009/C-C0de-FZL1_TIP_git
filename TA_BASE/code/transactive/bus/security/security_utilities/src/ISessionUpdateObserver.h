#ifndef ISESSIONUPDATEOBSERVER_H_INCLUDED
#define ISESSIONUPDATEOBSERVER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/ISessionUpdateObserver.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * There are a few different kinds of session updates. This allows
  * the SessionUpdateSubject to interpret the updates for the observer.
  * Otherwise the observer would stuck with an "update()" notification
  * and the observer would have to implement the interpretation of the
  * update which could result in the update being interpreted several
  * times depending on the number of observers.
  *
  * There are default implementations for all methods so the child of this
  * class need only override the update methods they are interested in.
  */

namespace TA_Base_Bus
{
	class ISessionUpdateObserver : public SessionUpdateSubject::ObserverType
	{
	public:
		ISessionUpdateObserver( SessionUpdateSubject& sessionUpdate );

        /** 
          * update
          *
          * Takes the update from the SessionUpdateSubject. This should
		  * not under normal circumstanced be overridden. If it is then
		  * the various sessionBegin/End etc methods will not get called
		  * when an update occurs unless the overrider calls back to this
		  * method.
          *
          * @param 
          */
		virtual void update( SessionUpdateSubject& sessionUpdate );

		/** 
		  * sessionBegin
		  *
		  * Provides all the available information about a new session.
		  *
		  * @param session
		  * @param user
		  * @param profile
		  * @param location
		  * @param workstation
		  *
		  */
		virtual void sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation );

        /** 
          * sessionEnd
          *
          * Passes the ending session.
          *
          * @param session
          */
		virtual void sessionEnd( const std::string& session );
		
		/** 
		  * sessionOverrideBegin
		  *
		  * The profile is one that is being ADDED to the session.
		  *
		  * @param session
		  * @param profile
		  *
		  */		
		virtual void sessionOverrideBegin( const std::string& session, unsigned long profile );

		/** 
		  * sessionOverrideEnd
		  *
		  * Note that the profile is the REMAINING profile, NOT the
		  * one that has been removed.
		  *
		  * @param session
		  * @param profile
		  *
		  */
		virtual void sessionOverrideEnd( const std::string& session, unsigned long profile );
	
	};
}
#endif // ISESSIONUPDATEOBSERVER_H_INCLUDED

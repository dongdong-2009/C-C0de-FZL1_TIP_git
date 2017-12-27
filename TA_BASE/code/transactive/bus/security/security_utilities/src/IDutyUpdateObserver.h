#ifndef IDUTYUPDATEOBSERVER_H_INCLUDED
#define IDUTYUPDATEOBSERVER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/IDutyUpdateObserver.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * TODO: Write this bit
  */

namespace TA_Base_Bus
{
	class IDutyUpdateObserver : public DutyUpdateSubject::ObserverType
	{
	public:
		IDutyUpdateObserver( DutyUpdateSubject& dutyUpdate );

        /** 
          * update
          *
          * Takes the update from the DutyUpdateSubject. This should
		  * not under normal circumstanced be overridden. If it is then
		  * the various methods will not get called
		  * when an update occurs unless the overrider calls back to this
		  * method.
          *
          * @param dutyUpdate - a reference to the DutyUpdateSubject object.
          */
		virtual void update( DutyUpdateSubject& dutyUpdate );

		/** 
		  * dutyChanged
		  *
		  * Notifies the observer of a duty change
		  *
		  * @param session - the session ID for which duty has changed
		  *
		  */
		virtual void dutyChanged( const std::string& session );
	
	};
}
#endif // IDUTYUPDATEOBSERVER_H_INCLUDED

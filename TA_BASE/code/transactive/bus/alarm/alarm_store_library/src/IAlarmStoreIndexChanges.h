/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/IAlarmStoreIndexChanges.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  IAlarmStoreIndexChanges.h
//  Implementation of the Interface IAlarmStoreIndexChanges
//  Created on:      24-Jan-2004 04:17:37 PM
///////////////////////////////////////////////////////////

#if !defined(IAlarmStoreIndexChanges_7604442B_6591_4bec_B138_4E3F0134CC95__INCLUDED_)
#define IAlarmStoreIndexChanges_7604442B_6591_4bec_B138_4E3F0134CC95__INCLUDED_


#include <string>

namespace TA_Base_Bus
{
	/**
	 * Interface defining the functions of a callback manager.  This has been added to
	 * detach the CORBA notification method from the alarm storage mechanics, easing
	 * the unit testing.
	 * @version 1.0
	 * @created 24-Jan-2004 04:17:37 PM
	 */
	class IAlarmStoreIndexChanges
	{
		public:
			/**
			 * @param callbackID    Individual callback to be notified
			 * @param indexID    Identity of index being updated
			 * 
			 */
			virtual void indexCleared(unsigned long callbackID, long indexID) =0;

			/**
			 * Index deleted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index
			 * @param alarmID    Alarm UUID
			 * 
			 */
			virtual void indexDeleted(unsigned long callbackID, long indexID,unsigned long position, std::string alarmID) =0;

			/**
			 * Called when an index entry is inserted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index where alarm inserted
			 * @param alarmID
			 * 
			 */
			virtual void indexInserted(unsigned long callbackID, long indexID,unsigned long position, std::string alarmID) =0;

			/**
			 * Called when an index entry is inserted
			 * @param callbackID    Individual callback to be notified
			 * @param indexID
			 * @param position    Position in index where alarm inserted
			 * @param alarmID
			 * 
			 */
			virtual void indexMoved(unsigned long callbackID, long indexID, unsigned long oldPosition, 
				                    unsigned long newPosition, std::string alarmID) =0;

	};
};
#endif // !defined(IAlarmStoreIndexChanges_7604442B_6591_4bec_B138_4E3F0134CC95__INCLUDED_)

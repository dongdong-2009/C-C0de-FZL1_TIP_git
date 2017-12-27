/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmActionFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This factory holds all the alarm actions that can be performed by this instance
 * of the alarm list. The main application needs to add the actions it wants to be
 * available from the list context menu. This class is not intended to be dynamic. All
 * actions should be added at the start and 
 */

#if !defined(AFX_AlarmActionFactory_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_AlarmActionFactory_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

namespace TA_Base_Bus
{
    // Forward declare
    class IAlarmAction;
    class IAlarmRetriever;

    class AlarmActionFactory
	{
	public:

        typedef std::vector<IAlarmAction*> AlarmActionList;


        /**
         * Constructor
         *
         * This adds all actions that can be performed by the user and will be available in the list
         * context menu. The IAlarmAction objects passed in become the property of this object and will
         * be deleted when this object is deleted.
         *
         * @param AlarmActionList - The alarm actions that are available. These objects become the property
         *                          of this class and will be deleted as such.
         */
        AlarmActionFactory(const AlarmActionList& actionList);


        /**
         * Destructor
         */
        virtual ~AlarmActionFactory();


        /**
         * getAlarmActions
         *
         * This retrieves all the alarm actions currently stored
         *
         * @return const AlarmActionList& - The action list is returned so the list control is
         *                                  able to iterate through the items and perform the actions required.
         */
        const AlarmActionList& getAlarmActions() const
        {
            return m_alarmActions;
        }


    private:
        
        AlarmActionList m_alarmActions;
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_AlarmActionFactory_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)

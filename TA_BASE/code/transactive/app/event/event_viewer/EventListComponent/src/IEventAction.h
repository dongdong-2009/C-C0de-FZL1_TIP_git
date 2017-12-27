/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/IEventAction.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This abstract class provides the framework for executing threaded actions on a particular
  * DisplayItem.
  *
  */

#if !defined(_IEVENTACTION_H_)
#define _IEVENTACTION_H_

class Filter;

class IEventAction
{
public:
    virtual ~IEventAction()
    {
    }

    /**
     * getActionId
     *
     * Returns the ID used to identify the action.
     *
     * @return unsigned int The ID of the action.
     */
    virtual unsigned int getActionId() =0;

    /**
     * executeAction
     *
     * Performs whatever task the action requires.
     *
     * @param item The DisplayItem that was double clicked on.
     */
    virtual void executeAction( Filter* filter ) =0;
};

#endif // !defined(_IACTION_H_)

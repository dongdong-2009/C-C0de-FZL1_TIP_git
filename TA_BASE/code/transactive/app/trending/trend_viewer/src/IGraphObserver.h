#if !defined(AFX_IGRAPHOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_IGRAPHOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * IGraphObserver.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IGraphObserver.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between a Graph object and an actual Graph observer. An object that
  * needs to be notified of any changes to the graph need to implement this interface.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>


namespace TA_Base_App
{
    
    //
    // IGraphObserver
    //
    class IGraphObserver  
    {
    public:
        
        /**
          * ~IGraphObserver
          * 
          * Destructor.
          */
        virtual ~IGraphObserver() {}

        /**
          * addItem
          * 
          * Adds the item and its range to the display area.
          *
          * @param index - index of item
          * @param name - name of item
          * @param lowRange - minimum value for the item
          * @param highRange - maximum value for the item
          * @param unitLabel - engineering unit label
          * @param precision - number of decimal places to display the value
          */
        virtual void addItem(int index, const std::string& name, double lowRange, double highRange, const std::string& unitLabel, int precision) = 0;

        /**
          * removeItem
          * 
          * Removes the item and its range from the display area.
          *
          * @param name - name of item
          */
        virtual void removeItem(const std::string& name) = 0;

        /**
          * setStartEndTime
          * 
          * Sets the start and end times on the GUI beneath the graph.
          *
          * @param startTime - start time
          * @param endTime - end time
          */
        virtual void setStartEndTime(time_t startTime, time_t endTime) = 0;

        /**
          * setCursorTime
          * 
          * Sets the cursor time in the display area.
          *
          * @param cursorTime - series number for the item
          */
        virtual void setCursorTime(time_t cursorTime) = 0;

        /**
          * setCursorTime
          * 
          * Sets the value at the cursor in the display area.
          *
          * @param index - index for the item
          * @param value - value at the cursor
          * @param precision - number of decimal places to display the value
          * @param unitLabel - engineering unit label
          */
        virtual void setCursorValue(int index, double value, int precision, const std::string& unitLabel) = 0;

        /**
          * setOffsetTime
          * 
          * Sets the offset time in the display area.
          *
          * @param offsetTime - series number for the item
          */
        virtual void setOffsetTime(time_t offsetTime) = 0;

        /**
          * setOffsetValue
          * 
          * Sets the value at the cursor in the display area.
          *
          * @param index - index for the item
          * @param value - value at the cursor
          * @param precision - number of decimal places to display the value
          * @param unitLabel - engineering unit label
          */
        virtual void setOffsetValue(int index, double value, double offsetValue, int precision, const std::string& unitLabel) = 0;

        /**
          * resetDefaultAxis
          * 
          * Resets the default (percentage) axis.
          */
        virtual void resetDefaultAxis() = 0;
    };
}

#endif // !defined(AFX_IGRAPHOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





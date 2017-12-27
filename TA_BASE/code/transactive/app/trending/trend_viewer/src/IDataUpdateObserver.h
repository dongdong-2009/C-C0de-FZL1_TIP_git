#if !defined(AFX_IDATAUPDATEOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_IDATAUPDATEOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * IDataUpdateObserver.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IDataUpdateObserver.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between an Update Subscriber and an actual Data Update Observer.
  * Objects that need to listen for data that has been updated need to implement this interface. When
  * a subscriber receives an update, a data observer will be notified of the updates through these
  * interface calls.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ctime>

namespace TA_Base_App
{
    
    //
    // IDataUpdateObserver
    //
    class IDataUpdateObserver  
    {
    public:
        
        /**
          * ~IDataUpdateObserver
          * 
          * Destructor.
          */
        virtual ~IDataUpdateObserver() {}

        /**
          * newDataReceived
          * 
          * Use the new data to display on the graph.
          *
          * @param timestamp - time new data was received
          * @param value - new value
          * @param isGoodStatus - flag indicating is status is good
          */
        virtual void newDataReceived(time_t timestamp, double value, bool isGoodStatus) = 0;

        /**
          * updateWithCurrentData
          * 
          * Use the current data to display on the graph.
          *
          * @param timestamp - time the current data is retrieved
          * @param samplePeriod - sample period configured for the graph
          */
        virtual void updateWithCurrentData(time_t timestamp, long samplePeriod) = 0;

    };
}

#endif // !defined(AFX_IDATAUPDATEOBSERVER_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





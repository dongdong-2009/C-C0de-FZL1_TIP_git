#if !defined(AbstractUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define AbstractUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * AbstractUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/AbstractUpdateSubscriber.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements IUpdateSubscriber and represents a subscriber.
  * A subscriber listens out for messages subscribed to by the inheriting
  * class and notifies the graphed item when a data source changes value.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include <ctime>

#include "app/trending/trend_viewer/src/IUpdateSubscriber.h"

namespace TA_Base_App
{
    //
    // Forward declarations
    //
    class IDataUpdateObserver;
    
    //
    // AbstractUpdateSubscriber class
    //
    class AbstractUpdateSubscriber : public IUpdateSubscriber
    {

    public:

        /**
          * AbstractUpdateSubscriber
          * 
          * Constructor.
          *
          */
        AbstractUpdateSubscriber(IDataUpdateObserver* dataUpdateObserver = 0)
            : m_dataUpdateObserver(dataUpdateObserver) {}

        /**
          * ~AbstractUpdateSubscriber
          * 
          * Destructor.
          *
          */
        virtual ~AbstractUpdateSubscriber() {}

         /**
          * newDataReceived
          * 
          * Use the new data to display on the graph.
          *
          * @param timestamp - time new data was received
          * @param value - new value
          * @param isGoodStatus - flag indicating is status is good
          */
        void newDataReceived(time_t timestamp, double value, bool isGoodStatus);

        /**
          * isObserverSet
          * 
          * Check if the observer has been set yet.
          *
          * @return whether the observer has been set.
          */
        bool isObserverSet();

    public:

        virtual void setObserver(IDataUpdateObserver* dataUpdateObserver)
        {
            m_dataUpdateObserver = dataUpdateObserver;
        }

    protected:

        void setEntityKey(unsigned long entityKey)
        {
            m_entityKey = entityKey;
        }

        unsigned long getEntityKey() const
        {
            return m_entityKey;
        }

        void setLocationKey(unsigned long locationKey)
        {
            m_locationKey = locationKey;
        }

        unsigned long getLocationKey() const
        {
            return m_locationKey;
        }

        void setSubscribed(bool isSubscribed)
        {
            m_isSubscribed = isSubscribed;
        }

        bool isSubscribed() const
        {
            return m_isSubscribed;
        }

        bool isLocationKeySet() const
        {
            return (m_locationKey != 0);   // Location key cannot be 0
        }

    private:

        AbstractUpdateSubscriber(const AbstractUpdateSubscriber& theAbstractUpdateSubscriber);
        AbstractUpdateSubscriber& operator=(const AbstractUpdateSubscriber &);

    private:

        IDataUpdateObserver* m_dataUpdateObserver;      // Observer that is to be notified when the item
                                                        // has changed value

        unsigned long m_entityKey;                      // Entity key of item this subscriber is associated with
        unsigned long m_locationKey;                    // Location subscriber is subscribed to
        bool m_isSubscribed;                            // Indicated whether subscribed or not
    };
}

#endif // !defined(AbstractUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)

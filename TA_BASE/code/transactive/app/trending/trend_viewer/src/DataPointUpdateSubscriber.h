#if !defined(DataPointUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define DataPointUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * DataPointUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointUpdateSubscriber.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements AbstractUpdateSubscriber and represents a subscriber that listens
  * for data point update events. When a data point value has changed, this object will notify
  * the graphed item associated with this data point so the graph can be udpated.
  *
  */

#include "app/trending/trend_viewer/src/AbstractUpdateSubscriber.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_App
{
    //
    // Forward declarations
    //
    class IDataUpdateObserver;
    
    //
    // DataPointUpdateSubscriber class
    //
    class DataPointUpdateSubscriber 
        : public AbstractUpdateSubscriber, 
          public TA_Base_Bus::IEntityUpdateEventProcessor
    {

    public:

        /**
          * DataPointUpdateSubscriber
          * 
          * Constructor.
          *
          */
        DataPointUpdateSubscriber(unsigned long entityKey, IDataUpdateObserver* dataUpdateObserver = NULL);

        /**
          * ~DataPointUpdateSubscriber
          * 
          * Destructor.
          *
          */
        virtual ~DataPointUpdateSubscriber() {}

        /**
          * processEntityUpdateEvent
          * 
          * This method satisfies the IEntityUpdateEventProcessor interface. This method is called when
          * the datapoint that a proxy has been set up for has been updated. When this is called, it tells
          * us that datapoint configuration has been initialised and it is now ok to retrieve datapoint
          * information from the proxy.
          *
          * @param entityKey - pkey of the data point the update is for
          * @param updateType - the type of update this is
          */
        void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /**
          * subscribeToMessage
          * 
          * Subscribes to the DataPointStateUpdate messages.
          *
          */
        virtual void subscribeToMessages();

        /**
          * unsubscribeToMessage
          * 
          * Unsubscribes to the DataPointStateUpdate messages.
          *
          */
        virtual void unsubscribeToMessages();

    private:

        DataPointUpdateSubscriber(const DataPointUpdateSubscriber& theSubscriber);
        DataPointUpdateSubscriber& operator=(const DataPointUpdateSubscriber &);
        TA_Base_Bus::DataPointProxySmartPtr m_dataPointProxy;   // datapoint proxy created 
        bool isFirstTimeSubscribed;

        void retrieveLocationKey();
        
    };
}

#endif // !defined(DataPointUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)

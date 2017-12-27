#if !defined(IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_)
#define IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_

/**
  * IUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IUpdateSubscriber.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between a Generic Item and an actual Update Subcriber. A Generic
  * Item makes calls on this interface to listen or not listen to messages.
  *
  */

namespace TA_Base_App
{
    //
    // Forward Declaration
    //
    class IDataUpdateObserver;

    //
    // IUpdateSubscriber
    //
    class IUpdateSubscriber
    {

    public:

        /**
          * ~IUpdateSubscriber
          * 
          * Destructor.
          */
        virtual ~IUpdateSubscriber() {}

        /**
          * subscribeToMessages
          * 
          * Subscribe to a message.
          */
        virtual void subscribeToMessages() =0;

        /**
          * unsubscribeToMessages
          * 
          * Unsubscribe to a message.
          */
        virtual void unsubscribeToMessages() =0;

        /**
          * setObserver
          * 
          * Set the data update observer for the subscriber
          *
          * @param dataUpdateObserver - pointer to update observer object
          */
        virtual void setObserver(IDataUpdateObserver* dataUpdateObserver) =0;
    };

}

#endif // !defined(IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_)

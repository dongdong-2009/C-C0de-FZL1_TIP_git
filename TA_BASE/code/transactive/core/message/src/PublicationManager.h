#if !defined(PublicationManager_E8109112_2C2D_4b63_8481_C63A4DA81622__INCLUDED_)
#define PublicationManager_E8109112_2C2D_4b63_8481_C63A4DA81622__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/PublicationManager.h $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */

#include <string>

// TODO LPP: Rename this file to StructuredEventSupplierMgr or similar.

namespace TA_Base_Core
{
    class StructuredEventSupplier;

    // The components assembled by gGetStructuredEventSupplierForChannel() are as follows:

    //   --            StructuredEventSupplier                  
    //   |                        |                             
    //   |                        v                             
    //   |                 SeqNumAppender                       
    //   |                        |                             
    //   |                        v       multiple threads      
    //   |                  BatchingQueue                       
    //   |                        |       single thread         
    //   |                        v                             
    //   | --              SESQueueHandler       <====== ChannelLocatorConnectionMgr notifications     
    //   | |                      |   \                         
    //   | |                      |    - DefaultAdminProvider 
    //   | |                      v                             
    //   | | --        RedundantEventSupplier                   
    //   | | |                  / | \       dispatches to ALL   
    //   | | |                 /  |  \      valid connections   
    //   | | |                v   v   v                                    
    //   | | |          SubscribedEventFilter    <===
    //   | | |                    |                 | 
    //   | | |                    v                 | subscription_change() notifications
    //   | | | --      CosNotifyChannelSupplier  ====
    //   | | | |                  |
    //   | | | |                  v
    //   --------         CNCA::EventChannel                              

    // Returns the supplier for the given channel, creating one if it doesn't already exist.
    // The current implementation returns one StructuredEventSupplier for each distinct channel.
    // DON'T delete the returned StructuredEventSupplier - gTerminateAndDeleteStructuredEventSuppliers()
    // will do this.
    StructuredEventSupplier* gGetStructuredEventSupplierForChannel(const std::string& channelName);

    // Terminates and deletes each of the suppliers that were created by calls to 
    // the above function, after detaching them from the ChannelLocatorConnectionMgr.  
    // gGetStructuredEventSupplierForChannel shouldn't be called after calling this function 
    // (otherwise suppliers will be recreated).
    // NOTE: this function must be called sometime before attempting to shutdown CorbaUtil, 
    // otherwise the application may not be able to terminate (CorbaUtil::shutdown() seems 
    // to block waiting for ChannelLocatorConnection threads to terminate - these threads
    // only terminate when all ChannelObservers have detached).
    void gTerminateAndDeleteStructuredEventSuppliers();

}; // TA_Base_Core

#endif // !defined(PublicationManager_E8109112_2C2D_4b63_8481_C63A4DA81622__INCLUDED_)

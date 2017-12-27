/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/CallbackUser.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Objects which wish to receive callbacks from the CallbackBroker need to
  * inherit from this class
  */

#ifndef CALLBACK_USER_H
#define CALLBACK_USER_H

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

#include <string>
#include "CallbackBroker.h"


class CallbackUser
{
    /** If you wish to recieve callbacks from the CallbackBroker then you
      * should inherit from this class and register yourself against one or
      * more submission keys (see CallbackBroker.h)
      */

public:
    /** Constructor
      *
      * @param key [IN]:
      *     (optional) register for submissions with this key at construction
      */
    CallbackUser() {};

    CallbackUser(const std::string& key)
    {
        CallbackBroker::getInstance().registerCallbackUser(this, key);
    }

    /** Destructor
      *
      * Note that the destructor will deregister you, even if you haven't
      * registered. This is okay and nothing to be concerned about
      */
    virtual ~CallbackUser()
    {
        CallbackBroker::getInstance().deregisterCallbackUser(this);
    }

    /** callback
      *
      * You need to override this pure virtual method in your derived class
      *
      * @param submission [IN]:
      *     A pointer to an object of class CallbackSubmission (or a derived
      *     class). You can use a dynamic_cast to convert to the derived class
      */
    virtual void callback(const CallbackSubmission* submission) = 0;
};


#endif // CALLBACK_USER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/CallbackBroker.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * A very simple observer/subscriber implementation
  */

#ifndef CALLBACK_BROKER_H
#define CALLBACK_BROKER_H

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

#include <string>
#include <map>

// Forward declarations:
class CallbackSubmission;
class CallbackUser;


class CallbackBroker
{
    /** The CallbackBroker is a singleton which handles forwarding of all
      * submissions to interested observers
      */

public:
    /** Destructor
      *
      * This object will not be destroyed until the statics are cleaned up
      * (after main() has returned)
      */
    ~CallbackBroker() {};

    /** getInstance()
      *
      * Retrieve the one-and-only instance of this class
      *
      * @return a reference to the singleton object
      */
    static CallbackBroker& getInstance();

    /** registerCallbackUser
      *
      * Register interest in submissions with a particular key (eg "FailOver").
      * you can register interest in several different submission keys
      *
      * @param user [IN]:
      *     a pointer to the CallbackUser object which is interested in
      *     submissions
      *
      * @param key [IN]:
      *     the submission key which the user is interested in
      */
    void registerCallbackUser(CallbackUser* user, const std::string& key);

    /** deregisterCallbackUser
      *
      * Remove all references to the given user from the maps. This is called
      * automatically from the CallbackUser destructor. It is safe to
      * deregister a user even if it hasn't been registered
      *
      * @param user [IN]:
      *     a pointer to the CallbackUser object to be deregistered
      */
    void deregisterCallbackUser(CallbackUser* user);

    /** submit
      *
      * Make callbacks to all the users who have registered interest in
      * submissions with the given key, passing a reference to the submission
      * itself to the user
      *
      * @param submission [IN]:
      *     a pointer to an object of class CallbackSubmission (or a subclass)
      */
    void submit(const CallbackSubmission* submission);

private:
    /** Constructor
      *
      * This is a singleton - constructor is private
      */
    CallbackBroker() {};

    // We store the keys which the users have registered interest in, indexed
    // by user. This is a multimap, so there can be multiple entries with the
    // same index (ie multiple registrations by the same user)
    typedef std::multimap<CallbackUser*, std::string> UserToKeyMap;
    UserToKeyMap m_userToKeyMap;

    // Here is the same data, but the otherway round - we store users, indexed
    // by key. This is what we need when we get a submission - to find all the
    // users that have registered interest in a particular key. Note that it is
    // *really* important that this member stays in sync with the previous
    // member
    typedef std::multimap<std::string, CallbackUser*> KeyToUserMap;
    KeyToUserMap m_keyToUserMap;
};


#endif // CALLBACK_BROKER_H

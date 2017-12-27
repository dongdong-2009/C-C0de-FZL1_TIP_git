/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/OnlineUpdateListener.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class listens for update messages and invoke the callbacks of those
  * objects that are registered for those messages.
  */


#if !defined(OnlineUpdateListener_4CD97833_A2DB_4499_9062_6D1C9A103943__INCLUDED_)
#define OnlineUpdateListener_4CD97833_A2DB_4499_9062_6D1C9A103943__INCLUDED_

#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/configuration_updates/src/ConfigMessageSubscriber.h"

#include <vector>
#include <map>

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


// Forward declaration for test rig
class OnlineUpdateListenerTest;
class ConfigMessageSubscriberTest;
class PublisherSubscriberThread;

namespace TA_Base_Core
{
    // Forward declaration
    class IOnlineUpdatable;
    class ConfigUpdateDetails;

    typedef std::map<unsigned int, IOnlineUpdatable*> KeyCallbackMap;
    struct SUpdateSettings
    {
        // The map that maps an entity key to a callback object 
        KeyCallbackMap keyCallback;

        // The list of callbacks.  When a 'Create' message is received,
        // update will be sent to each one of these callbacks.
        std::vector<IOnlineUpdatable*> callback;

        // If this is true, then there could only be 1 and only 1 callback element,
        // keyCallback is empty
        bool allKeysInterested;
    };



    class OnlineUpdateListener 
    {
        // We want to let ConfigMessageSubscriber have access to our private
        // methods processUpdate and hasAnyInterest.
        friend class ConfigMessageSubscriber;

        // Let our test rig test a few of our private methods
        friend class OnlineUpdateListenerTest;
        friend class ConfigMessageSubscriberTest;
        friend class PublisherSubscriberThread;

    public:
        /**
          * getInstance
          * 
          * Returns a single static instance of OnlineUpdateListener
          *
          * @return The one and only OnlineUpdateListener instance
          */
        static OnlineUpdateListener& getInstance();

        /**
          * ~OnlineUpdateListener 
          * 
          * Destructor
          */
        virtual ~OnlineUpdateListener();


        /**
          * registerInterest
          * 
          * Register interest to listen for update messages of the object
          * that has the specified type and key.  
          *
          * @param EObjectType - The object type that we're interested in getting
          *                      update messages
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and will be responsible for handling 
          *                            update messages of this object type
          * @param unsigned long - The key of the object that we're interested 
          *                        in getting update messages.
          *
          * Pre: - The callback object must not be null
          *      - This object type has not been registered to listen to all 
          *        update messages - assertion will be thrown even if 
          *        the callback used to register for all interests is
          *        the same as the one passed into this method.
          *      - The key has not been associated with another callback object
          */
        void registerInterest(
            const EObjectType objectType, 
            IOnlineUpdatable& callbackObj, 
            const unsigned long key);


        /**
          * registerInterests
          * 
          * Register interest to listen for update messages of the objects
          * that have the specified type and whose key is in the list of keys.
          *
          * @param EObjectType - The object type that we're interested in getting
          *                      update messages
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and will be responsible for handling 
          *                            update messages of this object type
          * @param vector<unsigned long>& - The keys of the object that we're 
          *                                 interested in getting update messages.
          *
          * Pre: - The callback object must not be null
          *      - This object type has not been registered to listen to all 
          *        the callback used to register for all interests is
          *        the same as the one passed into this method.
          *      - None of the keys have been associated with another callback object
          */
        void registerInterests(
            const EObjectType objectType, 
            IOnlineUpdatable& callbackObj, 
            const std::vector<unsigned long>& keys);


        /**
          * registerAllInterests
          * 
          * Register interest to listen for all update messages of the specified
          * object type.
          *
          * @param EObjectType - The object type that we're interested in getting
          *                      update messages
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and will be responsible for handling 
          *                            update messages of this object type
          *
          * Pre: - The callback object must not be null
          *      - This object type has not been registered with another callback
          *        object previously - assertion will be thrown if some of the keys
          *        of these object type is already handled by some other callbacks or
          *        another callback object has been registered to handle all updates
          *        of this object type.
          */
        void registerAllInterests(
            const EObjectType objectType, 
            IOnlineUpdatable& callbackObj);


        /**
          * deregisterInterest
          * 
          * Stop listening to the update messages of the object type
          * that has the particular key.  This method can only stop listening
          * to keys that the callback object has been registered to handle.  It
          * cannot remove the interest to a key that has been registered by some
          * other methods.
          *
          * @param EObjectType - The object type that we're interested in getting
          *                      update messages
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and will be responsible for handling 
          *                            update messages of this object type
          * @param unsigned long - The key of the object that we're interested 
          *                        in getting update messages.
          * @see deregisterInterests
          *
          * Pre: - The callback object must not be null
          *      - This object type has not been registered to listen to all 
          *        update messages
          *      - The key has been associated with this callback object
          *      - The key has not been associated with another callback object
          */
        void deregisterInterest(
            const EObjectType objectType, 
            const IOnlineUpdatable& callbackObj, 
            const unsigned long key);


        /**
          * deregisterInterests
          * 
          * Stop listening to the update messages of the object type
          * with these keys.  This method can only stop listening
          * to keys that the callback object has been registered to handle.  It
          * cannot remove the interest to a key that has been registered by some
          * other methods.
          * 
          * No assertion is thrown if this method is called when the callback 
          * object has not registered to listen to all update messages (To cater
          * for the cases where an object is created but deleted immediately, 
          * the object may not have the chance to register for update before 
          * being removed.)
          *
          * @param EObjectType - The object type that we're interested in getting
          *                      update messages
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and will be responsible for handling 
          *                            update messages of this object type
          * @param vector<unsigned long>& - The keys of the object that we're 
          *                                 interested in getting update messages.
          * @see deregisterInterest
          *
          * Pre: - The callback object must not be null
          *      - The key has not been associated with another callback object
          */
        void deregisterInterests(
            const EObjectType objectType, 
            const IOnlineUpdatable& callbackObj, 
            const std::vector<unsigned long>& keys);


        /**
          * deregisterAllInterests
          * 
          * Deregister for message update of this object type.
          * After this method is called, the library will no longer listen to 
          * update messages of this object type that are associated with the 
          * specified callbackObj.  
          * 
          * @param EObjectType - The object type to deregister to
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable 
          *                            and has been used to register for handling 
          *                            update messages of this object type
          * @see deregisterInterest
          * @see deregisterInterests
          *
          * Pre: - The callback object must not be null
          */
        void deregisterAllInterests(
            const EObjectType objectType, 
            const IOnlineUpdatable& callbackObj);


        /**
          * cleanUp
          * 
          * Deregister interest to online updates of ALL object types.  
          * Basically it just deletes the a single static instance of OnlineUpdateListener.
          * This should only be done if the intention is to deregister 
          * interest to all updates completely.  If only want to 
          * deregister interest for a particular object type, then use
          * deregisterInterest
          */
        static void cleanUp();


        /**
          * isObjectTypeRegistered
          * 
          * This method could be used to query if the object type
          * has already registered with some callback object
          *
          * @param EObjectType - The object type to query
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable
          *
          * @return true if object has been registered with this callback object
          *
          * Pre: - The callback object must not be null
          */
        bool isObjectTypeRegistered(
            const EObjectType objectType, 
            const IOnlineUpdatable& callbackObj);


        /**
          * isObjectTypeRegistered
          * 
          * This method could be used to query if the object type
          * has already registered with this callback object
          *
          * @param EObjectType - The object type to query
          *
          * @return true if object has been registered with this callback object
          *
          * Pre: - The callback object must not be null
          */
        bool isObjectTypeRegistered(const EObjectType objectType);


        /**
          * isKeyRegistered
          * 
          * This method could be used to query if the key of this object type has already 
          * been registered with the callback object
          *
          * @param EObjectType - The object type to query
          * @param IOnlineUpdatable& - The pointer to an object of a class 
          *                            that is extended from IOnlineUpdatable
          * @param unsigned int - The key which registers for the update of 
          *                       this object type
          *
          * @return true if key is registered to this particular callback object
          *
          * Pre: - The callback object must not be null
          */
        bool isKeyRegistered(
            const EObjectType objectType, 
            const IOnlineUpdatable& callbackObj, 
            const unsigned int key);


        /**
          * isKeyRegistered
          * 
          * This method could be used to query if the key of this object has already 
          * been registered with some callback.
          *
          * @param EObjectType - The object type to query
          * @param unsigned int - The key which registers for the update of 
          *                       this object type
          *
          * @return true if key is registered to any callback
          */
        bool isKeyRegistered(
            const EObjectType objectType, 
            const unsigned int key);


        /**
         * setListenOutForNewKeys
         *
         * @param listenOutForNewKeys if true tells the listener to automatically listen
         *          out of any Create updates that come through - if we're listening
         *          out for updates by individual key.  The callback object used
         *          corresponds to the primary (first registered) IOnlineUpdatable object
         *
         */
        void setListenOutForNewKeys(bool listenOutForNewKeys);
        
    private:

        /**
          * Default constructor
          *
          * Initialise member variables
          */
        OnlineUpdateListener();


        /**
          * Copy constructor
          *
          * Disable this.
          */
        OnlineUpdateListener(const OnlineUpdateListener& listener);


        /** 
          * operator=
          *
          * Assignment operator.  Disable this.
          *
          * @param &
          *
          * @return 
          */
        OnlineUpdateListener& operator=(const OnlineUpdateListener &) { };


        /**
          * hasAnyInterest
          *
          * Returns true if is still listening to any of the object types.
          * NOTE: The m_interestedObjectLock has to be guarded before calling this
          * method.
          *
          * @return true if is still listening to any of the object types
          */
        bool hasAnyInterest();


        /**
          * isSubscribing
          * 
          * Returns true if currently subscribing for update messages,
          * false otherwise.  Used for testing purposes.
          *
          * @return true if currently subscribing for update messages,
          * false otherwise.
          */
        bool isSubscribing() const
        {
            return m_messenger.isSubscribing();
        }


        /**
          * processUpdate
          *
          * Processes the configuration update and sends the updates to the subscribers
          * of the update
          * 
          * @param ConfigUpdateDetails& - The details of the configuration update 
          */
        virtual void processUpdate(ConfigUpdateDetails& update);

        
    // attributes
    private:
        // the config message subscriber that is responsible for receiving 
        // and sending online update configuration messages
        ConfigMessageSubscriber m_messenger;

        // if this is true, the listener will automatically register to new items of
        // the object types that it is already registering for when it receive 'Create' updates.
        bool m_listenOutForNewKeys;

        // object type - callback map.
        typedef std::map<EObjectType, SUpdateSettings> TypeSettingMap;
        TypeSettingMap m_interestedObject;

        // protect m_interestedObject creation
	    static ReEntrantThreadLockable m_interestedObjectLock;

	    // the one and only class instance
	    static OnlineUpdateListener* m_instance;

	    // protect singleton creation
	    static NonReEntrantThreadLockable m_singletonLock;
    };
}

#endif // !defined(OnlineUpdateListener_4CD97833_A2DB_4499_9062_6D1C9A103943__INCLUDED_)

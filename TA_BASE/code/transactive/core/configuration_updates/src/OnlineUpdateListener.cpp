/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/OnlineUpdateListener.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class listens for updates and invoke the callbacks of those
  * objects that are registered for those updates.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"

#include <algorithm>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/versioning/src/Version.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"


namespace TA_Base_Core
{
    // initialise statics
    OnlineUpdateListener* OnlineUpdateListener::m_instance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable OnlineUpdateListener::m_singletonLock;
    TA_Base_Core::ReEntrantThreadLockable OnlineUpdateListener::m_interestedObjectLock;


    OnlineUpdateListener& OnlineUpdateListener::getInstance()
    {
        // protect the instance check / creation
        ThreadGuard guard(m_singletonLock);

        // if instance not already created
        if (m_instance == NULL)
        {
	        // create it
	        m_instance = new OnlineUpdateListener();
        }

        return *m_instance;
    }


    OnlineUpdateListener::OnlineUpdateListener()
        :
        m_listenOutForNewKeys(false)
    {        
    }


    void OnlineUpdateListener::setListenOutForNewKeys(bool listenOutForNewKeys)
    {
        ThreadGuard guard(m_interestedObjectLock);        
        m_listenOutForNewKeys = listenOutForNewKeys; 
    }


    OnlineUpdateListener::~OnlineUpdateListener() 
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "destructor");
       
        { // start scope of guard on m_interestedObjectLock
            // guard the m_interestedObjectLock so that the m_interestedObject is
            // not being updated while we're trying to remove something
            ThreadGuard guard(m_interestedObjectLock);
            // clear the list
            m_interestedObject.erase(m_interestedObject.begin(), m_interestedObject.end());
        } // end scope of guard on m_interestedObjectLock
        m_messenger.unsubscribe();

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "destructor");
    }

    
    void OnlineUpdateListener::registerInterest(
        const EObjectType objectType, 
        IOnlineUpdatable& callbackObj, 
        const unsigned long key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "registerInterest");

        std::vector<unsigned long> keyVector;
        keyVector.push_back(key);
        registerInterests(objectType, callbackObj, keyVector);

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "registerInterest");
    }


    void OnlineUpdateListener::registerInterests(
        const EObjectType objectType, 
        IOnlineUpdatable& callbackObj, 
        const std::vector<unsigned long>& key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "registerInterests");

        { // start scope of guard on m_interestedObjectLock
            // lock the m_interestedObject so that we don't have 2 threads trying to 
            // add the same item at the same time
            ThreadGuard guard(m_interestedObjectLock);

			//xinsong++
			//modify the log content
			std::ostringstream keysStr;
			size_t keyCount = key.size();

            // if this object type has not been registered before, then just add it
            if (m_interestedObject.find(objectType) == m_interestedObject.end())
            {
                SUpdateSettings setting;
                setting.callback.push_back(&callbackObj);
				
                for (size_t i = 0; i < keyCount; i++)
                {
                    //LOGMORE(SourceInfo, "%d", key[i]);
					if ( i != (keyCount-1) )
					{
						keysStr << key[i] << ", ";
					}
					else
					{
						keysStr << key[i];
					}
                    setting.keyCallback.insert(KeyCallbackMap::value_type(key[i], &callbackObj));
                }
                LOG2(SourceInfo, DebugUtil::DebugInfo,
                    "Registering interest for the first time. Added object type = <%s> and keys = <%s>", getDescFromEObjectType(objectType).c_str(), keysStr.str().c_str());
				
                setting.allKeysInterested = false;
                m_interestedObject.insert(TypeSettingMap::value_type(objectType, setting));
                m_messenger.subscribe();
                LOG(SourceInfo, DebugUtil::FunctionExit, "registerInterests");
                return;
            }

            TA_ASSERT(!m_interestedObject[objectType].allKeysInterested, 
                "A callback has already been registered to handle all updates of this type");

            std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
            KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
            bool added = false;
            for (size_t i = 0; i < key.size(); i++)
            {
                if (keyCallback.find(key[i]) != keyCallback.end())
                {
                    // make sure they're pointing to the same callback object
                    TA_ASSERT(keyCallback[key[i]] == &callbackObj,
                        "Object type registered with different callback object");
                }
                else
                {
                    keyCallback.insert(KeyCallbackMap::value_type(key[i], &callbackObj));
                    added = true;
					if ( i != (keyCount-1) )
					{
						keysStr << i << ", ";
					}
					else
					{
						keysStr << i;
					}
                }
                LOG2(SourceInfo, DebugUtil::DebugInfo,
                    "Registering different interest for same object. Added object type = <%s> and keys = <%s>", getDescFromEObjectType(objectType).c_str(), keysStr.str().c_str());
            }

            if (added && (std::find(callback.begin(), callback.end(), &callbackObj) == callback.end()))
            {
                callback.push_back(&callbackObj);
            }
            if (hasAnyInterest())
            {
                m_messenger.subscribe();
            }
        } // end scope of guard on m_interestedObjectLock

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "registerInterests");
    }


    void OnlineUpdateListener::registerAllInterests(
        const EObjectType objectType, 
        IOnlineUpdatable& callbackObj)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "registerAllInterests");

        { // start scope of guard on m_interestedObjectLock
            // lock the m_interestedObject so that we don't have 2 threads trying to 
            // add the same item at the same time
            ThreadGuard guard(m_interestedObjectLock);

            // if this object type has not been registered before, then just add it
            if (m_interestedObject.find(objectType) == m_interestedObject.end())
            {
                SUpdateSettings setting;
                setting.callback.push_back(&callbackObj);
                setting.allKeysInterested = true;
                m_interestedObject.insert(TypeSettingMap::value_type(objectType, setting));
                m_messenger.subscribe();
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugInfo,
                    "Registering to listen to all updates of object type = %s", 
                    getDescFromEObjectType(objectType).c_str());
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "registerAllInterests");
                return;
            }

            std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
            if (callback.empty())
            {
                callback.push_back(&callbackObj);
                m_interestedObject[objectType].allKeysInterested = true;
                m_messenger.subscribe();
            }
            else
            {


                // We've registered to this object type before, see if it is registering all, 
                // assert if it is either not registering to all or it was registered with
                // a different callback.
				//chenlei-+
				//enable multiple callbackobj register to one type
				//need verification of designed documents
				//this part is used only in centralize duty/rights
				/*
                if (m_interestedObject[objectType].allKeysInterested)
                {
                    // if registered to listen to all keys, but the callback object is different,
                    // then assert
                    TA_ASSERT(callback[0] == &callbackObj,
                        "Another callback has been registered to listen to all updates of this object type");

                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                        "Already registered all with this callback object");
                }
                else
                {
                    TA_ASSERT(false,
                        "Already got some callbacks handling updates of this object types");
                }
				*/
				callback.push_back(&callbackObj);
            }
        } // end scope of guard on m_interestedObjectLock

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Registered to listen to all updates");
        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "registerAllInterests");
    }

    
    void OnlineUpdateListener::deregisterInterest(
        const EObjectType objectType, 
        const IOnlineUpdatable& callbackObj, 
        const unsigned long key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "deregisterInterest");

        std::vector<unsigned long> keyVector;
        keyVector.push_back(key);
        deregisterInterests(objectType, callbackObj, keyVector);

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "deregisterInterest");
    }
    

    void OnlineUpdateListener::deregisterInterests(
        const EObjectType objectType, 
        const IOnlineUpdatable& callbackObj, 
        const std::vector<unsigned long>& key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "deregisterInterests");

        // guard the m_interestedObjectLock so that the m_interestedObject is
        // not being updated while we're trying to remove an item
        ThreadGuard guard(m_interestedObjectLock);

        // Return if updates of this object type is currently not being listened for
        if (m_interestedObject.find(objectType) == m_interestedObject.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Object type has not been registered");
            LOG(SourceInfo, DebugUtil::FunctionExit, 
                "deregisterInterests");
            return;
        }

        // We won't be able to remove any key if listening to all keys
        TA_ASSERT(!m_interestedObject[objectType].allKeysInterested, 
            "All keys are registered.  Can't remove individual key");

        std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
        // Do a quick find to see if this callback object has been included in the 
        // callback list.  If it wasn't, then the callback hasn't been registered
        // to handle the updates of this object type, so throw an assertion.
        if (std::find(callback.begin(), callback.end(), &callbackObj) == callback.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Cannot remove interests as this callback has not been registered to get callbacks of this object type");
            LOG(SourceInfo, DebugUtil::FunctionExit, 
                "deregisterInterests");
            return;
        }

        if (key.empty())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugInfo,
                "No keys in the list to remove interest");
            LOG(SourceInfo, DebugUtil::FunctionExit, 
                "deregisterInterests");
            return;
        }
        
        KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
		std::ostringstream keysStr;
		keysStr <<"deregisterInterests: ";
        for (unsigned int i = 0; i < key.size(); i++)
        {
            if (keyCallback.find(key[i]) != keyCallback.end())
            {
                // make sure they're pointing to the same callback object
                TA_ASSERT(keyCallback[key[i]] == &callbackObj,
                    "Cannot remove interest for this key as it has been registered with different callback object");

                keyCallback.erase(key[i]);
				keysStr << key[i] << ", ";
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugDebug,
                    "Key = %d of object type = %d has not been registered. Ignore", key[i], objectType);
            }
        }
		LOGLARGESTRING(SourceInfo, DebugUtil::DebugInfo, keysStr.str().c_str());

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "deregisterInterests");
    }


    void OnlineUpdateListener::deregisterAllInterests(
        const EObjectType objectType, 
        const IOnlineUpdatable& callbackObj)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "deregisterAllInterests");

        { // start scope of guard on m_interestedObjectLock
            // guard the m_interestedObjectLock so that the m_interestedObject is
            // not being updated while we're trying to remove something
            ThreadGuard guard(m_interestedObjectLock);

            // return if updates of this object type are not being listened for
            if (m_interestedObject.find(objectType) == m_interestedObject.end())
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugDebug,
                    "Object type has not been registered");
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "deregisterAllInterests");
                return;
            }

			//chenlei--
			//enable multiple callbackobj register to one type
			//need verification of designed documents
			//this part is used only in centralize duty/rights

			/*
            if (m_interestedObject[objectType].allKeysInterested)
            {
                if (m_interestedObject[objectType].callback[0] == &callbackObj)
                {
                    m_interestedObject[objectType].allKeysInterested = false;

                    m_interestedObject[objectType].callback.clear();

                    LOG(SourceInfo, DebugUtil::GenericLog, 
                        DebugUtil::DebugInfo,
                        "Removed all interests for object type = %d", objectType);
                }
                else
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, 
                        DebugUtil::DebugDebug,
                        "Another callback object has registered interest for all updates of object type.  Nothing to remove.");
                }            
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "deregisterAllInterests");
                return;
            }
			*/

            std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
            // Do a quick find to see if this callback object has been included in the 
            // callback list.  If it wasn't, then the callback hasn't been registered
            // to handle the updates of this object type, so throw an assertion.
            if (std::find(callback.begin(), callback.end(), &callbackObj) == callback.end())
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugDebug,
                    "Cannot remove interests as this callback has not been registered to get callbacks of this object type");
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "deregisterAllInterests");
                return;
            }

            KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Removed the keys:");

			std::ostringstream keysStr;
			keysStr << "deregisterAllInterests: ";

            KeyCallbackMap::iterator iter = keyCallback.begin();
            while(iter != keyCallback.end())
            {
                // if we got the same callback object, then erase it
                if (iter->second == &callbackObj)
                {
                    //LOGMORE(SourceInfo, "deregisterAllInterests: %d", iter->first);
					keysStr << iter->first <<", ";
                    keyCallback.erase(iter++);
                }
                else
                {
                    ++iter;
                }
            }
			LOGLARGESTRING(SourceInfo, DebugUtil::DebugInfo, keysStr.str().c_str() );

            // remove the callback from callback list
            callback.erase(std::remove(callback.begin(), callback.end(), &callbackObj), callback.end());

			//chenlei++
			//enable multiple callbackobj register to one type
			//need verification of designed documents
			//this part is used only in centralize duty/rights
			if(callback.size()==0)
			{
				m_interestedObject[objectType].allKeysInterested = false;
			}

            // if don't have any interest left
            if (!hasAnyInterest())
            {
                m_messenger.unsubscribe();
            }
        } // end scope of guard on m_interestedObjectLock

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "deregisterAllInterests");
    }


    void OnlineUpdateListener::cleanUp() 
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "cleanUp");

        // protect the instance check / creation
        ThreadGuard guard(m_singletonLock);

        if (m_instance != NULL)
        {
            // delete the instance 
            delete m_instance;
            m_instance = NULL;
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "cleanUp");
    }


    void OnlineUpdateListener::processUpdate(ConfigUpdateDetails& update) 
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "processUpdate");

        EObjectType objectType = update.getType();

        { // start scope of guard on m_interestedObjectLock
            // guard m_interestedObject so that it won't be changing while we're 
            // using it
            ThreadGuard guard(m_interestedObjectLock);

            // if we are not interested in the update
            if (m_interestedObject.find(objectType) == m_interestedObject.end())
            {
                // discard and return
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugInfo,
                    "Not interested in update type=%d", objectType);

                return;
            }

            unsigned int key = update.getKey();

            // process create update - have to send the create update to each of the
            // callback object registered for this object type
            if (update.getModifications() == TA_Base_Core::Create)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugInfo,
                    "Calling the processUpdate method for objectType = %d for Create update", 
                    objectType);

                std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
                for (unsigned int i = 0; i < callback.size(); i++)
                {
                    // check to see if the callback is not null, just in case the 
                    // IOnlineUpdatable callback has been destroyed since then
                    if (callback[i] != NULL)
                    {
                        // performs callback on IOnlineUpdatable object
                        callback[i]->processUpdate(update);
                    }
                }

                if (!m_interestedObject[objectType].allKeysInterested &&
                        m_listenOutForNewKeys)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, 
                        DebugUtil::DebugInfo,
                        "Add the key as created");
                    KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
                    // Add the item to map, pointing to primary key
                    keyCallback[key] = m_interestedObject[objectType].callback[0];
                }

                // if this is an ENTITY_OWNER update, then regenerate a new 
                // update for the entity users
                if (objectType == ENTITY_OWNER)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, 
                        DebugUtil::DebugInfo,
                        "Generating new config update for entity users");

                    m_messenger.generateEntityUpdateMessage(update);
                }
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "processUpdate");
                return;
            }

            KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;

            // if we're not interested in the update
            if (!m_interestedObject[objectType].allKeysInterested &&
                (keyCallback.find(key) == keyCallback.end()))
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugInfo,
                    "Not interested in config update type=%d of entity=%d", objectType, key);
                LOG(SourceInfo, DebugUtil::FunctionExit, 
                    "processUpdate");
                return;
            }

            IOnlineUpdatable* callbackPtr = NULL;
            if (m_interestedObject[objectType].allKeysInterested)
            {
                callbackPtr = m_interestedObject[objectType].callback[0];
            }
            else
            {
                callbackPtr = keyCallback[key];

                // delete the key from the list so that 
                // IOnlineUpdatable don't have to do it explicitly
                if (update.getModifications() == TA_Base_Core::Delete)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, 
                        DebugUtil::DebugInfo,
                        "Removing the key since it's deleted");
                    // remove key from list of keys to listen for, for that particular object type
                    keyCallback.erase(key);
                }                 
            }
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugInfo,
                "Calling the processUpdate method for objectType = %d", 
                objectType);

            // performs callback on IOnlineUpdatable object
            callbackPtr->processUpdate(update);

        } // end scope of guard on m_interestedObjectLock

        // if this is an ENTITY_OWNER update, then regenerate a new 
        // update for the entity users
        if (objectType == ENTITY_OWNER)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugInfo,
                "Generating new config update for entity users");

            m_messenger.generateEntityUpdateMessage(update);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, 
            "processUpdate");
    }



    bool OnlineUpdateListener::hasAnyInterest()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "hasAnyInterest");

        if (m_interestedObject.empty())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "No object type is registered");
            return false;
        }
        for (TypeSettingMap::iterator i = m_interestedObject.begin();
                i != m_interestedObject.end();
                i++)
        {
            // if the object type is either listening for all keys
            // or callback list is not empty
            if ((i->second).allKeysInterested || 
                !(i->second).callback.empty())
            {
                LOG(SourceInfo, DebugUtil::GenericLog, 
                    DebugUtil::DebugDebug,
                    "Got interest");

                // then it still have interest in listening for updates
                return true;
            }
        }
        LOG(SourceInfo, DebugUtil::GenericLog, 
            DebugUtil::DebugDebug,
            "No interest");
        return false;
    }


    bool OnlineUpdateListener::isKeyRegistered(
        const EObjectType objectType, 
        const IOnlineUpdatable& callbackObj, 
        const unsigned int key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "isKeyRegistered");

        ThreadGuard guard(m_interestedObjectLock);
        if (m_interestedObject.find(objectType) == m_interestedObject.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Object type has not been registered");

            return false;
        }

        if (m_interestedObject[objectType].callback.empty())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "No callbacks registered for this object type");

            return false;
        }

        if ((m_interestedObject[objectType].allKeysInterested) &&
            (m_interestedObject[objectType].callback[0] == &callbackObj))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Interested in all keys");

            return true;
        }

        KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
        if (keyCallback.find(key) == keyCallback.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Key not registered");
            return false;
        }

        if (keyCallback[key] == &callbackObj)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Key is registered");
            return true;
        }

        return false;
    }


    bool OnlineUpdateListener::isKeyRegistered(
        const EObjectType objectType, 
        const unsigned int key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "isKeyRegistered");
        ThreadGuard guard(m_interestedObjectLock);
        if (m_interestedObject.find(objectType) == m_interestedObject.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Object type has not been registered");

            return false;
        }
        KeyCallbackMap& keyCallback = m_interestedObject[objectType].keyCallback;
        return (m_interestedObject[objectType].allKeysInterested ||
            (keyCallback.find(key) != keyCallback.end()));
    }


    bool OnlineUpdateListener::isObjectTypeRegistered(
        const EObjectType objectType, 
        const IOnlineUpdatable& callbackObj)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "isObjectTypeRegistered");

        ThreadGuard guard(m_interestedObjectLock);

        if (m_interestedObject.find(objectType) == m_interestedObject.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, 
                DebugUtil::DebugDebug,
                "Object type has not been registered");

            return false;
        }

        std::vector<IOnlineUpdatable*>& callback = m_interestedObject[objectType].callback;
        return (std::find(callback.begin(), callback.end(), &callbackObj) != callback.end());
    }


    bool OnlineUpdateListener::isObjectTypeRegistered(const EObjectType objectType)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, 
            "isObjectTypeRegistered");
        ThreadGuard guard(m_interestedObjectLock);
        return ((m_interestedObject.find(objectType) != m_interestedObject.end()) &&
                !m_interestedObject[objectType].callback.empty());
    }
}


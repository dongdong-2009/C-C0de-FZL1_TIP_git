/**
  * The source code in this file is the property of 
  * Combuilder PTE LTD and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/GenaConfigEntity.h $
  * @author Noel R. Tajanlangit
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/24 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Class definition for GenaConfigEntity
  */

#ifndef __GENA_CONFIG_ENTITY_H_INCLUDED__
#define __GENA_CONFIG_ENTITY_H_INCLUDED__

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class GenaConfigEntity
		: public EntityData
    {

    public:
		/** 
		 * Default Constructor
		 */
		GenaConfigEntity();
		
		/** 
		 * Constructor
		 *
		 * @param ulKey unique identifier for this entity
		 */
		GenaConfigEntity(unsigned long ulkey);

		/** 
		 * Destructor
		 */
		virtual ~GenaConfigEntity();

		/** 
		 * invalidate
		 *
		 * Marks data as invalid so reload could trigger on the next request of data
		 */
		virtual void invalidate();

		/** 
		 * getType
		 *
		 * Returns the type of this entity
		 */
		virtual std::string getType();

		/** 
		 * getStaticType
		 *
		 * Static function which returns the type of this entity
		 */
		static std::string getStaticType();

		/** 
		 * clone
		 *
		 * duplicates the entity with the provide ID
		 *
		 * @param entity key to clone
		 */
		static IEntityData* clone(unsigned long key);

		/** 
		 * getObjectType
		 *
		 * retrieves the gena object type configuration for this entity
		 */
		virtual std::string getObjectType();

		/** 
		 * getSubscriptionTimeOut
		 *
		 * retrieves the Gena resubscription timeout configuration for this entity
		 */
		virtual unsigned long getSubscriptionTimeOut();

		/** 
		 * getCallbackServerName
		 *
		 * retrieves the gena callback server name configuration for this entity
		 */
		virtual std::string getServerName();

		/** 
		 * getCallbackServerIP
		 *
		 * retrieves the gena callback server ip address configuration for this entity
		 */
		virtual std::string getServerIP();

		/** 
		 * getCallbackServerPort
		 *
		 * retrieves the gena callback server port configuration for this entity
		 */
		virtual unsigned long getServerPort();

		/** 
		 * getCbHTTPKeepAlive
		 *
		 * retrieves the gena callback HTTP keep alive configuration for this entity
		 */
		virtual bool getHTTPKeepAlive();

		/** 
		 * getObjectType
		 *
		 * retrieves the gena object type configuration for this entity
		 */
		virtual unsigned long getHTTPTimeout();

		/** 
		 * getResubscriptionTimeout
		 *
		 * retrieves the Gena resubscription timeout configuration for this entity
		 */
		virtual unsigned long getResubscriptionTimeout();

		/** 
		 * getCallbackServerName
		 *
		 * retrieves the gena callback server name configuration for this entity
		 */
		virtual std::string getCbServerName();

		/** 
		 * getCallbackServerIP
		 *
		 * retrieves the gena callback server ip address configuration for this entity
		 */
		virtual std::string getCbServerIP();

		/** 
		 * getCallbackServerPort
		 *
		 * retrieves the gena callback server port configuration for this entity
		 */
		virtual unsigned long getCbServerPort();

		/** 
		 * getCbHTTPKeepAlive
		 *
		 * retrieves the gena callback HTTP keep alive configuration for this entity
		 */
		virtual bool getCbHTTPKeepAlive();

		/** 
		 * getObjectType
		 *
		 * retrieves the gena object type configuration for this entity
		 */
		virtual unsigned long getCbHTTPTimeout();
    public:
		static const std::string GENA_CLIENT_TYPE;
		static const std::string GENA_SERVER_TYPE;
		static const std::string GENA_BRIDGE_TYPE;
    private:
		static const std::string ENTITY_TYPE;
		static const std::string OBJECT_TYPE;
		static const std::string SUBSCRIPTION_TIMEOUT;
		static const std::string SERVER_NAME;
		static const std::string SERVER_IP;
		static const std::string SERVER_PORT;
		static const std::string HTTP_KEEP_ALIVE;
		static const std::string HTTP_TIMEOUT;
		static const std::string RESUBSCRIPTION_TIMEOUT;
		static const std::string CALLBACK_SERVER_NAME;
		static const std::string CALLBACK_SERVER_IP;
		static const std::string CALLBACK_SERVER_PORT;
		static const std::string CALLBACK_HTTP_KEEP_ALIVE;
		static const std::string CALLBACK_HTTP_TIMEOUT;
    };

    typedef boost::shared_ptr<GenaConfigEntity> GenaConfigEntityPtr;
}

#endif // __GENA_CONFIG_ENTITY_H_INCLUDED__

/**
  * The source code in this file is the property of 
  * Combuilder PTE LTD and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/GenaConfigEntity.cpp $
  * @author Noel R. Tajanlangit
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/24 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Class implementation for GenaConfigEntity.
  */

#include "core/data_access_interface/entity_access/src/GenaConfigEntity.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	const std::string GenaConfigEntity::GENA_CLIENT_TYPE = "GenaClient";
	const std::string GenaConfigEntity::GENA_SERVER_TYPE = "GenaServer";
	const std::string GenaConfigEntity::GENA_BRIDGE_TYPE = "GenaBridge";

	const std::string GenaConfigEntity::ENTITY_TYPE = "GenaConfig";
	const std::string GenaConfigEntity::OBJECT_TYPE = "GenaObjectType";
	const std::string GenaConfigEntity::SUBSCRIPTION_TIMEOUT = "GenaSubscriptionTO";
	const std::string GenaConfigEntity::SERVER_NAME = "GenaServerName";
	const std::string GenaConfigEntity::SERVER_IP = "GenaServerIP";
	const std::string GenaConfigEntity::SERVER_PORT = "GenaServerPort";
	const std::string GenaConfigEntity::HTTP_KEEP_ALIVE = "GenaHTTPKeepAlive";
	const std::string GenaConfigEntity::HTTP_TIMEOUT = "GenaHTTPTimeOut";
	const std::string GenaConfigEntity::RESUBSCRIPTION_TIMEOUT = "GenaResubscriptionTO";
	const std::string GenaConfigEntity::CALLBACK_SERVER_NAME = "GenaCallbackServerName";
	const std::string GenaConfigEntity::CALLBACK_SERVER_IP = "GenaCallbackServerIP";
	const std::string GenaConfigEntity::CALLBACK_SERVER_PORT = "GenaCallbackServerPort";
	const std::string GenaConfigEntity::CALLBACK_HTTP_KEEP_ALIVE = "GenaCallbackHTTPKeepAlive";
	const std::string GenaConfigEntity::CALLBACK_HTTP_TIMEOUT = "GenaCallbackHTTPTimeOut";

	GenaConfigEntity::GenaConfigEntity(){}

	GenaConfigEntity::GenaConfigEntity(unsigned long ulkey) 
		: EntityData(ulkey, getStaticType())
	{
	}

	GenaConfigEntity::~GenaConfigEntity()
	{
	}

	void GenaConfigEntity::invalidate()
	{
		getHelper()->invalidate();
	}

	std::string GenaConfigEntity::getType()
	{
		return getStaticType();
	}

	std::string GenaConfigEntity::getStaticType()
	{
		return ENTITY_TYPE;
	}

	IEntityData* GenaConfigEntity::clone(unsigned long key)
	{
		return( new GenaConfigEntity(key) );
	}

	std::string GenaConfigEntity::getObjectType()
	{
		std::string strRetVal = "";

		try
		{
			strRetVal = getHelper()->getParameter(OBJECT_TYPE);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return strRetVal;
	}

	unsigned long GenaConfigEntity::getSubscriptionTimeOut()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string strTemp = getHelper()->getParameter(SUBSCRIPTION_TIMEOUT);
			return EntityHelper::getUnsignedLongData(strTemp);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	std::string GenaConfigEntity::getServerName()
	{
		std::string strRetVal = "";

		try
		{
			strRetVal = getHelper()->getParameter(SERVER_NAME);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return strRetVal;
	}

	std::string GenaConfigEntity::getServerIP()
	{
		std::string strRetVal = "";

		try
		{
			strRetVal = getHelper()->getParameter(SERVER_IP);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return strRetVal;
	}

	unsigned long GenaConfigEntity::getServerPort()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string cbServerPort = getHelper()->getParameter(SERVER_PORT);
			return EntityHelper::getUnsignedLongData(cbServerPort);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	bool GenaConfigEntity::getHTTPKeepAlive()
	{
		bool bRetVal = false;

		try
		{
			std::string cbKeepAlive = getHelper()->getParameter(HTTP_KEEP_ALIVE);
			unsigned long ulTemp = EntityHelper::getUnsignedLongData(cbKeepAlive);
			return ulTemp==0?false:true;
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return bRetVal;
	}

	unsigned long GenaConfigEntity::getHTTPTimeout()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string cbTimeout = getHelper()->getParameter(HTTP_TIMEOUT);
			return EntityHelper::getUnsignedLongData(cbTimeout);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	unsigned long GenaConfigEntity::getResubscriptionTimeout()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string evSubmitInterval = getHelper()->getParameter(RESUBSCRIPTION_TIMEOUT);
			return EntityHelper::getUnsignedLongData(evSubmitInterval);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	std::string GenaConfigEntity::getCbServerName()
	{
		std::string strRetVal = "";

		try
		{
			strRetVal = getHelper()->getParameter(CALLBACK_SERVER_NAME);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return strRetVal;
	}

	std::string GenaConfigEntity::getCbServerIP()
	{
		std::string strRetVal = "";

		try
		{
			strRetVal = getHelper()->getParameter(CALLBACK_SERVER_IP);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return strRetVal;
	}

	unsigned long GenaConfigEntity::getCbServerPort()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string cbServerPort = getHelper()->getParameter(CALLBACK_SERVER_PORT);
			return EntityHelper::getUnsignedLongData(cbServerPort);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}

	bool GenaConfigEntity::getCbHTTPKeepAlive()
	{
		bool bRetVal = false;

		try
		{
			std::string cbKeepAlive = getHelper()->getParameter(CALLBACK_HTTP_KEEP_ALIVE);
			unsigned long ulTemp = EntityHelper::getUnsignedLongData(cbKeepAlive);
			return ulTemp==0?false:true;
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return bRetVal;
	}

	unsigned long GenaConfigEntity::getCbHTTPTimeout()
	{
		unsigned long ulRetVal = 0;

		try
		{
			std::string cbTimeout = getHelper()->getParameter(CALLBACK_HTTP_TIMEOUT);
			return EntityHelper::getUnsignedLongData(cbTimeout);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return ulRetVal;
	}
}

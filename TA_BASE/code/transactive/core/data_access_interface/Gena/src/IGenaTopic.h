/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: IGenaTopic.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the declaration for IGenaTopic
  */

#ifndef __IGENA_CLIENT_TOPIC_H_INCLUDED__
#define __IGENA_CLIENT_TOPIC_H_INCLUDED__

#pragma once
#include <string>

namespace TA_Base_Core 
{

	class IGenaTopic
	{
	public: 
		// virtual destructor
		virtual ~IGenaTopic(){}

		// Pkey
		virtual void setKey(const unsigned long& pkey) = 0;
		virtual unsigned long getKey() = 0;
		 
		//Topic
		virtual void setTopic(const std::string& topic) = 0;
		virtual std::string getTopic() = 0;

		//SubscribeURL
		virtual void setSubscribeURL(const std::string& subscribeURL) = 0;
		virtual std::string getSubscribeURL() = 0;

		//EntityKey
		virtual void setEntityKey(const unsigned long& entityKey) = 0;
		virtual unsigned long getEntityKey() = 0;

		//Notification
		virtual void setNotification(const std::string& notification) = 0;
		virtual std::string getNotification() = 0;

		virtual void invalidate() =0;			

		virtual void applyChanges() =0;

	private:

	}; //IGenaTopic

} //TA_Base_Core


#endif //__IGENA_CLIENT_TOPIC_H_INCLUDED__

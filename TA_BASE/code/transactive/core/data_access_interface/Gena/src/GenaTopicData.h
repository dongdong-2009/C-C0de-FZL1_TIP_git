/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaTopicData.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the declaration for GenaTopicData
  */

#ifndef __GENA_TOPIC_DATA_H_INCLUDED__
#define __GENA_TOPIC_DATA_H_INCLUDED__

#include "IGenaTopic.h"
#include "core/data_access_interface/src/IData.h"

#pragma once

namespace TA_Base_Core 
{
	class GenaTopicData
		: public IGenaTopic
	{
	public: 
		// Constructor
		GenaTopicData();

		GenaTopicData(const unsigned long& pkey);

		/**
         * Constructor
         *
         * Construct a GenaClientSource class based on dataset
         */
		GenaTopicData(const unsigned long row, TA_Base_Core::IData& data);

		// Destructor
		~GenaTopicData();

		// Pkey
		virtual void setKey(const unsigned long& pkey);
		virtual unsigned long getKey();

		//Topic
		virtual void setTopic(const std::string& topic);
		virtual std::string getTopic();

		//SubscribeURL
		virtual void setSubscribeURL(const std::string& subscribeURL);
		virtual std::string getSubscribeURL();

		//EntityKey
		virtual void setEntityKey(const unsigned long& entityKey);
		virtual unsigned long getEntityKey();

		//Notification
		virtual void setNotification(const std::string& notification);
		virtual std::string getNotification();

		virtual void invalidate();

		virtual void applyChanges();

	private: // Methods
		void reload();
		void createNew();
		void updateExisting();

		void reloadInternal(const unsigned long rowIdx, TA_Base_Core::IData& data);

		/**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();
	private: // Variable
		unsigned long m_pkey;
		std::string	  m_topic;
		std::string	  m_subscribeURL;
		unsigned long m_entitykey;
		std::string	  m_notification;
		
		bool m_isNew;
		bool m_isValidData;

		bool m_isEntityKeyModified;
		bool m_isSubscribeURLModified;
		bool m_isTopicModified;
		bool m_isNotificationModified;

		// The name of the local database to send updates to
		std::string m_localDatabase;
	};
}


#endif //__GENA_TOPIC_DATA_H_INCLUDED__

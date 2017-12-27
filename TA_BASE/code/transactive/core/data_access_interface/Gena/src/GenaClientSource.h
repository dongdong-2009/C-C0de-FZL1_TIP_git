/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaClientSource.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the declaration for GenaClientSource
  */

#ifndef __GENA_CLIENT_SOURCE_H_INCLUDED__
#define __GENA_CLIENT_SOURCE_H_INCLUDED__

#include "IGenaClientSource.h"
#include "core/data_access_interface/src/IData.h"

#pragma once

namespace TA_Base_Core 
{
	class GenaClientSource
		: public IGenaClientSource
	{
	public: 
		// Constructor
		GenaClientSource();

		GenaClientSource(const unsigned long& pkey);

		/**
         * Constructor
         *
         * Construct a GenaClientSource class based on dataset
         */
		GenaClientSource(const unsigned long row, TA_Base_Core::IData& data);

		// Destructor
		~GenaClientSource();

		// Pkey
		virtual void setPkey(const unsigned long& pkey);
		virtual unsigned long getPkey();

		//Entitykey
		virtual void setEntityKey(const unsigned long& entityKey);
		virtual unsigned long getEntityKey();

		//ServerURL
		virtual void setServerURL(const std::string& serverURL);
		virtual std::string getServerURL();

		//MaxRetry
		virtual void setMaxRetry(const unsigned long& maxRetry);
		virtual unsigned long getMaxRetry();

		//TimeOut
		virtual void setTimeOut(const unsigned long& timeOut);
		virtual unsigned long getTimeOut();

		//KeepAlive
		virtual void setKeepAlive(const bool& keepAlive);
		virtual bool getKeepAlive();

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
		unsigned long m_entitykey;
		std::string	  m_serverURL;
		unsigned long m_maxRetry;
		unsigned long m_timeOut;
		unsigned long m_keepAlive;

		bool m_isNew;
		bool m_isValidData;

		bool m_isEntityKeyModified;
		bool m_isServerURLModified;
		bool m_isMaxRetryModified;
		bool m_isTimeOutModified;
		bool m_isKeepAliveModified;

		// The name of the local database to send updates to
		std::string m_localDatabase;
	};
}


#endif //__GENA_CLIENT_SOURCE_H_INCLUDED__

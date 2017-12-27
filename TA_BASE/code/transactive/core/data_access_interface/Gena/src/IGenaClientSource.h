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

#ifndef __IGENA_CLIENT_SOURCE_H_INCLUDED__
#define __IGENA_CLIENT_SOURCE_H_INCLUDED__

#pragma once
#include <string>

namespace TA_Base_Core 
{

	class IGenaClientSource
	{
	public: 
		// virtual destructor
		virtual ~IGenaClientSource(){}

		// Pkey
		virtual void setPkey(const unsigned long& pkey) = 0;
		virtual unsigned long getPkey() = 0;
		 
		//Entitykey
		virtual void setEntityKey(const unsigned long& entityKey) = 0;
		virtual unsigned long getEntityKey() = 0;

		//ServerURL
		virtual void setServerURL(const std::string& serverURL) = 0;
		virtual std::string getServerURL() = 0;

		//MaxRetry
		virtual void setMaxRetry(const unsigned long& maxRetry) = 0;
		virtual unsigned long getMaxRetry() = 0;

		//TimeOut
		virtual void setTimeOut(const unsigned long& timeOut) = 0;
		virtual unsigned long getTimeOut() = 0;

		//KeepAlive
		virtual void setKeepAlive(const bool& keepAlive) = 0;
		virtual bool getKeepAlive() = 0;

		virtual void invalidate() =0;			

		virtual void applyChanges() =0;

	private:

	};
}


#endif //__GENA_CLIENT_SOURCE_H_INCLUDED__

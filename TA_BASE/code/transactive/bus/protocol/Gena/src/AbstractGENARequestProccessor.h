/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: AbstractGENARequestProccessor.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaServerImp
  */

#ifndef __ABSTRACT_REQUEST_PROCCESSOR_H_INCLUDED__
#define __ABSTRACT_REQUEST_PROCCESSOR_H_INCLUDED__

#include "IRequestProccessor.h"
#include "CommonDef.h"
#include "GENARequest.h"

#pragma once

namespace TA_Base_Bus
{
	class AbstractGENARequestProccessor
		: public IRequestProccessor
	{
	public:
		/*
		* Constructor
		*/
		AbstractGENARequestProccessor(void);

		/*
		* Virtual Destructor
		*/
		virtual ~AbstractGENARequestProccessor(void);

		/*
		* proccessRequest
		*/
		virtual void proccessRequest(GENARequestPtr objRequest, GenaResponse& objResponse);

		/*
		* requestBirthMessage
		* @purpose: used to request for birth messages. 
		* @param:	topicURI - topic URI to identify the correct publisher
					SID - subscriber identification 
		* @return: none
		*/
		virtual void requestBirthMessage(const std::string& topicURI, std::string nSID) {} ; // Override if needed
	protected:
		/*
		* subscribe
		*/
		virtual std::string subscribe(GENARequestPtr objRequest) = 0;

		/*
		* unsubscribe
		*/
		virtual bool unsubscribe(GENARequestPtr objRequest) = 0;

		/*
		* notify
		*/
		virtual void notify(GENARequestPtr objRequest) = 0;

	private:

	};

} //TA_Base_Bus


#endif //__ABSTRACT_REQUEST_PROCCESSOR_H_INCLUDED__
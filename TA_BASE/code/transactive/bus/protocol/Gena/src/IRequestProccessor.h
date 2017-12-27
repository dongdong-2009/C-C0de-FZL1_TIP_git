/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: IRequestProccessor.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for processing the request.
  */

#ifndef __IREQUEST_LISTENER_H_INCLUDED__
#define __IREQUEST_LISTENER_H_INCLUDED__

#include "CommonDef.h"
#include "GENARequest.h"

namespace TA_Base_Bus
{
	class IRequestProccessor
	{
	public:
		/*
		* Virtual Destructor
		*/
		virtual ~IRequestProccessor(){};

		/*
		* proccessRequest
		*/
		virtual void proccessRequest(GENARequestPtr objRequest, GenaResponse& objResponse) = 0;

		/*
		* requestBirthMessage
		* @purpose: used to request for birth messages. 
		* @param:	topicURI - topic URI to identify the correct publisher
					SID - subscriber identification 
		* @return: none
		*/
		virtual void requestBirthMessage(const std::string& topicURI, std::string nSID) = 0;
	};

} //TA_Base_Bus


#endif // __IREQUEST_LISTENER_H_INCLUDED__
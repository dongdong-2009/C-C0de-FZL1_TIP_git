/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaServerImp.h  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaServerImp
  */


#ifndef __ITOPIC_PUBLISHER_H_INCLUDED__
#define __ITOPIC_PUBLISHER_H_INCLUDED__

#include "CommonDef.h"
#include "GENATopicData.h"
#include <string>

namespace TA_Base_Bus
{
class ITopicPublisher
{
public:
	/*
	* Virtual Destructor
	*/
	virtual ~ITopicPublisher(){};

	/*
	* Virtual subscribe
	*/
	virtual void subscribe(SubscriberInfoPtr objSubInfo) = 0;

	/*
	* Virtual unSubscribe
	*/
	virtual void unSubscribe(const std::string& nSID) = 0;

	/*
	* Virtual publish
	*/
	virtual void publish(GENATopicDataPtr ptopicUpdate, std::string nSID = "") = 0;

	/*
	* Virtual resendBirthMessage
	*/
	virtual void resendBirthMessage(std::string SID) = 0;
};

typedef std::map<std::string, ITopicPublisher* > TopicPublisherMap_T;
}


#endif //__ITOPIC_PUBLISHER_H_INCLUDED__
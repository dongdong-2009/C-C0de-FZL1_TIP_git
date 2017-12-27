/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: TopicPublisherFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for TopicPublisherFactory
  */

#ifndef __ITOPIC_LISTENER_H_INCLUDED__
#define __ITOPIC_LISTENER_H_INCLUDED__

#include "ITopicPublisher.h"
#include "GENAConnectionFactory.h"
#include "boost/shared_ptr.hpp"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#pragma once

namespace TA_Base_Bus
{
	// Forward declaration
	class GENAPublicationManager;

	class TopicPublisherFactory
	{
	public:

		/* 
		* Constructor
		*/ 
		TopicPublisherFactory(ConnectionFactoryPtr pConnectionMgr);
		
		/* 
		* Destructor
		*/ 
		~TopicPublisherFactory(void);

		/* 
		* createPublisher
		*/ 
		ITopicPublisher* createPublisher(GENAPublicationManager* pubMgr, const std::string& topic);

	private:		
		/* 
		* copy constructor disabled
		*/ 
		TopicPublisherFactory( const TopicPublisherFactory& rhs);
		TopicPublisherFactory& operator = ( const TopicPublisherFactory& rhs);

		ConnectionFactoryPtr m_pConnectionMgr;
	};

	typedef boost::shared_ptr<TopicPublisherFactory> TopicPublisherFactoryPtr;
} //TA_Base_Bus

#endif //__ITOPIC_LISTENER_H_INCLUDED__
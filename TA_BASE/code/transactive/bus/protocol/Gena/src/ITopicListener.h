/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ITopicListener.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for ITopicListener
  */


#ifndef __ITOPIC_LISTENER_H_INCLUDED__
#define __ITOPIC_LISTENER_H_INCLUDED__

#include "CommonDef.h"

#include <string>

namespace TA_Base_Bus
{
	class ITopicListener
	{
	public:
		/*
		* Virtual Destructor
		*/
		virtual ~ITopicListener(){};

		/*
		* Virtual onNotifyUpdate
		*/
		virtual void onNotifyUpdate(const TopicUpdate& topicUpdate) = 0;

	};

} //TA_Base_Bus

#endif //__ITOPIC_LISTENER_H_INCLUDED__
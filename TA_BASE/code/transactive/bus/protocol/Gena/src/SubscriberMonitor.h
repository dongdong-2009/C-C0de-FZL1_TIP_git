/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: SubscriberMonitor.h   $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for SubscriberMonitor
  */

#ifndef __SUBSCRIBER_MONITOR_H_INCLUDED__
#define __SUBSCRIBER_MONITOR_H_INCLUDED__

#pragma once

class SubscriberMonitor
	// Should inherit from thread //Todo:
{
public:
	SubscriberMonitor(unsigned long chkInterval);
	~SubscriberMonitor(void);

	virtual void run();

	virtual void terminate();



private:
	bool m_bTerminate;
	unsigned long m_chkInterval;
};

#endif //__SUBSCRIBER_MONITOR_H_INCLUDED__

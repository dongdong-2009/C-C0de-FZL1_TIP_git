
#include "SubscriberMonitor.h"

SubscriberMonitor::SubscriberMonitor(unsigned long chkInterval)
	: m_chkInterval(chkInterval)
	, m_bTerminate(false)
{
}

SubscriberMonitor::~SubscriberMonitor(void)
{
	//terminateAndWait(); 
}

void SubscriberMonitor::run()
{
	//Todo:
}

void SubscriberMonitor::terminate()
{
	//Todo:
}


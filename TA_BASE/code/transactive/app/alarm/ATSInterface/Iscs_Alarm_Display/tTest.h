#pragma once

#include "Thread.h"

class tTest : public TA_Base_Core::Thread
{
public:
	tTest(void);
	~tTest(void);
	virtual void run();
	virtual void terminate();
private:
	bool m_terminating;
};

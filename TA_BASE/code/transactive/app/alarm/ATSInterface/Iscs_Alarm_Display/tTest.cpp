#include "stdafx.h"
#include "tTest.h"

tTest::tTest(void)
{
}

tTest::~tTest(void)
{
}


void tTest::run()
{
	while(m_terminating)
	{
		this->sleep(1000);
		
	}
}

void tTest::terminate()
{
	m_terminating=true;
}

 
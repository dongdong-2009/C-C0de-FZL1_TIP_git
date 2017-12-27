#pragma once
#ifndef __IUPDATE_COUNTERS__
#define __IUPDATE_COUNTERS__

class IUpdateCounters
{
public:
	virtual void UpdateCounters(ULONG ulSubmitted, ULONG ulUnCloseCount) = 0;
};

#endif //__IUPDATE_COUNTERS__
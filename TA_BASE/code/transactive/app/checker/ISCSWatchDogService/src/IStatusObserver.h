#ifndef I_STATUS_OBSERVER_H
#define I_STATUS_OBSERVER_H

#include "StatusChecker.h"
#include<map>

//				StatusCheckerId, currentState
typedef std::pair<unsigned long, bool> ObserverData;

class IStatusObserver
{
public:
//	virtual void onStatusChanged() = 0;
	// actually only StatusChecker indicate Watched object has error occured,
	//	it will call onStatusChanged()
	virtual void onStatusChanged(const ObserverData& currentStatus) = 0;

	void registerObserver(StatusChecker& statusChecker)
	{
		statusChecker.addObserver(this);
	}
};

#endif
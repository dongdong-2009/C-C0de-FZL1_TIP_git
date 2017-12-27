#ifndef BUSINESS_H
#define BUSINESS_H

#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class Business
	{
	public:
		virtual void setToMonitor() = 0;
		virtual void setToControl() = 0;
		virtual void hostStart() = 0;
		virtual void hostStop() = 0;

		Business(){};
		virtual ~Business(){};
	};
}

#endif
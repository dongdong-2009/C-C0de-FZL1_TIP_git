#ifndef BUSINESS_FACTORY_H
#define BUSINESS_FACTORY_H

#include "bus/scada/business_library/src/Business.h"
#include "core/data_access_interface/src/IScadaBusinessItem.h"

#include <string>

namespace TA_Base_Bus
{
	class BusinessFactory
	{
	public:
		BusinessFactory(){};
		virtual ~BusinessFactory(){};
		Business* createBusiness(TA_Base_Core::IScadaBusinessItem* businessEntity);
	};
}

#endif
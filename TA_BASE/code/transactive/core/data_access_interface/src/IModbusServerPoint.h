/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/IModbusServerPoint.h $
  * @author:  Anita Lee
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * IModbusServerPoint is an interface to a ModbusServerPoint object.
  */

#if !defined(DAI_IMODBUSSERVERPOINT_H)
#define DAI_IMODBUSSERVERPOINT_H

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IModbusServerPoint : public virtual IItem
    {

    public:
        virtual ~IModbusServerPoint() { };
        virtual unsigned long getKey() = 0;
        virtual void invalidate() =0;

		virtual unsigned long getAgentKey() = 0;
		virtual std::string getAddress() = 0;
		virtual unsigned long getEntitykey() = 0;
		virtual unsigned short getPort() = 0;
		virtual std::string getLength() = 0;
		virtual std::string getName() = 0;
		virtual std::string getEntityAddress() = 0;

	};
} //close namespace TA_Base_Core

#endif // !defined(DAI_IMODBUSSERVERPOINT_H)

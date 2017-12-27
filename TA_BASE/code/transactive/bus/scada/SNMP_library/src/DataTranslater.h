/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/DataTranslater.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__DATA_TRANSLATER_H_
#define SNMP__DATA_TRANSLATER_H_

#include "bus/scada/common_library/src/DpValue.h"

#include "SNMPData.h"

namespace TA_Base_Bus
{
	/** translate data between DpValue type and Vb type.
     *  
     */
	class DataTranslater
	{
	public:

		DataTranslater();
		virtual ~DataTranslater();
		
		/** translate data from Vb type to DpValue type. this is a static function.
         *
         * @param dpValue: translate to this value.
		 * @param vb: translate from this value.
		 * @return void:
         */
		static void translateToDpValue(DpValue& dpValue, const /*Snmp_pp::*/Vb& vb);

		/** translate data from DpValue type to Vb type. this is a static function.
         *
         * @param dpValue: translate from this value.
		 * @param vb: translate to this value.
		 * @return void:
         */
		static void translateToSNMPVb(const DpValue& dpValue, /*Snmp_pp::*/Vb& vb);
	};
}

#endif

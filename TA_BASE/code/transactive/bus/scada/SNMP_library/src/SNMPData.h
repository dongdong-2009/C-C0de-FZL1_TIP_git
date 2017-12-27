/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_DATA_H_
#define SNMP__SNMP_DATA_H_


#include <string>
#include <vector>

#include "bus/scada/io_manager/src/IDeviceData.h"

//#define SNMP_PP_NAMESPACE	//using namespace Snmp_pp

#include "snmp_pp/snmp_pp.h"
#include "snmp_pp/v3.h"

const std::string DEVICE_NAME_APPEND_STRING = "_DEVICE";
///< define the device name must with the rear "_DEVICE".

namespace TA_Base_Bus
{
	/** define the SNMP protocol Data.
     *  implement the IDeviceData interface.
     */
	class SNMPData : public TA_Base_Bus::IDeviceData
	{
	public:

		SNMPData (){isGoodQuality = true; isPortionDevice = false;};
		virtual ~SNMPData(){};
	
		virtual std::string getStaticType() {return "SNMPData";};

		// operations
	private:
		SNMPData( const SNMPData & obj );
		void operator= ( const SNMPData & rhs );
		// attributes
	public:
		bool isGoodQuality;
		///< represent the data quality, true: good quality, false bad quality.

		bool isPortionDevice;
		///< true: portion of data. false: all the data of Device.

		/*Snmp_pp::*/Pdu pdu;
	};

	/** define the SNMP global helper.
     */
	class SNMPGlobalHelper
	{
	public:

		/** set SNMPpp debug log level, to remove all snmp_pp log.
         * this is a static function.
		 *
         */
		static void setSNMPppDebugLevel()
		{
			debug_set_level(0);	// remove all snmp_pp log.
		}

		/** compare the two names, to determin whether name matched.
         * this is a static function.
		 *
		 * @param deviceName: device should include DEVICE_NAME_APPEND_STRING as the rear.
		 * @param objName: object name should start with device name reduce DEVICE_NAME_APPEND_STRING.
         * @return bool: if matched return true. else false.
		 */
		static bool isNameMatchedDeviceName(const std::string& deviceName, const std::string& objName)
		{
			bool ret = false;
			std::string tempName = deviceName;
			if (std::string::npos != tempName.find(DEVICE_NAME_APPEND_STRING))
			{
				tempName = tempName.substr(0, tempName.length() - DEVICE_NAME_APPEND_STRING.length());
				tempName = tempName + ".";
				if (std::string::npos != objName.find(tempName))
				{
					ret = true;
				}
			}
			return ret;
		}
	};

}
#endif

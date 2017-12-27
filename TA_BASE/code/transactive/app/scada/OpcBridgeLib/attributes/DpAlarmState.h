/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  HoaVu
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * Represents the DataPoint's DpAlarmState attribute.  The DpAlarmState attribute
  * receives and processes the alarm information from the Alarm and Messaging
  * sub systems.
  *
  */

#ifndef DPALARMSTATE_H
#define DPALARMSTATE_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
	class DataPointAttributes;

	class DpAlarmState : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: Value
		//	Constructor
		//------------------------------------------------------------------------------------------
        DpAlarmState(DataPointAttributes* dpAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~Value
		//	Default Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpAlarmState();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_ALARMSTATE;}

	private:
        DataPointAttributes* m_dpAttribute;
	};
}
#endif

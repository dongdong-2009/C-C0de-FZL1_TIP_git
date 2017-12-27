/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * Class representing the DpValue attribute of a DataPoint
  *
  */

#ifndef DP_VALUE_H
#define DP_VALUE_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
	class DataPointAttributes;

	class DpValue : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DpValue
		//	Constructor
		//------------------------------------------------------------------------------------------
		DpValue(DataPointAttributes* dpAttribute, const std::string& tagName);


		//------------------------------------------------------------------------------------------
		// Operation: ~DpValue
		//	Default Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpValue();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_VALUE;}

	private:
        DataPointAttributes* m_dpAttribute;
	};
}

#endif

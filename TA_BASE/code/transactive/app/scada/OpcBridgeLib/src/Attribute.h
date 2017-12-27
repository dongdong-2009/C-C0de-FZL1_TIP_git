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
  * Represents the DataNode's attribute.  Each DataNode's attribute is associated
  * with one of the tag created in the OPC Server
  *
  */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "app/scada/OpcBridgeLib/src/OPCDataCache.h"
#include "app/scada/OpcBridgeLib/src/OpcTagManager.h"
#include "bus/scada/common_library/src/commondefs.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	enum EOPCATTRIBUTETYPE
	{
		E_OPC_ATTR_UNKNOWN = 0,
		E_OPC_DN_LABEL = 1,
		E_OPC_DN_SHORTLABEL,
		E_OPC_DN_ASSETNAME,
		E_OPC_DN_ONTIP,
		E_OPC_DN_ACKSTATE,
		E_OPC_DN_ALARMSTATE,
		E_OPC_DN_QUALITY,
		E_OPC_DP_ACKSTATE,
		E_OPC_DP_ALARMSTATE,
		E_OPC_DP_QUALITY,
		E_OPC_DP_VALUE,
		E_OPC_ATTR_MAX
	};

	class Attribute
	{
	public:

		/**
		 * Constructor
		 *
		 */
		Attribute(const std::string& tagName, _variant_t initValue, WORD  initQuality);

		/**
		 * Destructor
		 *
		 */
		virtual ~Attribute();

		/**
		 * isHandle
		 *
		 * Returns a boolean value to indicate if the specified OPC Tag handle
		 * matches with the one associated with this attribute
		 *
		 * @param 	Handle	The handle to search for
		 *
		 */
		virtual bool isHandle( HANDLE Handle );

		virtual void update(_variant_t  value, WORD quality);

		virtual void processProxyUpdate() { return; };

		virtual EOPCATTRIBUTETYPE getAttributeType() = 0;

		bool readTag( VARIANT& value, WORD& quality );

        static void incrementTotal();

        static void decrementTotal();

        static long getTotal();
		
		static void initDefaultValue();

		static EOPCDATAATTRIBUTETYPE convertAttrToCacheFlag(EOPCATTRIBUTETYPE attrType);
	private:
		_variant_t					m_value;
		WORD						m_quality;
		//to save memory, we only store tag name which tag create failed
		std::string					m_tagName;
		HANDLE						m_tagHandle;
		
		static bool					m_updateTagPeriodically;
		static long 				m_total;
		static TA_Base_Core::ReEntrantThreadLockable m_totalLock;
		static VARIANT				m_defaultValue;		
		static WORD					m_defaultQuality;
    public:
		bool						m_bAlias;
	};
}

#endif

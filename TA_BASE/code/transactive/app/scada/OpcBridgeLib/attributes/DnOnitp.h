/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Chin Wenching
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  * Class representing the ONITP attribute of a DataNode (TD13873)
  *
  */

#ifndef DNONITP_H
#define DNONITP_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
    class DataNodeAttributes;

	class DnOnitp : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DnOnitp
		//	Constructor
		//------------------------------------------------------------------------------------------
		DnOnitp(DataNodeAttributes* dnAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DnOnitp
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DnOnitp();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DN_ONTIP;}

	private:
		DataNodeAttributes* m_dnAttribute;
	};
}

#endif

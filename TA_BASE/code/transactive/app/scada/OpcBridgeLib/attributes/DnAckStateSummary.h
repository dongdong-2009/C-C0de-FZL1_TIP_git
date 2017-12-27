/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  * Class representing the Ack State Summary attribute of a DataNode
  *
  */

#ifndef DNACKSTATESUMMARY_H
#define DNACKSTATESUMMARY_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
    class DataNodeAttributes;
    
	class DnAckStateSummary : public Attribute 
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DnAckStateSummary
		//	Constructor
		//------------------------------------------------------------------------------------------
		DnAckStateSummary(DataNodeAttributes* dnAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DnAckStateSummary
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DnAckStateSummary();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DN_ACKSTATE;}
	private:
		DataNodeAttributes* m_dnAttribute;
	};
}

#endif

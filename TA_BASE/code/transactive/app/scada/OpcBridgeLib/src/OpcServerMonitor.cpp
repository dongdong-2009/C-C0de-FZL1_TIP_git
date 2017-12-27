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
  * Represents the DataNode entity defined in the database.  Object of the
  * DataNode performs the following functionalities:
  *		1)	maintains a list of attributes associated with the type of the
  *			DataNode.
  *		2)	responsible for receiving and processing the online changes of
  *			the DataNode's attributes.
  *		3)	maintains an internal clock to indicate how long the DataNode
  *			has been idled, i.e not queried by any client application
  *
  */

// Disable identifier truncated in debug warning
#pragma warning ( disable:4786 )

#include "app/scada/OpcBridgeLib/src/TransActiveDataSource.h"
#include "app/scada/OpcBridgeLib/src/OpcServerMonitor.h"

namespace TA_Base_App
{

	//
	// Thread derived functions to start/stop the
	// DataNodeQueueProcessorWorker thread
	//
	void OpcServerMonitor::run() 
	{
		int cycleCount = 0;
		int originalClientCount = 0;

		while ( m_keepRunning )
		{
			int clientCount = TransActiveDataSource::getInstance()->getOpcServerClientCount();
			if (clientCount != originalClientCount)
			{
				std::ostringstream text;
				text.clear();
				text << "OpcServerMonitor: Client Count changed from "; 
				text << originalClientCount << " to " << clientCount;
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", text.str().c_str() );
				TransActiveDataSource::getInstance()->tagLog(text.str());
				originalClientCount = clientCount;
			}
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OpcServerMonitor: Client Count %d",clientCount );

			if (cycleCount == 60)
			{
				cycleCount = 0;
				bool checkOk = TransActiveDataSource::getInstance()->checkOpcServerCache();

				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OpcServerMonitor: OPC Server cache %s", (checkOk?"OK":"Bad"));
			}

			Thread::sleep(1000);
			cycleCount++;
		}
	};

	void OpcServerMonitor::terminate()
	{
		m_keepRunning = false;
	};
}

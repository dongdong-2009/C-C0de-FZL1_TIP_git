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
  *	Thread for monitoring the OPC Server
  *
  */

#ifndef OPCSERVERMONITOR_H
#define OPCSERVERMONITOR_H

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{
	class OpcServerMonitor : public TA_Base_Core::Thread
	{
	public:

		OpcServerMonitor () {};


		virtual ~OpcServerMonitor () {};

		//
		// Thread derived functions to start/stop the
		// DataNodeQueueProcessorWorker thread
		//
		virtual void run();
		virtual void terminate();

	private:

		volatile bool						m_keepRunning;
	};
}

#endif

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/DpUpdateThread.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class extends TA_Base_Core::Thread.
  *
  * This class just continualy asks the parent driver for the next avaiable datapoint and
  * updates it until told to stop
  */

#if !defined(AFX_DPUPDATETHREAD_H__BAC7D6F2_0D1B_480C_BC3D_F9917F908903__INCLUDED_)
#define AFX_DPUPDATETHREAD_H__BAC7D6F2_0D1B_480C_BC3D_F9917F908903__INCLUDED_

#include "core/threads/src/Thread.h"


namespace TA_Base_App
{
	class TaComponentsDriver;

	class DpUpdateThread : public virtual TA_Base_Core::Thread 
	{
	public:
		DpUpdateThread(TaComponentsDriver* driver);
		virtual ~DpUpdateThread();

	//Implimentation of TA_Thread:

		/**
		  * run
		  *
		  * just continualy asks the parent driver for the next avaiable datapoint and
		  * updates it until told to stop via terminate()
		  */
		virtual void run();

		/**
		  * terminate
		  *
		  * Implementation of the TA_Thread terminate method
		  *
		  * causes the run() method to return
		  *
		  * do not call this method directly - use terminateAndWait()
		  * 
		  */
		virtual void terminate();

	//******************end of TA_Thread interface implimentation*****************

	private:
		bool m_requestStop;
		TaComponentsDriver* m_driver;
	};
} //end namespace TA_Base_App

#endif // !defined(AFX_DPUPDATETHREAD_H__BAC7D6F2_0D1B_480C_BC3D_F9917F908903__INCLUDED_)

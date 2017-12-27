/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/SCADA_development/TA_BASE/transactive/bus/scada/datapoint_library/src/LFThreadPoolSingleton.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  */

#ifndef LFTHREADPOOLSINGLETON_H
#define LFTHREADPOOLSINGLETON_H

#include "core/threads/src/LFThreadPoolManager.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    class LFThreadPoolSingleton
	{
	public:
		virtual ~LFThreadPoolSingleton();
		static LFThreadPoolSingleton& getInstance();
		static LFThreadPoolSingleton& getInstanceWithArgs(bool autoStart);

		// be sure only call one time.
		static void removeInstance(const LFThreadPoolSingleton& instance );

		LFThreadPoolManager* operator-> () const
		{
			TA_ASSERT(0 != m_threadPoolManager, "LFThreadPoolSingleton is null");
			return m_threadPoolManager;
		};

	private:
		LFThreadPoolSingleton(bool autoStart = true);
		LFThreadPoolSingleton( const LFThreadPoolSingleton & theInstance );
		LFThreadPoolSingleton & operator= ( const LFThreadPoolSingleton & rhs );


		static LFThreadPoolSingleton*	m_instance;
		static ReEntrantThreadLockable	m_lock;
		LFThreadPoolManager*			m_threadPoolManager;
	};

} // close namespace TA_Base_Bus

#endif

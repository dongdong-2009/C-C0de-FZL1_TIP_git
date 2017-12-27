/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUDownloadWorker.cpp $
  * @author:	Liang Weimin
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUDownloadWorker object is responsible for upload config file to RTU
  *
  */

#include <sys/timeb.h>

#include "bus/scada/rtu_library/src/RTUDownloadWorker.h"
#include "bus/scada/rtu_library/src/SteRTU.h"
#include "core/ftp/src/FTPManager.h"
#include "core/ftp/src/FTPException.h"

#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	RTUDownloadWorker::RTUDownloadWorker(RTU & rtu)
		:
		m_isTerminating(false),
		m_semaphore(0),
		m_rtu(rtu)
	{
        start();
	}


	RTUDownloadWorker::~RTUDownloadWorker()
	{
        terminateAndWait();
	}


	void RTUDownloadWorker::run()
	{
		m_isTerminating = false;
		
		while (false == m_isTerminating)
		{
			//to wait for the next download semaphore
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUDownloadWorker::run() is waiting for semaphore..." );
			m_semaphore.wait();
			
			//after semaphore is post, m_isTerminating maybe changed to true;
			if (true == m_isTerminating)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUDownloadWorker::run() exits because receive an exit semaphore." );
				break;
			}

			SteRTU* steRTU = dynamic_cast<SteRTU*> (&m_rtu);
			bool isSucceeded = false;
			std::string rtuFtpAddress = steRTU->getAddress();
			unsigned int rtuFtpPort = steRTU->getRTUFtpPort();
			std::string rtuFtpUsername = steRTU->getRTUFtpUsername();
			std::string rtuFtpPassword = steRTU->getRTUFtpPassword();
			std::string sourceFilePath = steRTU->getPMSConfigFilePath();
			std::string destFilePath = steRTU->getRTUConfigFilePath();
						
			try
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "FTP start to upload, [address:port]%s:%d [username:password]%s:%s [from->to]%s->%s", 
					rtuFtpAddress.c_str(), rtuFtpPort, rtuFtpUsername.c_str(), rtuFtpPassword.c_str(), sourceFilePath.c_str(), destFilePath.c_str() );
				
				TA_Base_Core::FTPManager::getInstance().sendFile(rtuFtpAddress, rtuFtpUsername, rtuFtpPassword, sourceFilePath, destFilePath, rtuFtpPort);
				
				isSucceeded = true;
			}
			catch ( const TA_Base_Core::FTPException& ftpex )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, ftpex.what() );
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception catched." );
			}
			
			 steRTU->setDownloadCompleted( isSucceeded );

		}
	}

	void RTUDownloadWorker::startDownload()
	{
		TA_ASSERT(getCurrentState() == THREAD_STATE_RUNNING, "Download thread is not running.");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUDownloadWorker::startDownload() posts a semaphore." );
	
		//to notify run function to continue
		m_semaphore.post();
	}
	
	void RTUDownloadWorker::terminate()
	{
		m_isTerminating = true;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUDownloadWorker::terminate() posts a semaphore." );
		
		//to notify run function to exit
		m_semaphore.post();
	}
	
}

/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/SteRTU.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2017/06/02 12:08:57 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTU object manages a set of DataPoints that have been configured for it.
  *
  */

#ifndef STERTU_H
#define STERTU_H

#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/RTUDownloadWorker.h"

namespace TA_Base_Bus
{
    class ModbusDpProcessor;
    class SteRTUManager;
    class RTUStatusWorker;
    class RTUPollingWorker;
    class RTUCommandWorker;
    
	class SteRTU : public TA_Base_Bus::RTU
	{
	public:
		SteRTU ( SteRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData );
		virtual ~SteRTU();

	// operations
	public:
 		virtual unsigned long adjustAddress( unsigned long registerAddress );
 		
		virtual IModbus* createModbusConnection( const std::string & rtuServicePortNumber, 
													int slaveID, 
													ModbusConnectionType connectionType );
        virtual void createRTUWorkers();
			
		virtual void processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock);
  		virtual bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );

 		virtual bool getIsCommsOK() const;
		virtual void startProcessing();
		virtual void stopProcessing();

		virtual void updateInServiceParameter( bool newState );
		virtual void updateScanTimeMSecs( int scanTimeMSecs );
		virtual void updateCommsErrorRetries( int retries );
		virtual void updatePollTimeout( int pollTimeout );

		// get ftp attributes;
		unsigned int getRTUFtpPort();
		std::string  getRTUFtpUsername();
		std::string  getRTUFtpPassword();
		std::string  getRTUConfigFilePath();
		std::string  getPMSConfigFilePath();
		
		bool getIsRTUSwitching() const;
		bool getIsPrimaryRTU() const;
		bool getIsDownloadRequired() const;//TD10586
		int  getSWCWeightage() const;
		bool switchPrimaryToStandby();
		bool switchStandbyToPrimary();
		bool getIsCommsWithStationSystemsValid ( int statuswordnumber,int slaveID ) const;
		bool setDownloadCompleted(bool isSucceeded); //TD10586
		//
		// operations to command RTU
		//
		bool resetRTU();
		bool setDownloadRequired();
		bool uploadConfigFile(); //TD10586

		bool enablePOWER1Polling();
		bool enablePOWER2Polling();
		bool enablePOWER3Polling();
		bool enableCLOCKPolling();
		bool enableWILDPolling();
		bool enableECSPolling();
		bool enableFPSPolling();
		bool enableTWPPolling();
		bool enablePSD1Polling();
		bool enablePSD2Polling();
		bool enablePLC1Polling();
		bool enablePLC2Polling();
		bool enablePLC3Polling();
		bool enablePLC4Polling();

		bool inhibitPOWER1Polling();
		bool inhibitPOWER2Polling();
		bool inhibitPOWER3Polling();
		bool inhibitCLOCKPolling();
		bool inhibitWILDPolling();
		bool inhibitECSPolling();
		bool inhibitFPSPolling();
		bool inhibitTWPPolling();
		bool inhibitPSD1Polling();
		bool inhibitPSD2Polling();
		bool inhibitPLC1Polling();
		bool inhibitPLC2Polling();
		bool inhibitPLC3Polling();
		bool inhibitPLC4Polling();

	// operations
	private:

        // disabled contructors
		SteRTU();
		SteRTU( const RTU & obj );
		void operator= ( const SteRTU & rhs );

	// attributes
	private:
		RTUPollingWorker*	  	m_rtuPolling;
		// ModbusDpProcessor* 		m_rtuInternalDpProcessor;
		
		volatile bool 			m_isUploadingConfigFile; //TD10586
		RTUCommandWorker*	  	m_rtuCommand;
		RTUDownloadWorker 		m_rtuDownloadWorker;
	};
}
#endif
